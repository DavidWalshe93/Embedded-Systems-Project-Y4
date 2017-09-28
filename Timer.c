/*
 * Timer.c
 *
 *  Created on: 19 Nov 2016
 *      Author: David
 */

#include "Timer.h"
/*FUNCTION**********************************************************************
 *
 * Function Name : tmr_HandleTimer
 * Description   : This function decrements and displays the timer to the console
 * along with the clock information.
 *
 *END**************************************************************************/
void tmr_HandleTimer(char *functionChar, TimeHolder *myTime,
		TimeHolder *alarmTime) {
	bool blink = 0;
	bool pause = 0;
	TimeHolder timer;
	th_InitTimeHolder(&timer);
	VT100_terminalClearFormat(g_terminalColour);
	tmr_LoadTimer(&timer);
	*functionChar = 'z';
	while (*functionChar != '0') {
		if (g_updateTime) {
			blink = !blink;
			if (pause == 0) {
				// Map the timer values to rawSeconds and decrement by 1 and remap.
				th_GetRawSecond(&timer);
				th_DecrementRawSecond(&timer, 1);
				th_MapSecondsToRtcTime(&timer);
			}
			// Show Code Printout
			VT100_terminalClearFormat(g_terminalColour);
			LPO_RTC_DRV_GetDatetime(0, &(myTime->t));
			th_PrintTimeAlarm(myTime, alarmTime, g_clockMode);
			PRINTF("\n\r\n\rTimer: %02d:%02d:%02d", timer.t.hour,
					timer.t.minute, timer.t.second);
			if (pause == 1 && blink == 1) {		// When paused blink a paused msg
				VT100_DisplayBWhite();
				VT100_DisplayFBlack();
				PRINTF("\n\r PAUSED ");
				VT100_terminalFormat(g_terminalColour);
			}
			if (timer.rawSecond == 0 && blink == 1) {	// When done blink a finished msg
				VT100_DisplayBYellow();
				VT100_DisplayFBlue();
				PRINTF("\n\r TIMER HAS COMPLETE ");
				VT100_terminalFormat(g_terminalColour);
				PRINTF(
						"\n\rOptions: \n\r- 1 -- Pause/Run Timer\n\r- 0 -- Exit");
			} else {
				PRINTF(
						"\n\n\rOptions: \n\r- 1 -- Pause/Run Timer\n\r- 0 -- Exit");
			}
			g_updateTime = 0;
			if (char_received()) {
				*functionChar = GETCHAR();
				if (*functionChar == '1') {
					pause = !pause;
				}
			}
		}
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : tmr_LoadTimer
 * Description   : This function asks a user to enter in the desired timer value.
 * The value is then loaded into the TimeHolder timer Struct.
 *
 *END**************************************************************************/
void tmr_LoadTimer(TimeHolder *timer) {
	bool done = 0;
	char input[8];
	if (done) {
		PRINTF("ERROR, Invalid Time Inputed\n\r");
	}
	PRINTF("Enter timer period (HH:MM:SS): ");
	for (int i = 0; i < 8; i++) {
		if (i != 2 && i != 5) {
			while (input[i] < 0x30 || input[i] > 0x39) {
				input[i] = GETCHAR();
				if (((input[i] == 0x7F) || input[i] == 0x8) && (i > 0)) {
					i--;
					if ((i == 2) || (i == 5)) {
						i--;
						VT100_CursorBackward(1);
					}
					input[i] = 'z';
					VT100_CursorBackward(1);
				}
			}
		} else {
			input[i] = ':';
		}
		PUTCHAR(input[i]);
		done = 1;
	}
	timer->t.hour = (input[0] - 0x30) * 10 + (input[1] - 0x30);
	timer->t.minute = (input[3] - 0x30) * 10 + (input[4] - 0x30);
	timer->t.second = (input[6] - 0x30) * 10 + (input[7] - 0x30);
}
