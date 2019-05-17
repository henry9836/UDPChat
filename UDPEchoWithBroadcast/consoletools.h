/**
*	@file ConsoleTools.h
*
*
*	@brief	Some simple tools to get input from the user on a windows console.
*
*/


#ifndef __CONSOLE_TOOLS_H__
#define __CONSOLE_TOOLS_H__

#include <iostream>
#include <ctype.h>
#include <conio.h>
#include <Windows.h>

	char* GetLineFromConsole(char* pBuffer, int iNumChars);

	//namespace utility
	//{
		template <size_t iNumChars>
		inline char* GetLineFromConsole(char(&pBuffer)[iNumChars])
		{
			return GetLineFromConsole(pBuffer, (int)iNumChars);
		}
	//}

	char QueryOption(const char* Question, const char* Accepted, bool bCaseSensitive = false);

	char* CollapseBackspacesAndCleanInput(char* pBuffer);

	unsigned short QueryPortNumber(unsigned short uDefault = 0);




#endif // ifndef __CONSOLE_TOOLS_H__
