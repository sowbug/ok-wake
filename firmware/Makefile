TARGET=ok-wake
MCU=attiny25
AVRDUDE_TARGET=t25
F_CPU=1000000UL

UISP=avrdude
PROGRAMMER=avrisp2
PORT=usb
PROGRAMMER_ARGS=-c $(PROGRAMMER) -P $(PORT) -p $(AVRDUDE_TARGET)

M_YEAR=$(shell date +%y)
M_MONTH=$(shell date +%m)
M_DAY=$(shell date +%d)
M_HOUR=$(shell date +%H)
M_MINUTE=$(shell date +%M)
M_SECOND=$(shell date +%S)

CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -std=c99 -pedantic -Wundef \
	-mmcu=$(MCU) \
	-DF_CPU=$(F_CPU) \
	-DM_YEAR=0x$(M_YEAR) \
	-DM_MONTH=0x$(M_MONTH) \
	-DM_DAY=0x$(M_DAY) \
	-DM_HOUR=0x$(M_HOUR) \
	-DM_MINUTE=0x$(M_MINUTE) \
  -DM_SECOND=0x$(M_SECOND)
OBJ2HEX=avr-objcopy

SOURCES=main.c pcf8523.c USI_TWI_Master.c attinyx5.c bcd.c

program: $(TARGET).hex
	$(UISP) $(PROGRAMMER_ARGS) \
	  -U flash:w:$(TARGET).hex

load-eeprom: eeprom
	$(UISP) $(PROGRAMMER_ARGS) \
	  -U eeprom:w:$(TARGET).eep

eeprom: $(TARGET).eep

$(TARGET).out: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET).out $(SOURCES)

%.hex: $(TARGET).out
	$(OBJ2HEX) -j .text -j .data -O ihex $< $@

%.eep: $(TARGET).out
	$(OBJ2HEX) -j .eeprom -O ihex $< $@

prod: CFLAGS += -DPRODUCTION=1
prod: clean program load-eeprom

uno: MCU=atmega328
uno: AVRDUDE_TARGET=m328p
uno: F_CPU=16000000UL
uno: PROGRAMMER=arduino
uno: PORT=/dev/tty.usbmodem411
uno: CFLAGS += -DNO_USI_TWI=1
uno: clean program eeprom

# http://www.frank-zhao.com/fusecalc/fusecalc.php?chip=attiny25&LOW=62&HIGH=D6
fuse-t25:
	sudo $(UISP)  $(PROGRAMMER_ARGS) \
    -e \
	  -U lfuse:w:0x62:m \
		-U hfuse:w:0xD6:m

clean:
	rm -f *.hex *.eep *.obj *.o *.out
