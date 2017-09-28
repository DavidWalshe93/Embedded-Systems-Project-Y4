/*
 * Calender.c
 *
 *  Created on: 25 Nov 2016
 *      Author: David
 */

#include "Calender.h"

/*FUNCTION**********************************************************************
 *
 * Function Name : cal_HandleCalender
 * Description   : This function manages and creates a calender display on the
 * console. The days of the week are synced to their correct date for any given
 * year.
 *
 *END**************************************************************************/

void cal_HandleCalender(char *inputChar, TimeHolder *myTime,
		TimeHolder *alarmTime) {
	TimeHolder calenderTime;
	char monthNames[13][20] = { "January", "February", "March", "April", "May",
			"June", "July", "August", "September", "October", "November",
			"December" };
	th_SaveTime(myTime, &calenderTime);
	while (*inputChar != '0') {	// Control for moving forward in time, month by month
		if (*inputChar == '2') {
			calenderTime.t.month++;
			if (calenderTime.t.month > 12) {
				calenderTime.t.month = 1;
				calenderTime.t.year++;
				if (calenderTime.t.year > 2099) {
					calenderTime.t.year = 2099;
				}
			}
		} else if (*inputChar == '1') {	// Control for moving back in time, month by month
			calenderTime.t.month--;
			if (calenderTime.t.month < 1) {
				calenderTime.t.month = 12;
				calenderTime.t.year--;
				if (calenderTime.t.year < 1970) {
					calenderTime.t.year = 1970;
				}
			}
		}
		th_GetWeekday(&calenderTime);		// Get the current weekday
		VT100_terminalClearFormat(0);
		// Display formated calender design on the terminal.
		PRINTF("\n\n\r%i\n\r%s\n\n\rM\tT\tW\tT\tF\tS\tS \n\n\r",
				calenderTime.t.year, monthNames[calenderTime.t.month - 1]);
		for (int i = 0; i < calenderTime.weekday; i++) {
			PRINTF("\t");
		}
		for (int i = 1; i < 32; i++) {
			calenderTime.t.day = i;
			// Check for the correct time format
			if (RTC_HAL_IsDatetimeCorrectFormat(&(calenderTime.t))) {
				// Highlight days off such as weekends, bank holidays, special holidays.
				if ((calenderTime.weekday + i) % 7 == 6
						|| (calenderTime.weekday + i) % 7 == 0) {
					VT100_DisplayFRed();
				} else if (cal_CheckFixedHoliday(&calenderTime)) {
					VT100_DisplayFRed();
				} else if (cal_CheckUnfixedHoliday(&calenderTime,
						((calenderTime.weekday + i) % 7))) {
					VT100_DisplayFRed();
				} else {
					VT100_DisplayFWhite();
				}

				if (cal_CheckCurrentDay(&calenderTime, myTime)) {
					VT100_DisplayBGreen();
					VT100_DisplayBWhite();
				} else if (cal_CheckAlarmDay(&calenderTime, alarmTime)) {
					VT100_DisplayBYellow();
					VT100_DisplayFBlack();
				}
				PRINTF("%02d\t", calenderTime.t.day);
				VT100_DisplayBBlack();
				if ((calenderTime.weekday + i) % 7 == 0) {
					PRINTF("\n\r");
				}
			}
		}
		// Display controls.
		VT100_DisplayFGreen();
		PRINTF("\n\n\r   <---(1\t   0 to Exit  \t\t2)--->");
		*inputChar = GETCHAR();
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : cal_CheckAlarmDay
 * Description   : This function highlights all the alarm days on the calender.
 *
 *END**************************************************************************/
bool cal_CheckAlarmDay(TimeHolder *calenderTime, TimeHolder *alarmTime) {
	bool returnValue = 0;
	if ((calenderTime->t.year == alarmTime->t.year)
			&& (calenderTime->t.month == alarmTime->t.month)
			&& (calenderTime->t.day == alarmTime->t.day)) {
		returnValue = 1;
	}
	return returnValue;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : cal_CheckCurrentDay
 * Description   : This function highlights the current day on the calender.
 *
 *END**************************************************************************/
bool cal_CheckCurrentDay(TimeHolder *calenderTime, TimeHolder *currentTime) {
	bool returnValue = 0;
	if ((calenderTime->t.year == currentTime->t.year)
			&& (calenderTime->t.month == currentTime->t.month)
			&& (calenderTime->t.day == currentTime->t.day)) {
		returnValue = 1;
	}
	return returnValue;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : cal_CheckUnfixedHoliday
 * Description   : This function highlights all the unfixed, floating holidays
 * days on the calender such as bank holidays.
 *
 *END**************************************************************************/
bool cal_CheckUnfixedHoliday(TimeHolder *calenderTime, int weekday) {
	const int UNFIXED_HOLIDAY_MONTH[] = { 5, 6, 8, 11 };
	bool returnValue = 0;
	if (calenderTime->t.month == 10) {
		if (calenderTime->t.day == 31) {
			if (weekday == 1) {
				returnValue = 1;
			}
		}
	}
	for (int i = 0; i < 4; i++) {

		if (calenderTime->t.month == UNFIXED_HOLIDAY_MONTH[i]) {
			if (calenderTime->t.day <= 7) {
				if (weekday == 1) {
					returnValue = 1;
				}
			}
		}
	}
	return returnValue;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : cal_CheckFixedHoliday
 * Description   : This function highlights all the fixed, static holidays
 * days on the calender such as christmas Day and St.Patricks Day..
 *
 *END**************************************************************************/
bool cal_CheckFixedHoliday(TimeHolder *calenderTime) {
	const int FIXED_HOLIDAY_MONTH[] = { 1, 3, 12, 12 };
	const int FIXED_HOLIDAY_DAY[] = { 1, 17, 25, 26 };
	bool returnValue = 0;
	for (int i = 0; i < 4; i++) {
		if (calenderTime->t.month == FIXED_HOLIDAY_MONTH[i]) {
			if (calenderTime->t.day == FIXED_HOLIDAY_DAY[i])
				returnValue = 1;
		}
	}
	return returnValue;
}

