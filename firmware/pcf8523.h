/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#if !defined(__PCF8523_H__)
#define __PCF8523_H__

#include <avr/io.h>

void init_rtc();

uint8_t is_rtc_connected();

uint8_t rtc_write_verify();

void set_rtc_time(uint8_t year, uint8_t month, uint8_t day,
                  uint8_t hour, uint8_t minute, uint8_t second);

uint8_t clear_rtc_interrupt_flags();

void set_rtc_alarm(uint8_t wake_hour, uint8_t wake_minute);

// Stop the default 32.768KHz CLKOUT signal on INT1.
void stop_32768_clkout();

void set_second_interrupt(uint8_t enable);

void refresh_time(uint8_t *time);

uint8_t bcd_to_decimal(uint8_t bcd);

uint8_t decimal_to_bcd(uint8_t decimal);

#endif  // guard
