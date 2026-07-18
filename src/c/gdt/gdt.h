#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_ENTRIES_LEN 5

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint32_t offset;
} GDTR;

typedef struct __attribute__((packed)) {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access_byte;
  uint8_t flags_limit_high; // 4 low - limit; 4 high - flags
  uint8_t base_high;
} GDT_ENTRY;

/*
Bassicaly:
newentry() for entry 0 - null
newentry() for entry 1 - kernel code
newentry() for entry 2 - kernel data
newentry() for entry 3 - user code
newentry() for entry 4 - user data

then load it using lgdt
*/
void gdt_init(void);

#endif