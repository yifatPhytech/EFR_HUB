/*
 * uart_decoder.h
 *
 *  Created on: Sep 27, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_UART_DECODER_UART_DECODER_H_
#define LIBRARIES_UART_DECODER_UART_DECODER_H_

#include <stdint.h>
#include <stdbool.h>

bool uart_decoder(const char *packet);
void usage_guide();
void print_all_values_from_flash();
bool decode_sensor_id(const char *packet);
bool decode_sensor_type(const char *packet);
bool decode_logger_id(const char *packet);
bool decode_sleep_control(const char *packet);
bool decode_tcxo_control(const char *packet);

#endif /* LIBRARIES_UART_DECODER_UART_DECODER_H_ */
