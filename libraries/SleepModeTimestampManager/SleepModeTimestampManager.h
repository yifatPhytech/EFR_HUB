/*
 * SleepModeTimestampManager.h
 *
 *  Created on: Sep 12, 2023
 *      Author: itay
 *
 *  Description:
 *      This header file provides functionalities for managing deep sleep mode
 *      and handling Unix timestamps using the BURAM retention registers.
 */
#ifndef LIBRARIES_SLEEPMODETIMESTAMPMANAGER_SLEEPMODETIMESTAMPMANAGER_H_
#define LIBRARIES_SLEEPMODETIMESTAMPMANAGER_SLEEPMODETIMESTAMPMANAGER_H_

// Include necessary headers
#include "sl_power_manager.h"
#include "em_device.h"

/// ------------------ SleepModeTimestampManager Configuration ------------------
// Wake-up pin configuration
#define EM4WU_PORT          gpioPortC
#define EM4WU_PIN           5
#define EM4WU_EM4WUEN_NUM   (7)                       // PC5 is EM4WUEN pin 7
#define EM4WU_EM4WUEN_MASK  (1 << EM4WU_EM4WUEN_NUM)

/// ------------------ UNIX Timestamp Configuration ------------------

// Define a retention register in BURAM to store the Unix timestamp
#define TIMESTAMP_RET_REG BURAM->RET[0].REG

/// ------------------ Public Function Prototypes ------------------

// SleepModeTimestampManager functions
void SMTM_Init(void);                // Initializes the manager and the RTCC for wakeup
void SMTM_EnterDeepSleep(void);      // Enter deep sleep mode and wait for RTC or other interrupts
void SMTM_BURTC_IRQHandler(void);    // RTCC interrupt handler (to be called from the actual RTC IRQ handler in the main application)

#endif /* LIBRARIES_SLEEPMODETIMESTAMPMANAGER_SLEEPMODETIMESTAMPMANAGER_H_ */
