/*
 * system_mode.c
 *
 *  Created on: Oct 16, 2023
 *      Author: itay
 */

#include "libraries/system_mode/system_mode.h"

#include "libraries/sensor_processing/sensor_processing.h"
#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/flash_storage/flash_storage.h"


// Private Variables
static system_mode_t currentSystemMode = MONITOR_MODE;

// Public Variables
uint32_t WAKEUP_TIME_PERIOD;                                // Wake-up time period (seconds)
uint32_t SECONDS_BEFORE_SLEEP_AGAIN;                        // Time duration before next sleep
uint32_t MAXIMUM_INTERVALS;                                 // Maximum EM4 wakeups before counter reset
sensorInfo_t sensorDetails;

void setSystemMode(system_mode_t mode)
{
  currentSystemMode = mode;
  printf("\r\nSystem Mode set to: %s\n", getSystemModeName(currentSystemMode));
}

system_mode_t getSystemMode(void)
{
  return currentSystemMode;
}

const char* getSystemModeName(system_mode_t mode)
{
  switch (mode)
  {
    case ACTIVE_MODE:   return "Active Mode";
    case MONITOR_MODE:  return "Monitor Mode";
    default:            return "Unknown Mode";
  }
}

bool initialize_sensor_details(void)
{
  // Read sensorID from flash and store in sensorDetails
  sensorDetails.sensorID = readFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS);

  // Read loggerID from flash and store in sensorDetails
//  sensorDetails.loggerID = readFlash_uint32(LOGGER_ID_FLASH_PAGE_ADDRESS); // Assuming a similar function exists for reading loggerID

  // Read sensorType from flash and store in sensorDetails
//  sensorDetails.sensorType = readFlash_uint8(SENSOR_TYPE_FLASH_PAGE_ADDRESS);

  // Hardcode the firmware version details into sensorDetails
  sensorDetails.fwVersionType = 'h'; // Replace with actual value
  sensorDetails.fwVersionMonth = 9; // Replace with actual value (for November)
  sensorDetails.fwVersionYear = 24;  // Replace with actual value (for 2023)
  sensorDetails.fwVersionIndex = 1;  // Replace with actual value for version index

  // Validate sensor and logger details
  if (sensorDetails.sensorID < 1000000 || sensorDetails.sensorID > 50000000)
    {
      printf("\r\nInvalid sensorID: %lu\n", sensorDetails.sensorID);
      return false;
    }

  // Validate loggerID. If it's outside the range [1000000, 9999999] and not already set to 0xFFFFFFFF,
  // reset it to 0xFFFFFFFF and save the updated value to flash.
//  if (sensorDetails.loggerID != 4294967295 && (sensorDetails.loggerID < 1000000 || sensorDetails.loggerID > 9999999))
//    {
//      printf("\r\nInvalid loggerID: %lu\n setting loggerID to 0xFFFFFFFF", sensorDetails.loggerID);
//      sensorDetails.loggerID = 4294967295; // 0xFFFFFFFF
//      writeFlash_uint32(LOGGER_ID_FLASH_PAGE_ADDRESS, sensorDetails.loggerID);       // Update loggerID in flash
//    }

//  if (sensorDetails.sensorType == 0 || sensorDetails.sensorType >= 255)
//    {
//      printf("\r\nInvalid sensorType: %u\n", sensorDetails.sensorType);
//      return false;
//    }

  // No validation needed for firmware version

  return true;
}

void print_sensor_details(void)
{
  // Print the sensor details
  printf("\r\nSensor ID: %lu\n", sensorDetails.sensorID);
//  printf("\r\nLogger ID: %lu\n", sensorDetails.loggerID); // Printing the logger ID
  printf("\r\nSensor Type: %u\n", sensorDetails.sensorType);

  // Print the firmware version
  printf("\r\nFirmware Version Type: %c\n", sensorDetails.fwVersionType);
  printf("\r\nFirmware Version Month: %u\n", sensorDetails.fwVersionMonth);
  printf("\r\nFirmware Version Year: %u\n", sensorDetails.fwVersionYear);
  printf("\r\nFirmware Version Index: %u\n", sensorDetails.fwVersionIndex);

  printf("\r\n\n");
}

bool handleOperationBySensorType(uint8_t sensorType)
{
  switch (sensorType)
  {
    case TYPE_4_20:
      return false;
      break;

    case TYPE_WATER_PRESSURE:
      return handle_wakeup_and_sensor_sampling();
      break;

    default:
      return false;
      break;
  }
}

uint32_t getSensorID(void)
{
  return sensorDetails.sensorID;
}

//uint32_t getLoggerID(void)
//{
//  return sensorDetails.loggerID;
//}

uint8_t getSensorType(void)
{
  return sensorDetails.sensorType;
}

void getFirmwareVersion(unsigned char* version)
{
  version[0] = sensorDetails.fwVersionType;
  version[1] = sensorDetails.fwVersionMonth;
  version[2] = sensorDetails.fwVersionYear;
  version[3] = sensorDetails.fwVersionIndex;
}

/* ------------------------- System Mode Functions ------------------------- */

// Set the system mode and associated parameters
void setSystemParametersBySensorType(uint8_t sensorType)
{
  switch (sensorType)
  {
    case TYPE_4_20:  // Assuming SENSOR_TYPE_4_20_MA is defined somewhere as the type code for 4-20mA sensors
      setWakeupTimePeriod(60);
      setSecondsBeforeSleepAgain(1000);
      setMaximumIntervals(60);
      break;

    case TYPE_WATER_PRESSURE:               // Replace with the actual type code for water pressure sensors
      setWakeupTimePeriod(60);              // seconds
      setSecondsBeforeSleepAgain(5000);     // milliseconds
      setMaximumIntervals(60);              // Intervals amount
      break;

    case TYPE_WATER_PRESSURE_HUB:  // Replace with the actual type code for HUB sensors
      setWakeupTimePeriod(70);
      setSecondsBeforeSleepAgain(1000);
      setMaximumIntervals(60);
      break;

      // ... other cases for other sensor types

    default:
      // Handle unknown sensor type or set default parameters
      break;
  }
}

// Get a string representation of the current system mode
const char* getSensorTypeName(uint8_t sensorType)
{
  return sensor_type_to_string(sensorType);
}


/* ---------------------- Wakeup Time Period Functions ---------------------- */

// Set the wakeup time period
void setWakeupTimePeriod(uint32_t period)
{
  WAKEUP_TIME_PERIOD = period;
}

// Retrieve the wakeup time period
uint32_t getWakeupTimePeriod(void)
{
  return WAKEUP_TIME_PERIOD;
}

/* -------------- Seconds Before Sleep Again Functions ---------------------- */

// Set the duration before the system sleeps again
void setSecondsBeforeSleepAgain(uint32_t seconds)
{
  SECONDS_BEFORE_SLEEP_AGAIN = seconds;
}

// Retrieve the time duration before the system sleeps again
uint32_t getSecondsBeforeSleepAgain(void)
{
  return SECONDS_BEFORE_SLEEP_AGAIN;
}

/* ----------------------- Maximum Intervals Functions ----------------------- */

// Set the maximum intervals for EM4 wakeups
void setMaximumIntervals(uint32_t intervals)
{
  MAXIMUM_INTERVALS = intervals;
}

// Retrieve the maximum intervals for EM4 wakeups
uint32_t getMaximumIntervals(void)
{
  return MAXIMUM_INTERVALS;
}
