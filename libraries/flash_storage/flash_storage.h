/*
 * flash_storage.h
 *
 *  Created on: Sep 26, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_FLASH_STORAGE_FLASH_STORAGE_H_
#define LIBRARIES_FLASH_STORAGE_FLASH_STORAGE_H_

#include <stdint.h>

// Flash page address definitions
#define SENSOR_ID_FLASH_PAGE_ADDRESS          0x08050000
#define SENSOR_TYPE_FLASH_PAGE_ADDRESS        0x08055000
#define LOGGER_ID_FLASH_PAGE_ADDRESS          0x08065000

// Functions for writing and reading uint32_t values
void writeFlash_uint32(uint32_t address, uint32_t value);
uint32_t readFlash_uint32(uint32_t address);

// Functions for writing and reading uint8_t values
void writeFlash_uint8(uint32_t address, uint8_t value);
uint8_t readFlash_uint8(uint32_t address);

// Functions for writing and reading unsigned char arrays
void writeFlash_ucharArray(uint32_t address, unsigned char* data, uint32_t len);
void readFlash_ucharArray(uint32_t address, unsigned char* data, uint32_t len);

void eraseAllHardcodedFlashPages(void); // Prototype for erasing all hardcoded flash pages
void eraseFlashPage(uint32_t flashAddress);  // New function prototype for erasing flash pages

#endif /* LIBRARIES_FLASH_STORAGE_FLASH_STORAGE_H_ */
