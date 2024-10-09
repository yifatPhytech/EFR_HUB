/*
 * BlinkLED.c
 *
 *  Created on: Sep 5, 2023
 *      Author: itay
 */

#include <libraries/106_BlinkLED/106_BlinkLED.h>
#include "em_cmu.h"     // Clock management unit library

bool ALLOW_BLINK = true;  // Global flag to control LED blinking

// Initialize the LED pin
void BlinkLED_init()
{
  CMU_ClockEnable(cmuClock_GPIO, true);                         // Enable clock for GPIO module
  GPIO_PinModeSet(LED0_PORT, LED0_PIN, gpioModePushPull, 0);    // Set LED0 as push-pull output
  BlinkLED_offLED0();                                           // Turn off LED0 initially
}

// Turn on LED0
void BlinkLED_onLED0()
{
  GPIO_PinOutSet(LED0_PORT, LED0_PIN);        // Set LED0 pin high
}

// Turn off LED0
void BlinkLED_offLED0()
{
  GPIO_PinOutClear(LED0_PORT, LED0_PIN);      // Clear LED0 pin low
}

// Toggle the state of LED0
void BlinkLED_toggleLED0()
{
  GPIO_PinOutToggle(LED0_PORT, LED0_PIN);     // Toggle LED0 pin state
}
