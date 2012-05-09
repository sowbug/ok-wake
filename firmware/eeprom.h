/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

#if !defined(__EEPROM_H__)
#define __EEPROM_H__

#include <avr/eeprom.h>

// Various sources say that a voltage problem can corrupt EEPROM
// address 0. So we'll block it off and ignore it.
uint8_t EEMEM kNeverUse = 0x55;

// These values will determine the contents of the .eep file, which
// can be used to program the EEPROM.
uint8_t EEMEM kShouldSet = 0x01;

uint8_t EEMEM kSeconds = M_SECONDS;
uint8_t EEMEM kMinutes = M_MINUTES;
uint8_t EEMEM kHours = M_HOURS;
uint8_t EEMEM kDays = M_DAYS;
uint8_t EEMEM kWeekdays = M_WEEKDAYS;
uint8_t EEMEM kMonths = M_MONTHS;
uint8_t EEMEM kYears = M_YEARS;

uint16_t EEMEM kWakeTime = 0x0600;

#endif  // guard
