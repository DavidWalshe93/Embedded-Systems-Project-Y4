/*
 * structs.c
 *
 *  Created on: 19 Nov 2016
 *      Author: David
 */

#include "Timeholder.h"

/*FUNCTION**********************************************************************
 *
 * Function Name : th_InitTimeHolder
 * Description   : This function accepts a struct pointer of type TimeHolder.
 * and assigns 00:00:00 01:01:1970 to the rtc_datetime_t struct member of
 * Timeholder
 *
 *END**************************************************************************/
void th_InitTimeHolder(TimeHolder *th) {
	th->millisecond = 0;
	th->t.second = 0;
	th->t.minute = 0;
	th->t.hour = 0;
	th->t.day = 1;
	th->t.month = 1;
	th->t.year = 1970;
	th->weekday = 0;
	th->rawSecond = 0;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_SaveTime
 * Description   : This function accepts two struct pointers of type TimeHoler
 * and assigns the members of the first to the members of the second.
 *
 *END**************************************************************************/
void th_SaveTime(TimeHolder *th, TimeHolder *snapshot) {
	snapshot->millisecond = th->millisecond;		// Assign milliseconds
	th_GetRawSecond(th);		// Get the raw second time from the struct th
	snapshot->rawSecond = th->rawSecond;// Assign the th->rawSecond to the snapshot->rawSecond
	th_MapSecondsToRtcTime(snapshot);// Map the rawSecond time to a rtc_datetime_t struct
}

void th_SaveTimeOffset(TimeHolder *principleTime, TimeHolder *offsetTime,
		signed long offset) {
	th_GetRawSecond(principleTime);
	offsetTime->rawSecond = principleTime->rawSecond + offset;
	th_MapSecondsToRtcTime(offsetTime);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_GetRawSecond
 * Description   : This function accepts a struct pointer of type TimeHolder.
 * It then calculates the seconds of the rtc_datetime_t struct member by using
 * the API function RTC_HAL_ConvertDatetimeToSecs(). This then returns the time
 * that has passed from 00:00:00 01-01-1970 until the set date in the rtc-datetime-t
 * struct member. The value of this conversion is saved to the member variable
 * rawSecond within the TimeHolder struct.
 *
 *END**************************************************************************/
void th_GetRawSecond(TimeHolder *th) {
	RTC_HAL_ConvertDatetimeToSecs(&(th->t), &(th->rawSecond));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_DecrementRawSecond
 * Description   : This function accepts a struct pointer of type TimeHoler and
 * an integer value. The function subtracts the integer value from the rawSecond
 * member. To prevent raw second form hitting a negative value a for loop is used
 * subtract 1 from rawSecond and the value of rawSecond is checked on each iteration.
 * If raw Second is still more then 0 then rawSecond is reduced by 1.
 * If it equals 0 then the for loop ends. The result of the subtraction is then
 * mapped to the TimeHolder struct passed.
 *
 *END**************************************************************************/
void th_DecrementRawSecond(TimeHolder *th, int decrementVal) {
	th_GetRawSecond(th);
	for (int i = 0; i < decrementVal; i++) {
		if (th->rawSecond > 0)
			th->rawSecond--;
		else
			break;
	}
	th_MapSecondsToRtcTime(th);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_IncrementRawSecond
 * Description   : This function accepts a struct pointer of type TimeHoler and
 * an integer value. The function adds the integer value to the rawSecond
 * member. To prevent raw second form hitting a LONG_INT_MAX, a for loop is used
 * add 1 to rawSecond and the value of rawSecond is checked on each iteration.
 * If rawSecond is still less then LONG_INT_MAX  then rawSecond is increased by 1.
 * If it equals 0 then the for loop ends. The result of the subtraction is then
 * mapped to the TimeHolder struct passed.
 *
 *END**************************************************************************/
void th_IncrementRawSecond(TimeHolder *th, unsigned int incrementVal) {
	const long int LONG_INT_MAX = (4, 294, 967, 295 - 1);
	th_GetRawSecond(th);
	for (int i = 0; i < incrementVal; i++) {
		if (th->rawSecond < LONG_INT_MAX)
			th->rawSecond++;
		else
			break;
	}
	th_MapSecondsToRtcTime(th);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_MapSecondsToRtcTime
 * Description   : This function accepts a struct pointer of type TimeHoler and
 * makes use of the API function RTC_HAL_ConvertSecsToDatetime. This function
 * maps the rawSecond member to second, minute, hour, day, month and year
 * members of the rtc_datetime_t struct member of TimeHolder
 *
 *END**************************************************************************/
void th_MapSecondsToRtcTime(TimeHolder *th) {
	RTC_HAL_ConvertSecsToDatetime(&(th->rawSecond), &(th->t));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_GetWeekday
 * Description   : This function accepts a struct pointer and calculates the
 * day of the week, by finding the days of the week from epoch to the
 * time in the passed struct and modulus he answer by 7 to get the current day.
 *
 *END**************************************************************************/
void th_GetWeekday(TimeHolder *th) {
	const int SECONDS_IN_A_DAY = 86400;
	const int DAYS_IN_A_WEEK = 7;
	int dayOffset = 0;
	th_GetRawSecond(th);
	dayOffset = (th->rawSecond / SECONDS_IN_A_DAY) % DAYS_IN_A_WEEK;
	switch (dayOffset) {
	case 0:
		th->weekday = 1;		// Tuesday
		break;
	case 1:
		th->weekday = 2;		// Wednesday
		break;
	case 2:
		th->weekday = 3;		// Thursday
		break;
	case 3:
		th->weekday = 4;		// Friday
		break;
	case 4:
		th->weekday = 5;		// Saturday
		break;
	case 5:
		th->weekday = 6;		// Sunday
		break;
	case 6:
		th->weekday = 0;		// Monday
		break;
	default:
		th->weekday = 0;
		break;
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_PrintTimeAlarm
 * Description   : This function accepts 2 Timeholder struct pointers and a bool flag.
 * The contents of the Timeholder structs are then printed to the screen. The format,
 * 12/24hr, is decided by the bool flag.
 *
 *END**************************************************************************/
void th_PrintTimeAlarm(TimeHolder *myTime, TimeHolder *alarmTime,
bool clockMode) {
	char timeSuffix = 'A';
	char alarmTimeSuffix = 'A';

	if (myTime->t.hour < 12)
		timeSuffix = 'A';
	else
		timeSuffix = 'P';

	if (myTime->t.hour < 12)
		timeSuffix = 'A';
	else
		timeSuffix = 'P';

	PRINTF("Real Time Clock\r\n");
	PRINTF("Current Time:\r\n");
	if ((clockMode) && (myTime->t.hour > 12))
		PRINTF("%02d:%02d:%02d %cM %02d-%02d-%04d \r\n", myTime->t.hour - 12,
				myTime->t.minute, myTime->t.second, timeSuffix, myTime->t.day,
				myTime->t.month, myTime->t.year);
	else if ((clockMode) && (myTime->t.hour == 0))
		PRINTF("%02d:%02d:%02d %cM %02d-%02d-%04d \r\n", myTime->t.hour + 12,
				myTime->t.minute, myTime->t.second, timeSuffix, myTime->t.day,
				myTime->t.month, myTime->t.year);
	else if (clockMode)
		PRINTF("%02d:%02d:%02d %cM %02d-%02d-%04d \r\n", myTime->t.hour,
				myTime->t.minute, myTime->t.second, timeSuffix, myTime->t.day,
				myTime->t.month, myTime->t.year);
	else
		PRINTF("%02d:%02d:%02d %02d-%02d-%04d \r\n", myTime->t.hour,
				myTime->t.minute, myTime->t.second, myTime->t.day,
				myTime->t.month, myTime->t.year);

	PRINTF("Alarm Times:\r\n");
	if ((clockMode) && (alarmTime->t.hour > 12))
		PRINTF("%02d:%02d:%02d %cM %02d-%02d-%04d \r\n", alarmTime->t.hour - 12,
				alarmTime->t.minute, alarmTime->t.second, timeSuffix,
				alarmTime->t.day, alarmTime->t.month, alarmTime->t.year);
	else if ((clockMode) && (alarmTime->t.hour == 0))
		PRINTF("%02d:%02d:%02d %cM %02d-%02d-%04d \r\n", alarmTime->t.hour + 12,
				alarmTime->t.minute, alarmTime->t.second, timeSuffix,
				alarmTime->t.day, alarmTime->t.month, alarmTime->t.year);
	else if (clockMode)
		PRINTF("%02d:%02d:%02d %cM %02d-%02d-%04d \r\n", alarmTime->t.hour,
				alarmTime->t.minute, alarmTime->t.second, timeSuffix,
				alarmTime->t.day, alarmTime->t.month, alarmTime->t.year);
	else
		PRINTF("%02d:%02d:%02d %02d-%02d-%04d \r\n", alarmTime->t.hour,
				alarmTime->t.minute, alarmTime->t.second, alarmTime->t.day,
				alarmTime->t.month, alarmTime->t.year);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_CheckTimeSetupStatus
 * Description   : This function checks to see if the passed Timeholder struct
 * is valid when loaded into the rtc, return true for success or false for fail.
 *
 *END**************************************************************************/
bool th_CheckTimeSetupStatus(TimeHolder *myTime) {
	bool OK = 1;
	if (LPO_RTC_DRV_SetDatetime(0, &(myTime->t))) {
		PRINTF("Time set successful\n\n\r");
	} else {
		PRINTF("Time set unsuccessful\n\n\r");
		OK = 0;
	}
	return OK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_SetDateTime
 * Description   : This function accepts a Timeholder pointer, 2 strings, and a
 * flag variable. The function asks the user for the time and date. depending
 * on the value of the flag variable, the inputed data is loaded into the alarm
 * or current time TimeHolder struct.
 *
 *END**************************************************************************/
void th_SetDateTime(TimeHolder *time, const char *infoTimeString,
		const char *infoDateString, int setAlarm) {
	char inputTime[8] = { 0 };
	char inputDate[10] = { 0 };
	TimeHolder tempTime;
	char dummy;

	PRINTF("%s", infoTimeString);
	th_GetTimeInfo(inputTime, ':', 8);		//sizeof(inputTime) / sizeof(char));

	PRINTF("%s", infoDateString);
	th_GetTimeInfo(inputDate, '-', 10);		//sizeof(inputDate) / sizeof(char));

	// Parse out data from user input into TimeHolder struct
	tempTime.t.hour = (inputTime[0] - 0x30) * 10 + (inputTime[1] - 0x30);
	tempTime.t.minute = (inputTime[3] - 0x30) * 10 + (inputTime[4] - 0x30);
	tempTime.t.second = (inputTime[6] - 0x30) * 10 + (inputTime[7] - 0x30);
	tempTime.t.day = (inputDate[0] - 0x30) * 10 + (inputDate[1] - 0x30);
	tempTime.t.month = (inputDate[3] - 0x30) * 10 + (inputDate[4] - 0x30);
	tempTime.t.year = ((inputDate[6] - 0x30) * 1000) + ((inputDate[7] - 0x30) * 100) + ((inputDate[8] - 0x30) * 10) + (inputDate[9] - 0x30);

	// Disable all irqs while loading the RTC
	__disable_irq();
	// Set Alarm time.
	if (setAlarm) {	// If alarm is to be set
		if (LPO_RTC_DRV_SetAlarm(0, &(tempTime.t), 1)) {	// Check if time is ok
			PRINTF("\n\n\rNew Alarm Time has been set !\n\r");
			th_SaveTime(&tempTime, time);
			RTC_DRV_SetAlarmIntCmd(0, 1);	// Set the alarm interrupt to enabled

		} else {
			PRINTF(	// Print error is something went wrong
					"\n\n\rTime is not in the correct format, time was not set !\n\r");
		}
	} else {	// If current time is to be set
		if (LPO_RTC_DRV_SetDatetime(0, &(tempTime.t))) {	// Check if time is ok
			PRINTF("\n\n\rNew Current Time has been set !\n\r");
			th_SaveTime(&tempTime, time);
		} else {
			PRINTF(	// Print error is something went wrong
					"\n\n\rTime is not in the correct format, time was not set !\n\r");
		}
	}
	__enable_irq();
	PRINTF("Please any key to continue\n\r");
	dummy = GETCHAR();	// Wait
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_GetTimeInfo
 * Description   : This function accepts user input and allows for data correction
 * with the backspace key. It accepts the input buffer,  value seperator for either
 * time or date and a Size value for for loop iteration.
 *
 *END**************************************************************************/
void th_GetTimeInfo(char *inputTime, const char valueSeperator, const int SIZE) {
	for (int i = 0; i < SIZE; i++) {
		if (i != 2 && i != 5) {
			while (*(inputTime + i) < 0x30 || *(inputTime + i) > 0x39) {	// while the buffer is not filled with all numerics
				*(inputTime + i) = GETCHAR();
				if (((*(inputTime + i) == 0x7F) || *(inputTime + i) == 0x8)	// If the input was Enter or backspace and i > 0
						&& (i > 0)) {
					i--;	// decrement the buffer pointer by 1
					if ((i == 2) || (i == 5)) {	// if the current buffer char is at location 2 or 5
						i--;		// decrement the buffer pointer by 1
						VT100_CursorBackward(1);	// go back on the screen 1 location
					}
					*(inputTime + i) = 'z';	// Assign z so not to false trigger the user menu
					VT100_CursorBackward(1); // go back on the screen 1 location
				}
			}
		} else {
			*(inputTime + i) = valueSeperator;	// If i = 2 or 5 then add the valueSeperator to the buffer
		}
		PUTCHAR(*(inputTime + i));
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_DecrementMillisecond
 * Description   : This function accepts a struct pointer and a value to
 * decrement the pointer by. Used to adjust the millisecond value within a TimeHolder
 * struct
 *
 *END**************************************************************************/
void th_DecrementMillisecond(TimeHolder *th, int decrementVal) {
	if (th->millisecond < decrementVal) {
		th->millisecond += 1000;
		th_DecrementRawSecond(th, 1);
	}
	th->millisecond -= decrementVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_IncrementMillisecond
 * Description   : This function accepts a struct pointer and a value to
 * increment the pointer by. Used to adjust the millisecond value within a TimeHolder
 * struct
 *
 *END**************************************************************************/
void th_IncrementMillisecond(TimeHolder *th, int incrementVal) {
	th->millisecond += incrementVal;
	if (th->millisecond > 999) {
		th->millisecond -= 1000;
		th_IncrementRawSecond(th, 1);
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_getNearestAlarm
 * Description   : This function accepts 3 timeHolder pointers, a SIZE variable
 * and a bool flag. This function detects the closest alarm to the current time
 * and passes the struct back to main.
 *
 *END**************************************************************************/
void th_getNearestAlarm(TimeHolder *alarmTime, TimeHolder *nearestAlarmTime,
		TimeHolder *myTime, const int SIZE, bool *alarmReady) {
	unsigned long lowestTime = 4294967295;
	long difference = 0;
	for (int i = 0; i < SIZE; i++) {
		th_GetRawSecond(alarmTime + i);
	}
	th_GetRawSecond(myTime);
	for (int i = 0; i < SIZE; i++) {
		difference = ((alarmTime + i)->rawSecond) - (myTime->rawSecond);
		if (difference > -5) {
			if (difference < lowestTime) {
				lowestTime = difference;
				th_SaveTime((alarmTime + i), nearestAlarmTime);
				*alarmReady = 1;
				PRINTF("\n\r%i -- %i\r\n ", i, difference);
			}
		}
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : th_LoadAlarm
 * Description   : This function accepts in a timeholder struct and attempts to
 * load in the struct value into the RTC alarm reg. If it is successful, the alarm
 * interrupt is enabled.
 *
 *END**************************************************************************/
void th_LoadAlarm(TimeHolder *alarmTime) {
	if (LPO_RTC_DRV_SetAlarm(0, &(alarmTime->t), 1)) {
		RTC_DRV_SetAlarmIntCmd(0, 1);
	}
}

