/*
 * tcxo_hander.c
 *
 *  Created on: Oct 1, 2023
 *      Author: itay
 */
#include "libraries/tcxo_handler/tcxo_handler.h"

#include "sl_sleeptimer.h"  // Include the sleep timer header for EFR32
#include <em_core.h>
#include <em_core_generic.h>
#include <rail_types.h>
#include <em_cmu.h>    // Library for Clock Management Unit (CMU) peripheral access
#include <stdio.h>     // for printf function
#include <rail.h>


//static void switchToRcClock(void);

// Switch to the internal RC clock
 void switchToRcClock(void)
{
  // HFRCODPLL refers to a High Frequency RC Oscillator with a Digitally Controlled PLL (Phase-Locked Loop).
  // This type of oscillator provides a stable high-frequency clock that can be used for CPU operation
  // and other high-speed peripherals. The digitally controlled PLL allows for frequency stabilization
  // and adjustment, making it a flexible clock source that can be fine-tuned for precise clock management.
  // Enable HFRCODPLL and wait for it to be ready
  CMU_OscillatorEnable(cmuOsc_FSRCO, true, true); // cmuOsc_HFRCODPLL

  // After enabling the HFRCODPLL, we switch the system clock source to it.
  // The system clock (SYSCLK) is the primary clock from which the CPU and peripheral clocks are derived.
  // Switching to HFRCODPLL for SYSCLK generally improves the energy efficiency of the system
  // while providing a reliable clock source for high-performance operation.
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO); //cmuSelect_HFRCODPLL

  // The HFXO is a High Frequency Crystal Oscillator that provides a precise and stable clock source,
  // but it consumes more power compared to the HFRCODPLL. Disabling the HFXO when it's not needed
  // saves power, which is particularly important in battery-operated or energy-sensitive applications.
  // Disable HFXO as it's no longer needed
  CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
}

void switchToTcxoClock(void)
{
    // First, enable the HFXO (High Frequency Crystal Oscillator).
    // This ensures that the external TCXO clock is ready before we switch the system clock to it.
    CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

    // Optionally, wait until HFXO is ready if your hardware requires it
    // You can check the status or wait in a loop until the oscillator is stable.

    // Switch the system clock source to HFXO.
    CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);

    // Optionally disable the HFRCODPLL since it's no longer needed.
    CMU_OscillatorEnable(cmuOsc_HFRCODPLL, false, false);
}

void init_tcxo(void)
{
  //switchToRcClock();

  // Enable the clock for the GPIO module
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PA0 as an output
  GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 1); // PA0

  // Set PA0 pin to activate the radio TCXO
  GPIO_PinOutSet(gpioPortA, 0);

}

void enable_tcxo(void)
{
  // printf("enable_tcxo");

  // Set PA0 pin to activate the radio TCXO
  GPIO_PinOutSet(gpioPortA, 0);

  // TCXO stabilization time is ~5 milliseconds.
  sl_sleeptimer_delay_millisecond(5);

  switchToTcxoClock();
  //CMU_OscillatorEnable(cmuOsc_HFXO, true, true); // Enable HFXO
}

void disable_tcxo(void)
{
  // printf("disable_tcxo");
  switchToRcClock();

  // Go critical to assess sleep decisions
  //  CORE_irqState_t irqState;
  //  irqState = CORE_EnterCritical();

  // Attempt to put the radio to sleep
  //  RAIL_Status_t status = RAIL_Sleep(0, &deepSleepAllowed);
  //  if (status != RAIL_STATUS_NO_ERROR)
  //    {
  //      CORE_ExitCritical(irqState);
  //      printf("Error: cannot set radio to sleep. Status: %d\n", status);
  //      return;
  //    }

  // Switch the system clock to the internal RC clock before disabling TCXO

  // Delay for a certain period if required
  sl_sleeptimer_delay_millisecond(5);

  // Clear PA0 pin to deactivate the radio TCXO
  GPIO_PinOutClear(gpioPortA, 0);
}
