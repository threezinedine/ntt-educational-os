#include "pmm.h"
#include "memory/kheap.h"
#include "utils/string.h"

#define PMM_SEGMENT_FREE 0x00
#define PMM_SEGMENT_USED 0xFF

static void*	map				 = NULL;
static void*	s_start_offset	 = NULL;
static uint32_t s_segments_count = 0;

error_t pmm_init(uint32_t segments_count)
{
	error_t err		 = OK;
	map				 = kzalloc(segments_count * sizeof(uint32_t));
	s_start_offset	 = kzalloc(segments_count * PAGE_SEGMENT_SIZE);
	s_segments_count = segments_count;

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

	if (s_start_offset)
	{
		kfree(s_start_offset);
		s_start_offset = NULL;
	}

	return OK;
}

void* pmm_get_free_segment()
{
	for (uint32_t i = 0; i < s_segments_count; i++)
	{
		if (((uint8_t*)map)[i] == PMM_SEGMENT_FREE)
		{
			((uint8_t*)map)[i] = PMM_SEGMENT_USED;
			return (void*)((uint32_t)s_start_offset + i * PAGE_SEGMENT_SIZE);
		}
	}

	panic("No free memory segments available");

	return NULL;
}

void pmm_free_segment(void* segment)
{
	if (!map || !segment) return;

	uint32_t index		   = ((uint32_t)segment - (uint32_t)s_start_offset) / PAGE_SEGMENT_SIZE;
	((uint8_t*)map)[index] = PMM_SEGMENT_FREE;
}