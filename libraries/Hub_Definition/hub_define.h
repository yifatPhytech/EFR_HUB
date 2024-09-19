#ifndef HUB_DEFINITION
#define HUB_DEFINITION

#include <stdbool.h>
#include <stdint.h>
#include "libraries/CommonHeaders/ProtocolDef.h"

#define DebugMode

#define APP_RTC_FREQ_HZ 10u
#define MAX_SEND_RETRY    4
#define MAX_EZR_BUFFER_SIZE 64

#define DEFAULT_ID  0xFFFFFFFF
#define MAX_SENSOR_SLOT     60
#define SLOT_INTERVAL_SEC   10
#define APP_RTC_TIMEOUT_1S (1000u)

//#define MAX_HSTR_CNT  12  //5
#define MAX_SLOT  360
#define CELL_EMPTY  0
#define CELL_BUSY 1

#define MAX_MSG_IN_STACK  5
#define MAX_AMIT_MSG_LEN  63+1

#define GPIO_LED1_PORT    gpioPortF //todo - check
#define GPIO_LED1_PIN     2

#define SWITCH_OFF_LED  GPIO_PinOutClear(GPIO_LED1_PORT, GPIO_LED1_PIN)
#define SWITCH_ON_LED   GPIO_PinOutSet(GPIO_LED1_PORT, GPIO_LED1_PIN)

typedef struct _InputRecord
{
  uint8_t Buffer[MAX_AMIT_MSG_LEN];
  uint8_t Rssi;
  uint8_t Status;
}InputRecord;

typedef enum _WorkingMode
{
  MODE_NONE,
  MODE_INSTALLATION,
  MODE_LISTENING,
  MODE_SENSORS_COMMUNICATION,
  MODE_SENDING,
  MODE_SYNCHRO,
  MODE_WAIT_RESPONSE,
  MODE_GPS,
  MODE_WRITE_EEPROM,
  MODE_SLEEPING,
}WorkingMode;


typedef enum _SendMsgType
{
  MSG_NONE,
  MSG_DATA,
  MSG_SNS_PRM,
  MSG_HUB_PRM,
  MSG_CONFIG,
  MSG_FW_UPDATE,
  MSG_CHANGE_SLOT,
  MSG_FOTA,
}SendMsgType;

typedef enum _Task
{
  TASK_WAIT,
  TASK_BUILD_MSG,
//  TASK_SLEEP,
  TASK_SYNC,
  TASK_SEND,
  //  TASK_GPS,
}Task;

typedef enum _SensorStatus
{
  SEN_STATUS_GOT_DATA,
  SEN_STATUS_SEND_DATA,
  SEN_STATUS_CELL_EMPTY,
  SEN_STATUS_GOT_ALERT_DATA,
  SEN_STATUS_SEND_PRM,
}SensorStatus;


typedef enum _SlotStatus
{
  SLOT_STATUS_EMPTY,
  SLOT_STATUS_STANDBY,
  SLOT_STATUS_BUSY,
}SlotStatus;

#pragma pack(1)
typedef struct _Slot
{
  uint16_t  index;
  SlotStatus  status;
}Slot;

#pragma pack(1)
typedef struct _sensor
{
	uint32_t 	ID;
	int16_t		msr;
	uint16_t	btr;
	uint8_t		rssi;
	uint8_t		type;
	//uint8_t data[6];
	int16_t 	HstrData[MAX_HSTR_CNT];
//	bool 		IsHstr;
	Slot		slot;
	uint8_t		DailyCnct;
	SensorStatus Status;
	bool 		slotDelta2updt;
	uint32_t 	version;
	uint8_t		IsNew;//DataStatus;
} sensor;

#endif

SendMsgType GetCurrentMsgType();
uint8_t GetCheckSum(uint8_t* buf, uint8_t len);
WorkingMode GetCurrentMode( );
bool SetHubSlot(uint8_t newSlot);
void Set10SecTimer();
void Stop10SecTimer();
void GoToSleep();
void SetCurrentMode(WorkingMode newMode);
void SetTicksCnt(uint16_t ticks);
void RadioOn();
void SetTimer4Sensors();


extern uint32_t g_LoggerID;
extern bool     g_bIsMoreData;
extern uint8_t    g_nIndex2SendPrm;
extern bool       g_bSendParams;
extern bool       g_bOnReset;
extern uint32_t g_LoggerID;
extern volatile uint16_t g_instlCycleCnt;
extern volatile uint8_t    g_hours2NextInstl;
extern volatile int8_t     g_nDeltaOfSlots;
extern uint32_t g_CurSensorLsn;
extern bool g_bMissionCompleted;
extern volatile  uint16_t  g_nCurTimeSlot;
extern bool       g_bSwapLgr;
extern uint16_t g_time2EndHubSlot;
extern volatile uint8_t    g_nMin;
extern volatile uint8_t    g_nSec;
extern bool       g_bIsFirstRound;


