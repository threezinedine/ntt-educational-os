#include "idt/idt.h"
#include "keyboard/keyboard.h"
#include "memory/memory.h"
#include "paging/paging.h"
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
	kinit();

	paging_init();

	page_directory_t default_page_directory = paging_create_page_directory(128 * 1024 * 1024 /* 128 MB */, 1);
	paging_switch_page_directory(default_page_directory);
	paging_enable_paging();

	void* ptr  = kalloc(1024);
	*(int*)ptr = 0x2A;
	vga_print_hex(*(int*)ptr);

	for (;;)
	{
	}

	kshutdown();
}