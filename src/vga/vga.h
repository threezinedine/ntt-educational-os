#ifndef _VGA_H_
#define _VGA_H_

void vga_init();
void vga_clear();
void vga_shutdown();

void vga_newline();
void vga_putc(char c);
void vga_print(char* str);

#endif // _VGA_H_