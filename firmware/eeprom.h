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
uint8_t EEMEM kYear = M_YEAR;
uint8_t EEMEM kMonth = M_MONTH;
uint8_t EEMEM kDay = M_DAY;
uint8_t EEMEM kHour = M_HOUR;
uint8_t EEMEM kMinute = M_MINUTE;
uint8_t EEMEM kSecond = M_SECOND;
uint8_t EEMEM kWakeHour = 0x06;
uint8_t EEMEM kWakeMinute = 0x00;
uint8_t EEMEM kAlarmHour = 0x05;  // TODO: these are temporary until I figure out BCD/time math
uint8_t EEMEM kAlarmMinute = 0x30;

#endif  // guard
