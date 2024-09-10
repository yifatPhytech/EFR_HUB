/*
 * ABP2_Sensor.h
 *
 *  Created on: Nov 6, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_I2C_ABP2_SENSOR_H_
#define LIBRARIES_I2C_ABP2_SENSOR_H_

#include <stdint.h>
#include <stdbool.h>
#include <sl_i2cspm.h> // Include the I2CSPM driver header

// Sensor I2C address
#define ABP2_SENSOR_ADDR 0x28

// Function prototypes
void ABP2_Init(void);
bool ABP2_ReadPressureAndTemperature(double *pressure, double *temperature);

#endif /* LIBRARIES_I2C_ABP2_SENSOR_H_ */
