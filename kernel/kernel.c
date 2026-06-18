#include "./framebuffer/framebuffer.h"
#include "./keyboard/keyboard.h"
#include "./multiboot/multiboot_info.h"

void kernel_main(uint32_t magic, uint32_t multiboot_info_addr)
{
	if (!multiboot_info_is_valid(magic, multiboot_info_addr))
		return;
	if (!multiboot_info_init_framebuffer(multiboot_info_addr))
		return;

	framebuffer_clear(GRAPHIC_COLOR_BLACK);
	framebuffer_set_cursor(0, 0);
	multiboot_info_print(multiboot_info_addr);
	keyboard_write_loop(0, g_framebuffer.cursor_y + FRAMEBUFFER_CHAR_HEIGHT);
}
