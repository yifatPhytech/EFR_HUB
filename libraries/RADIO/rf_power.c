/*
 * rf_power.c
 *
 *  Created on: Dec 5, 2023
 *      Author: itay
 */

#include "libraries/RADIO/rf_power.h"

//// Set the TX power in terms of deci-dBm
//RAIL_Status_t RAIL_SetTxPowerDbm(RAIL_Handle_t railHandle, RAIL_TxPower_t power)
//{
//  RAIL_TxPowerConfig_t txPowerConfig;
//  RAIL_GetTxPowerConfig(railHandle, &txPowerConfig);
//
//  RAIL_TxPowerLevel_t powerLevel = RAIL_ConvertDbmToRaw(railHandle, txPowerConfig.mode, power);
//  return RAIL_SetTxPower(railHandle, powerLevel);
//}
//
//// Get the TX power in terms of deci-dBm
//RAIL_TxPower_t RAIL_GetTxPowerDbm(RAIL_Handle_t railHandle)
//{
//  RAIL_TxPowerLevel_t powerLevel = RAIL_GetTxPower(railHandle);
//  RAIL_TxPowerConfig_t txPowerConfig;
//  RAIL_GetTxPowerConfig(railHandle, &txPowerConfig);
//
//  return RAIL_ConvertRawToDbm(railHandle, txPowerConfig.mode, powerLevel);
//}
