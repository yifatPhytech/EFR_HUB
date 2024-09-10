/*
 * flash_storage.c
 *
 *  Created on: Sep 26, 2023
 *      Author: itay
 *
 *        uint32_t sensorID;                    // Unique identifier for the sensor
 *        uint8_t sensorType;                   // Type of sensor (could be an enum or defined values)
 *        unsigned char firmwareVersion[4];     // Firmware version details: 'T', month, year, version index
 */

#include "libraries/flash_storage/flash_storage.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_msc.h"

/**
 * Write a 32-bit value to a specified flash address.
 * @param flashAddress - Address to write to
 * @param value - 32-bit value to write
 */
void writeFlash_uint32(uint32_t flashAddress, uint32_t value)
{
  uint32_t *address = (uint32_t *)flashAddress;

  MSC_Init();                                 // Initialize the Memory System Controller
  MSC_ErasePage(address);                     // Erase the target flash page
  MSC_WriteWord(address, &value, sizeof(value));  // Write the 32-bit value to flash
  MSC_Deinit();                               // De-initialize the Memory System Controller
}

/**
 * Read a 32-bit value from a specified flash address.
 * @param flashAddress - Address to read from
 * @return - Read 32-bit value
 */
uint32_t readFlash_uint32(uint32_t flashAddress)
{
  uint32_t *address = (uint32_t *)flashAddress;
  return *address;                            // Return the read value
}

/**
 * Write an 8-bit value to a specified flash address.
 * @param flashAddress - Address to write to
 * @param value - 8-bit value to write
 */
void writeFlash_uint8(uint32_t flashAddress, uint8_t value)
{
  // Temporary storage for the 8-bit value in a 32-bit variable
  uint32_t tempValue = value;

  // Convert the flash address to a word (32-bit) pointer
  uint32_t *address = (uint32_t *)flashAddress;

  MSC_Init();                                 // Initialize the Memory System Controller
  MSC_ErasePage(address);                     // Erase the target flash page
  MSC_WriteWord(address, &tempValue, sizeof(tempValue));  // Write the 8-bit value inside a 32-bit container to flash
  MSC_Deinit();                               // De-initialize the Memory System Controller
}

/**
 * Read an 8-bit value from a specified flash address.
 * @param flashAddress - Address to read from
 * @return - Read 8-bit value
 */
uint8_t readFlash_uint8(uint32_t flashAddress)
{
  // Read the value using a 32-bit pointer
  uint32_t *address = (uint32_t *)flashAddress;
  uint32_t tempValue = *address;

  // Extract the 8-bit value from the 32-bit read
  return (uint8_t)(tempValue & 0xFF);
}

/**
 * Write an array of unsigned chars to a specified flash address.
 * Note: Assumes that len is a multiple of 4 for now.
 * @param flashAddress - Address to write to
 * @param data - Pointer to the array data
 * @param len - Length of the array
 */
void writeFlash_ucharArray(uint32_t flashAddress, unsigned char* data, uint32_t len)
{
  uint32_t *address = (uint32_t *)flashAddress;

  MSC_Init();                                 // Initialize the Memory System Controller

  // Assuming len is a multiple of 4 for simplicity,
  // can be enhanced for non-multiple lengths
  for (uint32_t i = 0; i < len; i += 4)
    {
      MSC_ErasePage(address + (i/4)); // Erase the 32-bit chunk in flash
      MSC_WriteWord(address + (i/4), &data[i], sizeof(uint32_t)); // Write 4 bytes at a time
    }

  MSC_Deinit();                               // De-initialize the Memory System Controller
}

/**
 * Read an array of unsigned chars from a specified flash address.
 * @param flashAddress - Address to read from
 * @param data - Pointer to the destination buffer
 * @param len - Length of the array
 */
void readFlash_ucharArray(uint32_t flashAddress, unsigned char* data, uint32_t len)
{
  unsigned char *address = (unsigned char *)flashAddress;

  // Read each byte from flash to the buffer
  for (uint32_t i = 0; i < len; i++)
    {
      data[i] = address[i];
    }
}

// Erase FLASH memory
void eraseAllHardcodedFlashPages(void)
{
  // Erase SENSOR_ID_FLASH_PAGE
  eraseFlashPage(SENSOR_ID_FLASH_PAGE_ADDRESS);

  // Erase SENSOR_TYPE_FLASH_PAGE
  eraseFlashPage(SENSOR_TYPE_FLASH_PAGE_ADDRESS);

  // Erase LOGGER_ID_FLASH_PAGE
  eraseFlashPage(LOGGER_ID_FLASH_PAGE_ADDRESS);
}

/**
 * Erase a flash page at a specified address.
 * @param flashAddress - Start address of the page to erase.
 */
void eraseFlashPage(uint32_t flashAddress)
{
  uint32_t *address = (uint32_t *)flashAddress;

  MSC_Init();                                     // Initialize the Memory System Controller
  MSC_ErasePage(address);                         // Erase the target flash page
  MSC_Deinit();                                   // De-initialize the Memory System Controller
}
