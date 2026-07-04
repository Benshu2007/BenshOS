#include "vga.h"

static void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a" (value), "Nd" (port));
}

uint8_t vga_entry_color (enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

uint16_t vga_entry (unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}


void vga_disable_cursor (void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, cursor_end);
}

void vga_set_cursor(int row, int col)
{
    uint16_t pos = row * 80 + col;

    outb(0x3D4, 0x0F);          // Low byte
    outb(0x3D5, pos & 0xFF);

    outb(0x3D4, 0x0E);          // High byte
    outb(0x3D5, (pos >> 8) & 0xFF);
}

void move_cursor(size_t* row, size_t *col, char d) {
    switch (d) {
        case 'l':
            if (*col == 0) {
                if (*row == 0) {
                    break;
                }
                (*row)--;
            } else 
                (*col)++;
            break;
        
        case 'r':
            if (*col == VGA_WIDTH) {
                if (*row == VGA_HEIGHT) {
                    break;
                }
                (*row)++;
                *col = 0;
            }
            (*col)++;
            break;

        case 'u':
            if (*row != 0)
                (*row)--;
            break;

        case 'd':
            if (*row != VGA_HEIGHT)
                (*row)++;
            break;
    }
}
