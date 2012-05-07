/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 */

#include <util/delay.h>
#include "pcf8523.h"
#include "i2c.h"

// forward declaration. TODO: fix!
void i2c_init();

#define RTC_ADDR (0xD0)
enum {
  RTC_CONTROL_1 = 0,
  RTC_CONTROL_2,
  RTC_CONTROL_3,
  RTC_SECONDS,
  RTC_MINUTES,
  RTC_HOURS,
  RTC_DAYS,
  RTC_WEEKDAYS,
  RTC_MONTHS,
  RTC_YEARS,
  RTC_MINUTE_ALARM,
  RTC_HOUR_ALARM,
  RTC_DAY_ALARM,
  RTC_WEEKDAY_ALARM,
  RTC_OFFSET,
  RTC_TMR_CLKOUT_CTRL,
  RTC_TMR_A_FREQ_CTRL,
  RTC_TMR_A_REG,
  RTC_TMR_B_FREQ_CTRL,
  RTC_TMR_B_REG,
};

#define RTC_CLKOUT_DISABLED ((1 << 3) | (1 << 4) | (1 << 5))

void init_rtc() {
  i2c_init();
}

uint8_t is_rtc_connected() {
  uint8_t sample = read_i2c_byte(RTC_ADDR, RTC_SECONDS);
  _delay_ms(1010);
  return read_i2c_byte(RTC_ADDR, RTC_SECONDS) != sample;
}

uint8_t rtc_write_verify() {
  uint8_t sample = read_i2c_byte(RTC_ADDR, RTC_TMR_B_REG) + 7;
  write_i2c_byte(RTC_ADDR, RTC_TMR_B_REG, sample);
  return read_i2c_byte(RTC_ADDR, RTC_TMR_B_REG) == sample;
}

void set_rtc_time(uint8_t year, uint8_t month, uint8_t day, uint8_t hour,
                  uint8_t minute, uint8_t second) {
  write_i2c_byte(RTC_ADDR, RTC_YEARS, year);
  write_i2c_byte(RTC_ADDR, RTC_MONTHS, month);
  write_i2c_byte(RTC_ADDR, RTC_DAYS, day);
  write_i2c_byte(RTC_ADDR, RTC_HOURS, hour);
  write_i2c_byte(RTC_ADDR, RTC_MINUTES, minute);
  write_i2c_byte(RTC_ADDR, RTC_SECONDS, second);
}

uint8_t clear_second_flag() {
  uint8_t rc2 = read_i2c_byte(RTC_ADDR, RTC_CONTROL_2);
  if ((rc2 & (1 << 4)) != 0) {
    write_i2c_byte(RTC_ADDR, RTC_CONTROL_2, rc2 & ~(1 << 4));
    return 1;
  }
  return 0;
}

void set_rtc_alarm(uint8_t wake_hour, uint8_t wake_minute) {
  // The (1 << 7) enables the alarm component.
  write_i2c_byte(RTC_ADDR, RTC_MINUTE_ALARM,
                 (1 << 7) + decimal_to_bcd(wake_minute));
  write_i2c_byte(RTC_ADDR, RTC_HOUR_ALARM,
                 (1 << 7) + decimal_to_bcd(wake_hour));
  uint8_t rc1 = read_i2c_byte(RTC_ADDR, RTC_CONTROL_1);
  write_i2c_byte(RTC_ADDR, RTC_CONTROL_1, rc1 | (1 << 1));  // AIE=1
}

// Stop the default 32.768KHz CLKOUT signal on INT1.
void stop_32768_clkout() {
  write_i2c_byte(RTC_ADDR, RTC_TMR_CLKOUT_CTRL, RTC_CLKOUT_DISABLED);
}

void set_second_interrupt(uint8_t enable) {
  uint8_t rc1 = read_i2c_byte(RTC_ADDR, RTC_CONTROL_1);
  if (enable) {
    rc1 |= (1 << 2);  // SIE=1
  } else {
    rc1 &= ~(1 << 2);  // SIE=0
  }
  write_i2c_byte(RTC_ADDR, RTC_CONTROL_1, rc1);
}

void refresh_time(uint8_t *hour, uint8_t *minute, uint8_t *second) {
  *hour = bcd_to_decimal(read_i2c_byte(RTC_ADDR, RTC_HOURS));
  *minute = bcd_to_decimal(read_i2c_byte(RTC_ADDR, RTC_MINUTES));
  *second = bcd_to_decimal(read_i2c_byte(RTC_ADDR, RTC_SECONDS));
}

uint8_t bcd_to_decimal(uint8_t bcd) {
  uint8_t result = bcd & 0x0f;
  return result + ((bcd >> 4) & 0x0f) * 10;
}

uint8_t decimal_to_bcd(uint8_t decimal) {
  uint8_t result = (decimal / 10) * 16;
  return result + (decimal % 10);
}
