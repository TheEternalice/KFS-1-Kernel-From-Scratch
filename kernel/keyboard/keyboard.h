#ifndef KEYBOARD_H
# define KEYBOARD_H

# include <stdint.h>

# define KEYBOARD_QUEUE_SIZE	128

# define KEYBOARD_DATA_PORT		0x60
# define KEYBOARD_STATUS_PORT	0x64
# define KEYBOARD_OUTPUT_FULL	0x01

# define KEYBOARD_SCANCODE_RELEASE	0x80
# define KEYBOARD_SCANCODE_LSHIFT	0x2a
# define KEYBOARD_SCANCODE_RSHIFT	0x36
# define KEYBOARD_SCANCODE_EXTENDED	0xe0

# define KEYBOARD_SCANCODE_LEFT		0x4b
# define KEYBOARD_SCANCODE_RIGHT	0x4d
# define KEYBOARD_SCANCODE_UP		0x48
# define KEYBOARD_SCANCODE_DOWN		0x50
# define KEYBOARD_SCANCODE_DELETE	0x53

# define KEYBOARD_SCANCODE_1		0x02
# define KEYBOARD_SCANCODE_2		0x03
# define KEYBOARD_SCANCODE_3		0x04
# define KEYBOARD_SCANCODE_4		0x05
# define KEYBOARD_SCANCODE_5		0x06
# define KEYBOARD_SCANCODE_6		0x07
# define KEYBOARD_SCANCODE_7		0x08
# define KEYBOARD_SCANCODE_8		0x09
# define KEYBOARD_SCANCODE_9		0x0a
# define KEYBOARD_SCANCODE_0		0x0b
# define KEYBOARD_SCANCODE_BACKSPACE	0x0e
# define KEYBOARD_SCANCODE_Q		0x10
# define KEYBOARD_SCANCODE_W		0x11
# define KEYBOARD_SCANCODE_E		0x12
# define KEYBOARD_SCANCODE_R		0x13
# define KEYBOARD_SCANCODE_T		0x14
# define KEYBOARD_SCANCODE_Y		0x15
# define KEYBOARD_SCANCODE_U		0x16
# define KEYBOARD_SCANCODE_I		0x17
# define KEYBOARD_SCANCODE_O		0x18
# define KEYBOARD_SCANCODE_P		0x19
# define KEYBOARD_SCANCODE_ENTER	0x1c
# define KEYBOARD_SCANCODE_A		0x1e
# define KEYBOARD_SCANCODE_S		0x1f
# define KEYBOARD_SCANCODE_D		0x20
# define KEYBOARD_SCANCODE_F		0x21
# define KEYBOARD_SCANCODE_G		0x22
# define KEYBOARD_SCANCODE_H		0x23
# define KEYBOARD_SCANCODE_J		0x24
# define KEYBOARD_SCANCODE_K		0x25
# define KEYBOARD_SCANCODE_L		0x26
# define KEYBOARD_SCANCODE_Z		0x2c
# define KEYBOARD_SCANCODE_X		0x2d
# define KEYBOARD_SCANCODE_C		0x2e
# define KEYBOARD_SCANCODE_V		0x2f
# define KEYBOARD_SCANCODE_B		0x30
# define KEYBOARD_SCANCODE_N		0x31
# define KEYBOARD_SCANCODE_M		0x32
# define KEYBOARD_SCANCODE_SPACE	0x39

# define KEYBOARD_SCANCODE_MAP_SIZE	128

# define KEYBOARD_KEY_LEFT		0x0100
# define KEYBOARD_KEY_RIGHT		0x0101
# define KEYBOARD_KEY_UP		0x0102
# define KEYBOARD_KEY_DOWN		0x0103
# define KEYBOARD_KEY_DELETE	0x0104

typedef uint16_t	t_keyboard_key;

typedef struct s_keyboard_queue
{
	t_keyboard_key buffer[KEYBOARD_QUEUE_SIZE];
	unsigned read_index;
	unsigned write_index;
	unsigned count;
}	t_keyboard_queue;

# include "../framebuffer/screen.h"

void	keyboard_init(void);
void	keyboard_poll(void);
int		keyboard_pop(t_keyboard_key *key);
void	keyboard_write_loop(unsigned x, unsigned y);

#endif
