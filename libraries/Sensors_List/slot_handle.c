/*
 * hub_protocols.c
 *
 *  Created on: Sep 10, 2024
 *      Author: Yifat
 */

#include <stdio.h>                          // for printf function
#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/Sensors_List/SenorsListHandle.h"
#include "libraries/Sensors_List/slot_Handle.h"
#include "libraries/system_mode/system_mode.h"

uint64_t  g_lMySlots = 0;

uint8_t RestoreSlot(uint8_t index)
{
  printf("\r\nRestoreSlot");
//  Ecode_t res =  APP_ReadData(PAGE_DATA, OBJ_DATA);
//  // if cant load from nvm - init all
//  if (res != ECODE_EMDRV_NVM_OK)
//  {
//    return MAX_SENSOR_SLOT;//MAX_DATA;
//  }
//  else
  readAllSnsFromFlash();
    if (AllSns[index].snsID == MySensorsArr[index].ID)
    {
      MySensorsArr[index].slot.index = AllSns[index].slot;
      return MySensorsArr[index].slot.index;
    }
  return MAX_SENSOR_SLOT; //  MAX_DATA;
}

uint8_t GetHashFreeSlot(uint32_t id)
{
  uint8_t n, hash, cnt = 0;
  uint64_t tmpSlot;

  hash = (id + sensorDetails.sensorID) % MAX_SENSOR_SLOT;
  do
  {
    n = hash % 6;//15;
    // make sure its not burst slots
    if ((n != 0) && (n != 1))// && (n != 13) && (n != 14))
    {
      //if its free slot
      tmpSlot = (uint64_t)((uint64_t)1 << hash);
      if ((g_lMySlots & tmpSlot) == 0)
      {
        g_lMySlots = g_lMySlots | tmpSlot;
        printf("\r\nid: %d hash: %d ", id, hash);
        return hash;
      }
    }
    hash++;
    if (hash >= MAX_SENSOR_SLOT)
      hash = 0;
    cnt++;
  }
  while  (cnt < MAX_SENSOR_SLOT);

  return MAX_SENSOR_SLOT;
}

uint8_t SwapSlot(uint8_t index)
{
  uint8_t newSlot;
  printf("\r\nswap slot");
  // find new slot
  newSlot = GetHashFreeSlot(MySensorsArr[index].ID);//GetNextFreeSlot(MySensorsArr[index].type);
  if (newSlot >= MAX_SENSOR_SLOT) //MAX_DATA)
    return MAX_SENSOR_SLOT; //MAX_DATA;
  // delete old one
//  logd("g_lMySlots before swap: %d", g_lMySlots);
  uint64_t t = ~((uint64_t)1 << MySensorsArr[index].slot.index);
  g_lMySlots &= t;
  t = (uint64_t)1 << newSlot;
  g_lMySlots |= t;

  MySensorsArr[index].slot.index = newSlot;
  MySensorsArr[index].slot.status = SLOT_STATUS_BUSY;
  AllSns[index].slot = newSlot;
  writeAllSnsToFlash();
//  APP_StoreData(PAGE_DATA);
  return newSlot;
}
