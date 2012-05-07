/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 */

#if !defined(__PCF8523_H__)
#define __PCF8523_H__

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

#endif  // guard
