//
// (c) 2015 Media Design School
//
// File Name	: server.cpp
// Description	: Does server work
// Author		: Henry Oliver
// Mail			: henry983615@gmail.com
//

//Library Includes
#include <WS2tcpip.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//Local Includes
#include "utils.h"
#include "network.h"
#include "consoletools.h"
#include "socket.h"


//Local Includes
#include "server.h"

//Global Temps

std::string addrTmp = "";

CServer::CServer()
	:m_pcPacketData(0),
	m_pServerSocket(0)
{
	ZeroMemory(&m_ClientAddress, sizeof(m_ClientAddress));
}

CServer::~CServer()
{
	delete m_pConnectedClients;
	m_pConnectedClients = 0;

	delete m_pServerSocket;
	m_pServerSocket = 0;

	delete m_pWorkQueue;
	m_pWorkQueue = 0;
	
	delete[] m_pcPacketData;
	m_pcPacketData = 0;
}

bool CServer::Initialise()
{
	m_pcPacketData = new char[MAX_MESSAGE_LENGTH];
	
	//Create a work queue to distribute messages between the main  thread and the receive thread.
	m_pWorkQueue = new CWorkQueue<std::pair<sockaddr_in, std::string>>();

	//Create a socket object
	m_pServerSocket = new CSocket();

	//Get the port number to bind the socket to
	unsigned short _usServerPort = QueryPortNumber(DEFAULT_SERVER_PORT);

	//Initialise the socket to the local loop back address and port number
	if (!m_pServerSocket->Initialise(_usServerPort))
	{
		return false;
	}

	//Qs 2: Create the map to hold details of all connected clients
	m_pConnectedClients = new std::map < std::string, TClientDetails >() ;

	return true;
}

bool CServer::AddClient(std::string _strClientName)
{
	//TO DO : Add the code to add a client to the map here...
	
	for (auto it = m_pConnectedClients->begin(); it != m_pConnectedClients->end(); ++it)
	{
		//Check to see that the client to be added does not already exist in the map, 
		if(it->first == ToString(m_ClientAddress))
		{
			TPacket _packetToSend;
			std::string message = "Connection Refused: IP Already Exists";
			_packetToSend.Serialize(CLOSE, const_cast<char*>(message.c_str()));
			SendDataTo(_packetToSend.PacketData, m_ClientAddress);
			return false;
		}
		//also check for the existence of the username
		if (it->second.m_strName == _strClientName)
		{
			TPacket _packetToSend;
			std::string message = "Connection Refused: User Already Exists";
			_packetToSend.Serialize(CLOSE, const_cast<char*>(message.c_str()));
			SendDataTo(_packetToSend.PacketData, m_ClientAddress);
			return false;
		}
	}
	//Add the client to the map.
	TClientDetails _clientToAdd;
	_clientToAdd.m_strName = _strClientName;
	_clientToAdd.m_ClientAddress = this->m_ClientAddress;

	std::string _strAddress = ToString(m_ClientAddress);
	addrTmp = _strAddress;
	
	//Add security info
	_clientToAdd.SECURITY.authIP = _strAddress;
	_clientToAdd.SECURITY.authStatus = _clientToAdd.SECURITY.NOAUTH;
	_clientToAdd.SECURITY.authUser = _clientToAdd.m_strName;
	_clientToAdd.SECURITY.authUser = _clientToAdd.SECURITY.authUser.substr(1, _clientToAdd.SECURITY.authUser.length()); //get rid of padding
	_clientToAdd.SECURITY.key = 4 + rand() % 150;

	//Check for bad chars in security info

	std::string ENDLN = "$END";

	size_t pos = _clientToAdd.SECURITY.authUser.find(ENDLN);

	while (pos != std::string::npos)
	{
		_clientToAdd.SECURITY.authUser.replace(pos, ENDLN.size(), "");
		pos = _clientToAdd.SECURITY.authUser.find(ENDLN, pos + 0);
	}

	//Insert into the map

	m_pConnectedClients->insert(std::pair < std::string, TClientDetails > (_strAddress, _clientToAdd));

	return true;
}

bool CServer::SendData(char* _pcDataToSend)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;
	
	int iNumBytes = sendto(
		m_pServerSocket->GetSocketHandle(),				// socket to send through.
		_pcDataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<sockaddr*>(&m_ClientAddress),	// address to be filled with packet target
		sizeof(m_ClientAddress)							// size of the above address struct.
		);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

bool CServer::SendDataTo(char* _pcDataToSend, sockaddr_in _clientAdrress)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	int iNumBytes = sendto(
		m_pServerSocket->GetSocketHandle(),				// socket to send through.
		_pcDataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<sockaddr*>(&_clientAdrress),	// address to be filled with packet target
		sizeof(_clientAdrress)							// size of the above address struct.
	);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

void CServer::ReceiveData(char* _pcBufferToReceiveData)
{
	
	int iSizeOfAdd = sizeof(m_ClientAddress);
	int _iNumOfBytesReceived;
	int _iPacketSize;

	//Make a thread local buffer to receive data into
	char _buffer[MAX_MESSAGE_LENGTH];

	while (true)
	{
		// pull off the packet(s) using recvfrom()
		_iNumOfBytesReceived = recvfrom(			// pulls a packet from a single source...
			m_pServerSocket->GetSocketHandle(),						// client-end socket being used to read from
			_buffer,							// incoming packet to be filled
			MAX_MESSAGE_LENGTH,					   // length of incoming packet to be filled
			0,										// flags
			reinterpret_cast<sockaddr*>(&m_ClientAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
		);
		if (_iNumOfBytesReceived < 0)
		{
			int _iError = WSAGetLastError();
			ErrorRoutines::PrintWSAErrorInfo(_iError);
			//return false;
		}
		else
		{
			_iPacketSize = static_cast<int>(strlen(_buffer)) + 1;
			strcpy_s(_pcBufferToReceiveData, _iPacketSize, _buffer);
			char _IPAddress[100];
			inet_ntop(AF_INET, &m_ClientAddress.sin_addr, _IPAddress, sizeof(_IPAddress));
			
			std::cout << "Server Received \"" << _pcBufferToReceiveData << "\" from " <<
				_IPAddress << ":" << ntohs(m_ClientAddress.sin_port) << std::endl;

			//Push this packet data into the WorkQ
			m_pWorkQueue->push(std::make_pair(m_ClientAddress,_pcBufferToReceiveData));
		}
		//std::this_thread::yield();
		
	} //End of while (true)
}

void CServer::GetRemoteIPAddress(char *_pcSendersIP)
{
	char _temp[MAX_ADDRESS_LENGTH];
	int _iAddressLength;
	inet_ntop(AF_INET, &(m_ClientAddress.sin_addr), _temp, sizeof(_temp));
	_iAddressLength = static_cast<int>(strlen(_temp)) + 1;
	strcpy_s(_pcSendersIP, _iAddressLength, _temp);
}

unsigned short CServer::GetRemotePort()
{
	return ntohs(m_ClientAddress.sin_port);
}

std::string XOR(std::string input, int key, bool encode) {
	std::string output = "";
	std::string split = "\\";
	std::string tmpstr = "";
	std::vector<int> toDecode;
	char decodeTMP;
	int position = 0;
	int xor = 0;
	
	if (encode) {
		int xor = 0;
		for (size_t i = 0; i < input.length(); i++)
		{
			xor = ((input.at(i)) ^ key);
			output += std::to_string(xor) + split;
		}
	}
	else { //decode
		for (size_t i = 0; i < input.length(); i++)
		{
			if (input.at(i) != split.at(0)) {
				tmpstr += input.at(i);
			}
			else {
				//std::cout << tmpstr << std::endl;
				toDecode.push_back(std::atoi(tmpstr.c_str()));
				tmpstr = "";
			}
		}
		//std::cout << tmpstr << std::endl;
		toDecode.push_back(std::atoi(tmpstr.c_str()));
		tmpstr = "";
		for (size_t k = 0; k < toDecode.size(); k++)
		{
			xor = (toDecode.at(k) ^ key); //decode
			decodeTMP = xor; //convert to ascii
			output += decodeTMP; //add to output
		}
		std::cout << "Output: " << output << " Key used: " << key << std::endl;
	}
	return output;
}

void CServer::CheckPulse() {
	TPacket _packetRecvd, _packetToSend;
	std::string message = "Ping";
	for (std::map<std::string, TClientDetails>::const_iterator it = (*m_pConnectedClients).begin(); it != (*m_pConnectedClients).end(); ++it)
	{
		if ((*m_pConnectedClients)[it->first].SECURITY.authStatus == (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.AUTHCOMPLETE) { //only check authed clients
			_packetToSend.Serialize(KEEPALIVE, const_cast<char*>(message.c_str()));
			(*m_pConnectedClients)[it->first].m_bIsAlive = false;
			SendDataTo(_packetToSend.PacketData, (*m_pConnectedClients)[it->first].m_ClientAddress);
		}
	}
}

void CServer::DropTheDead(){
	TPacket _packetRecvd, _packetToSend;
	std::string message = "";
	int amountremoved = 0;
	for (std::map<std::string, TClientDetails>::const_iterator it = (*m_pConnectedClients).begin(); it != (*m_pConnectedClients).end(); ++it)
	{
		if ((*m_pConnectedClients)[it->first].SECURITY.authStatus == (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.AUTHCOMPLETE) { //only check authed clients
			if (!(*m_pConnectedClients)[it->first].m_bIsAlive) { //if client hasn't pinged then drop
				message += "$ENDUser: " + (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authUser + " has disconnected (Timed out)";
				it = (*m_pConnectedClients).erase(it); //remove user from list
				
			}
		}
	}
	if (message != "") {
		for (std::map<std::string, TClientDetails>::const_iterator it = (*m_pConnectedClients).begin(); it != (*m_pConnectedClients).end(); ++it)
		{
			if ((*m_pConnectedClients)[it->first].SECURITY.authStatus == (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.AUTHCOMPLETE) { //only authed clients can see msgs
				_packetToSend.Serialize(DATA, const_cast<char*>(message.c_str()));
				SendDataTo(_packetToSend.PacketData, (*m_pConnectedClients)[it->first].m_ClientAddress);
			}
		}
	}
}

void CServer::ProcessData(std::pair<sockaddr_in, std::string> dataItem)
{

	TPacket _packetRecvd, _packetToSend;
	_packetRecvd = _packetRecvd.Deserialize(const_cast<char*>(dataItem.second.c_str()));


	switch (_packetRecvd.MessageType)
	{
	case INITCONN:
	{
		std::string message = "Users in chatroom : ";
		std::cout << "Server received a request to start a handshake" << std::endl;
		if (AddClient(_packetRecvd.MessageContent))
		{
			//Qs 3: To DO : Add the code to do a handshake here
			//If this is first AUTH then we need create an xor challenge
			std::map<std::string, char> m;
			int key = (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.key;
			message = "%" + XOR((dataItem.second.substr(2, dataItem.second.length()) + "#" + std::to_string(dataItem.first.sin_addr.S_un.S_un_b.s_b1) + "." + std::to_string(dataItem.first.sin_addr.S_un.S_un_b.s_b2) + "." + std::to_string(dataItem.first.sin_addr.S_un.S_un_b.s_b3) + "." + std::to_string(dataItem.first.sin_addr.S_un.S_un_b.s_b4)), key, true);
			//ADD XOR KEY ON
			int amountofch = floor(key / 10);
			for (size_t i = 0; i < amountofch; i++)
			{
				int tmp = (48 + rand() % 78);
				char ctmp = tmp;
				message = ctmp + message;
			}
			_packetToSend.Serialize(AUTHCH, const_cast<char*>(message.c_str()));
			SendDataTo(_packetToSend.PacketData, dataItem.first);
		}
		else {
			std::cout << "Could not add the user" << std::endl;
		}
		break;
	}
	case AUTHRE: {
		std::cout << "Server received a handshake response" << std::endl;
		std::string message = "ACCEPT";
		std::string check = "";

		//CHECK RESPONSE
		int key = (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.key;
		check = XOR(_packetRecvd.MessageContent, key, false); //decode
		
		if (check.find((*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authUser) != std::string::npos) { //correct response
			std::cout << "HANDSHAKE COMPLETED" << std::endl;
			_packetToSend.Serialize(AUTHRE, const_cast<char*>(message.c_str()));
			SendDataTo(_packetToSend.PacketData, dataItem.first);
			message = MOTD + "Welcome to the chat " + (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authUser + "$ENDUsers in Chat:";
			for (std::map<std::string, TClientDetails>::const_iterator it = (*m_pConnectedClients).begin(); it != (*m_pConnectedClients).end(); ++it)
			{
				message += "$END [" + (*m_pConnectedClients)[it->first].SECURITY.authUser + "]";
			}
			message += "$END";
			_packetToSend.Serialize(DATA, const_cast<char*>(message.c_str()));
			SendDataTo(_packetToSend.PacketData, dataItem.first);
			(*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authStatus = (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.AUTHCOMPLETE;
			
			//Send to each user that user has joined chat
			std::string message = (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authUser + " has connected to the chat!";
			for (std::map<std::string, TClientDetails>::const_iterator it = (*m_pConnectedClients).begin(); it != (*m_pConnectedClients).end(); ++it)
			{
				_packetToSend.Serialize(DATA, const_cast<char*>(message.c_str()));
				SendDataTo(_packetToSend.PacketData, (*m_pConnectedClients)[it->first].m_ClientAddress);
			}
		}
		else { //incorrect response
			std::cout << "HANDSHAKE FAILED" << std::endl;
			message = "REJECT";
			_packetToSend.Serialize(AUTHRE, const_cast<char*>(message.c_str()));
			SendDataTo(_packetToSend.PacketData, dataItem.first);
			(*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authStatus = (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.NOAUTH;

			message = "QUIT";
			_packetToSend.Serialize(CLOSE, const_cast<char*>(message.c_str()));
			SendDataTo(_packetToSend.PacketData, (*m_pConnectedClients)[ToString(m_ClientAddress)].m_ClientAddress);
			m_pConnectedClients->erase(ToString(m_ClientAddress)); //remove user from list

		}
		
	}
	case DATA:
	{
		if ((*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authStatus == (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.AUTHCOMPLETE) {

			std::string message = _packetRecvd.MessageContent;

			message = message.substr(1, message.length()); //fix padding

			bool isCommand = false;

			if (message.at(0) == COMMANDID.at(0)) {
				isCommand = true;
			}

			if (!isCommand){ //Chat
				//Send to each user
				message = (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authUser + "> " + message;
				for (std::map<std::string, TClientDetails>::const_iterator it = (*m_pConnectedClients).begin(); it != (*m_pConnectedClients).end(); ++it)
				{
					if ((*m_pConnectedClients)[it->first].SECURITY.authStatus == (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.AUTHCOMPLETE) { //only authed clients can see msgs
						_packetToSend.Serialize(DATA, const_cast<char*>(message.c_str()));
						SendDataTo(_packetToSend.PacketData, (*m_pConnectedClients)[it->first].m_ClientAddress);
					}
				}
			}
			else { //Command

				if (message == COMMANDHELP) {
					message = HELP;
				}
				else if (message.find(COMMANDCHGMOTD) != std::string::npos) {
					MOTD = message.substr(COMMANDCHGMOTD.length() + 1, message.length());
					message = "motd changed to: " + MOTD;
				}
				else if (message == COMMANDMOTD) {
					message = MOTD;
				}
				else if (message == COMMANDQUIT) {
					//CLOSE CONNECTION
					(*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authStatus = (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.NOAUTH; //Deauth user
					message = "User: " + (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.authUser + " has disconnected (User Disconnect)";
					for (std::map<std::string, TClientDetails>::const_iterator it = (*m_pConnectedClients).begin(); it != (*m_pConnectedClients).end(); ++it)
					{
						if ((*m_pConnectedClients)[it->first].SECURITY.authStatus == (*m_pConnectedClients)[ToString(m_ClientAddress)].SECURITY.AUTHCOMPLETE) { //only authed clients can see msgs
							_packetToSend.Serialize(DATA, const_cast<char*>(message.c_str()));
							SendDataTo(_packetToSend.PacketData, (*m_pConnectedClients)[it->first].m_ClientAddress);
						}
					}
					message = "QUIT";
					_packetToSend.Serialize(CLOSE, const_cast<char*>(message.c_str()));
					SendDataTo(_packetToSend.PacketData, (*m_pConnectedClients)[ToString(m_ClientAddress)].m_ClientAddress);
					m_pConnectedClients->erase(ToString(m_ClientAddress)); //remove user from list
				}
				else if (message == COMMANDLIST) {
					message = "$ENDUsers in Chat:";
					for (std::map<std::string, TClientDetails>::const_iterator it = (*m_pConnectedClients).begin(); it != (*m_pConnectedClients).end(); ++it)
					{
						message += "$END [" + (*m_pConnectedClients)[it->first].SECURITY.authUser + "]";
					}
					message += "$END";
				}
				else{
					message = "Unknown Command";
				}
				_packetToSend.Serialize(DATA, const_cast<char*>(message.c_str()));
				SendDataTo(_packetToSend.PacketData, (*m_pConnectedClients)[ToString(m_ClientAddress)].m_ClientAddress);
			}

		}
		else {
			std::string out = (std::to_string(dataItem.first.sin_addr.S_un.S_un_b.s_b1) + "." + std::to_string(dataItem.first.sin_addr.S_un.S_un_b.s_b2) + "." + std::to_string(dataItem.first.sin_addr.S_un.S_un_b.s_b3) + "." + std::to_string(dataItem.first.sin_addr.S_un.S_un_b.s_b4));
			std::cout << "CLIENT WAS BLOCKED FROM USING SERVER AS THEY ARE NOT AUTHED: " << out << std::endl;
			out = "REJECT";
			_packetToSend.Serialize(AUTHRE, const_cast<char*>(out.c_str()));
			SendDataTo(_packetToSend.PacketData, dataItem.first);
		}
		break;
	}

	case BROADCAST:
	{
		std::cout << "Received a broadcast packet" << std::endl;
		//Just send out a packet to the back to the client again which will have the server's IP and port in it's sender fields
		std::string message = "<SERVER INFO>";
		_packetToSend.Serialize(BROADCASTINIT, const_cast<char*>(message.c_str()));
		SendData(_packetToSend.PacketData);
		break;
	}
	
	case KEEPALIVEC: {
		std::string message = "<SERVER INFO>";
		_packetToSend.Serialize(KEEPALIVEC, const_cast<char*>(message.c_str()));
		SendData(_packetToSend.PacketData);
		break;
	}

	case KEEPALIVE:
	{
		m_pConnectedClients->at(ToString(m_ClientAddress)).m_bIsAlive = true; //we have ping-pong
		break;
	}

	default:
		break;

	}
}

CWorkQueue<std::pair<sockaddr_in, std::string>>* CServer::GetWorkQueue()
{
	return m_pWorkQueue;
}
