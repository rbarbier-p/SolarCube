MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
PROGRAMMER = arduino
PORT = /dev/ttyUSB0
BAUD = 115200
SIZE = avr-size
SRAM = 2048    # ATmega328P RAM in bytes
FLASH = 32768  # ATmega328P Flash in bytes

BUILD_DIR = .build

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Iinc
CFLAGS += -DYEAR=$(shell date +%y)
CFLAGS += -DMONTH=$(shell date +%m)
CFLAGS += -DDAY=$(shell date +%d)
CFLAGS += -DHOUR=$(shell date +%H)
CFLAGS += -DMIN=$(shell date +%M)
CFLAGS += -DSEC=$(shell date +%S)

# Source files
SRC_DIR = src
SRC_FILES = main.c cube_control.c mySoftI2C.c myDS1307.c UART.c animations.c times.c
SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Output files in build folder
ELF = $(BUILD_DIR)/program.elf
HEX = $(BUILD_DIR)/program.hex
SIZE_TXT = $(BUILD_DIR)/size.txt

# Default target
all: flash

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile ELF from sources
$(ELF): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRC)

# Generate HEX from ELF
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

# Flash target: generates HEX if needed, flashes, and prints memory usage
flash: $(HEX)
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(HEX):i
	@echo "\033[0;34m-------  Memory usage  --------\033[0m"
	@$(MAKE) -s size
	@echo "\n"

# Print flash and RAM usage only (no Program/Data lines)
size: $(ELF) | $(BUILD_DIR)
	@$(SIZE) --format=avr --mcu=$(MCU) $(ELF) > $(SIZE_TXT)
	@awk -v flash_total=$(FLASH) -v sram_total=$(SRAM) ' \
	/Program:/ {match($$0,/([0-9]+) bytes/,a); flash=a[1]+0} \
	/Data:/ {match($$0,/([0-9]+) bytes/,a); ram=a[1]+0} \
	END {printf "Flash usage: \033[0;33m%d\033[0m bytes of %d KB (%.1f%%)\nRAM usage: \033[0;33m%d\033[0m bytes of %d KB (%.1f%%)\n", \
	flash, flash_total/1024, flash/flash_total*100, ram, sram_total/1024, ram/sram_total*100}' $(SIZE_TXT)

screen: all
	screen $(PORT) 115200
	
# Clean build files
clean:
	rm -rf $(BUILD_DIR)

re: clean all

# Declare phony targets
.PHONY: all flash size clean

