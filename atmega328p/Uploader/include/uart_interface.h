#pragma once

#include <Arduino.h>


#define SERIAL_BAUD       115200UL

#define UART_SOF          0xA5
#define UART_RESP_SOF     0x5A
#define STATUS_UART_CKSUM 0x01
#define STATUS_UART_FRAME 0x02


extern uint8_t uart_xor(uint8_t devaddr, uint8_t cmd, uint8_t len, const uint8_t *data);
extern void send_uart_response(uint8_t status, uint8_t ack);
extern void send_uart_response_data(uint8_t status, const uint8_t *data, uint8_t data_len);
extern bool read_exact(uint8_t *buf, size_t n, uint32_t timeout_ms = 2000);