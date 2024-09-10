/*
 * ABP2_Sensor.c
 *
 *  Created on: Nov 6, 2023
 *      Author: itay
 */

#include "libraries/ABP2_Sensor/ABP2_Sensor.h"

#include "libraries/I2C/I2C.h"
#include "em_gpio.h"    // Include the GPIO header
#include <stdio.h>      // for printf function

/* EOC (End-Of-Conversion) Pin Configuration
 * The EOC pin serves as an indicator from the sensor to signal the completion of data measurement.
 * It is set high by the sensor once the measurement is done and the data is ready to be read.
 */
#define EOC_PIN gpioPortC
#define EOC_PIN_NUMBER 6

static void ABP2_WaitForEOC(void);


void ABP2_Init(void)
{
  GPIO_PinModeSet(EOC_PIN, EOC_PIN_NUMBER, gpioModeInput, 0);   // Initialize the EOC pin
}

// Function to read pressure and temperature from the sensor
bool ABP2_ReadPressureAndTemperature(double *pressure, double *temperature)
{
  uint8_t data[7];
  uint8_t cmd[3] = {0xAA, 0x00, 0x00}; // Command to read the sensor

  // Send the command to the sensor
  if (!I2C_Write(ABP2_SENSOR_ADDR, cmd, sizeof(cmd))) return false; // I2C write failed

  // Wait for EOC (End Of Conversion) to go high indicating the sensor is ready
  ABP2_WaitForEOC();

  // Now that EOC is high, read the response from the sensor
  if (!I2C_Read(ABP2_SENSOR_ADDR, data, sizeof(data))) return false; // I2C read failed

  // Process the raw data to calculate pressure and temperature
  // The byte order is as per the reference code
  double press_counts = (double)(data[1] * 65536 + data[2] * 256 + data[3]);
  double temp_counts = (double)(data[4] * 65536 + data[5] * 256 + data[6]);

  // Constants from the datasheet or calibration
  double output_max = 15099494.0; // Output at maximum pressure (counts)
  double output_min = 1677722.0;  // Output at minimum pressure (counts)
  double p_max = 1.0;             // Maximum pressure range (e.g., bar, psi, kPa)
  double p_min = 0.0;             // Minimum pressure range (e.g., bar, psi, kPa)

  // Convert counts to actual pressure value
  *pressure = ((press_counts - output_min) * (p_max - p_min)) / (output_max - output_min) + p_min;

  // Convert counts to actual temperature value
  *temperature = (temp_counts * 200.0 / 16777215.0) - 50.0; // Calculate temperature in degrees Celsius

  // debug purposes
  //printf("Pressure: %f, Temperature: %f\n", *pressure, *temperature);

  return true;
}


// Internal function to wait for EOC signal
static void ABP2_WaitForEOC(void)
{
  // Poll the EOC pin until it goes high
  while (GPIO_PinInGet(EOC_PIN, EOC_PIN_NUMBER) == 1)
    {
      // #TODO add a timeout mechanism to prevent an infinite loop also add error indicator
    }
}
