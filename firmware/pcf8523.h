/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#if !defined(__PCF8523_H__)
#define __PCF8523_H__

#include <inttypes.h>

void reset_rtc();

uint8_t is_rtc_connected();

uint8_t rtc_write_verify();

// registers points to 7 bytes in order S/M/H/D/W/M/Y, corresponding
// to PCF8523 registers 03-09.
void set_rtc_time(uint8_t *registers);

uint8_t clear_rtc_interrupt_flags();

void set_rtc_alarm(uint16_t time_bcd);

// Stop the default 32.768KHz CLKOUT signal on /INT1.
void stop_32768_clkout();

void set_second_interrupt(uint8_t enable);

void refresh_time(uint16_t *time_bcd);

#endif  // guard
