#include "./framebuffer/framebuffer.h"
#include "./framebuffer/screen.h"
#include "./keyboard/keyboard.h"
#include "./multiboot/multiboot_info.h"

void kernel_main(uint32_t magic, uint32_t multiboot_info_addr)
{
	if (!multiboot_info_is_valid(magic, multiboot_info_addr))
		return;
	if (!multiboot_info_init_framebuffer(multiboot_info_addr))
		return;

	framebuffer_clear(GRAPHIC_COLOR_BLACK);
	keyboard_screen_init();
	multiboot_info_print(multiboot_info_addr);
	keyboard_write_loop();
}
