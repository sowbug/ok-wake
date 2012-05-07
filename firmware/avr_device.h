/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#if !defined(__AVR_DEVICE_H__)
#define __AVR_DEVICE_H__

#if defined(__AVR_ATtiny25__) || \
  defined (__AVR_ATtiny45__) || \
  defined (__AVR_ATtiny85__)
#include "USI_TWI_Master.h"
#include "attinyx5.h"
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
#include "atmega328.h"
#endif

static void init_ports() {
  // LEDs output
  LED_DDR |= QUIET_DD | WAKE_DD;

  // BUTTON input with pullup
  BUTTON_DDR &= ~BUTTON_DD;
  BUTTON_PORT |= BUTTON;
}

static void quiet_on() {
  LED_PORT |= QUIET;
  LED_PORT &= ~WAKE;
}

static void wake_on() {
  LED_PORT |= WAKE;
  LED_PORT &= ~QUIET;
}

static void leds_off() {
  LED_PORT &= ~(WAKE | QUIET);
}

#endif  // guard
