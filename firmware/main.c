/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#include <avr/io.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <util/delay.h>

#include "avr_device.h"
#include "eeprom.h"

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
static uint8_t _was_button_pressed = 0;
uint8_t was_button_pressed() {
  uint8_t result = _was_button_pressed;
  _was_button_pressed = 0;
  return result;
}

static void flicker_leds(uint8_t count) {
  while (count--) {
    quiet_on();
    _delay_ms(50);
    wake_on();
    _delay_ms(50);
  }
  leds_off();
}

static void flicker_quiet(uint8_t count) {
  while (count--) {
    quiet_on();
    _delay_ms(25);
    leds_off();
    _delay_ms(25);
  }
}

static void flicker_wake(uint8_t count) {
  while (count--) {
    wake_on();
    _delay_ms(25);
    leds_off();
    _delay_ms(25);
  }
}

static void breathe_wake() {
  uint8_t cycles = 2;
  uint8_t phase = 0;
  int8_t phase_direction = 4;
  while (cycles) {
    uint8_t sweep = 0;
    while (--sweep) {
      if (sweep < phase) {
        wake_on();
      } else {
        leds_off();
      }
    }
    phase += phase_direction;
    if (phase == 0) {
      phase_direction = -phase_direction;
      cycles--;
    }
  }
}

static uint8_t maybe_set_rtc_time() {
  if (!eeprom_read_byte(&kShouldSet)) {
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
  set_rtc_alarm(wake_hour, wake_minute);

  flicker_leds(1);
}

static int16_t calculate_minutes_until_wake() {
  int16_t now = hour * 60 + minute;
  int16_t wake_time = wake_hour * 60 + wake_minute;
  return now - wake_time;
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

  // We've woken up. Turn back on any peripherals we need while
  // running.
  init_power_reduction_register(0);
  init_rtc();

  // If we woke up and the '8523's SF and AF bits are clear, then it's
  // a safe guess the button press (as opposed to the '8523's /INT1
  // going active).
  _was_button_pressed = !clear_rtc_interrupt_flags();
  if (!_was_button_pressed) {
    // /INT1 on the '8523 must have gone low. Wait for it to come
    // back.
    while (is_button_pressed())
      ;
  }

  // Now wait for the rising edge to pass.
  refresh_time(&hour);
}

static void handle_NIGHT() {
}

static void handle_TWILIGHT(int16_t minutes_left) {
  if (minutes_left <= 3) {
    flicker_quiet(2);
    flicker_wake(1);
  } else {
    flicker_quiet(1);
  }
}

static void handle_DAWN(int16_t minutes_elapsed) {
  breathe_wake();
}

static void do_i2c_diagnostics() {
  while (!is_rtc_connected()) {
    flicker_leds(4);
    _delay_ms(500);
  }

  while (!rtc_write_verify()) {
    flicker_quiet(2);
    _delay_ms(500);
  }

  flicker_wake(4);
}

int main(void) {
  init_ports();
  init_rtc();
  stop_32768_clkout();
  enable_pin_interrupts();
  do_i2c_diagnostics();

  if (maybe_set_rtc_time()) {
    flicker_leds(8);
  }

  read_wake_time();
  set_rtc_alarm(wake_hour, wake_minute);
  set_second_interrupt(0);

  sei();

  uint8_t state = STATE_INIT;
  while (1) {
    if (was_button_pressed()) {
      set_wake_time();
      continue;
    }

    int16_t minutes_until_wake = calculate_minutes_until_wake();
    uint8_t in_pre_window =
        (minutes_until_wake <= WAKE_WINDOW_PRE_MINUTES &&
         minutes_until_wake > 0);
    uint8_t in_post_window =
        (minutes_until_wake >= -WAKE_WINDOW_POST_MINUTES &&
         minutes_until_wake <= 0);

    if (!in_pre_window && !in_post_window && state != STATE_NIGHT) {
      state = start_NIGHT();
    }
    if (in_pre_window && state != STATE_TWILIGHT) {
      state = start_TWILIGHT();
    }
    if (in_post_window && state != STATE_DAWN) {
      state = start_DAWN();
    }
    switch (state) {
      case STATE_NIGHT:
        handle_NIGHT();
        break;
      case STATE_TWILIGHT:
        handle_TWILIGHT(minutes_until_wake);
        break;
      case STATE_DAWN:
        handle_DAWN(-minutes_until_wake);
        break;
    }

    // We're in the right state. Power down, and we'll be woken up by
    // /INT. This function will return when we wake up again.
    power_down();
  }
}
