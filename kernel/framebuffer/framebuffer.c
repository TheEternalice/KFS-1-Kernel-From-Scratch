#include "framebuffer.h"

multiboot_uint32_t
	framebuffer_get_color(struct multiboot_tag_framebuffer *tagfb)
{
	switch (tagfb->common.framebuffer_type)
	{
		case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
		{
			unsigned best_distance, distance;
			struct multiboot_color *palette;
			multiboot_uint32_t color;
			unsigned i;

			palette = tagfb->framebuffer_palette;
			color = 0;
			best_distance = 4 * 256 * 256;

			for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++)
			{
				distance = (0xff - palette[i].blue)  * (0xff - palette[i].blue)
						+ palette[i].red   * palette[i].red
						+ palette[i].green * palette[i].green;
				if (distance < best_distance)
				{
					color = i;
					best_distance = distance;
				}
			}
			return color;
		}

		case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
			return ((1 << tagfb->framebuffer_blue_mask_size) - 1)
					<< tagfb->framebuffer_blue_field_position;

		case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			return '\\' | 0x0100;

		default:
			return 0xffffffff;
	}
}

void
	framebuffer_put_pixel(struct multiboot_tag_framebuffer *tagfb,
							unsigned x, unsigned y,
							multiboot_uint32_t color)
{
	void *fb = (void *) (uint32_t) tagfb->common.framebuffer_addr;
	unsigned pitch = tagfb->common.framebuffer_pitch;

	switch (tagfb->common.framebuffer_bpp)
	{
		case 8:
		{
			multiboot_uint8_t *pixel = fb + pitch * y + x;
			*pixel = (multiboot_uint8_t) color;
			break;
		}
		case 15:
		case 16:
		{
			multiboot_uint16_t *pixel = fb + pitch * y + 2 * x;
			*pixel = (multiboot_uint16_t) color;
			break;
		}
		case 24:
		{
			multiboot_uint32_t *pixel = fb + pitch * y + 3 * x;
			*pixel = (multiboot_uint32_t) color;
			break;
		}
		case 32:
		{
			multiboot_uint32_t *pixel = fb + pitch * y + 4 * x;
			*pixel = (multiboot_uint32_t) color;
			break;
		}
	}
}
