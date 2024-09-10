/*
 * ButtonHandler.h
 *
 *  Created on: Sep 20, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_106_BUTTONHANDLER_106_BUTTONHANDLER_H_
#define LIBRARIES_106_BUTTONHANDLER_106_BUTTONHANDLER_H_

#include "sl_simple_button_instances.h"  // Simple Button driver instances

// Function Declarations

void Init_ButtonHandler(void);                        // Initializes the button handler
void sl_button_on_change(const sl_button_t *handle);  // Callback for button state changes
bool Read_ON_OFF_Button(void);                        // Reads the ON/OFF button state

#endif /* LIBRARIES_106_BUTTONHANDLER_106_BUTTONHANDLER_H_ */
