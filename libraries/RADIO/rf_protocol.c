/*
 * rf_protocol.c
 *
 *  Created on: Oct 30, 2023
 *      Author: itay
 */
#include <libraries/106_ADC/106_adc_reader.h>
#include "libraries/RADIO/rf_protocol.h"


#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/RADIO/rf_monitor_state_machine.h"
#include "libraries/flash_storage/flash_storage.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/ABP2_Sensor/ABP2_Sensor.h"
#include "libraries/tools/tools.h"
#include "em_system.h"  // get unique chip id -> uint64_t SYSTEM_GetUnique
#include "libraries/Hub_Definition/hub_define.h"

#define RAIL_STATUS_SENSOR_READ_FAIL 0

/// ====================
///     NG Protocol
/// ====================
RAIL_Status_t send_MONITOR_GET_ID_NG(RAIL_Handle_t rail_handle)
{
  // Packet format:
  // {Header, size, battery[0-1], firmware version[0-3], chip_id[0-7], checksum}

  // Fetch and store individual components
  fetch_and_store_battery_voltage();
  fetch_and_store_firmware_version();
  fetch_and_store_chip_unique_id();

  // Allocate buffer for the entire packet including header and checksum
  // Assuming that header is 1 byte and size is 1 byte.
  uint8_t transmitBuffer[1 + 1 + sizeof(g_monitoringData.battery) + 4 + 8 + 1];

  // Assemble the packet and get the offset for checksum
  size_t offset = assemble_GET_ID_packet(transmitBuffer);

  // Calculate and set the checksum for the packet
  transmitBuffer[offset] = CalcChecksum(transmitBuffer, offset); // Calculate checksum for everything but the last byte

  // print the get id monitoring data
  print_get_id_monitoring_data();

  // Send the packet using RF
  return rf_send(rail_handle, transmitBuffer, sizeof(transmitBuffer));
}

RAIL_Status_t send_MONITOR_MEASURE(RAIL_Handle_t rail_handle)
{
  // Packet format:
  // {Header, Size, Measure[0], Measure[1], SensorType, Checksum}

  uint16_t pressure;
  // Try to fetch the pressure data
  if (fetch_and_store_pressure_data(&pressure))
    {
      uint8_t transmitBuffer[1 + 1 + 2 + 1 + 1 + 2];  // Adjust size if your packet structure differs
      size_t packetSize = assemble_MONITOR_MEASURE_packet(transmitBuffer, pressure);
      return rf_send(rail_handle, transmitBuffer, packetSize);
    }
  else
    {
      // Handle the error if sensor read fails
      // return RAIL_STATUS_SENSOR_READ_FAIL; // Use the defined error status
      // #TODO thats temp, real sensors should dont send rf command when I2C doesnt working!
      uint8_t transmitBuffer[1 + 1 + 2 + 1 + 1 + 2];  // Adjust size if your packet structure differs
      size_t packetSize = assemble_MONITOR_MEASURE_packet(transmitBuffer, 0);
      return rf_send(rail_handle, transmitBuffer, packetSize);
    }
}

RAIL_Status_t send_MONITOR_ID_OK(RAIL_Handle_t rail_handle)
{
  // Packet format:
  // {Header, Size, SensorID[0], SensorID[1], SensorID[2], SensorID[3], SensorType, Checksum}

  uint8_t transmitBuffer[1 + 1 + 4 + 1 + 1];
  size_t packetSize = assemble_MONITOR_ID_OK_packet(transmitBuffer);

  // Send the packet using RF
  return rf_send(rail_handle, transmitBuffer, packetSize);
}

RAIL_Status_t send_HOURLY_MESSAGE_NG(RAIL_Handle_t rail_handle, int16_t averages[6], uint8_t hourly_message_index)
{
  // Packet Format:
  // {sensorID[0-3], loggerID[0-3], Header, size, battery[0-1], SensorType, DataCnt, Gnr1, Gnr2, data[0-5], checksum}

  // Define the combined packet structure for NG protocol
  typedef struct _NGPacket
  {
    PrtlHdr header;
    PayloadSen2Lgr payload;
  } NGPacket;

  NGPacket packet;

  // Fill in the header details
  packet.header.m_ID = sensorDetails.sensorID;  // Assuming sensorDetails is globally accessible
  packet.header.m_addressee = g_LoggerID;  // Assuming loggerID is part of sensorDetails
  packet.header.m_Header = HEADER_SEN_LGR;

  // Fill in the payload details
  uint32_t internal_battery = readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
  packet.payload.m_battery = (uint16_t)(internal_battery);
  packet.payload.m_type = sensorDetails.sensorType;  // Assuming sensorType is part of sensorDetails
  packet.payload.m_nDataCnt = 6;  // As there are 6 average data points
  packet.payload.m_Gnrl1 = 0;  // Assuming default values for general fields
  packet.payload.m_Gnrl2 = hourly_message_index;  // Using hourly_message_index for general field

  // Calculate the size of the relevant data
  uint8_t dataSize = sizeof(packet.header.m_ID) +
      sizeof(packet.header.m_addressee) +
      sizeof(packet.header.m_Header) +
      sizeof(packet.header.m_size) +
      sizeof(packet.payload.m_battery) +
      sizeof(packet.payload.m_type) +
      sizeof(packet.payload.m_nDataCnt) +
      sizeof(packet.payload.m_Gnrl1) +
      sizeof(packet.payload.m_Gnrl2) +
      (sizeof(int16_t) * 6);  // 6 data points

  packet.header.m_size = dataSize + 1;  // Add one byte for the checksum

  // Allocate a transmit buffer large enough to hold the entire packet plus the checksum
  uint8_t transmitBuffer[dataSize + sizeof(uint8_t)];  // Include one byte for the checksum

  // Serialize the packet into the transmitBuffer byte by byte
  uint8_t *pBuffer = transmitBuffer;

  // Header
  memcpy(pBuffer, &packet.header.m_ID, sizeof(packet.header.m_ID));
  pBuffer += sizeof(packet.header.m_ID);
  memcpy(pBuffer, &packet.header.m_addressee, sizeof(packet.header.m_addressee));
  pBuffer += sizeof(packet.header.m_addressee);
  memcpy(pBuffer, &packet.header.m_Header, sizeof(packet.header.m_Header));
  pBuffer += sizeof(packet.header.m_Header);
  memcpy(pBuffer, &packet.header.m_size, sizeof(packet.header.m_size));
  pBuffer += sizeof(packet.header.m_size);

  // Payload
  memcpy(pBuffer, &packet.payload.m_battery, sizeof(packet.payload.m_battery));
  pBuffer += sizeof(packet.payload.m_battery);
  memcpy(pBuffer, &packet.payload.m_type, sizeof(packet.payload.m_type));
  pBuffer += sizeof(packet.payload.m_type);
  memcpy(pBuffer, &packet.payload.m_nDataCnt, sizeof(packet.payload.m_nDataCnt));
  pBuffer += sizeof(packet.payload.m_nDataCnt);
  memcpy(pBuffer, &packet.payload.m_Gnrl1, sizeof(packet.payload.m_Gnrl1));
  pBuffer += sizeof(packet.payload.m_Gnrl1);
  memcpy(pBuffer, &packet.payload.m_Gnrl2, sizeof(packet.payload.m_Gnrl2));
  pBuffer += sizeof(packet.payload.m_Gnrl2);

  // Copy the averages data into the buffer
  for (uint8_t i = 0; i < 6; i++)
    {
      memcpy(pBuffer, &averages[i], sizeof(averages[i]));
      pBuffer += sizeof(averages[i]);
    }

  // Calculate and append the checksum
  uint8_t checksum = CalcChecksum(transmitBuffer, dataSize);
  *pBuffer = checksum;  // pBuffer is now at the correct position to append the checksum

  // Send the combined packet using rf_send
  return rf_send(rail_handle, transmitBuffer, dataSize + sizeof(uint8_t));  // dataSize already includes the packet data, just add the checksum byte
}

RAIL_Status_t send_EXPLOSIVE_MESSAGE_NG(RAIL_Handle_t rail_handle, int16_t current_pump_status, uint8_t hourly_message_index)
{
  // Packet Format:
  // {sensorID[0-3], loggerID[0-3], Header, size, battery[0-1], SensorType, DataCnt, Gnr1, Gnr2, data[0-1], checksum}

  // Define the combined packet structure for NG protocol
  typedef struct _NGPacket
  {
    PrtlHdr header;
    PayloadSen2Lgr payload;
  } NGPacket;

  NGPacket packet;

  // Fill in the header details
  packet.header.m_ID = sensorDetails.sensorID;  // Assuming sensorDetails is globally accessible
  packet.header.m_addressee = g_LoggerID;  // Assuming loggerID is part of sensorDetails
  packet.header.m_Header = HEADER_SEN_LGR;  // Assuming HEADER_SEN_LGR is the correct header for explosive messages

  // Fill in the payload details
  uint32_t internal_battery = readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
  packet.payload.m_battery = (uint16_t)(internal_battery);  // Convert to millivolts
  packet.payload.m_type = sensorDetails.sensorType;  // Assuming sensorType is part of sensorDetails
  packet.payload.m_nDataCnt = 1;  // Only one data point for explosive message
  packet.payload.m_Gnrl1 = 0;  // Assuming default values for general fields
  packet.payload.m_Gnrl2 = hourly_message_index;  // Using hourly_message_index for general field

  // Set the explosive data (current pump status)
  packet.payload.m_data[0] = (int16_t)current_pump_status;

  // Calculate the size of the relevant data
  uint8_t dataSize = sizeof(packet.header.m_ID) +
      sizeof(packet.header.m_addressee) +
      sizeof(packet.header.m_Header) +
      sizeof(packet.header.m_size) +
      sizeof(packet.payload.m_battery) +
      sizeof(packet.payload.m_type) +
      sizeof(packet.payload.m_nDataCnt) +
      sizeof(packet.payload.m_Gnrl1) +
      sizeof(packet.payload.m_Gnrl2) +
      sizeof(current_pump_status);  // Only one data point for explosive message

  packet.header.m_size = dataSize + 1;  // first packet_length byte and the last checksum.

  // Allocate a transmit buffer large enough to hold the entire packet
  uint8_t transmitBuffer[dataSize + sizeof(uint8_t)];  // Include one byte for the checksum

  // Serialize the packet into the transmitBuffer byte by byte
  uint8_t *pBuffer = transmitBuffer;

  // Header
  memcpy(pBuffer, &packet.header.m_ID, sizeof(packet.header.m_ID));
  pBuffer += sizeof(packet.header.m_ID);
  memcpy(pBuffer, &packet.header.m_addressee, sizeof(packet.header.m_addressee));
  pBuffer += sizeof(packet.header.m_addressee);
  memcpy(pBuffer, &packet.header.m_Header, sizeof(packet.header.m_Header));
  pBuffer += sizeof(packet.header.m_Header);
  memcpy(pBuffer, &packet.header.m_size, sizeof(packet.header.m_size));
  pBuffer += sizeof(packet.header.m_size);

  // Payload
  memcpy(pBuffer, &packet.payload.m_battery, sizeof(packet.payload.m_battery));
  pBuffer += sizeof(packet.payload.m_battery);
  memcpy(pBuffer, &packet.payload.m_type, sizeof(packet.payload.m_type));
  pBuffer += sizeof(packet.payload.m_type);
  memcpy(pBuffer, &packet.payload.m_nDataCnt, sizeof(packet.payload.m_nDataCnt));
  pBuffer += sizeof(packet.payload.m_nDataCnt);
  memcpy(pBuffer, &packet.payload.m_Gnrl1, sizeof(packet.payload.m_Gnrl1));
  pBuffer += sizeof(packet.payload.m_Gnrl1);
  memcpy(pBuffer, &packet.payload.m_Gnrl2, sizeof(packet.payload.m_Gnrl2));
  pBuffer += sizeof(packet.payload.m_Gnrl2);

  // Set the explosive data (current pump status)
  uint8_t explosive_message_data[2];
  getExplosiveMessageData(current_pump_status, explosive_message_data);
  memcpy(pBuffer, &explosive_message_data, sizeof(explosive_message_data));
  pBuffer += sizeof(explosive_message_data);

  // Calculate and append the checksum
  uint8_t checksum = CalcChecksum(transmitBuffer, dataSize);
  *pBuffer = checksum;  // pBuffer is now at the correct position to append the checksum

  // Send the combined packet using rf_send
  return rf_send(rail_handle, transmitBuffer, dataSize + sizeof(uint8_t));  // dataSize already includes the packet data, just add the checksum byte
}


/// ====================
///     LEGACY Protocol
/// ====================

RAIL_Status_t send_ACK_Packet_LEGACY(RAIL_Handle_t rail_handle)
{
  uint8_t packet[7];

  // Fetching the sensor ID from flash
  uint32_t sensorID = sensorDetails.sensorID;

  // Convert the sensorID to little endian format
  uint32_to_little_endian(sensorID, packet);

  // Setting up the ACK header
  packet[4] = 'A';
  packet[5] = 'C';
  packet[6] = 'K';

  // Send the packet using rf_send
  return rf_send(rail_handle, packet, 7);
}

RAIL_Status_t send_MONITOR_START_LEGACY(RAIL_Handle_t rail_handle)
{
  // Packet format:
  // {'G', 'E', 'T', 'I', 'D', 12, battery[0], battery[1], VerLetter, VerMonth, VerYear, VerIndex}
  uint8_t packet[12] = {'G', 'E', 'T', 'I', 'D', 12};

  // Internal battery
  uint32_t internal_battery = readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
  uint16_t battery_mV = (uint16_t)(internal_battery * 1000);  // Convert to millivolts
  uint16_to_little_endian(battery_mV, &packet[6]);   // Store battery_mV into the packet

  // Retrieve firmware version details from sensorDetails and place into packet
  packet[8] = sensorDetails.fwVersionType;
  packet[9] = sensorDetails.fwVersionMonth;
  packet[10] = sensorDetails.fwVersionYear;
  packet[11] = sensorDetails.fwVersionIndex;

  return rf_send(rail_handle, packet, sizeof(packet));
}

RAIL_Status_t send_MONITOR_MEASURE_LEGACY(RAIL_Handle_t rail_handle)
{
  // Packet format is: {'D', 'A', 'T', 'A', battery[0], battery[1], CheckSum}
  uint8_t packet[7] = {'D', 'A', 'T', 'A'};

  // Read the battery voltage
  uint32_t internal_battery = readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
  uint16_t battery_mV = (uint16_t)(internal_battery * 1000);  // Convert to millivolts

  // Store battery_mV into the packet using the utility function
  uint16_to_little_endian(battery_mV, &packet[4]);

  // Calculate checksum for the entire packet except the last byte (which will store the checksum)
  packet[6] = CalcChecksum(packet, 6);

  // Send the packet using rf_send
  return rf_send(rail_handle, packet, sizeof(packet));
}

// #TODO modify send_MONITOR_CALIBR function
RAIL_Status_t send_MONITOR_CALIBR_LEGACY(RAIL_Handle_t rail_handle)
{
  // Packet format: {'C', 'L', calibration_value[0], calibration_value[1], measure_after_Calibration[0], measure_after_Calibration[1], CheckSum}
  uint8_t packet[8] = {'C', 'L'};

  // Fetch calibration value
  //  uint16_t calibration_value = getCalibrationValue();
  //  uint16_to_little_endian(calibration_value, &packet[2]);

  // Fetch measure after calibration
  //  uint16_t measure_after_calibration = getMeasureAfterCalibration();
  //  uint16_to_little_endian(measure_after_calibration, &packet[4]);

  // Calculate checksum for the entire packet except the last byte
  packet[7] = CalcChecksum(packet, 7);

  // Send the packet using rf_send
  return rf_send(rail_handle, packet, sizeof(packet));
}

RAIL_Status_t send_EXPLOSIVE_MESSAGE_LEGACY(RAIL_Handle_t rail_handle, uint8_t hourly_message_index)
{
  // Packet format:
  // {ID[0-3], msgType, m_Type, burst_value, g_indexer, battery[0-1], LOGGER_ID[0-3], CheckSum}
  uint8_t packet[20];

  // Extract and format sensor details
  uint32_t sensorID = sensorDetails.sensorID;  // Fetch the sensor ID
  uint32_to_little_endian(sensorID, packet);   // Place the sensorID at the beginning of the packet

  packet[4] = JOB_MSR_6_DATA;                  // Define the message job type
  packet[5] = sensorDetails.sensorType;        // Set the sensor type

  // Insert the burst value of -62 into positions 6-11 of the packet
  int8_t burstValue = -62;
  for (uint8_t i = 0; i < 6; i++)
    {
      packet[6+i] = (uint8_t)burstValue;              // Insert -62 as burst value
    }

  packet[12] = hourly_message_index;           // Set the message index

  // Extract, format, and insert battery details
  uint32_t internal_battery = readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
  uint16_t battery_mV = (uint16_t)(internal_battery * 1000);  // Convert reading to millivolts
  uint16_to_little_endian(battery_mV, &packet[13]);   // Place the battery voltage in positions 13-14

  // Extract and format logger details
  uint32_to_little_endian(g_LoggerID, &packet[15]);   // Insert the loggerID into positions 15-18

  // Calculate and insert checksum at the end of the packet
  packet[19] = CalcChecksum(packet, 19);

  // Send the packet
  return rf_send(rail_handle, packet, sizeof(packet));
}


RAIL_Status_t send_HOURLY_MESSAGE_LEGACY(RAIL_Handle_t rail_handle, uint8_t averages[6], uint8_t hourly_message_index)
{
  // Packet format:
  // {sensorID[0-3], msgType, m_Type, burst_value[0-5], g_indexer, battery[0-1], LOGGER_ID[0-3], CheckSum}
  uint8_t packet[20];

  // Extract and format sensor details
  uint32_t sensorID = sensorDetails.sensorID;  // Fetch the sensor ID
  uint32_to_little_endian(sensorID, packet);   // Place the sensorID at the beginning of the packet

  packet[4] = JOB_MSR_6_DATA;                  // Define the message job type
  packet[5] = sensorDetails.sensorType;        // Set the sensor type

  // Insert the averages into positions 6-11 of the packet
  for (uint8_t i = 0; i < 6; i++)
    {
      packet[6+i] = averages[i];
    }

  packet[12] = hourly_message_index;           // Set the message index

  // Extract, format, and insert battery details
  uint32_t internal_battery = readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
  uint16_t battery_mV = (uint16_t)(internal_battery * 1000);  // Convert reading to millivolts
  uint16_to_little_endian(battery_mV, &packet[13]);   // Place the battery voltage in positions 13-14

  // Extract and format logger details
  uint32_to_little_endian(g_LoggerID, &packet[15]);   // Insert the loggerID into positions 15-18

  // Calculate and insert checksum at the end of the packet
  packet[19] = CalcChecksum(packet, 19);

  // Send the packet
  return rf_send(rail_handle, packet, sizeof(packet));
}
