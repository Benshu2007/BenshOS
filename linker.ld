ENTRY(_start)

INPUT(
    build/kernel_entry.o
    build/terminal.o
    build/common.o
    build/gdt.o
    build/idt.o
    build/vga.o
    build/pic.o
    build/keyboard.o
    build/kernel.o
    build/flush_gdt.o
    build/flush_idt.o
    build/isr.o
    build/interrupts.o
    build/bits.o
)

OUTPUT(build/kernel.bin)

SECTIONS
{
    /* We load kernel at 0x1000 from bootloader */
    . = 0x1000;
    .text : ALIGN(4K)
    {
        *(.multiboot)
        *(.text*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    }

    .data : ALIGN(4K)
    {
        *(.data*)
    }

    .bss : ALIGN(4K)
    {
        _bss_start = .;
        *(COMMON)
        *(.bss*)
        _bss_end = .;
    }
}