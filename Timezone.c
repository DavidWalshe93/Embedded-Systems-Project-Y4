/*
 * Timezone.c
 *
 *  Created on: 22 Nov 2016
 *      Author: David
 */

#include "Timezone.h"

/*FUNCTION**********************************************************************
 *
 * Function Name : tz_HandleTimeZone
 * Description   : This function handles the timezone display.
 *
 * It assigns 24 different timezone times to 24 TimeHolder structs. The value of
 * the structs is then used to create a table view of the GMT times from around
 * the world.
 *
 *END**************************************************************************/
void tz_HandleTimezone(char *inputChar, TimeHolder *time) {
	const signed int SEC_IN_HOUR = 3600;
	const char CITY_NAMES[24][30] = { "\tAuckland\t\tGMT +12",
			"\tNew Caledonia\t\tGMT +11", "\tMelbourne\t\tGMT +10",
			"\tTokyo\t\t\tGMT +9", "\tHong Kong\t\tGMT +8",
			"\tBangkok\t\t\tGMT +7", "\tSri Lanka\t\tGMT +6",
			"\tNew Dehli\t\tGMT +5", "\tAbu Dhabi\t\tGMT +4",
			"\tMoscow\t\t\tGMT +3", "\tAthens\t\t\tGMT +2",
			"\tBerlin\t\t\tGMT +1", "\tDublin\t\t\tGMT +0",
			"\tCape Verde\t\tGMT -1", "\tMid-Atlantic\t\tGMT -2",
			"\tRio de Janero\t\tGMT -3", "\tSantiago\t\tGMT -4",
			"\tNew York\t\tGMT -5", "\tChicago\t\t\tGMT -6",
			"\tPhoenix\t\t\tGMT -7", "\tLos Angeles\t\tGMT -8",
			"\tJuneau\t\t\tGMT -9", "\tHonolulu\t\tGMT -10",
			"\tMidway Island\t\tGMT -11" };
	TimeHolder timezones[24];
	*inputChar = 'z';
	while (*inputChar != '0') {
		if (g_updateTime) {
			VT100_terminalClearFormat(g_terminalColour);
			LPO_RTC_DRV_GetDatetime(0, &(time->t));
			for (int i = -12; i < 12; i++) {
				th_SaveTimeOffset(time, &timezones[i + 12], SEC_IN_HOUR * i);
				tz_PrintGmtTime(&timezones[i + 12], &CITY_NAMES[i + 12][0], 0);
			}
			if (char_received()) {
				*inputChar = GETCHAR();
			}
			g_updateTime = 0;
		}
	}

}

/*FUNCTION**********************************************************************
 *
 * Function Name : tz_PrintGmtTime
 * Description   : This function displays the timezones in a table of information
 * on to the console.
 *
 *END**************************************************************************/
void tz_PrintGmtTime(TimeHolder *myTime, const char *locationStr,
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

	if ((clockMode) && (myTime->t.hour > 12))
		PRINTF("%02d:%02d:%02d %cM\t %02d-%02d-%04d %s\r\n",
				myTime->t.hour - 12, myTime->t.minute, myTime->t.second,
				timeSuffix, myTime->t.day, myTime->t.month, myTime->t.year,
				locationStr);
	else if ((clockMode) && (myTime->t.hour == 0))
		PRINTF("%02d:%02d:%02d %cM\t %02d-%02d-%04d %s\r\n",
				myTime->t.hour + 12, myTime->t.minute, myTime->t.second,
				timeSuffix, myTime->t.day, myTime->t.month, myTime->t.year,
				locationStr);
	else if (clockMode)
		PRINTF("%02d:%02d:%02d %cM\t %02d-%02d-%04d %s\r\n", myTime->t.hour,
				myTime->t.minute, myTime->t.second, timeSuffix, myTime->t.day,
				myTime->t.month, myTime->t.year, locationStr);
	else
		PRINTF("%02d:%02d:%02d \t %02d-%02d-%04d %s\r\n", myTime->t.hour,
				myTime->t.minute, myTime->t.second, myTime->t.day,
				myTime->t.month, myTime->t.year, locationStr);

}

