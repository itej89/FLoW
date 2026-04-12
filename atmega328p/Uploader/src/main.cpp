#include <Arduino.h>
#include <Wire.h>

#include "uart_interface.h"
#include "twi_interface.h"
#include "device_interface.h"
#include "status_codes.h"

static uint8_t rx_data[128];
static uint8_t response_data[16]; // For application mode responses

void setup() {
  Serial.begin(SERIAL_BAUD);
  twi_init_master(TWI_FREQ);
}

void loop() {
  if (!Serial.available()) {
    return;
  }

  int b = Serial.read();
  if (b < 0) {
    return;
  }

  if ((uint8_t)b != UART_SOF) {
    return;
  }

  uint8_t hdr[3];
  if (!read_exact(hdr, 3)) {
    send_uart_response(STATUS_UART_FRAME, 0x00);
    return;
  }

  uint8_t devaddr = hdr[0];
  uint8_t cmd = hdr[1];
  uint8_t len = hdr[2];

  if (len > sizeof(rx_data)) {
    send_uart_response(STATUS_UART_FRAME, 0x00);
    return;
  }

  if (len > 0) {
    if (!read_exact(rx_data, len)) {
      send_uart_response(STATUS_UART_FRAME, 0x00);
      return;
    }
  }

  uint8_t recv_chk;
  if (!read_exact(&recv_chk, 1)) {
    send_uart_response(STATUS_UART_FRAME, 0x00);
    return;
  }

  uint8_t calc_chk = uart_xor(devaddr, cmd, len, rx_data);
  if (recv_chk != calc_chk) {
    send_uart_response(STATUS_UART_CKSUM, 0x00);
    return;
  }


  // Handle mode setting command
  if (cmd == 0xFF && len >= 1) {
    current_mode = (enum DeviceMode)rx_data[0];
    if (current_mode == MODE_APPLICATION) {
      Wire.begin();
      Wire.setClock(TWI_FREQ);
    }
    if (current_mode == MODE_BOOTLOADER) {
      twi_init_master(TWI_FREQ);
    }
    send_uart_response(STATUS_OK, 0x00);
    return;
  }

  // Forward to target
  uint8_t response_len = 0;
  uint8_t status = forward_frame_to_target(devaddr, cmd, len, rx_data, response_data, &response_len);
  
  if (status == STATUS_OK) {
    if (current_mode == MODE_BOOTLOADER) {
      send_uart_response(status, response_data[0]);
    } else {
      send_uart_response_data(status, response_data, response_len);
    }
  } else {
    send_uart_response(status, 0x00);
  }
}