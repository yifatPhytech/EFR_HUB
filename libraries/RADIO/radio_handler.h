/*
 * radio_handler.h
 *
 * Created on: Oct 30, 2023
 * Author: itay
 */

#ifndef LIBRARIES_RADIO_RADIO_HANDLER_H
#define LIBRARIES_RADIO_RADIO_HANDLER_H

#include <libraries/106_ADC/106_adc_reader.h>
#include <stdint.h>
#include <rail.h>

void app_process_action(RAIL_Handle_t rail_handle);
void set_up_tx_fifo(RAIL_Handle_t rail_handle);
RAIL_Status_t rf_send(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len);
RAIL_Status_t rf_send_NG(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len);
RAIL_Status_t rf_send_LEGACY(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len);

// Add the function declaration for send_adc_results
RAIL_Status_t rf_send_adc_results(RAIL_Handle_t rail_handle, IADC_Result_t *adcResults, size_t length);

extern volatile bool tx_requested;
extern volatile bool rx_requested;
extern uint8_t out_packet[21];

#endif  // LIBRARIES_RADIO_RADIO_HANDLER_H
