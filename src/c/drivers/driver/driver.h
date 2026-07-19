#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define IRQ_MAX 16
#define KEYBOARD_PORT 0x60

typedef enum {
    DRIVER_KEYBOARD,
    DRIVER_MOUSE,
    DRIVER_TIMER,
    DRIVER_DISK
} driver_type_t;

// The universal contract that every hardware driver must implement
typedef struct __attribute__((packed)) {
    const char* name;
    driver_type_t type;
    
    bool (*init)(void);                  // Called once at boot to setup the hardware
    void (*handle_interrupt)(void);     // Called whenever this device's IRQ fires
    bool (*disable)(void);               // Called if you need to shut down the hardware
} driver_t;

void driver_manager_init(void);
bool driver_register(driver_t* driver, uint8_t irq_line);
void kernel_driver_route_interrupt(uint8_t irq_line);

#endif