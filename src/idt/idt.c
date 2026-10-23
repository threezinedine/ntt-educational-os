#include "idt.h"
#include "tasks/tasks.h"
#include "utils/utils.h"
#include "vga/vga.h"

#define IDT_HANDLER(number) extern void isr##number(void);
#include "idt.def"
#undef IDT_HANDLER

#define PIC1		   0x20 /* IO base address for master PIC */
#define PIC2		   0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND   PIC1
#define PIC1_DATA	   (PIC1 + 1)
#define PIC2_COMMAND   PIC2
#define PIC2_DATA	   (PIC2 + 1)
#define PIC_EOI		   0x20
#define ICW1_ICW4	   0x01 /* Indicates that ICW4 will be present */
#define ICW1_SINGLE	   0x02 /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL	   0x08 /* Level triggered (edge) mode */
#define ICW1_INIT	   0x10 /* Initialization - required! */

#define ICW4_8086		0x01 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO		0x02 /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08 /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM		0x10 /* Special fully nested (not) */

#define CASCADE_IRQ 2

struct idt_descriptor
{
	uint16_t size;	 /* IDT limit (size - 1), must come first for lidt */
	uint32_t offset; /* Linear base address of the IDT */
} __attribute__((packed));

struct idt_descriptor_entry
{
	uint16_t offset_low;
	uint16_t segment_selector;
	uint8_t	 reserved;
	uint8_t	 flags;
	uint16_t offset_high;
} __attribute__((packed));

static struct idt_descriptor	   idt_descriptor			 = {0};
static struct idt_descriptor_entry idt_entries[IDT_ENTRIES]	 = {0};
static idt_handler_t			   idt_handlers[IDT_ENTRIES] = {0};
static uint32_t					   kernel_apis[30];

extern void load_idt(void* idt_descriptor);
extern void idt_int80h_handler(void);
void		idt_dispatcher(uint32_t interrupt_number);

static void PIC_sendEOI(uint8_t irq);
static void PIC_disable(void);
static void io_wait(void);
static void PIC_remap(uint8_t offset1, uint8_t offset2);
static void IRQ_set_mask(uint8_t irq);
static void IRQ_clear_mask(uint8_t irq);

static error_t idt_setup(uint8_t	   interrupt_number,
						 uint8_t	   gate_type,
						 uint8_t	   privilege_level,
						 uint8_t	   segment_selector,
						 idt_handler_t handler);

static void idt_default_handler(void)
{
	vga_print("Unhandled interrupt\n");
}

static uint32_t timer_ticks = 0;

static void timer_callback(void)
{
	task_switch();
#if 0 // Disable interrupts while handling the timer
	timer_ticks++;
	vga_print("[1ms tick] ");
	vga_print("Ticks: ");
	vga_print_dec(timer_ticks);
	vga_print("\n");
#endif
}

static void pit_init(uint16_t frequency)
{
	// Divisor must fit in a 16-bit unsigned integer (1 - 65535)
	uint32_t divisor = 1193182 / frequency;
	if (divisor == 0) divisor = 1;
	if (divisor > 65535) divisor = 65535;

	// Mode 2 (Rate generator), Channel 0, lobyte/hibyte, 16-bit binary
	outb(0x43, 0x34);

	// Send frequency divisor (low byte, then high byte)
	outb(0x40, (uint8_t)(divisor & 0xFF));
	outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

error_t idt_init(void)
{
	memset(&kernel_apis, 0, sizeof(kernel_apis));

	error_t err = OK;
	memset(&idt_descriptor, 0, sizeof(idt_descriptor));
	memset(&idt_entries, 0, sizeof(idt_entries));
	memset(&idt_handlers, 0, sizeof(idt_handlers));

	idt_descriptor.offset = (uint32_t)&idt_entries;
	idt_descriptor.size	  = sizeof(idt_entries) - 1;

	/* Install a gate for every vector. A not-present gate raises #GP, which
	 * - with no usable handler - escalates to #DF and then a triple fault. */
	for (uint16_t i = 0; i < IDT_ENTRIES; i++)
	{
		err = idt_setup((uint8_t)i, INTERRUPT_GATE_32, PRIVILEGE_RING_0, KERNEL_CODE_SEGMENT, idt_default_handler);
		if (err != OK)
		{
			goto out;
		}
	}

	err = idt_setup((uint8_t)0x80, INTERRUPT_GATE_32, PRIVILEGE_RING_3, KERNEL_CODE_SEGMENT, idt_int80h_handler);

	if (err != OK)
	{
		goto out;
	}

#if 1
	// Register timer handler for IRQ 0 (vector 0x20 = PIC_TIMER_IRQ)
	err = idt_set_handler(PIC_TIMER_IRQ, timer_callback);
	if (err != OK)
	{
		goto out;
	}
#endif

	load_idt(&idt_descriptor);

	PIC_remap(PIC_OFFSET_1, PIC_OFFSET_2); // Remap PIC to avoid conflicts with CPU exceptions

	/* Mask all IRQs initially. */
	for (uint8_t irq = 0; irq < 16; ++irq)
	{
		IRQ_set_mask(irq);
	}

	// Initialize PIT to 1000 Hz (1ms tick) and unmask IRQ 0
	pit_init(1000);
	// idt_enable_irq(TIMER_IRQ);

out:
	return err;
}

error_t idt_set_handler(uint8_t irq, idt_handler_t handler)
{
	if (irq >= IDT_ENTRIES)
	{
		return ERROR_INVALID_INTERRUPT_NUMBER;
	}

	idt_handlers[irq] = handler;

	return OK;
}

void idt_enable_irq(uint8_t irq)
{
	IRQ_clear_mask(irq);
}

void idt_disable_irq(uint8_t irq)
{
	IRQ_set_mask(irq);
}

static error_t idt_setup(uint8_t	   interrupt_number,
						 uint8_t	   gate_type,
						 uint8_t	   privilege_level,
						 uint8_t	   segment_selector,
						 idt_handler_t handler)
{
	if (interrupt_number >= IDT_ENTRIES)
	{
		return ERROR_INVALID_INTERRUPT_NUMBER;
	}

	struct idt_descriptor_entry* entry = &idt_entries[interrupt_number];

	idt_handlers[interrupt_number] = handler;
	entry->segment_selector		   = segment_selector;
	entry->reserved				   = 0;
	entry->flags				   = (gate_type & 0x0F) | ((privilege_level & 0x03) << 5) | 0x80;

	void* handler_ptr = 0;
	if (interrupt_number == 0x80)
	{
		handler_ptr = (void*)handler;
	}
	else
	{
		switch (interrupt_number)
		{
#define IDT_HANDLER(number)                                                                                            \
	case number:                                                                                                       \
		handler_ptr = (void*)isr##number;                                                                              \
		break;
#include "idt.def"
#undef IDT_HANDLER
		default:
			return ERROR_INVALID_INTERRUPT_NUMBER;
		}
	}

	entry->offset_low  = (uint16_t)((uint32_t)handler_ptr & 0xFFFF);
	entry->offset_high = (uint16_t)(((uint32_t)handler_ptr >> 16) & 0xFFFF);

	return OK;
}

void idt_dispatcher(uint32_t interrupt_number)
{
	if (idt_handlers[interrupt_number])
	{
		idt_handlers[interrupt_number]();
	}

	/* Hardware IRQs are remapped to vectors PIC_OFFSET_1..PIC_OFFSET_1+15.
	 * The PIC must be told the interrupt has been handled, otherwise it will
	 * not deliver any further interrupts of equal or lower priority. */
	if (interrupt_number >= PIC_OFFSET_1 && interrupt_number < (PIC_OFFSET_1 + 16))
	{
		PIC_sendEOI((uint8_t)(interrupt_number - PIC_OFFSET_1));
	}
}

static void PIC_sendEOI(uint8_t irq)
{
	if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);

	outb(PIC1_COMMAND, PIC_EOI);
}

static void PIC_disable(void)
{
	outb(PIC1_DATA, 0xff);
	outb(PIC2_DATA, 0xff);
}

static void io_wait(void)
{
	/* Port 0x80 is used for 'checkpoints' during POST. */
	outb(0x80, 0);
}

static void PIC_remap(uint8_t offset1, uint8_t offset2)
{
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1); // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2); // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 1 << CASCADE_IRQ); // ICW3: tell Master PIC that there is a slave PIC at IRQ2
	io_wait();
	outb(PIC2_DATA, CASCADE_IRQ); // ICW3: tell Slave PIC its cascade identity
	io_wait();

	outb(PIC1_DATA, ICW4_8086); // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// Unmask both PICs.
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

static void IRQ_set_mask(uint8_t irq)
{
	uint16_t port;
	uint8_t	 value;

	if (irq < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		irq -= 8;
	}
	value = inb(port) | (1 << irq);
	outb(port, value);
}

static void IRQ_clear_mask(uint8_t irq)
{
	uint16_t port;
	uint8_t	 value;

	if (irq < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		irq -= 8;
	}
	value = inb(port) & ~(1 << irq);
	outb(port, value);
}

void idt_enable_timer()
{
	IRQ_clear_mask(TIMER_IRQ);
}

void idt_disable_timer()
{
	IRQ_set_mask(TIMER_IRQ);
}