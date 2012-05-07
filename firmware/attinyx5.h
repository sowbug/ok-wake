/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 */

#if !defined(__ATTINYX5_H__)
#define __ATTINYX5_H__

#include <avr/interrupt.h>
#include <avr/io.h>
#include "pcf8523.h"

// Function Tiny 328 Arduino
// Quiet    PB3  PB4 10
// Wake     PB4  PB2 12
// Button   PB1  PB0  8
// SCL      PB2  PC5 A5
// SDA      PB0  PC4 A4

#define QUIET _BV(PB3)
#define QUIET_DD _BV(DDB3)
#define WAKE _BV(PB4)
#define WAKE_DD _BV(DDB4)

#define BUTTON _BV(PB1)
#define BUTTON_DD _BV(DDB1)
#define BUTTON_INT _BV(PCIE)

#define LED_PORT PORTB
#define BUTTON_PORT PORTB
#define LED_DDR DDRB
#define BUTTON_DDR DDRB
#define BUTTON_PIN PINB

#define SCL _BV(PB2)
#define SCL_DD _BV(DDB2)
#define SDA _BV(PB0)
#define SDA_DD _BV(DDB0)

static int is_button_pressed() {
  return !(PINB & BUTTON);
}

void init_power_reduction_register(int for_power_down) {
  // All peripherals off except USI.
  PRR = _BV(PRADC) | _BV(PRTIM0) | _BV(PRTIM1);
  if (for_power_down) {
    PRR |= _BV(PRUSI);
  }
}

void enable_pin_interrupts() {
  PCMSK = _BV(PCINT1);  // PB1 any change.
  GIMSK = BUTTON_INT;  // Enable INT0 interrupt.
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

volatile uint8_t _was_button_pressed;
ISR(PCINT0_vect) {
  // If we got here and the '8523's SF bit is clear, then it's a safe guess
  // that the reason /INT1 went active was because of the button press.
  _was_button_pressed = !clear_second_flag();

  // Now wait for the rising edge to pass.
  while (is_button_pressed())
    ;
}

#endif  // #if !defined(__ATTINYX5_H__)
