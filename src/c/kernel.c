#include "helpers/terminal.h"
#include "helpers/keyboard.h"
#include <stdbool.h>

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i686__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main (void) {
	terminal_initialize ();

	terminal_writestring ("Hello World!\n");

	terminal_writestring ("This is terminal!");
	while (true) {
		KeyboardEvent ch = keyboard_input();
		if (ch.code != KEY_NONE)
			keyboard_handle_event(ch);
	}
}
