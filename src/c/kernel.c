#include "gdt/gdt.h"
#include "idt/idt.h"
#include "pic/pic.h"
#include "terminal/terminal.h"
#include <stdbool.h>

#if defined(__linux__)
#error                                                                         \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i686__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void) {
    terminal_start();
    gdt_init();
    idt_init();
    pic_init();

    while (true) {
        __asm__ volatile("hlt"); // CPU sleeps efficiently until an IRQ wakes it
    }
}

