#include "string.h"

void memset(void* ptr, uint8_t value, uint64_t num)
{
	for (uint64_t i = 0; i < num; i++)
	{
		((uint8_t*)ptr)[i] = value;
	}
}

void memclear(void* ptr, uint64_t num)
{
	memset(ptr, 0, num);
}

void memcpy(void* dest, const void* src, uint64_t num)
{
	for (uint64_t i = 0; i < num; i++)
	{
		((uint8_t*)dest)[i] = ((const uint8_t*)src)[i];
	}
}

uint32_t strlen(const char* str)
{
	uint32_t length = 0;
	while (str[length] != '\0')
	{
		length++;
	}
	return length;
}