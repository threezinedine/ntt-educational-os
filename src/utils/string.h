#ifndef _STRING_H_
#define _STRING_H_
#include "common.h"

void memset(void* ptr, uint8_t value, uint64_t num);
void memclear(void* ptr, uint64_t num);
void memcpy(void* dest, const void* src, uint64_t num);

#endif /* _STRING_H_ */