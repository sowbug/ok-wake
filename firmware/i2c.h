/*
 * OK Wake
 * https://github.com/sowbug/ok-wake/
 *
 * Copyright (c) 2012 Mike Tsao.
 */

void write_i2c_byte(uint8_t addr, uint8_t reg, uint8_t data);

uint8_t read_i2c_byte(uint8_t addr, uint8_t reg);
