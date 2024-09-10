/*
 * rf_protocol.h
 *
 *  Created on: Oct 30, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_RF_PROTOCOL_RF_PROTOCOL_H_
#define LIBRARIES_RF_PROTOCOL_RF_PROTOCOL_H_

// Includes
#include <stdint.h>
#include "libraries/RADIO/radio_handler.h"
#include "libraries/CommonHeaders/ProtocolDef.h"

/// ====================
/// NG Protocol Definitions
/// ====================

// Protocol Header
//typedef struct _ProtocolHeader
//{
//  uint32_t  m_ID;         // sensorID
//  uint32_t  m_addressee;  // loggerID or addressee
//  uint8_t   m_Header;     // MSG Type
//  uint8_t   m_size;       // packet_length
//} PrtlHdr;                // Total: 12 bytes, Padding because the uint32_t members making it 12 bytes in total instead of 10.

// Payload from Sensor to Logger
//typedef struct _PayloadSen2Lgr
//{
//  uint16_t  m_battery;    // inner battery
//  uint8_t   m_type;       // sensorType
//  uint8_t   m_nDataCnt;   // amount of data (hourly message its 6)
//  uint8_t   m_Gnrl1;      // general 1
//  uint8_t   m_Gnrl2;      // general 2
//  int16_t   m_data[20];   // data
//} PayloadSen2Lgr;         // is 46 bytes without any additional padding.

// NG Protocol Function Declarations
RAIL_Status_t send_MONITOR_GET_ID_NG(RAIL_Handle_t rail_handle);
RAIL_Status_t send_MONITOR_MEASURE(RAIL_Handle_t rail_handle);
RAIL_Status_t send_MONITOR_ID_OK(RAIL_Handle_t rail_handle);
RAIL_Status_t send_HOURLY_MESSAGE_NG(RAIL_Handle_t rail_handle, int16_t averages[6], uint8_t hourly_message_index);
RAIL_Status_t send_EXPLOSIVE_MESSAGE_NG(RAIL_Handle_t rail_handle, int16_t current_pump_status, uint8_t hourly_message_index);

/// ====================
/// Legacy Protocol Definitions
/// ====================

// Legacy Protocol Function Declarations
// # TODO LEGACY protocol not complete.
RAIL_Status_t send_ACK_Packet_LEGACY(RAIL_Handle_t rail_handle);
RAIL_Status_t send_MONITOR_START_LEGACY(RAIL_Handle_t rail_handle);
RAIL_Status_t send_MONITOR_MEASURE_LEGACY(RAIL_Handle_t rail_handle);
RAIL_Status_t send_MONITOR_CALIBR_LEGACY(RAIL_Handle_t rail_handle);
RAIL_Status_t send_JOB_LOCATION_LEGACY(RAIL_Handle_t rail_handle);

// Active mode
RAIL_Status_t send_EXPLOSIVE_MESSAGE_LEGACY(RAIL_Handle_t rail_handle, uint8_t hourly_message_index);
RAIL_Status_t send_HOURLY_MESSAGE_LEGACY(RAIL_Handle_t rail_handle, uint8_t averages[6], uint8_t hourly_message_index);

#endif /* LIBRARIES_RF_PROTOCOL_RF_PROTOCOL_H_ */
