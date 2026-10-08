#include "paging.h"
#include "memory/kheap.h"
#include "pmm.h"

#define PHYSICAL_PAGES_COUNT (1024) // Example value, adjust as needed

#define PRESENT_PAGE		 (1)
#define WRITABLE_PAGE		 (1 >> 2)
#define USER_ACCESSIBLE_PAGE (1 >> 3)
#define WRITETHROUGH_PAGE	 (1 >> 4)
#define CACHE_DISABLE_PAGE	 (1 >> 5)

error_t page_init()
{
	return pmm_init(PHYSICAL_PAGES_COUNT);
}

page_directory_t page_create_page_directory(uint32_t size, uint8_t root_page)
{
	if (size == 0 || size % PAGE_SEGMENT_SIZE != 0)
	{
		return INVALID_PAGE_DIRECTORY;
	}

	uint32_t page_directory_size = sizeof(uint32_t) * PAGE_DIRECTORY_ENTRIES;
	uint32_t page_table_size	 = sizeof(uint32_t) * PAGE_TABLE_ENTRIES;
	uint32_t total_size			 = page_directory_size + page_table_size * PAGE_DIRECTORY_ENTRIES;

	void*	 page_directory		= kalloc(total_size);
	void*	 current_page_table = (void*)((uint32_t)page_directory + page_directory_size);
	uint32_t size_counter		= 0;

	for (uint32_t i = 0; i < PAGE_DIRECTORY_ENTRIES; i++)
	{
		uint32_t* current_page_directory_entry_ptr = &((uint32_t*)page_directory)[i];
		*current_page_directory_entry_ptr		   = (uint32_t)current_page_table;

		if (size_counter < size)
		{
			*current_page_directory_entry_ptr |= PRESENT_PAGE;
		}

		for (uint32_t j = 0; j < PAGE_TABLE_ENTRIES; j++)
		{
			uint32_t* current_page_table_entry_ptr = &((uint32_t*)current_page_table)[j];
			uint32_t  page_address =
				 root_page != 0 ? (i * PAGE_TABLE_ENTRIES + j) * PAGE_SEGMENT_SIZE : (uint32_t)pmm_get_free_segment();

			*current_page_table_entry_ptr = page_address;

			if (size_counter < size)
			{
				*current_page_table_entry_ptr |= PRESENT_PAGE;
			}

			size_counter += PAGE_SEGMENT_SIZE;
		}

		current_page_table += page_table_size;
	}

out:
	return (page_directory_t)page_directory;
}

error_t page_destroy_page_directory(page_directory_t page_directory)
{
	error_t err = OK;

	void*	 current_page_table = (void*)((uint32_t)page_directory + sizeof(uint32_t) * PAGE_DIRECTORY_ENTRIES);
	uint32_t page_table_size	= sizeof(uint32_t) * PAGE_TABLE_ENTRIES;

	for (uint32_t i = 0; i < PAGE_DIRECTORY_ENTRIES; i++)
	{
		for (uint32_t j = 0; j < PAGE_TABLE_ENTRIES; j++)
		{
			uint32_t* current_page_table_entry_ptr = &((uint32_t*)current_page_table)[j];

			if (*current_page_table_entry_ptr & PRESENT_PAGE)
			{
				pmm_free_segment((void*)(*current_page_table_entry_ptr & 0xFFFFF000));
			}
		}

		current_page_table += page_table_size;
	}

out:
	kfree((void*)page_directory);

	return err;
}

error_t page_shutdown()
{
	return pmm_shutdown();
}