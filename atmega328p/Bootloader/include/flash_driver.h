#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/* ATmega328P page size */
#define FLASH_PAGE_SIZE 128U

/* Flash write helpers: write a full page from buffer (128 bytes). */
void flash_write_page(uint32_t page_word_addr, uint8_t *buf, uint8_t length);
uint8_t application_present(void);

#endif