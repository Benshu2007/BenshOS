#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

#define RECENT_ROWS 100

void terminal_initialize(void);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color);
void terminal_readchar(void);
void terminal_putchar(char c);
void terminal_delete_last(void);
void terminal_arrow_handle(char d);

#endif //TERMINAL_H
