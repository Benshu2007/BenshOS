#include "idt.h"
#include "../gdt/gdt.h"
#include <stdbool.h>
#include <stdint.h>

static idt_entry_t idt[IDT_ENTRIES_LEN];
static idtr_t idtr;
static bool vectors[IDT_ENTRIES_LEN];

extern void* isr_stub_table[];
extern void flush_idt(idtr_t* idtr);

static void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low     = (uint32_t)(uintptr_t)isr & 0xFFFF;
    descriptor->kernel_cs   = GDT_KERNEL_CODE;
    descriptor->attribtues  = flags;
    descriptor->isr_high    = (uint32_t)(uintptr_t)isr >> 16;
    descriptor->reserved    = 0;
}

void idt_init(void) {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = sizeof(idt) - 1;

    for (uint8_t vector = 0; vector < 7; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    flush_idt(&idtr);
}

