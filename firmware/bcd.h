/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#if !defined(__BCD_H__)
#define __BCD_H__

#include <inttypes.h>

extern const int16_t MINUTES_IN_HALF_DAY;
extern const int16_t MINUTES_IN_DAY;

uint8_t bcd_add(uint8_t a, uint8_t b);
uint8_t bcd_sub(uint8_t a, uint8_t b);

uint8_t bcd_to_decimal(uint8_t bcd);
uint16_t bcd_time_to_decimal(uint16_t a);
int16_t subtract_bcd_time_in_minutes(uint16_t a, uint16_t b);
uint16_t add_minutes_to_bcd_time(uint16_t wake_time_bcd,
                                 int16_t minutes);

// Returns the number of minutes between the given time and the given alarm
// time, interpreting the alarm time as the one closest to the given time.
//
// A positive number means the alarm is in the future.
// A negative number means the alarm is in the past.
int16_t smart_time_until_alarm(uint16_t time_bcd, uint16_t alarm_bcd);

#endif  // __BCD_H__
