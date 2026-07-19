#include "rtc.h"
#include "../common/common.h"
#include "../terminal/terminal.h"
#include <stdbool.h>
#include <stdint.h>

// Days in each month (Index 1 = January, Index 12 = December)
static const uint8_t days_in_months[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#define CMOS_PORT_SELECT 0x70
#define CMOS_PORT_DATA 0x71

static bool is_cmos_busy(void) {
  outb(CMOS_PORT_SELECT, 0x0A);
  return (inb(CMOS_PORT_DATA) & 0x80);
}

static uint8_t get_cmos_register(uint8_t reg) {
  outb(CMOS_PORT_SELECT, reg);
  return inb(CMOS_PORT_DATA);
}

static bool is_leap_year(uint32_t year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

rtc_time_t rtc_get_time(int8_t timezone) {
  while (is_cmos_busy()) {
  }

  rtc_time_t time;

  // Read the raw BCD bytes from the standard CMOS offsets
  uint8_t raw_second = get_cmos_register(0x00);
  uint8_t raw_minute = get_cmos_register(0x02);
  uint8_t raw_hour = get_cmos_register(0x04);
  uint8_t raw_day = get_cmos_register(0x07);
  uint8_t raw_month = get_cmos_register(0x08);
  uint8_t raw_year = get_cmos_register(0x09);

  // Convert Binary Coded Decimal (BCD) to regular binary integers
  // Example: BCD 0x25 becomes standard integer 25
  time.second = (raw_second & 0x0F) + ((raw_second / 16) * 10);
  time.minute = (raw_minute & 0x0F) + ((raw_minute / 16) * 10);
  time.hour = (raw_hour & 0x0F) + ((raw_hour / 16) * 10);
  time.day = (raw_day & 0x0F) + ((raw_day / 16) * 10);
  time.month = (raw_month & 0x0F) + ((raw_month / 16) * 10);
  time.year = (raw_year & 0x0F) + ((raw_year / 16) * 10);

  // Modernize the legacy 2-digit year format (e.g., "26" becomes "2026")
  time.year += 2000;

  int16_t adjusted_hour = (int16_t)time.hour + timezone;

  if (adjusted_hour >= 24) {
    time.hour = adjusted_hour - 24;
    time.day++;

    uint8_t max_days = days_in_months[time.month];
    if (time.month == 2 && is_leap_year(time.year)) {
        max_days = 29;
    }

    if (time.day > max_days) {
        time.month++;
        if (time.month > 12) {
            time.month = 1;
            time.year++;
        }
    }
  } else if (adjusted_hour < 0) {
    time.hour = adjusted_hour + 24;
    time.day--;

    if (time.day == 0) {
        time.month--;
        if (time.month == 0) {
            time.month = 12;
            time.year--;
        }

        time.day = days_in_months[time.month];
        if (time.month == 2 && is_leap_year(time.year)) {
            time.day = 29;
        }
    }
  } else {
    time.hour = (uint8_t) adjusted_hour;
  }

  return time;
}

void rtc_print_current_time(int8_t timezone) {
    rtc_time_t time = rtc_get_time(timezone);

    terminal_log("Date printing as DD/MM/YYYY H:M:S:");
    terminal_log("%n/%n/%n %n:%n:%n", time.day, time.month, time.year, time.hour, time.minute, time.second);
}
