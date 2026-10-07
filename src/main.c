#include "idt/idt.h"
#include "keyboard/keyboard.h"
#include "utils/utils.h"
#include "vga/vga.h"

void kernel_main()
{
	vga_init();

	vga_print("Vga Initialized!\n");

	vga_print("Setting the IDT...\n");
	idt_init();

	vga_print("IDT Set!\n");

	vga_print("Setting up keyboard...\n");
	keyboard_init();

	vga_print("Keyboard ready! Type something:\n");

	for (;;)
	{
	}
}