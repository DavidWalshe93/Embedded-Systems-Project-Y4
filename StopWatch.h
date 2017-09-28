/*
 * StopWatch.hF
 *
 *  Created on: 19 Nov 2016
 *      Author: David
 */

#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

// Included Libraries
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_pit_driver.h"
#include "VT100_Commands.h"
#include "rtc_lpo.h"
#include "Timeholder.h"

// Included extern global functions
extern char char_received();

// Included extern global variables
extern volatile int g_updateTime;
extern volatile int g_millis;
extern int g_terminalColour;

/*FUNCTION PROTOTYPES*************************************/

// Used to control and print out the stop watch counter.
void sw_HandleStopWatch(char *inputChar);
void sw_RunStopWatch(char *inputChar, TimeHolder *currentStopWatchTime,
		TimeHolder *lapTime, TimeHolder *temp);
void sw_PrintStopwatchTime(TimeHolder *currentStopWatchTime,
		TimeHolder *lapTime, TimeHolder *temp);

#endif /* CUSTOM_LIBRARIES_STOPWATCH_H_ */
