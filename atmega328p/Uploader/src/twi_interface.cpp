#include "twi_interface.h"

#include "status_codes.h"

void twi_init_master(uint32_t scl_hz) {
  TWSR = 0x00; // prescaler = 1
  TWBR = (uint8_t)((F_CPU / scl_hz - 16UL) / 2UL);
  TWCR = (1 << TWEN);
}

uint8_t twi_start_write(uint8_t address7) {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {}
  uint8_t st = TWSR & 0xF8;
  if (st != TW_START && st != TW_REP_START) {
    return STATUS_I2C_START;
  }

  TWDR = (address7 << 1) | 0;  // SLA+W
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {}
  st = TWSR & 0xF8;
  if (st != TW_MT_SLA_ACK) {
    return STATUS_I2C_SLAW;
  }

  return STATUS_OK;
}

uint8_t twi_start_read(uint8_t address7) {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {}
  uint8_t st = TWSR & 0xF8;
  if (st != TW_START && st != TW_REP_START) {
    return STATUS_I2C_STARTR;
  }

  TWDR = (address7 << 1) | 1;  // SLA+R
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {}
  st = TWSR & 0xF8;
  if (st != TW_MR_SLA_ACK) {
    return STATUS_I2C_SLAR;
  }

  return STATUS_OK;
}

void twi_stop(void) {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while (TWCR & (1 << TWSTO)) {}
}

uint8_t twi_write_byte(uint8_t b) {
  TWDR = b;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {}
  uint8_t st = TWSR & 0xF8;
  if (st != TW_MT_DATA_ACK) {
    return STATUS_I2C_WRITE;
  }
  return STATUS_OK;
}

uint8_t twi_read_byte_nack(uint8_t *out) {
  TWCR = (1 << TWINT) | (1 << TWEN);  // NACK after one byte
  while (!(TWCR & (1 << TWINT))) {}
  uint8_t st = TWSR & 0xF8;
  if (st != TW_MR_DATA_NACK) {
    return STATUS_I2C_SLAR;
  }
  *out = TWDR;
  return STATUS_OK;
}