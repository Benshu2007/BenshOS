#include "driver.h"
#include "../../pic/pic.h"
#include "../../terminal/terminal.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static driver_t *drivers[IRQ_MAX] = {NULL};

void driver_manager_init(void) {
  for (size_t i = 0; i < IRQ_MAX; ++i) {
    drivers[i] = NULL;
  }
}

bool driver_register(driver_t *driver, uint8_t irq_line) {
  if (irq_line > IRQ_MAX || driver == NULL) {
    return false;
  }

  drivers[irq_line] = driver;


  if (driver->init != NULL) {
    driver->init();
  }

  IRQ_clear_mask(irq_line);
  return true;
}

void kernel_driver_route_interrupt(uint8_t irq_line) {
    if (irq_line < IRQ_MAX && drivers[irq_line] != NULL) {
        drivers[irq_line]->handle_interrupt();
    }
}
