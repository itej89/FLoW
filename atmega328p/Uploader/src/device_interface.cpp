#include "device_interface.h"
#include "twi_interface.h"
#include "status_codes.h"

#include <Wire.h>

enum DeviceMode current_mode = MODE_APPLICATION;

uint8_t forward_frame_to_target(uint8_t devaddr, uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t *response_buf, uint8_t *response_len) {
    if (current_mode == MODE_BOOTLOADER) {
        // Bootloader protocol: cmd, len, data -> 1 byte ACK
        uint8_t err;
        uint8_t ack;

        err = twi_start_write(devaddr);
        if (err != STATUS_OK) {
            twi_stop();
            return err;
        }

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

        err = twi_start_read(devaddr);
        if (err != STATUS_OK) {
            twi_stop();
            return err;
        }

        err = twi_read_byte_nack(&ack);
        twi_stop();
        
        *response_len = 1;
        response_buf[0] = ack;
        return err;
    } else if (current_mode == MODE_APPLICATION) {
        uint8_t checksum = 0xFF;

        uint8_t operation = cmd; // 0=write, 1=read
        uint16_t address = (data[0] << 8) | data[1];
        uint8_t data_len;
        const uint8_t *value_data = nullptr;

        if (operation == 1) { // write
            data_len = (len > 2 ? len - 2 : 0);
            value_data = &data[2];
        } else { // read
            data_len = (len > 2 ? data[2] : 1);
        }

        // Construct header
        uint8_t header_high = (operation << 7) | ((data_len & 0x0F) << 3) | ((address >> 8) & 0x07);
        uint8_t header_low = address & 0xFF;
        
        Wire.beginTransmission(devaddr);
        Wire.write(0xD8);
        checksum ^= 0xD8;
        Wire.write(header_high);
        checksum ^= header_high;
        Wire.write(header_low);
        checksum ^= header_low;

        if (operation == 1) {
            for (uint8_t i = 0; i < data_len; i++) {
                Wire.write(value_data[i]);
                checksum ^= value_data[i];
            }
        }

        Wire.write(checksum);
        Wire.write(0xD9);
        if (Wire.endTransmission() != 0) {
            return STATUS_I2C_START;
        }

        delay(1);
        if (operation == 1) {
            uint8_t count = Wire.requestFrom((int)devaddr, 2);
            if (count != 2) {
                return STATUS_I2C_SLAR;
            }
            response_buf[0] = Wire.read();
            response_buf[1] = Wire.read();
            *response_len = 2;
        } else {
            if (data_len == 0) {
                return STATUS_I2C_SLAR;
            }
            uint8_t count = Wire.requestFrom((int)devaddr, data_len + 2);
            if (count != data_len) {
                return STATUS_I2C_SLAR;
            }
            response_buf[0] = Wire.read();
            response_buf[1] = Wire.read();
            for (uint8_t i = 0; i < data_len; i++) {
                response_buf[i+2] = Wire.read();
            }
            *response_len = data_len+2;
        }

        return STATUS_OK;
    }

    return STATUS_OK; // Should not reach
}