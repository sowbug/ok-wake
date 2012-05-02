TARGET=ok-wake
MCU=attiny25
AVRDUDE_TARGET=t25
F_CPU=1200000UL

UISP=avrdude
PROGRAMMER=avrisp2
PORT=usb
PROGRAMMER_ARGS=-c $(PROGRAMMER) -P $(PORT) -p $(AVRDUDE_TARGET) -v

CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -std=c99 -pedantic -Wundef \
	-mmcu=$(MCU) \
	-DF_CPU=$(F_CPU)
OBJ2HEX=avr-objcopy

program : $(TARGET).hex
	sudo $(UISP)  $(PROGRAMMER_ARGS) \
	  -U flash:w:$(TARGET).hex

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : main.obj
	$(OBJ2HEX) -j .text -O ihex $< $@

prod: CFLAGS += -DPRODUCTION=1
prod: clean program

fuse:
	sudo $(UISP)  $(PROGRAMMER_ARGS) \
    -e \
	  -U lfuse:w:0x6A:m \
		-U hfuse:w:0xDF:m

clean :
	rm -f *.hex *.obj *.o a.out
