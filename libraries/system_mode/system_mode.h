/*
 * system_mode.h
 *
 * Description: Header file for system mode management functions and types.
 *
 * Created on: Oct 16, 2023
 * Last updated on: [insert last updated date if applicable]
 * Author: itay
 *
 * Usage:
 *   setSystemMode(MODE_4_20);
 *   SystemMode_t currentMode = getSystemMode();
 *   printf("Current mode is: %s\n", getSystemModeName(currentMode));
 *
 *   // Get system parameters
 *   uint32_t sensor_id = getSensorID();
 *   uint32_t logger_id = getLoggerID();
 *   uint8_t sensor_type = getSensorType();
 *
 *   unsigned char firmware_version[4];
 *   getFirmwareVersion(firmware_version);
 *   printf("Firmware Version: %c %u %u %u\n", firmware_version[0], firmware_version[1],
 *          firmware_version[2], firmware_version[3]);
 */

#ifndef LIBRARIES_SYSTEM_MODE_SYSTEM_MODE_H_
#define LIBRARIES_SYSTEM_MODE_SYSTEM_MODE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


/// Enums
typedef enum
{
  ACTIVE_MODE,    // The system is in active mode
  MONITOR_MODE,   // The system is in monitoring mode
} system_mode_t;

typedef struct
{
  uint32_t sensorID;    // Unique identifier for the sensor
//  uint32_t loggerID;    // Unique identifier for the logger
  uint8_t sensorType;   // Type of sensor (could be an enum or defined values)
  // Hardcoded firmware version details:
  // 'd', month, year, version index ('d' - for pressure sensor)
  unsigned char fwVersionType;
  uint8_t fwVersionMonth;
  uint8_t fwVersionYear;
  uint8_t fwVersionIndex;
} sensorInfo_t;


/// External Variables
extern uint32_t WAKEUP_TIME_PERIOD;
extern uint32_t SECONDS_BEFORE_SLEEP_AGAIN;
extern uint32_t MAXIMUM_INTERVALS;
extern sensorInfo_t sensorDetails;

/// Function Prototypes
// System Mode
void setSystemMode(system_mode_t mode);
system_mode_t getSystemMode(void);
const char* getSystemModeName(system_mode_t mode);

// Sensor details
bool initialize_sensor_details(void);
void print_sensor_details(void);
bool handleOperationBySensorType(uint8_t sensorType);

// Get system parameters
uint32_t getSensorID(void);
//uint32_t getLoggerID(void);
uint8_t getSensorType(void);
void getFirmwareVersion(unsigned char* version);

// System mode related
void setSystemParametersBySensorType(uint8_t sensorType);
const char* getSensorTypeName(uint8_t sensorType);

// Wakeup time period related
void setWakeupTimePeriod(uint32_t period);
uint32_t getWakeupTimePeriod(void);

// Seconds before sleep again related
void setSecondsBeforeSleepAgain(uint32_t seconds);
uint32_t getSecondsBeforeSleepAgain(void);

// Maximum intervals related
void setMaximumIntervals(uint32_t intervals);
uint32_t getMaximumIntervals(void);

#endif /* LIBRARIES_SYSTEM_MODE_SYSTEM_MODE_H_ */
