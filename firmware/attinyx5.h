/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#if !defined(__ATTINYX5_H__)
#define __ATTINYX5_H__

#include <avr/interrupt.h>
#include <avr/io.h>
#include "pcf8523.h"

// Function t25
// Quiet    PB3
// Wake     PB4
// Button   PB1
// SCL      PB2
// SDA      PB0

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

void init_ports();
void quiet_on();
void wake_on();
void leds_off();

int is_button_pressed();

void init_power_reduction_register();

void enable_pin_interrupts(uint8_t enable);

uint8_t read_i2c_byte(uint8_t addr, uint8_t reg);
void write_i2c_byte(uint8_t addr, uint8_t reg, uint8_t data);
void write_i2c_bytes(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len);

#endif  // #if !defined(__ATTINYX5_H__)
