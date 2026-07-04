#include "terminal.h"

void terminal_initialize (void) {
    terminal_buffer = (uint16_t*)VGA_MEMORY;
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    vga_enable_cursor(14, 15);
    vga_set_cursor(terminal_row, terminal_col);
}

void terminal_setcolor (uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat (char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar (char c) {
    if (c == '\n') {
        terminal_col = 0;
        ++terminal_row;

        if (terminal_row >= VGA_HEIGHT)
            terminal_row = 0;

        vga_set_cursor(terminal_row, terminal_col);
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_col, terminal_row);

    if (++terminal_col == VGA_WIDTH) {
        terminal_col = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }

    vga_set_cursor(terminal_row, terminal_col);
}

void terminal_write (const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}
