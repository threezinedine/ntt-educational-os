#include "vga/vga.h"

void kernel_main()
{
	vga_init();

	vga_print("Hello, World!\n");
	vga_print("Welcome to NTT-OS!\n");

	for (;;)
	{
	}
}