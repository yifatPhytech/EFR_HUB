#ifndef SENSORS_LIST_HANDLE
#define SENSORS_LIST_HANDLE

#include <stdint.h>
#include "libraries/Hub_Definition/hub_define.h"

#define MAX_DATA  40
#define NO_DATA   -9999

typedef struct
{
	uint32_t 	m_ID;
//	uint32_t	m_gpsID;
	uint8_t		m_Type;
} IDStruct;

typedef struct
{
	uint32_t	snsID;
	uint8_t		slot;
}SensorBasic;

extern IDStruct myData;
extern SensorBasic AllSns[MAX_DATA];
extern sensor MySensorsArr[MAX_DATA];


void InitSensorArray();
void InitSnsParams();
uint8_t GetSensorIndex(uint32_t senID);
void RemoveSensor(uint8_t i);
int8_t InsertNewSensor(uint32_t senID,uint8_t senType, bool bMulti);
void writeAllSnsToFlash(void);
void readAllSnsFromFlash(void);



#endif
