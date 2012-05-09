/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#include "attinyx5.h"
#include "i2cmaster.h"
#include "pcf8523.h"

#include <avr/io.h>
#include <util/delay.h>

const uint8_t AF = _BV(3);
const uint8_t AIE = _BV(1);
const uint8_t RTC_ADDR = 0xD0;
const uint8_t RTC_CLKOUT_DISABLED = (_BV(3) | _BV(4) | _BV(5));
const uint8_t SF = _BV(4);
const uint8_t SIE = _BV(2);

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

void init_rtc() {
  i2c_init();
}

void reset_rtc() {
  write_i2c_byte(RTC_ADDR, RTC_CONTROL_1, 0x58);
}

static uint8_t read_seconds() {
  return read_i2c_byte(RTC_ADDR, RTC_SECONDS);
}

uint8_t is_rtc_connected() {
  uint8_t sample = read_seconds();
  _delay_ms(1010);
  return read_seconds() != sample;
}

uint8_t rtc_write_verify() {
  uint8_t sample = read_i2c_byte(RTC_ADDR, RTC_TMR_B_REG) + 7;
  write_i2c_byte(RTC_ADDR, RTC_TMR_B_REG, sample);
  return read_i2c_byte(RTC_ADDR, RTC_TMR_B_REG) == sample;
}

void set_rtc_time(uint8_t year, uint8_t month, uint8_t day,
                  uint8_t hour, uint8_t minute, uint8_t second) {
  write_i2c_byte(RTC_ADDR, RTC_YEARS, year);
  write_i2c_byte(RTC_ADDR, RTC_MONTHS, month);
  write_i2c_byte(RTC_ADDR, RTC_DAYS, day);
  write_i2c_byte(RTC_ADDR, RTC_HOURS, hour);
  write_i2c_byte(RTC_ADDR, RTC_MINUTES, minute);
  write_i2c_byte(RTC_ADDR, RTC_SECONDS, second);
}

uint8_t clear_rtc_interrupt_flags() {
  uint8_t rc2 = read_i2c_byte(RTC_ADDR, RTC_CONTROL_2) & (SF | AF);
  write_i2c_byte(RTC_ADDR, RTC_CONTROL_2, 0);  // Just zero the whole thing
  return rc2 != 0;
}

void set_rtc_alarm(uint16_t time_bcd) {
  write_i2c_byte(RTC_ADDR, RTC_MINUTE_ALARM, time_bcd);
  write_i2c_byte(RTC_ADDR, RTC_HOUR_ALARM, time_bcd >> 8);
  write_i2c_byte(RTC_ADDR, RTC_CONTROL_1,
                 read_i2c_byte(RTC_ADDR, RTC_CONTROL_1) | AIE);
}

// Stop the default 32.768KHz CLKOUT signal on INT1.
void stop_32768_clkout() {
  write_i2c_byte(RTC_ADDR, RTC_TMR_CLKOUT_CTRL, RTC_CLKOUT_DISABLED);
}

void set_second_interrupt(uint8_t enable) {
  uint8_t rc1 = read_i2c_byte(RTC_ADDR, RTC_CONTROL_1);

  if (enable) {
    rc1 |= SIE;
  } else {
    rc1 &= ~SIE;
  }

  write_i2c_byte(RTC_ADDR, RTC_CONTROL_1, rc1);
}

void refresh_time(uint16_t *time_bcd) {
  *time_bcd = (read_i2c_byte(RTC_ADDR, RTC_HOURS) << 8) |
      read_i2c_byte(RTC_ADDR, RTC_MINUTES);
}
