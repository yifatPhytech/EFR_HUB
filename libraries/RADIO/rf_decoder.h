/*
 * rf_decoder.h
 *
 *  Created on: Nov 1, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_RADIO_RF_DECODER_H_
#define LIBRARIES_RADIO_RF_DECODER_H_

#include <stdbool.h>   // Include standard boolean types
#include <stdint.h>

extern bool SENSOR_TO_LOGGER_ACK_RECEIVED;

// Function prototypes
void print_received_packet(uint8_t *packet, uint16_t packet_length);
void decode_rf_packet(uint8_t *packet, uint16_t packet_length);
void message_handler(uint8_t msgType, uint32_t senderID);

#endif /* LIBRARIES_RADIO_RF_DECODER_H_ */
