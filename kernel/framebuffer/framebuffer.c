#include "framebuffer.h"

t_framebuffer g_framebuffer;

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
	framebuffer_put_pixel(unsigned x, unsigned y, uint32_t color)
{
	void *fb;
	unsigned pitch;
	uint32_t *pixel;

	if (!g_framebuffer.tag)
		return;
	fb = (void *) (uint32_t) g_framebuffer.tag->common.framebuffer_addr;
	pitch = g_framebuffer.tag->common.framebuffer_pitch;
	pixel = fb + pitch * y + FRAMEBUFFER_BYTES_PER_PIXEL * x;
	*pixel = color;
}

void
	framebuffer_init(struct multiboot_tag_framebuffer *tagfb)
{
	g_framebuffer.tag = tagfb;
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
	framebuffer_fill_rect(unsigned x, unsigned y,
							unsigned width, unsigned height,
							uint32_t rgb)
{
	uint32_t color;
	unsigned current_x;
	unsigned current_y;

	if (!g_framebuffer.tag || width == 0 || height == 0)
		return;
	color = convert_color_for_framebuffer(rgb);
	current_y = y;
	while (current_y < y + height && current_y < framebuffer_height())
	{
		current_x = x;
		while (current_x < x + width && current_x < framebuffer_width())
		{
			framebuffer_put_pixel(current_x, current_y, color);
			current_x++;
		}
		current_y++;
	}
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
	y = 0;
	while (y < framebuffer_height())
	{
		x = 0;
		while (x < framebuffer_width())
		{
			framebuffer_put_pixel(x, y, color);
			x++;
		}
		y++;
	}
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

	if (!g_framebuffer.tag || offset == 0 || offset >= height)
		return;
	fb = (uint8_t *) (uint32_t) g_framebuffer.tag->common.framebuffer_addr;
	pitch = g_framebuffer.tag->common.framebuffer_pitch;
	current_y = y;
	while (current_y < y + height - offset)
	{
		dst = (uint32_t *) (fb + pitch * current_y);
		src = (uint32_t *) (fb + pitch * (current_y + offset));
		x = 0;
		while (x < framebuffer_width())
		{
			dst[x] = src[x];
			x++;
		}
		current_y++;
	}
	framebuffer_fill_rect(0, y + height - offset, framebuffer_width(), offset,
			rgb);
}

void
	framebuffer_draw_glyph(unsigned x, unsigned y,
							unsigned char c, uint32_t fg, uint32_t bg)
{
	const uint8_t *glyph;
	uint32_t foreground_color;
	uint32_t background_color;
	unsigned row;
	unsigned col;

	if (!g_framebuffer.tag)
		return;
	if (c >= 128 || !g_glyph_present[c])
		c = ' ';
	glyph = g_glyphs[c];
	foreground_color = convert_color_for_framebuffer(fg);
	background_color = convert_color_for_framebuffer(bg);
	row = 0;
	while (row < GLYPH_HEIGHT)
	{
		col = 0;
		while (col < GLYPH_WIDTH)
		{
			if (glyph[row] & (1 << (GLYPH_WIDTH - 1 - col)))
				framebuffer_put_pixel(x + col, y + row, foreground_color);
			else
				framebuffer_put_pixel(x + col, y + row, background_color);
			col++;
		}
		row++;
	}
}

void
	framebuffer_draw_glyph_overlay(unsigned x, unsigned y,
									unsigned char c, uint32_t fg)
{
	const uint8_t *glyph;
	uint32_t foreground_color;
	unsigned row;
	unsigned col;

	if (!g_framebuffer.tag)
		return;
	if (c >= 128 || !g_glyph_present[c])
		return;
	glyph = g_glyphs[c];
	foreground_color = convert_color_for_framebuffer(fg);
	row = 0;
	while (row < GLYPH_HEIGHT)
	{
		col = 0;
		while (col < GLYPH_WIDTH)
		{
			if (glyph[row] & (1 << (GLYPH_WIDTH - 1 - col)))
				framebuffer_put_pixel(x + col, y + row, foreground_color);
			col++;
		}
		row++;
	}
}
