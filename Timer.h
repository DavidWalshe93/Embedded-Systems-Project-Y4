/*
 * Timer.h
 *
 *  Created on: 19 Nov 2016
 *      Author: David
 */

#ifndef _TIMER_H_
#define _TIMER_H_

// Included Libraries
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "VT100_Commands.h"
#include "rtc_lpo.h"
#include "TimeHolder.h"

// Included extern global functions
extern char char_received();
extern void terminalFormat();

// Included extern global variables
extern volatile int g_updateTime;
extern bool g_clockMode;
extern int g_terminalColour;

/*FUNCTION PROTOTYPES*************************************/

// Function to handle timer functionality from the main loop
void tmr_HandleTimer(char *functionChar, TimeHolder *myTime, TimeHolder *alarm);

// Routine to load the timer with its initial value.
void tmr_LoadTimer(TimeHolder *timer);

#endif /* CUSTOM_LIBRARIES_TIMER_H_ */
