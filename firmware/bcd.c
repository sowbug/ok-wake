/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#include "bcd.h"

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
