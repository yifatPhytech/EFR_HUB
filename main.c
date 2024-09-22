#include "em_emu.h"

#include <libraries/106_ADC/106_adc_reader.h>
#include <libraries/106_BlinkLED/106_BlinkLED.h>
#include <libraries/106_ButtonHandler/106_ButtonHandler.h>
#include "sl_system_process_action.h"
#include "sl_component_catalog.h"
#include "sl_power_manager.h"
#include "sl_event_handler.h"
#include "sl_system_init.h"
#include "libraries/SleepModeTimestampManager/SleepModeTimestampManager.h"
#include "libraries/sensor_processing/sensor_processing.h"
#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/NonBlockingDelay/NonBlockingDelay.h"
#include "libraries/RADIO/rf_monitor_state_machine.h"
#include "libraries/flash_storage/flash_storage.h"
#include "libraries/tcxo_handler/tcxo_handler.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/RADIO/rf_state_machine.h"
#include "libraries/RADIO/radio_handler.h"
#include "libraries/RADIO/rf_protocol.h"
#include "libraries/RADIO/radio_init.h"
#include "libraries/ABP2_Sensor/ABP2_Sensor.h"
#include "libraries/UART/UARTComm.h"
#include "libraries/I2C/I2C.h"
#include "libraries/Sensors_List/SenorsListHandle.h"
#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/Hub_Definition/rf_rx_handle.h"
#include "libraries/Hub_Definition/rf_parser.h"
#include "libraries/Hub_Definition/hub_protocols.h"
#include "libraries/Hub_Definition/sensor_sm.h"
#include "libraries/Hub_Definition/logger_sm.h"
#include "libraries/tools/timers.h"


#include "em_burtc.h"                                 // for BURTC_IRQHandler


#define APP_RTC_TIMEOUT_MS (1000u / APP_RTC_FREQ_HZ)
#define APP_RTC_TIMEOUT_1S (1000u)
#define MAX_OPTIONAL_SLOT   20


static RAIL_Handle_t rail_handle;                     // RAIL handle for radio operations
//static bool is_hourly_or_explosive_message = false;   // is_hourly_or_explosive_message flag

static WorkingMode g_wCurMode = MODE_SENDING;
//static Task       g_nCurTask = TASK_WAIT;
static uint8_t    g_nRetryCnt = 0;
static bool       g_bAlert2Send;
bool       g_bIsFirstRound;
static bool       g_bLsn10Min = false;
volatile int8_t     g_nDeltaOfSlots;
volatile uint8_t    g_hours2NextInstl;
bool       g_bSwapLgr = false;
static bool       g_bHubRndSlot = false;
static bool       g_bDataIn;
volatile  uint16_t  g_nCurTimeSlot;
static bool     g_bflagWakeup;
static bool g_bflagWakeupLsn = false;
static bool g_bflagWakeupSnd = false;
static bool g_bSlotTimerInt = false;
static  uint8_t   g_nHubSlot;
static  bool    g_bRadioStateOpen;

//static int8_t     gReadStack = 0;
//static int8_t     gWriteStack = 0;
volatile uint16_t g_instlCycleCnt;
static uint8_t    g_nHour = 0;
volatile uint8_t    g_nMin;
volatile uint8_t    g_nSec;
bool       g_bSendParams = false;
uint8_t    g_nIndex2SendPrm;
uint32_t  g_CurSensorLsn;
bool       g_bOnReset = true;
uint16_t rtcTickCnt;
bool g_bMissionCompleted;
bool     g_bIsMoreData;
uint16_t  g_time2EndHubSlot;

//sl_sleeptimer_timer_handle_t rtc_tick_timer;
sl_sleeptimer_timer_handle_t rtc10SecTimer;
sl_sleeptimer_timer_handle_t rtcHubLgrTimer;

uint32_t g_LoggerID = DEFAULT_ID;

// NonBlockingDelay instances
NonBlockingDelay led_interval_instance;               // NonBlockingDelay instance for LED toggle
//NonBlockingDelay rtc_tick_instance;               // NonBlockingDelay instance for sensor check
NonBlockingDelay sleepInstance;                       // NonBlockingDelay instance before sleep

void print_header()
{
  printf("\r\n");
  printf("Phytech HUB EFR\r\n");
  printf("for NG systems\r\n");
  printf("Hub ID: %lu\n", sensorDetails.sensorID);
  printf("Version: %c.%d.%d.%d\r\n", sensorDetails.fwVersionType,sensorDetails.fwVersionMonth,sensorDetails.fwVersionYear,sensorDetails.fwVersionIndex);
}

uint8_t GetCheckSum(uint8_t* buf, uint8_t len)
{
  uint8_t i, res = 0;
  for (i = 0; i < len; i++)
    res+= buf[i];
  return res;
}

static const char *GetModeName(WorkingMode mode)
{
  static const char *modeNames[] = {
    [MODE_NONE] = "NONE",
    [MODE_INSTALLATION] = "INSTALLATION",
    [MODE_LISTENING] = "LISTENING",
    [MODE_SENSORS_COMMUNICATION] = "SENSORS_COMMUNICATION",
    [MODE_SENDING] = "SENDING",
    [MODE_SYNCHRO] = "SYNCHRO",
    [MODE_WAIT_RESPONSE] = "WAIT_RESPONSE",
    [MODE_GPS] = "GPS",
    [MODE_WRITE_EEPROM] = "WRITE_EEPROM",
    [MODE_SLEEPING] = "SLEEPING",
    [MODE_END_INSTALL] = "MODE_END_INSTALL",
  };

  return mode < sizeof(modeNames) / sizeof(modeNames[0]) ? modeNames[mode] : "???";
}

//static const char *GetTaskName(Task task)
//{
//  static const char *taskNames[] = {
//    [TASK_WAIT] = "WAIT",
//    [TASK_BUILD_MSG] = "BUILD_MSG",
//    [TASK_SYNC] = "SYNC",
//    [TASK_SEND] = "SEND",
//  };
//
//  return task < sizeof(taskNames) / sizeof(taskNames[0]) ? taskNames[task] : "???";
//}



void SetCurrentMode(WorkingMode newMode)
{
//  if (FOTA_HUB_CLIENT_IsPrepared() || FOTA_HUB_CLIENT_IsEnabled())
//  {
//    if (newMode != MODE_SENDING)
//    {
//      logw("%s disabled during FOTA", GetModeName(newMode));
//      newMode = MODE_SENDING;
//    }
//  }

  if (newMode == g_wCurMode)
  {
    return;
  }

  WorkingMode prevMode = g_wCurMode;

  printf("mode %s (prev %s)\n", GetModeName(newMode), GetModeName(prevMode));
  g_wCurMode = newMode;

  if (prevMode == MODE_SLEEPING)
  {
//    OnExitSleep();
  }

  if (prevMode == MODE_INSTALLATION)
  {
//    OnExitInstallation();
  }
}

WorkingMode GetCurrentMode( )
{
  return g_wCurMode;
}

//static void SetCurrentTask(Task newTask)
//{
//  if (newTask == g_nCurTask)
//  {
//    return;
//  }
//
//  printf("task %s (prev %s)\n", GetTaskName(newTask), GetTaskName(g_nCurTask));
//  g_nCurTask = newTask;
//}

bool SetHubSlot(uint8_t newSlot)
{
  g_nHubSlot = newSlot;
  printf("new HUBSLOT: %d\n", g_nHubSlot);
  // if got unreasonable slot - dont take it
  if (g_nHubSlot >= MAX_OPTIONAL_SLOT)
    if (g_bOnReset)
      return false;
    else
      g_nHubSlot = 0;
  return true;
}

void SetTicksCnt(uint16_t ticks)
{
  rtcTickCnt = ticks;
}

void InitVarsOnReset()
{
  g_bOnReset = true;
  g_nRetryCnt = 0;
  g_bAlert2Send = false;
  g_bIsFirstRound = false;
  g_bSendParams = false;
  g_bLsn10Min = false;
  g_nDeltaOfSlots = 0;
//  g_time2StartSend = 0;
  g_nIndex2SendPrm = 0;
  g_hours2NextInstl = 0;
  g_bSwapLgr = false;
  g_bHubRndSlot = false;
  g_bRadioStateOpen = false;
}

void MoveData2Hstr()
{

}

//void RTC_App_IRQHandler()   //100ms timer
//{
//  if (rtcTickCnt > 0)
//    rtcTickCnt--;
//  if (g_time2EndHubSlot > 0)
//    g_time2EndHubSlot--;
//}

void RTC_TimeSlot()   //10 sec timer
{
  g_bSlotTimerInt = true;
  g_bflagWakeup = true;
}


uint8_t CalcHubSlot()
{
  return (g_nHubSlot * 15 + 62);
}

void RTC_HubSlotTimer()
{
  g_nCurTimeSlot = CalcHubSlot();
  StopTimer(&rtcHubLgrTimer);
//  InitLoggerSM();
  g_bflagWakeup = true;
  g_bflagWakeupSnd = true;
}

void RTC_SnsSlotTimer()
{
  g_nCurTimeSlot = 0;
  StopTimer(&rtcHubLgrTimer);
//  InitSensorSM();
  g_bflagWakeup = true;
  g_bflagWakeupLsn = true;
}

void RadioOn()
{
  if (g_bRadioStateOpen == true)
     return;
  printf("wake-up Radio\n");
// init_tcxo();
////  sl_stack_init();                                      // Initialize Silicon Labs RADIO components
////  rail_handle = Initialize_RADIO();                     // Initialize the radio
//  set_radio_to_wakeup_mode();
//  set_radio_to_rx_mode(rail_handle);
  g_bRadioStateOpen = true;
}

void RadioOff()
{
  if (g_bRadioStateOpen == false)
    return;
  printf("RadioOff\n");
//  set_radio_to_sleep_mode();
//  disable_tcxo();
  g_bRadioStateOpen = false;
}


//bool StartTickTimer()
//{
//  bool bTimerRun = false;
//  if (sl_sleeptimer_is_timer_running(&rtc_tick_timer, &bTimerRun) == SL_STATUS_OK)
//    {
//      if (!bTimerRun)
//        if (sl_sleeptimer_start_periodic_timer_ms(&rtc_tick_timer, 100, RTC_App_IRQHandler, NULL, 0, 0) != SL_STATUS_OK)
//          return false;
//    }
//  return true;
//}

void DeepSleep()
{
  printf("DeepSleep\n");
  StopTimer(&rtc10SecTimer);
  StopTimer(&rtcHubLgrTimer);
  while (g_bflagWakeup == false)
    EMU_EnterEM2(true);
}

void GoToSleep()
{
  printf("GoToSleep\n");
//  StopTimer(&rtc_tick_timer);
  ResetAll();
//  if (RTCDRV_IsRunning(rtc10SecTimer, &bTimerRun) == ECODE_EMDRV_RTCDRV_OK) //todo add
//    if (bTimerRun == false)
//      if (ECODE_EMDRV_RTCDRV_OK
//          != RTCDRV_StartTimer(rtc10SecTimer, rtcdrvTimerTypePeriodic, APP_RTC_TIMEOUT_1S * SLOT_INTERVAL_SEC,
//                     (RTCDRV_Callback_t)RTC_TimeSlot, NULL) )
//        {
//          printf("\r\nfailed to set slots timer");
//        }
//  RTCDRV_Delay(5);  //50

  BlinkLED_offLED0();
  RadioOff();
  g_bflagWakeup = false;
  if (GetCurrentMode() == MODE_SLEEPING)
    DeepSleep();
  sl_sleeptimer_delay_millisecond(100);
  while (g_bflagWakeup == false)
    EMU_EnterEM2(true);
}

bool CanEnterSleep()
{
  if ((IsSnsSmSleep()) && (IsLgrSmSleep()))
      return true;
  return false;
}

/*void GetNextTask()
{
//  if (g_nMaxRetryCnt < g_nRetryCnt)
//    g_nMaxRetryCnt = g_nRetryCnt;
  if (IsNewRxData())//(g_bDataIn)
  {
    g_bDataIn = false;

//    gReadStack =  GetFirstBusyCell();
    if (ParseLoggerMsg() == true)
    {
      g_nRetryCnt = 0;
//      g_nRfFail_cnt = 0;
      printf("\r\nParse OK");
      if (g_msgType == MSG_CONFIG)
        g_nCurTask = TASK_BUILD_MSG;
      else
      if (g_bOnReset)
      {
        if (g_msgType == MSG_FW_UPDATE)
        {
            printf("\r\nshould start FW update");
          g_wCurMode = MODE_WRITE_EEPROM;
          rtcTickCnt = 20;
        }
        else
        {
          if (g_msgType == MSG_DATA)
          {
              printf("\r\nsend params");
            g_msgType = MSG_HUB_PRM;
            g_nCurTask = TASK_BUILD_MSG;
            g_nRetryCnt = 0;
          }
          else
          {
            g_nCurTask = TASK_SYNC;
            rtcTickCnt = SLOT_INTERVAL_SEC * APP_RTC_TIMEOUT_MS;
            printf("\r\nstart sync");
            BlinkLED_onLED0();
//            g_bLedOn = TRUE;
//            g_ledCnt = 10 * APP_RTC_FREQ_HZ;
            return;
          }
        }
      }
      else
      {
        GetNextMission(true);
      }
    }
    ResetAfterReadRow();
//    NewMsgStack[gReadStack].Status = CELL_EMPTY;
//    gReadStack = GetFirstBusyCell();
  }
  else
    if (rtcTickCnt == 0)
    {
        printf("\r\ntimeout");
      if (g_msgType == MSG_CONFIG)
      {
//        g_ledCnt = 0;
        if (myData.m_ID > 0)
        {
          BlinkLED_offLED0();
          sl_sleeptimer_delay_millisecond(5000);
        }
        g_wCurMode = MODE_SLEEPING;
        return;
      }
//      if (appTxActive == true)  //todo
//      {
//        appTxActive = false;
//        printf("\r\nreset fifo. ");
//        ezradioResetTRxFifo();
//      }

      if (g_msgType == MSG_FW_UPDATE)
      {
          printf("\r\nno new FW to upadte");
        g_msgType = MSG_DATA;
        g_nCurTask = TASK_BUILD_MSG;
        g_nRetryCnt = 0;
        return;
      }
      if (g_nRetryCnt < MAX_SEND_RETRY)
      {
        g_nCurTask = TASK_BUILD_MSG;//TASK_SEND;
//        g_time2StartSend = (myData.m_ID % 100)/ 2;
//        if (g_nRetryCnt == 1)   //todo
//          g_curLgrRfPwr = POWER_OUT_13;
//        if (g_nRetryCnt > 1)
//          g_curLgrRfPwr = POWER_OUT_127;

//        ezr32hg_SetPA_PWR_LVL(g_curLgrRfPwr);
      }
      else
        if (g_bOnReset)
        {
          g_wCurMode = MODE_SLEEPING;
          return;
        }
        else
        {
//          g_nRfFail_cnt++;
//          printf("\r\nreset fifo. g_nRfFail_cnt = %d", g_nRfFail_cnt);
//          ezradioResetTRxFifo();
          GetNextMission(false);  //todo
//          if (g_wCurMode == MODE_SLEEPING)
//            if (g_nRfFail_cnt >= FAIL_BEFORE_BC)//2)
//            {
//              g_nRfFail_cnt = 0;
//              g_LoggerID = DEFAULT_ID;
//              printf("\r\nreset radio. g_nRfFail_cnt = %d", g_nRfFail_cnt);
////              ezradio_reset();
////              ezradio_configuration_init(MY_Radio_Configuration_Data_Array);
////              ezradioResetTRxFifo();
////              ezradioStartRx(appRadioHandle);
//            }
        }
    }
}
*/
void Set10SecTimer()
{
  SetTimer(&rtc10SecTimer, APP_RTC_TIMEOUT_1S * SLOT_INTERVAL_SEC, RTC_TimeSlot);

}
void Stop10SecTimer()
{
  StopTimer(&rtc10SecTimer);

}

uint32_t CalcTimeToHubSlot()
{
  uint8_t n = CalcHubSlot();
  if (g_nCurTimeSlot <= g_nHubSlot)
    return (g_nHubSlot - g_nCurTimeSlot) * 10;
  return (MAX_SLOT - g_nCurTimeSlot + g_nHubSlot);
}

uint32_t CalcTimeToSnsSlot()
{
  return (MAX_SLOT - g_nCurTimeSlot) * 10;
}

void SetTimer4Logger()
{
  uint32_t t = CalcTimeToHubSlot();
  printf("set timer to wu for sending %lu seconds\n", t);
  SetTimer(&rtcHubLgrTimer, APP_RTC_TIMEOUT_1S * t, RTC_HubSlotTimer);
}

void SetTimer4Sensors()
{
  uint32_t t = CalcTimeToSnsSlot();
  printf("set timer to wu for listening in %lu seconds\n", t);
  SetTimer(&rtcHubLgrTimer, APP_RTC_TIMEOUT_1S * t, RTC_SnsSlotTimer);
}

/*void TaskManager()
{
  if (g_bflagWakeup == true)
  {
    g_bflagWakeup = false;
    if (g_bHubRndSlot == false)
    {
      HandleTimeSlot();
      printf("\r\nSLOT: %d, MODE: %d, TASK: %d ",g_nCurTimeSlot ,g_wCurMode ,g_nCurTask);
       if (g_nCurTimeSlot == (MAX_SLOT-1))
        MoveData2Hstr();
    }
    else
      g_bHubRndSlot = false;
    if (g_wCurMode != MODE_SLEEPING)
    {
//      if (g_time2StartSend == 0)
      RadioOn();
      StartTickTimer();
    }
  }
  switch (g_wCurMode)
     {
     case MODE_LISTENING:
     case MODE_INSTALLATION:

//     if (g_bDataIn)
//         if ((appTxActive == true) && (rtcTickCnt > 0))//todo
//           break;
//       g_bDataIn = false;

//       gReadStack = GetFirstBusyCell();
 //      do
 //      {
       if (IsNewRxData())
           {
         if (ParseSensorMsg() == true)
         {
//           if  (g_wCurMode != MODE_INSTALLATION)
//             DefineEzradio_PWR_LVL(NewMsgStack[gReadStack].Rssi); //todo
           BufferEnvelopeTransmit();  //todo
           rtcTickCnt = 3;
         }
         ResetAfterReadRow(); //NewMsgStack[gReadStack].Status = CELL_EMPTY;
//         gReadStack = GetFirstBusyCell();
//         if (gReadStack != MAX_MSG_IN_STACK)
//           g_bDataIn = true;
 //      }
 //      while (gReadStack != MAX_MSG_IN_STACK);
     }
     else
       if ((g_bMissionCompleted == true) && (rtcTickCnt == 0))
         g_wCurMode = MODE_SLEEPING;

     break;
     case MODE_SENDING:
       switch (g_nCurTask)
       {
       case TASK_BUILD_MSG:
//         if (g_time2StartSend > 0)
//           break;
//         RadioOn();
         BuildTx();
         SetCurrentTask(TASK_SEND);
 #ifdef POWER_ON_RST
         if ((g_bOnReset) && (g_msgType != MSG_CONFIG))
           RTCDRV_Delay(5000);
 #endif
       break;
       case TASK_SEND:
         BufferEnvelopeTransmit();  //todo
         rtcTickCnt = 7;
         if ((g_bOnReset) || (g_LoggerID == DEFAULT_ID))
         {
           rtcTickCnt = 25;
         }
//       if (g_msgType == MSG_CONFIG)
//         rtcTickCnt = 50;
//       SetCurrentTask(TASK_WAIT);

       break;
//       case TASK_WAIT:
//         GetNextTask();
//
//       break;
       case TASK_SYNC:
         SyncClock();
       break;
       }
       break;

     case MODE_SLEEPING:
//       if (appTxActive == true) //todo
//         break;

       GoToSleep();
       break;
     default:
       printf("\r\ndont know what to do");
       break;
     } // SWITCH
}*/
void HandleSlotTimer()
{
  if (g_bSlotTimerInt)
    g_nCurTimeSlot++;
  g_bSlotTimerInt = false;
  if (g_bflagWakeupLsn)
    {
    g_nCurTimeSlot = 0;
//    InitSensorSM();
    Set10SecTimer();
    g_bflagWakeupLsn = false;
    }
  if (g_bflagWakeupSnd)
    {
      g_nCurTimeSlot = CalcHubSlot();
      InitLoggerSM();
      g_bflagWakeupSnd = false;
      return;
    }

  if (g_nCurTimeSlot >= MAX_SLOT)
    g_nCurTimeSlot = 0;
  printf("wake up! current slot: %d\n", g_nCurTimeSlot);
  if  (GetCurrentMode() == MODE_INSTALLATION)
   {
     if (g_instlCycleCnt >= 1)
     {
       g_instlCycleCnt--;
       printf("another %d slot to end Install hour\n", g_instlCycleCnt);
       if  (g_instlCycleCnt == 0)
        {
            SetCurrentMode(MODE_END_INSTALL);
//           g_bOnReset = false;
//           InitLoggerSM();
        }
     }
   }
   else
   {
       if (g_nCurTimeSlot >= 60)
         {
           Stop10SecTimer();
           SetTimer4Logger();
         }
       else
         {
             if (ShouldListen())
               {
                 RadioOn();
                 InitSensorSM();
               }
         }
     }
}


int main(void)
{
  CHIP_Init();
  // #TODO Note: Current implementation of disabling TCXO causes the microcontroller to hang
  // #TODO Note: Search for 'UART FLUSH' command.
  // #TODO Note: EFR FOTA
  init_tcxo();                                       // Initialize the external TCXO
//  sensor_processing_init();                          // Initialize BURAM for sensor processing
  writeFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS, 1706210);
//  ResetAllSns();
  // Load sensor parameters from flash into sensorInfo_t struct
  if (initialize_sensor_details())                                   // If the sensor details is valid we can switch to ACTIVE_MODE (sensorID, sensorType firmwareVersion)
    {
      setSystemMode(ACTIVE_MODE);
      NonBlockingDelay_Init(&led_interval_instance, 1000);           // Initialize LED toggle delay
    }
  else
    setSystemMode(MONITOR_MODE);

  readAllSnsFromFlash();
  // Set system parameters by sensor type
  setSystemParametersBySensorType(sensorDetails.sensorType);           // Set the system mode

  // Initialize Silicon Labs components
  sl_system_init();

  // Handle sensor operation by sensor type
//  is_hourly_or_explosive_message = handleOperationBySensorType(sensorDetails.sensorType);
//  if (is_hourly_or_explosive_message || getSystemMode() == MONITOR_MODE)
    {
      sl_stack_init();                                      // Initialize Silicon Labs RADIO components
      rail_handle = Initialize_RADIO();                     // Initialize the radio
    }
//  else
//    {
//      disable_tcxo();
//    }

  Init_ButtonHandler();                         // Initialize ButtonHandler library
  init_rf_state_machine();                      // Initialize RF state machine
  init_rf_monitor_state_machine();
  BlinkLED_init();                              // Initialize LED blinking functionality
  initADC();                                    // Initialize ADC library
  Initialize_I2C();                             // Initialize I2C library
  ABP2_Init();                                  // Initialize ABP2 (Pressure I2C sensor)
  UARTComm_init(false);                         // Initialize UART communication
  print_header();
  printf("\r\n\n\nWake Up!\r\n");                   // Send wake-up message
//  SetTimer(&rtc_tick_timer, 100, RTC_App_IRQHandler);
  if (ALLOW_SLEEP) SMTM_Init();                 // Initialize sleep manager if allowed

  // NonBlockingDelay_Init
  NonBlockingDelay_Init(&led_interval_instance, 200);                // Initialize LED toggle delay
//  NonBlockingDelay_Init(&rtc_tick_instance, 100);               // Initialize 100 ms tick timer
  NonBlockingDelay_Init(&sleepInstance, SECONDS_BEFORE_SLEEP_AGAIN); // Initialize sleep delay
  InitSensorArray();
  InitSnsParams();
  SetCurrentMode(MODE_SENDING);
//  SetCurrentMsgType(MSG_DATA);
//  SetCurrentTask(TASK_BUILD_MSG);
  InitLoggerSM();
  InitVarsOnReset();
  BlinkLED_onLED0();

//  WDOG_Enable(true);
  g_bDataIn = 0;

  while (1)
    {
      sl_system_process_action();               // Process Silicon Labs actions
      if (g_bflagWakeup)
        {
          g_bflagWakeup = false;
//          if (g_bSlotTimerInt)
            {
              HandleSlotTimer();
              PrintCurrentSnsState();
              PrintCurrentLgrState();

            }
//          SetTimer(&rtc_tick_timer, 100, RTC_App_IRQHandler);
        }
      switch (getSystemMode())
      {
        case ACTIVE_MODE:
          {
            if (ALLOW_BLINK && (NonBlockingDelay_check(&led_interval_instance))) BlinkLED_toggleLED0();  // Toggle LED if delay met
            Read_ON_OFF_Button();
//            TaskManager();
//            rf_state_machine(rail_handle);    // Process RF state machine
            app_process_action(rail_handle);  // Process radio app actions
            SensorStateMachine();
            LoggerStateMachine();
            UARTComm_process_action();        // Process UART actions
            if (CanEnterSleep())
              GoToSleep();
//            if (ALLOW_SLEEP && (SLEEP_IMMEDIATELY == true || NonBlockingDelay_check(&sleepInstance))) SMTM_EnterDeepSleep();  // Sleep if conditions met
          }
          break;

        case MONITOR_MODE:
          {
            if (MONITOR_LED && NonBlockingDelay_check(&led_interval_instance)) BlinkLED_toggleLED0();  // Toggle LED if delay met
            UARTComm_process_action();        // Process UART actions
            app_process_action(rail_handle);  // Process radio app actions

            // MONITOR Handler
            rf_monitor_state_machine(rail_handle);
//            if (NonBlockingDelay_check(&sensor_check_instance) && initialize_sensor_details() == true)   // Check sensor details periodically
//              {
//                MONITOR_LED = false;
//                //setSystemMode(ACTIVE_MODE);
//              }
          }
          break;
      }
    }
}


void BURTC_IRQHandler(void)
{
  NonBlockingDelay_reset(&sleepInstance);   // reset sleepInstance
  BURTC_IntClear(BURTC_IEN_COMP);
  //SMTM_BURTC_IRQHandler();                // Call the DeepSleepManager's BURTC interrupt handler
}
