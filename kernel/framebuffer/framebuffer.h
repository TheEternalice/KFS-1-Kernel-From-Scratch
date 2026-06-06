#ifndef FRAMEBUFFER_H
# define FRAMEBUFFER_H

# include "../multiboot2.h"
# include <stdint.h>

/*  Chose the color to use for each framebuffer type
	Also return encoded value in native framebuffer format  */
uint32_t
	convert_color_for_framebuffer(struct multiboot_tag_framebuffer *tagfb,
							uint32_t rgb);

/*  Write color pixel at coordonate   */
void
	framebuffer_put_pixel(	struct multiboot_tag_framebuffer *tagfb,
							unsigned x, unsigned y,
							uint32_t color);

#endif