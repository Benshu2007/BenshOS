#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

typedef struct __attribute__((packed))  {
    uint32_t es;
    uint32_t ds;

    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;

    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t vector;
    uint32_t error;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} interrupt_frame_t;


void exception_handler(interrupt_frame_t* frame);

#endif