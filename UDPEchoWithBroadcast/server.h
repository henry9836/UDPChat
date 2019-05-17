//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: server.h
// Description	: Holds server info
// Author		: Henry Oliver
// Mail			: henry983615@gmail.com
//

#ifndef __SERVER_H__
#define __SERVER_H__

// Library Includes
#include <Windows.h>
#include <map>
#include <time.h>
#include <vector>

// Local Includes
#include "networkentity.h"
#include "WorkQueue.h"

// Types

// Constants

//Forward Declaration
class CSocket;

//Structure to hold User Session Security Details
struct USSD {
	enum AUTHSTATUS {
		NOAUTH,
		AUTHING,
		AUTHCOMPLETE
	};
	int key = 0;
	int authStatus = NOAUTH;
	std::string authIP = "";
	std::string authUser = "";
};

//Structure to hold the details of all connected clients
struct TClientDetails
{
	sockaddr_in m_ClientAddress;
	bool m_bIsAlive = true;
	std::string m_strName;
	USSD SECURITY;
};

class CServer : public INetworkEntity
{
public:
	// Default Constructors/Destructors
	CServer();
	~CServer();

	//MESSAGES

	std::string MOTD = "$END-= WELCOME TO THE UDP CHAT SERVER =-$END";
	const std::string HELP = "$END[Commands]$END !help - displays this help text$END !motd - displays motd$END !chgmotd [new motd] - changes the motd$END !list - list connected users$END !quit - close connection";

	//COMMANDS

	const std::string COMMANDID = "!";
	const std::string COMMANDHELP = "!help";
	const std::string COMMANDMOTD = "!motd";
	const std::string COMMANDCHGMOTD = "!chgmotd";
	const std::string COMMANDQUIT = "!quit";
	const std::string COMMANDLIST = "!list";

	// Virtual Methods from the Network Entity Interface.
	virtual bool Initialise(); //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend);
	bool SendDataTo(char* _pcDataToSend, sockaddr_in _clientAdrress);
	virtual void ReceiveData(char* _pcBufferToReceiveData);
	virtual void CheckPulse();
	void DropTheDead();
	virtual void ProcessData(std::pair<sockaddr_in, std::string> dataItem);
	virtual void GetRemoteIPAddress(char* _pcSendersIP);
	virtual unsigned short GetRemotePort();

	CWorkQueue<std::pair<sockaddr_in, std::string>>* GetWorkQueue();

	//Qs 2: Function to add clients to the map.
private:
	bool AddClient(std::string _strClientName);

private:
	//A Buffer to contain all packet data for the server
	char* m_pcPacketData;
	//A server has a socket object to create the UDP socket at its end.
	CSocket* m_pServerSocket;
	// Make a member variable to extract the IP and port number of the sender from whom we are receiving
	//Since it is a UDP socket capable of receiving from multiple clients; these details will change depending on who has sent the packet we are currently processing.
	sockaddr_in m_ClientAddress; 

	//Qs 2 : Make a map to hold the details of all the client who have connected. 
	//The structure maps client addresses to client details
	std::map<std::string, TClientDetails>* m_pConnectedClients;

	//A workQueue to distribute messages between the main thread and Receive thread.
	CWorkQueue<std::pair<sockaddr_in, std::string>>* m_pWorkQueue;
};

#endif