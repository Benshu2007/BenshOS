#include "interrupts.h"
#include "../terminal/terminal.h"
#include "../drivers/driver/driver.h"
#include "../pic/pic.h"
#include <stdint.h>

void exception_handler(interrupt_frame_t* frame)
{
    terminal_log("Exception in vector");

    terminal_log_hex(frame->vector);

    while(1)
        asm volatile("hlt");
}

void irq_handler(interrupt_frame_t* frame) {
    uint8_t irq_line = frame->vector - 32;

    kernel_driver_route_interrupt(irq_line);

    PIC_send_EOI(irq_line);
}
