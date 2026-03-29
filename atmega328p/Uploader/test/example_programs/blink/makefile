MCU = atmega328p
F_CPU = 16000000UL
BLINK_TARGET = blink

CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -g -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os

all: $(BLINK_TARGET).hex

$(BLINK_TARGET).elf: blink.c
	$(CC) $(CFLAGS) -o $@ blink.c $(LDFLAGS)

$(BLINK_TARGET).hex: $(BLINK_TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -f *.elf *.hex
