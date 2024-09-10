/*
 * rf_monitor_decoder.c
 *
 *  Created on: Nov 7, 2023
 *      Author: itay
 */

#include "libraries/RADIO/rf_monitor_decoder.h"

#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/RADIO/rf_monitor_state_machine.h"
#include "libraries/flash_storage/flash_storage.h"
#include "libraries/tools/tools.h"
#include <stdio.h> // for printf


bool MONITOR_GET_ID_ACK_RECEIVED = false;
bool MONITOR_MEASURE_ACK_RECEIVED = false;

static void print_monitor_packet(uint8_t *packet, uint16_t packet_length);
static void handle_get_id_ack(uint8_t *packet, uint16_t packet_length);
static void handle_monitor_measure_ack(uint8_t *packet, uint16_t packet_length);

void decode_monitor_rf_packet(uint8_t *packet, uint16_t packet_length)
{
  print_monitor_packet(packet, packet_length);

  uint8_t header = packet[1];

  switch (header)
  {
    case HEADER_MON_GETID_ACK:
      handle_get_id_ack(packet, packet_length);
      break;

    case HEADER_MON_MSR_ACK:
      handle_monitor_measure_ack(packet, packet_length);
      break;

    default:
      printf("Unknown monitor header: %02X\n", header);
      break;
  }
}

static void print_monitor_packet(uint8_t *packet, uint16_t packet_length)
{
  printf("Monitor packet: ");

  for (uint16_t i = 0; i < packet_length; i++) {
      printf("%u ", packet[i]); // Print in hexadecimal
  }

  printf("\n");
}

static void handle_get_id_ack(uint8_t *packet, uint16_t packet_length)
{
  // Packet format is:
  // {Header, size, battery[0], battery[1], SensorType}
  if (packet_length >= 6)
    {
      // Extract battery level from packet
      uint16_t battery = (uint16_t)packet[3] | ((uint16_t)packet[4] << 8);

      // Check if the extracted battery level matches g_monitoringData.battery
      if (g_monitoringData.battery == battery)
        {
          uint8_t sensor_type_from_monitor = packet[5];
          if (sensor_type_from_monitor > 0 && sensor_type_from_monitor < 255)
            {
              printf("Monitor says I'm %d SensorType!\n", sensor_type_from_monitor);
              writeFlash_uint8(SENSOR_TYPE_FLASH_PAGE_ADDRESS, sensor_type_from_monitor);

              // Store the measurement inside the monitoring struct
              g_monitoringData.sensor_type = readFlash_uint8(SENSOR_TYPE_FLASH_PAGE_ADDRESS);

              // Switch MONITOR_GET_ID_ACK_RECEIVED to true
              printf("Battery level ACK received and matches local data: %u\n", battery);
              MONITOR_GET_ID_ACK_RECEIVED = true; // Indicate that the ACK was received and matches
            }
          else
            {
              printf("Received invalid SensorType value: %d\n", sensor_type_from_monitor);
            }
        }
      else
        {
          printf("Mismatch in battery level. Local: %u, Received: %u\n", g_monitoringData.battery, battery);
        }
    }
  else
    {
      printf("Invalid packet length for HEADER_GETID_ACK\n");
    }
}

static void handle_monitor_measure_ack(uint8_t *packet, uint16_t packet_length)
{
  // Packet format is:
  // {Header, size, sensor_id[0], sensor_id[1], sensor_id[2], sensor_id[3], SensorType}

  if (packet_length >= 7)
    { // Ensure packet is the expected size
      uint8_t sensor_type_from_monitor = packet[7]; // Corrected index for SensorType

      if (sensor_type_from_monitor == g_monitoringData.sensor_type)
        {
          uint32_t sensor_id_from_monitor = little_endian_to_uint32(&packet[3]);

          // Check SensorID is 7 digits
          if (sensor_id_from_monitor >= 1000000 && sensor_id_from_monitor <= 9999999)
            {
              printf("Monitor says I'm %lu SensorID!\n", sensor_id_from_monitor);
              writeFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS, sensor_id_from_monitor);

              // Sensor type matches, proceed to store sensor ID
              g_monitoringData.sensor_id = readFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS);

              // Acknowledgment received and sensor ID stored
              printf("Monitor Measure ACK received. Sensor ID stored: %lu\n", g_monitoringData.sensor_id);
              MONITOR_MEASURE_ACK_RECEIVED = true; // Indicate that the ACK was received
            }
          else
            {
              printf("Received SensorID is not a 7-digit number: %lu\n", sensor_id_from_monitor);
            }
        }
      else
        {
          printf("Mismatch in sensor type. Local: %u, Received: %u\n", g_monitoringData.sensor_type, sensor_type_from_monitor);
        }
    }
  else
    {
      printf("Invalid packet length for HEADER_MON_MSR_ACK\n");
    }
}
