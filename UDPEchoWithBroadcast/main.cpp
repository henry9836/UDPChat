//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: main.cpp
// Description	: Main program loop
// Author		: Henry Oliver
// Mail			: henry983615@gmail.com
//

//Library Includes
#include <Windows.h>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <stdlib.h>
//#include <vld.h>
#include <thread>

//Local Includes
#include "consoletools.h"
#include "network.h"
#include "client.h"
#include "server.h"
#include "conio.h"
#include "InputLineBuffer.h"
#include <functional>

// make sure the winsock lib is included...
#pragma comment(lib,"ws2_32.lib")

using namespace std::chrono;

bool InitPulse = true;
int TIMEOUT = 10;
int WINDOW = 10;
int cTIMEOUT = 11;
int cWINDOW = 11;
int MAXRETRY = 10;
int retryCount = 0;

//A pointer to hold a client instance
CClient* _pClient = nullptr;
//A pointer to hold a server instance
CServer* _pServer = nullptr;

void CloseProgram() {
	_pClient->DropUs();
}

int main()
{
	srand(static_cast<unsigned int>(time(NULL)));

	high_resolution_clock::time_point start = high_resolution_clock::now();

	char* _pcPacketData = 0; //A local buffer to receive packet data info
	_pcPacketData = new char[MAX_MESSAGE_LENGTH];
	strcpy_s(_pcPacketData, strlen("") + 1, "");

	char _cIPAddress[MAX_ADDRESS_LENGTH]; // An array to hold the IP Address as a string
										  //ZeroMemory(&_cIPAddress, strlen(_cIPAddress));

	unsigned char _ucChoice;
	EEntityType _eNetworkEntityType;
	CInputLineBuffer _InputBuffer(MAX_MESSAGE_LENGTH);
	std::thread _ClientReceiveThread, _ServerReceiveThread;

	//Get the instance of the network
	CNetwork& _rNetwork = CNetwork::GetInstance();
	_rNetwork.StartUp();

	// query, is this to be a client or a server?
	_ucChoice = QueryOption("Do you want to run a client or server (C/S)?", "CS");
	switch (_ucChoice)
	{
	case 'C':
	{
		_eNetworkEntityType = CLIENT;
		break;
	}
	case 'S':
	{
		_eNetworkEntityType = SERVER;
		break;
	}
	default:
	{
		std::cout << "This is not a valid option" << std::endl;
		return 0;
		break;
	}
	}
	if (!_rNetwork.GetInstance().Initialise(_eNetworkEntityType))
	{
		std::cout << "Unable to initialise the Network........Press any key to continue......";
		_getch();
		return 0;
	}

	//Run receive on a separate thread so that it does not block the main client thread.
	if (_eNetworkEntityType == CLIENT) //if network entity is a client
	{

		_pClient = static_cast<CClient*>(_rNetwork.GetInstance().GetNetworkEntity());
		_ClientReceiveThread = std::thread(&CClient::ReceiveData, _pClient, std::ref(_pcPacketData));

	}

	//Run receive of server also on a separate thread 
	else if (_eNetworkEntityType == SERVER) //if network entity is a server
	{

		_pServer = static_cast<CServer*>(_rNetwork.GetInstance().GetNetworkEntity());
		_ServerReceiveThread = std::thread(&CServer::ReceiveData, _pServer, std::ref(_pcPacketData));

	}

	while (_rNetwork.IsOnline())
	{
		if (_eNetworkEntityType == CLIENT) //if network entity is a client
		{
			atexit(CloseProgram);
			_pClient = static_cast<CClient*>(_rNetwork.GetInstance().GetNetworkEntity());

			//Prepare for reading input from the user
			_InputBuffer.PrintToScreenTop();

			//Get input from the user
			if (_InputBuffer.Update())
			{

				std::string msg = _InputBuffer.GetString();

				msg = _pClient->XORClient(msg, _pClient->key, true);

				// we completed a message, lets send it:
				//int _iMessageSize = static_cast<int>(strlen(_InputBuffer.GetString()));

				//Put the message into a packet structure
				TPacket _packet;
				_packet.Serialize(DATA, const_cast<char*>(msg.c_str()));
				_rNetwork.GetInstance().GetNetworkEntity()->SendData(_packet.PacketData);
				//Clear the Input Buffer
				_InputBuffer.ClearString();
				//Print To Screen Top
				_InputBuffer.PrintToScreenTop();
			}
			if (_pClient != nullptr)
			{
				high_resolution_clock::time_point end = high_resolution_clock::now();
				auto timeElapsed = duration_cast<seconds>(end - start).count();

				if (_pClient->InitPulse && (timeElapsed > cTIMEOUT)) {
					if (retryCount == 0) {
						_pClient->ConnectionAlive = false;
					}
					_pClient->CheckPulse();
					retryCount++;
					if (retryCount >= MAXRETRY) {
						start = high_resolution_clock::now();
						_pClient->InitPulse = false;
					}
				}
				else if ((!_pClient->InitPulse && timeElapsed > cWINDOW) && retryCount >= MAXRETRY) {
					_pClient->DropTheDead(); //Drop server because of timeout
					_pClient->InitPulse = true;
					start = high_resolution_clock::now();
					retryCount = 0;
				}
				//else {
				//	_pClient->CheckPulse();
				//	start = high_resolution_clock::now();
				//	retryCount++;
				//}
				//If the message queue is empty 
				if (_pClient->GetWorkQueue()->empty())
				{
					//Don't do anything
				}
				else
				{
					//Retrieve off a message from the queue and process it
					std::string temp;
					_pClient->GetWorkQueue()->pop(temp);
					_pClient->ProcessData(const_cast<char*>(temp.c_str()));
				}
			}

		}
		else //if you are running a server instance
		{

			if (_pServer != nullptr)
			{
				high_resolution_clock::time_point end = high_resolution_clock::now();
				auto timeElapsed = duration_cast<seconds>(end - start).count();

				if (InitPulse && (timeElapsed > TIMEOUT)) {
					std::cout << "Sent out pulse" << std::endl;
					_pServer->CheckPulse();
					retryCount++;
					if (retryCount >= MAXRETRY) {
						start = high_resolution_clock::now();
						InitPulse = false;
					}
				}
				else if ((!InitPulse && timeElapsed > WINDOW) && retryCount >= MAXRETRY) {
					std::cout << "Dropping the dead..." << std::endl;
					_pServer->DropTheDead(); //Drop any clients that have taken too long to respond
					InitPulse = true;
					start = high_resolution_clock::now();
					retryCount = 0;
				}
				//else {
				//	_pServer->CheckPulse();
				//	start = high_resolution_clock::now();
				//	retryCount++;
				//}
				if (!_pServer->GetWorkQueue()->empty())
				{
					_rNetwork.GetInstance().GetNetworkEntity()->GetRemoteIPAddress(_cIPAddress);
					//std::cout << _cIPAddress
					//<< ":" << _rNetwork.GetInstance().GetNetworkEntity()->GetRemotePort() << "> " << _pcPacketData << std::endl;

					//Retrieve off a message from the queue and process it
					std::pair<sockaddr_in, std::string> dataItem;
					_pServer->GetWorkQueue()->pop(dataItem);
					_pServer->ProcessData(dataItem);
				}
			}
		}


	} //End of while network is Online

	_ClientReceiveThread.join();
	_ServerReceiveThread.join();

	//Shut Down the Network
	_rNetwork.ShutDown();
	_rNetwork.DestroyInstance();

	delete[] _pcPacketData;
	return 0;
}