/**
*	@file ConsoleTools.h
*
*
*	@brief	Some simple tools to get input from the user on a windows console.
*
*/

#include "consoletools.h"

char* GetLineFromConsole(char* pBuffer, int iNumChars)
{
	fgets(pBuffer, iNumChars, stdin);
	char* pBufferEnd = pBuffer + iNumChars;
	// remove any trailing \n:
	for (char* pBuf = pBuffer; pBuf < pBufferEnd && *pBuf != 0; pBuf++)
	{
		if (*pBuf == '\n')
		{
			*pBuf = 0;
			return pBuffer;
		}
	}
	// OK, so we need to strip the rest out up to the newline now as we were cut off:
	char Clip[2] = { 1, 0 };
	while (Clip[0] != '\n')
	{
		fgets(Clip, 2, stdin);
	}
	return pBuffer;
}

//template <size_t iNumChars> 
//char* utility::GetLineFromConsole(char(&pBuffer)[iNumChars])
//{
//	return GetLineFromConsole(pBuffer, (int)iNumChars);
//}

/**
*	Ask the user a question, accepting a number of answers, if nothing acceptable is entered then
*	ask them again.
*
**	@param Question			The question to be asked.
*	@param Accepted			String of characters to accept.
*	@param bCaseSensitive	Are the accepted answers case sensitive?
*	@return					The character of the accepted command entered by the user.
*/

char QueryOption(const char* Question, const char* Accepted, bool bCaseSensitive)
{
	while (true) // forever
	{
		std::cout << Question;
		char Str[2];
		GetLineFromConsole(Str);
		char res = Str[0];
		const char* pTestAccepted = Accepted;
		if (false == bCaseSensitive)
		{
			res = (char)tolower(res);
		}
		while (*pTestAccepted != 0)
		{
			char cTest = *pTestAccepted;
			if (false == bCaseSensitive)
			{
				cTest = (char)tolower(cTest);
			}
			if (res == cTest)
			{
				return *pTestAccepted;
			}
			pTestAccepted++;
		}

		std::cout << "\nSorry" << res << "is not an acceptable answer, please try again.\n", res;
	}
}

/**
*	Go through a string and resolve any backspaces, deleting the
*	requisite characters.
*	Will also clean out any tabs.
*
*	*
*	@param	pBuffer	String to have backspaces resolved.
*	@return A pointer to the start of pBuffer.
*/
char* CollapseBackspacesAndCleanInput(char* pBuffer)
{
	// remove all backspaces and tabs:
	char* pDest = pBuffer;
	for (char* pTest = pBuffer; *pTest != 0; pTest++)
	{
		if (*pTest == '\t')
		{
			*pTest = ' ';
		}
		if (*pTest == '\b')
		{
			if (pDest > pBuffer)
			{
				pDest--;
			}
		}
		else
		{
			if (pDest != pTest)
			{
				*pDest = *pTest;
			}
			pDest++;
		}
	}
	*pDest = 0;
	return pBuffer;
}

/**
*	*
*	@param uDefault, default port number to use if none entered.  If zero do not allow an empty port.
*	@return a port number from the user.
*/
unsigned short QueryPortNumber(unsigned short uDefault)
{
	int iPort = 0;
	while (true)
	{
		char Port[128];


		std::cout << "Enter port number (or just press Enter for default): ";
		GetLineFromConsole(Port);
		iPort = atoi(Port);
		if (iPort != 0)
		{
			return (unsigned short)iPort;
		}
		if (uDefault != 0)
		{
			//std::cout << "Using default " << uDefault << std::endl;
			return uDefault;
		}
		std::cout << "Didn't understand that port number" << std::endl;
	}
}

