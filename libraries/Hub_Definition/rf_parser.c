/*
 * hub_protocols.c
 *
 *  Created on: Sep 10, 2024
 *      Author: Yifat
 */

#include "sl_sleeptimer.h"

#include "libraries/CommonHeaders/ProtocolDef.h"
#include "libraries/CommonHeaders/ProtocolMonitorDef.h"
#include "libraries/CommonHeaders/GlobalDefine.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/Hub_Definition/rf_rx_handle.h"
#include "libraries/Hub_Definition/rf_parser.h"
#include "libraries/flash_storage/flash_storage.h"
#include "libraries/Hub_Definition/hub_protocols.h"
#include "libraries/Sensors_List/SenorsListHandle.h"
#include "libraries/Sensors_List/slot_Handle.h"
#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/Hub_Definition/logger_sm.h"
#include "libraries/tools/tools.h"

#define MIN_RSSI_4_ANSWER 60
#define MIN_RSSI_4_DELAY  100
#define DELAY_ACK_MS_LENGTH 150


Sen_Hub_Rec_Msg msgIn;
ProtocolMntrHeader mntr;
static uint8_t    g_bRadioOK = 0;


uint16_t GetSecToConnect(uint16_t nSlot)
{
  uint16_t nSlots2Wait;

  if (g_nCurTimeSlot < nSlot)
    nSlots2Wait = nSlot - (g_nCurTimeSlot+1);
  else
    nSlots2Wait = MAX_SLOT - (g_nCurTimeSlot+1) + nSlot;

  return (SLOT_INTERVAL_SEC * nSlots2Wait) ;//+ ((slotTickCnt / APP_RTC_FREQ_HZ)) ;//todo add + ((slotTickCnt / APP_RTC_FREQ_HZ)
}

int16_t CalcDrift()
{
  uint8_t m,s, max_m, max_s, min_m, min_s;
  bool bNeg = false;
  m = g_nCurTimeSlot / 6;
  s = 10 * (g_nCurTimeSlot % 6 ) + ((100 - g_time2EndHubSlot) / 10);  // calc seconds from hub slot started
  printf("hub clock: %d:%d\n", m,s);
  if (g_nMin == m)
    return (s - g_nSec);
  else
  {
    if (g_nMin > m)
    {
      max_m = g_nMin;
      max_s = g_nSec;
      min_m = m;
      min_s = s;
      bNeg = true;
    }
    else
    {
      max_m = m;
      max_s = s;
      min_m = g_nMin;
      min_s = g_nSec;
    }
    int16_t delta = max_m - (min_m + 1) * 60; // calc seconds between minutes
    delta += (max_s + (60 - min_s));
    delta *= bNeg;
    return delta;
  }
}


bool ParseSensorMsg()
{
//  gReadStack = GetFirstBusyCell();

  uint8_t bNewSensor = false;
  uint8_t i,  cs, hstrIndex, multIndex;
  uint8_t size = NewMsgStack[gReadStack].Buffer[FIRST_FIELD];
  uint8_t nSlot = MAX_SENSOR_SLOT ,res = false;
  int8_t  senIndex;


  cs = GetCheckSum(&NewMsgStack[gReadStack].Buffer[1],size-1);
  if (NewMsgStack[gReadStack].Buffer[size] != cs)
  {
//#ifdef DEBUG_MODE
    printf("wrong CS. size = %d, cs = %d, NewMsgStack[gReadStack][size] = %d\n", size, cs, NewMsgStack[gReadStack].Buffer[size]);
//#endif
    NewMsgStack[gReadStack].Status = CELL_EMPTY;
    return false;
  }
  g_bRadioOK = 0;
  //copy to struct
  for ( i = 0; i < size; i++)
  {
    ((( uint8_t *) &msgIn)[i]) = NewMsgStack[gReadStack].Buffer[i+1];
  }

  NewMsgStack[gReadStack].Status = CELL_EMPTY;
  printf("sensor ID: %lu to: %lu header: %d size: %d\n", msgIn.Header.m_ID, msgIn.Header.m_addressee, msgIn.Header.m_Header, msgIn.Header.m_size);

  if ((GetCurrentMode() == MODE_INSTALLATION)  && (msgIn.Header.m_Header == HEADER_HUB_STOP) && (size == (sizeof(msgIn.Header) + 1)))
  {
    printf("got stop signal\n");
    g_instlCycleCnt = 1;
    g_hours2NextInstl = 0;
    msgOut = msgIn;
    msgOut.Header.m_Header = HEADER_HUB_STOP_ACK;
    return true;
  }
  // if its data or history msg from sensor
  if ((msgIn.Header.m_Header == HEADER_MSR_ONLY) || (msgIn.Header.m_Header == HEADER_SEN_LOST) ||
      (msgIn.Header.m_Header == HEADER_SEN_PRM) || (msgIn.Header.m_Header == HEADER_MSR_MLT_ONLY))
      /*(msgIn.Header.m_Header == HEADER_MSR_URGENT) || (msgIn.Header.m_Header == HEADER_MSR_ONLY_INT))*/
  {
    if (msgIn.Header.m_addressee != getSensorID())// myData.m_ID)
      if (msgIn.Header.m_addressee != DEFAULT_ID)
      {
//#ifdef DEBUG_MODE
        printf("message not for me\n");
//#endif
        // if hub registered sensor before but it doesn't transmit to it - remove it from list
        senIndex = GetSensorIndex(msgIn.Header.m_ID);
        if (senIndex < MAX_DATA)
          RemoveSensor(senIndex);
        // msg not for me
        return false;
      }

    printf("got %d message\n", msgIn.Header.m_Header);
    senIndex = GetSensorIndex(msgIn.Header.m_ID);
    printf("sensor ID: %lu at index %d slot: %d RSSI: %d\n", msgIn.Header.m_ID, senIndex, MySensorsArr[senIndex].slot.index, NewMsgStack[gReadStack].Rssi);
    // not my sensor
    if (senIndex >= MAX_DATA)
    {
      printf("not my sensor\n");

        // only for installation (HUB & SENSORS)- delay ack according to RSSI
        if (GetCurrentMode() == MODE_INSTALLATION)
        {
          if (msgIn.Header.m_Header == HEADER_SEN_PRM)  //todo - return
          {
            if (NewMsgStack[gReadStack].Rssi <= MIN_RSSI_4_ANSWER)
              return false;
            if (NewMsgStack[gReadStack].Rssi <= MIN_RSSI_4_DELAY)
            {
              uint8_t nDelay = (uint8_t)((260 - NewMsgStack[gReadStack].Rssi) / 10) - 14;
              if (nDelay <= 7)
                sl_sleeptimer_delay_millisecond(nDelay * DELAY_ACK_MS_LENGTH);//RTCDRV_Delay(nDelay * DELAY_ACK_MS_LENGTH);
            }
          }
        }
        else
        {
          // if sensor in broadcast and rssi is lower than 60- do not answer
          if (msgIn.Header.m_addressee == DEFAULT_ID)
            if (NewMsgStack[gReadStack].Rssi <= MIN_RSSI_4_ANSWER)
              {
                printf("low rssi\n");
              return false;
              }
        }
//        bool bIsMulty = (msgIn.Header.m_Header == HEADER_MSR_MLT_ONLY)? true: false;
        if (msgIn.Header.m_Header == HEADER_SEN_PRM)
          senIndex = InsertNewSensor(msgIn.Header.m_ID, msgIn.PrmPayload.m_type, false);
        else
          senIndex = InsertNewSensor(msgIn.Header.m_ID, 0, false);
        printf("senIndex = %d, slot status %d" , senIndex, MySensorsArr[senIndex].slot.status);

        if (senIndex == -1) //MAX_DATA)
        {
          printf("not enough space for more sensor\n");
        // cant insert this sensor
          return false;
        }
        // sign that sensor is new for next
        bNewSensor = true;
    }
    nSlot = MySensorsArr[senIndex].slot.index;
    if (nSlot >= MAX_SENSOR_SLOT) //MAX_DATA)
    {
      nSlot = RestoreSlot(senIndex);
      if (nSlot >= MAX_SENSOR_SLOT) //MAX_DATA)
      {
        printf("unreasonable slot. \n");
        nSlot = SwapSlot(senIndex);
        if (nSlot >= MAX_SENSOR_SLOT) //MAX_DATA)
          return false;
      }
    }

    switch (msgIn.Header.m_Header)
    {
    case HEADER_MSR_ONLY:
      printf("Standard measure msg");
      MySensorsArr[senIndex].msr = msgIn.DataHstPayload.m_data[0];
      MySensorsArr[senIndex].Status = SEN_STATUS_GOT_DATA;
      MySensorsArr[senIndex].IsNew = 0;
//      printf("measure1 saved: %d. ", MySensorsArr[senIndex].msr);
      if (msgIn.DataHstPayload.m_nDataCnt > 1)
      {
        // make sure history is no more than MAX_HSTR_CNT
        if ((msgIn.DataHstPayload.m_nDataCnt -1) > MAX_HSTR_CNT)
          msgIn.DataHstPayload.m_nDataCnt = MAX_HSTR_CNT+1;
        for (i = 1; i < msgIn.DataHstPayload.m_nDataCnt; i++)
        {
          MySensorsArr[senIndex].HstrData[i-1] = msgIn.DataHstPayload.m_data[i];
//          printf("old measure saved: %d. ", MySensorsArr[senIndex].HstrData[i-1]);
        }
        // if sensor send more than 2 data - means it hasnt heard ACK last time-
        if (msgIn.DataHstPayload.m_nDataCnt > 2)
          MySensorsArr[senIndex].IsNew = 1; // mark as if it hasnt connectet this hour, so next hour will listen again
      }
      // if its installation mode but the sensor was known already or if sensor is out of its slot
      if (/*(msgIn.Header.m_Header != HEADER_MSR_URGENT) && */(bNewSensor == false))
        if  (((GetCurrentMode() == MODE_INSTALLATION) && (msgIn.Header.m_addressee == DEFAULT_ID)) ||
           ((GetCurrentMode() != MODE_INSTALLATION) && ((MySensorsArr[senIndex].slot.index != g_nCurTimeSlot) && (g_nDeltaOfSlots == 0))))
          nSlot = SwapSlot(senIndex);
/*#ifndef VISHAY_SENSORS_ONLY
      if (msgIn.Header.m_Header == HEADER_MSR_URGENT)
      {
        if (g_wCurMode != MODE_INSTALLATION)
          g_bAlert2Send = true;
        MySensorsArr[senIndex].Status = SEN_STATUS_GOT_ALERT_DATA;
      }
#endif*/
    break;
    case HEADER_SEN_PRM:
      MySensorsArr[senIndex].version = msgIn.PrmPayload.m_version;
      MySensorsArr[senIndex].btr = msgIn.PrmPayload.m_battery;
      MySensorsArr[senIndex].type = msgIn.PrmPayload.m_type;
      MySensorsArr[senIndex].rssi = NewMsgStack[gReadStack].Rssi;
      printf("btr: %d, type: %d, ver: %lu, RSSI: %d\n", MySensorsArr[senIndex].btr,MySensorsArr[senIndex].type,MySensorsArr[senIndex].version,MySensorsArr[senIndex].rssi);
      break;

    case HEADER_SEN_LOST:
      break;
    case HEADER_MSR_MLT_ONLY:
/*      hstrIndex = msgIn.MultiDataPayload.m_nHstrIndex;
      multIndex = GetMultySensorIndex(msgIn.Header.m_ID);
      if (multIndex >= 5)
      {
        uint8_t j = 0;
        do
        {
          if (MultySensorArr[j].ID == 0)
          {
//            printf("insert data to index %d at multy data table", j);
            MultySensorArr[j].ID = msgIn.Header.m_ID;
            multIndex = j;
          }
          j++;
        }
        while ((j < 5) && (multIndex >= 5));

        if (multIndex >= 5)
          return false;
      }

      printf("parse data of multy sensor at multy index %d, history index: %d", multIndex, hstrIndex);

      for (i = 0; i < msgIn.MultiDataPayload.m_nDataCnt; i++)
      {
        if (hstrIndex == 0)
          MultySensorArr[multIndex].msr[i] = msgIn.MultiDataPayload.m_data[i];
        else
          MultySensorArr[multIndex].HstrData[hstrIndex][i] = msgIn.MultiDataPayload.m_data[i];
        printf("measure saved: %d. ", msgIn.MultiDataPayload.m_data[i]);
      }
      MySensorsArr[senIndex].Status = SEN_STATUS_GOT_DATA;
      MySensorsArr[senIndex].IsNew = 0;
      // if its installation mode but the sensor was known already or if sensor is out of its slot
      if  (((GetCurrentMode() == MODE_INSTALLATION) && (bNewSensor == false) && (msgIn.Header.m_addressee == DEFAULT_ID))
          || ((MySensorsArr[senIndex].slot.index != g_nCurTimeSlot) && (g_nDeltaOfSlots == 0) && (bNewSensor == false)))
        nSlot = SwapSlot(senIndex);*/
      break;
    } // switch

    MySensorsArr[senIndex].DailyCnct = 0;
    //Build response:
    //if need to get prms from sensor - send special ACK

    if ((g_bSendParams) && (GetCurrentMode() != MODE_INSTALLATION)
        && ((msgIn.Header.m_Header == HEADER_MSR_ONLY) || (msgIn.Header.m_Header == HEADER_MSR_MLT_ONLY)))
    {
      msgOut.Header.m_Header = HEADER_MSR_ACK_GET_PRM;
    }
    else
    {
      msgOut.Header.m_Header = msgIn.Header.m_Header + 1;
      if (msgIn.Header.m_Header == HEADER_MSR_MLT_ONLY)
        msgOut.Header.m_Header = HEADER_MSR_ONLY_ACK;
      if ((msgIn.Header.m_ID == g_CurSensorLsn) && (g_CurSensorLsn != 0))
        g_bMissionCompleted = true;
    }
    msgOut.Header.m_size = sizeof(msgOut.Header) + sizeof(msgOut.AckExtPayload) + 1;
    msgOut.Header.m_addressee = msgIn.Header.m_ID;
    msgOut.Header.m_ID = getSensorID();  //myData.m_ID;
    msgOut.AckExtPayload.m_indexEcho = 0;//msgIn.DataPayload.m_index;
    //reset timer every hour
    msgOut.AckExtPayload.m_slot = nSlot;
    msgOut.AckExtPayload.m_sec2cnct = GetSecToConnect(nSlot)/*+1*/; // add 1 sec in order to be on the safe side
    msgOut.AckExtPayload.m_sec4Burst = 0; //GetSecToBurst();
    printf("Second to connect: %d\n", msgOut.AckExtPayload.m_sec2cnct);
    msgOut.AckExtPayload.m_HeardRssi = NewMsgStack[gReadStack].Rssi;
    res = true;
  }
  else
  {
#ifdef DEBUG_MODE
    printf("not sensor msg\n");
    return false;
#endif
  }
  return res;
}


bool ParseLoggerMsg()
{
  uint8_t i,  cs;
  uint8_t senIndex, size = NewMsgStack[gReadStack].Buffer[FIRST_FIELD];
  uint8_t res = false;
  SendMsgType type;

  printf("ParseLoggerMsg. size= %d, gReadStack = %d\n", size, gReadStack);
  cs = GetCheckSum(&NewMsgStack[gReadStack].Buffer[1],size-1);
  if (NewMsgStack[gReadStack].Buffer[size] != cs)
  {
    printf("wrong CS. size = %d, cs = %d, NewMsgStack[gReadStack][size] = %d\n", size, cs, NewMsgStack[gReadStack].Buffer[size]);
    NewMsgStack[gReadStack].Status = CELL_EMPTY;
    return false;
  }
  g_bRadioOK = 0;
  type = GetCurrentMsgType();
  //copy to struct
  for ( i = 0; i < size; i++)
    if ((type == MSG_CONFIG) || (type == MSG_FW_UPDATE))
    {
      ((( uint8_t *) &mntr)[i]) = NewMsgStack[gReadStack].Buffer[i+1];//radioRxPkt[i+1];//NewMsgStack[gReadStack][i+1];
//      printf("radioRxPkt[%d] = %d", i+1, NewMsgStack[gReadStack].Buffer[i+1]); // radioRxPkt[i+1]);//NewMsgStack[gReadStack][i+1]);
    }
    else
    {
      ((( uint8_t *) &msgIn)[i]) = NewMsgStack[gReadStack].Buffer[i+1];
    }
  NewMsgStack[gReadStack].Status = CELL_EMPTY;

  if ((type != MSG_CONFIG) && (type != MSG_FW_UPDATE))
    if (msgIn.Header.m_addressee != getSensorID()) //myData.m_ID)
      return false;

  DefineRadio_PWR_LVL2LGR(NewMsgStack[gReadStack].Rssi);  //todo
  uint32_t sensor_id_from_monitor = little_endian_to_uint32(&mntr.m_buffer[2]);
  switch (type)
  {
    case MSG_CONFIG:
      if (mntr.m_Header != HEADER_GETID_ACK)
      {
        printf("Header not Fit\n");
        break;
      }
//      if (Bytes2Int(&mntr.m_buffer[0]) != g_iBtr) //todo
//      {
//        printf("Echo Battery not fit");
//        break;
//      }
      writeFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS, sensor_id_from_monitor);
      initialize_sensor_details();
//      myData.m_ID = Bytes2Long(&mntr.m_buffer[2]);
      printf("set new ID: %lu\n", getSensorID());
//      if (APP_StoreData(PAGE_ID_TYPE) != ECODE_EMDRV_NVM_OK)
//        break;

      res = true;
      break;
    case MSG_DATA:
      printf("Ack for data sending. from logger: %lu. my logger %lu\n", msgIn.Header.m_ID, g_LoggerID);
      //check if its the right message type
      if (msgIn.Header.m_Header != HEADER_SND_DATA_ACK)
        break;

//      if (g_bAlert2Send == false)
      {
        g_nMin = msgIn.RecAckPayload.m_min;
        g_nSec = msgIn.RecAckPayload.m_sec;
        printf("time: %d:%d\n", g_nMin, g_nSec);
        if ((!g_bOnReset) && (g_bIsFirstRound == true))
        {
          uint8_t t = (100 - g_time2EndHubSlot) / 10; // calc seconds from hub slot started
          int16_t dl = CalcDrift();
          printf("logger time: %d:%d. Sec from Slot started: %d drift: %d\n", g_nMin, g_nSec, t, dl);
//          if ((dl > -10) && (dl < 0))     //todo
//          {
//            uint16_t t = g_time2EndHubSlot + (dl * 10);
//            printf("set new timer: %d", t);
//            // reset 10 sec timer
//            Set10SecTimer();
//          }
          // check if slot has drift in time
          uint16_t curSlot = (g_nMin * 60 + g_nSec - t) / SLOT_INTERVAL_SEC;
          if ((curSlot != g_nCurTimeSlot))
          {
            g_nDeltaOfSlots = g_nCurTimeSlot - curSlot; //
            printf("different between hub slot to rec slot of %d\n", g_nDeltaOfSlots);
//            if ((g_nDeltaOfSlots != -1) && (g_nDeltaOfSlots != -2) && (g_nDeltaOfSlots != 1))   //todo
//            {
//              g_nCurTimeSlot = curSlot;
//              SetSensorsDeltaUpdt();
//            }
          }
        }
        // if it was broadcast - save the logger id and  HUBSLOT number
        if (g_LoggerID == DEFAULT_ID)
        {
          g_LoggerID = msgIn.Header.m_ID;
          printf("logger ID: %lu\n", g_LoggerID);
          if (g_bOnReset == false)
            g_bSwapLgr = true;
        }
        //if logger sent new hubslot - change it
        if (msgIn.RecAckPayload.m_slotMin != 0xFF)
        {
            if (SetHubSlot(msgIn.RecAckPayload.m_slotMin) == false)
              return false;
        }
      }
      // sign all data that was send OK - can be removedvoid ResetAllSensorsData()
      ResetAllSensorsData();

      res = true;
      break;
    case MSG_HUB_PRM:
      //check if its the right message type
      if (msgIn.Header.m_Header != HEADER_HUB_PRM_ACK)
        break;
      if (msgIn.Header.m_addressee != getSensorID())  //myData.m_ID)
        break;
      res = true;
      break;
    case MSG_SNS_PRM:
      //check if its the right message type
      if (msgIn.Header.m_Header != HEADER_HUB_SNS_PRM_ACK)
        break;
      if (msgIn.Header.m_addressee != getSensorID())//myData.m_ID)
        break;
      senIndex = 0;
      do
      {
        senIndex = GetNextSensor(senIndex);
        if (senIndex < MAX_DATA)
        {
          if (MySensorsArr[senIndex].Status == SEN_STATUS_SEND_PRM)
          {
            MySensorsArr[senIndex].Status = SEN_STATUS_CELL_EMPTY;  //SEN_STATUS_DATA_OK;//
            printf("Parameters of index %d sent OK\n", senIndex);
          }
          senIndex++;
        }
      }
      while (senIndex < MAX_DATA);
      res = true;
      break;
//    case MSG_FW_UPDATE:
//      printf("got update");
//      if (mntr.m_Header == HEADER_HUB_UPDATE_START_PKT)
//      {
//        SavePacketPrm(&radioRxPkt[3]);
//        res = true;
//      }
//      break;
    default:
      break;
  }//switch (msg_type)

  return res;
}
