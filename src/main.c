#include "idt/idt.h"
#include "vga/vga.h"

void handle_10()
{
	vga_print("Handling interrupt 10\n");
}

void kernel_main()
{
	vga_init();

	vga_print("Vga Initialized!\n");

	vga_print("Setting the IDT...\n");
	idt_init();
	idt_set_handler(10, handle_10);

	vga_print("IDT Set!\n");

	for (;;)
	{
	}
}