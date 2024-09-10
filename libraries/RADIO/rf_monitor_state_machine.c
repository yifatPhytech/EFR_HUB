/*
 * rf_monitor_state_machine.c
 *
 * Created on: Nov 7, 2023
 * Author: itay
 */

#include "libraries/RADIO/rf_monitor_state_machine.h"

#include "libraries/NonBlockingDelay/NonBlockingDelay.h"
#include "libraries/RADIO/rf_monitor_decoder.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/RADIO/rf_protocol.h"
#include "libraries/RADIO/rf_decoder.h"
#include <em_device.h>
#include <libraries/106_BlinkLED/106_BlinkLED.h>
#include <stdio.h>

// Static variables for this state machine
MonitoringData g_monitoringData;  // Define the instance of MonitoringData
static RFMonitorState current_state = MONITOR_GET_ID;
static uint8_t failed_ack_counter = 0;               // Counter for failed ACKs

bool MONITOR_LED;
NonBlockingDelay MONITOR_REPEAT_INSTANCE;               // Non-blocking delay instance for ACK wait
NonBlockingDelay MONITOR_ackWaitDelay;               // Non-blocking delay instance for ACK wait


// Private prototype functions
static void handle_monitor_get_id(RAIL_Handle_t rail_handle);
static void handle_monitor_get_id_ack(void);
static void handle_monitor_measure(RAIL_Handle_t rail_handle);
static void handle_monitor_measure_ack(void);
static void handle_monitor_id_ok(RAIL_Handle_t rail_handle);
static void handle_monitor_finish(void);

static void print_global_monitoring_data(void);
static void set_rf_monitor_state(RFMonitorState new_state);


// Function to initialize the RF monitor state machine
void init_rf_monitor_state_machine(void)
{
  MONITOR_LED = true;
  NonBlockingDelay_Init(&MONITOR_REPEAT_INSTANCE, 10000);       // Initialize the ackWaitDelay with the desired interval
  NonBlockingDelay_Init(&MONITOR_ackWaitDelay, 10000);       // Initialize the ackWaitDelay with the desired interval
}

// Function to set the state of the RF monitor state machine
static void set_rf_monitor_state(RFMonitorState new_state)
{
  current_state = new_state;
  switch (current_state)
  {
    case REPEAT_MONITOR_PROCESS:  printf("New State: REPEAT_MONITOR_PROCESS\n");        break;
    case MONITOR_GET_ID:          printf("New State: MONITOR_GET_ID\n");                break;
    case MONITOR_GET_ID_ACK:      printf("New State: MONITOR_GET_ID_ACK\n");            break;
    case MONITOR_MEASURE:         printf("New State: MONITOR_MEASURE\n");               break;
    case MONITOR_MEASURE_ACK:     printf("New State: MONITOR_MEASURE_ACK\n");           break;
    case MONITOR_ID_OK:           printf("New State: MONITOR_ID_OK\n");                 break;
    case MONITOR_FINISH:          printf("New State: MONITOR_FINISH\n");                break;
    default:                      printf("Unknown RF Monitor State\n");                 break;
  }
}

// Main function to handle the state machine's logic
void rf_monitor_state_machine(RAIL_Handle_t rail_handle)
{
  switch (current_state)
  {
    case REPEAT_MONITOR_PROCESS:
      if (NonBlockingDelay_check(&MONITOR_REPEAT_INSTANCE))
        {
          set_rf_monitor_state(MONITOR_GET_ID);
        }
      break;

    case MONITOR_GET_ID:
      handle_monitor_get_id(rail_handle);
      break;

    case MONITOR_GET_ID_ACK:
      handle_monitor_get_id_ack();
      break;

    case MONITOR_MEASURE:
      handle_monitor_measure(rail_handle);
      break;

    case MONITOR_MEASURE_ACK:
      handle_monitor_measure_ack();
      break;

    case MONITOR_ID_OK:
      handle_monitor_id_ok(rail_handle);
      break;

    case MONITOR_FINISH:
      handle_monitor_finish();
      break;

    default:
      // Handle unknown state
      break;
  }
}

static void handle_monitor_get_id(RAIL_Handle_t rail_handle)
{
  // Send the get ID command
  send_MONITOR_GET_ID_NG(rail_handle);
  // Start waiting for ACK
  NonBlockingDelay_reset(&MONITOR_ackWaitDelay);
  set_rf_monitor_state(MONITOR_GET_ID_ACK);
}

static void handle_monitor_get_id_ack(void)
{
  if (MONITOR_GET_ID_ACK_RECEIVED == true)
    {
      // ACK Received, move to next state
      MONITOR_GET_ID_ACK_RECEIVED = false; // Reset the flag
      set_rf_monitor_state(MONITOR_MEASURE);
    }
  else if (NonBlockingDelay_check(&MONITOR_ackWaitDelay))
    {
      // ACK wait timeout occurred
      failed_ack_counter++; // Increment the failed ACK counter
      if (failed_ack_counter > 3)
        {
          // Exceeded maximum retries, handle accordingly
          printf("Failed to receive ACK for Get ID\n");
          // Reset variables and transition to a fail-safe state or retry logic
          failed_ack_counter = 0; // Reset the counter

          NonBlockingDelay_reset(&MONITOR_REPEAT_INSTANCE);
          set_rf_monitor_state(REPEAT_MONITOR_PROCESS);
        }
      else
        {
          // Retry logic, send the command again
          set_rf_monitor_state(MONITOR_GET_ID);
        }
    }
}

static void handle_monitor_measure(RAIL_Handle_t rail_handle)
{
  // Send the measure command
  send_MONITOR_MEASURE(rail_handle);
  // Start waiting for ACK
  NonBlockingDelay_reset(&MONITOR_ackWaitDelay);
  set_rf_monitor_state(MONITOR_MEASURE_ACK);
}

static void handle_monitor_measure_ack(void)
{
  if (MONITOR_MEASURE_ACK_RECEIVED == true)
    {
      // ACK Received, move to next state
      MONITOR_MEASURE_ACK_RECEIVED = false; // Reset the flag
      NonBlockingDelay_setInterval(&MONITOR_ackWaitDelay, 100);
      set_rf_monitor_state(MONITOR_ID_OK);
    }
  else if (NonBlockingDelay_check(&MONITOR_ackWaitDelay))
    {
      // ACK wait timeout occurred
      failed_ack_counter++; // Increment the failed ACK counter
      if (failed_ack_counter > 3)
        {
          // Exceeded maximum retries, handle accordingly
          printf("Failed to receive ACK for Measure\n");
          // Reset variables and transition to a fail-safe state or retry logic
          failed_ack_counter = 0; // Reset the counter

          NonBlockingDelay_reset(&MONITOR_REPEAT_INSTANCE);
          set_rf_monitor_state(REPEAT_MONITOR_PROCESS);
        }
      else
        {
          // Retry logic, send the command again
          set_rf_monitor_state(MONITOR_MEASURE);
        }
    }
}

static void handle_monitor_id_ok(RAIL_Handle_t rail_handle)
{
  // Acknowledge successful measurement
  send_MONITOR_ID_OK(rail_handle);

  // Print the monitoring data
  print_global_monitoring_data();

  //send_MONITOR_ID_OK(rail_handle);

  BlinkLED_onLED0();

  set_rf_monitor_state(MONITOR_FINISH);
  // Perform a software reset
  // NVIC_SystemReset();
}

static void handle_monitor_finish()
{
  // ** DO NOTHING  **
}

// Function to print the global MonitoringData struct
static void print_global_monitoring_data(void)
{
  // MONITORING PROCESS ARE FINISHED SUCCESSFULLY!
  printf("MONITORING PROCESS ARE FINISHED SUCCESSFULLY!\n");

  printf("Sensor ID: %lu\n", (unsigned long)g_monitoringData.sensor_id);
  printf("Sensor Type: %u\n", g_monitoringData.sensor_type);
  printf("Battery Voltage: %u mV\n", g_monitoringData.battery);
  printf("Measurement: %u\n", g_monitoringData.measurement);
  printf("Firmware Version: d%u%u%u\n",
         g_monitoringData.firmwareVersion[1], // Assuming month is the second byte
         g_monitoringData.firmwareVersion[2], // Assuming year is the third byte
         g_monitoringData.firmwareVersion[3]  // Assuming version index is the fourth byte
  );
  printf("Chip Unique ID: %llu\n", (unsigned long long)g_monitoringData.chipUniqueID);
}

