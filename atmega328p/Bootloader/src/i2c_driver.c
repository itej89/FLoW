#include "i2c_driver.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart_debug.h"
#include "timing.h"

enum I2C_STATES I2C_STATE = I2C_RESET;

uint8_t _i2c_data_len = 0;
uint8_t _i2c_buf_idx = 0;
uint8_t _i2c_buff[160];

uint8_t _i2c_wait_timeout(int16_t timeout_ms)
{
    while (timeout_ms > 0) {
        // //uart_puts("WAITING\r\n");
        // //uart_puts("\ni2c_looping: ");
        // char string_time_ms[10];
        // int_to_string(timeout_ms, string_time_ms);
        // //uart_puts(string_time_ms);
        // check TWI interrupt flag
        if (TWCR & (1<<TWINT)) {
            return 1; // success
        }

        // 1 millisecond delay using NOP
        for (uint32_t i = 0; i < ITER_PER_MS; i++) {
            __asm__ __volatile__("nop");
        }

        timeout_ms = timeout_ms-1;
    }
    // //uart_puts("\n\n\ni2c_looping break... ");
    return 0; // timeout
}

void _i2c_do_transmission(enum I2C_MODE mode){
    if(!_i2c_wait_timeout(I2C_TIMEOUT_25)){
        I2C_STATE = I2C_TIMEOUT;
        TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
        return;
    }

    uint8_t ack = 1;
    // uart_putc(TWI_STATUS);
    // //uart_puts("\r\n");
    switch (TWI_STATUS) 
    {
    case I2C_HW_SLA_W:
    case I2C_HW_SLA_W_ARB_LOST:
    case I2C_HW_GEN_W:
    case I2C_HW_GEN_W_ARB_LOST:
        if (mode == TX) {
        //uart_puts("SLA_W_TX\r\n");
            ack = 1; 
            break;
        }
        //uart_puts("SLA_W\r\n");
        I2C_STATE = I2C_RECEPTION_BEGIN;
        _i2c_data_len = 0;
        _i2c_buf_idx = 0;
        break;
    case I2C_HW_SLA_W_DATA_ACK:
    case I2C_HW_SLA_W_DATA_NACK:
    case I2C_HW_GEN_W_DATA_ACK:
    case I2C_HW_GEN_W_DATA_NACK:
        if (mode == TX)  {
        //uart_puts("SLA_W_DATA_ACK_TX\r\n");
            ack = 1; 
            break;
        }
        //uart_puts("SLA_W_DATA_ACK\r\n");
        _i2c_buff[_i2c_buf_idx++] = TWDR;
        break;
    case I2C_HW_RX_STOP:
        if (mode == TX)  {
            //uart_puts("RX_STOP_TX\r\n");
            ack = 1; 
            break;
        }
        //uart_puts("RX_STOP\r\n");
        I2C_STATE = I2C_RECEPTION_END;
        _i2c_buf_idx = 0;
        _i2c_data_len = 0;
        break;
    case I2C_HW_SLA_R:
    case I2C_HW_SLA_R_ARB_LOST:
        if (mode == RX) {
            //uart_puts("SLA_R_RX\r\n");
            if (I2C_STATE == I2C_RECEPTION_BEGIN) {
                I2C_STATE = I2C_RECEPTION_END;
            }
            ack = 1; 
            break;
        }
        //uart_puts("SLA_R\r\n");
        _i2c_buf_idx = 0;
        I2C_STATE = I2C_TRANSMIT_BEGIN;
        if (_i2c_buf_idx < _i2c_data_len) {
            TWDR = _i2c_buff[_i2c_buf_idx++];
        } else {
            TWDR = 0x00;  // defined padding
        }

        break;
    case I2C_HW_SLA_R_DATA:
        if (mode == RX)  {
            //uart_puts("SLA_R_DATA_RX\r\n");
            ack = 1; 
            break;
        }
        //uart_puts("SLA_R_DATA\r\n");
        if (_i2c_buf_idx < _i2c_data_len) {
            TWDR = _i2c_buff[_i2c_buf_idx++];
        } else {
            TWDR = 0x00;  // defined padding
        }

        break;
    case I2C_HW_SLA_R_DATA_NACK:
    case I2C_HW_SLA_R_LAST_DATA_ACK:
        if (mode == RX)  {
            //uart_puts("SLA_R_DATA_NACK_RX\r\n");
            ack = 1; 
            break;
        }
        //uart_puts("SLA_R_DATA_NACK\r\n");
        I2C_STATE = I2C_TRANSMIT_END;
        _i2c_buf_idx = 0;
        break;
    
    default:
        //uart_puts("UNKNOWN STATUS: ");
        break;
    }

    TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
}

void i2c_init_device(uint8_t addr) {
    /* Set own TWI (slave) address */
    TWAR = (addr << 1); /* TWAR uses address in upper bits; TWGCE=0 by default */
    /* Enable TWI, ack, and TWI interrupt disabled (we'll poll TWCR). Use prescaler = 1 */
    TWSR = 0;
    TWBR = 72; /* just a value; in slave mode bitrate not critical */
    TWCR = (1<<TWEN) | (1<<TWEA) | (1<<TWINT);
}

enum I2C_STATES i2c_send(uint8_t len){
    uart_puts("\r\ni2c_send: \r\n");
    // uart_putc(TWI_STATUS);
    //uart_puts("\r\n");
    i2c_set_data_len(len);

    I2C_STATE = I2C_RESET;
    while (I2C_STATE != I2C_TRANSMIT_END &&
        I2C_STATE != I2C_TIMEOUT){
        _i2c_do_transmission(TX);
    }
    return I2C_STATE;
}

enum I2C_STATES i2_receive(){
    uart_puts("\r\ni2_receive\r\n");
    I2C_STATE = I2C_RESET;
    while (I2C_STATE != I2C_RECEPTION_END &&
        I2C_STATE != I2C_TIMEOUT){
        _i2c_do_transmission(RX);
    }
    return I2C_STATE;
}