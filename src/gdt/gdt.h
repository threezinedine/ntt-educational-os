#ifndef _GDT_H_
#define _GDT_H_

#include "utils/utils.h"

#define GDT_PRESENT	  (1 << 7)
#define GDT_DPL_RING0 (0 << 5)
#define GDT_DPL_RING3 (3 << 5)

#define GDT_CODE_SEGMENT (0b11 << 3)
#define GDT_DATA_SEGMENT (0b10 << 3)
#define GDT_TSS_SEGMENT	 (0b01 << 3)

#define GDT_DATA_READONLY  (0 << 1)
#define GDT_DATA_READWRITE (1 << 1)

#define GDT_CODE_READABLE (1 << 1)

#define GDT_DISABLED (1 << 0)

#define GDT_GRANULARITY_4K (1 << 3)
#define GDT_32BIT		   (1 << 2)
#define GDT_16BIT		   (0 << 2)
#define GDT_LONG_MODE	   (1 << 1)
#define GDT_NO_LONG_MODE   (0 << 1)

error_t		gdt_setup();
error_t		gdt_set_gate(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
extern void tss_flush();

#endif /* _GDT_H_ */