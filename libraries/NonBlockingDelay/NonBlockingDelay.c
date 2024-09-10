/*
 * NonBlockingDelay.c
 *
 *  Created on: Sep 20, 2023
 *      Author: itay
 */

#include "libraries/NonBlockingDelay/NonBlockingDelay.h"


void NonBlockingDelay_Init(NonBlockingDelay* delay, uint32_t interval)
{
  NonBlockingDelay_setInterval(delay, interval);
  delay->_previousTicks = 0;
}

void NonBlockingDelay_setInterval(NonBlockingDelay* delay, uint32_t interval)
{
  // Convert the interval from milliseconds to ticks
  delay->_interval = sl_sleeptimer_ms_to_tick(interval);
}

void NonBlockingDelay_reset(NonBlockingDelay* delay)
{
  delay->_previousTicks = sl_sleeptimer_get_tick_count();  // Get the current tick count
}

bool NonBlockingDelay_check(NonBlockingDelay* delay)
{
  uint32_t currentTicks = sl_sleeptimer_get_tick_count();
  if ((currentTicks - delay->_previousTicks) >= delay->_interval)
    {
      delay->_previousTicks = currentTicks;
      return true;
    }
  else
    {
      return false;
    }
}
