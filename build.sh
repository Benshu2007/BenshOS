#!/bin/zsh

mkdir -p build
nasm -f bin src/asm/boot.asm -o build/boot.bin
i686-elf-gcc -ffreestanding -c src/c/kernel.c -o build/kernel.o
i686-elf-gcc -ffreestanding -c src/c/helpers/vga.c -o build/vga.o
i686-elf-gcc -ffreestanding -c src/c/helpers/terminal.c -o build/terminal.o
i686-elf-gcc -ffreestanding -c src/c/helpers/common.c -o build/common.o
nasm -f elf32 src/asm/kernel_entry.asm -o build/kernel_entry.o

i686-elf-ld -T linker.ld

dd if=/dev/zero of=build/os-image.bin bs=512 count=2880
dd if=build/boot.bin of=build/os-image.bin bs=512 count=1 conv=notrunc
dd if=build/kernel.bin of=build/os-image.bin bs=512 seek=1 conv=notrunc