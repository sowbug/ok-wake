/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 */

// Function Tiny 328 Arduino
// Quiet    PB4  PB4 10
// Wake     PB3  PB2 12
// Button   PB1  PB0  8
// SCL      PB2  PC5 A5
// SDA      PB0  PC4 A4

#define QUIET _BV(PB4)
#define QUIET_DD _BV(DDB4)
#define WAKE _BV(PB3)
#define WAKE_DD _BV(DDB3)

#define BUTTON _BV(PB1)
#define BUTTON_DD _BV(DDB1)
#define BUTTON_INT _BV(INT0)

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

static void init_power_reduction_register() {
  PRR = _BV(PRADC) | _BV(PRTIM0);  // All peripherals off.
}

static void enable_pin_interrupts() {
  MCUCR = _BV(ISC01);  // The falling edge of INT0 generates an interrupt.
  GIMSK = BUTTON_INT;  // Enable INT0 interrupt.
}
