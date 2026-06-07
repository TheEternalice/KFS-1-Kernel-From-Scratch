#ifndef LIBC_H
# define LIBC_H

# include "../framebuffer/framebuffer.h"
# include <stdint.h>

void	itoa_base(char *buf, int base, int d);
void	printk(const char *format, ...);

#endif