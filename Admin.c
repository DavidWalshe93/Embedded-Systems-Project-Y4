/*
 * Admin.c
 *
 *  Created on: 7 Dec 2016
 *      Author: David
 */

#include "Admin.h"

bool adm_CheckPassword(const char *currentPassword, const int PASSWORD_LENGTH) {
	char passwordEntryBuffer[PASSWORD_LENGTH];
	bool passwordOK = 1;
	PRINTF("ADMIN PASSWORD (# to exit): ");
	int i = 0;
	while (1) {
		passwordEntryBuffer[i] = GETCHAR();
		if (passwordEntryBuffer[i] == '#') {
			return 0;
		}
		if ((passwordEntryBuffer[i] == 0x7F) && (i > 0)) {

			VT100_CursorBackward(1);
			PUTCHAR(' ');
			VT100_CursorBackward(1);
			i--;
		} else if (passwordEntryBuffer[i] != 0x7F
				&& passwordEntryBuffer[i] != 0xD) {
			PUTCHAR('*');
			i++;
		}
		if (passwordEntryBuffer[i] == 0xD) {
			while (i < PASSWORD_LENGTH) {
				passwordEntryBuffer[i] = 0x0;
				i++;
			}
			break;
		}

	}
	for (int i = 0; i < PASSWORD_LENGTH; i++) {
		if (passwordEntryBuffer[i] != *(currentPassword + i)) {
			passwordOK = 0;
			break;
		}
	}
	return passwordOK;
}

void adm_ChangePassword(char *currentPassword, const int PASSWORD_LENGTH) {

	char passwordEntry[2][PASSWORD_LENGTH];
	bool passwordsOK = 1;
	int i = 0;
	PRINTF("\n\rEnter New Password: ");
	while (1) {
		passwordEntry[0][i] = GETCHAR();
		if ((passwordEntry[0][i] == 0x7F) && (i > 0)) {
			VT100_CursorBackward(1);
			PUTCHAR(' ');
			VT100_CursorBackward(1);
			i--;
		} else if (passwordEntry[0][i] != 0x7F && passwordEntry[0][i] != 0xD) {
			PUTCHAR('*');
			i++;
		}
		if (passwordEntry[0][i] == 0xD) {
			while (i < PASSWORD_LENGTH) {
				passwordEntry[0][i] = 0x0;
				i++;
			}
			break;
		}
	}
	i = 0;
	PRINTF("\n\rRe-Enter New Password: ");
	while (1) {
		passwordEntry[1][i] = GETCHAR();
		if ((passwordEntry[1][i] == 0x7F) && (i > 0)) {
			VT100_CursorBackward(1);
			PUTCHAR(' ');
			VT100_CursorBackward(1);
			i--;
		} else if (passwordEntry[1][i] != 0x7F && passwordEntry[1][i] != 0xD) {
			PUTCHAR('*');
			i++;
		}
		if (passwordEntry[1][i] == 0xD) {
			while (i < PASSWORD_LENGTH) {
				passwordEntry[1][i] = 0x0;
				i++;
			}
			break;
		}
	}
	for (int i = 0; i < PASSWORD_LENGTH - 1; i++) {
		//PRINTF("\n\r1 - %c   2 - %c\n\r", passwordEntry[0][i], passwordEntry[0][i]);
		if (passwordEntry[0][i] != passwordEntry[1][i]) {
			PRINTF(
					"\n\rPasswords entered do not match ! \n\r PASSWORD WAS NOT CHANGED !\n\r");
			passwordsOK = 0;
			GETCHAR();
			break;
		}
	}
	if (passwordsOK) {
		for (int i = 0; i < PASSWORD_LENGTH; i++) {
			*(currentPassword + i) = passwordEntry[0][i];
		}
		PRINTF("PASSWORD WAS CHANGED ! \n\r");
	}
}

bool setAlarm(TimeHolder *alarmTime, const int SIZE, char *inputChar) {
	const char alarmTimeString[] =
			"\n\n\rPlease Enter the alarm time (HH:MM:SS): ";
	const char alarmDateString[] =
			"\n\n\rPlease Enter the current time (DD:MM:YYYY): ";

	*inputChar = 'z';
	PRINTF(
			"\n\n\rSelect Alarm To Set: \n\n\r- 1 -- Alarm 1 -- %02d:%02d:%02d  %02d-%02d-%02d\n\r- 2 -- Alarm 2 -- %02d:%02d:%02d  %02d-%02d-%02d\n\r- 3 -- Alarm 3 -- %02d:%02d:%02d  %02d-%02d-%02d\n\r- 4 -- Alarm 4 -- %02d:%02d:%02d  %02d-%02d-%02d\n\r- 5 -- Alarm 5 -- %02d:%02d:%02d  %02d-%02d-%02d",
			(alarmTime + 0)->t.hour, (alarmTime + 0)->t.minute,
			(alarmTime + 0)->t.second, (alarmTime + 0)->t.day,
			(alarmTime + 0)->t.month, (alarmTime + 0)->t.year,
			(alarmTime + 1)->t.hour, (alarmTime + 1)->t.minute,
			(alarmTime + 1)->t.second, (alarmTime + 1)->t.day,
			(alarmTime + 1)->t.month, (alarmTime + 1)->t.year,
			(alarmTime + 2)->t.hour, (alarmTime + 2)->t.minute,
			(alarmTime + 2)->t.second, (alarmTime + 2)->t.day,
			(alarmTime + 2)->t.month, (alarmTime + 2)->t.year,
			(alarmTime + 3)->t.hour, (alarmTime + 3)->t.minute,
			(alarmTime + 3)->t.second, (alarmTime + 3)->t.day,
			(alarmTime + 3)->t.month, (alarmTime + 3)->t.year,
			(alarmTime + 4)->t.hour, (alarmTime + 4)->t.minute,
			(alarmTime + 4)->t.second, (alarmTime + 4)->t.day,
			(alarmTime + 4)->t.month, (alarmTime + 4)->t.year);
	if (char_received()) {
		*inputChar = GETCHAR();
		switch (*inputChar) {
		case '1':
			PRINTF("\n\n\rALARM 1");
			do {
				th_SetDateTime(alarmTime, alarmTimeString, alarmDateString, 1);
			} while (th_CheckTimeSetupStatus(alarmTime) == 0);
			return 1;
			break;
		case '2':
			PRINTF("\n\n\rALARM 2");
			do {
				th_SetDateTime((alarmTime + 1), alarmTimeString,
						alarmDateString, 1);
			} while (th_CheckTimeSetupStatus(alarmTime + 1) == 0);
			return 1;
			break;
		case '3':
			PRINTF("\n\n\rALARM 3");
			do {
				th_SetDateTime((alarmTime + 2), alarmTimeString,
						alarmDateString, 1);
			} while (th_CheckTimeSetupStatus(alarmTime + 2) == 0);
			return 1;
			break;
		case '4':
			PRINTF("\n\n\rALARM 4");
			do {
				th_SetDateTime((alarmTime + 3), alarmTimeString,
						alarmDateString, 1);
			} while (th_CheckTimeSetupStatus(alarmTime + 3) == 0);
			return 1;
			break;
		case '5':
			PRINTF("\n\n\rALARM 5");
			do {
				th_SetDateTime((alarmTime + 4), alarmTimeString,
						alarmDateString, 1);
			} while (th_CheckTimeSetupStatus(alarmTime + 4) == 0);
			return 1;
			break;
		default:
			return 0;
		}
	}
	return 0;
}
