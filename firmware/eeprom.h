/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 */

#include <avr/eeprom.h>

// Various sources say that a voltage problem can corrupt EEPROM address 0.
// So we'll block it off and ignore it.
uint32_t EEMEM kNeverUse = 0x55;

// These values will determine the contents of the .eep file, which can be
// used to program the EEPROM.
uint8_t EEMEM kShouldSet = 0x01;
uint8_t EEMEM kYear = 12;
uint8_t EEMEM kMonth = 5;
uint8_t EEMEM kDay = 15;
uint8_t EEMEM kHour = 12;
uint8_t EEMEM kMinute = 13;
uint8_t EEMEM kWakeHour = 14;
uint8_t EEMEM kWakeMinute = 15;

