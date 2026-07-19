#include "pic.h"
#include "../helpers/common.h"
#include "../terminal/terminal.h"
#include <stdint.h>

void PIC_remap(uint32_t offset1, uint32_t offset2) {
  // starts the initialization sequence (in cascade mode)
  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();

  // ICW2: PICs vector offsets
  outb(PIC1_DATA, offset1);
  io_wait();
  outb(PIC2_DATA, offset2);
  // ICW3: tell Master PIC that there is a slave PIC at IRQ2
  outb(PIC1_DATA, 1 << CASCADE_IRQ);
  io_wait();
  // ICW3: tell Slave PIC its cascade identity
  outb(PIC2_DATA, CASCADE_IRQ);
  io_wait();

  // ICW4: have the PICs use 8086 mode (and not 8080 mode)
  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  // SAFE FIX: Disable all hardware IRQs for now.
  // Only leave IRQ 2 (Master bit 2) open so the Slave PIC can function.
  outb(PIC1_DATA, 0xFB);
  outb(PIC2_DATA, 0xFF);
}

void PIC_send_EOI(uint8_t irq) {
  if (irq >= 8)
    outb(PIC2_COMMAND, PIC_EOI);
  outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_disable(void) {
  outb(PIC1_DATA, 0xff);
  outb(PIC2_DATA, 0xff);
}

void IRQ_set_mask(uint8_t IRQline) {
  uint16_t port;
  uint8_t value;

  if (IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) | (1 << IRQline);
  outb(port, value);
}

void IRQ_clear_mask(uint8_t IRQline) {
  uint16_t port;
  uint8_t value;

  if (IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) & ~(1 << IRQline);
  outb(port, value);
}

/* Helper func */
static uint16_t __pic_get_irq_reg(int ocw3) {
  /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
   * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
  outb(PIC1_COMMAND, ocw3);
  outb(PIC2_COMMAND, ocw3);
  return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t PIC_get_IRR(void) { return __pic_get_irq_reg(PIC_READ_IRR); }

uint16_t PIC_get_ISR(void) { return __pic_get_irq_reg(PIC_READ_ISR); }

void pic_init(void) {
  terminal_log("Initializing PIC Routing...");
  PIC_remap(0x20, 0x28); // Vectors 32-39 for Master, 40-47 for Slave

  // Activate the specific wires you want your irq_handler to capture
  IRQ_clear_mask(0); // Unmask Timer (IRQ 0 / Vector 32)
  IRQ_clear_mask(1); // Unmask Keyboard (IRQ 1 / Vector 33)

  terminal_log("Enabling Interrupt Flag...");
  __asm__ volatile("sti"); // Signals the CPU to start processing hardware lines

  terminal_log("System live and listening.");
}
