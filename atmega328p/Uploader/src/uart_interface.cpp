#include "uart_interface.h"

#include <Arduino.h>

uint8_t uart_xor(uint8_t devaddr, uint8_t cmd, uint8_t len, const uint8_t *data) {
  uint8_t x = devaddr ^ cmd ^ len;
  for (uint8_t i = 0; i < len; i++) {
    x ^= data[i];
  }
  return x;
}

void send_uart_response(uint8_t status, uint8_t ack) {
  uint8_t chk = status ^ ack;
  Serial.write(UART_RESP_SOF);
  Serial.write(status);
  Serial.write(ack);
  Serial.write(chk);
}

void send_uart_response_data(uint8_t status, const uint8_t *data, uint8_t data_len) {
  uint8_t chk = status ^ data_len;
  Serial.write(UART_RESP_SOF);
  Serial.write(status);
  Serial.write(data_len);
  for (uint8_t i = 0; i < data_len; i++) {
    Serial.write(data[i]);
    chk ^= data[i];
  }
  Serial.write(chk);
}

bool read_exact(uint8_t *buf, size_t n, uint32_t timeout_ms = 2000) {
  uint32_t start = millis();
  size_t got = 0;
  while (got < n) {
    if (Serial.available()) {
      buf[got++] = (uint8_t)Serial.read();
      start = millis();
    } else {
      if ((millis() - start) > timeout_ms) {
        return false;
      }
    }
  }
  return true;
}