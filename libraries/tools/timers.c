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
    if (sl_sleeptimer_is_timer_running(timer, &bTimerRun) == SL_STATUS_OK)
      {
        if (bTimerRun)
          {
          sl_sleeptimer_stop_timer(timer);
          }
          if (sl_sleeptimer_start_periodic_timer_ms(timer, timeout, callback_data, NULL, 0, 0) != SL_STATUS_OK)
            printf("failed to set timer\n");
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
