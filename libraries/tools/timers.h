#ifndef TIMERS
#define TIMERS

#include "sl_sleeptimer.h"

void SetTimer(sl_sleeptimer_timer_handle_t* timer, uint32_t timeout, void *callback_data);
void StopTimer(sl_sleeptimer_timer_handle_t* timer);


#endif
