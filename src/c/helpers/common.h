#include <stddef.h>
#include <stdint.h>

#ifndef SRC_COMMON_H
#define SRC_COMMON_H

size_t strlen (const char* str);
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

#endif //SRC_COMMON_H
