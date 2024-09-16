/*
 * hub_protocols.c
 *
 *  Created on: Sep 10, 2024
 *      Author: Yifat
 */
#include <stdio.h>                          // for printf function

#include <libraries/106_ADC/106_adc_reader.h>
#include "libraries/CommonHeaders/ProtocolDef.h"
#include "libraries/CommonHeaders/GlobalDefine.h"
#include "libraries/Sensors_List/SenorsListHandle.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/Hub_Definition/hub_protocols.h"
#include "libraries/phytech_protocol/phytech_protocol.h"


Sen_Hub_Rec_Msg msgOut;
uint16_t  g_iBtr;

uint8_t GetNumSensors()
{
  uint8_t i, n = 0;
  for (i = 0; i < MAX_DATA; i++)
    if (AllSns[i].snsID != 0)
      n++;

  return n;
}

bool IsMultySensor(uint32_t id)
{
//  for (uint8_t j = 0; j < 5; j++)
//    if (MultySensorArr[j].ID == id)
//      return true;
  return false;
}

bool IsHistory(uint8_t ind)
{
  for (uint8_t i = 0; i < MAX_HSTR_CNT; i++)
  {
    if (MySensorsArr[ind].HstrData[i] != NO_DATA)
      return true;
  }
  return false;
}

uint8_t GetSensorDataLen(uint8_t senIndex)
{
  uint8_t i;

  if (MySensorsArr[senIndex].ID == 0)
    return 0;
//  printf("GetSensorDataLen: id: %d  status: %d", MySensorsArr[senIndex].ID, MySensorsArr[senIndex].Status);
  // if no new data & no old data
  if ((MySensorsArr[senIndex].Status == SEN_STATUS_CELL_EMPTY) && (IsHistory(senIndex) == false)) //(MySensorsArr[senIndex].IsHstr == false))
    return 0;

//  if ((IsMultySensor(MySensorsArr[senIndex].ID)))// && (g_bAlert2Send == false) )
//  {
//    uint8_t n = GetMultySensorIndex(MySensorsArr[senIndex].ID); //MySensorsArr[senIndex].msr;
//    printf("get length of multy sensor %d at multy index %d",MySensorsArr[senIndex].ID, n );
//    if (n >= 5)
//      return 0;
//
//    return LENGTH_MULTY_SNS_DATA_MSG;
//  }

//  if ((g_bAlert2Send == true) && (MySensorsArr[senIndex].Status != SEN_STATUS_GOT_ALERT_DATA))
//    return 0;
  printf("index %d: check data of sensor %d. ", senIndex, MySensorsArr[senIndex].ID);
    for (i = MAX_HSTR_CNT; i > 0; i--)
    {
      if (MySensorsArr[senIndex].HstrData[i-1] != NO_DATA)
      {
        break;
      }
    }
  return (LENGTH_SNS_DATA_MSG + (i * 2));
}

uint8_t GetSensorData_n_History(uint8_t senIndex, uint8_t* tmp, uint8_t bufIndex)
{
  uint8_t i;//, res = 0;

  if (MySensorsArr[senIndex].ID == 0)
    return 0;
  // if no new data & no old data
  if ((MySensorsArr[senIndex].Status == SEN_STATUS_CELL_EMPTY) && (IsHistory(senIndex) == false)) //(MySensorsArr[senIndex].IsHstr == false))
    return 0;

//  if ((IsMultySensor(MySensorsArr[senIndex].ID)) )//&& (g_bAlert2Send == false) )
//  {
//    uint8_t n = GetMultySensorIndex(MySensorsArr[senIndex].ID); //MySensorsArr[senIndex].msr;
//    printf("get data of multy sensor %d at multi index %d",MySensorsArr[senIndex].ID, n );
//    if (n >= 5)
//      return 0;
//    tmp[0] = LENGTH_MULTY_SNS_DATA_MSG;
//    tmp[1] = TYPE_MLT_DATA;
//
//    //copy ID
////    memcpy(,Long2Bytes() , 4);
//    uint32_to_little_endian(&tmp[2], MySensorsArr[senIndex].ID);
//    return tmp[0];
//  }

  tmp[0] = LENGTH_SNS_DATA_MSG;
  tmp[1] = TYPE_DATA_N_HST;
  //COPY ID and last measure
  for (i = 0; i < 6; i++)
    tmp[i+2] = (((uint8_t*)&(MySensorsArr[senIndex]))[i]);

#ifdef DEBUG_MODE
  printf("index %d: send data of sensor %d. ", senIndex, MySensorsArr[senIndex].ID);
#endif
    //add history at the end of msg
    for (i = MAX_HSTR_CNT; i > 0; i--)
    {
      if (MySensorsArr[senIndex].HstrData[i-1] != NO_DATA)
      {
#ifdef DEBUG_MODE
        printf("found history (%d) at index %d", MySensorsArr[senIndex].HstrData[i-1] ,i);
#endif
        break;
      }
    }
    //copy history
    memcpy(&tmp[LENGTH_SNS_DATA_MSG],  (uint8_t*)&(MySensorsArr[senIndex].HstrData[0]), i*2);
    tmp[0] += i*2;    // set new size
  return tmp[0];
}

uint8_t GetNextSensor(uint8_t i)
{
  if (i >= MAX_DATA)
    return MAX_DATA;
  do
  {
    if (MySensorsArr[i].ID != 0)
      return i;
    else
      i++;
  }
  while (i < MAX_DATA);

  return MAX_DATA;
}

uint8_t BuildDataMsg()
{
  uint8_t bufIndex = 0, senIndex = 0 , i =0;
  g_bIsMoreData = false;
#ifdef DEBUG_MODE
  printf("BuildDataMsg. g_LoggerID= %d current slot = %d", g_LoggerID, g_nCurTimeSlot);
#endif

  msgOut.Header.m_Header = HEADER_SND_DATA;
//  if (g_bAlert2Send == true)
//    msgOut.Header.m_Header = HEADER_SND_DATA_ALERT;
  msgOut.Header.m_addressee = g_LoggerID;
  msgOut.Header.m_ID = myData.m_ID;
  //set data
  do
  {
    senIndex = GetNextSensor(senIndex); //GetNextSensor2Send(senIndex); //
    if (senIndex < MAX_DATA)
    {
      i = GetSensorDataLen(senIndex);
      if (i > 0)
      {
        if ((i + bufIndex) < MAX_PAYLOAD_LEN_NEW)
        {
          i = GetSensorData_n_History(senIndex, &msgOut.HubDataPayload.m_data[bufIndex], bufIndex);
          bufIndex += i;
          MySensorsArr[senIndex].Status = SEN_STATUS_SEND_DATA;
  #ifdef DEBUG_MODE
          printf("add %d bytes to buffer. total len is: %d", i, bufIndex);
  #endif
          senIndex++;
        }
        else
          g_bIsMoreData = true;
      }
      else
        senIndex++;
    }
  }//end of while
  while ((senIndex < MAX_DATA) && (g_bIsMoreData == false));

  senIndex = GetNextSensor(senIndex);

  // if no data at all - write 0 as payload size. but if no data after already one packet sent - do not send nothing
  if ((g_bOnReset) || (bufIndex == 0))//(i == 0))
  {
    msgOut.HubDataPayload.m_data[0] = 0;
    bufIndex = 1;
#ifdef DEBUG_MODE
    printf("empty");
#endif
  }

  // if there is more data to send but not enough space - mark flag
  if ((i > 0) && (senIndex < MAX_DATA))
    g_bIsMoreData = true;
  // write size
  msgOut.Header.m_size = sizeof(msgOut.Header) + bufIndex + 1; // + CS
  return bufIndex;
}

uint8_t BuildSnsPrmMsg()
{
  uint8_t bufIndex = 0,/*INDEX_DATA,*/ senIndex = g_nIndex2SendPrm;
  g_bIsMoreData = false;
#ifdef DEBUG_MODE
  printf("BuildSnsPrmMsg. from index %d", senIndex);
#endif

  msgOut.Header.m_Header = HEADER_HUB_SNS_PRM;
  msgOut.Header.m_addressee = g_LoggerID;
  msgOut.Header.m_ID = myData.m_ID;

  //set parameters
  do
  {
    senIndex = GetNextSensor(senIndex);
    if (senIndex < MAX_DATA)
    {
      // send params only if connected once in last 24 hours
      if ((MySensorsArr[senIndex].DailyCnct == 0) && (MySensorsArr[senIndex].type != 0))
//      if (MySensorsArr[senIndex].Status != SEN_STATUS_CELL_EMPTY)
      {
        printf("get prms of sns = %d, btr = %d",MySensorsArr[senIndex].ID,MySensorsArr[senIndex].btr);
//        Copy(&msgOut.HubDataPayload.m_data[bufIndex], Long2Bytes(MySensorsArr[senIndex].ID),4);
        uint32_to_little_endian(&msgOut.HubDataPayload.m_data[bufIndex], MySensorsArr[senIndex].ID);
//        Copy(, Int2Bytes(),2);
        uint16_to_little_endian(&msgOut.HubDataPayload.m_data[bufIndex+4], MySensorsArr[senIndex].btr);
        msgOut.HubDataPayload.m_data[bufIndex+6] = MySensorsArr[senIndex].type;
        msgOut.HubDataPayload.m_data[bufIndex+7] = MySensorsArr[senIndex].rssi;
//        Copy(, Long2Bytes(),4);
        uint32_to_little_endian(&msgOut.HubDataPayload.m_data[bufIndex+8], MySensorsArr[senIndex].version);
        MySensorsArr[senIndex].Status = SEN_STATUS_SEND_PRM;
        bufIndex += LENGTH_SNS_PRM_MSG;//sizeof(sp);
      }
      else
        printf("no type or not connected");
      senIndex++;
      // if can not add anothe sensor parameters cos it ll be too long-
      if ((bufIndex + LENGTH_SNS_PRM_MSG/*sizeof(sp)*/) >= MAX_PAYLOAD_LEN_NEW)
      {
        // check if there is next sensor
        senIndex = GetNextSensor(senIndex);
        // if so - sign that there is more data in order to make another sending round
        if (senIndex < MAX_DATA)
          g_bIsMoreData = true;
      }
    }
  } //end of while
  while ((senIndex < MAX_DATA) && (g_bIsMoreData == false));

  g_nIndex2SendPrm = senIndex; //GetNextSensor(senIndex);

  // if no data at all - write 0 as payload size. but if no data after already one packet sent - do not send nothing
  if ((bufIndex == 0))//(i == 0))
  {
//    msgOut.HubDataPayload.m_data[0] = 0;
//    bufIndex = 1;
#ifdef DEBUG_MODE
    printf("empty. nothing to send");
#endif
  }

  // write size
  msgOut.Header.m_size = sizeof(msgOut.Header) + bufIndex + 1; // + CS
  return bufIndex;
}

//build msg for monitor
uint8_t BuildConfigMsg()
{
/*  uint8_t n = 0;
  Uint16toBytes i2b;
  Uint32toBytes ui2b;
  Uint64toBytes l2b;
  printf("BuildConfigMsg");

  if (myData.m_ID == 0)
  {
    mntr.m_Header = HEADER_GETID;//0xA6;
    i2b.iVal = g_iBtr;
    Copy(&mntr.m_buffer[n], i2b.bVal, 2);
    n += 2;
    Copy(&mntr.m_buffer[n], (uint8_t*)&Version, 4);
    n += 4;
    l2b.iVal = SYSTEM_GetUnique();
    Copy(&mntr.m_buffer[n], l2b.bVal, 8);
    n += 8;
    mntr.m_size = n + 2;  //sizeof(mntrHdr) + sizeof(mntrMsg.stage1Payload) + 1;
  }
  else
  {
    mntr.m_Header = HEADER_ID_OK;
    ui2b.iVal = myData.m_ID;
    Copy(&mntr.m_buffer[n], ui2b.bVal, 4);
    n += 4;
    mntr.m_size = n+2;//sizeof(mntrMsg.stage3Payload);
  }*/
  return 1;
}


uint8_t BuildPrmMsg()
{
  union _FloatToBytes
  {
    float fVal;
      uint8_t bVal[4];
  } fl2byte;
  char ver[4];

  g_iBtr = readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);
  printf("battery = %d", g_iBtr);
  g_bIsMoreData = false;
  g_bSendParams = false;

  msgOut.Header.m_Header = HEADER_HUB_PRM;
  msgOut.Header.m_addressee = g_LoggerID;
  msgOut.Header.m_ID = myData.m_ID;
  msgOut.Header.m_size = sizeof(msgOut.Header) + sizeof(msgOut.HubPrmsExtPayload) + 1; // +1
#ifdef TEST_BTR_MODE
  msgOut.Header.m_size = 63;
#endif
  msgOut.HubPrmsExtPayload.m_lat = GetNumSensors();//g_fLat;
//  fl2byte.bVal[0] = g_nMaxRetryCnt;
//  fl2byte.bVal[1] = g_curLgrRfPwr;
  msgOut.HubPrmsExtPayload.m_lon = fl2byte.fVal;  //g_fLon;
//  g_fLon = g_nMaxRetryCnt;
  getFirmwareVersion(ver);

  msgOut.HubPrmsExtPayload.m_version = little_endian_to_uint32(ver);// Bytes2Long((uint8_t*)ver);
  msgOut.HubPrmsExtPayload.m_battery = g_iBtr;

  printf("BuildPrmMsg. header %d", msgOut.Header.m_Header);
  return 1;
}


uint8_t BuildTx()
{
  uint8_t res = 0;
//  if (g_wCurMode == MODE_SENDING)
    switch (GetCurrentMsgType())
    {
    case MSG_DATA:
      res = BuildDataMsg();
      break;
    case MSG_SNS_PRM:
      res = BuildSnsPrmMsg();
      break;
    case MSG_HUB_PRM:
      res = BuildPrmMsg();  //BuildGpsMsg();//
      break;
    case MSG_CONFIG:
      g_iBtr = (uint16_t)readADCChannel(ADC0_INTERNAL_BATT_PC2_CHANNEL_2);  //EZR32_GetBattVoltage_mV();
      printf("Btr = %d", g_iBtr);
      res = BuildConfigMsg();
      break;
    case MSG_FW_UPDATE:
     // BuildCheckUpdate();//res = BuildPrmMsg(true);
      res = 1;
      break;
    default:
      break;
  }
  // if nothing to send - (can happened in sensors data or sensors message only)-
  // send hub parameters instead
//  if (res == 0)
//  {
//    g_msgType = MSG_HUB_PRM;
//    res = BuildPrmMsg();
//  }

  return res;
}
