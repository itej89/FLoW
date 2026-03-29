#include "flash_driver.h"

/* Flash write helpers: write a full page from buffer (128 bytes). */
void flash_write_page(uint32_t page_word_addr, uint8_t *buf, uint8_t length) {
    uint16_t i;
    uint16_t word;
    uint32_t addr = page_word_addr; /* word address (SPM needs byte address but we use word) */

    /* Erase page */
    uint32_t byte_addr = addr * 2UL;
    boot_page_erase(byte_addr);
    boot_spm_busy_wait();

    /* Fill page buffer */
    for (i = 0; i < FLASH_PAGE_SIZE; i += 2) {
        uint16_t w = (uint16_t)buf[i] | ((uint16_t)buf[i+1] << 8);
        boot_page_fill(byte_addr + i, w);
    }

    /* Write page */
    boot_page_write(byte_addr);
    boot_spm_busy_wait();

    /* Reenable RWW-section */
    boot_rww_enable();
}


uint8_t application_present(void)
{
    uint16_t reset_vector = pgm_read_word((uint16_t*)0x0000);

    if (reset_vector == 0xFFFF)
        return 0;

    return 1;
}