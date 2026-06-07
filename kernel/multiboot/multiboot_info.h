#ifndef MULTIBOOT_INFO_H
# define MULTIBOOT_INFO_H

# include "../multiboot2.h"
# include <stdint.h>
# include "../framebuffer/framebuffer.h"
# include "../libc/libc.h"

int		multiboot_info_is_valid(uint32_t magic, uint32_t multiboot_info_addr);
int		multiboot_info_init_framebuffer(uint32_t multiboot_info_addr);
void	multiboot_info_print(uint32_t multiboot_info_addr);

#endif
