#ifndef _HEAP_H_
#define _HEAP_H_

#include "utils/common.h"

#define FREE_BLOCK	   0x00
#define ISOLATED_BLOCK 0x01
#define START_BLOCK	   0x02
#define MIDDLE_BLOCK   0x03
#define END_BLOCK	   0x04

struct heap
{
	uint8_t* blocks_meta;
	void*	 offset;
	uint32_t block_size;
	uint32_t blocks_count;
};

error_t heap_init(struct heap* heap, void* offset, uint32_t block_size, uint32_t blocks_count);
void*	heap_alloc(struct heap* heap, uint32_t size);
void	heap_free(struct heap* heap, void* ptr);
error_t heap_shutdown(struct heap* heap);

#endif /* _HEAP_H_ */