/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

// Build/run: gcc test.c && ./a.out

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bcd.h"

static void test_bcd() {
  // Calculate the number of minutes between two BCD times.
  {
    const uint16_t a = 0x830;
    const uint16_t b = 0x930;
    assert(subtract_bcd_time_in_minutes(a, b) == -60);
    assert(subtract_bcd_time_in_minutes(b, a) == 60);
    assert(subtract_bcd_time_in_minutes(a, a) == 0);
  }
  {
    const uint16_t a = 0x2330;
    const uint16_t b = 0x0;
    assert(subtract_bcd_time_in_minutes(a, b) == 1410);
    assert(subtract_bcd_time_in_minutes(b, a) == -1410);
    assert(smart_time_until_alarm(a, b) == -30);
    assert(smart_time_until_alarm(b, a) == 30);
  }
  {
    const uint16_t a = 0x0001;
    const uint16_t b = 0x2359;

    assert(subtract_bcd_time_in_minutes(a, b) == -1438);
    assert(subtract_bcd_time_in_minutes(b, a) == 1438);
    assert(smart_time_until_alarm(a, b) == 2);
    assert(smart_time_until_alarm(b, a) == -2);
  }

  assert(add_minutes_to_bcd_time(0x1130, 1) == 0x1131);
  assert(add_minutes_to_bcd_time(0x1817, 720) == 0x0617);
  assert(add_minutes_to_bcd_time(0x1130, -1) == 0x1129);
  assert(add_minutes_to_bcd_time(0x2359, 1) == 0x0000);
  assert(add_minutes_to_bcd_time(0x0000, -1) == 0x2359);
}

int main() {
  test_bcd();
  return 0;
}
