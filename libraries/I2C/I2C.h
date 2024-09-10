/*
 * I2C.h
 *
 *  Created on: Nov 7, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_I2C_I2C_H_
#define LIBRARIES_I2C_I2C_H_

#include <stdbool.h>
#include <stdint.h>

// I2C initialization
void Initialize_I2C(void);

// Low-level I2C communication
bool I2C_Write(uint8_t addr, uint8_t *command, uint16_t cmd_len);
bool I2C_Read(uint8_t addr, uint8_t *data, uint16_t len);

#endif /* LIBRARIES_I2C_I2C_H_ */
