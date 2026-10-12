#ifndef _UTILS_H_
#define _UTILS_H_
#include "common.h"
#include "config.h"
#include "string.h"

extern void	   outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);
extern void	   outsw(uint16_t port, uint32_t* buffer, uint32_t count);
extern void	   insw(uint16_t port, uint32_t* buffer, uint32_t count);

#include "pathparser.h"

#include "panic.h"

#endif /* _UTILS_H_ */