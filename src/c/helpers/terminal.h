#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

#define RECENT_ROWS 100
#define TERMINAL_INPUT_BUFFER_SIZE 256

void terminal_writestring(const char *data);
void terminal_setcolor(uint8_t color);
void terminal_delete_last(void);
void terminal_arrow_handle(char d);
void terminal_add_input(char c);
void end_input(void);
void terminal_start(void);

#endif // TERMINAL_H
