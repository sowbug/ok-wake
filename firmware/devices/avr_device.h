/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
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

ISR(INT0_vect) {
  // We don't do any work here. We just wake up from power-down.
}

void write_i2c_byte(uint8_t addr, uint8_t reg, uint8_t data) {
  i2c_start(addr);
  i2c_write(reg);
  i2c_write(data);
  i2c_stop();
}

uint8_t read_i2c_byte(uint8_t addr, uint8_t reg) {
  uint8_t result = 0;

  i2c_start(addr);
  i2c_write(reg);

  i2c_start(addr + 1);
  result = i2c_read(0);
  i2c_stop();
  return result;
}

#endif  // guard
