/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#if !defined(__BCD_H__)
#define __BCD_H__

#include <avr/io.h>

uint8_t bcd_add(uint8_t a, uint8_t b);
uint8_t bcd_sub(uint8_t a, uint8_t b);

#endif  // __BCD_H__
