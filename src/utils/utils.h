#ifndef _UTILS_H_
#define _UTILS_H_
#include "common.h"
#include "config.h"
#include "string.h"

extern void			 outb(unsigned short port, unsigned char value);
extern unsigned char inb(unsigned short port);

#endif /* _UTILS_H_ */