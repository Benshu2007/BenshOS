ENTRY(_start)

INPUT(
    build/rtc.o
    build/drivers/driver.o
    build/drivers/keyboard.o
    build/drivers/timer.o
    build/kernel_entry.o
    build/terminal.o
    build/common.o
    build/gdt.o
    build/idt.o
    build/vga.o
    build/pic.o
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
    /* Load kernel at 0x1000 from bootloader */
    . = 0x1000;

    /* Enforce 8K block padding with capitalized uppercase ALIGN syntax */
    .text BLOCK(8K) : ALIGN(8K)
    {
        *(.multiboot)
        *(.text)
        *(.text.*)      /* Captures GCC compiler sub-functions safely */
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)    /* Captures all local string constants safely */
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
        *(.data.*)      /* Captures all initialized global sub-variables safely */
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)       /* Captures all uninitialized memory spaces safely */
        _bss_end = .;
    }
}
