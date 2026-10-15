#ifndef _VGA_H_
#define _VGA_H_
#include <stdint.h>

#define TEXT_COLOR_RED	   0x04
#define TEXT_COLOR_GREEN   0x02
#define TEXT_COLOR_BLUE	   0x01
#define TEXT_COLOR_WHITE   0x07
#define TEXT_COLOR_BLACK   0x00
#define TEXT_COLOR_YELLOW  0x0E
#define TEXT_COLOR_CYAN	   0x03
#define TEXT_COLOR_MAGENTA 0x05

void vga_init();
void vga_clear();
void vga_shutdown();

void vga_set_text_color(uint8_t color);
void vga_set_background_color(uint8_t color);

void vga_newline();
void vga_putc(char c);
void vga_deletec();
void vga_print(char* str);
void vga_print_hex(uint32_t value);
void vga_print_dec(uint32_t value);

void vga_scrollup();

#endif // _VGA_H_