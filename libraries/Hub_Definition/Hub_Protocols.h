#ifndef HUB_PROTOCOLS
#define HUB_PROTOCOLS

#include "libraries/CommonHeaders/ProtocolDef.h"

extern Sen_Hub_Rec_Msg msgOut;

uint8_t BuildTx();
bool IsMultySensor(uint32_t id);
uint8_t GetNextSensor(uint8_t i);

#endif
