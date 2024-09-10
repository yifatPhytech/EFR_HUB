/*
 * NonBlockingDelay.h
 *
 * This header file declares the NonBlockingDelay struct and functions for creating
 * and managing non-blocking delays using the sl_sleeptimer component of the EFR32
 * Sleep Timer library. This allows for delay operations in embedded systems without
 * halting the processor, enabling other tasks to run concurrently.
 *
 * Non-blocking delays are useful in systems that require multitasking or have
 * operations that should not block the main execution thread while waiting for
 * a period to elapse.
 *
 * The NonBlockingDelay struct holds the necessary information to keep track of
 * the delay interval and the last time the delay was checked. Functions are provided
 * to initialize the delay, set the interval, reset the delay, and check if the
 * interval has passed.
 *
 *  Created on: Sep 20, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_NONBLOCKINGDELAY_NONBLOCKINGDELAY_H_
#define LIBRARIES_NONBLOCKINGDELAY_NONBLOCKINGDELAY_H_

#include "sl_sleeptimer.h"  // Include the sleep timer header for EFR32
#include "em_device.h"

typedef struct
{
  uint32_t _interval;
  uint32_t _previousTicks;
} NonBlockingDelay;

void NonBlockingDelay_Init(NonBlockingDelay* delay, uint32_t interval);
void NonBlockingDelay_setInterval(NonBlockingDelay* delay, uint32_t interval);
void NonBlockingDelay_reset(NonBlockingDelay* delay);
bool NonBlockingDelay_check(NonBlockingDelay* delay);

#endif /* LIBRARIES_NONBLOCKINGDELAY_NONBLOCKINGDELAY_H_ */
