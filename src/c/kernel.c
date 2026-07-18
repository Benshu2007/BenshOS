#include "gdt/gdt.h"
#include "helpers/terminal.h"
#include <stdbool.h>

#if defined(__linux__)
#error                                                                         \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i686__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void) {
  gdt_init();
  terminal_start();

  terminal_log("GDT LOADED");

  while (true);
}
