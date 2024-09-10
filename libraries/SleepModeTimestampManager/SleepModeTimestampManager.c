/*
 * SleepModeTimestampManager.c
 *
 *  Created on: Sep 20, 2023
 *      Author: itay
 */
#include <stdio.h>        // for printf
#include <em_device.h>
#include <em_chip.h>
#include <em_cmu.h>
#include <em_gpio.h>
#include <em_emu.h>
#include <em_burtc.h>

// My libraries
#include "libraries/SleepModeTimestampManager/SleepModeTimestampManager.h"
#include "libraries/system_mode/system_mode.h"
#include "libraries/UART/UARTComm.h"


#define ULFRCO_TICKS_PER_SECOND 1000  // ULFRCO -> 1 kHz
static uint32_t WAKEUP_INTERVAL_TICKS;

static void BURTC_Setup(void);
static void ConfigureWakeupPin(void);

static void BURTC_Setup(void)
{
  // Enable the ULFRCO and set it as the clock source for BURTC
  CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
  CMU_ClockSelectSet(cmuClock_BURTC, cmuSelect_ULFRCO);
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);

  // Enable the BURTC clock
  CMU_ClockEnable(cmuClock_BURTC, true);
  CMU_ClockEnable(cmuClock_BURAM, true);


  // Define the BURTC initialization structure
  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.start = true;               // Start BURTC after initialization
  burtcInit.debugRun = false;           // Counter will halt under debug
  burtcInit.clkDiv = 1;                 // Assuming you want the fastest clock (this might need adjustment) range: 1-32768
  burtcInit.compare0Top = true;         // Reset the counter when COMP0 is reached
  burtcInit.em4comp = true;             // Enable EM4 wakeup on compare match
  burtcInit.em4overflow = false;        // Disable EM4 wakeup on counter overflow

  BURTC_Init(&burtcInit);

  // Set the compare value for the BURTC
  WAKEUP_INTERVAL_TICKS = WAKEUP_TIME_PERIOD * ULFRCO_TICKS_PER_SECOND;
  BURTC_CounterReset();
  BURTC_CompareSet(0, WAKEUP_INTERVAL_TICKS);

  // Enable interrupt for BURTC COMP0 match
  BURTC_IntEnable(BURTC_IEN_COMP);            // compare match

  // BURTC interrupt is correctly configured and enabled in the NVIC.
  NVIC_EnableIRQ(BURTC_IRQn); // enable or disable BURTC_IRQHandler function **

  BURTC_Enable(true);
}

static void ConfigureWakeupPin(void)
{
  // Configure PC5 as input with pull-up
  GPIO_PinModeSet(EM4WU_PORT, EM4WU_PIN, gpioModeInputPullFilter, 1);
  //GPIO_PinModeSet(gpioPortC, 5, gpioModeInputPull, 1); // 1 for pull-up

  // Configure interrupt for PC5 using GPIO_ExtIntConfig
  // intNo is 5 for pin 5, as it belongs to the group 1 (pins 4-7)
  GPIO_ExtIntConfig(EM4WU_PORT, EM4WU_PIN, EM4WU_EM4WUEN_NUM, true, false, true); // (port, pin, intNo, risingEdge, fallingEdge, enable)
  GPIO_EM4EnablePinWakeup(EM4WU_EM4WUEN_MASK << _GPIO_EM4WUEN_EM4WUEN_SHIFT, 0);

  // Enable GPIO_ODD interrupt in NVIC
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  EMU_EM4Init(&em4Init);
}

void SMTM_Init(void)
{
  BURTC_Setup();              // Initialize the BURTC
  ConfigureWakeupPin();       // Configure GPIO PC5 as wake-up pin
}

void SMTM_EnterDeepSleep(void)
{
  const char *message = "going to sleep!\n";
  UARTComm_SendString(message, strlen(message)); // strlen is safe here because the string is a literal and null-terminated

  // Enter deep sleep mode (EM4)
  EMU_EnterEM4();
}

void SMTM_BURTC_IRQHandler(void)
{
  // Clear the BURTC interrupt flag
  BURTC_IntClear(BURTC_IEN_COMP);
  printf("Backup RTC interrupt handler.\r\n");
}
