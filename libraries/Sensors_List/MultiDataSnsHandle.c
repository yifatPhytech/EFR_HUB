#include <stdio.h>
#include <stdbool.h>

#include "libraries/Sensors_List/MultiDataSnsHandle.h"
#include "libraries/Hub_Definition/hub_define.h"

//#include "libraries/CommonHeaders/ProtocolDef.h"

static MultySensor MultySensorArr[MAX_MULTI_SENSORS];

bool IsMultySensor(uint32_t id)
{
  for (uint8_t j = 0; j < MAX_MULTI_SENSORS; j++)
    if (MultySensorArr[j].ID == id)
      return true;
  return false;
}

uint8_t GetMultySensorIndex(uint32_t id)
{
  for (uint8_t j = 0; j < MAX_MULTI_SENSORS; j++)
    if (MultySensorArr[j].ID == id)
      return j;
  return 5;
}

void ResetMultiArray()
{
  uint8_t i, j, k;
  for (i = 0; i < MAX_MULTI_SENSORS; i++)
    {
      MultySensorArr[i].ID = 0;
      for (j = 0; j < MAX_HSTR_CNT; j++)
      {
        MultySensorArr[i].msr[j] = NO_DATA;
        for (k = 0; k < 12; k++)
          MultySensorArr[i].HstrData[j][k] = NO_DATA;
      }
    }
}

void RemoveFromMultiSns(uint32_t lID)
{
  // if its SMP - remove sensor from multi table
  if (IsMultySensor(lID))
  {
    uint8_t n = GetMultySensorIndex(lID);
    if (n < MAX_MULTI_SENSORS)
      MultySensorArr[n].ID = 0;
  }
}

uint8_t InsertNewMultiSns(uint32_t senID)
{
  uint8_t i = 0;
//    bool bFoundEmpty = false;
    do
    {
      if (MultySensorArr[i].ID == 0)
      {
        printf("insert sensor to index %d at multy data table\n", i);
        MultySensorArr[i].ID = senID;
        return i;
//        bFoundEmpty = true;
      }
      i++;
    }
    while ((i < MAX_MULTI_SENSORS) );//&& (bFoundEmpty == false));
    return MAX_MULTI_SENSORS;
}

bool SaveMultiData(uint32_t lID, PayloadMultiSenData_n_Hstr payload)
{
  uint8_t multIndex = GetMultySensorIndex(lID);
  if (multIndex >= MAX_MULTI_SENSORS)
  {
      multIndex = InsertNewMultiSns(lID);

    if (multIndex >= MAX_MULTI_SENSORS)
      return false;
  }

  printf("parse data of multi sensor at multi index %d, history index: %d", multIndex, payload.m_nHstrIndex);
  for (uint8_t i = 0; i < payload.m_nDataCnt; i++)
    {
      if (payload.m_nHstrIndex == 0)
        MultySensorArr[multIndex].msr[i] = payload.m_data[i];
      else
        MultySensorArr[multIndex].HstrData[payload.m_nHstrIndex][i] = payload.m_data[i];
      printf("measure saved: %d. \n", payload.m_data[i]);
    }
  return true;
}
