#include "graphic.h"

static int xpos;
static int ypos;
static volatile unsigned char *video;

void	cls(void)
{
	int i;

	video = (unsigned char *) VIDEO;

	for (i = 0; i < COLUMNS * LINES * 2; i++)
		*(video + i) = 0;

	xpos = 0;
	ypos = 0;
}

void	putchar(int c)
{
	if (c == '\n' || c == '\r')
	{
		newline:
		xpos = 0;
		ypos++;
		if (ypos >= LINES)
			ypos = 0;
		return;
	}

	*(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
	*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

	xpos++;
	if (xpos >= COLUMNS)
		goto newline;
}
