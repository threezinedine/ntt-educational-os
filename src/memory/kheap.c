#include "kheap.h"

#define KERNEL_BLOCKS_COUNT	 1024
#define KERNEL_BLOCKS_SIZE	 1024
#define KERNEL_MEMORY_OFFSET 0x100000

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