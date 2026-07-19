#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>

size_t strlen (const char* str);
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void io_wait(void);
void* memset(void* dest, int value, size_t count);


#endif //COMMON_H
