#include "idt/idt.h"
#include "keyboard/keyboard.h"
#include "memory/memory.h"
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

	void* ptr = kalloc(1024);
	vga_print("ptr : ");
	vga_print_hex((uint32_t)ptr);
	vga_print("\n");

	void* ptr2 = kalloc(512);
	vga_print("ptr2: ");
	vga_print_hex((uint32_t)ptr2);
	vga_print("\n");

	void* ptr3 = kalloc(4212);
	vga_print("ptr3: ");
	vga_print_hex((uint32_t)ptr3);
	vga_print("\n");

	void* ptr4 = kalloc(1025);
	vga_print("ptr4: ");
	vga_print_hex((uint32_t)ptr4);
	vga_print("\n");

	kfree(ptr3);
	void* ptr5 = kalloc(12);
	vga_print("ptr5: ");
	vga_print_hex((uint32_t)ptr5);
	vga_print("\n");

	for (;;)
	{
	}

	kshutdown();
}