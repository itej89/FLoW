
#ifdef DEBUG_UART     // Debug enabled

#include "uart_debug.h"

#include <avr/io.h>
#include <stdarg.h>
#include <stdio.h>

void uart_init(void) {
    UCSR0A &= ~(1 << U2X0);

    uint16_t ubrr = ((F_CPU + DEBUG_BAUD * 8) / ((DEBUG_BAUD * 16))) - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);

    // Set frame format: 8 data bits, no parity, 1 stop bit
    UCSR0C = (3 << UCSZ00); // UCSZ00 and UCSZ01 set to 1 for 8 data bits

    // Enable transmitter and receiver
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void uart_putc(char c) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = c;
}

void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

void uart_printf(const char *fmt, ...) {
    char buf[64];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    uart_puts(buf);
}

// Helper function to reverse a string
void reverse(char* str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Function to convert an integer to a string
void int_to_string(int num, char* str) {
    int i = 0;
    int isNegative = 0;

    // Handle 0 separately, otherwise the loop below wouldn't run
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0'; // Null-terminate the string
        return;
    }

    // Handle negative numbers
    if (num < 0) {
        isNegative = 1;
        num = -num; // Make the number positive for digit extraction
    }

    // Extract digits in reverse order
    while (num > 0) {
        int digit = num % 10;
        // Convert digit to its ASCII character representation by adding '0'
        str[i++] = digit + '0'; 
        num = num / 10;
    }

    // Append the negative sign if necessary
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Null-terminate the string

    // Reverse the string to get the correct order
    reverse(str, i);
}

#endif