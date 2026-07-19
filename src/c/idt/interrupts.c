#include "interrupts.h"
#include "../terminal/terminal.h"
#include "../pic/pic.h"
#include <stdint.h>

void exception_handler(interrupt_frame_t* frame)
{
    terminal_log("Exception");

    terminal_log_hex(frame->vector);

    while(1)
        asm volatile("hlt");
}

void irq_handler(interrupt_frame_t* frame) {
    // Vector 39 corresponds to IRQ 7
    if (frame->vector == 39) {
        uint16_t pic_isr = PIC_get_ISR();
        
        // If Bit 7 of the Master PIC's ISR isn't set, it's a phantom wire glitch!
        if (!(pic_isr & (1 << 7))) {
            // SPURIOUS! Do NOT send an EOI for master spurious interrupts.
            // Just return safely.
            return; 
        }
    }

    // Vector 47 corresponds to IRQ 15
    if (frame->vector == 47) {
        uint16_t pic_isr = PIC_get_ISR();
        
        // If Bit 15 of the Slave PIC's ISR isn't set, it's spurious!
        if (!(pic_isr & (1 << 15))) {
            // For SLAVE spurious interrupts, you MUST send an EOI to the master PIC only.
            PIC_send_EOI(2); 
            return;
        }
    }

    // Standard Handlers Below...
    if (frame->vector == 32) {
        // ... (Timer logic)
        PIC_send_EOI(0);
    }
}

