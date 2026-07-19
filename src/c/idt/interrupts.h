#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    // Segment selectors (safely stored as 32-bit values)
    uint32_t es;
    uint32_t ds;

    // Registers pushed by 'pusha' (ordered from lowest to highest address)
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp; // Note: This is the ESP value *before* pusha executed
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // Pushed by our interrupt macros
    uint32_t vector;
    uint32_t error;

    // Pushed automatically by the CPU hardware
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} interrupt_frame_t;


void exception_handler(interrupt_frame_t* frame);
void irq_handler(interrupt_frame_t* frame);

#endif