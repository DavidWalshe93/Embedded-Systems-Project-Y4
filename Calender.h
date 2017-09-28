/*
 * Calender.h
 *
 *  Created on: 25 Nov 2016
 *      Author: David
 */

#ifndef _CALENDER_H_
#define _CALENDER_H_

// Included Libraries
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "VT100_Commands.h"
#include "rtc_lpo.h"
#include "TimeHolder.h"

/*EXTERN VARIABLES****************************************/
extern int g_terminalColour;

/*FUNCTION PROTOTYPES*************************************/

// Function to handle calender functionalilty from the main loop
void cal_HandleCalender(char *functionChar, TimeHolder *myTime,
		TimeHolder *alarmTime);

// Check if its a fixed date holiday i.e. New Years Day
bool cal_CheckFixedHoliday(TimeHolder *calenderTime);

// Check if its an unfixed date holiday i.e. May Bank Holiday
bool cal_CheckUnfixedHoliday(TimeHolder *calenderTime, int offset);

// Check if the date is the current date set
bool cal_CheckCurrentDay(TimeHolder *calenderTime, TimeHolder *currentTime);

// Check if its an alarm day.
bool cal_CheckAlarmDay(TimeHolder *calenderTime, TimeHolder *currentTime);

#endif /* _CALENDER_H_ */
