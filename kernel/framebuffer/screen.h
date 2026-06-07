#ifndef KEYBOARD_SCREEN_H
# define KEYBOARD_SCREEN_H

# include "../keyboard/keyboard.h"
# include "framebuffer.h"
# include "glyph.h"

# define KEYBOARD_MAX_ROWS		128
# define KEYBOARD_MAX_COLUMNS	256
# define KEYBOARD_CURSOR_CHAR	GLYPH_CURSOR
# define KEYBOARD_NEWLINE		'\n'
# define KEYBOARD_BACKSPACE		'\b'
# define KEYBOARD_LINE_WRAP		0
# define KEYBOARD_LINE_BREAK	1

typedef struct s_keyboard_screen
{
	unsigned start_x;
	unsigned start_y;
	unsigned cursor_col;
	unsigned cursor_row;
	unsigned cursor_x;
	unsigned cursor_y;
	unsigned columns;
	unsigned rows;
	unsigned line_lengths[KEYBOARD_MAX_ROWS];
	int line_started_by_newline[KEYBOARD_MAX_ROWS];
	char lines[KEYBOARD_MAX_ROWS][KEYBOARD_MAX_COLUMNS];
}	t_keyboard_screen;

extern t_keyboard_screen g_keyboard_screen;

void	keyboard_screen_init(unsigned x, unsigned y);
void	keyboard_screen_draw_cursor(void);
void	keyboard_screen_erase_cursor(void);
void	keyboard_screen_handle_key(t_keyboard_key key);

#endif
