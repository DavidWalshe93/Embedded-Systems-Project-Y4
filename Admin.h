/*
 * Admin.h
 *
 *  Created on: 7 Dec 2016
 *      Author: David
 */

#ifndef _ADMIN_H_
#define _ADMIN_H_

// Included Libraries
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "VT100_Commands.h"
#include "rtc_lpo.h"
#include "TimeHolder.h"

// External variables
extern char char_received();

// Function Prototypes

// checks an entered user password is correct
bool adm_CheckPassword(const char *currentPassword, const int PASSWORD_LENGTH);

// Change the admin password
void adm_ChangePassword(char *currentPassword, const int PASSWORD_LENGTH);

// Set any one of 5 alarms.
bool setAlarm(TimeHolder *alarmTimes, const int SIZE, char *inputCHar);

#endif /* _ADMIN_H_ */
