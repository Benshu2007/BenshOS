#include "interrupts.h"
#include "../terminal/terminal.h"
#include <stdint.h>

void exception_handler(interrupt_frame_t* frame)
{
    terminal_log("Exception");

    terminal_log_hex(frame->vector);
    terminal_log_hex(frame->eip);

    while(1)
        asm volatile("hlt");
}
