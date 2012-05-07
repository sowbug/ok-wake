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

int is_button_pressed();

void init_power_reduction_register(int for_power_down);

void enable_pin_interrupts();

void i2c_init();

void write_i2c_byte(uint8_t addr, uint8_t reg, uint8_t data);

uint8_t read_i2c_byte(uint8_t addr, uint8_t reg);

#endif  // #if !defined(__ATTINYX5_H__)
