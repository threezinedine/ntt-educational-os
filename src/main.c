#include "idt/idt.h"
#include "vga/vga.h"

void kernel_main()
{
	vga_init();

	vga_print("Vga Initialized!\n");

	vga_print("Setting the IDT...\n");
	idt_init();
	vga_print("IDT Set!\n");

	for (;;)
	{
	}
}