/*
 * sensor_processing.c
 *
 *  Created on: Oct 26, 2023
 *      Author: itay
 */
#include "libraries/sensor_processing/sensor_processing.h"    // Custom library for sensor processing functions and definitions

#include "libraries/system_mode/system_mode.h"
#include "libraries/RADIO/rf_protocol.h"
#include "libraries/ABP2_Sensor/ABP2_Sensor.h"
#include "libraries/tools/tools.h"
#include "em_burtc.h"                                         // Library for Backup Real-Time Counter (BURTC) peripheral access
#include "em_cmu.h"                                           // Library for Clock Management Unit (CMU) peripheral access
#include "em_rmu.h"                                           // Library for Reset Management Unit (RMU) peripheral access


bool is_hourly_message = false;  // initial value is set to false
bool is_explosive_message = false;  // initial value is set to false
int16_t current_pump_status;

// -------------------------
// Constants
// -------------------------
#define WAKEUPS_INDEX_ADDR                      (&BURAM->RET[0].REG)      // Address for EM4 wakeup counter in BURAM
#define HOURLY_MESSAGE_INDEX_ADDR               (&BURAM->RET[1].REG)      // Address for hourly message index counter in BURAM
#define SENSOR_VALUES_START_ADDR                (&BURAM->RET[10].REG)     // Start address for sensor values in BURAM
#define AVERAGES_START_ADDR                     (&BURAM->RET[20].REG)     // Start address for average values in BURAM

#define EXPLOSIVE_THRESHOLD_OPEN_PUMP       28   //
#define EXPLOSIVE_THRESHOLD_CLOSE_PUMP      20   //
#define IRRIGATION_STATUS_ON    -63
#define IRRIGATION_STATUS_OFF   -62
#define IRRIGATION_NO_STATUS    -1

// Function prototypes
static void BURAM_init(void);
static void clear_BURAM(void);
static void print_BURAM_debug_info(void);

static void BURAM_init(void)
{
  // Enable clock for BURAM
  CMU_ClockEnable(cmuClock_BURAM, true);
}

static void clear_BURAM(void)
{
  *WAKEUPS_INDEX_ADDR = 0;                    // Reset EM4 wakeup counter
  *HOURLY_MESSAGE_INDEX_ADDR = 1;             // Reset hourly message index counter

  // Clear sensor values
  for (uint8_t i = 0; i < 10; i++)
    {
      SENSOR_VALUES_START_ADDR[i] = 0;
    }

  // Clear average values
  for (uint8_t i = 0; i < 6; i++)
    {
      AVERAGES_START_ADDR[i] = 0;
    }
}

static void print_BURAM_debug_info(void)
{
  // Print the number of EM4 wakeups
  printf("\n-------- BURAM Debug Information --------\n");

  // Display the EM4 wakeup counter
  printf("-- Number of EM4 wakeups = %ld \n", *WAKEUPS_INDEX_ADDR);

  // Display the hourly message index counter
  printf("-- Hourly Message Index = %ld \n", *HOURLY_MESSAGE_INDEX_ADDR); // Added line

  // Display the 10 sensor samples
  printf("\n---- 10 Sensor Samples ----\n");
  for (uint8_t i = 0; i < 10; i++)
    {
      printf("Sensor Value %d: %lu\n", i + 1, SENSOR_VALUES_START_ADDR[i]);
    }
  printf("---------------------------\n");

  // Display the 6 average values
  printf("\n---- 6 Average Values ----\n");
  for (uint8_t i = 0; i < 6; i++)
    {
      printf("Average Value %d: %lu\n", i + 1, AVERAGES_START_ADDR[i]);
    }
  printf("-------------------------\n");
  printf("-------------------------------------------\n");
}

// -------------------------
// BURAM Initialization
// -------------------------
void sensor_processing_init(void)
{
  BURAM_init();
}


// -------------------------
// Wakeup and Sensor Sampling Handlers
// -------------------------
/**
 * @brief Handles wakeup events, samples sensor values, and provides debug information.
 *
 * This function manages the wakeup events, especially the EM4 (Energy Mode 4) wakeup.
 * On an EM4 wakeup event, it samples the sensor value. If 6 average sensor values have been
 * stored (indicated by is_hourly_message) or if an explosive condition is detected
 * (indicated by is_explosive_message), the function is set to return true. For other reset
 * causes, it clears the BURAM (Backup RAM) and provides debug information related to BURAM.
 *
 * @return Returns true if 6 average sensor values are stored or an explosive condition is detected.
 *         Otherwise, returns false.
 */
bool handle_wakeup_and_sensor_sampling(void)
{
  // Get and clear the reset cause
  uint32_t cause = RMU_ResetCauseGet();
  printf("Reset cause: %lu\n", cause);
  RMU_ResetCauseClear();

  bool result = false;

  // Handle EM4 wakeup
  if (cause == 4)
    {
      sample_sensor_value();
      // If 6 average values are stored or explosive condition detected, set result to true
      if(is_hourly_message || is_explosive_message)
        {
          result = true;
        }
    }
  // Handle PIN reset
  //else if (cause & EMU_RSTCAUSE_PIN)
  else
    {
      clear_BURAM();
    }

  // Call the debug print function
  print_BURAM_debug_info();

  return result;
}


// -------------------------
// Sensor Sampling
// -------------------------
uint32_t get_sensor_value(void)
{
  //  double pressure = 0.0; // Pressure in bars
  //  double temperature = 0.0;
  //
  //  // Call the function to read pressure and temperature
  //  bool success = ABP2_ReadPressureAndTemperature(&pressure, &temperature);
  //
  //  if (success)
  //    {
  //      // Convert pressure from bars to millibars
  //      pressure *= 1000.0; // 1 bar = 1000 millibars
  //
  //      // Assuming the sensor's pressure range and resolution allow it to be
  //      // safely represented as an integer in millibars
  //      // You might need to adjust the casting depending on the expected range and resolution
  //      return (uint32_t)pressure;
  //    }
  //  else
  //    {
  //      // #TODO Handle the error, for example, by returning a default value or an error code
  //      return 0xFFFFFFFF; // Indicate that an error has occurred
  //    }
  return *WAKEUPS_INDEX_ADDR;
}

/**
 * @brief Function to determine irrigation status based on the sensor value.
 *
 * This function analyzes the provided sensor value and determines whether the irrigation
 * system should be turned on, turned off, or if there is no conclusive status.
 *
 * @param sensor_value The sensor value to be analyzed.
 * @return The irrigation status: IRRIGATION_STATUS_ON, IRRIGATION_STATUS_OFF, or IRRIGATION_NO_STATUS.
 */
int16_t get_irrigation_status(uint32_t sensor_value)
{
  // Conditions to open the pump and start irrigation
  if (sensor_value > EXPLOSIVE_THRESHOLD_OPEN_PUMP)
    {
      return IRRIGATION_STATUS_ON;
    }
  // Conditions to close the pump and stop irrigation
  else if (sensor_value < EXPLOSIVE_THRESHOLD_CLOSE_PUMP)
    {
      return IRRIGATION_STATUS_OFF;
    }
  else
    {
      // Default case where no specific action is required
      return IRRIGATION_NO_STATUS;
    }
}

void sample_sensor_value(void)
{
  // Get the current sample count
  uint32_t wakeup_count = *WAKEUPS_INDEX_ADDR;

  // Increment the wakeup count
  wakeup_count++;
  *WAKEUPS_INDEX_ADDR = wakeup_count;

  // Get the current sensor value and store it in the appropriate index
  uint32_t current_sensor_value = get_sensor_value();
  uint8_t index = (wakeup_count - 1) % 10; // Index for current sensor value
  SENSOR_VALUES_START_ADDR[index] = current_sensor_value;

  // Assess change in irrigation status with the current and the previous sensor value
  current_pump_status = get_irrigation_status(current_sensor_value);
  // Print current pump status for debugging
  printf("Debug: Current pump status: %d\n", current_pump_status);

  uint32_t prev_sensor_value = SENSOR_VALUES_START_ADDR[(index == 0) ? 9 : index - 1]; // Get the previous sensor value
  int16_t prev_pump_status = get_irrigation_status(prev_sensor_value);
  // Print previous pump status for debugging
  printf("Debug: Previous pump status: %d\n", prev_pump_status);

  // Calculate if there is an explosive message condition every time a sample is taken
  is_explosive_message = ((current_pump_status != prev_pump_status) && (current_pump_status != IRRIGATION_NO_STATUS));

  // Only display explosive message when status changes, not when there's no status
  if (is_explosive_message) printf("Explosive message! Current pump status: %d\n", current_pump_status);

  // Every 10 wakeups, compute and store the average
  if (wakeup_count % 10 == 0)
    {
      compute_and_store_average();
    }

  // If MAXIMUM_INTERVALS wakeups are reached, reset counter and set hourly message flag
  if (wakeup_count >= MAXIMUM_INTERVALS)
    {
      is_hourly_message = true;
      *WAKEUPS_INDEX_ADDR = 0; // Reset the wakeup count
    }
}


// -------------------------
// Data Processing
// -------------------------
void compute_and_store_average(void)
{
  // Compute the average of 10 sensor values
  uint32_t sum = 0;
  for (uint8_t i = 0; i < 10; i++)
    {
      sum += SENSOR_VALUES_START_ADDR[i];
    }
  uint32_t average = sum / 10;

  // Determine which average slot to use based on the wakeup count
  uint32_t wakeup_count = *WAKEUPS_INDEX_ADDR;
  uint8_t slot = (wakeup_count / 10) - 1; // Calculate the slot index
  AVERAGES_START_ADDR[slot] = average;
}

void increment_hourly_message_index(void)
{
  // Increment the counter
  (*HOURLY_MESSAGE_INDEX_ADDR)++;

  // Check if the counter exceeds 255 and reset it
  if (*HOURLY_MESSAGE_INDEX_ADDR > 255)
    {
      *HOURLY_MESSAGE_INDEX_ADDR = 0;
    }
}


/// ----------------------------
///   Messaging  NG Protocol!
/// ----------------------------
RAIL_Status_t send_averages_message(RAIL_Handle_t rail_handle)
{
  // Print the average values
  printf("Sending Average Values:\n");

  int16_t averages[6];  // Use int16_t to safely handle values up to 32767

  // Populate the averages array and clear the values in BURAM
  for (uint8_t i = 0; i < 6; i++)
    {
      // Ensure that the value fits into int16_t
      if (AVERAGES_START_ADDR[i] <= 32767)
        {
          printf("Average %d: %lu\n", i+1, AVERAGES_START_ADDR[i]);
          averages[i] = (int16_t)AVERAGES_START_ADDR[i];
        }
      else
        {
          // Handle the error if the value is too large
          printf("Error: Average %d is too large to be handled as int16_t\n", i+1);
          // You might want to handle this error appropriately, perhaps return an error code
          // return RAIL_STATUS_ERROR;
        }

      AVERAGES_START_ADDR[i] = 0;  // Clear the average value after printing
    }

  // Retrieve the hourly message index counter from BURAM
  uint8_t hourly_message_index = (uint8_t)*HOURLY_MESSAGE_INDEX_ADDR;

  // Now, send the packet with the averages included and the hourly message index
  return send_HOURLY_MESSAGE_NG(rail_handle, averages, hourly_message_index);
}

RAIL_Status_t send_explosive_message(RAIL_Handle_t rail_handle)
{
  // Print the current pump status
  printf("Sending Current Pump Status: %d\n", current_pump_status);

  // Retrieve the hourly message index counter from BURAM
  uint8_t hourly_message_index = (uint8_t)*HOURLY_MESSAGE_INDEX_ADDR;

  // Now, send the packet with the current pump status and the hourly message index
  return send_EXPLOSIVE_MESSAGE_NG(rail_handle, current_pump_status, hourly_message_index);
}
