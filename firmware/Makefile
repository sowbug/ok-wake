TARGET=ok-wake
MCU=attiny25
AVRDUDE_TARGET=t25
F_CPU=1000000UL

UISP=avrdude
PROGRAMMER=avrisp2
PORT=usb
PROGRAMMER_ARGS=-c $(PROGRAMMER) -P $(PORT) -p $(AVRDUDE_TARGET)

M_SECONDS=$(shell date +%S)
M_MINUTES=$(shell date +%M)
M_HOURS=$(shell date +%H)
M_DAYS=$(shell date +%d)
M_WEEKDAYS=$(shell date +%w)
M_MONTHS=$(shell date +%m)
M_YEARS=$(shell date +%y)

CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -std=c99 -pedantic -Wundef \
	-funsigned-char -funsigned-bitfields -fpack-struct \
	-fshort-enums -ffunction-sections \
	--combine -fwhole-program -fno-tree-scev-cprop -fno-split-wide-types \
	-mmcu=$(MCU) \
	-DF_CPU=$(F_CPU) \
	-DM_SECONDS=0x$(M_SECONDS) \
	-DM_MINUTES=0x$(M_MINUTES) \
	-DM_HOURS=0x$(M_HOURS) \
	-DM_DAYS=0x$(M_DAYS) \
	-DM_WEEKDAYS=0x$(M_WEEKDAYS) \
	-DM_MONTHS=0x$(M_MONTHS) \
	-DM_YEARS=0x$(M_YEARS)
OBJ2HEX=avr-objcopy

SOURCES= \
	attinyx5.c \
	bcd.c \
	i2cmaster.S \
	main.c \
	pcf8523.c

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

# http://www.frank-zhao.com/fusecalc/fusecalc.php?chip=attiny25&LOW=62&HIGH=D6
fuse-t25:
	sudo $(UISP)  $(PROGRAMMER_ARGS) \
	-e \
	-U lfuse:w:0x62:m \
	-U hfuse:w:0xD6:m

test: test.c bcd.c
	gcc -g -o $@.out test.c bcd.c
	./$@.out

clean:
	rm -f *.hex *.eep *.obj *.o *.out
