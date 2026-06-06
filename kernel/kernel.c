#include "multiboot2.h"
#include "./graphic/graphic.h"
#include "./libc/libc.h"
#include "./framebuffer/framebuffer.h"

void kernel_main(uint32_t magic, uint32_t multiboot_info_addr)
{
	struct multiboot_tag *tag;
	unsigned size;

	cls();

	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
	{
		printk("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}

	if (multiboot_info_addr & 7)
	{
		printk("Unaligned mbi: 0x%x\n", multiboot_info_addr);
		return;
	}

	size = *(unsigned *)multiboot_info_addr;
	printk("Announced mbi size 0x%x\n", size);
	for (tag = (struct multiboot_tag *) (multiboot_info_addr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										+ ((tag->size + 7) & ~7)))
	{
		printk ("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
		switch (tag->type)
		{
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				printk("Command line = %s\n",
						((struct multiboot_tag_string *) tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				printk("Boot loader name = %s\n",
						((struct multiboot_tag_string *) tag)->string);
				break;
			case MULTIBOOT_TAG_TYPE_MODULE:
				printk("Module at 0x%x-0x%x. Command line %s\n",
						((struct multiboot_tag_module *) tag)->mod_start,
						((struct multiboot_tag_module *) tag)->mod_end,
						((struct multiboot_tag_module *) tag)->cmdline);
				break;
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				printk("mem_lower = %uKB, mem_upper = %uKB\n",
						((struct multiboot_tag_basic_meminfo *) tag)->mem_lower,
						((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);
				break;
			case MULTIBOOT_TAG_TYPE_BOOTDEV:
				printk("Boot device 0x%x,%u,%u\n",
						((struct multiboot_tag_bootdev *) tag)->biosdev,
						((struct multiboot_tag_bootdev *) tag)->slice,
						((struct multiboot_tag_bootdev *) tag)->part);
				break;
			case MULTIBOOT_TAG_TYPE_MMAP:
			{
				multiboot_memory_map_t *mmap;

				printk("mmap\n");
		
				for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
					(multiboot_uint8_t *) mmap 
					< (multiboot_uint8_t *) tag + tag->size;
					mmap = (multiboot_memory_map_t *) 
					((uint32_t) mmap
						+ ((struct multiboot_tag_mmap *) tag)->entry_size))
				printk(" base_addr = 0x%x%x,"
						" length = 0x%x%x, type = 0x%x\n",
						(unsigned) (mmap->addr >> 32),
						(unsigned) (mmap->addr & 0xffffffff),
						(unsigned) (mmap->len >> 32),
						(unsigned) (mmap->len & 0xffffffff),
						(unsigned) mmap->type);
				break;
			}
			
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{
				struct multiboot_tag_framebuffer *tagfb
				= (struct multiboot_tag_framebuffer *) tag;
				multiboot_uint32_t color = framebuffer_get_color(tagfb);
				unsigned i;
				
				for (i = 0;
					i < tagfb->common.framebuffer_width
					&& i < tagfb->common.framebuffer_height;
					i++)
				framebuffer_put_pixel(tagfb, i, i, color);
				break;
			}
		}
	}
	tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
									+ ((tag->size + 7) & ~7));
	printk("Total mbi size 0x%x\n", (unsigned) tag - multiboot_info_addr);
}
