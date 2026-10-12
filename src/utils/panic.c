#include "panic.h"
#include "vga/vga.h"

void panic(const char* message)
{
	vga_set_text_color(TEXT_COLOR_WHITE);
	vga_set_background_color(TEXT_COLOR_RED);
	vga_print("\n");
	vga_print(message);

	while (1)
	{
	}
}