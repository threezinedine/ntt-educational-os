#ifndef _VGA_H_
#define _VGA_H_
#include <stdint.h>

void vga_init();
void vga_clear();
void vga_shutdown();

void vga_newline();
void vga_putc(char c);
void vga_deletec();
void vga_print(char* str);
void vga_print_hex(uint32_t value);

#endif // _VGA_H_