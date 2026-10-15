#include "fs.h"
#include "disk/disk.h"
#include "memory/memory.h"
#include "utils/utils.h"
#include "vga/vga.h"

#define MAGIC							  0x33344333
#define MAX_start_fs_start_fs_block_index 125
#define START_SEARCH_SECTOR				  1

#define FREE_BLOCK	0x00
#define START_BLOCK 0x01
#define USED_BLOCK	0x01
#define END_BLOCK	0xFF

#define ITEM_NAME_LENGTH 64
#define BLOCK_SIZE		 512

#define INODE_ITEMS_ARRAY_SIZE ((BLOCK_SIZE - ITEM_NAME_LENGTH - 4) / 2)
#define INODE_DATA_ARRAY_SIZE  (BLOCK_SIZE - ITEM_NAME_LENGTH - 4)

fs_item ROOT_FOLDER = 0x0000;

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
	union {
		uint16_t items[INODE_ITEMS_ARRAY_SIZE]; // 4 bytes for other fields in inode_entry
		uint8_t	 data[INODE_DATA_ARRAY_SIZE];	// 4 bytes for other fields in inode_entry
	};
} __attribute__((packed));

struct fs_internal
{
	uint32_t		   start_fs_start_fs_block_index;
	uint32_t		   table_start_block_index;
	uint32_t		   table_count;
	uint32_t		   root_block_real_index;
	struct setup_block setup;
	uint32_t		   root_block;
	uint8_t			   table_bitmap[BLOCK_SIZE];
};

static bool				   fs_resolve(struct file_system* pSystem, struct path path);
static error_t			   fs_open(struct file_system* pSystem, struct path path, struct file_description* pFile);
static error_t			   fs_close(struct file_system* pSystem, struct file_description* pFile);
static struct inode_entry* fs_get_inode(struct file_system* pSystem, uint16_t inode_index);
static void				   fs_print_inode(struct inode_entry* pInode);
static uint32_t			   fs_get_children_count(struct file_system* pSystem, struct inode_entry* pInode);
static uint32_t			   fs_create_directory(struct file_system* pSystem, fs_item parent, const char* name);

struct file_system* fs_create()
{
	if (sizeof(struct inode_entry) != BLOCK_SIZE)
	{
		panic("Inode entry size does not match block size");
	}

	struct file_system* pSystem	  = (struct file_system*)kzalloc(sizeof(struct file_system));
	struct fs_internal* pInternal = (struct fs_internal*)kzalloc(sizeof(struct fs_internal));

	// searching match magic
	uint32_t start_fs_start_fs_block_index = START_SEARCH_SECTOR;
	uint8_t	 buffer[BLOCK_SIZE];
	while (true)
	{
		uint32_t magic;
		memset(buffer, 0, sizeof(buffer));
		disk_read(0, start_fs_start_fs_block_index, 1, buffer);
		magic = *(uint32_t*)buffer;

		if (magic == MAGIC)
		{
			break;
		}
		start_fs_start_fs_block_index++;

		if (start_fs_start_fs_block_index > MAX_start_fs_start_fs_block_index)
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

	pInternal->start_fs_start_fs_block_index = start_fs_start_fs_block_index;
	pInternal->table_start_block_index		 = pInternal->start_fs_start_fs_block_index + 1;
	pInternal->table_count					 = total_table_size / pInternal->setup.node_block_size;
	pInternal->root_block					 = 0x0000;
	pInternal->root_block_real_index		 = pInternal->root_block + pInternal->table_start_block_index + 1;

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

	// load table bit map
	disk_read(0, pInternal->table_start_block_index, 1, pInternal->table_bitmap);

#if 0
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

	// Table sector be next to the start_fs_start_fs_block_index
	uint32_t table_sector	   = pInternal->table_start_block_index;
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
	vga_print("    Children count: ");
	char	 buffer[12];
	uint32_t children_count = fs_get_children_count(NULL, pInode);
	vga_print_dec(children_count);
	vga_print("\n");
	vga_print("----------------------------\n");
}

static uint32_t fs_get_children_count(struct file_system* pSystem, struct inode_entry* pInode)
{
	if (pInode == NULL || !pInode->is_directory)
	{
		return 0;
	}

	for (uint32_t i = 0; i < INODE_ITEMS_ARRAY_SIZE; i++)
	{
		if (pInode->items[i] == 0)
		{
			return i;
		}
	}

	if (pInode->has_next_inode == 0)
	{
		return INODE_ITEMS_ARRAY_SIZE;
	}

	// TODO: Handle the case where the directory has more than one inode.
	return INODE_ITEMS_ARRAY_SIZE;
}

void fs_mkdir(struct file_system* pSystem, struct path path)
{
	if (pSystem == NULL)
	{
		panic("File system is NULL");
	}

	char root[64];
	path_get_root(&path, root);

	if (strcmp(root, "/") != 0)
	{
		panic("Cannot create directory at root");
	}

	struct fs_internal* pInternal	 = (struct fs_internal*)pSystem->pInternal;
	struct inode_entry* pRoot		 = fs_get_inode(pSystem, pInternal->root_block);
	struct inode_entry* pCurrentRoot = pRoot;
	fs_item				current_item = 0;

	char current[64];
	while (path_next(&path))
	{
		uint8_t	 found		 = 0;
		uint32_t child_index = 0;
		path_get_current(&path, current);
		for (child_index = 0; child_index < INODE_ITEMS_ARRAY_SIZE; child_index++)
		{
			if (pCurrentRoot->items[child_index] == 0)
			{
				break;
			}

			struct inode_entry* pChild = fs_get_inode(pSystem, pCurrentRoot->items[child_index]);

			if (pChild == NULL)
			{
				panic("Failed to get child inode");
			}

			if (strcmp(pChild->name, current) == 0)
			{
				// TODO: free inode later
				current_item = pCurrentRoot->items[child_index];
				pCurrentRoot = pChild;
				found		 = 1;
				break;
			}
		}
		// Create the directory for the current path component

		if (!found)
		{
			uint32_t new_block_index = fs_create_directory(pSystem, current_item, current);
			// TODO: free inode later
			pCurrentRoot			 = fs_get_inode(pSystem, new_block_index);
			current_item			 = new_block_index;
		}
	}
}

static uint32_t fs_allocate_block(struct fs_internal* pInternal)
{
	for (uint32_t i = 0; i < pInternal->setup.node_blocks_counts; ++i)
	{
		if (pInternal->table_bitmap[i] == FREE_BLOCK)
		{
			// Mark the index as used
			pInternal->table_bitmap[i] = USED_BLOCK;
			disk_write(0, pInternal->table_start_block_index, 1, pInternal->table_bitmap);
			disk_read(0, pInternal->table_start_block_index, 1, pInternal->table_bitmap);
			return i;
		}
	}

	panic("Out of memory");
}

static uint32_t fs_create_directory(struct file_system* pSystem, fs_item pParent, const char* name)
{
	if (name == NULL || strlen(name) == 0 || strlen(name) > ITEM_NAME_LENGTH)
	{
		panic("Invalid directory name");
	}

	struct fs_internal* pInternal = (struct fs_internal*)pSystem->pInternal;

	// Find the first unused index
	uint32_t block_index = fs_allocate_block(pInternal);

	struct inode_entry* pNewDir = fs_get_inode(pSystem, block_index);
	pNewDir->is_directory		= 1;
	memcpy(pNewDir->name, name, strlen(name) + 1);
	pNewDir->has_next_inode = 0;

	disk_write(0, pInternal->root_block_real_index + block_index, 1, (void*)pNewDir);
	kfree(pNewDir);

	struct inode_entry* pParentInode = fs_get_inode(pSystem, pParent);

	uint16_t child_index = 0;
	for (child_index = 0; child_index < INODE_ITEMS_ARRAY_SIZE; child_index++)
	{
		if (pParentInode->items[child_index] == 0)
		{
			break;
		}
	}
	if (child_index == INODE_ITEMS_ARRAY_SIZE)
	{
		panic("No free slot available in root inode");
	}
	pParentInode->items[child_index] = block_index;
	disk_write(0, pInternal->root_block_real_index, 1, (void*)pParentInode);
	kfree(pParentInode);

	return block_index;
}

void fs_get_folder_name(struct file_system* pSystem, uint32_t folder, char* name, uint32_t name_size)
{
	struct inode_entry* pFolder = fs_get_inode(pSystem, folder);
	if (pFolder == NULL)
	{
		panic("Failed to get folder inode");
	}

	if (strlen(pFolder->name) + 1 > name_size)
	{
		panic("Name buffer is too small");
	}
	memcpy(name, pFolder->name, name_size);
	kfree(pFolder);
}

void fs_get_file_name(struct file_system* pSystem, uint32_t file, char* name, uint32_t name_size)
{
	struct inode_entry* pFile = fs_get_inode(pSystem, file);
	if (pFile == NULL)
	{
		panic("Failed to get file inode");
	}

	if (strlen(pFile->name) + 1 > name_size)
	{
		panic("Name buffer is too small");
	}
	memcpy(name, pFile->name, name_size);
	kfree(pFile);
}

uint32_t fs_get_items_count(struct file_system* pSystem, uint32_t folder)
{
	struct inode_entry* pFolder = fs_get_inode(pSystem, folder);
	if (pFolder == NULL)
	{
		panic("Failed to get folder inode");
	}

	uint32_t count = 0;
	for (uint32_t i = 0; i < INODE_ITEMS_ARRAY_SIZE; i++)
	{
		if (pFolder->items[i] != 0)
		{
			count++;
		}
		else
		{
			break;
		}
	}
	kfree(pFolder);
	return count;
}

fs_item fs_get_item(struct file_system* pSystem, uint32_t folder, uint32_t index)
{
	struct inode_entry* pFolder = fs_get_inode(pSystem, folder);
	if (pFolder == NULL)
	{
		panic("Failed to get folder inode");
	}

	if (index >= INODE_ITEMS_ARRAY_SIZE)
	{
		panic("Index out of bounds");
	}

	fs_item item = pFolder->items[index];
	kfree(pFolder);
	return item;
}

bool fs_is_folder(struct file_system* pSystem, fs_item item)
{
	struct inode_entry* pItem = fs_get_inode(pSystem, item);
	if (pItem == NULL)
	{
		panic("Failed to get item inode");
	}

	bool is_folder = (pItem->is_directory);
	kfree(pItem);
	return is_folder;
}