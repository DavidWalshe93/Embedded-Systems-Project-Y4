/*
 * structs.h
 *
 *  Created on: 19 Nov 2016
 *      Author: David
 */

#ifndef _TIMEHOLDER_H_
#define _TIMEHOLDER_H_

// Libraries
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "rtc_lpo.h"
#include "VT100_Commands.h"

// Struct is wrapped around the rtc_datetime_t struct to allow for additional functionality such as millisecond
// handling and rawSecond to structured time operations.
typedef struct TimeHolder {
	unsigned int millisecond;
	rtc_datetime_t t;
	unsigned long int rawSecond;
	uint8_t weekday;
} TimeHolder;

// Used to initialize the Timeholder struct to 0.
void th_InitTimeHolder(TimeHolder *th);

// Used to record the stop watch time when in operation.
void th_SaveTime(TimeHolder *th, TimeHolder *snapshot);

// Get the time in seconds from 00:00:00 01:01:1997 to the set time in the rtc_datetime_t struct in TimeHolder.
void th_GetRawSecond(TimeHolder *th);

// Decrement the current raw second variable in TimeHolder by a user defined value // Subtracting time/
void th_DecrementRawSecond(TimeHolder *th, int decrementVal);

// Increment the current rawSecond variable in Timeholder by a user defined value // Adding time.
void th_IncrementRawSecond(TimeHolder *th, unsigned int incrementVal);

// Maps the rawSecond variable to the rtc_datetime_t struct time to seconds, minutes, hours, days, months, years.
void th_MapSecondsToRtcTime(TimeHolder *th);

// Gets the weekday from the rawSecond time from epoch
void th_GetWeekday(TimeHolder *th);

// Prints the current time in 12/24hr format to the console
void th_PrintTime(TimeHolder *myTime, bool clockMode);

// Used to Assign a date to the rtc_datetime_t struct. This time can be offset to increase or decrease the time
void th_SaveTimeOffset(TimeHolder *principleTime, TimeHolder *offsetTime,
		signed long offset);

// Used to check if the time entered is a valid time/date.
bool th_CheckTimeSetupStatus(TimeHolder *myTime);

// Prints the current time and alarm time in 12/24hr format to the console
void th_PrintTimeAlarm(TimeHolder *myTime, TimeHolder *alarmTime,
		bool clockMode);

// Used to set the current time and alarm time during operation
void th_SetDateTime(TimeHolder *time, const char *infoTimeString,
		const char *infoDateString, int setAlarm);

// Used to decrement the millisecond variable in the Timeholder Struct
void th_DecrementMillisecond(TimeHolder *th, int decrementVal);

// Used to encrement the millisecond variable in the Timeholder Struct
void th_IncrementMillisecond(TimeHolder *th, int incremental);

// Get date/time info form the user
void th_GetTimeInfo(char *inputTime, const char valueSeperator, const int SIZE);

// Auto loader for nearest executing alarm
void th_getNearestAlarm(TimeHolder *alarmTime, TimeHolder *nearestAlarmTime,
		TimeHolder *myTime, const int SIZE, bool *alarmReady);

// Attempt to load alarm into rtc and set alarm interrupt to enabled
void th_LoadAlarm(TimeHolder *alarmTime);

#endif /* _TIMEHOLDER_H_ */
