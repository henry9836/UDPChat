//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: networkentity.h
// Description	: Holds network info
// Author		: Henry Oliver
// Mail			: henry983615@gmail.com
//

#ifndef __NETWORKENTITY_H__
#define __NETWORKENTITY_H__

#include <string>
#include <sstream>

enum EMessageType : unsigned char
{
	HANDSHAKE,
	HANDSHAKE_SUCCESS,
	DATA,
	KEEPALIVE,
	COMMAND,
	BROADCAST,
	INITCONN,
	AUTHCH,
	AUTHRE,
	BROADCASTINIT,
	KEEPALIVEC,
	CLOSE
};

struct TPacket 
{
	unsigned short MessageType;
	char MessageContent[1024];
	char PacketData[1024];
	unsigned short PacketSize;

	//void set_packet(short _x, short _y, WORD _object_type, short _object_index, WORD _param)
	void Serialize(EMessageType _MessageType, char* _message)
	{
		
		MessageType = _MessageType;
		strcpy_s(MessageContent, strlen(_message) + 1, _message);

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << MessageContent;
		
		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = static_cast<unsigned short>(_strToSend.size());
	}
	
	TPacket Deserialize(char* _PacketData)
	{
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);
		
		iss >> this->MessageType;
		
		//iss >> this->MessageContent;
		std::string _tempMessageBuffer;
		getline(iss, _tempMessageBuffer);
		strcpy_s(MessageContent, _tempMessageBuffer.length() + 1, _tempMessageBuffer.c_str());
		
		
		return *this;
	}

};

class INetworkEntity
{
public:
	virtual bool Initialise() = 0; //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend) = 0;
	virtual void ReceiveData(char* _pcBufferToReceiveData) = 0;
	virtual void GetRemoteIPAddress(char *_pcSendersIP) = 0;
	virtual unsigned short GetRemotePort() = 0;
	
protected:
	//Additional state variable to indicate whether a network entity is online or not
	bool m_bOnline;
};
#endif 