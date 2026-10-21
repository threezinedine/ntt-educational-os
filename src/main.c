#include "disk/disk.h"
#include "filesystem/filesystem.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "keyboard/keyboard.h"
#include "memory/memory.h"
#include "paging/paging.h"
#include "tasks/tasks.h"
#include "utils/utils.h"
#include "vfs/vfs.h"
#include "vga/vga.h"

#define SIZE_128_MB (128 * 1024 * 1024)
#define SIZE_64_MB	(64 * 1024 * 1024)

void kernel_main()
{
	gdt_setup();

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

	page_directory_t default_page_directory = page_create_page_directory(SIZE_128_MB, 1);
	page_switch_page_directory(default_page_directory);
	page_enable_paging();

	struct task* pTask = task_create("hello");
	task_run(pTask);

#if 0
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

	fs_mkdir(pFs, from_c_string("/hello/test"));

	vga_print("------------------------\n");
	vga_print("Root folder items count: ");
	uint32_t root_items_count = fs_get_items_count(pFs, ROOT_FOLDER);
	vga_print_dec(root_items_count);
	vga_print("\n");

	uint32_t tab_level = 1;
	for (uint32_t i = 0; i < root_items_count; i++)
	{
		char name[64];
		for (uint32_t j = 0; j < tab_level; ++j)
		{
			vga_print("\t");
		}

		fs_get_folder_name(pFs, fs_get_item(pFs, ROOT_FOLDER, i), name, sizeof(name));
		vga_print("Item ");
		vga_print_dec(i);
		vga_print(": ");
		vga_print(name);
		if (fs_is_folder(pFs, fs_get_item(pFs, ROOT_FOLDER, i)))
		{
			vga_print(" (Folder)\n");
			tab_level++;

			fs_item	 sub_folder		 = fs_get_item(pFs, ROOT_FOLDER, i);
			uint32_t sub_items_count = fs_get_items_count(pFs, sub_folder);

			for (uint32_t j = 0; j < tab_level; ++j)
			{
				vga_print("\t");
			}

			vga_print("Sub-items count: ");
			vga_print_dec(sub_items_count);
			vga_print("\n");

			for (uint32_t j = 0; j < sub_items_count; ++j)
			{
				for (uint32_t k = 0; k < tab_level; ++k)
				{
					vga_print("\t");
				}
				vga_print("Sub-item ");
				vga_print_dec(j);
				vga_print("\n");
			}

			tab_level--;
		}
		vga_print("\n");
	}

	vga_print("------------------------\n");

	// fs_mkdir(pFs, from_c_string("/test"));

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
#endif

loop:
	for (;;)
	{
	}

#if 0
	vfs_unregister_file_system(pFs);
	fs_destroy(pFs);
	vfs_shutdown();
#endif
	kshutdown();
}

static void int_80_handler()
{
	vga_print("Interrupt 80h called\n");
}