#pragma once

#include <Arduino.h>
uint8_t forward_frame_to_target(uint8_t devaddr, uint8_t cmd, uint8_t len, const uint8_t *data, uint8_t *ack);