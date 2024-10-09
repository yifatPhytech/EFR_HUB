/*
 * Timers.c
 *
 *  Created on: Sep 10, 2024
 *      Author: Yifat
 */
#include <stdio.h>
#include <stdbool.h>
#include "libraries/tools/timers.h"


void SetTimer(sl_sleeptimer_timer_handle_t* timer, uint32_t timeout, void *callback_data)
{
  bool bTimerRun = false;
  uint32_t ticks;
  printf("set new timer for %lu ms\n", timeout);
    if (sl_sleeptimer_is_timer_running(timer, &bTimerRun) == SL_STATUS_OK)
      {
        if (bTimerRun)
          {
            printf("first stop timer\n");
          sl_sleeptimer_stop_timer(timer);
          }
//          ticks = sl_sleeptimer_ms_to_tick(timeout);
          if (sl_sleeptimer_ms32_to_tick(timeout, &ticks) == SL_STATUS_OK)
            {
          printf("tick = %lu\n", ticks);
          if (sl_sleeptimer_start_periodic_timer(timer, ticks,  callback_data, NULL, 0, 0) != SL_STATUS_OK)
            printf("failed to set timer\n");
            }
      }
}

void StopTimer(sl_sleeptimer_timer_handle_t* timer)
{
  bool bTimerRun = false;
  if (sl_sleeptimer_is_timer_running(timer, &bTimerRun) == SL_STATUS_OK)
    {
      if (bTimerRun)
        sl_sleeptimer_stop_timer(timer);
    }
}
