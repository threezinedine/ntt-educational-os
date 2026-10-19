#include "kheap.h"
#include "utils/string.h"

#define KERNEL_BLOCKS_COUNT	 1024 * 1024 * 2
#define KERNEL_BLOCKS_SIZE	 4096
#define KERNEL_MEMORY_OFFSET 0x100000

// Total 4096 B * 1024 * 1024 * 2 = 8 GB

static struct heap kheap;

error_t kinit()
{
	return heap_init(&kheap, (void*)KERNEL_MEMORY_OFFSET, KERNEL_BLOCKS_SIZE, KERNEL_BLOCKS_COUNT);
}

void* kalloc(uint32_t size)
{
	return heap_alloc(&kheap, size);
}

void kfree(void* ptr)
{
	heap_free(&kheap, ptr);
}

error_t kshutdown()
{
	return heap_shutdown(&kheap);
}

void* kzalloc(uint32_t size)
{
	void* ptr = heap_alloc(&kheap, size);
	if (ptr)
	{
		memset(ptr, 0, size);
	}
	return ptr;
}