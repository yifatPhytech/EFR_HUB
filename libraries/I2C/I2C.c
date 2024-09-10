/*
 * I2C.c
 *
 *  Created on: Nov 7, 2023
 *      Author: itay
 */

#include "libraries/I2C/I2C.h"

#include <sl_i2cspm.h>  // I2C driver
#include <stddef.h>     // Include for NULL definition

// Global or static variable to hold the I2C instance
static I2C_TypeDef *i2c;

void Initialize_I2C(void)
{
  I2CSPM_Init_TypeDef i2cspm_init =
      {
          .port = I2C0, // Use the correct I2C instance
          .sclPort = gpioPortA,
          .sclPin = 8,
          .sdaPort = gpioPortA,
          .sdaPin = 7,
          // Other initialization parameters...
      };

  I2CSPM_Init(&i2cspm_init);
  i2c = i2cspm_init.port; // Assign the initialized I2C instance to the global/static variable
}

// Function to perform I2C write using I2CSPM
bool I2C_Write(uint8_t addr, uint8_t *command, uint16_t cmd_len)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = addr << 1; // Left shift the address for I2C protocol
  seq.flags = I2C_FLAG_WRITE; // Use the correct flag for a write operation
  seq.buf[0].data = command;
  seq.buf[0].len = cmd_len;
  seq.buf[1].data = NULL; // No read operation
  seq.buf[1].len = 0;

  ret = I2CSPM_Transfer(i2c, &seq);

  return (ret == i2cTransferDone);
}

// Function to perform I2C read using I2CSPM
bool I2C_Read(uint8_t addr, uint8_t *data, uint16_t len)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  seq.addr = addr << 1; // Left shift the address for I2C protocol
  seq.flags = I2C_FLAG_READ; // Use the correct flag for a read operation
  seq.buf[0].data = data;
  seq.buf[0].len = len;
  seq.buf[1].data = NULL; // No write operation
  seq.buf[1].len = 0;

  ret = I2CSPM_Transfer(i2c, &seq);

  return (ret == i2cTransferDone);
}
