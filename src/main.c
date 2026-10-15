#include "disk/disk.h"
#include "filesystem/filesystem.h"
#include "idt/idt.h"
#include "keyboard/keyboard.h"
#include "memory/memory.h"
#include "paging/paging.h"
#include "utils/utils.h"
#include "vfs/vfs.h"
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

	// page_directory_t test_page_directory = page_create_page_directory(128 * 1024 * 1024 /* 128 MB */, 0);
	// page_destroy_page_directory(test_page_directory);

	uint8_t* buffer		  = kzalloc(512);
	uint32_t sector_index = 1;
	uint32_t bytes_show	  = 5;
	disk_read(0, sector_index, 1, buffer);

	vga_print("----- Disk read buffer -----\n");
	for (int i = 0; i < bytes_show; i++)
	{
		vga_print("[");
		vga_print_dec(i);
		vga_print("] ");
		vga_print_hex(buffer[i]);
		vga_print("\n");
	}

	vga_print("----- End of disk read buffer -----\n");

	vga_print("\nModify first byte and 3rd byte in the sector\n");
	buffer[0] = 0xFF;
	buffer[2] = 0xFF;
	disk_write(0, sector_index, 1, buffer);
	memclear(buffer, 512);
	disk_read(0, sector_index, 1, buffer);

	vga_print("----- Disk read buffer after modification -----\n");
	for (int i = 0; i < bytes_show; i++)
	{
		vga_print("[");
		vga_print_dec(i);
		vga_print("] ");
		vga_print_hex(buffer[i]);
		vga_print("\n");
	}
	vga_print("----- End of disk read buffer after modification -----\n");

	void* ptr  = kalloc(1024);
	*(int*)ptr = 0x2A;
	vga_print_hex(*(int*)ptr);
	vga_print("\n");

	vfs_init();
	struct file_system* pFs = fs_create();
	vfs_register_file_system(pFs);

	fs_mkdir(pFs, from_c_string("/test"));

#if 0
	const char* example_path = "/home/user/docs";
	struct path p			 = from_c_string(example_path);

	vga_print("Path root: ");
	char root_buffer[256];
	path_get_root(&p, root_buffer);
	vga_print(root_buffer);
	vga_print("\n");

	while (path_next(&p))
	{
		vga_print("Path current: ");
		char current_buffer[256];
		path_get_current(&p, current_buffer);
		vga_print(current_buffer);
		vga_print("\n");
	}

	struct file_description fd = vfs_open(p);

	vga_print("File opened: ");
	vga_print(fd.is_open ? "true" : "false");
	vga_print("\n");

	vfs_close(&fd);
#endif

loop:
	for (;;)
	{
	}

	vfs_unregister_file_system(pFs);
	fs_destroy(pFs);
	vfs_shutdown();
	kshutdown();
}