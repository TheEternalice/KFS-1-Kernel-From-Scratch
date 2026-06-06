#include "framebuffer.h"

uint32_t
	convert_color_for_framebuffer(struct multiboot_tag_framebuffer *tagfb,
							uint32_t rgb)
{
	uint8_t red = (rgb >> 16) & 0xff;
	uint8_t green = (rgb >> 8) & 0xff;
	uint8_t blue = rgb & 0xff;

	switch (tagfb->common.framebuffer_type)
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

			red_mask = (1 << tagfb->framebuffer_red_mask_size) - 1;
			green_mask = (1 << tagfb->framebuffer_green_mask_size) - 1;
			blue_mask = (1 << tagfb->framebuffer_blue_mask_size) - 1;

			red_value = (red >> (8 - tagfb->framebuffer_red_mask_size))
						& red_mask;
			green_value = (green >> (8 - tagfb->framebuffer_green_mask_size))
						& green_mask;
			blue_value = (blue >> (8 - tagfb->framebuffer_blue_mask_size))
						& blue_mask;

			red_pixel = red_value << tagfb->framebuffer_red_field_position;
			green_pixel = green_value << tagfb->framebuffer_green_field_position;
			blue_pixel = blue_value << tagfb->framebuffer_blue_field_position;

			return red_pixel | green_pixel | blue_pixel;
		}

		default:
			printk("Unsupported framebuffer type: %u\n", tagfb->common.framebuffer_type);
			return 0;
	}
}

void
	framebuffer_put_pixel(struct multiboot_tag_framebuffer *tagfb,
							unsigned x, unsigned y,
							uint32_t color)
{
	void *fb = (void *) (uint32_t) tagfb->common.framebuffer_addr;
	unsigned pitch = tagfb->common.framebuffer_pitch;
	uint32_t *pixel = fb + pitch * y + 4 * x;
	*pixel = color;
}
