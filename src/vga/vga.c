#include "vga.h"
#include "utils/utils.h"

static unsigned char* vga_buffer = (unsigned char*)0xB8000;

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static void set_pointer_position(int x, int y)
{
	unsigned short position = (y * VGA_WIDTH + x);
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char)(position & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

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
			vga_buffer[(y * VGA_WIDTH + x) * 2]		= ' ';
			vga_buffer[(y * VGA_WIDTH + x) * 2 + 1] = 0x07;
		}
	}
	set_pointer_position(0, 0);
}

void vga_shutdown()
{
}