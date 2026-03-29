#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <avr/io.h>

#define bool uint8_t

//Some Useful defines
#define WAIT_INTERRUPT !(TWCR & (1<<TWINT))
#define TWI_STATUS (TWSR & 0xF8)


// I2C TRANSMISSION HARDWARE STATES
// I2C Valid Status Register Codes for Slave Receive
// ==================================================
#define I2C_HW_SLA_W 0x60
#define I2C_HW_SLA_W_ARB_LOST 0x68
#define I2C_HW_GEN_W 0x70
#define I2C_HW_GEN_W_ARB_LOST 0x78

#define I2C_HW_SLA_W_DATA_ACK 0x80
#define I2C_HW_SLA_W_DATA_NACK 0x88
#define I2C_HW_GEN_W_DATA_ACK 0x90
#define I2C_HW_GEN_W_DATA_NACK 0x98
#define I2C_HW_RX_STOP 0xA0

// I2C Valid Status Register Codes for Slave Transmit
// ===================================================
#define I2C_HW_SLA_R 0xA8
#define I2C_HW_SLA_R_ARB_LOST 0xB0
#define I2C_HW_SLA_R_DATA 0xB8

#define I2C_HW_SLA_R_DATA_NACK 0xC0
#define I2C_HW_SLA_R_LAST_DATA_ACK 0xC8

enum I2C_MODE {
    TX,
    RX
};

enum I2C_STATES {
    I2C_RESET,
    I2C_RECEPTION_BEGIN,
    I2C_RECEPTION_END,
    I2C_TRANSMIT_BEGIN,
    I2C_TRANSMIT_END,
    I2C_TIMEOUT

};
extern enum I2C_STATES I2C_STATE;
#define I2C_TIMEOUT_25        1000 //ms

extern uint8_t _i2c_data_len;
extern uint8_t _i2c_buf_idx;
extern uint8_t _i2c_buff[160];

// API Interface
inline void i2c_set_data_len(uint8_t len){
    _i2c_data_len = len;
}

inline int i2c_get_data_len(uint8_t len){
    return _i2c_data_len;
}

inline uint8_t* i2c_get_buffer_ptr(){
    return _i2c_buff;
}

void i2c_init_device(uint8_t addr);
enum I2C_STATES i2c_send(uint8_t len);
enum I2C_STATES i2_receive();

#endif