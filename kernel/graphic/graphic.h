#ifndef GRAPHIC_H
# define GRAPHIC_H

# define COLUMNS		80
# define LINES			24
# define ATTRIBUTE		7
# define VIDEO			0xB8000

# define GRAPHIC_COLOR_BLACK	0x000000
# define GRAPHIC_COLOR_WHITE	0xFFFFFF
# define GRAPHIC_COLOR_BLUE		0x0000FF
# define GRAPHIC_COLOR_GREEN	0x00FF00
# define GRAPHIC_COLOR_RED		0xFF0000

void	cls(void);
void	putchar(int c);

#endif