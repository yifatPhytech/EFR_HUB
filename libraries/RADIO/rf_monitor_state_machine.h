/*
 * rf_monitor_state_machine.h
 *
 *  Created on: Nov 7, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_RADIO_RF_MONITOR_STATE_MACHINE_H_
#define LIBRARIES_RADIO_RF_MONITOR_STATE_MACHINE_H_

#include "libraries/RADIO/radio_handler.h"
#include <stdint.h> // Include this for fixed width integer types

// Define the states for the RF monitor state machine
typedef enum
{
  REPEAT_MONITOR_PROCESS,
  MONITOR_GET_ID,
  MONITOR_GET_ID_ACK,
  MONITOR_MEASURE,
  MONITOR_MEASURE_ACK,
  MONITOR_ID_OK,
  MONITOR_FINISH
} RFMonitorState;

// Struct for monitoring purposes
typedef struct MonitoringData
{
  uint32_t sensor_id;                    // Unique identifier for the sensor
  uint8_t sensor_type;                   // Type of the sensor
  uint16_t battery;                      // Battery level/voltage
  uint16_t measurement;                  // Measurement data from the sensor
  unsigned char firmwareVersion[4];      // Firmware version details: 'd', month, year, version_index
  uint64_t chipUniqueID;                 // Unique identifier for the chip
} MonitoringData;

extern bool MONITOR_LED;
extern MonitoringData g_monitoringData;  // Declare an extern instance of MonitoringData

// Function declarations
void init_rf_monitor_state_machine(void);
void rf_monitor_state_machine(RAIL_Handle_t rail_handle);

#endif /* LIBRARIES_RADIO_RF_MONITOR_STATE_MACHINE_H_ */
