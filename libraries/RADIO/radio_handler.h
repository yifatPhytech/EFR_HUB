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

//#define POWER_OUT_1   0 //1 //  -10dbm
//#define POWER_OUT_4   1 //4 //  0dbm
//#define POWER_OUT_13  2 //13  //  10dbm
//#define POWER_OUT_127 3 //127 //  20dbm

typedef enum
{
 POWER_OUT_0,     //  -10dbm
 POWER_OUT_1,     //  0dbm
 POWER_OUT_2,     //  10dbm
 POWER_OUT_3,     //  20dbm
} rf_power;

static const int16_t tx_power_dbm[] = {
    [POWER_OUT_0] = -100,
    [POWER_OUT_1] = 0,
    [POWER_OUT_2] = 100,
    [POWER_OUT_3] = 200};

void app_process_action(RAIL_Handle_t rail_handle);
void set_up_tx_fifo(RAIL_Handle_t rail_handle);
RAIL_Status_t rf_send(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len);
RAIL_Status_t rf_send_NG(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len);
RAIL_Status_t rf_send_LEGACY(RAIL_Handle_t rail_handle, uint8_t *packet, size_t packet_len);
void BufferEnvelopeTransmit();
//void ChangeRfPower(RAIL_Handle_t , rf_power);
void SetNewRfPower(rf_power newPwr);

// Add the function declaration for send_adc_results
RAIL_Status_t rf_send_adc_results(RAIL_Handle_t rail_handle, IADC_Result_t *adcResults, size_t length);

extern volatile bool tx_requested;
extern volatile bool rx_requested;
extern uint8_t out_packet[21];

#endif  // LIBRARIES_RADIO_RADIO_HANDLER_H
