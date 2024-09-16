#ifndef RF_RX_HANDLE
#define RF_RX_HANDLE

#include "libraries/Hub_Definition/hub_define.h"

extern InputRecord NewMsgStack[MAX_MSG_IN_STACK];
extern volatile int8_t     gReadStack;

void ResetRow(uint8_t index);
uint8_t GetFirstBusyCell();
uint8_t GetFirstEmptyCell();
void ResetAll();
bool SaveNewPacket(uint8_t* radioRxPkt, uint16_t packet_length, int16_t nRssi);
bool IsNewRxData();

#endif //RF_RX_HANDLE
