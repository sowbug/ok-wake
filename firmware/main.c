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
#include "devices/avr_device.h"
#include "eeprom.h"
#include "pcf8523.h"

enum {
  STATE_INIT = 0,
  STATE_NIGHT,
  STATE_TWILIGHT,
  STATE_DAWN,
};

// How long before wake time we should start blinking lights.
#define WAKE_WINDOW_PRE_MINUTES (30)

// How long after wake time we should keep blinking lights.
#define WAKE_WINDOW_POST_MINUTES (10)

uint8_t wake_hour;
uint8_t wake_minute;
uint8_t second, minute, hour;  // in same memory order as registers

// Side effect: clears value.
static uint8_t _was_button_pressed;
uint8_t was_button_pressed() {
  uint8_t result = _was_button_pressed;
  _was_button_pressed = 0;
  return result;
}

uint8_t check_button_after_interrupt() {
  // If we got here and the '8523's SF bit is clear, then it's a safe guess
  // that the reason /INT1 went active was because of the button press.
  return !clear_second_flag();
}

static void flicker_leds(uint8_t count) {
  while (count--) {
    quiet_on();
    _delay_ms(5);
    wake_on();
    _delay_ms(5);
  }
  leds_off();
}

static void flicker_quiet(uint8_t count) {
  while (count--) {
    quiet_on();
    _delay_ms(5);
    leds_off();
    _delay_ms(15);
  }
}

static void flicker_wake(uint8_t count) {
  while (count--) {
    wake_on();
    _delay_ms(5);
    leds_off();
    _delay_ms(15);
  }
}

static void breathe_wake() {
  uint8_t cycles = 2;
  uint8_t phase = 0;
  int8_t phase_direction = 1;
  while (cycles) {
    uint8_t sweep = 0;
    while (--sweep) {
      if (sweep < phase) {
        wake_on();
      } else {
        leds_off();
      }
      _delay_us(2);
    }
    phase += phase_direction;
    if (phase == 0) {
      phase_direction = -phase_direction;
      cycles--;
    }
  }
}

static uint8_t maybe_set_rtc_time() {
  uint8_t should_set = eeprom_read_byte(&kShouldSet);
  if (!should_set) {
    return 0;
  }

  // Update RTC.
  set_rtc_time(eeprom_read_byte(&kYear),
               eeprom_read_byte(&kMonth),
               eeprom_read_byte(&kDay),
               eeprom_read_byte(&kHour),
               eeprom_read_byte(&kMinute),
               eeprom_read_byte(&kSecond) + 15);

  // Mark the time set.
  eeprom_update_byte(&kShouldSet, 0);

  return 1;
}

static uint8_t bcd_to_decimal(uint8_t bcd) {
  uint8_t result = bcd & 0x0f;
  return result + ((bcd >> 4) & 0x0f) * 10;
}

static uint8_t decimal_to_bcd(uint8_t decimal) {
  uint8_t result = (decimal / 10) * 16;
  return result + (decimal % 10);
}

static void refresh_time() {
  hour = bcd_to_decimal(read_i2c_byte(RTC_ADDR, RTC_HOURS));
  minute = bcd_to_decimal(read_i2c_byte(RTC_ADDR, RTC_MINUTES));
  second = bcd_to_decimal(read_i2c_byte(RTC_ADDR, RTC_SECONDS));
}

static void read_wake_time() {
  wake_hour = bcd_to_decimal(eeprom_read_byte(&kWakeHour));
  wake_minute = bcd_to_decimal(eeprom_read_byte(&kWakeMinute));
}

// The idea is you push the button at 6pm, and it'll set the clock for a
// wakeup time of 6am.
static void set_wake_time() {
#define SET_WAKE_OFFSET_MINUTES (60 * 12)
  int16_t now_plus = hour * 60 + minute + SET_WAKE_OFFSET_MINUTES;
  while (now_plus >= (60 * 24)) {
    now_plus -= 60 * 24;
  }
  wake_hour = now_plus / 60;
  wake_minute = now_plus % 60;
  eeprom_update_byte(&kWakeHour, decimal_to_bcd(wake_hour));
  eeprom_update_byte(&kWakeMinute, decimal_to_bcd(wake_minute));

  flicker_leds(1);
}

static void set_rtc_alarm() {
  // The (1 << 7) enables the alarm component.
  write_i2c_byte(RTC_ADDR, RTC_MINUTE_ALARM,
                 (1 << 7) + decimal_to_bcd(wake_minute));
  write_i2c_byte(RTC_ADDR, RTC_HOUR_ALARM,
                 (1 << 7) + decimal_to_bcd(wake_hour));
  uint8_t rc1 = read_i2c_byte(RTC_ADDR, RTC_CONTROL_1);
  write_i2c_byte(RTC_ADDR, RTC_CONTROL_1, rc1 | (1 << 1));  // AIE=1
}

static int16_t calculate_minutes_until_wake() {
  int16_t now = hour * 60 + minute;
  int16_t wake_time = wake_hour * 60 + wake_minute;
  return now - wake_time;
}

// Stop the default 32.768KHz CLKOUT signal on INT1.
static void stop_32768_clkout() {
  write_i2c_byte(RTC_ADDR, RTC_TMR_CLKOUT_CTRL, RTC_CLKOUT_DISABLED);
}

static void set_second_interrupt(uint8_t enable) {
  uint8_t rc1 = read_i2c_byte(RTC_ADDR, RTC_CONTROL_1);
  if (enable) {
    rc1 |= (1 << 2);  // SIE=1
  } else {
    rc1 &= ~(1 << 2);  // SIE=0
  }
  write_i2c_byte(RTC_ADDR, RTC_CONTROL_1, rc1);
}

static uint8_t start_NIGHT() {
  set_second_interrupt(0);
  return STATE_NIGHT;
}

static uint8_t start_TWILIGHT() {
  set_second_interrupt(1);
  return STATE_TWILIGHT;
}

static uint8_t start_DAWN() {
  set_second_interrupt(1);
  return STATE_DAWN;
}

static void power_down() {
  leds_off();
  init_power_reduction_register(1);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Go to sleep.
  sleep_mode();

  // We've woken up. Turn back on any peripherals we need while running.
  init_power_reduction_register(0);
  i2c_init();
  _was_button_pressed = check_button_after_interrupt();
  refresh_time();
}

int main(void) {
  init_ports();
  i2c_init();
  stop_32768_clkout();
  enable_pin_interrupts();
  if (maybe_set_rtc_time()) {
    flicker_leds(16);
  }
  read_wake_time();
  set_rtc_alarm();
  set_second_interrupt(0);

  sei();

  uint8_t state = STATE_NIGHT;
  while (1) {
    // First thing we do is shut off. We'll be woken up by INT0 or button.
    power_down();

    // We woke up!
    if (was_button_pressed()) {
      set_wake_time();
      continue;
    }

    int16_t minutes_until_wake = calculate_minutes_until_wake();
    uint8_t in_pre_window = (minutes_until_wake >= -WAKE_WINDOW_PRE_MINUTES &&
                             minutes_until_wake < 0);
    uint8_t in_post_window = (minutes_until_wake >= 0 &&
                              minutes_until_wake < WAKE_WINDOW_POST_MINUTES);

    //
    in_pre_window = 0;
    in_post_window = 1;
    //

    if (in_pre_window && state != STATE_TWILIGHT) {
      state = start_TWILIGHT();
    }
    if (in_post_window && state != STATE_DAWN) {
      state = start_DAWN();
    }
    if (!in_pre_window && !in_post_window && state != STATE_NIGHT) {
      state = start_NIGHT();
    }
    switch (state) {
    case STATE_NIGHT:
      break;
    case STATE_TWILIGHT:
      if (0 && minutes_until_wake > -3) {
        flicker_quiet(2);
        flicker_wake(1);
      } else {
        flicker_quiet(1);
      }
      break;
    case STATE_DAWN:
      breathe_wake();
      break;
    }
  }
}
