#ifndef KEYBOARD_SCREEN_H
# define KEYBOARD_SCREEN_H

# include "kfs_config.h"
# include "../keyboard/keyboard.h"
# include "framebuffer.h"

# define SCREEN_MAX_COLUMNS		(SCREEN_WIDTH / FRAMEBUFFER_CHAR_WIDTH)
# define SCREEN_MAX_ROWS		(SCREEN_HEIGHT / FRAMEBUFFER_CHAR_HEIGHT)

# define KEYBOARD_AREA_START_X		0
# define KEYBOARD_AREA_START_Y		FRAMEBUFFER_CHAR_HEIGHT

# define SCREEN_TERM()				(&g_screen.terms[g_screen.active])

typedef struct s_terminal
{
	unsigned col;
	unsigned row;
	unsigned lengths[SCREEN_MAX_ROWS];
	int started_by_newline[SCREEN_MAX_ROWS];
	char lines[SCREEN_MAX_ROWS][SCREEN_MAX_COLUMNS];
}	t_terminal;

typedef struct s_keyboard_screen
{
	unsigned columns;
	unsigned rows;
	unsigned active;
	int cursor_on;
	t_terminal terms[VIRTUAL_DESKTOP_COUNT];
}	t_keyboard_screen;

extern t_keyboard_screen g_screen;

void	keyboard_screen_init(void);
void	keyboard_screen_putchar(char c);
void	keyboard_screen_draw_cursor(void);
void	keyboard_screen_erase_cursor(void);
void	keyboard_screen_handle_key(t_keyboard_key key);

#endif
