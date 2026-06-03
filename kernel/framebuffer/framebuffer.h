#ifndef FRAMEBUFFER_H
# define FRAMEBUFFER_H

# include "../multiboot2.h"
# include <stdint.h>

/*  Chose the color to use for each framebuffer type
	Also return encoded value in native framebuffer format  */
multiboot_uint32_t
	framebuffer_get_color(struct multiboot_tag_framebuffer *tagfb);

/*  Write color pixel at coordonate   */
void
	framebuffer_put_pixel(	struct multiboot_tag_framebuffer *tagfb,
							unsigned x, unsigned y,
							multiboot_uint32_t color);

#endif