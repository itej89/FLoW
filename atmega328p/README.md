# FLoW - ATMega328p

## Overview

This project provides a custom I2C bootloader for the ATmega328P microcontroller, enabling firmware updates over the I2C interface. It includes both the bootloader firmware that runs on the target device and a host-side uploader tool for flashing programs.

The system consists of two main components:
- **Bootloader**: Firmware that resides in the bootloader section of the ATmega328P, handling I2C communication for programming operations.
- **Uploader**: An Arduino-based gateway that bridges UART commands from a PC to I2C transactions with the target bootloader.

## Features

- **I2C-Based Programming**: Reliable firmware updates via I2C interface
- **UART to I2C Gateway**: Arduino acts as a serial-to-I2C bridge
- **Python Upload Utility**: Command-line tool for uploading hex files
- **Application Development Template**: `wire_app_template` for building applications with bootloader integration

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
   - Use the Python uploader: `python3 ./tools/uploader.py <hex_file> -p <port> --device <address> [--factory]`

For detailed instructions, see the README files in each component directory.

## Application Development

The `wire_app_template` in `Uploader/test/example_programs/` serves as the primary template for developing applications that integrate with the bootloader system. This template demonstrates:

- **I2C Communication**: Basic I2C slave functionality for application-level communication
- **Bootloader Integration**: Seamless switching from application mode to bootloader mode
- **PlatformIO Build System**: Ready-to-use configuration for building and uploading applications

### Using the Template

1. **Copy the Template**:
   ```bash
   cp -r Uploader/test/example_programs/wire_app_template/ my_app/
   cd my_app/
   ```

2. **Modify the Code**:
   - Edit `src/main.cpp` to implement your application logic
   - Update `platformio.ini` if needed for different board configurations

3. **Build and Upload**:
   ```bash
   platformio run
   python3 ../Uploader/tools/uploader.py .pio/build/pro16MHzatmega328/firmware.hex -p /dev/ttyACM0 --device 0x42 
   ```

The template includes example code for triggering bootloader mode from the application, allowing for over-the-air updates and firmware management.

**License**
-----------

This repository is licensed under the [GNU GENERAL PUBLIC LICENSE]
