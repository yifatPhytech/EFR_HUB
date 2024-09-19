/*
 * hub_protocols.c
 *
 *  Created on: Sep 10, 2024
 *      Author: Yifat
 */
#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/Hub_Definition/rf_rx_handle.h"

InputRecord NewMsgStack[MAX_MSG_IN_STACK];
volatile int8_t     gReadStack = 0;


void ResetAfterReadRow()
{
  ResetRow(gReadStack);
}

uint8_t GetFirstBusyCell()
{
    for (int8_t i = 0; i < MAX_MSG_IN_STACK; i++)
      if (NewMsgStack[i].Status == CELL_BUSY)
        return i;
    return MAX_MSG_IN_STACK;
}

uint8_t GetFirstEmptyCell()
{
    for (int8_t i = 0; i < MAX_MSG_IN_STACK; i++)
      if (NewMsgStack[i].Status == CELL_EMPTY)
        return i;
    return MAX_MSG_IN_STACK;
}

void ResetRow(uint8_t index)
{
  if (index < MAX_MSG_IN_STACK)
    NewMsgStack[index].Status = CELL_EMPTY;
}

void ResetAll()
{
  for (uint8_t i = 0; i < MAX_MSG_IN_STACK; i++)
    NewMsgStack[i].Status = CELL_EMPTY;

}

bool IsNewRxData()
{
    gReadStack = GetFirstBusyCell();
    if (gReadStack != MAX_MSG_IN_STACK)
        return true;
    return false;
}

bool SaveNewPacket(uint8_t* radioRxPkt, uint16_t packet_length, int16_t nRssi)
{
  uint8_t gWriteStack = GetFirstEmptyCell();
      if (gWriteStack == MAX_MSG_IN_STACK)
      {
        printf("doesn't have empty cell. missed msg");
        return false;
      }
      if (packet_length > MAX_EZR_BUFFER_SIZE)
        return false;
      printf("save packet at index %d\n", gWriteStack);
      memcpy(&NewMsgStack[gWriteStack].Buffer, radioRxPkt, packet_length);
//      for (int8_t i = 0; i <= radioRxPkt[0]+1; i++)
//        NewMsgStack[gWriteStack].Buffer[i] = radioRxPkt[i];
      NewMsgStack[gWriteStack].Rssi = nRssi;
      NewMsgStack[gWriteStack].Status = CELL_BUSY;
      return true;
}
