#pragma once

#include <avr/io.h>
#include <util/twi.h>

#define STATUS_I2C_START  0x10
#define STATUS_I2C_SLAW   0x11
#define STATUS_I2C_WRITE  0x12
#define STATUS_I2C_STARTR 0x13
#define STATUS_I2C_SLAR   0x14

#define TWI_FREQ          100000UL

extern void twi_init_master(uint32_t scl_hz);
extern uint8_t twi_start_write(uint8_t address7);
extern uint8_t twi_start_read(uint8_t address7);
extern void twi_stop(void);
extern uint8_t twi_write_byte(uint8_t b);
extern uint8_t twi_read_byte_nack(uint8_t *out);