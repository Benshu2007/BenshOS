#include "timer.h"
#include "../../helpers/common.h"

// Hardware baseline constants
#define PIT_OSCILLATOR_BASE_HZ 1193182
#define TARGET_TICK_RATE_HZ    1000  // 1000 ticks per second = 1 millisecond intervals

// Target hardware ports
#define PIT_PORT_CHANNEL_0     0x40
#define PIT_PORT_COMMAND       0x43

// Static atomic global counter
static volatile uint32_t system_ticks = 0;

// Private helper to flash internal hardware registers
static void program_pit_rate(uint32_t target_hz) {
    // Calculate the 16-bit divisor frequency slice
    uint32_t divisor = PIT_OSCILLATOR_BASE_HZ / target_hz;

    // Command configuration word: 
    // 0x36 = Channel 0, Access lowest then highest byte, Mode 3 (Square Wave Generator), Binary
    outb(PIT_PORT_COMMAND, 0x36);

    // Send the split 16-bit register payload down the data channel line
    outb(PIT_PORT_CHANNEL_0, (uint8_t)(divisor & 0xFF));        // Lower 8-bits
    outb(PIT_PORT_CHANNEL_0, (uint8_t)((divisor >> 8) & 0xFF)); // Upper 8-bits
}

// --- UNIVERSAL INTERFACE CONTRACT FUNCTIONS ---

static bool timer_init(void) {
    system_ticks = 0;
    
    // Calibrate hardware to generate steady pulses every 1ms
    program_pit_rate(TARGET_TICK_RATE_HZ);
    return true;
}

static void timer_interrupt_handler(void) {
    system_ticks++;
}

// Map operations into the public struct layout configuration template
driver_t timer_driver = {
    .name = "Intel 8253 PIT Multi-Channel Timer",
    .type = DRIVER_TIMER,
    .init = timer_init,
    .handle_interrupt = timer_interrupt_handler,
    .disable = NULL
};


// --- PUBLIC IMPLEMENTATION API ---

uint32_t timer_get_ticks(void) {
    return system_ticks;
}

// A safe, hardware-driven blocking delay
void sleep(uint32_t milliseconds) {
    uint32_t target_end_ticks = system_ticks + milliseconds;
    
    while (system_ticks < target_end_ticks) {
        // Puts the CPU into a power-saving halt mode until the next IRQ 0 tick fires
        __asm__ volatile("hlt"); 
    }
}
