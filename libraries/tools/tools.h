/*
 * tools.h
 *
 *  Created on: Oct 31, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_TOOLS_TOOLS_H_
#define LIBRARIES_TOOLS_TOOLS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h> // Include for size_t definition


// Calculates the checksum of a buffer.
uint8_t CalcChecksum(uint8_t* buf, uint8_t len);

// Converts a uint16_t value to little endian format and stores it in a buffer.
void uint16_to_little_endian(uint16_t value, uint8_t* buffer);

// Converts a uint32_t value to little endian format and stores it in a byte array.
void uint32_to_little_endian(uint32_t value, uint8_t *bytes);

// Converts 2 bytes from little endian format to uint16_t.
uint16_t little_endian_to_uint16(const uint8_t *bytes);

// Converts 4 bytes from little endian format to uint32_t.
uint32_t little_endian_to_uint32(const uint8_t *bytes);


/// ====================
///       MONITOR
/// ====================
void fetch_and_store_battery_voltage(void);
void fetch_and_store_firmware_version(void);
void fetch_and_store_chip_unique_id(void);
void store_chip_id_in_buffer(uint8_t *buffer, uint64_t chipUniqueID);
bool fetch_and_store_pressure_data(uint16_t *pressure_out);


// Assemble packets
size_t assemble_GET_ID_packet(uint8_t *buffer);
size_t assemble_MONITOR_MEASURE_packet(uint8_t *buffer, uint16_t pressure);
size_t assemble_MONITOR_ID_OK_packet(uint8_t *buffer);

void getExplosiveMessageData(int16_t current_pump_status, uint8_t *explosive_message_data);

// print data
void print_get_id_monitoring_data(void);

#endif /* LIBRARIES_TOOLS_TOOLS_H_ */
