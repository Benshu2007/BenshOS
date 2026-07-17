#include "terminal.h"
#include "vga.h"
#include "common.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static size_t terminal_viewport_top;
static size_t terminal_row;
static size_t terminal_col;
static uint8_t terminal_color;
static uint16_t* terminal_viewport_buffer;
static uint16_t terminal_buffer[VGA_WIDTH * RECENT_ROWS];

static size_t get_row_written_length(size_t y) {
    for (int x = (int)VGA_WIDTH - 1; x >= 0; x--) {
        size_t index = y * VGA_WIDTH + (size_t)x;
        char c = (char)(terminal_buffer[index] & 0xFF);
        if (c != ' ') {
            return (size_t)(x + 1);
        }
    }
    return 0;
}

static void shift_history_up(void) {
    for (size_t y = 1; y < RECENT_ROWS; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_WIDTH + x];
        }
    }

    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(RECENT_ROWS - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
}

static void terminal_refresh_screen(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            size_t history_index = (terminal_viewport_top + y) * VGA_WIDTH + x;
            size_t screen_index = y * VGA_WIDTH + x;
            terminal_viewport_buffer[screen_index] = terminal_buffer[history_index];
        }
    }
}

static void increase_row(void) {
    terminal_row++;
    
    if (terminal_row >= VGA_HEIGHT) {
        terminal_viewport_top = terminal_row - (VGA_HEIGHT - 1);
    }

    if (terminal_row >= RECENT_ROWS) {
        shift_history_up();
        terminal_row = RECENT_ROWS - 1;
        terminal_viewport_top = RECENT_ROWS - VGA_HEIGHT;
    }    
}

static void increase_col(void) {
    if (++terminal_col >= VGA_WIDTH) {
        increase_row();
        terminal_col = 0;
    }
}

static void decrease_row(void) {
    if (terminal_row > 0) {
        terminal_row--;
        
        if (terminal_row < terminal_viewport_top) {
            terminal_viewport_top = terminal_row;
        }        
    }
}

static void decrease_col(void) {
    if (terminal_col > 0) {
        terminal_col--;
    } else {
        if (terminal_row > 0) {
            decrease_row();
            terminal_col = get_row_written_length(terminal_row);
        }
    }
}

static bool is_escape_sequence(char c) {
    return c == '\n' || c == '\r' || c == '\t' || c == '\b'; 
}

static void terminal_flush(void) {
    terminal_refresh_screen();
    vga_set_cursor(terminal_col,
        terminal_row - terminal_viewport_top);
}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

static void terminal_putchar_raw(char c) {    
    if (!is_escape_sequence(c)) {
        terminal_putentryat(c, terminal_color, terminal_col, terminal_row);
        increase_col();
    } else if (c == '\n') {
        increase_row();
        terminal_col = 0;
    }
}

static void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar_raw(data[i]);
    }
}




// --- PUBLIC TERMINAL INTERFACE ---
void terminal_initialize(void) {
    terminal_viewport_buffer = (uint16_t*)VGA_MEMORY;
    terminal_viewport_top = 0;
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

    for (size_t y = 0; y < RECENT_ROWS; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
    
    terminal_refresh_screen();
    vga_enable_cursor(14, 15);
    terminal_flush();
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));

    terminal_flush();
}

void terminal_readchar(void) {
    uint8_t ch = inb(0x60);
    terminal_putchar(ch);
}

void terminal_arrow_handle(char d) {
    size_t line_limit = 0;

    switch (d) {
        case 'l':
            if (terminal_col == 0 && terminal_row > 0) {
                decrease_row();
                terminal_col = get_row_written_length(terminal_row) - 1;
            } else if (terminal_col > 0) {
                line_limit = get_row_written_length(terminal_row);
                if (terminal_col > line_limit) {
                    terminal_col = line_limit - 1;
                } else {
                    terminal_col--;
                }
            }
            break;
        
        case 'r':
            line_limit = get_row_written_length(terminal_row);
            if (terminal_col < line_limit) {
                terminal_col++;
            } else if (terminal_col == line_limit && terminal_row + 1 < RECENT_ROWS) {
                if (get_row_written_length(terminal_row + 1) > 0) {
                    increase_row();
                    terminal_col = 0;
                }
            }
            break;

        case 'u':
            if (terminal_row > 0) {
                decrease_row();
                line_limit = get_row_written_length(terminal_row);
                if (line_limit == 0) {
                    terminal_col = 0;
                } else if (terminal_col > line_limit) {
                    terminal_col = line_limit - 1;
                }
            }
            break;

        case 'd':
            if (terminal_row + 1 < RECENT_ROWS) {
                line_limit = get_row_written_length(terminal_row + 1);
                if (line_limit > 0) {
                    increase_row();
                    if (terminal_col > line_limit) {
                        terminal_col = line_limit - 1;
                    }
                } else {
                    terminal_col = get_row_written_length(terminal_row) - 1;
                }
            }
            break;
    }

    terminal_flush();
}

void terminal_delete_last(void) {
    if (terminal_row == 0 && terminal_col == 0)
        return;

    decrease_col();

    terminal_putentryat(' ', terminal_color, terminal_col, terminal_row);
    terminal_flush();
}

void terminal_putchar(char c) {
    terminal_putchar_raw(c);
    terminal_flush();
}
