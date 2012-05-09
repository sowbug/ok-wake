/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#if !defined(__EEPROM_H__)
#define __EEPROM_H__

#include <avr/eeprom.h>

// These values will determine the contents of the .eep file, which
// can be used to program the EEPROM.

// Various sources say that a voltage problem can corrupt EEPROM
// address 0. So we'll block it off and ignore it.
__attribute__((used)) uint8_t EEMEM kNeverUse = 0x55;

// Whether the EEPROM contains values that need to be written to the RTC.
__attribute__((used)) uint8_t EEMEM kShouldSetRTC = 0x01;

// Registers 03-09 of the PFC8523.
__attribute__((used)) uint8_t EEMEM kRTCTimeCircuits[7] = {
  M_SECONDS, M_MINUTES, M_HOURS, M_DAYS, M_WEEKDAYS, M_MONTHS, M_YEARS };

__attribute__((used)) uint16_t EEMEM kWakeTime = 0x0600;

#endif  // guard
