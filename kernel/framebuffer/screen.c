#include "screen.h"

t_keyboard_screen g_screen;

static unsigned
	column_to_pixel_x(unsigned col)
{
	return KEYBOARD_AREA_START_X + col * FRAMEBUFFER_CHAR_WIDTH;
}

static unsigned
	row_to_pixel_y(unsigned row)
{
	return KEYBOARD_AREA_START_Y + row * FRAMEBUFFER_CHAR_HEIGHT;
}

static void
	draw_cell(unsigned col, unsigned row, char c)
{
	framebuffer_draw_glyph(column_to_pixel_x(col), row_to_pixel_y(row),
			(unsigned char) c, FRAMEBUFFER_DEFAULT_COLOR, GRAPHIC_COLOR_BLACK);
}

static void
	redraw_line(unsigned row, unsigned from_col)
{
	t_terminal *t;
	unsigned col;
	unsigned len;
	unsigned pixel_x;
	unsigned pixel_y;

	t = SCREEN_TERM();
	if (from_col >= g_screen.columns)
		return;
	len = t->lengths[row];
	pixel_x = column_to_pixel_x(from_col);
	pixel_y = row_to_pixel_y(row);
	framebuffer_fill_rect(pixel_x, pixel_y,
			(g_screen.columns - from_col) * FRAMEBUFFER_CHAR_WIDTH,
			FRAMEBUFFER_CHAR_HEIGHT, GRAPHIC_COLOR_BLACK);
	col = from_col;
	while (col < len)
	{
		draw_cell(col, row, t->lines[row][col]);
		col++;
	}
}

static void
	redraw_all(void)
{
	t_terminal *t;
	unsigned row;
	unsigned pixel_y;

	t = SCREEN_TERM();
	pixel_y = row_to_pixel_y(0);
	framebuffer_fill_rect(KEYBOARD_AREA_START_X, pixel_y, framebuffer_width(),
			g_screen.rows * FRAMEBUFFER_CHAR_HEIGHT, GRAPHIC_COLOR_BLACK);
	row = 0;
	while (row < g_screen.rows)
	{
		if (t->lengths[row] > 0)
			redraw_line(row, 0);
		row++;
	}
}

static void
	clear_line(t_terminal *t, unsigned row, int from_newline)
{
	unsigned col;

	t->lengths[row] = 0;
	t->started_by_newline[row] = from_newline;
	col = 0;
	while (col < SCREEN_MAX_COLUMNS)
		t->lines[row][col++] = 0;
}

static void
	newline(int from_enter_key)
{
	t_terminal *t;
	unsigned row;
	unsigned pixel_y;

	t = SCREEN_TERM();
	t->col = 0;
	if (t->row + 1 >= g_screen.rows)
	{
		framebuffer_scroll_up(row_to_pixel_y(0),
				g_screen.rows * FRAMEBUFFER_CHAR_HEIGHT,
				FRAMEBUFFER_CHAR_HEIGHT, GRAPHIC_COLOR_BLACK);
		row = 0;
		while (row + 1 < g_screen.rows)
		{
			unsigned col;

			t->lengths[row] = t->lengths[row + 1];
			t->started_by_newline[row] = t->started_by_newline[row + 1];
			col = 0;
			while (col < SCREEN_MAX_COLUMNS)
			{
				t->lines[row][col] = t->lines[row + 1][col];
				col++;
			}
			row++;
		}
		clear_line(t, g_screen.rows - 1, from_enter_key);
		t->row = g_screen.rows - 1;
	}
	else
	{
		t->row++;
		clear_line(t, t->row, from_enter_key);
		pixel_y = row_to_pixel_y(t->row);
		framebuffer_fill_rect(KEYBOARD_AREA_START_X, pixel_y,
				g_screen.columns * FRAMEBUFFER_CHAR_WIDTH,
				FRAMEBUFFER_CHAR_HEIGHT, GRAPHIC_COLOR_BLACK);
	}
}

static void
	put_char(char c)
{
	t_terminal *t;
	unsigned row;
	unsigned len;
	unsigned col;

	t = SCREEN_TERM();
	row = t->row;
	len = t->lengths[row];
	if (t->col >= g_screen.columns)
		newline(0);
	if (len >= g_screen.columns && t->col >= g_screen.columns - 1)
		return;
	if (t->col == len)
	{
		t->lines[row][t->col] = c;
		t->lengths[row] = len + 1;
		draw_cell(t->col, row, c);
		t->col++;
	}
	else
	{
		col = len;
		while (col > t->col)
		{
			t->lines[row][col] = t->lines[row][col - 1];
			col--;
		}
		t->lines[row][t->col] = c;
		t->lengths[row] = len + 1;
		redraw_line(row, t->col);
		t->col++;
	}
	if (t->col >= g_screen.columns)
		newline(0);
}

static void
	delete_at(unsigned col, unsigned row)
{
	t_terminal *t;
	unsigned len;
	unsigned start;

	t = SCREEN_TERM();
	len = t->lengths[row];
	start = col;
	if (col >= len)
		return;
	while (col + 1 < len)
	{
		t->lines[row][col] = t->lines[row][col + 1];
		col++;
	}
	t->lines[row][len - 1] = 0;
	t->lengths[row] = len - 1;
	redraw_line(row, start);
}

static void
	draw_status_bar(void)
{
	unsigned i;
	unsigned tab_pixel_x;
	uint32_t foreground_color;

	framebuffer_fill_rect(0, 0, framebuffer_width(), FRAMEBUFFER_CHAR_HEIGHT,
			GRAPHIC_COLOR_BLUE);
	tab_pixel_x = 8;
	i = 0;
	while (i < VIRTUAL_DESKTOP_COUNT)
	{
		foreground_color = (i == g_screen.active) ? GRAPHIC_COLOR_GREEN
			: FRAMEBUFFER_DEFAULT_COLOR;
		framebuffer_draw_glyph(tab_pixel_x, 0, 'F', foreground_color,
				GRAPHIC_COLOR_BLUE);
		framebuffer_draw_glyph(tab_pixel_x + FRAMEBUFFER_CHAR_WIDTH, 0,
				'1' + (unsigned char) i, foreground_color, GRAPHIC_COLOR_BLUE);
		tab_pixel_x += FRAMEBUFFER_CHAR_WIDTH * 4;
		i++;
	}
}

void
	keyboard_screen_erase_cursor(void)
{
	t_terminal *t;
	char c;

	if (!g_screen.cursor_on)
		return;
	t = SCREEN_TERM();
	c = (t->col < t->lengths[t->row]) ? t->lines[t->row][t->col] : ' ';
	draw_cell(t->col, t->row, c);
	g_screen.cursor_on = 0;
}

void
	keyboard_screen_draw_cursor(void)
{
	t_terminal *t;
	char c;

	t = SCREEN_TERM();
	c = (t->col < t->lengths[t->row]) ? t->lines[t->row][t->col] : ' ';
	draw_cell(t->col, t->row, c);
	framebuffer_draw_glyph_overlay(column_to_pixel_x(t->col),
			row_to_pixel_y(t->row), GLYPH_CURSOR, FRAMEBUFFER_DEFAULT_COLOR);
	g_screen.cursor_on = 1;
}

void
	keyboard_screen_putchar(char c)
{
	if (c == KEYBOARD_CHAR_NEWLINE)
		newline(1);
	else
		put_char(c);
}

void
	keyboard_screen_handle_key(t_keyboard_key key)
{
	t_terminal *t;

	if (key >= KEYBOARD_KEY_F1
		&& key < KEYBOARD_KEY_F1 + VIRTUAL_DESKTOP_COUNT)
	{
		unsigned index;

		index = (unsigned) (key - KEYBOARD_KEY_F1);
		if (index != g_screen.active)
		{
			keyboard_screen_erase_cursor();
			g_screen.active = index;
			redraw_all();
			draw_status_bar();
			keyboard_screen_draw_cursor();
		}
		return;
	}
	t = SCREEN_TERM();
	switch (key)
	{
		case KEYBOARD_KEY_LEFT:
			if (t->col > 0)
				t->col--;
			else if (t->row > 0)
			{
				t->row--;
				t->col = t->lengths[t->row];
			}
			break;
		case KEYBOARD_KEY_RIGHT:
			if (t->col < t->lengths[t->row])
				t->col++;
			else if (t->row + 1 < g_screen.rows
				&& t->started_by_newline[t->row + 1])
			{
				t->row++;
				t->col = 0;
			}
			break;
		case KEYBOARD_KEY_UP:
			if (t->row > 0)
			{
				t->row--;
				if (t->col > t->lengths[t->row])
					t->col = t->lengths[t->row];
			}
			break;
		case KEYBOARD_KEY_DOWN:
			if (t->row + 1 >= g_screen.rows)
				break;
			if (!t->started_by_newline[t->row + 1]
				&& t->lengths[t->row + 1] == 0)
				break;
			t->row++;
			if (t->col > t->lengths[t->row])
				t->col = t->lengths[t->row];
			break;
		case KEYBOARD_KEY_DELETE:
			delete_at(t->col, t->row);
			break;
		case KEYBOARD_CHAR_NEWLINE:
			newline(1);
			break;
		case KEYBOARD_CHAR_BACKSPACE:
			if (t->col > 0)
			{
				t->col--;
				delete_at(t->col, t->row);
			}
			break;
		default:
			put_char((char) key);
			break;
	}
}

void
	keyboard_screen_init(void)
{
	unsigned i;
	unsigned row;
	unsigned height;

	height = framebuffer_height();
	g_screen.columns = framebuffer_width() / FRAMEBUFFER_CHAR_WIDTH;
	g_screen.rows = (height - KEYBOARD_AREA_START_Y) / FRAMEBUFFER_CHAR_HEIGHT;
	if (g_screen.columns > SCREEN_MAX_COLUMNS)
		g_screen.columns = SCREEN_MAX_COLUMNS;
	if (g_screen.rows > SCREEN_MAX_ROWS)
		g_screen.rows = SCREEN_MAX_ROWS;
	if (g_screen.columns == 0)
		g_screen.columns = 1;
	if (g_screen.rows == 0)
		g_screen.rows = 1;
	g_screen.active = 0;
	g_screen.cursor_on = 0;
	i = 0;
	while (i < VIRTUAL_DESKTOP_COUNT)
	{
		g_screen.terms[i].col = 0;
		g_screen.terms[i].row = 0;
		row = 0;
		while (row < SCREEN_MAX_ROWS)
			clear_line(&g_screen.terms[i], row++, 0);
		i++;
	}
	draw_status_bar();
}
