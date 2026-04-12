#include <CommandRequestHandler.h>
#include <DebugPrintManager.h>
#include <MemoryManager.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <util/delay.h>


/* survives watchdog reset */
#define BOOT_MAGIC 0xB007
#define BOOT_KEY_ADDR ((volatile uint16_t *)0x08FA)

static void enter_bootloader_via_wdt(void) {
    cli();

    *BOOT_KEY_ADDR = BOOT_MAGIC;

    // Wait until software TX buffer is empty
    // while (tx_head != tx_tail) {}

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

// Handle Command Requests
void serveCommands() {

    if (CMD_REQ_BOOTLOADER){
      delay(1);
      enter_bootloader_via_wdt();
    }

  }