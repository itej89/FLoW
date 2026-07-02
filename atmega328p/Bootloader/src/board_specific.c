#include <avr/io.h>
#include <stdint.h>

#include "board_specific.h"

// Arduino digital 17 == A3 == PC3 on ATmega328P
#define I2C_SELECT_PIN PC3  

void set_board_config()
{   
    //Add board specific configurations here
    // DDRC |= (1 << I2C_SELECT_PIN);
    // PORTC |= (1 << I2C_SELECT_PIN);
}