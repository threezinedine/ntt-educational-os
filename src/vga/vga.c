#include "vga.h"
#include "utils/utils.h"

static unsigned char* vga_buffer = (unsigned char*)0xB8000;

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;
static void			set_pointer_position(int x, int y);
static void			set_char(char c, int x, int y);
static void			next_cursor();

void vga_init()
{
	vga_clear();
}

void vga_clear()
{
	for (int y = 0; y < VGA_HEIGHT; y++)
	{
		for (int x = 0; x < VGA_WIDTH; x++)
		{
			set_char(' ', x, y);
		}
	}
	set_pointer_position(0, 0);
}

void vga_shutdown()
{
}

void vga_newline()
{
	cursor_x = 0;
	cursor_y++;
	if (cursor_y >= VGA_HEIGHT)
	{
		cursor_y = 0;
	}
	set_pointer_position(cursor_x, cursor_y);
}

void vga_putc(char c)
{
	if (c == '\n')
	{
		vga_newline();
	}
	else
	{
		set_char(c, cursor_x, cursor_y);
		next_cursor();
	}
}

void vga_deletec()
{
	if (cursor_x > 0)
	{
		cursor_x--;
	}
	else if (cursor_y > 0)
	{
		cursor_y--;
		cursor_x = VGA_WIDTH - 1;
	}
	else
	{
		cursor_x = VGA_WIDTH - 1;
		cursor_y = VGA_HEIGHT - 1;
	}

	set_char(' ', cursor_x, cursor_y);
	set_pointer_position(cursor_x, cursor_y);
}

void vga_print(char* str)
{
	while (*str)
	{
		vga_putc(*str++);
	}
}

static void set_pointer_position(int x, int y)
{
	cursor_x				= x;
	cursor_y				= y;
	unsigned short position = (y * VGA_WIDTH + x);
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char)(position & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

static void next_cursor()
{
	cursor_x++;
	if (cursor_x >= VGA_WIDTH)
	{
		cursor_x = 0;
		cursor_y++;
		if (cursor_y >= VGA_HEIGHT)
		{
			cursor_y = 0;
		}
	}
	set_pointer_position(cursor_x, cursor_y);
}

static void set_char(char c, int x, int y)
{
	vga_buffer[(y * VGA_WIDTH + x) * 2]		= c;
	vga_buffer[(y * VGA_WIDTH + x) * 2 + 1] = 0x07;
}