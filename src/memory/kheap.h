#ifndef _KHEAP_H_
#define _KHEAP_H_

#include "heap.h"

error_t kinit();
void*	kalloc(uint32_t size);
void*	kzalloc(uint32_t size);
void	kfree(void* ptr);
error_t kshutdown();

#endif //_KHEAP_H_