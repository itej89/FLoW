# Controller Uploader

An UART to I2C serial bridge firmware for Arduino-based bootloader integration with ATmega328 microcontrollers.

## Features

- **Serial-to-I2C Bridge**: UART to I2C communication protocol conversion
- **Python Upload Utility**: Command-line tool for intuitive hex file uploads

## System Architecture

```
PC ----UART----> Arduino Bridge ----I2C----> ATmega328 Target Bootloader
```

The Arduino acts as a pass-through gateway, translating UART commands from the PC into I2C transactions targeting bootloader operations on the ATmega328 microcontroller.

## Prerequisites

- **Hardware**: Arduino board with ATmega microcontroller, USB connection
- **Software**: 
  - Python 3.6+
  - PlatformIO (for firmware compilation)
  - Standard AVR toolchain (via PlatformIO)

## Building the Firmware

```bash
cd /path/to/Uploader
platformio run
```

For uploading to the Arduino acting as Bridge:
```bash
platformio run --target upload
```

## Installation

1. Configure your Arduino board and COM port in `platformio.ini`
2. Build and upload the firmware to your Arduino device

## Usage

### Command-Line Upload

```bash
python3 ./tools/uploader.py <hex_file> -p <port> --device <address> [--factory]
```

**Parameters:**
- `<hex_file>`: Path to the Intel HEX firmware file to upload
- `-p <port>`: Serial port (e.g., `/dev/ttyACM0`, `COM3`)
- `--device <address>`: I2C address of target device (hexadecimal)
- `--factory`: Factory mode: no application to switch from. Use this the first time when Target Arduino Bootloader is re-flashed

**Example:**
```bash
python3 ./tools/uploader.py -p /dev/ttyACM0 -d 0x42 ./test/example_programs/wire_app_template/.pio/build/pro16MHzatmega328/firmware.hex
```

## Application Development Template

The `wire_app_template` in `test/example_programs/` serves as the application development template. It demonstrates application-to-bootloader switching functionality:

- **Bootloader Trigger**: Send I2C command to switch the target device into bootloader mode
- **Application Mode**: Normal operation with I2C communication
- **Seamless Switching**: Application can request bootloader entry for firmware updates

**Usage:**
1. Build the template: `cd test/example_programs/wire_app_template && platformio run`
2. Upload via uploader: Use the example command above
3. The application supports bootloader mode switching via I2C commands

### UART Packet Format

**PC to Arduino (Command):**
```
[0xA5] [CMD] [LEN] [DATA...] [CHK]
```
where `CHK = CMD ^ LEN ^ all DATA bytes`

**Arduino to PC (Response):**
```
[0x5A] [STATUS] [ACK] [CHK]
```
where `CHK = STATUS ^ ACK`

### Status Codes

| Code | Description |
|------|-------------|
| 0x00 | Success |
| 0x01 | Bad UART checksum |
| 0x02 | UART framing or length error |
| 0x10 | I2C START condition failed |
| 0x11 | I2C SLA+W (slave address + write) failed |
| 0x12 | I2C data write failed |
| 0x13 | I2C repeated START or read START failed |
| 0x14 | I2C SLA+R (slave address + read) failed |

## Troubleshooting

### Upload Fails with Status 0x01 or 0x02
- **Issue**: Checksum or framing error
- **Solution**: Check serial port baud rate matches firmware (typically 115200), verify USB cable connection

### Upload Fails with Status 0x10–0x14
- **Issue**: I2C communication problem
- **Solution**: Verify target device I2C address, check pull-up resistors on SDA/SCL lines, ensure target device is powered and accessible

### Serial Port Not Found
- **Issue**: Arduino not detected
- **Solution**: Check USB connection, verify board drivers installed, confirm correct serial port in command

## Project Structure

```
.
├── include/          # Header files
├── src/              # Firmware source code
├── tools/            # Upload utility
└── test/             # Application Template Program
```

**License**
-----------

This repository is licensed under the [GNU GENERAL PUBLIC LICENSE]
