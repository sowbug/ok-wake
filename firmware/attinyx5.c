/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#include "attinyx5.h"
#include "i2cmaster.h"
#include "pcf8523.h"

#include <avr/interrupt.h>
#include <avr/io.h>

void init_ports() {
  // LEDs output
  LED_DDR |= QUIET_DD | WAKE_DD;
  // BUTTON input with pullup
  BUTTON_DDR &= ~BUTTON_DD;
  BUTTON_PORT |= BUTTON;
}

void quiet_on() {
  LED_PORT |= QUIET;
  LED_PORT &= ~WAKE;
}

void wake_on() {
  LED_PORT |= WAKE;
  LED_PORT &= ~QUIET;
}

void leds_off() {
  LED_PORT &= ~(WAKE | QUIET);
}

int is_button_pressed() {
  return !(PINB & BUTTON);
}

void init_power_reduction_register() {
  // All peripherals off.
  PRR = _BV(PRADC) | _BV(PRTIM0) | _BV(PRTIM1) | _BV(PRUSI);
}

void enable_pin_interrupts(uint8_t enable) {
  PCMSK = _BV(PCINT1);  // PB1 any change.
  GIMSK = enable ? BUTTON_INT : 0;  // Enable INT0 interrupt.
}

void write_i2c_byte(uint8_t addr, uint8_t reg, uint8_t data) {
  i2c_start_wait(addr + I2C_WRITE);
  i2c_write(reg);
  i2c_write(data);
  i2c_stop();
}

void write_i2c_bytes(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len) {
  i2c_start_wait(addr + I2C_WRITE);
  i2c_write(reg);
  while (len--)
    i2c_write(*data++);
  i2c_stop();
}

uint8_t read_i2c_byte(uint8_t addr, uint8_t reg) {
  i2c_start_wait(addr + I2C_WRITE);
  i2c_write(reg);
  i2c_rep_start(addr + I2C_READ);
  uint8_t r = i2c_readNak();
  i2c_stop();
  return r;
}

ISR(PCINT0_vect) {
  // We don't do any work here. We just wake up from power-down.
}
