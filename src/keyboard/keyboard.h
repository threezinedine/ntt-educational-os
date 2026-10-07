#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "utils/common.h"

#define KEYBOARD_DATA_PORT	 0x60
#define KEYBOARD_STATUS_PORT 0x64

/* IRQ line the keyboard is wired to (IRQ1). */
#define KEYBOARD_IRQ 1

error_t keyboard_init(void);

#endif /* _KEYBOARD_H_ */
