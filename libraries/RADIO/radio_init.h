#ifndef LIBRARIES_RADIO_RADIO_INIT_H
#define LIBRARIES_RADIO_RADIO_INIT_H

#include "rail.h"

/**************************************************************************//**
 * The function is used for some basic initialization related to the app.
 *
 * @param None
 * @returns RAIL_Handle_t RAIL handle
 *
 * It ensures the followings:
 * - Start RAIL reception
 *****************************************************************************/
RAIL_Handle_t Initialize_RADIO(void);
void radio_sleep_configuration(RAIL_Handle_t railHandle);

void set_radio_to_sleep_mode(RAIL_Handle_t railHandle);
bool set_radio_to_idle_mode(RAIL_Handle_t rail_handle);
void set_radio_to_rx_mode(RAIL_Handle_t rail_handle);
void set_radio_to_wakeup_mode(void);

#endif  // LIBRARIES_RADIO_RADIO_INIT_H
