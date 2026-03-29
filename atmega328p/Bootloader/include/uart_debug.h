#ifndef DEBUG_UART_H
#define DEBUG_UART_H

#ifdef DEBUG_UART     // Debug enabled

#include <avr/io.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef DEBUG_BAUD
#define DEBUG_BAUD 115200UL
#endif

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_printf(const char *fmt, ...);

// Helper function to reverse a string
void reverse(char* str, int length);

// Function to convert an integer to a string
void int_to_string(int num, char* str);

#else   /* DEBUG_UART not defined — all debug removed */

static inline void uart_init(void) {}
static inline void uart_putc(char c) {}
static inline void uart_puts(const char *s) {}
static inline void uart_printf(const char *fmt, ...) {}
static inline void reverse(char* str, int length){}
static inline void int_to_string(int num, char* str){}

#endif  /* DEBUG_UART */

#endif