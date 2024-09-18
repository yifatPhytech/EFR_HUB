/*
 * rf_state_machine.c
 *
 *  Created on: Nov 5, 2023
 *      Author: itay
 */

#include "libraries/RADIO/rf_state_machine.h"

#include "libraries/sensor_processing/sensor_processing.h"
#include "libraries/NonBlockingDelay/NonBlockingDelay.h"
#include "libraries/RADIO/rf_decoder.h"
#include <stdio.h>

// Static variables
static RFState current_state = IDLE_MODE;            // Current state of the RF state machine
static uint8_t failed_ack_counter = 0;               // Counter for failed ACKs
NonBlockingDelay ackWaitDelay;                       // Non-blocking delay instance for ACK wait

bool ALLOW_SLEEP = true;                             // Flag to allow sleep
bool SLEEP_IMMEDIATELY = false;                      // Flag to indicate immediate sleep

/**
 * @brief Initialize the RF state machine.
 */
void init_rf_state_machine(void)
{
  NonBlockingDelay_Init(&ackWaitDelay, 500);       // Initialize the ackWaitDelay with the desired interval
}

/**
 * @brief Set the current state of the RF state machine and print the new state.
 *
 * @param new_state The new state to set.
 */
void set_rf_state(RFState new_state)
{
  current_state = new_state;
  switch (current_state)
  {
    case IDLE_MODE:      printf("\r\nNew State: IDLE_MODE\n");       break;
    case SEND_MESSAGE:   printf("\r\nNew State: SEND_MESSAGE\n");    break;
    case WAIT_FOR_ACK:   printf("\r\nNew State: WAIT_FOR_ACK\n");    break;
    case SLEEP_IMEDITLY: printf("\r\nNew State: SLEEP_IMEDITLY\n");  break;
    default:             printf("\r\nUnknown state\n");              break;
  }
}

/**
 * @brief RF state machine logic.
 *
 * @param rail_handle Handle for radio operations.
 */
void rf_state_machine(RAIL_Handle_t rail_handle)
{
  switch (current_state)
  {
    case IDLE_MODE:
      if (is_hourly_message == true || is_explosive_message == true)
        {
          set_rf_state(SEND_MESSAGE);
        }
      else
        {
          set_rf_state(SLEEP_IMEDITLY);
        }
      break;

    case SEND_MESSAGE:
      if (is_hourly_message)
        {
          send_averages_message(rail_handle);
          NonBlockingDelay_reset(&ackWaitDelay);  // Reset the delay timer when sending a message
          set_rf_state(WAIT_FOR_ACK);
        }
      else if (is_explosive_message)
        {
          send_explosive_message(rail_handle);
          NonBlockingDelay_reset(&ackWaitDelay);  // Reset the delay timer when sending a message
          set_rf_state(WAIT_FOR_ACK);
        }
      break;

    case WAIT_FOR_ACK:
      if (SENSOR_TO_LOGGER_ACK_RECEIVED && is_hourly_message)
        {
          increment_hourly_message_index();                 // Increment the hourly message index counter before sending the message
          SENSOR_TO_LOGGER_ACK_RECEIVED = false;
          is_hourly_message = false;
          failed_ack_counter = 0;                           // Reset the counter when ACK is received
          set_rf_state(SLEEP_IMEDITLY);
        }
      else if (SENSOR_TO_LOGGER_ACK_RECEIVED && is_explosive_message)
        {
          increment_hourly_message_index();                 // Increment the hourly message index counter before sending the message
          SENSOR_TO_LOGGER_ACK_RECEIVED = false;
          is_explosive_message = false;
          failed_ack_counter = 0;  // Reset the counter when ACK is received
          set_rf_state(SLEEP_IMEDITLY);
        }
      else if (NonBlockingDelay_check(&ackWaitDelay))  // Check if the delay has passed
        {
          failed_ack_counter++;  // Increment the failed ACK counter
          if (failed_ack_counter > 3)
            {
              printf("\r\nI failed to receive ACK message/n");
              is_hourly_message = false;
              is_explosive_message = false;
              set_rf_state(SLEEP_IMEDITLY);  // Go to SLEEP_IMEDITLY state after 3 failed ACKs
            }
          else
            {
              set_rf_state(SEND_MESSAGE);  // Return back to SEND_MESSAGE state
            }
        }
      break;

    case SLEEP_IMEDITLY:
      if (is_hourly_message == false && is_explosive_message == false)
        {
          SLEEP_IMMEDIATELY = true;
        }
      else
        {
          set_rf_state(IDLE_MODE);  // Return back to IDLE_MODE state
        }
      break;

    default:
      break;
  }
}
