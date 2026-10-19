#include "gdt.h"

extern void gdt_flush(uint32_t gdt);

struct gdt_entry
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t	 base_middle;
	uint8_t	 access;
	uint8_t	 granularity;
	uint8_t	 base_high;
} __attribute__((packed));

struct gdt_table
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

struct tss_entry
{
	uint32_t prev_tss; // Previous TSS (if hardware task linking is used)
	uint32_t esp0;	   // Ring 0 stack pointer (CPU loads this on Ring 3 -> Ring 0 interrupt)
	uint32_t ss0;	   // Ring 0 stack segment (kernel data selector, e.g. 0x10)
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed));

static struct gdt_entry entries[6] = {};
static struct gdt_table table	   = {};
static struct tss_entry tss		   = {};

error_t gdt_setup()
{
	table.limit = sizeof(entries) - 1;
	table.base	= (uint32_t)&entries;

	memset(entries, 0, sizeof(entries));

	// The first entry is the null descriptor, which is required by the x86 architecture.

	gdt_set_gate(1,
				 0,
				 0xFFFFFFFF,
				 GDT_PRESENT | GDT_DPL_RING0 | GDT_CODE_SEGMENT | GDT_CODE_READABLE,
				 GDT_GRANULARITY_4K | GDT_32BIT);

	gdt_set_gate(2,
				 0,
				 0xFFFFFFFF,
				 GDT_PRESENT | GDT_DPL_RING0 | GDT_DATA_SEGMENT | GDT_DATA_READWRITE,
				 GDT_GRANULARITY_4K | GDT_32BIT);

	gdt_set_gate(3,
				 0,
				 0xFFFFFFFF,
				 GDT_PRESENT | GDT_DPL_RING3 | GDT_CODE_SEGMENT | GDT_CODE_READABLE,
				 GDT_GRANULARITY_4K | GDT_32BIT);
	gdt_set_gate(4,
				 0,
				 0xFFFFFFFF,
				 GDT_PRESENT | GDT_DPL_RING3 | GDT_DATA_SEGMENT | GDT_DATA_READWRITE,
				 GDT_GRANULARITY_4K | GDT_32BIT);
	gdt_set_gate(5, (uint32_t)&tss, sizeof(tss) - 1, GDT_PRESENT | GDT_DPL_RING0 | GDT_TSS_SEGMENT | GDT_DISABLED, 0);

	gdt_flush((uint32_t)&table);

	return OK;
}

error_t gdt_set_gate(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	error_t result = OK;

	if (index >= sizeof(entries) / sizeof(entries[0]))
	{
		result = ERROR_INVALID_ARGUMENT;
		goto out;
	}

	entries[index].limit_low   = limit & 0xFFFF;
	entries[index].base_low	   = base & 0xFFFF;
	entries[index].base_middle = (base >> 16) & 0xFF;
	entries[index].access	   = access;
	entries[index].granularity = ((limit >> 16) & 0x0F) | ((flags << 4) & 0xF0);
	entries[index].base_high   = (base >> 24) & 0xFF;

out:
	return result;
}