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
error_t idt_set_handler(uint8_t irq, idt_handler_t handler);

void idt_enable_irq(uint8_t irq);
void idt_disable_irq(uint8_t irq);

void idt_enable_timer();
void idt_disable_timer();

#endif /* _IDT_H_ */