#ifndef FRAMEBUFFER_H
# define FRAMEBUFFER_H

# define GRAPHIC_COLOR_BLACK	0x000000
# define GRAPHIC_COLOR_WHITE	0xFFFFFF
# define GRAPHIC_COLOR_BLUE		0x0000FF
# define GRAPHIC_COLOR_GREEN	0x00FF00
# define GRAPHIC_COLOR_RED		0xFF0000

# define FRAMEBUFFER_DEFAULT_COLOR	GRAPHIC_COLOR_WHITE

# define SCREEN_WIDTH 1920
# define SCREEN_HEIGHT 1080

# define FRAMEBUFFER_CHAR_WIDTH		8
# define FRAMEBUFFER_CHAR_HEIGHT	16

# include "../multiboot2.h"
# include <stdint.h>
# include "glyph.h"
# include "../libc/libc.h"

typedef struct s_framebuffer
{
	struct multiboot_tag_framebuffer *tag;
	unsigned cursor_x;
	unsigned cursor_y;
}	t_framebuffer;

extern t_framebuffer g_framebuffer;

void
	framebuffer_init(struct multiboot_tag_framebuffer *tagfb);

unsigned
	framebuffer_width(void);

unsigned
	framebuffer_height(void);

void
	framebuffer_set_cursor(unsigned x, unsigned y);

void
	framebuffer_fill_rect(unsigned x, unsigned y,
							unsigned width, unsigned height,
							uint32_t rgb);

void
	framebuffer_scroll_up(unsigned y, unsigned height,
							unsigned offset, uint32_t rgb);

void
	framebuffer_clear(uint32_t rgb);

void
	framebuffer_draw_char(unsigned char c);

void
	framebuffer_draw_string(const char *str);

#endif