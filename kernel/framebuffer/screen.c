#include "screen.h"

t_keyboard_screen g_keyboard_screen;

static void
	keyboard_screen_update_cursor(void)
{
	g_keyboard_screen.cursor_x = g_keyboard_screen.start_x
		+ g_keyboard_screen.cursor_col * FRAMEBUFFER_CHAR_WIDTH;
	g_keyboard_screen.cursor_y = g_keyboard_screen.start_y
		+ g_keyboard_screen.cursor_row * FRAMEBUFFER_CHAR_HEIGHT;
}

static void
	keyboard_screen_render_line(unsigned row)
{
	unsigned col;
	unsigned y;

	y = g_keyboard_screen.start_y + row * FRAMEBUFFER_CHAR_HEIGHT;
	framebuffer_fill_rect(g_keyboard_screen.start_x, y,
			g_keyboard_screen.columns * FRAMEBUFFER_CHAR_WIDTH,
			FRAMEBUFFER_CHAR_HEIGHT, GRAPHIC_COLOR_BLACK);
	col = 0;
	while (col < g_keyboard_screen.line_lengths[row])
	{
		framebuffer_set_cursor(
				g_keyboard_screen.start_x + col * FRAMEBUFFER_CHAR_WIDTH, y);
		framebuffer_draw_char(g_keyboard_screen.lines[row][col]);
		col++;
	}
}

void
	keyboard_screen_erase_cursor(void)
{
	keyboard_screen_render_line(g_keyboard_screen.cursor_row);
}

void
	keyboard_screen_draw_cursor(void)
{
	keyboard_screen_update_cursor();
	framebuffer_set_cursor(g_keyboard_screen.cursor_x,
			g_keyboard_screen.cursor_y);
	framebuffer_draw_char(KEYBOARD_CURSOR_CHAR);
}

static void
	keyboard_screen_copy_line(unsigned dst, unsigned src)
{
	unsigned col;

	g_keyboard_screen.line_lengths[dst] = g_keyboard_screen.line_lengths[src];
	g_keyboard_screen.line_started_by_newline[dst]
		= g_keyboard_screen.line_started_by_newline[src];
	col = 0;
	while (col < KEYBOARD_MAX_COLUMNS)
	{
		g_keyboard_screen.lines[dst][col] = g_keyboard_screen.lines[src][col];
		col++;
	}
}

static void
	keyboard_screen_clear_line(unsigned row, int started_by_newline)
{
	unsigned col;

	g_keyboard_screen.line_lengths[row] = 0;
	g_keyboard_screen.line_started_by_newline[row] = started_by_newline;
	col = 0;
	while (col < KEYBOARD_MAX_COLUMNS)
	{
		g_keyboard_screen.lines[row][col] = 0;
		col++;
	}
}

static void
	keyboard_screen_remove_line(unsigned row)
{
	while (row + 1 < g_keyboard_screen.rows)
	{
		keyboard_screen_copy_line(row, row + 1);
		keyboard_screen_render_line(row);
		row++;
	}
	keyboard_screen_clear_line(g_keyboard_screen.rows - 1, KEYBOARD_LINE_WRAP);
	keyboard_screen_render_line(g_keyboard_screen.rows - 1);
}

static void
	keyboard_screen_merge_next_line(void)
{
	unsigned dst;
	unsigned src;
	unsigned next_row;

	next_row = g_keyboard_screen.cursor_row + 1;
	if (next_row >= g_keyboard_screen.rows)
		return;
	dst = g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row];
	src = 0;
	while (src < g_keyboard_screen.line_lengths[next_row]
		&& dst < g_keyboard_screen.columns)
	{
		g_keyboard_screen.lines[g_keyboard_screen.cursor_row][dst]
			= g_keyboard_screen.lines[next_row][src];
		dst++;
		src++;
	}
	g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row] = dst;
	keyboard_screen_render_line(g_keyboard_screen.cursor_row);
	keyboard_screen_remove_line(next_row);
}

static void
	keyboard_screen_scroll(int started_by_newline)
{
	unsigned row;

	framebuffer_scroll_up(g_keyboard_screen.start_y,
			g_keyboard_screen.rows * FRAMEBUFFER_CHAR_HEIGHT,
			FRAMEBUFFER_CHAR_HEIGHT, GRAPHIC_COLOR_BLACK);
	row = 0;
	while (row + 1 < g_keyboard_screen.rows)
	{
		keyboard_screen_copy_line(row, row + 1);
		row++;
	}
	keyboard_screen_clear_line(g_keyboard_screen.rows - 1,
			started_by_newline);
	g_keyboard_screen.cursor_row = g_keyboard_screen.rows - 1;
}

static void
	keyboard_screen_next_line(int started_by_newline)
{
	g_keyboard_screen.cursor_col = 0;
	if (g_keyboard_screen.cursor_row + 1 >= g_keyboard_screen.rows)
		keyboard_screen_scroll(started_by_newline);
	else
	{
		g_keyboard_screen.cursor_row++;
		keyboard_screen_clear_line(g_keyboard_screen.cursor_row,
				started_by_newline);
		keyboard_screen_render_line(g_keyboard_screen.cursor_row);
	}
}

static void
	keyboard_screen_write_char(char c)
{
	unsigned col;
	unsigned len;

	len = g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row];
	if (g_keyboard_screen.cursor_col >= g_keyboard_screen.columns)
		keyboard_screen_next_line(KEYBOARD_LINE_WRAP);
	if (len >= g_keyboard_screen.columns
		&& g_keyboard_screen.cursor_col >= g_keyboard_screen.columns - 1)
		return;
	if (len >= g_keyboard_screen.columns)
		len = g_keyboard_screen.columns - 1;
	col = len;
	while (col > g_keyboard_screen.cursor_col)
	{
		g_keyboard_screen.lines[g_keyboard_screen.cursor_row][col]
			= g_keyboard_screen.lines[g_keyboard_screen.cursor_row][col - 1];
		col--;
	}
	g_keyboard_screen.lines[g_keyboard_screen.cursor_row]
		[g_keyboard_screen.cursor_col] = c;
	g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row] = len + 1;
	keyboard_screen_render_line(g_keyboard_screen.cursor_row);
	g_keyboard_screen.cursor_col++;
	if (g_keyboard_screen.cursor_col >= g_keyboard_screen.columns)
		keyboard_screen_next_line(KEYBOARD_LINE_WRAP);
}

static void
	keyboard_screen_delete_char_at(unsigned col, unsigned row)
{
	unsigned len;

	len = g_keyboard_screen.line_lengths[row];
	if (col >= len)
		return;
	while (col + 1 < len)
	{
		g_keyboard_screen.lines[row][col] = g_keyboard_screen.lines[row][col + 1];
		col++;
	}
	g_keyboard_screen.lines[row][len - 1] = 0;
	g_keyboard_screen.line_lengths[row] = len - 1;
	keyboard_screen_render_line(row);
}

static void
	keyboard_screen_delete_previous_char(void)
{
	if (g_keyboard_screen.cursor_col > 0)
	{
		g_keyboard_screen.cursor_col--;
		keyboard_screen_delete_char_at(g_keyboard_screen.cursor_col,
				g_keyboard_screen.cursor_row);
	}
	else if (g_keyboard_screen.cursor_row > 0
		&& g_keyboard_screen.line_started_by_newline
			[g_keyboard_screen.cursor_row])
	{
		g_keyboard_screen.cursor_row--;
		g_keyboard_screen.cursor_col
			= g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row];
		keyboard_screen_merge_next_line();
	}
	else if (g_keyboard_screen.cursor_row > 0)
	{
		g_keyboard_screen.cursor_row--;
		g_keyboard_screen.cursor_col
			= g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row];
		keyboard_screen_delete_previous_char();
	}
}

static void
	keyboard_screen_delete_next_char(void)
{
	if (g_keyboard_screen.cursor_col
		< g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row])
		keyboard_screen_delete_char_at(g_keyboard_screen.cursor_col,
				g_keyboard_screen.cursor_row);
	else if (g_keyboard_screen.cursor_row + 1 < g_keyboard_screen.rows
		&& g_keyboard_screen.line_started_by_newline
			[g_keyboard_screen.cursor_row + 1])
		keyboard_screen_merge_next_line();
}

static void
	keyboard_screen_move_cursor_left(void)
{
	if (g_keyboard_screen.cursor_col > 0)
		g_keyboard_screen.cursor_col--;
	else if (g_keyboard_screen.cursor_row > 0)
	{
		g_keyboard_screen.cursor_row--;
		g_keyboard_screen.cursor_col
			= g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row];
	}
}

static void
	keyboard_screen_move_cursor_right(void)
{
	if (g_keyboard_screen.cursor_col
		< g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row])
		g_keyboard_screen.cursor_col++;
	else if (g_keyboard_screen.cursor_row + 1 < g_keyboard_screen.rows
		&& g_keyboard_screen.line_started_by_newline
			[g_keyboard_screen.cursor_row + 1])
	{
		g_keyboard_screen.cursor_row++;
		g_keyboard_screen.cursor_col = 0;
	}
}

static void
	keyboard_screen_move_cursor_up(void)
{
	if (g_keyboard_screen.cursor_row == 0)
		return;
	g_keyboard_screen.cursor_row--;
	if (g_keyboard_screen.cursor_col
		> g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row])
		g_keyboard_screen.cursor_col
			= g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row];
}

static void
	keyboard_screen_move_cursor_down(void)
{
	if (g_keyboard_screen.cursor_row + 1 >= g_keyboard_screen.rows)
		return;
	if (!g_keyboard_screen.line_started_by_newline
		[g_keyboard_screen.cursor_row + 1]
		&& g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row + 1] == 0)
		return;
	g_keyboard_screen.cursor_row++;
	if (g_keyboard_screen.cursor_col
		> g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row])
		g_keyboard_screen.cursor_col
			= g_keyboard_screen.line_lengths[g_keyboard_screen.cursor_row];
}

void
	keyboard_screen_handle_key(t_keyboard_key key)
{
	switch (key)
	{
		case KEYBOARD_KEY_LEFT:
			keyboard_screen_move_cursor_left();
			break;
		case KEYBOARD_KEY_RIGHT:
			keyboard_screen_move_cursor_right();
			break;
		case KEYBOARD_KEY_UP:
			keyboard_screen_move_cursor_up();
			break;
		case KEYBOARD_KEY_DOWN:
			keyboard_screen_move_cursor_down();
			break;
		case KEYBOARD_KEY_DELETE:
			keyboard_screen_delete_next_char();
			break;
		case KEYBOARD_NEWLINE:
			keyboard_screen_next_line(KEYBOARD_LINE_BREAK);
			break;
		case KEYBOARD_BACKSPACE:
			keyboard_screen_delete_previous_char();
			break;
		default:
			keyboard_screen_write_char((char) key);
			break;
	}
}

void
	keyboard_screen_init(unsigned x, unsigned y)
{
	unsigned row;

	if (x >= framebuffer_width())
		x = framebuffer_width() - FRAMEBUFFER_CHAR_WIDTH;
	if (y >= framebuffer_height())
		y = framebuffer_height() - FRAMEBUFFER_CHAR_HEIGHT;
	g_keyboard_screen.start_x = 0;
	g_keyboard_screen.start_y = 0;
	g_keyboard_screen.columns = framebuffer_width() / FRAMEBUFFER_CHAR_WIDTH;
	g_keyboard_screen.rows = framebuffer_height() / FRAMEBUFFER_CHAR_HEIGHT;
	if (g_keyboard_screen.rows > KEYBOARD_MAX_ROWS)
		g_keyboard_screen.rows = KEYBOARD_MAX_ROWS;
	if (g_keyboard_screen.columns > KEYBOARD_MAX_COLUMNS)
		g_keyboard_screen.columns = KEYBOARD_MAX_COLUMNS;
	if (g_keyboard_screen.columns == 0)
		g_keyboard_screen.columns = 1;
	if (g_keyboard_screen.rows == 0)
		g_keyboard_screen.rows = 1;
	g_keyboard_screen.cursor_col = x / FRAMEBUFFER_CHAR_WIDTH;
	g_keyboard_screen.cursor_row = y / FRAMEBUFFER_CHAR_HEIGHT;
	if (g_keyboard_screen.cursor_col >= g_keyboard_screen.columns)
		g_keyboard_screen.cursor_col = g_keyboard_screen.columns - 1;
	if (g_keyboard_screen.cursor_row >= g_keyboard_screen.rows)
		g_keyboard_screen.cursor_row = g_keyboard_screen.rows - 1;
	row = 0;
	while (row < KEYBOARD_MAX_ROWS)
	{
		keyboard_screen_clear_line(row, KEYBOARD_LINE_WRAP);
		row++;
	}
	keyboard_screen_update_cursor();
}
