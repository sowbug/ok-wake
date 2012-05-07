/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 */

// Function Tiny 328 Arduino
// Quiet    PB4  PB4 10
// Wake     PB3  PB2 12
// Button   PB1  PD2  2
// SCL      PB2  PC5 A5
// SDA      PB0  PC4 A4

#if !defined(__ATMEGA328_H__)
#define __ATMEGA328_H__

#include <compat/twi.h>

#define QUIET _BV(PB4)
#define QUIET_DD _BV(DDB4)
#define WAKE _BV(PB2)
#define WAKE_DD _BV(DDB2)

#define BUTTON _BV(PD2)
#define BUTTON_DD _BV(DDD2)
#define BUTTON_INT _BV(INT0)

#define LED_PORT PORTB
#define BUTTON_PORT PORTD
#define LED_DDR DDRB
#define BUTTON_DDR DDRD
#define BUTTON_PIN PIND

#define SCL _BV(PC5)
#define SCL_DD _BV(DDC5)
#define SDA _BV(PC4)
#define SDA_DD _BV(DDC4)

static void init_power_reduction_register(int for_power_down) {
  PRR = _BV(PRTIM2) |
    _BV(PRTIM1) |
    _BV(PRTIM0) |
    _BV(PRSPI) |
    _BV(PRUSART0) |
    _BV(PRADC);  // All peripherals off except TWI.

  if (for_power_down) {
    PRR |= _BV(PRTWI);
  }
}

static void enable_pin_interrupts() {
  EICRA = _BV(ISC01);  // The falling edge of INT0 generates an interrupt.
  EIMSK = BUTTON_INT;  // Enable INT0 interrupt.
}

#define SCL_CLOCK (100000L)

void i2c_init() {
  // no prescaler
  TWSR = 0;

  // set scl frequency
  TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2;
}

void i2c_write(uint8_t byte) {
  TWDR = byte;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
}

void i2c_start(uint8_t addr) {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  i2c_write(addr);
}

void i2c_stop() {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

uint8_t i2c_read(int ack) {
  if (ack) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  } else {
    TWCR = (1 << TWINT) | (1 << TWEN);
  }
  while (!(TWCR & (1 << TWINT)))
    ;
  return TWDR;
}

#endif  // #if !defined(__ATMEGA328_H__)
