#pragma once

#include <Arduino.h>

enum DeviceMode {
    MODE_BOOTLOADER = 0,
    MODE_APPLICATION = 1
};

extern enum DeviceMode current_mode;

uint8_t forward_frame_to_target(uint8_t devaddr, uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t *response_buf, uint8_t *response_len);