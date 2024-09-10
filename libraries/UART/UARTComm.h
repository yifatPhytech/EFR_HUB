/*
 * UARTComm.h
 *
 *  Created on: Sep 20, 2023
 *      Author: itay
 *
 *  check for sl_iostream_init_usart_instances.c inside autogen folder
 */

#ifndef LIBRARIES_UARTCOMM_UARTCOMM_H_
#define LIBRARIES_UARTCOMM_UARTCOMM_H_

#include <stdio.h>
#include <string.h>
#include "em_chip.h"
#include "sl_iostream.h"
#include "sl_iostream_handles.h"
#include "sl_iostream_init_instances.h"

// Declare the initialization and processing functions
void UARTComm_init(bool allow_print);
void UARTComm_SendNewLine(void);
void UARTComm_process_action(void);
void UARTComm_SendString(const char *str, size_t length);
void UARTComm_SendByteArray(const char *byteArray, size_t length);
void UARTComm_Flush(void);

#endif /* LIBRARIES_UARTCOMM_UARTCOMM_H_ */
