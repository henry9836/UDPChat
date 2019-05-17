//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: inputlinebuffer.cpp
// Description	: Input text into a buffer without stalling in a Win32 console window.
// Author		: Henry Oliver
// Mail			: henry983615@gmail.com
//

//Library Includes
#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <sstream>

//This Include
#include "InputLineBuffer.h"
	/**
	*	Constructor.
	*
	*	Creates the buffer and sets the string to be empty.
	*
	*	@param uBufferSize	The maximum size of the input buffer.
	*						No more characters than this will be parsed.
	*/
CInputLineBuffer::CInputLineBuffer(unsigned int uBufferSize)
	: m_uBufferSize(uBufferSize)
	, m_uBufferPos(0)
	, m_pBuffer(new char[uBufferSize])
{
	m_pBuffer[0] = 0;
}

	/**
	*	Deconstructor.
	*	Cleans up and frees all memory.
	*
	*	*/
CInputLineBuffer::~CInputLineBuffer()
{
	delete[] m_pBuffer;
	m_pBuffer = nullptr;
	m_uBufferSize = m_uBufferPos = 0;
}

	/**
	*	Clear the current buffered string and set it to empty.
	*
	*/
void CInputLineBuffer::ClearString(void)
	{
		m_pBuffer[0] = 0;
		m_uBufferPos = 0;
	}

	/**
	*	Update the contents of this line buffer.
	*	Will update the view at the top of the window too.
	*	@see PrintToScreenTop
	*
	*
	*	@return true if a newline was encountered.
	*/
bool CInputLineBuffer::Update()
{
	//while (_kbhit())
	//{
	//	char cNext = (char)_getch();
	//	if (cNext == '\t')
	//	{
	//		continue; // we don't like tabs so ignore them.
	//	}
	//	if (cNext == '\b')
	//	{
	//		if (m_uBufferPos)
	//		{
	//			m_uBufferPos--;
	//			m_pBuffer[m_uBufferPos] = 0;
	//			PrintToScreenTop();
	//		}
	//		continue; // no good for the rest now we've deleted a character.
	//	}
	//	if (cNext == '\n' || cNext == '\r')
	//	{
	//		return true;
	//	}
	//	if (m_uBufferSize > m_uBufferPos + 1)
	//	{
	//		m_pBuffer[m_uBufferPos] = cNext;
	//		m_uBufferPos++;
	//		m_pBuffer[m_uBufferPos] = 0;
	//		PrintToScreenTop();
	//	}
	//}
	//return false; // no newline found.

	if (!_kbhit()) return false;

	std::string line;
	std::getline(std::cin, line);
	strcpy_s(m_pBuffer, line.size() + 1, line.c_str());
	m_uBufferPos = static_cast<unsigned int>(line.length());
	m_pBuffer[m_uBufferPos] = 0;

	return true; //uses a blocking call now
}

	/**
	*	Prints the current contents of the buffer to the top of the
	*	screen (terminal window).
	*
	*
	*/
void CInputLineBuffer::PrintToScreenTop(void)
{
	CONSOLE_SCREEN_BUFFER_INFO BufInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufInfo);

	COORD coord = { 0, 0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
	std::cout << m_pBuffer;
	const int iExtraToClearLine = BufInfo.dwSize.X - (m_uBufferPos%BufInfo.dwSize.X);
	for (int i = 0; i<iExtraToClearLine; i++)
	{
		std::cout << " ";
	}
	for (int i = 0; i<BufInfo.dwSize.X; i++)
	{
		std::cout << "-";
	}
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), BufInfo.dwCursorPosition);
}

	/**
	*	Get the current contents of the buffer as entered by the user.
	*
	*
	*	@return The current string contents.
	*/
const char* CInputLineBuffer::GetString(void) const
{ 
	return m_pBuffer; 
}

