/*
 * Lab12a.c
 *
 *  Created on: 03 Nov 2016
 *      Author: David Walshe - G00313010
 *      Description: This code creates a clock simulator by making use of the onboard RTC peripheral on the KL26Z
 *
 *      This allows accurate and low efficient time keeping without the need of PIT timers or using CPU resources.
 *      The RTC peripheral keeps track of the current time
 *
 *      The code updates the screen every second for the user to see by reading the current time from the RTC clock.
 *      The RTC peripheral is loaded with 00:00:00 00-00-00 by default
 *
 *      If a user hits a key, then they may enter their own defined time. The entered time is passed as a string and
 *      parsed into the hours minutes and seconds. The string is parsed for error checking to make sure the char numbers
 *      in the array are usable time entry. if they are the time is set to the user defined time. If the user defined time
 *      entered is not usable then an error msg is printed and the clock continues from its original time.
 *	
 *	The code also includes several features such as stopwatch, timer, auto-reloaded alarms, interrupt driven alarm  *`	sequence, GMT Time Zone, admin mode, password checking and calender functionality.  
 *
 */

// Libraries
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_pit_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_tsi_driver.h"
#include "VT100_Commands.h"
#include "rtc_lpo.h"
#include "StopWatch.h"
#include "Timer.h"
#include "Timezone.h"
#include "Calender.h"
#include "TimeHolder.h"
#include "Admin.h"

// Constants
#define TSI_THRESHOLD_SAMPLING      (100u)
#define TSI_INSTANCE                0

// Function Prototypes
void terminalFormat();
char char_received();
extern void TSI_DRV_IRQHandler(uint32_t instance);//TSI interrupt handler in SDK
void getTerminalColor();
int tsi_config(uint32_t * avg_TSI_untouch);			//Configures TSI module
int tsi_measure(uint32_t * left, uint32_t * right);	//Takes a measurement fromthe TSI module

// Global Variables
volatile int g_updateTime = 0;
volatile int g_alarmCompleteCounter = 0;
volatile int g_millis = 0;
bool g_clockMode = 0;
int g_terminalColour = 0;
uint8_t g_tsiChn[BOARD_TSI_ELECTRODE_CNT];	//array of TSI channels
tsi_state_t g_myTsiState;						//driver data storage

/*Interrupt Service Routines
 /**********************************************************************************************/

//TSI Interrupt Handler
void TSI0_IRQHandler(void) {
	TSI_DRV_IRQHandler(0);
}

//RTC Second Interrupt Handler
void RTC_Seconds_IRQHandler() {
	RTC_prescale();	// Adjust timer prescale Reg to get a 1 second output from the RTC
	g_updateTime = 1;	// Set for use in main
}

// RTC Alarm Interrupt Handler
void RTC_IRQHandler() {
	RTC_DRV_SetAlarmIntCmd(0, 0);	// Turn off the Alarm IRQ
	PIT_DRV_StartTimer(0, 1);		// Start Timer 1 for alarm sequence
}

// PIT Dual Channel Interrupt handler
void PIT_IRQHandler() {
	if (PIT_DRV_IsIntPending(0, 0)) {	// PIT 0 used for stopwatch milliseconds
		PIT_DRV_ClearIntFlag(0, 0);			// Clear the timer interrupt flag
		g_millis++;	// increment the global value for use in the stopwatch routine
	} else if (PIT_DRV_IsIntPending(0, 1)) {// PIT 1 used for Alarm flashing sequence
		PIT_DRV_ClearIntFlag(0, 1);				// Clear the PIT 1 ISR flag
		GPIO_DRV_TogglePinOutput(RedLED);// Toggle the RED LED to simulate an alarm sequence
		if (++g_alarmCompleteCounter >= 120) {// After 30 seconds stop the timer sequence.
			g_alarmCompleteCounter = 0;	// Reset the global counter for next alarm sequence
			PIT_DRV_StopTimer(0, 1);			// Stop PIT 1.
			GPIO_DRV_SetPinOutput(RedLED);// Turn the LED off once alarm sequence is complete.
		}
	}
}

// GPIO Port C & D Interrupt Handler
void PORTC_PORTD_IRQHandler() {
	GPIO_DRV_ClearPinIntFlag(SW1);// Clear interrupt triggered flag to prevent re-occuring ISR calls from the same Event
	GPIO_DRV_SetPinOutput(RedLED);	// Set the red LED off
	PIT_DRV_StopTimer(0, 1);		// Stop PIT 1
}

/* Main Loop
 /**********************************************************************************************/

int main(void) {
	// Const Strings for menu displays
	const char currentTimeString[] =
			"\n\n\rPlease Enter the current time (HH:MM:SS): ";
	const char currentDateString[] =
			"\n\n\rPlease Enter the current date (DD:MM:YYYY): ";
	const char mainMenuStr[] =
			"\n\nMain Menu\n\rOptions:\n\r- 1 -- Clock mode (24/12 Hr)\r\n- 2 -- Peripherals\r\n- 3 -- Time Zone Config\r\n- 4 -- Admin Settings\r\n- 5 -- Calender\n\r";
	const char peripheralMenuStr[] =
			"\n\n\rPeripheral Menu\n\rOptions: \n\r- 1 -- Stopwatch\r\n- 2 -- Timer\r\n- 0 -- Exit";
	const char adminMenuStr[] =
			"\n\n\rAdministration Menu\n\rOptions: \n\r- 1 -- Adjust Current Time\r\n- 2 -- Set Alarm\r\n- 3 -- Change Password \r\n- 0 -- Exit";

	// local variables
	char functionChar = 'z';
	int alarmDuration = 0;
	int done = 0;
	char currentPassword[30] = "abc123";
	int passwordLength = sizeof(currentPassword) / sizeof(char);
	bool adminMode = 0;
	bool OK = 0;
	unsigned long int avgUntouch = 0;
	bool alarmReady = 0;

	// FSM State declaration
	enum STATES {
		INIT,
		CLOCKMODE,
		PERIPHERALS,
		TIMEZONE,
		ADMIN,
		TIMER,
		STOPWATCH,
		CALENDER,
		SETDATETIME,
		SETALARMTIME,
		CHANGEPASSWORD
	};
	enum STATES currentState = INIT;

	//Initialise the FRDM-KL26Z Board
	hardware_init();
	// Initialize the OS abstraction layer
	OSA_Init();

	// Enable Interrupts.
	__enable_irq();

	// Configuration structs for PIT Timers
	pit_user_config_t pitChannel0 = { .isInterruptEnabled = true, .periodUs =
			1000U };

	pit_user_config_t pitChannel1 = { .isInterruptEnabled = true, .periodUs =
			250000U };

	// Initialise PIT instance 0, timer 0.
	PIT_DRV_Init(0, 0);
	PIT_DRV_InitChannel(0, 0, &pitChannel0);
	PIT_DRV_ClearIntFlag(0, 0);

	// Initialise PIT instance 0, timer 1.
	PIT_DRV_Init(0, 1);
	PIT_DRV_InitChannel(0, 1, &pitChannel1);
	PIT_DRV_ClearIntFlag(0, 1);

	//Initialise GPIO pins
	GPIO_DRV_Init(switchPins, ledPins);
	//Initialise RTC peripheral
	RTC_DRV_Init(0);
	//Initialise TSI peripheral
	tsi_config(&avgUntouch);

	// Create timeholder structs for RTC application
	TimeHolder myTime;
	TimeHolder nearestAlarm;
	TimeHolder alarmTime[5];

	// Init all Timeholders to 00:00:00 01-01-1970 ( epoch )
	for (int i = 0; i < 5; i++) {
		th_InitTimeHolder(&alarmTime[i]);
	}
	th_InitTimeHolder(&nearestAlarm);
	th_InitTimeHolder(&myTime);

	// Ask the user for the current time on startup.
	do {
		th_SetDateTime(&myTime, currentTimeString, currentDateString, 0);
		th_SaveTimeOffset(&myTime, &alarmTime[0], 30);// Assign a default alarm value of 30 seconds from start time // For Fast debugging
	} while (th_CheckTimeSetupStatus(&myTime) == 0);// If the time the user entered is invalid then ask them to re-enter

	VT100_terminalClearFormat(g_terminalColour);

	RTC_DRV_SetSecsIntCmd(0, 1);	// Enable the seconds interrupt

	while (1) {
		if (g_updateTime) {
			g_updateTime = 0;
			LPO_RTC_DRV_GetDatetime(0, &(myTime.t));	// Get the current RTC time
			th_InitTimeHolder(&nearestAlarm);			// Init the InitTimeHolder to epoch, used to detect bugs when auto loading an alarm value
			th_getNearestAlarm(alarmTime, &nearestAlarm, &myTime, 5,
					&alarmReady);	// Get the nearest alarm time to the current time.
			if (alarmReady) {	// If there is a valid alarm value to load
				th_LoadAlarm(&nearestAlarm);	// Load the alarm register and enable the alarm interrupt.
				alarmReady = 0;
			}
			LPO_RTC_DRV_GetAlarm(0, &(nearestAlarm.t));		// Get the current Alarm time.
			VT100_terminalClearFormat(g_terminalColour);	// Refresh terminal
			th_PrintTimeAlarm(&myTime, &nearestAlarm, g_clockMode);		// Print the Time and alarm info to the console
			getTerminalColor();	// Check for user input on the TSI touch pad
			switch (currentState) {
			case INIT:
				PRINTF("%s", mainMenuStr);
				if (char_received()) {
					functionChar = GETCHAR();
					switch (functionChar) {
					case '1':
						currentState = CLOCKMODE;
						PRINTF("\n\n\rCLOCKMODE");
						break;
					case '2':
						currentState = PERIPHERALS;
						PRINTF("\n\n\rPERIPHERALS");
						break;
					case '3':
						currentState = TIMEZONE;
						PRINTF("\n\n\rTIMEZONE");
						break;
					case '4':
						currentState = ADMIN;
						PRINTF("\n\n\rADMIN");
						break;
					case '5':
						currentState = CALENDER;
						PRINTF("\n\n\rCALENDER");
						break;
					case '6':
						currentState = SETDATETIME;
						PRINTF("SET DATE TIME");
						break;
					default:
						PRINTF("Command not recognised");
						PRINTF("\n\n\rDEFAULT");
						currentState = INIT;
						break;
					}
				}
				break;
			case CLOCKMODE:
				PRINTF("%s", mainMenuStr);
				g_clockMode = !g_clockMode;
				g_updateTime = 1;	// Get instant refresh on the console.
				currentState = INIT;
				break;
			case PERIPHERALS:
				PRINTF("%s", peripheralMenuStr);
				if (char_received()) {
					functionChar = GETCHAR();
					switch (functionChar) {
					case '1':
						currentState = STOPWATCH;
						PRINTF("\n\n\rSTOPWATCH");
						break;
					case '2':
						currentState = TIMER;
						PRINTF("\n\n\rTIMER");
						break;
					case '0':
						currentState = INIT;
						PRINTF("\n\n\rINIT");
						break;
					default:
						PRINTF("Command not recognised");
						PRINTF("\n\n\rDEFAULT");
						currentState = PERIPHERALS;
						break;
					}
				}
				break;
			case STOPWATCH:
				sw_HandleStopWatch(&functionChar);
				if (functionChar == '0') {
					currentState = PERIPHERALS;
					PIT_DRV_StopTimer(0, 0);
				}
				break;
			case TIMER:
				tmr_HandleTimer(&functionChar, &myTime, &alarmTime[0]);
				if (functionChar == '0') {
					currentState = PERIPHERALS;
				}
				break;
			case TIMEZONE:
				tz_HandleTimezone(&functionChar, &myTime);
				functionChar = 'z';
				currentState = INIT;
				break;
			case CALENDER:
				cal_HandleCalender(&functionChar, &myTime, &alarmTime[0]);
				currentState = INIT;
				break;
			case ADMIN:
				// As the user to log in to admin mode
				if (adminMode == 0) {
					if (adm_CheckPassword(currentPassword, passwordLength)) {
						adminMode = 1;
					} else {
						PRINTF("\n\rIncorrect Password\n\r");
						GETCHAR();
						VT100_terminalClearFormat(g_terminalColour);
						currentState = INIT;
					}
				}
				functionChar = 'z';
				PRINTF("%s", adminMenuStr);
				if (char_received()) {
					functionChar = GETCHAR();
					switch (functionChar) {
					case '1':
						currentState = SETDATETIME;
						PRINTF("\n\n\rSet Current Date and Time");
						break;
					case '2':
						currentState = SETALARMTIME;
						PRINTF("\n\n\rSet Alarm Date and Time");
						break;
					case '3':
						currentState = CHANGEPASSWORD;
						PRINTF("\n\n\rChange Password");
						break;
					case '0':
						adminMode = 0;
						currentState = INIT;
						PRINTF("\n\n\rINIT");
						break;
					default:
						PRINTF("Command not recognised");
						PRINTF("\n\n\rDEFAULT");
						currentState = ADMIN;
						break;
					}
				}
				break;
			case SETDATETIME:
				do {
					th_SetDateTime(&myTime, currentTimeString,
							currentDateString, 0);
				} while (th_CheckTimeSetupStatus(&myTime) == 0);
				currentState = ADMIN;
				break;
			case SETALARMTIME:
				OK = setAlarm(alarmTime, sizeof(alarmTime) / sizeof(TimeHolder),
						&functionChar);
				if (OK)
					currentState = ADMIN;
				break;
			case CHANGEPASSWORD:
				adm_ChangePassword(currentPassword, passwordLength);
				currentState = ADMIN;
				break;
			default:
				currentState = INIT;
				break;
			}
		}
	};
	/* Never leave main */
	return 0;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : char_received
 * Description   : This function checks to see if data is pending in the serial
 * RX buffer and returns true or false. This stops User I/O blocking the
 * flow of the program when no user input is presented.
 *
 *END**************************************************************************/
char char_received()// Checks for a char on the serial buffer, without blocking the normal
{							// operation of the code with a simple getChar();
	if (UART0_S1 & 0x20)
		return 1;
	else
		return 0;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : getTerminalColour
 * Description   : This function is used to detect the presence of a finger
 * on the KL26Z touch pad and depending on where the finger is detected the
 * colour of the text on the console will change between 3 different states.
 *
 *END**************************************************************************/

void getTerminalColor() {
	// TSI variables
	uint32_t avgUntouch = 0; 	//Average TSI value of untouched state
	uint32_t avgMeasure, i;
	tsi_status_t result;
	uint32_t left_electrode, right_electrode;
	int32_t diff = 0;
	tsi_measure(&left_electrode, &right_electrode);
	avgMeasure = (left_electrode + right_electrode) / 2;
	diff = left_electrode - right_electrode;

	if (diff > 200)		// Left touch
			{
		g_terminalColour = 1;
	} else if (diff < -200)	// Right touch
			{
		g_terminalColour = 2;
	} else if (diff < 200 && diff > -200 && avgMeasure > avgUntouch + 300)// Centre touch
			{
		g_terminalColour = 0;
	} else		// Untouched
	{
		g_terminalColour = g_terminalColour;
	}
}


// 3rd party functions
/*********************************************************************************
 * Return readings for the left and right electrode
 */
int tsi_measure(uint32_t * left, uint32_t * right) {
	tsi_status_t result;
	uint16_t measureResult[BOARD_TSI_ELECTRODE_CNT];
	uint32_t i, avgMeasure = 0;

	result = TSI_DRV_MeasureBlocking(TSI_INSTANCE);
	if (result != kStatus_TSI_Success) {
		PRINTF("\r\nThe measure of TSI failed.");
		return -1;
	}

	// Init average measurement.
	avgMeasure = 0;
	for (i = 0; i < BOARD_TSI_ELECTRODE_CNT; i++) {
		result = TSI_DRV_GetCounter(TSI_INSTANCE, g_tsiChn[i],
				&measureResult[i]);
		if (result != kStatus_TSI_Success) {
			PRINTF("\r\nThe read of TSI channel %d failed.", g_tsiChn[i]);
			return -1;
		}
		avgMeasure += measureResult[i];
	}
	// Calculates average measurement.
	avgMeasure /= BOARD_TSI_ELECTRODE_CNT;
	*left = measureResult[0];
	*right = measureResult[1];

	return (kStatus_TSI_Success);
}

/*************************************************************************************
 * Configure TSI module and return untouched reading
 */
int tsi_config(uint32_t * avg_TSI_untouch) {
	uint16_t measureResult[BOARD_TSI_ELECTRODE_CNT];
	tsi_status_t result;
	uint32_t i, j;
	uint32_t sumUntouch = 0;
	uint32_t avgMeasure;

#if (FSL_FEATURE_TSI_VERSION == 2)
	// Set up the HW configuration for normal mode of TSI
	static const tsi_config_t tsiHwConfig =
	{
		.ps = kTsiElecOscPrescaler_2div, /*! Prescaler */
		.extchrg = kTsiExtOscChargeCurrent_10uA, /*! Electrode charge current */
		.refchrg = kTsiRefOscChargeCurrent_10uA, /*! Reference charge current */
		.nscn = kTsiConsecutiveScansNumber_8time,/*! Number of scans. */
		.lpclks = kTsiLowPowerInterval_100ms, /*! Low power clock. */
		.amclks = kTsiActiveClkSource_BusClock, /*! Active mode clock source. */
		.ampsc = kTsiActiveModePrescaler_8div, /*! Active mode prescaler. */
		.lpscnitv = kTsiLowPowerInterval_100ms, /*! Low power scan interval. */
		.thresh = 100u, /*! High byte of threshold. */
		.thresl = 200u, /*! Low byte of threshold. */
	};
#elif (FSL_FEATURE_TSI_VERSION == 4)
	// Set up the HW configuration for normal mode of TSI
	static const tsi_config_t tsiHwConfig = { .ps = kTsiElecOscPrescaler_2div, /*! Prescaler */
	.extchrg = kTsiExtOscChargeCurrent_8uA, /*! Electrode charge current */
	.refchrg = kTsiRefOscChargeCurrent_8uA, /*! Reference charge current */
	.nscn = kTsiConsecutiveScansNumber_8time, /*! Number of scans. */
	.mode = kTsiAnalogModeSel_Capacitive, /*! TSI analog modes in a TSI instance */
	.dvolt = kTsiOscVolRails_Dv_103, .thresh = 100, /*! High byte of threshold. */
	.thresl = 200, /*! Low byte of threshold. */
	};
#endif

// Set up the configuration of initialization structure
	const tsi_user_config_t tsiConfig = {
			.config = (tsi_config_t*) &tsiHwConfig, .pCallBackFunc = NULL,
			.usrData = 0, };

	// Get TSI channel.
	g_tsiChn[0] = BOARD_TSI_ELECTRODE_1;
#if (BOARD_TSI_ELECTRODE_CNT > 1)
	g_tsiChn[1] = BOARD_TSI_ELECTRODE_2;
#endif
#if (BOARD_TSI_ELECTRODE_CNT > 2)
	g_tsiChn[2] = BOARD_TSI_ELECTRODE_3;
#endif
#if (BOARD_TSI_ELECTRODE_CNT > 3)
	g_tsiChn[3] = BOARD_TSI_ELECTRODE_4;
#endif

	// Driver initialization
	result = TSI_DRV_Init(TSI_INSTANCE, &g_myTsiState, &tsiConfig);
	if (result != kStatus_TSI_Success) {
		PRINTF("\r\nThe initialization of TSI driver failed ");
		return -1;
	}

	// Enable electrodes for normal mode
	for (i = 0; i < BOARD_TSI_ELECTRODE_CNT; i++) {
		result = TSI_DRV_EnableElectrode(TSI_INSTANCE, g_tsiChn[i], true);
		if (result != kStatus_TSI_Success) {
			PRINTF("\r\nThe initialization of TSI channel %d failed \r\n",
					g_tsiChn[i]);
			return -1;
		}
	}
	// Measures average value of untouched state.
	result = TSI_DRV_MeasureBlocking(TSI_INSTANCE);
	if (result != kStatus_TSI_Success) {
		PRINTF("\r\nThe measure of TSI failed. ");
		return -1;
	}

	// Measures average value in untouched mode.
	for (i = 0; i < TSI_THRESHOLD_SAMPLING; i++) {
		for (j = 0; j < BOARD_TSI_ELECTRODE_CNT; j++) {
			result = TSI_DRV_GetCounter(TSI_INSTANCE, g_tsiChn[j],
					&measureResult[j]);
			if (result != kStatus_TSI_Success) {
				PRINTF("\r\nThe read of TSI channel %d failed.", g_tsiChn[j]);
				return -1;
			}
			// Calculates sum of average values.
			sumUntouch += measureResult[j];
		}
	}
	// Calculates average value afer 100 times measurement.
	*avg_TSI_untouch = sumUntouch
			/ (TSI_THRESHOLD_SAMPLING * BOARD_TSI_ELECTRODE_CNT);

	return (kStatus_TSI_Success);

}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
