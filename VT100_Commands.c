/*
 * VT100_Commands.c
 *
 *  Created on: 10 Nov 2016
 *      Author: David
 */

#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_pit_driver.h"
#include "VT100_Commands.h"

// Get various styles on the output of the terminal and clear the current console.
void VT100_terminalClearFormat(int select) {
	VT100_SetupResetDevice();
	VT100_DisplayBBlack();
	switch(select)
	{
	case 0:
		VT100_DisplayFGreen();
		VT100_DisplayBBlack();
		break;
	case 1:
		VT100_DisplayFBlue();
		VT100_DisplayBWhite();
		break;
	case 2:
		VT100_DisplayFYellow();
		VT100_DisplayBBlack();
		break;
	default:
		VT100_DisplayFGreen();
		VT100_DisplayBBlack();
		break;
	}
}

// Get various styles on the output of the terminal without clearing the console
void VT100_terminalFormat(int select) {
	switch(select)
	{
	case 0:
		VT100_DisplayFGreen();
		VT100_DisplayBBlack();
		break;
	case 1:
		VT100_DisplayFBlue();
		VT100_DisplayBWhite();
		break;
	case 2:
		VT100_DisplayFYellow();
		VT100_DisplayBBlack();
		break;
	default:
		VT100_DisplayFGreen();
		VT100_DisplayBBlack();
		break;
	}
}
