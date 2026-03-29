#!/usr/bin/env python3
import argparse
import sys
import time
from pathlib import Path

import serial


# Bootloader commands
PRG_INT_BEGIN = 0x01
PRG_INT_SET_PAGE = 0x02
PRG_INT_WRITE_PAGE = 0x03
PRG_INT_EXIT = 0x05

# Bootloader ACKs
PRG_INT_BEGIN_ACK = 0xAA
PRG_INT_SET_PAGE_ACK = 0xAB
PRG_INT_WRITE_PAGE_ACK = 0xAC
PRG_INT_EXIT_ACK = 0xEE

# UART framing with the master Arduino
UART_SOF = 0xA5
UART_RESP_SOF = 0x5A

FLASH_PAGE_SIZE = 128
DEFAULT_BAUD = 115200


def parse_args():
    p = argparse.ArgumentParser(description="Upload AVR Intel HEX to controller through Arduino UART->I2C bridge")
    p.add_argument("hexfile", type=Path, help="Input Intel HEX file")
    p.add_argument("-d", "--device", required=True, help="Target Device I2C Address in hex")
    p.add_argument("-p", "--port", required=True, help="Serial port, e.g. COM5 or /dev/ttyUSB0")
    p.add_argument("-b", "--baud", type=int, default=DEFAULT_BAUD, help=f"Serial baud rate (default: {DEFAULT_BAUD})")
    p.add_argument("--page-size", type=int, default=FLASH_PAGE_SIZE, help="Flash page size in bytes")
    p.add_argument("--timeout", type=float, default=2.0, help="Serial timeout in seconds")
    p.add_argument("--retries", type=int, default=3, help="Retries per command")
    p.add_argument("--inter-frame-delay", type=float, default=0.01, help="Delay after each UART frame")
    return p.parse_args()


def parse_intel_hex(path: Path):
    """
    Parse Intel HEX into {byte_address: byte_value}.
    Supports:
      00 = data
      01 = EOF
      04 = extended linear address
    """
    memory = {}
    upper = 0

    with path.open("r", encoding="ascii") as f:
        for lineno, raw in enumerate(f, start=1):
            line = raw.strip()
            if not line:
                continue
            if not line.startswith(":"):
                raise ValueError(f"{path}:{lineno}: missing ':'")

            try:
                rec = bytes.fromhex(line[1:])
            except ValueError:
                raise ValueError(f"{path}:{lineno}: invalid hex")

            if len(rec) < 5:
                raise ValueError(f"{path}:{lineno}: too short")

            count = rec[0]
            addr = (rec[1] << 8) | rec[2]
            rectype = rec[3]
            data = rec[4:4 + count]
            cksum = rec[4 + count]

            total = (sum(rec[:-1]) + cksum) & 0xFF
            if total != 0:
                raise ValueError(f"{path}:{lineno}: checksum error")

            if rectype == 0x00:
                base = (upper << 16) | addr
                for i, b in enumerate(data):
                    memory[base + i] = b
            elif rectype == 0x01:
                break
            elif rectype == 0x04:
                if count != 2:
                    raise ValueError(f"{path}:{lineno}: invalid extended linear address record")
                upper = (data[0] << 8) | data[1]
            else:
                # Ignore other record types
                pass

    return memory


def build_pages(memory: dict, page_size: int):
    """
    Turn sparse byte memory into full flash pages padded with 0xFF.
    Returns list of (page_start_byte_addr, page_bytes).
    """
    if not memory:
        return []

    min_addr = min(memory.keys())
    max_addr = max(memory.keys())

    first_page = (min_addr // page_size) * page_size
    last_page = (max_addr // page_size) * page_size

    pages = []
    for page_start in range(first_page, last_page + 1, page_size):
        page = bytearray([0xFF] * page_size)
        used = False
        for i in range(page_size):
            a = page_start + i
            if a in memory:
                page[i] = memory[a]
                used = True
        if used:
            pages.append((page_start, bytes(page)))
    return pages


def xor_checksum(device_addr:int, cmd: int, length: int, data: bytes) -> int:
    x = device_addr ^ cmd ^ length
    for b in data:
        x ^= b
    return x & 0xFF


def send_uart_packet(ser: serial.Serial, device_addr: int, cmd: int, data: bytes, expected_ack: int, retries: int = 3, inter_frame_delay: float = 0.01):
    if len(data) > 255:
        raise ValueError("UART payload too large")

    length = len(data)
    chk = xor_checksum(device_addr, cmd, length, data)
    pkt = bytes([UART_SOF, device_addr, cmd, length]) + data + bytes([chk])

    last_err = None

    for attempt in range(1, retries + 1):
        ser.reset_input_buffer()
        ser.write(pkt)
        ser.flush()
        time.sleep(inter_frame_delay)

        try:
            hdr = ser.read(1)
            if len(hdr) != 1:
                raise RuntimeError("timeout waiting for response SOF")
            if hdr[0] != UART_RESP_SOF:
                raise RuntimeError(f"bad response SOF: 0x{hdr[0]:02X}")

            tail = ser.read(3)
            if len(tail) != 3:
                raise RuntimeError("short response")

            status, ack, rchk = tail
            if ((status ^ ack) & 0xFF) != rchk:
                raise RuntimeError("response checksum mismatch")

            if status != 0x00:
                raise RuntimeError(f"master transport error: status=0x{status:02X}")

            if ack != expected_ack:
                raise RuntimeError(f"unexpected bootloader ACK: got 0x{ack:02X}, expected 0x{expected_ack:02X}")

            return ack

        except Exception as e:
            last_err = e
            time.sleep(0.05)

    raise RuntimeError(str(last_err))


def program_device(ser: serial.Serial, device_addr, pages, retries: int, inter_frame_delay: float):
    print("BEGIN")
    send_uart_packet(
        ser,
        device_addr,
        PRG_INT_BEGIN,
        b"", 
        PRG_INT_BEGIN_ACK,
        retries=retries,
        inter_frame_delay=inter_frame_delay,
    )

    for idx, (page_start_byte_addr, page_data) in enumerate(pages, start=1):
        if page_start_byte_addr & 0x01:
            raise RuntimeError(f"page start address is not word-aligned: 0x{page_start_byte_addr:06X}")

        word_addr = page_start_byte_addr // 2
        if word_addr > 0xFFFFFF:
            raise RuntimeError("word address exceeds 24-bit protocol field")

        addr_payload = bytes([
            (word_addr >> 16) & 0xFF,
            (word_addr >> 8) & 0xFF,
            word_addr & 0xFF,
        ])

        print(f"SET_PAGE  {idx}/{len(pages)}  byte=0x{page_start_byte_addr:06X}  word=0x{word_addr:06X}")
        send_uart_packet(
            ser,
            device_addr,
            PRG_INT_SET_PAGE,
            addr_payload,
            PRG_INT_SET_PAGE_ACK,
            retries=retries,
            inter_frame_delay=inter_frame_delay,
        )

        print(f"WRITE_PAGE {idx}/{len(pages)}  {len(page_data)} bytes")
        send_uart_packet(
            ser,
            device_addr,
            PRG_INT_WRITE_PAGE,
            page_data,
            PRG_INT_WRITE_PAGE_ACK,
            retries=retries,
            inter_frame_delay=inter_frame_delay,
        )

    print("EXIT")
    send_uart_packet(
        ser,
        device_addr,
        PRG_INT_EXIT,
        b"",
        PRG_INT_EXIT_ACK,
        retries=retries,
        inter_frame_delay=inter_frame_delay,
    )

    print("Upload complete.")


def main():
    args = parse_args()

    if not args.hexfile.exists():
        print(f"HEX file not found: {args.hexfile}", file=sys.stderr)
        sys.exit(1)

    memory = parse_intel_hex(args.hexfile)
    pages = build_pages(memory, args.page_size)

    if not pages:
        print("No data pages found in HEX.", file=sys.stderr)
        sys.exit(1)

    print(f"Parsed {len(memory)} programmed bytes")
    print(f"Prepared {len(pages)} flash pages of {args.page_size} bytes")

    try:
        with serial.Serial(args.port, args.baud, timeout=args.timeout) as ser:
            # Give Arduino time after DTR reset
            time.sleep(2.0)
            ser.reset_input_buffer()
            ser.reset_output_buffer()

            device_addr = int(args.device, 16)
            program_device(
                ser,
                device_addr, 
                pages,
                retries=args.retries,
                inter_frame_delay=args.inter_frame_delay,
            )

    except serial.SerialException as e:
        print(f"Serial error: {e}", file=sys.stderr)
        sys.exit(2)
    except Exception as e:
        print(f"Upload failed: {e}", file=sys.stderr)
        sys.exit(3)


if __name__ == "__main__":
    main()