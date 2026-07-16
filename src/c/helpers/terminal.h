#pragma once
#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include <stddef.h>
#include "vga.h"
#include "common.h"

#define RECENT_ROWS 100

static size_t terminal_viewport_top;
static size_t terminal_row;
static size_t terminal_col;
static uint8_t terminal_color;
static uint16_t* terminal_viewport_buffer;
static uint16_t terminal_buffer[VGA_WIDTH * RECENT_ROWS];



void terminal_initialize ();
void terminal_setcolor (uint8_t color);
void terminal_putentryat (char c, uint8_t color, size_t x, size_t y);
void terminal_putchar (char c);
void terminal_write (const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_readchar();
void terminal_arrow_handle(char d);
void terminal_delete_last();
void terminal_refresh_screen();


void increase_row();
void increase_col();
void decrease_row();
void decrease_col();
void shift_history_up();
bool is_coordinate_empty(size_t x, size_t y);
bool is_escape_sequence(char c);

#endif //TERMINAL_H
