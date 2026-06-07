#include "framebuffer.h"

t_framebuffer g_framebuffer;

void
	framebuffer_init(struct multiboot_tag_framebuffer *tagfb)
{
	g_framebuffer.tag = tagfb;
	g_framebuffer.cursor_x = 0;
	g_framebuffer.cursor_y = 0;
}

unsigned
	framebuffer_width(void)
{
	if (!g_framebuffer.tag)
		return 0;
	return g_framebuffer.tag->common.framebuffer_width;
}

unsigned
	framebuffer_height(void)
{
	if (!g_framebuffer.tag)
		return 0;
	return g_framebuffer.tag->common.framebuffer_height;
}

void
	framebuffer_set_cursor(unsigned x, unsigned y)
{
	g_framebuffer.cursor_x = x;
	g_framebuffer.cursor_y = y;
}

static uint32_t
	convert_color_for_framebuffer(uint32_t rgb)
{
	uint8_t red = (rgb >> 16) & 0xff;
	uint8_t green = (rgb >> 8) & 0xff;
	uint8_t blue = rgb & 0xff;

	if (!g_framebuffer.tag)
		return 0;
	switch (g_framebuffer.tag->common.framebuffer_type)
	{
		case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
		{
			uint32_t red_mask;
			uint32_t green_mask;
			uint32_t blue_mask;
			uint32_t red_value;
			uint32_t green_value;
			uint32_t blue_value;
			uint32_t red_pixel;
			uint32_t green_pixel;
			uint32_t blue_pixel;

			red_mask = (1 << g_framebuffer.tag->framebuffer_red_mask_size) - 1;
			green_mask = (1 << g_framebuffer.tag->framebuffer_green_mask_size) - 1;
			blue_mask = (1 << g_framebuffer.tag->framebuffer_blue_mask_size) - 1;

			red_value = (red >> (8 - g_framebuffer.tag->framebuffer_red_mask_size))
						& red_mask;
			green_value = (green >> (8 - g_framebuffer.tag->framebuffer_green_mask_size))
						& green_mask;
			blue_value = (blue >> (8 - g_framebuffer.tag->framebuffer_blue_mask_size))
						& blue_mask;

			red_pixel = red_value << g_framebuffer.tag->framebuffer_red_field_position;
			green_pixel = green_value << g_framebuffer.tag->framebuffer_green_field_position;
			blue_pixel = blue_value << g_framebuffer.tag->framebuffer_blue_field_position;

			return red_pixel | green_pixel | blue_pixel;
		}

		default:
			printk("Unsupported framebuffer type: %u\n",
					g_framebuffer.tag->common.framebuffer_type);
			return 0;
	}
}

static void
	framebuffer_put_pixel(unsigned x, unsigned y,
							uint32_t color)
{
	void *fb;
	unsigned pitch;
	uint32_t *pixel;

	if (!g_framebuffer.tag)
		return;
	fb = (void *) (uint32_t) g_framebuffer.tag->common.framebuffer_addr;
	pitch = g_framebuffer.tag->common.framebuffer_pitch;
	pixel = fb + pitch * y + 4 * x;
	*pixel = color;
}

void
	framebuffer_fill_rect(unsigned x, unsigned y,
							unsigned width, unsigned height,
							uint32_t rgb)
{
	uint32_t color;
	unsigned current_x;
	unsigned current_y;

	if (!g_framebuffer.tag)
		return;
	color = convert_color_for_framebuffer(rgb);
	for (current_y = y; current_y < y + height
		&& current_y < framebuffer_height(); current_y++)
		for (current_x = x; current_x < x + width
			&& current_x < framebuffer_width(); current_x++)
			framebuffer_put_pixel(current_x, current_y, color);
}

void
	framebuffer_scroll_up(unsigned y, unsigned height,
							unsigned offset, uint32_t rgb)
{
	uint32_t *src;
	uint32_t *dst;
	uint8_t *fb;
	unsigned pitch;
	unsigned current_y;
	unsigned x;

	if (!g_framebuffer.tag)
		return;
	if (offset == 0 || offset >= height)
		return;
	fb = (uint8_t *) (uint32_t) g_framebuffer.tag->common.framebuffer_addr;
	pitch = g_framebuffer.tag->common.framebuffer_pitch;
	for (current_y = y; current_y < y + height - offset; current_y++)
	{
		dst = (uint32_t *) (fb + pitch * current_y);
		src = (uint32_t *) (fb + pitch * (current_y + offset));
		for (x = 0; x < framebuffer_width(); x++)
			dst[x] = src[x];
	}
	framebuffer_fill_rect(0, y + height - offset,
		framebuffer_width(), offset, rgb);
}

void
	framebuffer_clear(uint32_t rgb)
{
	uint32_t color;
	unsigned x;
	unsigned y;

	if (!g_framebuffer.tag)
		return;
	color = convert_color_for_framebuffer(rgb);
	for (y = 0; y < framebuffer_height(); y++)
		for (x = 0; x < framebuffer_width(); x++)
			framebuffer_put_pixel(x, y, color);
}

void
	framebuffer_draw_char(unsigned char c)
{
	const uint8_t *glyph;
	uint32_t color;
	unsigned row;
	unsigned col;

	if (c == '\n')
	{
		g_framebuffer.cursor_x = 0;
		g_framebuffer.cursor_y += GLYPH_HEIGHT;
		return;
	}
	if (!g_framebuffer.tag)
		return;
	if (c >= 128 || !g_glyph_present[c])
		c = ' ';
	glyph = g_glyphs[c];
	color = convert_color_for_framebuffer(FRAMEBUFFER_DEFAULT_COLOR);
	for (row = 0; row < GLYPH_HEIGHT; row++)
		for (col = 0; col < GLYPH_WIDTH; col++)
			if (glyph[row] & (1 << (GLYPH_WIDTH - 1 - col)))
				framebuffer_put_pixel(g_framebuffer.cursor_x + col,
						g_framebuffer.cursor_y + row, color);
	g_framebuffer.cursor_x += GLYPH_WIDTH;
	if (g_framebuffer.cursor_x + GLYPH_WIDTH > framebuffer_width())
	{
		g_framebuffer.cursor_x = 0;
		g_framebuffer.cursor_y += GLYPH_HEIGHT;
	}
}

void
	framebuffer_draw_string(const char *str)
{
	unsigned start_x;

	start_x = g_framebuffer.cursor_x;
	while (*str)
	{
		if (*str == '\n')
		{
			g_framebuffer.cursor_x = start_x;
			g_framebuffer.cursor_y += GLYPH_HEIGHT;
		}
		else
			framebuffer_draw_char(*str);
		str++;
	}
}