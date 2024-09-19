/*
 * hub_protocols.c
 *
 *  Created on: Sep 10, 2024
 *      Author: Yifat
 */
#include <stdio.h>
#include <stdbool.h>
//#include <unistd.h> // For sleep function, replace with your real-time system's sleep function
#include "libraries/Hub_Definition/logger_sm.h"

#include <libraries/106_BlinkLED/106_BlinkLED.h>

#include "libraries/system_mode/system_mode.h"
#include "libraries/Sensors_List/SenorsListHandle.h"
#include "libraries/Hub_Definition/rf_parser.h"
#include "libraries/Hub_Definition/rf_rx_handle.h"
#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/RADIO/radio_handler.h"
#include "sl_flex_rail_package_assistant.h"
#include "sl_flex_rail_package_assistant_config.h"
#include "libraries/Sensors_List/slot_Handle.h"
#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/Hub_Definition/hub_protocols.h"
#include "libraries/NonBlockingDelay/NonBlockingDelay.h"
#include "libraries/Hub_Definition/sensor_sm.h"

//#define POWER_OUT_1   0 //1 //  -10dbm
//#define POWER_OUT_4   1 //4 //  0dbm
//#define POWER_OUT_13  2 //13  //  10dbm
//#define POWER_OUT_127 3 //127 //  20dbm

#define FAIL_BEFORE_BC      3
#define INSTALLATION_CYCLES   360 //((uint16_t)MAX_SLOT)  //20

// Define states
typedef enum {
    LOGGER_STATE_SLEEP,
    LOGGER_STATE_INIT,
    LOGGER_STATE_SEND_MEASUREMENTS,
    LOGGER_STATE_WAIT_FOR_LOGGER_ACK,
    LOGGER_STATE_SYNC,
    LOGGER_STATE_ERROR_HANDLING,
    LOGGER_STATE_DEINIT
} LoggerState;

// Define global variables
LoggerState currentState = LOGGER_STATE_SLEEP;
bool loggerAckReceived = false;
static SendMsgType g_msgType = MSG_DATA;
static uint8_t    g_nRetryCnt;
static rf_power   g_curLgrRfPwr;
static uint16_t   g_time2StartSend;
static uint8_t    g_nRfFail_cnt = 0;
NonBlockingDelay ACK_timer_instance;               // NonBlockingDelay instance for LED toggle
NonBlockingDelay  single_sec_timer;

#ifdef DebugMode
uint8_t printX = 0;
#endif

static const char *GetLoggerStateName(LoggerState state)
{
  static const char *loggerStateNames[] = {
    [LOGGER_STATE_SLEEP] = "LOGGER_STATE_SLEEP",
    [LOGGER_STATE_INIT] = "LOGGER_STATE_INIT",
    [LOGGER_STATE_SEND_MEASUREMENTS] = "LOGGER_STATE_SEND_MEASUREMENTS",
    [LOGGER_STATE_WAIT_FOR_LOGGER_ACK] = "LOGGER_STATE_WAIT_FOR_LOGGER_ACK",
    [LOGGER_STATE_SYNC] = "LOGGER_STATE_SYNC",
    [LOGGER_STATE_ERROR_HANDLING] = "LOGGER_STATE_ERROR_HANDLING",
    [LOGGER_STATE_DEINIT] = "LOGGER_STATE_DEINIT",
  };

  return loggerStateNames[state];
}

static const char *GetMessageTypeName(SendMsgType msgType)
{
  static const char *msgTypeNames[] = {
    [MSG_NONE] = "NONE",
    [MSG_DATA] = "DATA",
    [MSG_SNS_PRM] = "SNS_PRM",
    [MSG_HUB_PRM] = "HUB_PRM",
    [MSG_CONFIG] = "CONFIG",
    [MSG_FW_UPDATE] = "FW_UPDATE",
    [MSG_CHANGE_SLOT] = "CHANGE_SLOT",
    [MSG_FOTA] = "FOTA",
  };

  return msgType < sizeof(msgTypeNames) / sizeof(msgTypeNames[0]) ? msgTypeNames[msgType] : "???";
}

static void SetCurrentMsgType(SendMsgType newMsgType)
{
//  if (FOTA_HUB_CLIENT_IsPrepared() || FOTA_HUB_CLIENT_IsEnabled())
//  {
//    if (newMsgType != MSG_HUB_PRM && newMsgType != MSG_FOTA)
//    {
//      logw("%s disabled during FOTA", GetMessageTypeName(newMsgType));
//      newMsgType = FOTA_HUB_CLIENT_IsPrepared() || FOTA_HUB_CLIENT_IsDone() ? MSG_HUB_PRM : MSG_FOTA;
//    }
//  }

  if (newMsgType == g_msgType)
  {
    return;
  }

  printf("msgType %s (prev %s)\n", GetMessageTypeName(newMsgType), GetMessageTypeName(g_msgType));
  g_msgType = newMsgType;
}

SendMsgType GetCurrentMsgType()
{
  return g_msgType ;
}

void InitLoggerSM()
{
  uint32_t interval = 500;
//  g_wCurMode = MODE_SENDING;
//  g_nCurTask = TASK_BUILD_MSG;
  g_msgType = MSG_DATA;
  g_nRetryCnt = 0;
//  g_nMaxRetryCnt = 0;
  g_bIsFirstRound = true;
  RadioOn();
//  if (g_nRfFail_cnt > 0)  //todo
//    g_curLgrRfPwr = POWER_OUT_4;
  SetNewRfPower(g_curLgrRfPwr);
  g_time2EndHubSlot = SLOT_INTERVAL_SEC * APP_RTC_FREQ_HZ;
  currentState = LOGGER_STATE_INIT;
  if (g_bOnReset)
    {
    interval = 2500;
    NonBlockingDelay_Init(&single_sec_timer, 1000);

    }
  else
    Set10SecTimer();

  NonBlockingDelay_Init(&ACK_timer_instance, interval);           // Initialize LED toggle delay
}

void DefineRadio_PWR_LVL2LGR(uint8_t rssi)
{
  rf_power requiredPwr = g_curLgrRfPwr;//g_defPower;

    if ((rssi > 90) && (g_curLgrRfPwr > POWER_OUT_0))
      g_curLgrRfPwr--;
    if ((rssi < 70) && (g_curLgrRfPwr < POWER_OUT_3))
      g_curLgrRfPwr++;

  printf("\r\nrssi %d, cur rf power: %d next: %d\n", rssi, requiredPwr, g_curLgrRfPwr);
  SetNewRfPower(g_curLgrRfPwr);
}

LoggerState GetNextMission(bool bPrevMissionOK)
{
  g_nRetryCnt = 0;
  // if more data - do another sending task
  if ((g_bIsMoreData) && (bPrevMissionOK == true))
  {
//    g_nCurTask = TASK_BUILD_MSG;
    g_bIsFirstRound = false;
    return LOGGER_STATE_SEND_MEASUREMENTS;
  }
  else
    if (g_msgType == MSG_DATA)
    {
//        g_nCurTask = TASK_BUILD_MSG;
        g_bIsMoreData = false;
        if (g_bSendParams == true)
          SetCurrentMsgType( MSG_SNS_PRM);
        else
          SetCurrentMsgType(MSG_HUB_PRM);
        return LOGGER_STATE_SEND_MEASUREMENTS;

    }
    else
      if (g_msgType == MSG_SNS_PRM) //&& (g_bSendParams == true))
      {
        {
          SetCurrentMsgType(MSG_HUB_PRM);
          return LOGGER_STATE_SEND_MEASUREMENTS;
//          g_nCurTask = TASK_BUILD_MSG;
        }
      }
      else
        if (g_msgType == MSG_HUB_PRM)
          {
          SetCurrentMsgType( MSG_NONE);
          if (g_bOnReset)
            {
              NonBlockingDelay_reset(&single_sec_timer);
              return LOGGER_STATE_SYNC;
            }
          }
//      else
//        g_wCurMode = MODE_SLEEPING;
  return LOGGER_STATE_DEINIT;// LOGGER_STATE_SLEEP;
}

void sendMeasurementsToLogger()
{
  g_nRetryCnt++;
  // Simulate sending measurements to the logger
  printf("Sending measurements to logger %dth time...\n", g_nRetryCnt);
  NonBlockingDelay_reset(&ACK_timer_instance);
  loggerAckReceived = false;
  BuildTx();
  BufferEnvelopeTransmit();
#ifdef DebugMode
 printX = 0;
#endif

}

void waitForLoggerAck()
{
#ifdef DebugMode
if ( printX == 0)
  {
  printf("Waiting for logger ACK...\n");
  printX = 1;
  }
#endif

    // Simulate waiting for logger ACK
    if (IsNewRxData())
     loggerAckReceived = true; // Simulate receiving ACK
//       currentSnsState = SENSOR_STATE_RECEIVE_MEASUREMENT;

}

LoggerState handleError()
{
  rf_power new_pwr;

  // Simulate error handling
    printf("Handling error...\n");
    if (g_msgType == MSG_CONFIG)
      {
        if (sensorDetails.sensorID == 0)
        {
          BlinkLED_offLED0();
//                RTCDRV_Delay(5000);
        }
        SetCurrentMode(MODE_SLEEPING);
        return LOGGER_STATE_DEINIT; // LOGGER_STATE_SLEEP;
      }
      if (g_nRetryCnt < MAX_SEND_RETRY)
        {
          g_time2StartSend = (sensorDetails.sensorID % 100)/ 2;
          if (g_nRetryCnt == 1)
            new_pwr = POWER_OUT_2;
          if (g_nRetryCnt > 1)
            new_pwr = POWER_OUT_3;
          SetNewRfPower(new_pwr);
//          ezr32hg_SetPA_PWR_LVL(g_curLgrRfPwr);
          return LOGGER_STATE_SEND_MEASUREMENTS;
        }
      else
        {
          if (g_bOnReset)
          {
            SetCurrentMode(MODE_SLEEPING);
            return LOGGER_STATE_SLEEP;
          }
          g_nRfFail_cnt++;
//          if (g_wCurMode == MODE_SLEEPING)
            if (g_nRfFail_cnt >= FAIL_BEFORE_BC)
            {
              g_nRfFail_cnt = 0;
              g_LoggerID = DEFAULT_ID;
            }
            return GetNextMission(false);
        }
}

bool SyncClock()
{
  if (NonBlockingDelay_check(&single_sec_timer))
    {
    g_nSec++;
    if (g_nSec >= 60)
      {
        g_nSec = 0;
        g_nMin++;
        if (g_nMin >= 60)
          g_nMin = 0;
      }
    NonBlockingDelay_reset(&single_sec_timer);
    }
  if ((g_nSec % SLOT_INTERVAL_SEC) == 0)
  {
    g_nCurTimeSlot = (g_nMin * 60 + g_nSec) / SLOT_INTERVAL_SEC;//170;//
    if (g_nCurTimeSlot >= 360)
      g_nCurTimeSlot = 0;
    printf("synchronize! slot is: %d\nstart installation hour\n", g_nCurTimeSlot);
    g_instlCycleCnt = INSTALLATION_CYCLES;// should be: MAX_SLOT. for whole hour.
    g_hours2NextInstl = 6;
    g_bSendParams = true;
    g_nIndex2SendPrm = 0;
    return true;
  }
  return false;
}
// State machine logic
void LoggerStateMachine()
{
  bool bRes = false, bTimeout = false;
  LoggerState prevState = currentState;
    switch (currentState)
    {
        case LOGGER_STATE_SLEEP:
            // Simulate waiting for the new hour or period
//            printf("HUB is sleeping.\n");
//            sleep(1); // Replace with actual time handling
            // Transition to Listen state
//            currentState = LOGGER_STATE_LISTEN;
            break;

        case LOGGER_STATE_INIT:
//          InitLoggerSM();
          currentState = LOGGER_STATE_SEND_MEASUREMENTS;
          break;
        case LOGGER_STATE_SEND_MEASUREMENTS:
            sendMeasurementsToLogger();
            // Transition to Wait for Logger ACK state
            currentState = LOGGER_STATE_WAIT_FOR_LOGGER_ACK;
            break;

        case LOGGER_STATE_WAIT_FOR_LOGGER_ACK:
            waitForLoggerAck();
            if (loggerAckReceived)
              {
                bRes = ParseLoggerMsg();
                currentState = GetNextMission(bRes);
            }
            else
              {
                bTimeout = NonBlockingDelay_check(&ACK_timer_instance);
                if (bTimeout)
                   currentState = LOGGER_STATE_ERROR_HANDLING;
              }
            break;

        case LOGGER_STATE_ERROR_HANDLING:
            currentState = handleError();
            break;

        case LOGGER_STATE_SYNC:
          if (SyncClock())
            {
              SetCurrentMode(MODE_INSTALLATION);
              InitSensorSM();
              currentState = LOGGER_STATE_SLEEP;
            }
          break;

        case LOGGER_STATE_DEINIT:
          Stop10SecTimer();
          SetTimer4Sensors();
          currentState = LOGGER_STATE_SLEEP;
           break;

        default:
            // Unexpected state
            printf("\r\nUnexpected state!\n");
            break;
    }
    if (currentState != prevState)
      printf("set next logger state to %s\n" , GetLoggerStateName(currentState));
}

bool IsLgrSmSleep()
{
  return (currentState == LOGGER_STATE_SLEEP);
}
