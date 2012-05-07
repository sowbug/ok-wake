/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 */

#if !defined(__ATTINYX5_H__)
#define __ATTINYX5_H__

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

uint8_t i2c_buffer[3] = { RTC_ADDR };

void i2c_init() {
  USI_TWI_Master_Initialise();
}

void i2c_write(uint8_t byte) {
  i2c_buffer[2] = byte;
  USI_TWI_Start_Random_Write(i2c_buffer, 3);
}

void i2c_start(uint8_t addr) {
  i2c_buffer[1] = addr;
}

void i2c_stop() {
}

uint8_t i2c_read(int ack) {
  USI_TWI_Start_Random_Read(i2c_buffer, 3);
  return i2c_buffer[2];
}

#endif  // #if !defined(__ATTINYX5_H__)
