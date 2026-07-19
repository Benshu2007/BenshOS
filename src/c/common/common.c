#include "common.h"

size_t kstrlen (const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a" (value), "Nd" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));

    return value;
}

void io_wait(void)
{
    outb(0x80, 0);
}

void* kmemset(void* dest, int value, size_t count) {
    unsigned char* ptr = dest;

    for (size_t i = 0; i < count; i++) {
        ptr[i] = (unsigned char)value;
    }

    return dest;
}
