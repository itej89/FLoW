MCU = atmega328p
F_CPU = 16000000UL
TARGET = uart_echo

CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

CFLAGS = -Wall -Wextra -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)

all: $(TARGET).hex

$(TARGET).elf: $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $<
	$(SIZE) $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -f *.elf *.hex