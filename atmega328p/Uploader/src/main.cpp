#include <Arduino.h>

#include "uart_interface.h"
#include "twi_interface.h"
#include "device_interface.h"

static uint8_t rx_data[128];

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

  uint8_t ack = 0x00;
  uint8_t status = forward_frame_to_target(devaddr, cmd, len, rx_data, &ack);
  send_uart_response(status, ack);
}