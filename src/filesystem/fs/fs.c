#include "fs.h"
#include "disk/disk.h"
#include "memory/memory.h"
#include "utils/utils.h"
#include "vga/vga.h"

#define MAGIC				0x33344333
#define MAX_BLOCK_INDEX		125
#define START_SEARCH_SECTOR 1

/**
 * 1 sector = 1 inode block
 *
 * Sector 1: Magic
 */

// First 512 sector
struct setup_block
{
	uint32_t magic;
	uint32_t inode_blocks_counts;
	uint32_t inode_block_size;
	uint32_t inode_meta_block_size;
};

struct inode_entry
{
};

struct fs_internal
{
	struct setup_block setup;
};

static bool	   fs_resolve(struct file_system* pSystem, struct path path);
static error_t fs_open(struct file_system* pSystem, struct path path, struct file_description* pFile);
static error_t fs_close(struct file_system* pSystem, struct file_description* pFile);

struct file_system* fs_create()
{
	struct file_system* pSystem	  = (struct file_system*)kzalloc(sizeof(struct file_system));
	struct fs_internal* pInternal = (struct fs_internal*)kzalloc(sizeof(struct fs_internal));

	// searching match magic
	uint32_t block_index = START_SEARCH_SECTOR;
	uint8_t	 buffer[512];
	while (true)
	{
		uint32_t magic;
		memset(buffer, 0, sizeof(buffer));
		disk_read(0, block_index, 1, buffer);
		magic = *(uint32_t*)buffer;

		if (magic == MAGIC)
		{
			break;
		}
		block_index++;

		if (block_index > MAX_BLOCK_INDEX)
		{
			panic("Failed to load the filesystem");
		}
	}

	memcpy(&pInternal->setup, buffer, sizeof(struct setup_block));

#if 1
	vga_print("Filesystem setup loaded:\n");
	vga_print("Magic: 0x");
	vga_print_hex(pInternal->setup.magic);
	vga_print("\n");
	vga_print("Inode blocks count: ");
	vga_print_dec(pInternal->setup.inode_blocks_counts);
	vga_print("\n");
	vga_print("Inode block size: ");
	vga_print_dec(pInternal->setup.inode_block_size);
	vga_print("\n");
	vga_print("Inode meta block size: ");
	vga_print_dec(pInternal->setup.inode_meta_block_size);
	vga_print("\n");
#endif

	pSystem->pInternal = pInternal;
	pSystem->resolve   = fs_resolve;
	pSystem->open	   = fs_open;
	pSystem->close	   = fs_close;

	return pSystem;
}

error_t fs_destroy(struct file_system* pSystem)
{
	struct fs_internal* pInternal = (struct fs_internal*)pSystem->pInternal;
	kfree(pInternal);
	kfree(pSystem);

	return OK;
}

static bool fs_resolve(struct file_system* pSystem, struct path path)
{
	return true;
}

static error_t fs_open(struct file_system* pSystem, struct path path, struct file_description* pFile)
{
	error_t result = OK;
	return result;
}

static error_t fs_close(struct file_system* pSystem, struct file_description* pFile)
{
	error_t result = OK;
	return result;
}