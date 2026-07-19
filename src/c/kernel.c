#include "drivers/driver/driver.h"
#include "drivers/keyboard/keyboard.h"
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
    terminal_log("GDT Successfully initialized!");
    idt_init();
    terminal_log("IDT Successfully initialized!");
    pic_init();
    terminal_log("PIC Successfully initialized!");


    if (!driver_register(&keyboard_driver, 1)) {
        terminal_log("Error registring keyboard driver!");
    } else 
        terminal_log("Keyboard driver Successfully registerd!");

    terminal_log("Driver interface established. Enabling STI master flag.");
    __asm__ volatile("sti"); 

    while (true) {
        __asm__ volatile("hlt"); // Rest the CPU cycles smoothly
    }
}

