#include "idt.h"
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

extern void load_idt(void* idt_descriptor);
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

error_t idt_init(void)
{
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

	load_idt(&idt_descriptor);

	PIC_remap(0x20, 0x28); // Remap PIC to avoid conflicts with CPU exceptions

	/* Mask all IRQs initially. */
	for (uint8_t irq = 0; irq < 16; ++irq)
	{
		IRQ_set_mask(irq);
	}

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