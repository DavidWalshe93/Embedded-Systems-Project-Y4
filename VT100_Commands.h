/*
 * VT100_Commands.h
 *
 *  Created on: 27 Oct 2016
 *      Author: David
 */

#ifndef _VT100_Commands_h_
#define _VT100_Commands_h_

#include <stdio.h>
#include <string.h>

#define ESC 0x1B

// Terminal Setup Commands
#define VT100_SetupResetDevice() PRINTF("%cc", ESC)
#define VT100_SetupEnableLineWrap() PRINTF("%c[7h", ESC)
#define VT100_SetupDisableLineWrap() PRINTF("%c[7l", ESC)

// Terminal Font Commands
#define VT100_SetDefaultFont() PRINTF("%c(", ESC)
#define VT100_SetAltFont() PRINTF("%c)", ESC)

// Terminal Cursor Control Commands
#define VT100_CursorHome() PRINTF("%c[H", ESC)
#define VT100_CursorUp(count) PRINTF("%c[%iA", ESC, count)
#define VT100_CursorDown(count) PRINTF("%c[%iB", ESC, count)
#define VT100_CursorForward(count) PRINTF("%c[%iC", ESC, count)
#define VT100_CursorBackward(count) PRINTF("%c[%iD", ESC, count)
#define VT100_CursorForcePos(countX, countY) PRINTF("%c[%i;%if", ESC, countX, countY)
#define VT100_CursorSave() PRINTF("%c[s", ESC)
#define VT100_CursorUnsave() PRINTF("%c[u", ESC)
#define VT100_CursorSaveAttri() PRINTF("%c7", ESC)
#define VT100_CursorUnsaveAttri() PRINTF("%c8", ESC)

// Terminal Scrolling Commands
#define VT100_ScrollScreen() PRINTF("%c[r", ESC)
#define VT100_ScrollScreenPos(start, end) PRINTF("%c[%i;%ir", ESC, start, end)
#define VT100_ScrollDown() PRINTF("%cD", ESC)
#define VT100_ScrollUp() PRINTF("%cM", ESC)

// Terminal Tab Control Commands
#define VT100_TabSet() PRINTF("%cH", ESC)
#define VT100_TabClear() PRINTF("%c[g", ESC)
#define VT100_TabClearAll() PRINTF("%c[3g", ESC)

// Terminal Erase Text Commands
#define VT100_EraseEoL() PRINTF("%c[K", ESC)
#define VT100_EraseSoL() PRINTF("%c[1K", ESC)
#define VT100_EraseLine() PRINTF("%c[2K", ESC)
#define VT100_EraseDown() PRINTF("%c[J", ESC)
#define VT100_EraseUp() PRINTF("%c[1J]", ESC)
#define VT100_EraseScreen() PRINTF("%c[2J", ESC)

// Terminal Printing Commands
#define VT100_PrintScreen() PRINTF("%c[i", ESC)
#define VT100_PrintLine() PRINTF("%c[1i", ESC)
#define VT100_PrintStopLog() PRINTF("%c[4i", ESC)
#define VT100_PrintStartLog() PRINTF("%c[5i", ESC)

// Terminal Define Key Command
#define VT100_DefineKey(key, string) PRINTF("%c[%d;\"%s\"p", ESC, key, string)

// Terminal Define Display Attributes
#define VT100_DisplayReset() PRINTF("%c[0m", ESC)
#define VT100_DisplayBright() PRINTF("%c[1m", ESC)
#define VT100_DisplayDim() PRINTF("%c[2m", ESC)
#define VT100_DisplayUnderscore() PRINTF("%c[4m", ESC)
#define VT100_DisplayBlink() PRINTF("%c[5m", ESC)
#define VT100_DisplayReverse() PRINTF("%c[7m", ESC)
#define VT100_DisplayHidden() PRINTF("%c[8m", ESC)

#define VT100_DisplayFBlack() PRINTF("%c[30m", ESC)
#define VT100_DisplayFRed() PRINTF("%c[31m", ESC)
#define VT100_DisplayFGreen() PRINTF("%c[32m", ESC)
#define VT100_DisplayFYellow() PRINTF("%c[33m", ESC)
#define VT100_DisplayFBlue() PRINTF("%c[34m", ESC)
#define VT100_DisplayFMagenta() PRINTF("%c[35m", ESC)
#define VT100_DisplayFCyan() PRINTF("%c[36m", ESC)
#define VT100_DisplayFWhite() PRINTF("%c[37m", ESC)

#define VT100_DisplayBBlack() PRINTF("%c[40m", ESC)
#define VT100_DisplayBRed() PRINTF("%c[41m", ESC)
#define VT100_DisplayBGreen() PRINTF("%c[42m", ESC)
#define VT100_DisplayBYellow() PRINTF("%c[43m", ESC)
#define VT100_DisplayBBlue() PRINTF("%c[44m", ESC)
#define VT100_DisplayBMagenta() PRINTF("%c[45m", ESC)
#define VT100_DisplayBCyan() PRINTF("%c[46m", ESC)
#define VT100_DisplayBWhite() PRINTF("%c[47m", ESC)

void VT100_terminalClearFormat(int select);
void VT100_terminalFormat(int select);


#endif /* UTILITIES_VT100_COMMANDS_H_ */
