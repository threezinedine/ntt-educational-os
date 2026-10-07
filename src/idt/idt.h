#ifndef _IDT_H_
#define _IDT_H_

#include "utils/common.h"

#define TASK_GATE		  0x05
#define INTERRUPT_GATE_16 0x06
#define TRAP_GATE_16	  0x07
#define INTERRUPT_GATE_32 0x0E
#define TRAP_GATE_32	  0x0F

typedef void (*idt_handler_t)(void);

error_t idt_init(void);
error_t idt_setup(uint8_t		interrupt_number,
				  uint8_t		gate_type,
				  uint8_t		privilege_level,
				  uint8_t		segment_selector,
				  idt_handler_t handler);

#endif /* _IDT_H_ */