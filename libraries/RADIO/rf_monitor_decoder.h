/*
 * rf_monitor_decoder.h
 *
 *  Created on: Nov 7, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_RADIO_RF_MONITOR_DECODER_H_
#define LIBRARIES_RADIO_RF_MONITOR_DECODER_H_

#include <stdbool.h>
#include <stdint.h>

extern bool MONITOR_GET_ID_ACK_RECEIVED;
extern bool MONITOR_MEASURE_ACK_RECEIVED;

// Function prototypes for MONITOR decoder
void decode_monitor_rf_packet(uint8_t *packet, uint16_t packet_length);

#endif /* LIBRARIES_RADIO_RF_MONITOR_DECODER_H_ */
