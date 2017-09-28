/*
 * Timezone.h
 *
 *  Created on: 22 Nov 2016
 *      Author: David
 */

#ifndef CUSTOM_LIBRARIES_TIMEZONE_H_
#define CUSTOM_LIBRARIES_TIMEZONE_H_

// Included Libraries
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_pit_driver.h"
#include "VT100_Commands.h"
#include "rtc_lpo.h"
#include "TimeHolder.h"

// Included extern global functions
extern char char_received();

// Included extern global variables
extern volatile int g_updateTime;
extern int g_terminalColour;

/*FUNCTION PROTOTYPES*************************************/

// Function to handle timezone functionality from the main loop.
void tz_HandleTimezone(char *inputChar, TimeHolder *time);

// Used to print out the GMT time list to the console
void tz_PrintGmtTime(TimeHolder *myTime, const char *locationStr,
		bool clockMode);

#endif /* CUSTOM_LIBRARIES_TIMEZONE_H_ */
