#include "keyboard.h"

#include "idt/idt.h"
#include "utils/utils.h"
#include "vga/vga.h"

/* Scancode set 1 (XT) -> ASCII, unshifted. */
static const char scancode_ascii[128] = {
	0,	 27,  '1', '2', '3', '4', '5',	'6', '7', '8', '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e',  'r', 't', 'y',
	'u', 'i', 'o', 'p', '[', ']', '\n', 0,	 'a', 's', 'd', 'f', 'g', 'h', 'j',	 'k',  'l', ';', '\'', '`', 0,	 '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm',	',', '.', '/', 0,	'*', 0,	  ' ', 0,	 0,	   0,	0,	 0,	   0,
};

/* Same table, but for when Shift is held. */
static const char scancode_ascii_shift[128] = {
	0,	 27,  '!', '@', '#', '$', '%',	'^', '&', '*', '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y',
	'U', 'I', 'O', 'P', '{', '}', '\n', 0,	 'A', 'S', 'D', 'F', 'G', 'H', 'J',	 'K',  'L', ':', '"', '~', 0,	'|',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',	'<', '>', '?', 0,	'*', 0,	  ' ', 0,	 0,	   0,	0,	 0,	  0,
};

static uint8_t shift_pressed = 0;
static uint8_t caps_lock	 = 0;
static uint8_t extended		 = 0;

static void keyboard_irq_handler(void)
{
	uint8_t scancode  = inb(KEYBOARD_DATA_PORT);
	uint8_t backspace = 0;

	/* 0xE0 prefixes an extended (arrow/function) key. Ignore those for now. */
	if (scancode == 0xE0)
	{
		extended = 1;
		return;
	}
	if (extended)
	{
		extended = 0;
		return;
	}

	/* Bit 7 set means the key was released. */
	if (scancode & 0x80)
	{
		uint8_t released = scancode & 0x7F;
		if (released == 0x2A || released == 0x36) /* Left/right shift */
		{
			shift_pressed = 0;
		}
		return;
	}

	switch (scancode)
	{
	case 0x0E: /* Backspace */
		vga_deletec();
		return;
	case 0x2A: /* Left shift */
	case 0x36: /* Right shift */
		shift_pressed = 1;
		return;
	case 0x3A: /* Caps lock */
		caps_lock ^= 1;
		return;
	default:
		break;
	}

	char c = shift_pressed ? scancode_ascii_shift[scancode] : scancode_ascii[scancode];

	if (caps_lock && c >= 'a' && c <= 'z')
	{
		c -= 'a' - 'A';
	}

	if (c)
	{
		vga_putc(c);
	}
}

error_t keyboard_init(void)
{
	error_t err = idt_set_handler(PIC_KEYBOARD_IRQ, keyboard_irq_handler);
	if (err != OK)
	{
		return err;
	}

	/* idt_init() masks every IRQ, so the keyboard line must be unmasked. */
	idt_enable_irq(KEYBOARD_IRQ);

	return OK;
}
