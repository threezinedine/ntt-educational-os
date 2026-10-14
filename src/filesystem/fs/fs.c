#include "fs.h"
#include "disk/disk.h"
#include "memory/memory.h"
#include "utils/utils.h"
#include "vga/vga.h"

#define MAGIC				0x33344333
#define MAX_BLOCK_INDEX		125
#define START_SEARCH_SECTOR 1

#define FREE_BLOCK	0x00
#define START_BLOCK 0x01
#define USED_BLOCK	0x01
#define END_BLOCK	0xFF

#define ITEM_NAME_LENGTH 64

/**
 * 1 sector = 1 inode block
 *
 * Sector 1: Magic
 */

// First 512 sector
struct setup_block
{
	uint32_t magic;
	uint32_t node_blocks_counts;
	uint32_t node_block_size;
	uint32_t inode_meta_block_size;
};

struct inode_entry
{
	char	 name[ITEM_NAME_LENGTH];
	uint8_t	 is_directory;
	uint8_t	 has_next_inode;
	uint16_t next_inode;
} __attribute__((packed));

struct fs_internal
{
	uint32_t		   block_index;
	uint32_t		   table_count;
	struct setup_block setup;
	uint32_t		   root_block;
};

static bool				   fs_resolve(struct file_system* pSystem, struct path path);
static error_t			   fs_open(struct file_system* pSystem, struct path path, struct file_description* pFile);
static error_t			   fs_close(struct file_system* pSystem, struct file_description* pFile);
static struct inode_entry* fs_get_inode(struct file_system* pSystem, uint16_t inode_index);
static void				   fs_print_inode(struct inode_entry* pInode);

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

	uint32_t total_table_size = pInternal->setup.inode_meta_block_size * pInternal->setup.node_block_size;

	if (total_table_size % pInternal->setup.node_block_size != 0)
	{
		panic("Invalid table size");
	}

	pInternal->block_index = block_index;
	pInternal->table_count = total_table_size / pInternal->setup.node_block_size;
	pInternal->root_block  = 0x0000;

	vga_print("############################\n");
	vga_print("Filesystem setup loaded:\n");
	vga_print("Magic: 0x");
	vga_print_hex(pInternal->setup.magic);
	vga_print("\n");
	vga_print("Inode blocks count: ");
	vga_print_dec(pInternal->setup.node_blocks_counts);
	vga_print("\n");
	vga_print("Inode block size: ");
	vga_print_dec(pInternal->setup.node_block_size);
	vga_print("\n");
	vga_print("Inode meta block size: ");
	vga_print_dec(pInternal->setup.inode_meta_block_size);
	vga_print("\n");
	vga_print("Table count: ");
	vga_print_dec(pInternal->table_count);
	vga_print("\n");
	vga_print("############################\n");

	pSystem->pInternal = pInternal;
	pSystem->resolve   = fs_resolve;
	pSystem->open	   = fs_open;
	pSystem->close	   = fs_close;

#if 1
	struct inode_entry* pRootInode = fs_get_inode(pSystem, 0);
	fs_print_inode(pRootInode); // Print root directory
	kfree(pRootInode);
#endif

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

static struct inode_entry* fs_get_inode(struct file_system* pSystem, uint16_t inode_index)
{
	struct inode_entry* pInode = kzalloc(sizeof(struct inode_entry));

	struct fs_internal* pInternal = (struct fs_internal*)pSystem->pInternal;

	// Table sector be next to the block_index
	uint32_t table_sector	   = pInternal->block_index + 1;
	uint32_t start_node_sector = table_sector + 1 + inode_index;

	// Read the table sector into a buffer
	uint8_t buffer[512];
	disk_read(0, table_sector, 1, buffer);

	if (buffer[inode_index] != START_BLOCK)
	{
		return NULL;
	}

	memclear(buffer, 512);
	disk_read(0, start_node_sector, 1, buffer);

	memcpy(pInode, buffer, sizeof(struct inode_entry));

	return pInode;
}

static void fs_print_inode(struct inode_entry* pInode)
{
	if (pInode == NULL)
	{
		vga_print("----------------------------\n");
		vga_print("Inode details:\n");
		vga_print("    Inode is NULL\n");
		vga_print("----------------------------\n");
		return;
	}

	vga_print("----------------------------\n");
	vga_print("Inode details:\n");
	vga_print("    Is directory: ");
	vga_print(pInode->is_directory ? "Yes" : "No");
	vga_print("\n");
	vga_print("    Name: ");
	vga_print(pInode->name);
	vga_print("\n");
	vga_print("----------------------------\n");
}