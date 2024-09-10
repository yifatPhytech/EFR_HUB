/*
 * BlinkLED.h
 *
 *  Created on: Sep 20, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_106_BLINKLED_106_BLINKLED_H_
#define LIBRARIES_106_BLINKLED_106_BLINKLED_H_

#include <stdbool.h>  // Include library for bool data type

// LED0 Configuration
#define LED0_PORT gpioPortB  // Port for LED0
#define LED0_PIN  2          // Pin for LED0

extern bool ALLOW_BLINK;  // Global flag to allow blinking

// Function Prototypes:
void BlinkLED_init();        // Initialize the LED pins
void BlinkLED_onLED0();      // Turn LED0 on
void BlinkLED_offLED0();     // Turn LED0 off
void BlinkLED_toggleLED0();  // Toggle LED0 state

#endif /* LIBRARIES_106_BLINKLED_106_BLINKLED_H_ */
