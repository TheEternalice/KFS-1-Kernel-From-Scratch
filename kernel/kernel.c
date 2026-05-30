void kernel_main(uint32_t magic, uint32_t multiboot_info_addr)
{
	for (;;)
		__asm__ volatile ("hlt");
}
