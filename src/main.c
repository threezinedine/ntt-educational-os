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

	page_init();

	page_directory_t default_page_directory = page_create_page_directory(128 * 1024 * 1024 /* 128 MB */, 1);
	page_switch_page_directory(default_page_directory);
	page_enable_paging();

	page_directory_t test_page_directory = page_create_page_directory(128 * 1024 * 1024 /* 128 MB */, 0);
	page_destroy_page_directory(test_page_directory);

	void* ptr  = kalloc(1024);
	*(int*)ptr = 0x2A;
	vga_print_hex(*(int*)ptr);

	for (;;)
	{
	}

	kshutdown();
}