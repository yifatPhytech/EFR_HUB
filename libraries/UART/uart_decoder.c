/*
 * uart_decoder.c
 *
 *  Created on: Sep 27, 2023
 *      Author: itay
 *
 * This module decodes UART packets and performs relevant actions such as
 * writing to flash, adjusting system parameters, or reading from flash
 * for debug purposes.
 *
 * Usage Examples:
 *
 * To set values:
 * 1. Sensor ID:          "I,1111111"     - Sets the sensor ID to 1111111.
 * 2. Sensor Type:        "T,191"         - Sets the sensor type to 123 (if in range 1-255).
 * 3. Logger ID:          "L,2222222"     - Sets the logger ID to 2222222.
 * 4. Sleep Mode:         "s1" or "s0"    - Sets ALLOW_SLEEP to true or false respectively.
 * 5. TCXO Control:       "tcxo,1" or "tcxo,0" - Enables or disables the external TCXO respectively.
 *
 * To read/debug values:
 * 1. Sensor ID:          "i"        - Reads and prints the sensor ID.
 * 2. Sensor Type:        "t"        - Reads and prints the sensor type.
 * 3. Logger ID:          "l"        - Reads and prints the logger ID.
 *
 * Additional Commands:
 * 1. Delete All Flash Pages: "monitor_delete"  - Erases all hardcoded flash pages.
 * 2. Reset Logger Data     : "logger_delete"   - Resets the logger data.
 */

#include "libraries/UART/uart_decoder.h"

#include "libraries/flash_storage/flash_storage.h"
#include "libraries/tcxo_handler/tcxo_handler.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/RADIO/rf_state_machine.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>    // for printf


bool uart_decoder(const char *packet)
{
  if (packet == NULL)
    return false;

  if (packet[0] == '?')
    {
      print_all_values_from_flash();
      return true;
    }

  if (strcmp(packet, "help") == 0)
    {
      usage_guide();
      return true;
    }

  if (strcmp(packet, "monitor_delete") == 0)
    {
      eraseAllHardcodedFlashPages();
      printf("All hardcoded flash pages have been erased.\n");
      return true;
    }

  if (strcmp(packet, "logger_delete") == 0)
    {
      eraseFlashPage(LOGGER_ID_FLASH_PAGE_ADDRESS);      // Erase LOGGER_ID_FLASH_PAGE
      printf("Logger data has been reset.\n");
      return true;
    }

  if (decode_tcxo_control(packet))
    return true;

  if (decode_sensor_id(packet))
    return true;

  if (decode_sensor_type(packet))
    return true;

  if (decode_logger_id(packet))
    return true;

  if (decode_sleep_control(packet))
    return true;

  return false; // No known packet type was matched.
}

void usage_guide()
{
  printf("====================================\n");
  printf("       UART Decoder Usage Guide\n");
  printf("====================================\n");
  printf("\n");

  printf("To set values:\n");
  printf("1. Set Sensor ID:        \"I,[Sensor_ID]\"    - Example: \"I,1111111\"\n");
  printf("2. Set Sensor Type:      \"T,[Sensor_Type]\"  - Example: \"T,191\" (Range: 1-255)\n");
  printf("4. Set Logger ID:        \"L,[Logger_ID]\"    - Example: \"L,2222222\"\n");
  printf("\n");

  printf("To read/debug values:\n");
  printf("1. Read Sensor ID:          \"i\"   - Outputs the current Sensor ID.\n");
  printf("2. Read Sensor Type:        \"t\"   - Outputs the current Sensor Type.\n");
  printf("4. Read Logger ID:          \"l\"   - Outputs the current Logger ID.\n");
  printf("\n");

  printf("To read all values at once:\n");
  printf("1. Output All Values:       \"?\"   - Outputs Sensor ID, Sensor Type, and Logger ID.\n");
  printf("\n");

  printf("To control sleep state:\n");
  printf("1. Enable Sleep:         \"s1\"  - Sets ALLOW_SLEEP to TRUE.\n");
  printf("2. Disable Sleep:        \"s0\"  - Sets ALLOW_SLEEP to FALSE.\n");
  printf("\n");

  printf("To control TCXO:\n");
  printf("1. Enable TCXO:          \"tcxo,1\"  - Enables the external TCXO.\n");
  printf("2. Disable TCXO:         \"tcxo,0\"  - Disables the external TCXO.\n");
  printf("\n");

  printf("To delete all hardcoded flash pages:\n");
  printf("1. Delete Flash Pages:   \"monitor_delete\"  - Erases all hardcoded flash pages.\n");
  printf("\n");

  printf("To reset logger data flash page:\n");
  printf("1. Reset Logger Data:    \"logger_delete\"  - Resets the logger data.\n");
  printf("\n");

  printf("Note: For all set commands, ensure no spaces and use appropriate ranges/values as indicated.\n");
  printf("====================================\n");
}

void print_all_values_from_flash()
{
  // Read Sensor ID from Flash and Print
  sensorDetails.sensorID = readFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS);
  printf("Sensor ID: %lu\n", sensorDetails.sensorID);

  // Read Sensor Type from Flash and Print
  sensorDetails.sensorType = readFlash_uint8(SENSOR_TYPE_FLASH_PAGE_ADDRESS);
  printf("Sensor Type: %u\n", sensorDetails.sensorType);

  // Print Firmware Version from sensorDetails
  printf("Firmware char_index: %c\nmonth: %u\nyear: %u\nversion_index: %u\n",
         sensorDetails.fwVersionType, sensorDetails.fwVersionMonth,
         sensorDetails.fwVersionYear, sensorDetails.fwVersionIndex);

  // Read Logger ID from Flash and Print
//  sensorDetails.loggerID = readFlash_uint32(LOGGER_ID_FLASH_PAGE_ADDRESS);
//  printf("Logger ID: %lu\n", sensorDetails.loggerID);
}

bool decode_sensor_id(const char *packet)
{
  if (packet[0] == 'I' && packet[1] == ',')
    {
      const char* id_str = &packet[2];
      uint32_t sensorID = (uint32_t)strtoul(id_str, NULL, 10);
      if (sensorID == 0)
        return false;

      writeFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS, sensorID);
      return true;
    }
  else if (packet[0] == 'i')
    {
      uint32_t sensorID = readFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS);
      printf("Sensor ID: %lu\n", sensorID);
      return true;
    }
  return false;
}

bool decode_sensor_type(const char *packet)
{
  if (packet[0] == 'T' && packet[1] == ',')
    {
      const char* type_str = &packet[2];
      uint32_t sensorType = (uint32_t)strtoul(type_str, NULL, 10);
      if (sensorType == 0 || sensorType > 255)
        return false;

      writeFlash_uint8(SENSOR_TYPE_FLASH_PAGE_ADDRESS, (uint8_t)sensorType);
      return true;
    }
  else if (packet[0] == 't')
    {
      uint8_t sensorType = readFlash_uint8(SENSOR_TYPE_FLASH_PAGE_ADDRESS);
      printf("Sensor Type: %u\n", sensorType);
      return true;
    }
  return false;
}

bool decode_logger_id(const char *packet)
{
  if (packet[0] == 'L' && packet[1] == ',')
    {
      const char* id_str = &packet[2];
      uint32_t loggerID = (uint32_t)strtoul(id_str, NULL, 10);
      if (loggerID == 0)
        return false;

      writeFlash_uint32(LOGGER_ID_FLASH_PAGE_ADDRESS, loggerID);
      return true;
    }
  else if (packet[0] == 'l')
    {
      uint32_t loggerID = readFlash_uint32(LOGGER_ID_FLASH_PAGE_ADDRESS);
      printf("Logger ID: %lu\n", loggerID);
      return true;
    }
  return false;
}

bool decode_sleep_control(const char *packet)
{
  if (strcmp(packet, "s1") == 0)
    {
      ALLOW_SLEEP = true;
      printf("ALLOW_SLEEP set to TRUE\n");
      return true;
    }
  else if (strcmp(packet, "s0") == 0)
    {
      ALLOW_SLEEP = false;
      printf("ALLOW_SLEEP set to FALSE\n");
      return true;
    }
  return false;
}

bool decode_tcxo_control(const char *packet)
{
  if (strcmp(packet, "tcxo,1") == 0)
    {
      enable_tcxo();
      printf("TCXO Enabled\n");
      return true;
    }
  else if (strcmp(packet, "tcxo,0") == 0)
    {
      disable_tcxo();
      printf("TCXO Disabled\n");
      return true;
    }
  return false;
}
