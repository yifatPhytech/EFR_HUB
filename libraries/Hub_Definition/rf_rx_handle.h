#ifndef RF_RX_HANDLE
#define RF_RX_HANDLE

#include "libraries/Hub_Definition/hub_define.h"

extern InputRecord NewMsgStack[MAX_MSG_IN_STACK];

void ResetRow(uint8_t index);
uint8_t GetFirstBusyCell();
uint8_t GetFirstEmptyCell();
void ResetAll();


#endif RF_RX_HANDLE
