#ifndef TIMER_H
#define TIMER_H

#include "../driver/driver.h"
#include <stdint.h>

extern driver_t timer_driver;

void sleep(uint32_t ms);
uint32_t timer_get_ticks(void);

#endif