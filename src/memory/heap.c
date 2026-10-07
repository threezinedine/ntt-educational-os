#include "heap.h"
#include "utils/utils.h"

static uint32_t get_needed_blocks_count(uint32_t size, uint32_t block_size);
static int32_t	get_match_free_hole_index(struct heap* heap, uint32_t needed_blocks);

error_t heap_init(struct heap* heap, void* offset, uint32_t block_size, uint32_t blocks_count)
{
	error_t err = OK;

	if (heap == NULL || offset == NULL || block_size == 0 || blocks_count == 0)
	{
		err = ERROR_INVALID_ARGUMENT;
		goto out;
	}

	heap->blocks_meta  = offset;
	heap->offset	   = offset + blocks_count;
	heap->block_size   = block_size;
	heap->blocks_count = blocks_count;

	if (((uint32_t)heap->offset % block_size) != 0)
	{
		return ERROR_MISALIGNED_BLOCK;
	}

	memset(heap->blocks_meta, FREE_BLOCK, blocks_count);

out:
	return err;
}

// TODO: trigger error if any allocation fails
void* heap_alloc(struct heap* heap, uint32_t size)
{
	if (heap == NULL || size == 0)
	{
		return NULL;
	}

	uint32_t needed_blocks = get_needed_blocks_count(size, heap->block_size);

	if (needed_blocks == 0)
	{
		return NULL;
	}

	int32_t free_hole_index = get_match_free_hole_index(heap, needed_blocks);
	if (free_hole_index == -1)
	{
		return NULL;
	}

	if (needed_blocks == 1)
	{
		heap->blocks_meta[free_hole_index] = ISOLATED_BLOCK;
	}

	if (needed_blocks > 1)
	{
		for (uint32_t i = 0; i < needed_blocks; i++)
		{
			if (i == 0)
			{
				heap->blocks_meta[free_hole_index + i] = START_BLOCK;
			}
			else if (i == needed_blocks - 1)
			{
				heap->blocks_meta[free_hole_index + i] = END_BLOCK;
			}
			else
			{
				heap->blocks_meta[free_hole_index + i] = MIDDLE_BLOCK;
			}
		}
	}

	return heap->offset + (free_hole_index * heap->block_size);
}

// TODO: trigger error if any deallocation fails
void heap_free(struct heap* heap, void* ptr)
{
	if (heap == NULL || ptr == NULL)
	{
		return;
	}

	uint32_t block_index = ((uint32_t)ptr - (uint32_t)heap->offset) / heap->block_size;
	if (block_index >= heap->blocks_count)
	{
		return;
	}

	switch (heap->blocks_meta[block_index])
	{
	case ISOLATED_BLOCK:
		heap->blocks_meta[block_index] = FREE_BLOCK;
		break;
	case START_BLOCK:
		heap->blocks_meta[block_index] = FREE_BLOCK;
		block_index++;
		while (block_index < heap->blocks_count && heap->blocks_meta[block_index] == MIDDLE_BLOCK)
		{
			heap->blocks_meta[block_index] = FREE_BLOCK;
			block_index++;
		}
		if (block_index < heap->blocks_count && heap->blocks_meta[block_index] == END_BLOCK)
		{
			heap->blocks_meta[block_index] = FREE_BLOCK;
		}
		break;
	default:
		break;
	}
}

error_t heap_shutdown(struct heap* heap)
{
	error_t err = OK;

out:
	return err;
}

static uint32_t get_needed_blocks_count(uint32_t size, uint32_t block_size)
{
	if (size == 0 || block_size == 0)
	{
		return 0;
	}

	if (size % block_size == 0)
	{
		return size / block_size;
	}
	else
	{
		return (size / block_size) + 1;
	}
}

static int32_t get_match_free_hole_index(struct heap* heap, uint32_t needed_blocks)
{
	uint32_t consecutive_free_blocks = 0;

	for (uint32_t i = 0; i < heap->blocks_count; i++)
	{
		if (heap->blocks_meta[i] == FREE_BLOCK)
		{
			consecutive_free_blocks++;
			if (consecutive_free_blocks == needed_blocks)
			{
				return i - needed_blocks + 1;
			}
		}
		else
		{
			consecutive_free_blocks = 0;
		}
	}

	return -1;
}