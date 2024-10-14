#include "em_emu.h"
#include "sli_em_cmu.h"
#include <libraries/106_ADC/106_adc_reader.h>
#include <libraries/106_BlinkLED/106_BlinkLED.h>
#include <libraries/106_ButtonHandler/106_ButtonHandler.h>
#include "sl_system_process_action.h"
#include "sl_component_catalog.h"
//#include "sl_power_manager.h"
#include "sl_event_handler.h"
#include "sl_system_init.h"
#include "sl_board_control.h"
#include "libraries/SleepModeTimestampManager/SleepModeTimestampManager.h"
#include "libraries/sensor_processing/sensor_processing.h"
#include "libraries/phytech_protocol/phytech_protocol.h"
#include "libraries/NonBlockingDelay/NonBlockingDelay.h"
#include "libraries/RADIO/rf_monitor_state_machine.h"
#include "libraries/flash_storage/flash_storage.h"
//#include "libraries/tcxo_handler/tcxo_handler.h"
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
static  bool    g_bRadioStateOpen = false;

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
//NonBlockingDelay sleepInstance;                       // NonBlockingDelay instance before sleep

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
//  g_nCurTimeSlot = CalcHubSlot();
  StopTimer(&rtcHubLgrTimer);
//  InitLoggerSM();
  g_bflagWakeup = true;
  g_bflagWakeupSnd = true;
}

void RTC_SnsSlotTimer()
{
  BlinkLED_onLED0();
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
  sl_sleeptimer_delay_millisecond(1);
  set_radio_to_wakeup_mode();
  set_radio_to_rx_mode(rail_handle);
  g_bRadioStateOpen = true;
}

void RadioOff()
{
  if (g_bRadioStateOpen == false)
    return;
  printf("RadioOff\n");
  set_radio_to_sleep_mode(rail_handle);

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
  sl_sleeptimer_delay_millisecond(10);
  while (g_bflagWakeup == false)
    EMU_EnterEM2(true);
}

void GoToSleep()
{
  printf("GoToSleep\n");
//  StopTimer(&rtc_tick_timer);
  ResetAll();
  BlinkLED_offLED0();
  RadioOff();
  g_bflagWakeup = false;
    if (GetCurrentMode() == MODE_SLEEPING)
        DeepSleep();

  sl_sleeptimer_delay_millisecond(10);
  while (g_bflagWakeup == false)
    EMU_EnterEM2(true);
  EUSART_Enable(EUSART1, true);
  EUSART1->CMD = EUSART_CMD_TXEN | EUSART_CMD_RXEN;
//  printf("wake up\n");
}

bool CanEnterSleep()
{
  if ((IsSnsSmSleep()) && (IsLgrSmSleep()))
      return true;
  return false;
}

uint32_t GetTimeLeft()
{
  uint32_t timeLeft, msLeft;
  sl_sleeptimer_get_timer_time_remaining(&rtc10SecTimer, &timeLeft);
  msLeft = sl_sleeptimer_tick_to_ms(timeLeft);
  printf("time left to next 10 seconds timer is: %lu ms: %lu\n", timeLeft, msLeft);
  return (msLeft/100);
}

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
  if (g_nCurTimeSlot <= n)
    return (n - g_nCurTimeSlot) * 10;
  return (MAX_SLOT - g_nCurTimeSlot + n);
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
  uint32_t t = 60;//CalcTimeToSnsSlot();
  printf("set timer to wu for listening in %lu seconds\n", t);
  SetTimer(&rtcHubLgrTimer, APP_RTC_TIMEOUT_1S * t, RTC_SnsSlotTimer);
}

void HandleSlotTimer()
{
  if (g_bSlotTimerInt)
    g_nCurTimeSlot++;
  g_bSlotTimerInt = false;
  if (g_bflagWakeupLsn)
    {
    g_nCurTimeSlot = 0;
    MoveData2Hstr();
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
//  CHIP_Init();
  // #TODO Note: Current implementation of disabling TCXO causes the microcontroller to hang
  // #TODO Note: Search for 'UART FLUSH' command.
  // #TODO Note: EFR FOTA
  // Initialize Silicon Labs components
  sl_system_init();

//  init_tcxo();                                       // Initialize the external TCXO
//  sensor_processing_init();                          // Initialize BURAM for sensor processing
  writeFlash_uint32(SENSOR_ID_FLASH_PAGE_ADDRESS, 1706210);
  // Load sensor parameters from flash into sensorInfo_t struct
  if (initialize_sensor_details())                                   // If the sensor details is valid we can switch to ACTIVE_MODE (sensorID, sensorType firmwareVersion)
    {
      setSystemMode(ACTIVE_MODE);
//      NonBlockingDelay_Init(&led_interval_instance, 500);           // Initialize LED toggle delay
    }
  else
    setSystemMode(MONITOR_MODE);
  NonBlockingDelay_Init(&led_interval_instance, 500);           // Initialize LED toggle delay

//  readAllSnsFromFlash();
  // Set system parameters by sensor type
//  setSystemParametersBySensorType(sensorDetails.sensorType);           // Set the system mode


  // Handle sensor operation by sensor type
//  is_hourly_or_explosive_message = handleOperationBySensorType(sensorDetails.sensorType);
//  if (is_hourly_or_explosive_message || getSystemMode() == MONITOR_MODE)
    {
      sl_stack_init();                                      // Initialize Silicon Labs RADIO components
      rail_handle = Initialize_RADIO();                     // Initialize the radio
//      g_bRadioStateOpen = true;
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
//  ABP2_Init();                                  // Initialize ABP2 (Pressure I2C sensor)
  UARTComm_init(false);                         // Initialize UART communication
  print_header();
  printf("\r\n\n\nWake Up!\r\n");                   // Send wake-up message
//  SetTimer(&rtc_tick_timer, 100, RTC_App_IRQHandler);
//  if (ALLOW_SLEEP) SMTM_Init();                 // Initialize sleep manager if allowed

  // NonBlockingDelay_Init
//  NonBlockingDelay_Init(&led_interval_instance, 200);                // Initialize LED toggle delay
//  NonBlockingDelay_Init(&rtc_tick_instance, 100);               // Initialize 100 ms tick timer
//  NonBlockingDelay_Init(&sleepInstance, SECONDS_BEFORE_SLEEP_AGAIN); // Initialize sleep delay
//  ResetAllSns();
  InitSensorArray();
  InitSnsParams();
  SetCurrentMode(MODE_SENDING);
//  SetCurrentMsgType(MSG_DATA);
//  SetCurrentTask(TASK_BUILD_MSG);
  InitLoggerSM();
  InitVarsOnReset();
  BlinkLED_onLED0();
  NonBlockingDelay_reset(&led_interval_instance);

//  WDOG_Enable(true);
  g_bDataIn = 0;
  ALLOW_BLINK = true;
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
        }
      switch (getSystemMode())
      {
        case ACTIVE_MODE:
          {
            if (ALLOW_BLINK && (NonBlockingDelay_check(&led_interval_instance)))
              {
                BlinkLED_toggleLED0();  // Toggle LED if delay met
//                printf("toggle led\n");
              }
            Read_ON_OFF_Button();
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


//void BURTC_IRQHandler(void)
//{
//  NonBlockingDelay_reset(&sleepInstance);   // reset sleepInstance
//  BURTC_IntClear(BURTC_IEN_COMP);
//  //SMTM_BURTC_IRQHandler();                // Call the DeepSleepManager's BURTC interrupt handler
//}

/*
 * 1. parse RSSI of package (SaveNewPacket)
 * 2. handle radio sleep
 * 3. read button as I2C instead of IO
 * 4. add  set_radio_to_idle_mode function
 * 5. use RAIL_SetTxPowerDbm to change RF power
 * 6. fix bug in flash char * (writeFlash_ucharArray)
 * 7. remove use of tcxo handler
 * 8. use sl_sleeptimer_ms32_to_tick function in settimer function
 *
 */
