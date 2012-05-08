/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#include "bcd.h"

const int16_t MINUTES_IN_HALF_DAY = 60 * 12;
const int16_t MINUTES_IN_DAY = 60 * 24;

uint8_t bcd_add(uint8_t a, uint8_t b) {
  uint8_t t1 = a + 0x06;
  uint8_t t2 = t1 + b;
  uint8_t t3 = t1 ^ b;
  uint8_t t4 = t2 ^ t3;
  uint8_t t5 = ~t4 & 0x10;
  uint8_t t6 = (t5 >> 2) | (t5 >> 3);
  return t2 - t6;
}

static uint8_t tencomp(uint8_t a) {
  uint8_t t1 = 0xF9 - a;
  return bcd_add(t1, 1);
}

uint8_t bcd_sub(uint8_t a, uint8_t b) {
  return bcd_sub(a, tencomp(b));
}

int16_t mul60(int16_t a) {
  return (a << 6) - (a << 2);
}

int16_t mul10(int16_t a) {
  return (a << 3) + (a << 1);
}

uint8_t bcd_to_decimal(uint8_t bcd) {
  return (bcd & 0x0f) + mul10((bcd >> 4) & 0x0f);
}

uint16_t bcd_time_to_minutes(uint16_t a) {
  return bcd_to_decimal(a & 0xff) +
      mul60(bcd_to_decimal((a >> 8) & 0xff));
}

int16_t subtract_bcd_time_in_minutes(uint16_t a, uint16_t b) {
  return bcd_time_to_minutes(a) - bcd_time_to_minutes(b);
}

int16_t smart_time_until_alarm(uint16_t time_bcd, uint16_t alarm_bcd) {
  int16_t diff_minutes = subtract_bcd_time_in_minutes(time_bcd, alarm_bcd);
  if (diff_minutes >= MINUTES_IN_HALF_DAY) {
    return diff_minutes - MINUTES_IN_DAY;
  }
  if (diff_minutes <= -MINUTES_IN_HALF_DAY) {
    return diff_minutes + MINUTES_IN_DAY;
  }
  return diff_minutes;
}

uint16_t add_decimal_to_bcd_time(uint16_t wake_time_bcd,
                                 uint8_t minutes) {
  int16_t new_time_minutes = bcd_time_to_minutes(wake_time_bcd) + minutes;
  while (new_time_minutes < 0) {
    new_time_minutes += MINUTES_IN_DAY;
  }
  while (new_time_minutes >= MINUTES_IN_DAY) {
    new_time_minutes -= MINUTES_IN_DAY;
  }
  return new_time_minutes;
}
