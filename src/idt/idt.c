#include "idt.h"
#include "utils/utils.h"
#include "vga/vga.h"

#define IDT_HANDLER(number) extern void isr##number(void);
#include "idt.def"
#undef IDT_HANDLER

struct idt_descriptor
{
	uint32_t offset;
	uint16_t size;
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
extern void idt_test(void); // TODO: Delete later
void		idt_dispatcher(uint32_t interrupt_number);

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

	load_idt(&idt_descriptor);

	err = idt_setup(10, INTERRUPT_GATE_32, PRIVILEGE_RING_0, KERNEL_CODE_SEGMENT, idt_default_handler);

	if (err != OK)
	{
		goto out;
	}

	idt_test(); // TODO: Delete later

out:
	return err;
}

error_t idt_setup(uint8_t		interrupt_number,
				  uint8_t		gate_type,
				  uint8_t		privilege_level,
				  uint8_t		segment_selector,
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
