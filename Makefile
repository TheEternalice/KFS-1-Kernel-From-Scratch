C_SRCS			=	kernel/kernel.c kernel/framebuffer/framebuffer.c\
					kernel/graphic/graphic.c kernel/libc/libc.c
C_OBJS			=	$(C_SRCS:%.c=$(OBJ_DIR)/%.o)

ASM_SRCS		=	boot/boot.s
ASM_OBJS		=	$(ASM_SRCS:%.s=$(OBJ_DIR)/%.o)

LINKER_SCRIPT	=	linker/linker.ld

CC				=	gcc

AS				=	as

LD				=	ld

TARGET_DIR		=	target
OBJ_DIR			=	$(TARGET_DIR)/objs
ISO				=	$(TARGET_DIR)/kfs.iso
ISODIR			=	$(TARGET_DIR)/isodir
KERNEL			=	$(ISODIR)/boot/kernel.elf

CFLAGS			=	-m32 -ffreestanding -Wall -Wextra -Werror -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs

ASFLAGS			=	--32

LDFLAGS			=	-m elf_i386 -T $(LINKER_SCRIPT) -nostdlib -nodefaultlibs

RM				=	rm -f

NAME			=	kfs1

OBJS			=	$(ASM_OBJS) $(C_OBJS)

all: $(KERNEL)

$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJS) $(LINKER_SCRIPT)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

iso: $(ISO)

$(ISO): $(KERNEL) grub/grub.cfg
	@mkdir -p $(ISODIR)/boot/grub
	cp grub/grub.cfg $(ISODIR)/boot/grub/
	grub-mkrescue -o $(ISO) $(ISODIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(TARGET_DIR)

re: fclean all

.PHONY: all iso clean fclean re
