/*
 * rf_decoder.c
 *
 *  Created on: Nov 1, 2023
 *      Author: itay
 */

#include "libraries/RADIO/rf_decoder.h"

#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/flash_storage/flash_storage.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/tools/tools.h"
#include "libraries/CommonHeaders/GlobalDefine.h"
#include <string.h> // for strcmp
#include <stdio.h> // for printf
#include "libraries/Hub_Definition/hub_define.h"

bool SENSOR_TO_LOGGER_ACK_RECEIVED = false;

void print_received_packet(uint8_t *packet, uint16_t packet_length)
{
  printf("Received packet: ");

  for (uint16_t i = 0; i < packet_length; i++)
    {
      printf("%u ", packet[i]);
    }

  printf("\n");
}

void decode_rf_packet(uint8_t *packet, uint16_t packet_length)
{
  // print the received packet
  print_received_packet(packet, packet_length);

  uint32_t senderID = little_endian_to_uint32(&packet[1]);
  uint32_t addresseeID = little_endian_to_uint32(&packet[5]);
  uint8_t msgType = packet[9];

  printf("Received packet length: %u\n", packet[0]);
  printf("Sender ID: %lu\n", senderID);
  printf("Addressee ID: %lu\n", addresseeID);
  printf("Message Type: %s\n", header_to_string(msgType));   // Using the header_to_string function here

  // Check if the addresseeID matches with our sensorID
  if(addresseeID == getSensorID())
    {
      printf("Message approved, Message Type: %s\n", header_to_string(msgType));
      message_handler(msgType, senderID);  // Call the message handler
    }
  else
    {
      printf("Message not for me\n");
    }
}

void message_handler(uint8_t msgType, uint32_t senderID)
{
  switch(msgType)
  {
    case HEADER_SEN_LGR_ACK:
      {
        SENSOR_TO_LOGGER_ACK_RECEIVED = true;

        // Check if senderID is not equal to loggerID
        if(senderID != g_LoggerID)
          {
            g_LoggerID = senderID;
            // Save the senderID to flash as loggerID
//            writeFlash_uint32(LOGGER_ID_FLASH_PAGE_ADDRESS, senderID);
            printf("Logger ID saved: %lu\n", senderID);
          }
        else
          {
            printf("Logger ID not updated as it's the same as sender ID.\n");
          }
      }
      break;

      // ... [add other cases as necessary]
    default:
      printf("Unknown message type: %s\n", header_to_string(msgType));
      break;
  }
}
