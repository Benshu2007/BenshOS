#!/bin/zsh

mkdir -p build
nasm -f bin src/asm/boot.asm -o build/boot.bin
i686-elf-gcc -g -ffreestanding -c src/c/kernel.c -o build/kernel.o
i686-elf-gcc -ffreestanding -c src/c/helpers/vga.c -o build/vga.o
i686-elf-gcc -ffreestanding -c src/c/terminal/terminal.c -o build/terminal.o
i686-elf-gcc -ffreestanding -c src/c/helpers/common.c -o build/common.o
i686-elf-gcc -ffreestanding -c src/c/helpers/bits.c -o build/bits.o
i686-elf-gcc -ffreestanding -c src/c/gdt/gdt.c -o build/gdt.o
i686-elf-gcc -ffreestanding -c src/c/pic/pic.c -o build/pic.o
i686-elf-gcc -ffreestanding -c src/c/idt/idt.c -o build/idt.o
i686-elf-gcc -ffreestanding -c src/c/idt/interrupts.c -o build/interrupts.o
i686-elf-gcc -ffreestanding -c src/c/helpers/keyboard.c -o build/keyboard.o
nasm -f elf32 src/asm/kernel_entry.asm -o build/kernel_entry.o
nasm -f elf32 src/asm/flush_gdt.asm -o build/flush_gdt.o
nasm -f elf32 src/asm/flush_idt.asm -o build/flush_idt.o
nasm -f elf32 src/asm/isr/isr.asm -o build/isr.o

i686-elf-ld -g -T linker.ld

dd if=/dev/zero of=build/os-image.bin bs=512 count=2880
dd if=build/boot.bin of=build/os-image.bin bs=512 count=1 conv=notrunc
dd if=build/kernel.bin of=build/os-image.bin bs=512 seek=1 conv=notrunc