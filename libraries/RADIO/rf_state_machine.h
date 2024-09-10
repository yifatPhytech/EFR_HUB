/*
 * rf_state_machine.h
 *
 *  Created on: Nov 5, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_RF_STATE_MACHINE_RF_STATE_MACHINE_H_
#define LIBRARIES_RF_STATE_MACHINE_RF_STATE_MACHINE_H_

#include "libraries/RADIO/radio_handler.h"
#include <stdbool.h>  // Include the library for the bool data type

extern bool ALLOW_SLEEP;          // Declare the external variable for allowing sleep
extern bool SLEEP_IMMEDIATELY;    // Declare the external variable for immediate sleep

typedef enum
{
  IDLE_MODE,
  SEND_MESSAGE,
  WAIT_FOR_ACK,
  SLEEP_IMEDITLY
} RFState;

void init_rf_state_machine(void);
void rf_state_machine(RAIL_Handle_t rail_handle);  // Modified to include rail_handle argument
void set_rf_state(RFState new_state);

#endif /* LIBRARIES_RF_STATE_MACHINE_RF_STATE_MACHINE_H_ */
