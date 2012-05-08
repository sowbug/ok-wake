/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#include "attinyx5.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include "pcf8523.h"
#include "USI_TWI_Master.h"

int is_button_pressed() {
  return !(PINB & BUTTON);
}

void init_power_reduction_register(int for_power_down) {
  // All peripherals off except USI.
  PRR = _BV(PRADC) | _BV(PRTIM0) | _BV(PRTIM1);
  if (for_power_down) {
    PRR |= _BV(PRUSI);
  }
}

void enable_pin_interrupts(uint8_t enable) {
  PCMSK = _BV(PCINT1);  // PB1 any change.
  GIMSK = enable ? BUTTON_INT : 0;  // Enable INT0 interrupt.
}

void i2c_init() {
  USI_TWI_Master_Initialize();
}

void write_i2c_byte(uint8_t addr, uint8_t reg, uint8_t data) {
  uint8_t i2c_buffer[3] = { addr, reg, data };
  USI_TWI_Start_Read_Write(i2c_buffer, 2 + 1);
}

uint8_t read_i2c_byte(uint8_t addr, uint8_t reg) {
  uint8_t i2c_buffer[2] = { addr, reg };
  USI_TWI_Start_Read_Write(i2c_buffer, 2);
  i2c_buffer[0] |= 1;
  i2c_buffer[1] = 0;
  USI_TWI_Start_Read_Write(i2c_buffer, 2);
  return i2c_buffer[1];
}

ISR(PCINT0_vect) {
  // We don't do any work here. We just wake up from power-down.
}
