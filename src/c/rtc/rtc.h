#ifndef RTC_H
#define RTC_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint32_t year;
} rtc_time_t;

rtc_time_t rtc_get_time(int8_t timezone);
void rtc_print_current_time(int8_t timezone);

#endif
