.set MULTIBOOT2_MAGIC, 0xe85250d6
.set MULTIBOOT2_ARCHITECTURE,  0

.set MULTIBOOT2_ALIGNMENT, 8

.set MULTIBOOT2_TAG_TYPE_UNUSED, 0
.set MULTIBOOT2_TAG_SIZE_FRAMEBUFFER, 20

.set MULTIBOOT2_TAG_TYPE_END, 0
.set MULTIBOOT2_TAG_TYPE_FRAMEBUFFER, 5

.set MULTIBOOT2_FRAMEBUFFER_WIDTH, 1920
.set MULTIBOOT2_FRAMEBUFFER_HEIGHT, 1080
.set MULTIBOOT2_FRAMEBUFFER_BPP, 32

.set MULTIBOOT2_LAST_TAG_SIZE, 8

.section .multiboot
.align MULTIBOOT2_ALIGNMENT
multiboot2_header_start:
.long MULTIBOOT2_MAGIC
.long MULTIBOOT2_ARCHITECTURE
.long multiboot2_header_end - multiboot2_header_start
.long -(MULTIBOOT2_MAGIC + MULTIBOOT2_ARCHITECTURE + (multiboot2_header_end - multiboot2_header_start))

# Tags
# Tag Framebuffer
.align MULTIBOOT2_ALIGNMENT
.short MULTIBOOT2_TAG_TYPE_FRAMEBUFFER
.short MULTIBOOT2_TAG_TYPE_UNUSED
.long MULTIBOOT2_TAG_SIZE_FRAMEBUFFER
.long MULTIBOOT2_FRAMEBUFFER_WIDTH
.long MULTIBOOT2_FRAMEBUFFER_HEIGHT
.long MULTIBOOT2_FRAMEBUFFER_BPP

# Tag de fin
.align MULTIBOOT2_ALIGNMENT
.short MULTIBOOT2_TAG_TYPE_END
.short MULTIBOOT2_TAG_TYPE_UNUSED
.long MULTIBOOT2_LAST_TAG_SIZE
multiboot2_header_end:

.set ALIGN_STACK, 16
.set STACK_SIZE, 16384

# Initialisation de la stack
.section .bss
.align ALIGN_STACK
stack_bottom:
.skip STACK_SIZE
stack_top:

.section .text
.global _start
_start:
	mov $stack_top, %esp
	push %ebx
	push %eax
	call kernel_main

hang:
	cli
	hlt
	jmp hang
