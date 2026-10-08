#include "pmm.h"
#include "memory/kheap.h"
#include "utils/string.h"

#define PMM_SEGMENT_FREE 0x00
#define PMM_SEGMENT_USED 0xFF

static void* map = NULL;

error_t pmm_init(uint32_t segments_count)
{
	error_t err = OK;
	map			= kzalloc(segments_count * sizeof(uint32_t));

	if (!map)
	{
		err = ERROR_OUT_OF_MEMORY;
		goto out;
	}

out:
	return err;
}

error_t pmm_shutdown()
{
	if (map)
	{
		kfree(map);
		map = NULL;
	}

	return OK;
}

void* pmm_get_free_segment()
{
	for (uint32_t i = 0; i < PAGE_SEGMENT_SIZE; i++)
	{
		if (((uint8_t*)map)[i] == PMM_SEGMENT_FREE)
		{
			((uint8_t*)map)[i] = PMM_SEGMENT_USED;
			return (void*)(i * PAGE_SEGMENT_SIZE);
		}
	}

	return NULL;
}

void pmm_free_segment(void* segment)
{
	if (!map || !segment) return;

	uint32_t index		   = (uint32_t)segment / PAGE_SEGMENT_SIZE;
	((uint8_t*)map)[index] = PMM_SEGMENT_FREE;
}