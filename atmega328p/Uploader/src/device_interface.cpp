#include "device_interface.h"

#include "twi_interface.h"

#include "status_codes.h"

uint8_t forward_frame_to_target(uint8_t devaddr, uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t *ack) {
  uint8_t err;

  // START + SLA+W
  err = twi_start_write(devaddr);
  if (err != STATUS_OK) {
    twi_stop();
    return err;
  }

  // Write programmer frame
  err = twi_write_byte(cmd);
  if (err != STATUS_OK) {
    twi_stop();
    return err;
  }

  err = twi_write_byte(len);
  if (err != STATUS_OK) {
    twi_stop();
    return err;
  }

  for (uint8_t i = 0; i < len; i++) {
    err = twi_write_byte(data[i]);
    if (err != STATUS_OK) {
      twi_stop();
      return err;
    }
  }

  // IMPORTANT: NO STOP HERE
  // Issue repeated START directly
  err = twi_start_read(devaddr);
  if (err != STATUS_OK) {
    twi_stop();
    return err;
  }

  // Read single ACK byte, then NACK it
  err = twi_read_byte_nack(ack);
  twi_stop();
  return err;
}