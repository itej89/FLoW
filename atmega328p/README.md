# FLoW - ATMega328p

## Overview

This project provides a custom I2C bootloader for the ATmega328P microcontroller, enabling firmware updates over the I2C interface. It includes both the bootloader firmware that runs on the target device and a host-side uploader tool for flashing programs.

The system consists of two main components:
- **Bootloader**: Firmware that resides in the bootloader section of the ATmega328P, handling I2C communication for programming operations.
- **Uploader**: An Arduino-based gateway that bridges UART commands from a PC to I2C transactions with the target bootloader.

## Features

- **I2C-Based Programming**: Reliable firmware updates via I2C interface
- **UART to I2C Gateway**: Arduino acts as a serial-to-I2C bridge
- **Checksummed Protocol**: XOR-based packet validation for data integrity
- **Python Upload Utility**: Command-line tool for uploading hex files
- **Example Programs**: Test programs like blink and UART echo included

## System Architecture

### Bootloader Flashing

```
PC ----USBISP----> ATmega328P
```

### Programming

```
PC ----UART----> Arduino Gateway ----I2C----> ATmega328P Bootloader
```

## Prerequisites

- AVR toolchain (gcc-avr, binutils-avr, avr-libc, avrdude)
- Python 3.6+
- PlatformIO for building the uploader firmware
- Arduino board for the gateway

## Getting Started

1. **Build the Bootloader**:
   - Navigate to `atmega328p/Bootloader/`
   - Run `make all`
   - Flash using `./flash.sh`

2. **Build and Upload the Uploader Firmware**:
   - Navigate to `atmega328p/Uploader/`
   - Run `platformio run --target upload`

3. **Upload a Program**:
   - Use the Python uploader: `python3 ./tools/uploader.py <hex_file> -p <port> --device <address>`

For detailed instructions, see the README files in each component directory.

**License**
-----------

This repository is licensed under the [GNU GENERAL PUBLIC LICENSE]
