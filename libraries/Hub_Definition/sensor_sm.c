/*
 * hub_protocols.c
 *
 *  Created on: Sep 10, 2024
 *      Author: Yifat
 */
#include <stdio.h>
#include <stdbool.h>
//#include <unistd.h> // For sleep function, replace with your real-time system's sleep function

#include "libraries/Sensors_List/SenorsListHandle.h"
#include "libraries/Hub_Definition/rf_parser.h"
#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/RADIO/radio_handler.h"
#include "sl_flex_rail_package_assistant.h"
#include "sl_flex_rail_package_assistant_config.h"
#include "libraries/Sensors_List/slot_Handle.h"
#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/Hub_Definition/sensor_sm.h"
#include "libraries/Hub_Definition/hub_protocols.h"
#include "libraries/Hub_Definition/rf_rx_handle.h"
#include "libraries/tools/timers.h"



// Define states
typedef enum {
    SENSOR_STATE_SLEEP,
    SENSOR_STATE_NEW_SLOT,
    SENSOR_STATE_LISTEN,
    SENSOR_STATE_RECEIVE_MEASUREMENT,
    SENSOR_STATE_SEND_ACK,
    SENSOR_STATE_ERROR_HANDLING
} SensorState;

// Define global variables
SensorState currentSnsState = SENSOR_STATE_SLEEP;
bool measurementReceived = false;
//bool loggerAckReceived = false;
//sl_sleeptimer_timer_handle_t rtc10SecTimer;

bool IsBusySlotOrNeighbor(uint16_t slot, bool bCheckNeighbor)
{
  uint64_t temp;
  if (bCheckNeighbor)
    temp = ((uint64_t)7 << (slot-1));
  else
    temp = ((uint64_t)1 << slot);

//  logd("temp = %l - %l - %l", (uint16_t)(temp >> 32),(uint16_t)(temp >> 16), (uint16_t)temp);

  if ((g_lMySlots & temp) != (uint64_t)0)
  {
    return true;
  }
  return false;
}

bool IsMulySnsrProb(uint8_t type)
{
  switch (type)
  {
    case TYPE_SMP_GNRL:
    case TYPE_SMP_3:
    case TYPE_SMP_4:
    case TYPE_SMP_6:
    case TYPE_SENTEK_SOIL:
    case TYPE_ENV_80:
      return true;
      break;
    default:
      break;
  }
  return false;
}

bool ShouldListen()
{
  uint8_t i = 0, slotOfSns = 0;
  g_CurSensorLsn = 0;

  if (g_instlCycleCnt > 0)
    return true;

  if ((IsBusySlotOrNeighbor(g_nCurTimeSlot, true)) || (IsBusySlotOrNeighbor(g_nCurTimeSlot+g_nDeltaOfSlots, false)))  //((g_nDeltaOfSlots != 0) && (IsBusySlot(g_nCurTimeSlot+g_nDeltaOfSlots))))
    do
    {
      i = GetNextSensor(i);
      if (i < MAX_DATA)
      {
        slotOfSns = MySensorsArr[i].slot.index;
        // if find the cur slot sensor
        if (slotOfSns == g_nCurTimeSlot)
        {
          if ((MySensorsArr[i].IsNew > 1) || (IsMulySnsrProb(MySensorsArr[i].type)) || (IsMultySensor(MySensorsArr[i].ID)))
          {
            g_CurSensorLsn =  MySensorsArr[i].ID;
            return true;
          }
          else
          {
            printf("no need to listen to sensor %lu this hour", MySensorsArr[i].ID);
            return false;
          }
        }
        if ((g_nDeltaOfSlots != 0) && (slotOfSns == g_nCurTimeSlot+g_nDeltaOfSlots))
        {
//          g_CurSensorLsn =  MySensorsArr[i].ID;
          return true;
        }
        // if it first 24 hours after sensor disappeared - and it one slot after or before
        if (((slotOfSns -1 == g_nCurTimeSlot) || (slotOfSns +1 == g_nCurTimeSlot )) &&
            ((MySensorsArr[i].IsNew > 2) && (MySensorsArr[i].IsNew < 24)))
        {
          // check if need to listen
//            g_CurSensorLsn =  MySensorsArr[i].ID;
            return true;
        }
      }
      i++;
    }
    while (i < MAX_DATA);

  return false;
}

// Dummy functions to simulate sensor and logger interactions
//void listenForSensors() {
//    // Simulate listening for sensors
//    printf("Listening for sensors...\n");
//}

void DefineEzradio_PWR_LVL(uint8_t rssi)
{
  uint8_t rfPwr;
  if (rssi >= 110)
    rfPwr = POWER_OUT_0;
  else
    if (rssi >= 90)
      rfPwr = POWER_OUT_1;
    else
      if (rssi >= 70)
        rfPwr = POWER_OUT_2;
      else
        rfPwr = POWER_OUT_3;

//  printf("according to RSSI %d set RF power to: %d", rssi, rfPwr);
  SetNewRfPower(rfPwr);
//  ezr32hg_SetPA_PWR_LVL(rfPwr);
}

void sendMeasurementAck() {
    // Simulate sending an ACK to the sensor
    printf("Measurement ACK sent.\n");
    BufferEnvelopeTransmit();  //todo
//    rtcTickCnt = 3;
}

void handleSnsError()
{
    // Simulate error handling
    printf("Handling error...\n");
}

void InitSensorSM()
{
  currentSnsState = SENSOR_STATE_LISTEN;
  Set10SecTimer();
  printf("start sensors sm");
//    SetCurrentMode(MODE_INSTALLATION);  // g_wCurMode = ;
  SetTicksCnt( SLOT_INTERVAL_SEC * APP_RTC_FREQ_HZ);
}

// State machine logic
void SensorStateMachine()
{
    switch (currentSnsState)
    {
        case SENSOR_STATE_SLEEP:
            // Simulate waiting for the new hour or period
            printf("sensor sm sleeping.\n");
 //           GoToSleep();
            // Transition to Listen state
            break;
        case SENSOR_STATE_NEW_SLOT:
          if  (GetCurrentMode() == MODE_INSTALLATION)
              if (g_instlCycleCnt > 1)
              {
                g_instlCycleCnt--;
              }
              else
                {
                  if (g_nCurTimeSlot >= 60)
                    {
                      Stop10SecTimer();
                      SetTimer4Logger();
                      currentSnsState = SENSOR_STATE_SLEEP;
                    }
                }
            if (ShouldListen())
              currentSnsState = SENSOR_STATE_LISTEN;
            else
              {
              if  (GetCurrentMode() == MODE_INSTALLATION)
                {
                  g_bOnReset = false;
                  InitLoggerSM();
                }
              currentSnsState = SENSOR_STATE_SLEEP;
              }
          break;
        case SENSOR_STATE_LISTEN:
//            listenForSensors();
            // Transition to Receive Measurement state if a measurement is received
            // For simulation, we assume measurement is always received
          if (IsNewRxData())
              currentSnsState = SENSOR_STATE_RECEIVE_MEASUREMENT;
            break;

        case SENSOR_STATE_RECEIVE_MEASUREMENT:
          if (ParseSensorMsg() == true)
          {
 //           if  (g_wCurMode != MODE_INSTALLATION)
              DefineEzradio_PWR_LVL(NewMsgStack[gReadStack].Rssi); //todo
            currentSnsState = SENSOR_STATE_SEND_ACK;
          }
          else
            {
            ResetAfterReadRow();
            currentSnsState = SENSOR_STATE_LISTEN;
            }
            // Transition to Store Measurement state
            break;

        case SENSOR_STATE_SEND_ACK:
          sendMeasurementAck();
            // Transition to Sleep state (simulate waiting for the next send period)
          if (ShouldListen())
            currentSnsState = SENSOR_STATE_LISTEN;
          else
            {
            currentSnsState = SENSOR_STATE_SLEEP;
            }
            break;

        case SENSOR_STATE_ERROR_HANDLING:
          handleSnsError();
            // Transition back to Sleep state after handling error
            currentSnsState = SENSOR_STATE_SLEEP;
            break;

        default:
            // Unexpected state
            printf("Unexpected state!\n");
            break;
    }
}

bool IsSnsSmSleep()
{
  return currentSnsState == SENSOR_STATE_SLEEP;
}

//int main() {
//    while (true) {
//        stateMachine();
//        // Add delay or condition to break the loop in a real system
//        // This loop simulates continuous operation
//        sleep(1); // Replace with actual time handling
//    }
//    return 0;
//}
