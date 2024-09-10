/*
 * tools.c
 *
 *  Created on: Oct 31, 2023
 *      Author: itay
 */

#include <libraries/106_ADC/106_adc_reader.h>
#include "libraries/tools/tools.h"

#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/RADIO/rf_monitor_state_machine.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/ABP2_Sensor/ABP2_Sensor.h"


uint8_t CalcChecksum(uint8_t* buf, uint8_t len)
{
  uint8_t i, res = 0;
  for (i = 0; i < len; i++)
    res += buf[i];
  return res;
}

void uint16_to_little_endian(uint16_t value, uint8_t* buffer)
{
  buffer[0] = value & 0xFF;        // Low byte
  buffer[1] = (value >> 8) & 0xFF; // High byte
}

void uint32_to_little_endian(uint32_t value, uint8_t *bytes)
{
  bytes[0] = value & 0xFF;
  bytes[1] = (value >> 8) & 0xFF;
  bytes[2] = (value >> 16) & 0xFF;
  bytes[3] = (value >> 24) & 0xFF;
}

uint16_t little_endian_to_uint16(const uint8_t *bytes)
{
  return (bytes[0] | (bytes[1] << 8));
}

uint32_t little_endian_to_uint32(const uint8_t *bytes)
{
  return (bytes[0] |
      (bytes[1] << 8) |
      (bytes[2] << 16) |
      (bytes[3] << 24));
}

/// ====================
///       MONITOR
/// ====================
void fetch_and_store_battery_voltage(void)
{
  g_monitoringData.battery = (uint16_t)readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
  printf("Stored Battery Voltage (in millivolts): %u\n", g_monitoringData.battery);
}

void fetch_and_store_firmware_version(void)
{
  // Assuming g_monitoringData.firmwareVersion is an array that can hold at least 4 elements.
  g_monitoringData.firmwareVersion[0] = sensorDetails.fwVersionType;
  g_monitoringData.firmwareVersion[1] = sensorDetails.fwVersionMonth;
  g_monitoringData.firmwareVersion[2] = sensorDetails.fwVersionYear;
  g_monitoringData.firmwareVersion[3] = sensorDetails.fwVersionIndex;
}

void fetch_and_store_chip_unique_id(void)
{
  g_monitoringData.chipUniqueID = SYSTEM_GetUnique();
}

void store_chip_id_in_buffer(uint8_t *buffer, uint64_t chipUniqueID)
{
  // Store the 64-bit chipUniqueID in little-endian format
  for (size_t i = 0; i < 8; i++)
    {
      buffer[i] = (uint8_t)(chipUniqueID >> (i * 8)) & 0xFF;
    }
}

bool fetch_and_store_pressure_data(uint16_t *pressure_out)
{
  double pressure_reading, temperature_reading;
  if (ABP2_ReadPressureAndTemperature(&pressure_reading, &temperature_reading))
    {
      // Successfully read the pressure and temperature, now convert the pressure
      uint16_t pressure_millibars = (uint16_t)(pressure_reading * 1000.0); // Convert bars to millibars
      if (pressure_out)
        {
          *pressure_out = pressure_millibars;
        }
      // You may still want to store the value globally as well
      g_monitoringData.measurement = pressure_millibars;
      return true; // Indicate success
    }
  else
    {
      // Log an error indicating sensor read failure
      printf("Error: Pressure and temperature read failed.\n");
      return false; // Indicate failure
    }
}


// Packet Assembly Functions
size_t assemble_GET_ID_packet(uint8_t *buffer)
{
  size_t offset = 0;
  buffer[offset++] = HEADER_MON_GETID;
  // Calculate the size of the packet: header + battery + fwVersionType + fwVersionMonth + fwVersionYear + fwVersionIndex + chip_id
  uint8_t size = 1 + sizeof(g_monitoringData.battery) + 1 + 1 + 1 + 1 + 8 + 2; // chip_id is always 8 bytes
  buffer[offset++] = size;

  // Add battery to buffer
  uint16_to_little_endian(g_monitoringData.battery, &buffer[offset]);
  offset += sizeof(g_monitoringData.battery);

  // Add firmware version details to buffer
  buffer[offset++] = sensorDetails.fwVersionType;
  buffer[offset++] = sensorDetails.fwVersionMonth;
  buffer[offset++] = sensorDetails.fwVersionYear;
  buffer[offset++] = sensorDetails.fwVersionIndex;

  // Add chip ID to buffer
  store_chip_id_in_buffer(&buffer[offset], g_monitoringData.chipUniqueID);
  offset += 8; // chip_id is always 8 bytes

  return offset; // Return the next offset for checksum to be added
}

size_t assemble_MONITOR_MEASURE_packet(uint8_t *buffer, uint16_t pressure)
{
  size_t offset = 0;

  // Header
  buffer[offset++] = HEADER_MON_MSR;

  // Size of payload
  uint8_t payloadSize = 6;
  buffer[offset++] = payloadSize;

  // Pressure (converted to little-endian format)
  uint16_to_little_endian(pressure, &buffer[offset]);
  offset += sizeof(pressure);

  // Sensor Type
  buffer[offset++] = g_monitoringData.sensor_type;

  // Checksum (calculated for the entire packet except the checksum byte itself)
  buffer[offset] = CalcChecksum(buffer, offset);  // Pass the current offset as the length for checksum calculation

  return offset + 1; // Return packet size including checksum byte
}

size_t assemble_MONITOR_ID_OK_packet(uint8_t *buffer)
{
  size_t offset = 0;
  buffer[offset++] = HEADER_MON_ID_OK; // Assuming HEADER_MON_ID_OK is defined correctly
  uint8_t size = 8; // Size of payload
  buffer[offset++] = size;
  uint32_to_little_endian(g_monitoringData.sensor_id, &buffer[offset]);
  offset += sizeof(g_monitoringData.sensor_id);
  buffer[offset++] = g_monitoringData.sensor_type;
  buffer[offset] = CalcChecksum(buffer, offset); // Calculate checksum for the packet up to this point
  return offset + 1; // Packet size including checksum
}

void getExplosiveMessageData(int16_t current_pump_status, uint8_t *explosive_message_data)
{
  // Ensure the passed array has space for 2 bytes
  explosive_message_data[0] = (uint8_t)(current_pump_status & 0xFF);          // Lower byte
  explosive_message_data[1] = (uint8_t)((current_pump_status >> 8) & 0xFF);   // Upper byte
}

void print_get_id_monitoring_data(void)
{
  // Print Battery Voltage
  printf("Battery Voltage: %u mV\n", g_monitoringData.battery);

  // Print Firmware Version
  printf("Firmware Version: %c%02u.%02u.%02u\n",
         g_monitoringData.firmwareVersion[0],  // Firmware Type
         g_monitoringData.firmwareVersion[1],  // Firmware Month
         g_monitoringData.firmwareVersion[2],  // Firmware Year
         g_monitoringData.firmwareVersion[3]); // Firmware Version Index

  // Print Chip Unique ID
  printf("Chip Unique ID: %llu\n", (unsigned long long)g_monitoringData.chipUniqueID);
}
