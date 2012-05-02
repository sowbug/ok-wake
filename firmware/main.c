/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <util/delay.h>

#define QUIET _BV(PB3)
#define QUIET_DD _BV(DDB3)
#define WAKE _BV(PB4)
#define WAKE_DD _BV(DDB4)

#define SCL _BV(PB2)
#define SCL_DD _BV(DDB2)
#define SDA _BV(PB0)
#define SDA_DD _BV(DDB0)

#define INT_CLK _BV(PB1)
#define INT_CLK_DD _BV(DDB1)
#define INT_CLK_PCINT _BV(PCINT1)

static int is_button_pressed() {
  return !(PINB & INT_CLK);
}

ISR(PCINT0_vect) {
  // If this is an RTC interrupt, give it a moment to unassert.
  _delay_ms(1);

  if (is_button_pressed()) {
  } else {
  }
}

static void disable_prr() {
  PRR = _BV(PRADC) | _BV(PRTIM0);  // All peripherals off.
}

static void enable_pin_interrupts() {
  GIMSK = _BV(PCIE);  // Enable pin-change interrupts
  PCMSK = INT_CLK_PCINT;  // Mask off all but the int pin for interrupts
}

static void set_port_state() {
  // LEDs output
  DDRB |= QUIET_DD | WAKE_DD;

  // INT_CLK input with pullup
  DDRB &= ~INT_CLK_DD;
  PORTB |= INT_CLK;
}

static void quiet_on() {
  PORTB |= QUIET;
  PORTB &= ~WAKE;
}

static void wake_on() {
  PORTB |= WAKE;
  PORTB &= ~QUIET;
}

static void leds_off() {
  PORTB &= ~(WAKE | QUIET);
}

int main() {
  set_port_state();
  disable_prr();
  enable_pin_interrupts();
  leds_off();

  while (1) {
    quiet_on();
    _delay_ms(500);
    wake_on();
    _delay_ms(500);
  }

  return 0;
}
