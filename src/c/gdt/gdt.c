#include "gdt.h"
#include "../helpers/bits.h"
#include <stdint.h>

extern void flush_gdt(GDTR* gdtr);

GDT_ENTRY gdt[GDT_ENTRIES_LEN] = {0};
GDTR gdtr = { 0 };

static void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access,
                   uint8_t flags) {
  if (index < 0 || index >= GDT_ENTRIES_LEN)
    return;

  GDT_ENTRY entry;
  entry.base_low = get_16_low(base);
  entry.base_mid = get_8_low(get_16_high(base));
  entry.base_high = get_8_high(get_16_high(base));

  entry.limit_low = get_16_low(limit);
  entry.flags_limit_high = (flags << 4) | ((limit >> 16) & 0x0F);
  entry.access_byte = access;

  gdt[index] = entry;
}

static void set_gdtr(void) {
    gdtr.limit = sizeof(gdt) - 1;
    gdtr.offset = (uint32_t)(uintptr_t)&gdt;
}

void gdt_init(void) {
  gdt_set_entry(0, 0, 0, 0, 0);            // Null Descriptor
  gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xC); // Kernel Mode Code Segment
  gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC); // Kernel Mode Data Segment
  gdt_set_entry(3, 0, 0xFFFFF, 0xFA, 0xC); // User Mode Code Segment
  gdt_set_entry(4, 0, 0xFFFFF, 0xF2, 0xC); // User Mode Data Segment

  // for later.
  // gdt_set_entry(5, (uint32_t)&tss, sizeof(tss) - 1, 0x89, 0x0); // Task State
  // Segment

  set_gdtr();

  // call asm file flush_gdt.asm
  flush_gdt(&gdtr);
}
