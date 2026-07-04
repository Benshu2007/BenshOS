#pragma once
#ifndef SRC_VGA_H
#define SRC_VGA_H

#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

uint8_t vga_entry_color (enum vga_color fg, enum vga_color bg);
uint16_t vga_entry (unsigned char uc, uint8_t color);
void vga_disable_cursor (void);
void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void vga_set_cursor(int row, int col);
// d==  'l' - left
//      'r' - right
//      'u' - up
//      'd' - down
void vga_move_cursor(size_t* row, size_t* col, char d);


#endif //SRC_VGA_H
