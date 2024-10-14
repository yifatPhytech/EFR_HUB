
#ifndef MULTI_DATA_SNS_HANDLE
#define MULTI_DATA_SNS_HANDLE


#include "libraries/CommonHeaders/ProtocolDef.h"

typedef struct _MultySensor
{
  uint32_t  ID;
  int16_t   msr[12];
  int16_t   HstrData[MAX_HSTR_CNT][12];
} MultySensor;

bool IsMultySensor(uint32_t id);
uint8_t GetMultySensorIndex(uint32_t id);
void ResetMultiArray();
void RemoveFromMultiSns(uint32_t lID);
uint8_t InsertNewMultiSns(uint32_t senID);
bool SaveMultiData(uint32_t lID, PayloadMultiSenData_n_Hstr payload);

#endif
