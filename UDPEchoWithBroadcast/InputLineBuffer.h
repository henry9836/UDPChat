//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: InputLineBuffer.h
// Description	: Input text into a buffer without stalling in a Win32 console window.
// Author		: Henry Oliver
// Mail			: henry983615@gmail.com
//

#ifndef __INPUTLINEBUFFER_H__
#define __INPUTLINEBUFFER_H__

class CInputLineBuffer
{
public:
	CInputLineBuffer(unsigned int uBufferSize);
	~CInputLineBuffer();
	
	void ClearString();
	bool Update();
	void PrintToScreenTop();
	const char* GetString() const;

protected:
	unsigned int	m_uBufferSize;	//!< The total size of the buffer.
	unsigned int	m_uBufferPos;	//!< The position of the next char in the buffer to be entered by the user.
	char*			m_pBuffer;		//!< The buffer contents.
};

#endif