/*
 * phytech_protocol.h
 *
 * This header file defines the protocol constants, enums, and functions
 * used for communication in Phytech systems. It includes definitions for
 * various message headers and sensor types used in the NG (Next Generation)
 * Protocol, as well as job identifiers and alerts used in the LEGACY Protocol.
 *
 * The NG Protocol section outlines the message header values and sensor types
 * for a modern, structured communication between devices.
 *
 * The LEGACY Protocol section defines the job types and alerts which are used
 * in existing systems and maintained for backward compatibility.
 *
 *  Created on: Oct 30, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_PHYTECH_PROTOCOL_PHYTECH_PROTOCOL_H_
#define LIBRARIES_PHYTECH_PROTOCOL_PHYTECH_PROTOCOL_H_

#include <stdint.h>

/// ====================
///     NG Protocol
/// ====================
// Enum for message headers
typedef enum _Headers
{
  HEADER_MSR_URGENT            = 0x10,  // 16
  HEADER_MSR_URGENT_ACK        = 0x11,  // 17
  HEADER_SEN_LGR               = 0x12,  // 18
  HEADER_SEN_LGR_ACK           = 0x13,  // 19
  HEADER_MON_GETID             = 0xA1,  // 161
  HEADER_MON_GETID_ACK         = 0xA2,  // 162
  HEADER_MON_MSR               = 0xA3,  // 163
  HEADER_MON_MSR_ACK           = 0xA4,  // 164
  HEADER_MON_ID_OK             = 0xA5,  // 165
  HEADER_GETID                 = 0xA6,  // 166
  HEADER_GETID_ACK             = 0xA7,  // 167
  HEADER_ID_OK                 = 0xA8,  // 168
  HEADER_SEN_LOST              = 0xB0,  // 176
  HEADER_SEN_LOST_ACK          = 0xB1,  // 177
  HEADER_MSR_ONLY_INT          = 0xB2,  // 178
  HEADER_MSR_MLT_ONLY          = 0xB4,  // 180
  HEADER_MSR_ACK_GET_PRM       = 0xB5,  // 181
  HEADER_SEN_PRM               = 0xB6,  // 182
  HEADER_SEN_PRM_ACK           = 0xB7,  // 183
  HEADER_MSR_ONLY              = 0xB8,  // 184
  HEADER_MSR_ONLY_ACK          = 0xB9,  // 185
  HEADER_SEN_CHECK_4_UPDATE    = 0xBA,  // 186
  HEADER_SEN_UPDATE_STRT_PKT   = 0xBB,  // 187
  HEADER_SEN_UPDATE            = 0xBC,  // 188
  HEADER_SEN_UPDATE_ACK        = 0xBD,  // 189
  HEADER_SEN_FCTR_RST          = 0xBE,  // 190
  HEADER_SEN_FCTR_RST_ACK      = 0xBF,  // 191
  HEADER_SND_DATA              = 0xC1,  // 193
  HEADER_SND_DATA_ACK          = 0xC2,  // 194
  HEADER_SND_DATA_ALERT        = 0xC9,  // 201
  HEADER_HUB_PRM               = 0xC3,  // 195
  HEADER_HUB_PRM_ACK           = 0xC4,  // 196
  HEADER_HUB_SNS_PRM           = 0xC5,  // 197
  HEADER_HUB_SNS_PRM_ACK       = 0xC6,  // 198
  HEADER_HUB_CHANGE_SLOT       = 0xC7,  // 199
  HEADER_HUB_CHANGE_SLOT_ACK   = 0xC8,  // 200
  HEADER_HUB_CHECK_4_UPDATE    = 0xCA,  // 202
  HEADER_HUB_UPDATE_START_PKT  = 0xCB,  // 203
  HEADER_HUB_UPDATE            = 0xCC,  // 204
  HEADER_HUB_UPDATE_ACK        = 0xCD,  // 205
  HEADER_HUB_STOP              = 0xCE,  // 206
  HEADER_HUB_STOP_ACK          = 0xCF,  // 207
  HEADER_TEST_RF               = 0xD1,  // 209
  HEADER_TEST_RF_ACK           = 0xD2   // 210
} Headers;

// Sensor Types
typedef enum _SensorTypes
{
  TYPE_IRRIGATION           = 89,     // 89
  TYPE_SMP_6                = 98,     // 98
  TYPE_ENV_80               = 108,    // 108
  TYPE_FLOW_MTR             = 109,    // 109
  TYPE_PRSR_IRG             = 111,    // 111
  TYPE_SENTEK_SOIL          = 112,    // 112
  TYPE_4_20                 = 113,    // 113
  TYPE_SMP_GNRL             = 116,    // 116
  TYPE_SMP_3                = 117,    // 117
  TYPE_SMP_4                = 118,    // 118
  TYPE_ATH                  = 119,    // 119
  TYPE_FERT_TANK_LVL        = 120,    // 120
  TYPE_WND                  = 121,    // 121
  TYPE_POND_LVL             = 130,    // 130
  TYPE_TANK_LVL_4_20        = 131,    // 131
  TYPE_FERT_FLOW_MTR        = 133,    // 133
  TYPE_MAIN_LINE_4_20       = 134,    // 134
  TYPE_SMALL_FI             = 141,    // 141
  TYPE_AT_COOLING           = 142,    // 142
  TYPE_WATER_PRESSURE_HUB   = 143,    // 143 (Added)
  TYPE_WATER_PRESSURE       = 191     // 191
} SensorTypes;


/// ====================
///     LEGACY Protocol
/// ====================
// LEGACY Protocol Enums
typedef enum _JobIdentifiers
{
  JOB_LOCATION           = 0,    // 0
  JOB_MSR                = 1,    // 1
  JOB_MONITOR            = 2,    // 2
  JOB_MSR_FIRST          = 4,    // 4
  JOB_MSR_HSTRY          = 5,    // 5
  JOB_MSR_2DATA          = 8,    // 8
  JOB_MSR_4DATA          = 7,    // 7
  JOB_MSR_3DATA_NEW      = 10,   // 10
  JOB_MSR_SINGLE_DATA    = 11,   // 11
  JOB_MSR_6_DATA         = 12,   // 12
  JOB_MSR_4_TIME_DATA    = 13    // 13
} JobIdentifiers;

typedef enum _JobAlerts
{
  JOB_MSR_ALERT          = 0x80, // 128
  JOB_MSR_FIRST_NEW      = 0x40  // 64
} JobAlerts;


// Function prototypes
const char* header_to_string(uint8_t header);
const char* sensor_type_to_string(SensorTypes sensor_type);

#endif /* LIBRARIES_PHYTECH_PROTOCOL_PHYTECH_PROTOCOL_H_ */
