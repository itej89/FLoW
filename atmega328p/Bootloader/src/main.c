//  * I2C (TWI) slave bootloader for ATmega328P
//  * Intended as an Optiboot-style bootloader replacement.
//  *
//  * Notes:
//  *  - Configure BAUD/ADDRESS/F_CPU via Makefile defines.
//  *  - Boot section size should be set in linker options (e.g., -Wl,--section-start=.text=0x7E00 for 1KB boot).
//  */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>

#include "uart_debug.h"
#include "i2c_driver.h"
#include "flash_driver.h"
#include "programmer.h"

#include <avr/wdt.h>
#include <stdint.h>

#define BOOT_MAGIC 0xB007
#define BOOT_KEY_ADDR ((volatile uint16_t *)0x08FA)

/* Config */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#ifndef TWI_ADDR
#define TWI_ADDR 0x42   /* default 7-bit slave address (change as needed) */
#endif

static inline void clear_all_interrupts(void)
{
    // 1. Stop all interrupts globally
    cli();     
    wdt_disable();              

    /* Disable peripheral interrupt sources */
    TIMSK0 = 0;
    TIMSK1 = 0;
    TIMSK2 = 0;
    UCSR0B = 0;
    TWCR   = 0;

    /* Clear pending interrupt flags */
    TIFR0 = 0xFF;
    TIFR1 = 0xFF;
    TIFR2 = 0xFF;

    UCSR0A |= (1 << TXC0);     // USART TX flag
    (void)UDR0;               // USART RX flag

    TWCR = (1 << TWINT);      // TWI flag
}

int main(void) __attribute__ ((OS_main)) __attribute__ ((section (".init9"))) __attribute__((used));
int main(void) {
    uint8_t reset_flags = MCUSR;
    MCUSR = 0;
    wdt_disable();

    clear_all_interrupts();
    
    uart_init();
    i2c_init_device(TWI_ADDR);

    uint8_t stay_in_boot = 0;


    uart_puts("in boot... \r\n");

    if ((reset_flags & (1 << WDRF)) && (*BOOT_KEY_ADDR == BOOT_MAGIC)) {
        stay_in_boot = 1;
        uart_puts("wdt+magic\r\n");
    } else if (!application_present()) {
        stay_in_boot = 1;
        uart_puts("no app\r\n");
    } else {
        uart_puts("jump app\r\n");
    }

    *BOOT_KEY_ADDR = 0;

    DDRB |= (1 << PB5);

    if (stay_in_boot) {
        for (uint8_t i = 0; i < 2; i++) {
            PORTB |= (1 << PB5);
            _delay_ms(100);
            PORTB &= ~(1 << PB5);
            _delay_ms(100);
        }

        prg_try_flashing();
        _delay_ms(10);
    }

    void (*app_start)(void) = 0x0000;
    sei();
    app_start();

    while (1) {}
    return 0;
}