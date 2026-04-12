#pragma once

struct FirmwareMetadata {
    const char* version = "1.0.0";
    const char* buildDate = __DATE__;
    const char* buildTime = __TIME__;
    const char* board = "ATmega328P";
};