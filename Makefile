MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
PROGRAMMER = arduino
PORT = /dev/ttyUSB0
BAUD = 115200

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Iinc

CFLAGS += -DYEAR=$(shell date +%y)
CFLAGS += -DMONTH=$(shell date +%m)
CFLAGS += -DDAY=$(shell date +%d)
CFLAGS += -DHOUR=$(shell date +%H)
CFLAGS += -DMIN=$(shell date +%M)
CFLAGS += -DSEC=$(shell date +%S)

SRC_DIR = src

SRC_FILES = main.c mySoftI2C.c myDS1307.c UART.c
SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
BIN = program.bin
HEX = program.hex

all: hex flash

hex:
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)
	$(OBJCOPY) -O ihex $(BIN) $(HEX)

flash:
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(HEX):i

clean:
	rm -f $(BIN) $(HEX)

.PHONY: all hex flash clean
