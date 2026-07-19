#include "drivers/driver/driver.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/timer/timer.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "pic/pic.h"
#include "rtc/rtc.h"
#include "terminal/terminal.h"
#include <stdbool.h>

#if defined(__linux__)
#error                                                                         \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i686__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void my_terminal_keyboard_callback(KeyboardEvent ev) {
  // 1. Only act when a human presses a key down (ignore release/break codes)
  if (ev.press) {

    // 2. Optional: Intercept a control hotkey combination (e.g., Ctrl + C)
    if (ev.ctrl && ev.code == KEY_C) {
      terminal_log("\n[Caught Ctrl+C!]\n");
      return;
    }

    // 3. Fallback: If it's a standard letter/number, translate and print
    // (Note: ensure keyboard_translate_event_to_ascii is either public or
    // accessible)
    char ascii = keyboard_translate(ev);
    if (ascii != '\0') {
      const char str[2] = {ascii, '\0'};
      terminal_log(str);
    }
  }
}

void kregister_driver(uint8_t irq_line, driver_t *driver,
                      const char *driver_name) {
  if (!driver_register(driver, irq_line)) {
    terminal_log("Error registring %s driver!", driver_name);
  } else {
    terminal_log("%s driver Successfully registerd!", driver_name);
  }
}

void drivers_init(void) {
  kregister_driver(0, &timer_driver, "Timer");
  kregister_driver(1, &keyboard_driver, "Keyboard");

  keyboard_set_callback(my_terminal_keyboard_callback);
}

void kernel_init(void) {
  terminal_start();
  gdt_init();
  terminal_log("GDT Successfully initialized!");
  idt_init();
  terminal_log("IDT Successfully initialized!");
  pic_init();
  terminal_log("PIC Successfully initialized!");
  drivers_init();
  terminal_log("Drivers Successfully initialized!");

  __asm__ volatile("sti");
}

void kernel_main(void) {
  kernel_init();

  rtc_print_current_time(3);

  while (true) {
    __asm__ volatile("hlt"); // Rest the CPU cycles smoothly
  }
}
