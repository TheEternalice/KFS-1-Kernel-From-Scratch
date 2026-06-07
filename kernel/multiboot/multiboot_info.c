#include "multiboot_info.h"

static struct multiboot_tag
	*first_tag(uint32_t multiboot_info_addr)
{
	return (struct multiboot_tag *) (multiboot_info_addr + 8);
}

static struct multiboot_tag
	*next_tag(struct multiboot_tag *tag)
{
	return (struct multiboot_tag *) ((multiboot_uint8_t *) tag
		+ ((tag->size + 7) & ~7));
}

int
	multiboot_info_is_valid(uint32_t magic, uint32_t multiboot_info_addr)
{
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		return 0;
	if (multiboot_info_addr & 7)
		return 0;
	return 1;
}

static struct multiboot_tag
	*find_tag(uint32_t multiboot_info_addr, uint32_t type)
{
	struct multiboot_tag *tag;

	for (tag = first_tag(multiboot_info_addr);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = next_tag(tag))
	{
		if (tag->type == type)
			return tag;
	}
	return 0;
}

int
	multiboot_info_init_framebuffer(uint32_t multiboot_info_addr)
{
	struct multiboot_tag *tag;

	tag = find_tag(multiboot_info_addr, MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
	if (!tag)
		return 0;
	framebuffer_init((struct multiboot_tag_framebuffer *) tag);
	return 1;
}

static const char
	*tag_type_name(uint32_t type)
{
	switch (type)
	{
		case MULTIBOOT_TAG_TYPE_CMDLINE:
			return "Command line";
		case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
			return "Boot loader";
		case MULTIBOOT_TAG_TYPE_MODULE:
			return "Module";
		case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
			return "Basic memory";
		case MULTIBOOT_TAG_TYPE_BOOTDEV:
			return "Boot device";
		case MULTIBOOT_TAG_TYPE_MMAP:
			return "Memory map";
		case MULTIBOOT_TAG_TYPE_VBE:
			return "VBE";
		case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			return "Framebuffer";
		case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
			return "ELF sections";
		case MULTIBOOT_TAG_TYPE_APM:
			return "APM";
		case MULTIBOOT_TAG_TYPE_EFI32:
			return "EFI 32-bit";
		case MULTIBOOT_TAG_TYPE_EFI64:
			return "EFI 64-bit";
		case MULTIBOOT_TAG_TYPE_SMBIOS:
			return "SMBIOS";
		case MULTIBOOT_TAG_TYPE_ACPI_OLD:
			return "ACPI old RSDP";
		case MULTIBOOT_TAG_TYPE_ACPI_NEW:
			return "ACPI new RSDP";
		case MULTIBOOT_TAG_TYPE_NETWORK:
			return "Network";
		case MULTIBOOT_TAG_TYPE_EFI_MMAP:
			return "EFI memory map";
		case MULTIBOOT_TAG_TYPE_EFI_BS:
			return "EFI boot services";
		case MULTIBOOT_TAG_TYPE_EFI32_IH:
			return "EFI 32-bit image handle";
		case MULTIBOOT_TAG_TYPE_EFI64_IH:
			return "EFI 64-bit image handle";
		case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
			return "Load base address";
		default:
			return "Unknown";
	}
}

static const char
	*memory_type_name(uint32_t type)
{
	switch (type)
	{
		case MULTIBOOT_MEMORY_AVAILABLE:
			return "available";
		case MULTIBOOT_MEMORY_RESERVED:
			return "reserved";
		case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
			return "ACPI reclaimable";
		case MULTIBOOT_MEMORY_NVS:
			return "NVS";
		case MULTIBOOT_MEMORY_BADRAM:
			return "bad RAM";
		default:
			return "unknown";
	}
}

static const char
	*framebuffer_type_name(uint32_t type)
{
	switch (type)
	{
		case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
			return "indexed palette";
		case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
			return "direct RGB";
		case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			return "EGA text";
		default:
			return "unknown";
	}
}

static void
	print_memory_map(struct multiboot_tag *tag)
{
	multiboot_memory_map_t *mmap;
	struct multiboot_tag_mmap *mmap_tag;

	mmap_tag = (struct multiboot_tag_mmap *) tag;
	printk(" Memory entries: entry size %u bytes, version %u\n",
			mmap_tag->entry_size, mmap_tag->entry_version);
	for (mmap = mmap_tag->entries;
		(multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size;
		mmap = (multiboot_memory_map_t *)
		((uint32_t) mmap + mmap_tag->entry_size))
		printk("  - base: 0x%x%08x, length: 0x%x%08x, type: %s (%u)\n",
				(unsigned) (mmap->addr >> 32),
				(unsigned) (mmap->addr & 0xffffffff),
				(unsigned) (mmap->len >> 32),
				(unsigned) (mmap->len & 0xffffffff),
				memory_type_name(mmap->type),
				(unsigned) mmap->type);
}

static void
	print_framebuffer(struct multiboot_tag *tag)
{
	struct multiboot_tag_framebuffer *fb;

	fb = (struct multiboot_tag_framebuffer *) tag;
	printk(" Resolution: %ux%u pixels\n",
			fb->common.framebuffer_width,
			fb->common.framebuffer_height);
	printk(" Pixel format: %s (%u), %u bits per pixel\n",
			framebuffer_type_name(fb->common.framebuffer_type),
			fb->common.framebuffer_type,
			fb->common.framebuffer_bpp);
	printk(" Pitch: %u bytes per line\n", fb->common.framebuffer_pitch);
	printk(" Address: 0x%x%08x\n",
			(unsigned) (fb->common.framebuffer_addr >> 32),
			(unsigned) (fb->common.framebuffer_addr & 0xffffffff));
}

static void
	print_tag(struct multiboot_tag *tag)
{
	printk("\nTag %s size %u bytes\n",
			tag_type_name(tag->type), tag->size);
	switch (tag->type)
	{
		case MULTIBOOT_TAG_TYPE_CMDLINE:
			printk(" Kernel command line %s\n",
					((struct multiboot_tag_string *) tag)->string);
			break;
		case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
			printk(" Boot loader name %s\n",
					((struct multiboot_tag_string *) tag)->string);
			break;
		case MULTIBOOT_TAG_TYPE_MODULE:
			printk(" Module range 0x%x - 0x%x\n",
					((struct multiboot_tag_module *) tag)->mod_start,
					((struct multiboot_tag_module *) tag)->mod_end);
			printk(" Module size %u bytes\n",
					((struct multiboot_tag_module *) tag)->mod_end
					- ((struct multiboot_tag_module *) tag)->mod_start);
			printk(" Module command line %s\n",
					((struct multiboot_tag_module *) tag)->cmdline);
			break;
		case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
			printk(" Lower memory %u KB\n",
					((struct multiboot_tag_basic_meminfo *) tag)->mem_lower);
			printk(" Upper memory %u KB\n",
					((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);
			break;
		case MULTIBOOT_TAG_TYPE_BOOTDEV:
			printk(" BIOS device 0x%x\n",
					((struct multiboot_tag_bootdev *) tag)->biosdev);
			printk(" Partition slice %u partition %u\n",
					((struct multiboot_tag_bootdev *) tag)->slice,
					((struct multiboot_tag_bootdev *) tag)->part);
			break;
		case MULTIBOOT_TAG_TYPE_MMAP:
			print_memory_map(tag);
			break;
		case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			print_framebuffer(tag);
			break;
		default:
			break;
	}
}

void
	multiboot_info_print(uint32_t multiboot_info_addr)
{
	struct multiboot_tag *tag;
	unsigned size;

	size = *(unsigned *) multiboot_info_addr;
	printk("Multiboot information size: %u bytes\n", size);
	for (tag = first_tag(multiboot_info_addr);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = next_tag(tag))
		print_tag(tag);
	tag = next_tag(tag);
	printk("\nParsed multiboot size %u bytes\n",
			(unsigned) tag - multiboot_info_addr);
}
