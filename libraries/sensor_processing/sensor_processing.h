/*
 * sensor_processing.h
 *
 *  Created on: Oct 26, 2023
 *      Author: itay
 */

/*
 * BURAM (Backup RAM) Overview & Memory Locations
 * -------------------------
 *
 * BURAM (Backup RAM) is a specialized section of memory designed for retaining data
 * during low-power modes, such as deep sleep or energy mode 4 (EM4). Its primary benefits include:
 * 1. Power Efficiency: Retains data with the main power off, minimizing energy use during sleep.
 * 2. Fast Wakeup: Provides immediate data access after waking up, avoiding re-fetching/computation.
 * 3. Reliability: Safeguards critical data between power cycles or resets.
 * 4. Flexibility: Independent memory section enabling separate management for diverse needs.
 *
 * In our system, BURAM's specific usage is:
 * 1. EM4 Wakeup Counter:
 *    - Address: BURAM->RET[0].REG (WAKEUPS_INDEX_ADDR)
 *    - Purpose: Tracks EM4 wakeups and sensor sampling, guiding the next storage location.
 *
 * 2. Hourly Message Index Counter:
 *    - Address: BURAM->RET[1].REG (HOURLY_MESSAGE_INDEX_ADDR)
 *    - Purpose: Keeps track of the hourly message index, which increments with every sent message.
 *
 * 3. Sensor Values:
 *    - Addresses: BURAM->RET[10].REG to BURAM->RET[19].REG (SENSOR_VALUES_START_ADDR)
 *    - Purpose: Contains 10 consecutive locations for sensor readings upon EM4 wakeups.
 *
 * 4. Averages:
 *    - Addresses: BURAM->RET[20].REG to BURAM->RET[25].REG (AVERAGES_START_ADDR)
 *    - Purpose: Holds 6 computed averages after gathering 10 sensor readings.
 *
 * This structure supports a cyclic operation: sensor data collection, average computation,
 * and data transmission/processing.
 */

#ifndef LIBRARIES_SENSOR_PROCESSING_SENSOR_PROCESSING_H_
#define LIBRARIES_SENSOR_PROCESSING_SENSOR_PROCESSING_H_

// -------------------------
// Standard includes
// -------------------------
#include "libraries/RADIO/radio_handler.h"
#include <stdbool.h>   // Include standard boolean types
#include <stdint.h>    // Include standard integer types
#include <stdio.h>     // for printf function


extern bool is_hourly_message;
extern bool is_explosive_message;
extern int16_t current_pump_status;


// -------------------------
// Function prototypes
// -------------------------

// Initialization
void sensor_processing_init(void);
bool handle_wakeup_and_sensor_sampling(void);

// Sensor Sampling
void sample_sensor_value(void);
int16_t get_irrigation_status(uint32_t sensor_value);
uint32_t get_sensor_value(void);

// Data Processing
void compute_and_store_average(void);
void increment_hourly_message_index(void);

// Messaging
RAIL_Status_t send_averages_message(RAIL_Handle_t rail_handle);
RAIL_Status_t send_explosive_message(RAIL_Handle_t rail_handle);

#endif /* LIBRARIES_SENSOR_PROCESSING_SENSOR_PROCESSING_H_ */
