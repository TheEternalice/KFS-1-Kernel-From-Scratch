#ifndef FRAMEBUFFER_H
# define FRAMEBUFFER_H

# define GRAPHIC_COLOR_BLACK	0x000000
# define GRAPHIC_COLOR_WHITE	0xFFFFFF
# define GRAPHIC_COLOR_BLUE		0x0000FF
# define GRAPHIC_COLOR_GREEN	0x00FF00
# define GRAPHIC_COLOR_RED		0xFF0000

# define FRAMEBUFFER_DEFAULT_COLOR	GRAPHIC_COLOR_WHITE
# define FRAMEBUFFER_CHAR_WIDTH		GLYPH_WIDTH
# define FRAMEBUFFER_CHAR_HEIGHT	GLYPH_HEIGHT
# define FRAMEBUFFER_BYTES_PER_PIXEL	4

# include "../multiboot2.h"
# include <stdint.h>
# include "glyph.h"
# include "../libc/libc.h"

typedef struct s_framebuffer
{
	struct multiboot_tag_framebuffer *tag;
}	t_framebuffer;

extern t_framebuffer g_framebuffer;

void		framebuffer_init(struct multiboot_tag_framebuffer *tagfb);
unsigned	framebuffer_width(void);
unsigned	framebuffer_height(void);
void		framebuffer_clear(uint32_t rgb);
void		framebuffer_fill_rect(unsigned x, unsigned y,
									unsigned width, unsigned height,
									uint32_t rgb);
void		framebuffer_scroll_up(unsigned y, unsigned height,
									unsigned offset, uint32_t rgb);
void		framebuffer_draw_glyph(unsigned x, unsigned y,
									unsigned char c, uint32_t fg, uint32_t bg);
void		framebuffer_draw_glyph_overlay(unsigned x, unsigned y,
											unsigned char c, uint32_t fg);

#endif
