/*
 * StopWatch.c
 *
 *  Created on: 19 Nov 2016
 *      Author: David
 */

#include "StopWatch.h"

/*FUNCTION**********************************************************************
 *
 * Function Name : sw_HandleStopWatch
 * Description   : This function is used to produce stop watch functionality from
 * the main loop.
 *
 * It uses 3 Timeholder structs to keep track of the current stopwatch time and
 * 2 lap times.
 *
 * The function makes use of a PIT timer running in the background, to get timing
 * accuracy to 1 millisecond.
 *
 * The console is refreshed every 100mS to prevent the console form being overloaded
 *
 *END**************************************************************************/

void sw_HandleStopWatch(char *inputChar) {
	// Init Timeholder structs for running time and lap times
	TimeHolder currentStopWatchTime;
	TimeHolder lapTime[3];
	TimeHolder temp[3];

	// Init the timeholders to start of epoch (00:00:00 01-01-1997)
	th_InitTimeHolder(&currentStopWatchTime);
	th_InitTimeHolder(lapTime);
	th_InitTimeHolder((lapTime + 1));
	th_InitTimeHolder((lapTime + 2));

	th_InitTimeHolder(temp);
	th_InitTimeHolder((temp + 1));
	th_InitTimeHolder((temp + 2));

	*inputChar = 'z';		// stop input buffer from falsely selecting control

	sw_PrintStopwatchTime(&currentStopWatchTime, lapTime, temp);
	while (*inputChar != '0')			// While user has not hit exit key
	{
		if (char_received())// Check if a char is received form the console - Non Blocking
		{
			*inputChar = GETCHAR();		// Read the inputted char
		}
		if (*inputChar == '1') {
			sw_RunStopWatch(inputChar, &currentStopWatchTime, lapTime, temp);
			PIT_DRV_StopTimer(0, 0);
		}
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : sw_RunStopWatch
 * Description   : This function increments the stopwatch timer
 *
 * It also allows for lap times to be recorded with both total time taken and
 * time sense last lap displayed. Up to 3 laps can be displayed at a time.
 *
 *END**************************************************************************/
void sw_RunStopWatch(char *inputChar, TimeHolder *currentStopWatchTime,
		TimeHolder *lapTime, TimeHolder *temp) {
	*inputChar = 'z';
	PIT_DRV_StartTimer(0, 0);
	while (*inputChar != '2' && *inputChar != '0') {
		if (char_received()) {
			*inputChar = GETCHAR();
			if (*inputChar == '3') {		// Lap record
				// Knockon effect of time Saves.
				th_SaveTime((lapTime + 1), (lapTime + 2));
				th_SaveTime((temp + 1), (temp + 2));
				th_SaveTime((lapTime), (lapTime + 1));
				th_SaveTime((temp), (temp + 1));
				th_SaveTime(currentStopWatchTime, (temp));
				th_GetRawSecond((lapTime));
				th_DecrementRawSecond((temp), lapTime->rawSecond);
				th_DecrementMillisecond((temp), (lapTime)->millisecond);
				th_SaveTime(currentStopWatchTime, (lapTime));// Save the current time to the first laptime
			}
		}
		if (g_updateTime)			// second interrupt has occurred
		{
			g_updateTime = 0;		// reset flag
			__disable_irq();		// disable interrupts
			g_millis = 0;			// reset millis
			__enable_irq();			// enable interrupts
			th_IncrementRawSecond(currentStopWatchTime, 1);	// Increment the time by 1 second
		}
		// Prints the current time in 12/24hr format to the console
		currentStopWatchTime->millisecond = g_millis;// Save the current millis from the PIT timer
		if (g_millis % 100 == 0)// every 100mS refresh the stopwatch on the console
				{
			sw_PrintStopwatchTime(currentStopWatchTime, lapTime, temp);
			if (g_millis > 999)	// Stop the millis from going over 3 decimels for the console print out
					{
				__disable_irq();	// Disable interrupts
				g_millis = 0;		// Reset millis value
				__enable_irq();		// Enable interrupts
			}
		}
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : sw_PrintStopwatchTime
 * Description   : This function displays a formatted view of the stopwatch
 * time as well as all the lap times.
 *
 *END**************************************************************************/
void sw_PrintStopwatchTime(TimeHolder *currentStopWatchTime,
		TimeHolder *lapTime, TimeHolder *temp) {
	// Console print out to user
	VT100_terminalClearFormat(g_terminalColour);
	PRINTF("Stop watch: %02d:%02d:%03d\n\r", currentStopWatchTime->t.minute,
			currentStopWatchTime->t.second, currentStopWatchTime->millisecond);
	PRINTF("Total Lap Time\t\t\tSince Last Lap Time\n\r");
	PRINTF("Lap 1: %02d:%02d:%03d\t\t%02d:%02d%:%03d\n\r", lapTime->t.minute,
			lapTime->t.second, lapTime->millisecond, temp->t.minute,
			temp->t.second, temp->millisecond);
	PRINTF("Lap 2: %02d:%02d:%03d\t\t%02d:%02d%:%03d\n\r",
			(lapTime + 1)->t.minute, (lapTime + 1)->t.second,
			(lapTime + 1)->millisecond, (temp + 1)->t.minute,
			(temp + 1)->t.second, (temp + 1)->millisecond);
	PRINTF("Lap 3: %02d:%02d:%03d\t\t%02d:%02d%:%03d\n\r",
			(lapTime + 2)->t.minute, (lapTime + 2)->t.second,
			(lapTime + 2)->millisecond, (temp + 2)->t.minute,
			(temp + 2)->t.second, (temp + 2)->millisecond);
	PRINTF(
			"\n\n\rOptions: \n\r- 1 -- Run\n\r- 2 -- Stop\n\r- 3 -- Record Lap Time\n\r- 0 -- Exit");
}
