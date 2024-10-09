/*
 * tcxo_hander.h
 *
 *  Created on: Oct 1, 2023
 *      Author: itay
 */

#ifndef LIBRARIES_TCXO_HANDLER_TCXO_HANDLER_H_
#define LIBRARIES_TCXO_HANDLER_TCXO_HANDLER_H_

#include <em_gpio.h>          // Including the necessary GPIO header
#include <em_cmu.h>           // Including the necessary Clock Management Unit header


void init_tcxo(void);      // Declaration of the init_radio function
void enable_tcxo(void);     // Declaration of the enable_tcxo function
void disable_tcxo(void);    // Declaration of the disable_tcxo function
void switchToTcxoClock(void);
void switchToRcClock(void);
#endif /* LIBRARIES_TCXO_HANDLER_TCXO_HANDLER_H_ */
