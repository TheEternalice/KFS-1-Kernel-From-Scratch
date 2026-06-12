#include "keyboard.h"

static t_keyboard_queue g_keyboard_queue;
static int g_shift_pressed;
static int g_extended_scancode;

static uint8_t
	inb(uint16_t port)
{
	uint8_t value;

	__asm__ volatile ("inb %1, %0" : "=a" (value) : "Nd" (port));
	return value;
}

static int
	queue_push(t_keyboard_key key)
{
	if (g_keyboard_queue.count == KEYBOARD_QUEUE_SIZE)
		return 0;
	g_keyboard_queue.buffer[g_keyboard_queue.write_index] = key;
	g_keyboard_queue.write_index++;
	if (g_keyboard_queue.write_index == KEYBOARD_QUEUE_SIZE)
		g_keyboard_queue.write_index = 0;
	g_keyboard_queue.count++;
	return 1;
}

int
	keyboard_pop(t_keyboard_key *key)
{
	if (g_keyboard_queue.count == 0)
		return 0;
	*key = g_keyboard_queue.buffer[g_keyboard_queue.read_index];
	g_keyboard_queue.read_index++;
	if (g_keyboard_queue.read_index == KEYBOARD_QUEUE_SIZE)
		g_keyboard_queue.read_index = 0;
	g_keyboard_queue.count--;
	return 1;
}

void
	keyboard_init(void)
{
	g_keyboard_queue.read_index = 0;
	g_keyboard_queue.write_index = 0;
	g_keyboard_queue.count = 0;
	g_shift_pressed = 0;
	g_extended_scancode = 0;
}

static char
	scancode_to_char(uint8_t scancode)
{
	static const char normal_map[KEYBOARD_SCANCODE_MAP_SIZE] = {
		[KEYBOARD_SCANCODE_GRAVE] = '`',
		[KEYBOARD_SCANCODE_1] = '1',
		[KEYBOARD_SCANCODE_2] = '2',
		[KEYBOARD_SCANCODE_3] = '3',
		[KEYBOARD_SCANCODE_4] = '4',
		[KEYBOARD_SCANCODE_5] = '5',
		[KEYBOARD_SCANCODE_6] = '6',
		[KEYBOARD_SCANCODE_7] = '7',
		[KEYBOARD_SCANCODE_8] = '8',
		[KEYBOARD_SCANCODE_9] = '9',
		[KEYBOARD_SCANCODE_0] = '0',
		[KEYBOARD_SCANCODE_MINUS] = '-',
		[KEYBOARD_SCANCODE_EQUAL] = '=',
		[KEYBOARD_SCANCODE_BACKSPACE] = '\b',
		[KEYBOARD_SCANCODE_TAB] = '\t',
		[KEYBOARD_SCANCODE_Q] = 'q',
		[KEYBOARD_SCANCODE_W] = 'w',
		[KEYBOARD_SCANCODE_E] = 'e',
		[KEYBOARD_SCANCODE_R] = 'r',
		[KEYBOARD_SCANCODE_T] = 't',
		[KEYBOARD_SCANCODE_Y] = 'y',
		[KEYBOARD_SCANCODE_U] = 'u',
		[KEYBOARD_SCANCODE_I] = 'i',
		[KEYBOARD_SCANCODE_O] = 'o',
		[KEYBOARD_SCANCODE_P] = 'p',
		[KEYBOARD_SCANCODE_LBRACKET] = '[',
		[KEYBOARD_SCANCODE_RBRACKET] = ']',
		[KEYBOARD_SCANCODE_ENTER] = '\n',
		[KEYBOARD_SCANCODE_A] = 'a',
		[KEYBOARD_SCANCODE_S] = 's',
		[KEYBOARD_SCANCODE_D] = 'd',
		[KEYBOARD_SCANCODE_F] = 'f',
		[KEYBOARD_SCANCODE_G] = 'g',
		[KEYBOARD_SCANCODE_H] = 'h',
		[KEYBOARD_SCANCODE_J] = 'j',
		[KEYBOARD_SCANCODE_K] = 'k',
		[KEYBOARD_SCANCODE_L] = 'l',
		[KEYBOARD_SCANCODE_SEMICOLON] = ';',
		[KEYBOARD_SCANCODE_APOSTROPHE] = '\'',
		[KEYBOARD_SCANCODE_BACKSLASH] = '\\',
		[KEYBOARD_SCANCODE_Z] = 'z',
		[KEYBOARD_SCANCODE_X] = 'x',
		[KEYBOARD_SCANCODE_C] = 'c',
		[KEYBOARD_SCANCODE_V] = 'v',
		[KEYBOARD_SCANCODE_B] = 'b',
		[KEYBOARD_SCANCODE_N] = 'n',
		[KEYBOARD_SCANCODE_M] = 'm',
		[KEYBOARD_SCANCODE_COMMA] = ',',
		[KEYBOARD_SCANCODE_DOT] = '.',
		[KEYBOARD_SCANCODE_SLASH] = '/',
		[KEYBOARD_SCANCODE_COMMA] = '<',
		[KEYBOARD_SCANCODE_DOT] = '>',
		[KEYBOARD_SCANCODE_SLASH] = '?',
		[KEYBOARD_SCANCODE_SPACE] = ' '
	};

	static const char shift_map[KEYBOARD_SCANCODE_MAP_SIZE] = {
		[KEYBOARD_SCANCODE_1] = '!',
		[KEYBOARD_SCANCODE_2] = '@',
		[KEYBOARD_SCANCODE_3] = '#',
		[KEYBOARD_SCANCODE_4] = '$',
		[KEYBOARD_SCANCODE_5] = '%',
		[KEYBOARD_SCANCODE_6] = '^',
		[KEYBOARD_SCANCODE_7] = '&',
		[KEYBOARD_SCANCODE_8] = '*',
		[KEYBOARD_SCANCODE_9] = '(',
		[KEYBOARD_SCANCODE_0] = ')',
		[KEYBOARD_SCANCODE_MINUS] = '_',
		[KEYBOARD_SCANCODE_EQUAL] = '+',
		[KEYBOARD_SCANCODE_LBRACKET] = '{',
		[KEYBOARD_SCANCODE_RBRACKET] = '}',
		[KEYBOARD_SCANCODE_SEMICOLON] = ':',
		[KEYBOARD_SCANCODE_APOSTROPHE] = '"',
		[KEYBOARD_SCANCODE_GRAVE] = '~',
		[KEYBOARD_SCANCODE_BACKSLASH] = '|'
	};
	char c;

	if (g_shift_pressed)
	{
		c = shift_map[scancode];
		if (c != 0)
			return c;
	}

	c = normal_map[scancode];
	if (g_shift_pressed && c >= 'a' && c <= 'z')
		c = c - ('a' - 'A');
	return c;
}

static t_keyboard_key
	extended_scancode_to_key(uint8_t scancode)
{
	switch (scancode)
	{
		case KEYBOARD_SCANCODE_LEFT:
			return KEYBOARD_KEY_LEFT;
		case KEYBOARD_SCANCODE_RIGHT:
			return KEYBOARD_KEY_RIGHT;
		case KEYBOARD_SCANCODE_UP:
			return KEYBOARD_KEY_UP;
		case KEYBOARD_SCANCODE_DOWN:
			return KEYBOARD_KEY_DOWN;
		case KEYBOARD_SCANCODE_DELETE:
			return KEYBOARD_KEY_DELETE;
		default:
			return 0;
	}
}

static int
	is_shift_scancode(uint8_t scancode)
{
	switch (scancode)
	{
		case KEYBOARD_SCANCODE_LSHIFT:
		case KEYBOARD_SCANCODE_RSHIFT:
			return 1;
		default:
			return 0;
	}
}

static int
	is_shift_release_scancode(uint8_t scancode)
{
	switch (scancode)
	{
		case KEYBOARD_SCANCODE_LSHIFT | KEYBOARD_SCANCODE_RELEASE:
		case KEYBOARD_SCANCODE_RSHIFT | KEYBOARD_SCANCODE_RELEASE:
			return 1;
		default:
			return 0;
	}
}

void
	keyboard_poll(void)
{
	uint8_t scancode;
	char c;
	t_keyboard_key key;

	if ((inb(KEYBOARD_STATUS_PORT) & KEYBOARD_OUTPUT_FULL) == 0)
		return;
	scancode = inb(KEYBOARD_DATA_PORT);
	if (scancode == KEYBOARD_SCANCODE_EXTENDED)
	{
		g_extended_scancode = 1;
		return;
	}
	if (g_extended_scancode)
	{
		g_extended_scancode = 0;
		if (scancode & KEYBOARD_SCANCODE_RELEASE)
			return;
		key = extended_scancode_to_key(scancode);
		if (key != 0)
			queue_push(key);
		return;
	}
	if (is_shift_scancode(scancode))
	{
		g_shift_pressed = 1;
		return;
	}
	if (is_shift_release_scancode(scancode))
	{
		g_shift_pressed = 0;
		return;
	}
	if (scancode & KEYBOARD_SCANCODE_RELEASE)
		return;
	c = scancode_to_char(scancode);
	if (c != 0)
		queue_push(c);
}

void
	keyboard_write_loop(unsigned x, unsigned y)
{
	t_keyboard_key key;

	keyboard_init();
	keyboard_screen_init(x, y);
	keyboard_screen_draw_cursor();
	while (1)
	{
		keyboard_poll();
		while (keyboard_pop(&key))
		{
			keyboard_screen_erase_cursor();
			keyboard_screen_handle_key(key);
			keyboard_screen_draw_cursor();
		}
		__asm__ volatile ("pause");
	}
}
