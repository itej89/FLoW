MCU   = atmega328p
F_CPU = 16000000UL

CC        = avr-gcc
OBJCOPY   = avr-objcopy
SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build

SRC = $(wildcard $(SRC_DIR)/*.c)

ELF    = $(BUILD_DIR)/blAT328pTWI.elf
TARGET = $(BUILD_DIR)/blAT328pTWI.hex

CFLAGS  = -g -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -ffunction-sections -fdata-sections
# CFLAGS += -DDEBUG_UART
CFLAGS += -I$(INC_DIR)
LDFLAGS = -Wl,--section-start=.text=0x7800 -Wl,-gc-sections  # 0x7800 for 2KB Bootloader
# Example: for 1KB bootloader, .text start = 0x7C00; for 512B = 0x7E00; adjust for your fuses/bootsize.

all: $(TARGET)

$(TARGET): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(ELF): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean