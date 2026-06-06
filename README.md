# KFS-1-Kernel-From-Scratch-

## Prerequisites

To build and run the kernel, you need a Linux environment with the following tools installed:

- `make`
- `gcc`
- `as`
- `ld`
- `grub-mkrescue`
- `xorriso`
- `qemu-system-i386`

You can install the required packages with:

```bash
sudo apt update
sudo apt install build-essential binutils grub-pc-bin grub-common xorriso qemu-system-x86
```

## Installation and running

```bash
  git clone https://github.com/TheEternalice/KFS-1-Kernel-From-Scratch.git
```
```bash
  make iso
```
```bash
   qemu-system-i386 -cdrom ./target/kfs.iso
```
