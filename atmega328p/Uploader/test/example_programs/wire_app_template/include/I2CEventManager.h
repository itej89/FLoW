#pragma once

#define I2C_ADDRESS 0x05

// Maximum time for transmitting a single command from I2C master before transmission reset
#define I2C_COMMAND_TRANSMISSION_TIME_OUT_MS 5000
// Maximum time for transmitting first wo bytes of a single commnd from I2C master
#define I2C_COMMAND_HEADER_TIME_OUT_MS 1000
// I2C SOF Recieved
#define I2C_SOF 0xD8
// I2C EOF Recieved
#define I2C_EOF 0xD9
#define I2C_ACK_OK 0x01
// I2C CHKSUM SEED
#define I2C_CHKSUM_SEED 0xFF

//Frame Format
// 1Byte SOF | 1bit R/W | 4bits Data Len | 11bits Address | Data(optional) | 1Byte CHKSUM | 1Byte EOF 
#define REG_OP_MODE(x)  (x>>7 ? WRITE : READ)
#define DATA_LENGTH(x)       (x&0x78) >> 3
#define REG_ADDRESS(high_byte, low_byte) (((uint16_t)(high_byte & 0x07) << 8) | (low_byte))
#define BASE_FRAME_LENGTH 4 //SOF+HEADER+CHKSUM+EOF

// I2C Error States
#define I2C_CORRUPTED_DATA 0x01

void initializeCommunications();
void requestEvent();
void receiveEvent(int count);
