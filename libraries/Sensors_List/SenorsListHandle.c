#include <stdio.h>

#include "libraries/Sensors_List/SenorsListHandle.h"

#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/flash_storage/flash_storage.h"
#include "libraries/Sensors_List/slot_Handle.h"
#include "libraries/Sensors_List/MultiDataSnsHandle.h"



//IDStruct myData;
SensorBasic AllSns[MAX_DATA];
sensor MySensorsArr[MAX_DATA];
//uint64_t  g_lMySlots = 0;

void writeAllSnsToFlash(void)
{
    uint32_t len = sizeof(AllSns);
    writeFlash_ucharArray(SENSOR_BASIC_FLASH_ADDRESS, (unsigned char*)AllSns, len);
}

void readAllSnsFromFlash(void)
{
    uint32_t len = sizeof(AllSns);
    readFlash_ucharArray(SENSOR_BASIC_FLASH_ADDRESS, (unsigned char*)AllSns, len);
}

void ResetAllSns()
{
  for (uint8_t i = 0; i < MAX_DATA; i++)
    {
  AllSns[i].snsID = 0;
  AllSns[i].slot = 0;
    }
  writeAllSnsToFlash();
}

void writeSensorBasicRowToFlash(uint32_t index)
{
    if (index >= MAX_DATA)
    {
        // Handle invalid index
        return;
    }

    uint32_t len = sizeof(SensorBasic);  // Size of one element
    uint32_t offset = index * len;        // Calculate the offset in bytes

    // Address to write to
    uint32_t address = SENSOR_BASIC_FLASH_ADDRESS + offset;

    // Write the specific element to flash
    writeFlash_ucharArray(address, (unsigned char*)&AllSns[index], len);
}

void readSensorBasicRowFromFlash(uint32_t index)
{
    if (index >= MAX_DATA)
    {
        // Handle invalid index
        return;
    }

    uint32_t len = sizeof(SensorBasic);  // Size of one element
    uint32_t offset = index * len;        // Calculate the offset in bytes

    // Address to read from
    uint32_t address = SENSOR_BASIC_FLASH_ADDRESS + offset;

    // Read the specific element from flash
    readFlash_ucharArray(address, (unsigned char*)&AllSns[index], len);
}

void InitSnsParams()
{
  for (uint8_t i = 0; i < MAX_DATA; i++)
  {
    MySensorsArr[i].btr = 0;
    MySensorsArr[i].type = 0;
    MySensorsArr[i].rssi = 0;
    MySensorsArr[i].version = 0;
  }
}

void InitSensorArray()
{
  uint8_t i, j;//, k;
  readAllSnsFromFlash();
  g_lMySlots = 0;
  // if cant load from nvm - init all
//  if (res != ECODE_EMDRV_NVM_OK)
//  {
//    for (i = 0; i < MAX_DATA; i++)
//    {
//      AllSns[i].snsID = 0;    //init id
//      AllSns[i].slot = 0;   // init slot
//    }
//    APP_StoreData(PAGE_DATA);
//  }
  for (i = 0; i < MAX_DATA; i++)
  {
    MySensorsArr[i].ID = AllSns[i].snsID;
    MySensorsArr[i].slot.index = AllSns[i].slot;
    MySensorsArr[i].slot.status = SLOT_STATUS_BUSY;
    MySensorsArr[i].Status = SEN_STATUS_CELL_EMPTY;
    MySensorsArr[i].DailyCnct = 0;
    MySensorsArr[i].version = 0;
    MySensorsArr[i].slotDelta2updt = false;
    MySensorsArr[i].msr = NO_DATA;
    for (j = 0; j < MAX_HSTR_CNT; j++)
      MySensorsArr[i].HstrData[j] = NO_DATA;
    printf("sensor %d at index %d slot %d\n" ,MySensorsArr[i].ID, i, MySensorsArr[i].slot.index);
    if (AllSns[i].slot != 0)
    {
      uint64_t n = (uint64_t)1 << AllSns[i].slot;
      g_lMySlots |= n;
    }
  }
  ResetMultiArray();

  printf("g_lMySlots = %l - %l - %l\n", (uint16_t)(g_lMySlots >> 32),(uint16_t)(g_lMySlots >> 16), (uint16_t)g_lMySlots);

}

uint8_t GetSensorIndex(uint32_t senID)
{
  uint8_t i = 0;
  do
  {
    if (MySensorsArr[i].ID == senID)
      return i;
    i++;
  }
  while(i < MAX_DATA);
  return MAX_DATA;
}

void MoveData2Hstr()
{
  printf("Move Data 2 Hstr\n");
//logd("g_lMySlots = %l - %l - %l", (uint16_t)(g_lMySlots >> 32),(uint16_t)(g_lMySlots >> 16), (uint16_t)g_lMySlots);
  uint8_t senIndex = 0, i;
  do
  {
    senIndex = GetNextSensor(senIndex);
    if (senIndex != MAX_DATA)
    // if prev data hasnt sent to logger
    {
      // move all history data 1 step ahead
      for (i = (MAX_HSTR_CNT - 1); i > 0; i--)
      {
        MySensorsArr[senIndex].HstrData[i] = MySensorsArr[senIndex].HstrData[(i-1)];
      }
      // copy current data to history data
      MySensorsArr[senIndex].HstrData[0] = MySensorsArr[senIndex].msr;
      //set default data
      MySensorsArr[senIndex].msr = NO_DATA;
      MySensorsArr[senIndex].Status = SEN_STATUS_CELL_EMPTY;
      MySensorsArr[senIndex].IsNew++;
  //#ifdef DEBUG_MODE
      printf("%d: hours without connect: %d", MySensorsArr[senIndex].ID, MySensorsArr[senIndex].IsNew);
//  #endif
    }
    senIndex++;
}
while (senIndex < MAX_DATA);
}

void RemoveSensor(uint8_t i)
{
  uint64_t tmp;
  printf("\r\nremove sensor %d from index %d", MySensorsArr[i].ID, i);
  MySensorsArr[i].ID = 0;//DEFAULT_ID;
  MySensorsArr[i].slot.status = SLOT_STATUS_EMPTY;
  MySensorsArr[i].DailyCnct = 0;
  MySensorsArr[i].slotDelta2updt = false;
  // if its SMP - remove sensor from multi table
  RemoveFromMultiSns(MySensorsArr[i].ID);

  tmp = ~((uint64_t)1 << MySensorsArr[i].slot.index);
  g_lMySlots &= tmp;
  AllSns[i].snsID = 0;
  AllSns[i].slot = 0;
  writeAllSnsToFlash();
}

int8_t InsertNewSensor(uint32_t senID,uint8_t senType, bool bMulti)
{
  uint8_t nSlot, j, i = 0, startIndex, lastIndex;
  bool bFoundEmpty = false;
  printf("insert new sensor\n");

//  if ((IsMulySnsrProb(senType) == true) || (bMulti))
//  {
//    startIndex = 0;
//    lastIndex = 5;
//  }
//  else
  {
    startIndex = 0;
    lastIndex = MAX_DATA;
  }
  i = startIndex;
  do
  {
    if (MySensorsArr[i].ID != 0)
      if (MySensorsArr[i].ID == senID)
        return i;
      else
        i++;
    else
      bFoundEmpty = true;
  }
  while ((i < lastIndex) && (!bFoundEmpty));

  if (bFoundEmpty)
  {
    ///////////////////////////////////
    nSlot = GetHashFreeSlot(senID); //GetNextFreeSlot(senType);
    // if no space - exit
    if (nSlot >= MAX_SENSOR_SLOT)
      return -1;  //MAX_SENSOR_SLOT;

    MySensorsArr[i].slot.index = nSlot;
    MySensorsArr[i].slot.status = SLOT_STATUS_BUSY;
    MySensorsArr[i].type = senType;
    MySensorsArr[i].DailyCnct = 0;
    AllSns[i].slot = nSlot;
    /////////////////////////////////////
    MySensorsArr[i].ID = senID;
    printf("\r\ninsert sensor: %d to index: %d", senID, i);
    MySensorsArr[i].Status = SEN_STATUS_CELL_EMPTY;
    for (j = 0; j < MAX_HSTR_CNT; j++)
      MySensorsArr[i].HstrData[j] = NO_DATA;
    MySensorsArr[i].IsNew = 1;  //DataStatus |= EXTRA_STATUS_NEW;//
    MySensorsArr[i].version = 0;
    MySensorsArr[i].slotDelta2updt = false;
    if (IsMulySnsrProb(senType))
    {
        InsertNewMultiSns(senID);
    }
    AllSns[i].snsID = senID;
    writeAllSnsToFlash();
    return i;
  }
  return -1;
}

void ResetAllSensorsData()
{

  uint8_t senIndex = 0;
  do
  {
    senIndex = GetNextSensor(senIndex);
    if (senIndex < MAX_DATA)
    {
      if (MySensorsArr[senIndex].Status == SEN_STATUS_SEND_DATA)
      {
        MySensorsArr[senIndex].Status = SEN_STATUS_CELL_EMPTY;
        MySensorsArr[senIndex].msr = NO_DATA;
        for (uint8_t j = 0; j < MAX_HSTR_CNT; j++)
          MySensorsArr[senIndex].HstrData[j] = NO_DATA;
        printf("\r\ndata of index %d sent OK", senIndex);
      }
      senIndex++;
    }
  }
  while (senIndex < MAX_DATA);
}
