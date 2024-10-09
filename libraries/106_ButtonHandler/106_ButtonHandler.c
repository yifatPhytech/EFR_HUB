/*
 * ButtonHandler.c
 *
 *  Created on: Sep 20, 2023
 *      Author: itay
 */

#include "libraries/RADIO/rf_state_machine.h"
#include <em_gpio.h>                                          // GPIO library
#include <libraries/106_BlinkLED/106_BlinkLED.h>
#include <libraries/106_ButtonHandler/106_ButtonHandler.h>
#include "libraries/I2C/I2C.h"

#include <stdio.h>

// Button pin and port definitions
#define BLE_BUTTON_PORT     gpioPortC                        // BLE button port
#define BLE_BUTTON_PIN      4                                // BLE button pin
#define ON_OFF_BUTTON_PORT  gpioPortC                        // ON/OFF button port
#define ON_OFF_BUTTON_PIN    5                               // ON/OFF button pin

// Function Definitions

void Init_ButtonHandler(void)
{
  // Configure BLE button as an input with pull-up and filter
  GPIO_PinModeSet(BLE_BUTTON_PORT, BLE_BUTTON_PIN, gpioModeInputPullFilter, 1);

  // Configure ON/OFF button as an input with pull-up and filter
  GPIO_PinModeSet(ON_OFF_BUTTON_PORT, ON_OFF_BUTTON_PIN, gpioModeInputPullFilter, 1);
  // Note: Debounce time configuration could be added here if required
}

/******************************************************************************
 * Button callback, triggered on button state changes.
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  // Process BLE button events
  if (handle == &sl_button_btn0)
    {
      if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)
        {
          printf("Button ble was pressed.\r\n");
        }
      else if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED)
        {
          printf("Button ble was released.\r\n");
        }
    }
  // Process ON/OFF button events
  else if (handle == &sl_button_btn1)
    {
      if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)
        {
          ALLOW_BLINK = true;
          ALLOW_SLEEP = false;
          printf("Button on_off was pressed.\r\n");
        }
      else if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED)
        {
          ALLOW_BLINK = false;
          ALLOW_SLEEP = true;
          BlinkLED_offLED0();
          printf("Button on_off was released.\r\n");
        }
    }
}

bool Read_ON_OFF_Button(void)
{
  // Read the ON/OFF button state
  bool buttonState = false;// = GPIO_PinInGet(ON_OFF_BUTTON_PORT, ON_OFF_BUTTON_PIN);
  uint8_t data[7];
  uint8_t cmd[3] = {0xAA, 0x00, 0x00}; // Command to read the sensor

  // Send the command to the sensor
  if (!I2C_Write(0x28, cmd, sizeof(cmd)))
    return false; // I2C write failed

  // Now that EOC is high, read the response from the sensor
  if (!I2C_Read(0x28, data, sizeof(data)))
    return false; // I2C read failed



  // If the button is pressed, update flags and return true
  if (buttonState == 0) // Button pressed
    {
      ALLOW_BLINK = true;
      ALLOW_SLEEP = false;
      return true;
    }
  else // Button not pressed
    {
      return false;
    }
}
