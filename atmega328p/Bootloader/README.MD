# I2C Bootloader for Controllers

## Overview

This repository contains I2C bootloader implementations for AtMega328p, providing a reliable way to update firmware over the I2C interface.

## Supported Controllers

### ATMega328p

#### Source Files
- `src/main.c`
- `include/flash_driver.h`
- `include/i2c_driver.h`
- `include/programmer.h`
- `include/timing.h`
- `include/uart_debug.h`

#### Prerequisites

Install the required tools using the following commands:

```bash
sudo apt-get install gcc-avr binutils-avr avr-libc avrdude
sudo apt install avrdude
```

**Hardware Requirements:**
- A USB ISP Programmer (e.g., USBasp) is required to flash the bootloader to the ATmega328P microcontroller.

#### Building the Bootloader

To build the bootloader, run:

```bash
make all
```

#### Reading Fuse Bits

Before flashing, read the current fuse bits to verify the bootloader size. The existing makefile is configured for 2KB of Bootloader Region

```bash
avrdude -c usbasp -p atmega328p -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h
```

#### Flashing the Bootloader

To upload the bootloader to the device:

```bash
./flash.sh
```

**License**
-----------

This repository is licensed under the [GNU GENERAL PUBLIC LICENSE]

**Acknowledgments**
------------------

* Thanks for optimboot for flash_driver implementation
