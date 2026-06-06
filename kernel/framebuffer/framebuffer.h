#ifndef FRAMEBUFFER_H
# define FRAMEBUFFER_H

# include "../multiboot2.h"
# include <stdint.h>

/*  Convert color to framebuffer format  */
uint32_t
	convert_color_for_framebuffer(struct multiboot_tag_framebuffer *tagfb,
							uint32_t rgb);

/*  Write color pixel at coordinates (assume 32 bits per pixel with the boot.s file) */
void
	framebuffer_put_pixel(	struct multiboot_tag_framebuffer *tagfb,
							unsigned x, unsigned y,
							uint32_t color);

#endif