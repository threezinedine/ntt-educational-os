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

int8_t strcmp(const char* str1, const char* str2)
{
	uint32_t i = 0;
	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] > str2[i]) return 1;
		if (str1[i] < str2[i]) return -1;
		i++;
	}
	if (str1[i] == '\0' && str2[i] == '\0') return 0;
	if (str1[i] == '\0') return -1;
	return 1;
}

int32_t index_of(const char* str, char c)
{
	int32_t i = 0;
	while (str[i] != '\0')
	{
		if (str[i] == c) return i;
		i++;
	}
	return -1;
}