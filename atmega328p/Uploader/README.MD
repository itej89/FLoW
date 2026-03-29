# Controller Uploader

An UART to I2C serial gateway firmware for Arduino-based bootloader integration with ATmega328 microcontrollers.

## Features

- **Serial-to-I2C Bridge**: Seamless UART to I2C communication protocol conversion
- **Reliable Checksummed Protocol**: XOR-based packet validation for data integrity
- **Python Upload Utility**: Command-line tool for intuitive hex file uploads
- **Comprehensive Error Reporting**: Detailed status codes for debugging and troubleshooting
- **Cross-Platform Support**: Linux, macOS, and Windows compatible

## System Architecture

```
PC ----UART----> Arduino Gateway ----I2C----> ATmega328 Target Bootloader
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

For uploading to the Arduino:
```bash
platformio run --target upload
```

## Installation

1. Configure your Arduino board and COM port in `platformio.ini`
2. Build and upload the firmware to your Arduino device

## Usage

### Command-Line Upload

```bash
python3 ./tools/uploader.py <hex_file> -p <port> --device <address>
```

**Parameters:**
- `<hex_file>`: Path to the Intel HEX firmware file to upload
- `-p <port>`: Serial port (e.g., `/dev/ttyACM0`, `COM3`)
- `--device <address>`: I2C address of target device (hexadecimal)

**Example:**
```bash
python3 ./tools/uploader.py ./test/example_programs/uart_echo/uart_echo.hex -p /dev/ttyACM0 --device 0x42
```

## Bootloader Trigger (uart_echo)

The `uart_echo` example supports requesting bootloader entry by sending the ASCII command `B` over UART.

Behavior in `test/example_programs/uart_echo/uart_echo.c`:
- UART RX ISR captures received byte: `c = UDR0`.
- If `c == 'B'`, sets `boot_request = 1` and returns immediately.
- Main loop detects `boot_request` and prints `BOOT\r\n`.
- It waits for TX queue flush and UART shift register empty.
- It calls `enter_bootloader_via_wdt()`:
  - Writes `BOOT_MAGIC` (`0xB007`) to RAM location `0x08FA`.
  - Disables UART and timers.
  - Enables watchdog reset (`WDTO_15MS`) and spins forever.
- On reset, bootloader checks the magic key and enters bootloader mode.

**Usage:**
1. Build and upload firmware as usual.
2. Open serial terminal at `115200 8N1`.
3. Send `B` (single character) to request bootloader.
4. Arduino responds `BOOT` and resets into bootloader mode.

## Communication Protocol

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

## Example Programs

Test programs are provided in the `test/example_programs/` directory to verify system functionality:

### LED Blink (`blink/`)

A simple LED blink program that toggles pin PB5 (Arduino Pin 13) at 5 Hz.

- **Purpose**: Basic functionality test to verify device programming and execution
- **Build**: `cd test/example_programs/blink && make`
- **Usage**: Upload the generated `blink.hex` via the uploader utility

**Example:**
```bash
python3 ./tools/uploader.py ./test/example_programs/blink/blink.hex -p /dev/ttyACM0 --device 0x42
```

### UART Echo (`uart_echo/`)

A comprehensive UART communication program with bootloader integration capabilities.

- **Purpose**: Advanced testing of UART communication, bootloader entry, and device control sequences
- **Features**:
  - UART communication handler with buffered TX queue
  - Bootloader entry via watchdog timer (WDT)
  - Boot magic key validation for safe bootloader transitions
- **Build**: `cd test/example_programs/uart_echo && make`
- **Usage**: Upload the generated `uart_echo.hex` via the uploader utility

**Example:**
```bash
python3 ./tools/uploader.py ./test/example_programs/uart_echo/uart_echo.hex -p /dev/ttyACM0 --device 0x42
```

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
├── tools/            # Upload utility and auxiliary tools
└── test/             # Test programs and examples
```

**License**
-----------

This repository is licensed under the [GNU GENERAL PUBLIC LICENSE]
