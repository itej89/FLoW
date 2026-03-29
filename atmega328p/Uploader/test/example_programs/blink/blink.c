#define F_CPU 16000000UL // Define CPU frequency, e.g., 16MHz
#include <avr/io.h>      // Required for AVR I/O operations
#include <util/delay.h>  // Required for delay functions

int main(void) {
    // Set Pin B0 as an output pin
    DDRB |= (1 << PB5); 

    while (1) {
        // Turn LED on (set Pin B0 high)
        PORTB |= (1 << PB5);
        _delay_ms(100); // Delay for 500 milliseconds

        // Turn LED off (set Pin B0 low)
        PORTB &= ~(1 << PB5);
        _delay_ms(100); // Delay for 500 milliseconds
    }
    return 0; // This line is technically unreachable in an infinite loop
}