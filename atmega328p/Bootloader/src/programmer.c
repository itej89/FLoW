#include "programmer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "timing.h"
#include "i2c_driver.h"
#include "flash_driver.h"
#include "uart_debug.h"

enum PRG_STATES PRG_STATE = PRG_RESET;
struct PRG_FRAME _prg_frame;
struct PRG_PAGE_DATA _prg_page_data;

void _prg_parse_frame(uint8_t* buffer) {
    _prg_frame.Command = buffer[0];
    _prg_frame.length = buffer[1];
    _prg_frame.data = &buffer[2];
}

uint8_t _prg_receive_wait_timeout(int16_t retry_count)
{
    while (retry_count > 0) {
        enum I2C_STATES err = i2_receive();
        // check TWI interrupt flag
        if (err != I2C_TIMEOUT) {
            return 1; // success
        }

        retry_count = retry_count-1;
    }
    
    //uart_puts("\n\n\nprg looping broken..");                                     
    PRG_STATE = PRG_TIMEOUT; 
    return 0; // timeout
}

uint8_t _prg_send_wait_timeout(uint8_t len, uint16_t retry_count)
{
    while (retry_count--) {
        
        enum I2C_STATES err = i2c_send(len);
        // check TWI interrupt flag
        if (err != I2C_TIMEOUT) {
            return 1; // success
        }
    }                                     \
    PRG_STATE = PRG_TIMEOUT; 
    return 0; // timeout
}

void _do_programming() {
    //Reuse the I2C_driver Data Buffer to avoid copies
    uint8_t* buffer = i2c_get_buffer_ptr();

    if (!_prg_receive_wait_timeout(PRG_RETRY_COUNT)) 
        return;

    _prg_parse_frame(buffer);
    switch (_prg_frame.Command)
    {
    case PRG_INT_BEGIN:
        uart_puts("PRG_INT_BEGIN\r\n");
        PRG_STATE=PRG_BEGIN;

        // buffer[0] = 1;
        // if (!_prg_send_wait_timeout(1, PRG_RETRY_COUNT)) 
        //     return;

        buffer[0] = PRG_INT_BEGIN_ACK;
        if (!_prg_send_wait_timeout(1, PRG_RETRY_COUNT)) 
            return;

        break;
    
    case PRG_INT_SET_PAGE:
        uart_puts("PRG_INT_SET_PAGE\r\n");
        //PARSE ADDRESS
        _prg_page_data.address = ((uint32_t)_prg_frame.data[0] << 16) | \
                        ((uint32_t)_prg_frame.data[1] << 8) | \
                        _prg_frame.data[2];
        buffer[0] = PRG_INT_SET_PAGE_ACK;
        
        if (!_prg_send_wait_timeout(1, PRG_RETRY_COUNT)) 
            return;

        break;

    case PRG_INT_WRITE_PAGE:
        uart_puts("PRG_INT_WRITE_PAGE\r\n");
        //WRITE DATA==================
        _prg_page_data.length=_prg_frame.length;
        _prg_page_data.data=_prg_frame.data;
        flash_write_page(_prg_page_data.address,
                        _prg_page_data.data,
                        _prg_page_data.length);
        buffer[0] = PRG_INT_WRITE_PAGE_ACK;
        
        if (!_prg_send_wait_timeout(1, PRG_RETRY_COUNT)) 
            return;
            
        break;

    case PRG_INT_EXIT:
        uart_puts("PRG_INT_EXIT\r\n");
        PRG_STATE=PRG_END;
        buffer[0] = PRG_INT_EXIT_ACK;
        
        if (!_prg_send_wait_timeout(1, PRG_RETRY_COUNT)) 
            return;
            
        break;
    }
}


void prg_try_flashing(){
    PRG_STATE = PRG_RESET;
    while(PRG_STATE!=PRG_END &&
        PRG_STATE!=PRG_TIMEOUT) {
            _do_programming();
        }
}