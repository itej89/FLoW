#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <util/delay.h>

#define BAUD 115200UL
#define UBRR_VALUE ((F_CPU / (8UL * BAUD)) - 1)

#define TX_BUF_SIZE 64

volatile uint8_t tx_buf[TX_BUF_SIZE];
volatile uint8_t tx_head = 0;
volatile uint8_t tx_tail = 0;
volatile uint8_t boot_request = 0;

/* survives watchdog reset */
#define BOOT_MAGIC 0xB007
#define BOOT_KEY_ADDR ((volatile uint16_t *)0x08FA)

static void uart_init(void) {
    UCSR0A = (1 << U2X0);
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UBRR_VALUE & 0xFF);

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

static void uart_queue_byte(uint8_t c) {
    uint8_t next = (tx_head + 1) % TX_BUF_SIZE;
    while (next == tx_tail) {}
    tx_buf[tx_head] = c;
    tx_head = next;
    UCSR0B |= (1 << UDRIE0);
}

static void uart_queue_string(const char *s) {
    while (*s) {
        uart_queue_byte((uint8_t)*s++);
    }
}

static void enter_bootloader_via_wdt(void) {
    cli();

    *BOOT_KEY_ADDR = BOOT_MAGIC;

    // Wait until software TX buffer is empty
    while (tx_head != tx_tail) {}

    // Wait until UART shift register finished sending last byte
    while (!(UCSR0A & (1 << TXC0))) {}
    UCSR0A |= (1 << TXC0);

    // Disable UART + interrupts/peripherals
    UCSR0B = 0;
    UCSR0A = 0;

    TIMSK0 = 0;
    TIMSK1 = 0;
    TIMSK2 = 0;

    wdt_enable(WDTO_15MS);
    while (1) {}
}

ISR(USART_RX_vect) {
    uint8_t c = UDR0;

    if (c == 'B') {
        boot_request = 1;
        return;
    }

    uart_queue_byte(c);
    uart_queue_string(" ACK\r\n");
}

ISR(USART_UDRE_vect) {
    if (tx_head == tx_tail) {
        UCSR0B &= ~(1 << UDRIE0);
        return;
    }

    UDR0 = tx_buf[tx_tail];
    tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
}

int main(void) {
    if (!(MCUSR & (1 << WDRF))) {
        *BOOT_KEY_ADDR = 0;
    }
    MCUSR = 0;
    wdt_disable();

    uart_init();
    sei();

    DDRB |= (1 << PB5);

    uart_queue_string("APP READY\r\n");

    while (1) {
        if (boot_request) {
            uart_queue_string("BOOT\r\n");

            // allow UDRE ISR to flush the queued bytes
            while (tx_head != tx_tail) {}
            while (!(UCSR0A & (1 << TXC0))) {}
            UCSR0A |= (1 << TXC0);

            enter_bootloader_via_wdt();
        }

        PORTB |= (1 << PB5);
        _delay_ms(100);

        if (boot_request) {
            uart_queue_string("BOOT\r\n");
            while (tx_head != tx_tail) {}
            while (!(UCSR0A & (1 << TXC0))) {}
            UCSR0A |= (1 << TXC0);
            enter_bootloader_via_wdt();
        }

        PORTB &= ~(1 << PB5);
        _delay_ms(100);
    }

    return 0;
}