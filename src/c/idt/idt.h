#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES_LEN 256

typedef struct __attribute__((packed)) {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t reserved; // must be zero always!!!
    uint8_t attribtues;
    uint16_t isr_high;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint32_t base;
} idtr_t;

void idt_init(void);

#endif