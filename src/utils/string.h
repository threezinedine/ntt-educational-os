#ifndef _STRING_H_
#define _STRING_H_
#include "common.h"

void memset(void* ptr, uint8_t value, uint64_t num);
void memclear(void* ptr, uint64_t num);
void memcpy(void* dest, const void* src, uint64_t num);

uint32_t strlen(const char* str);
int8_t	 strcmp(const char* str1, const char* str2);				 // 0: str1 == str2, 1: str1 > str2, -1: str1 < str2
int8_t	 strncmp(const char* str1, const char* str2, uint32_t size); // 0: str1 == str2, 1: str1 > str2, -1: str1 < str2

int32_t index_of(const char* str, char c);

#endif /* _STRING_H_ */