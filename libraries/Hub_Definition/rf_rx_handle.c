/*
 * hub_protocols.c
 *
 *  Created on: Sep 10, 2024
 *      Author: Yifat
 */
#include "libraries/Hub_Definition/hub_define.h"
#include "libraries/Hub_Definition/rf_rx_handle.h"

InputRecord NewMsgStack[MAX_MSG_IN_STACK];


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
