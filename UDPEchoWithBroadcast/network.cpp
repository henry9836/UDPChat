//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: network.cpp
// Description	: does network work
// Author		: Henry Oliver
// Mail			: henry983615@gmail.com
//

//Library Includes
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>

//Local Includes
#include "utils.h"
#include "consoletools.h"
#include "server.h"
#include "client.h"

//This includes
#include "network.h"

// Static Variables
CNetwork* CNetwork::s_pNetwork = 0;


CNetwork::CNetwork()
	: m_pNetworkEntity(0)
	, m_bOnline(false)
{

}


CNetwork::~CNetwork()
{
	delete m_pNetworkEntity;
	m_pNetworkEntity = 0;
}

bool
CNetwork::Initialise(EEntityType _eType)
{
	switch (_eType)
	{
	case CLIENT:
		{
			m_pNetworkEntity = new CClient();
			break;
		}
	case SERVER:
		{
			m_pNetworkEntity = new CServer();
			break;
		}
	default:
		{
			//Add some error handling in here
			break;
		}
	}
	VALIDATE(m_pNetworkEntity->Initialise());
	return (true);
}

void
CNetwork::StartUp()
{
	// startup windows sockets:
	WSADATA wsaData;
	int _iError; 
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		_iError = WSAGetLastError();
		//Diagnostic error messages to be added!!
	}
	m_bOnline = true;
}

void
CNetwork::ShutDown()
{
	int _iError;
	if (WSACleanup() != 0)
	{
		_iError = WSAGetLastError();
		//Diagnostic error messages to be added!!
	}
	m_bOnline = false;
}

CNetwork&
CNetwork::GetInstance()
{
	if (s_pNetwork == 0)
	{
		s_pNetwork = new CNetwork();
	}

	return (*s_pNetwork);
}

void
CNetwork::DestroyInstance()
{
	delete s_pNetwork;
	s_pNetwork = 0;
}


INetworkEntity* CNetwork::GetNetworkEntity()
{
	return m_pNetworkEntity;
}

bool CNetwork::IsOnline()
{
	return m_bOnline;
}

void ErrorRoutines::PrintWSAErrorInfo(int iError)
{
	switch (iError)
	{
	case WSA_INVALID_HANDLE:
		printf("WSA_INVALID_HANDLE\n");
		printf("Specified event object handle is invalid.\n");
		printf("An application attempts to use an event object, but the specified handle is not valid. Note that this error is returned by the operating system, so the error number may change in future releases of Windows.\n");
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		printf("WSA_NOT_ENOUGH_MEMORY\n");
		printf("Insufficient memory available.\n");
		printf("An application used a Windows Sockets function that directly maps to a Windows function. The Windows function is indicating a lack of required memory resources. Note that this error is returned by the operating system, so the error number may change in future releases of Windows.\n");
		break;
	case WSA_INVALID_PARAMETER:
		printf("WSA_INVALID_PARAMETER\n");
		printf("One or more parameters are invalid.\n");
		printf("An application used a Windows Sockets function which directly maps to a Windows function. The Windows function is indicating a problem with one or more parameters. Note that this error is returned by the operating system, so the error number may change in future releases of Windows.\n");
		break;
	case WSA_OPERATION_ABORTED:
		printf("WSA_OPERATION_ABORTED\n");
		printf("Overlapped operation aborted.\n");
		printf("An overlapped operation was canceled due to the closure of the socket, or the execution of the SIO_FLUSH command in WSAIoctl. Note that this error is returned by the operating system, so the error number may change in future releases of Windows.\n");
		break;
	case WSA_IO_INCOMPLETE:
		printf("WSA_IO_INCOMPLETE\n");
		printf("Overlapped I/O event object not in signaled state.\n");
		printf("The application has tried to determine the status of an overlapped operation which is not yet completed. Applications that use WSAGetOverlappedResult (with the fWait flag set to false) in a polling mode to determine when an overlapped operation has completed, get this error code until the operation is complete. Note that this error is returned by the operating system, so the error number may change in future releases of Windows.\n");
		break;
	case WSA_IO_PENDING:
		printf("WSA_IO_PENDING\n");
		printf("Overlapped operations will complete later.\n");
		printf("The application has initiated an overlapped operation that cannot be completed immediately. A completion indication will be given later when the operation has been completed. Note that this error is returned by the operating system, so the error number may change in future releases of Windows.\n");
		break;
	case WSAEINTR:
		printf("WSAEINTR\n");
		printf("Interrupted function call.\n");
		printf("A blocking operation was interrupted by a call to WSACancelBlockingCall.\n");
		break;
	case WSAEBADF:
		printf("WSAEBADF\n");
		printf("File handle is not valid.\n");
		printf("The file handle supplied is not valid.\n");
		break;
	case WSAEACCES:
		printf("WSAEACCES\n");
		printf("Permission denied.\n");
		printf("An attempt was made to access a socket in a way forbidden by its access permissions. An example is using a broadcast address for sendto without broadcast permission being set using setsockopt(SO_BROADCAST).    Another possible reason for the WSAEACCES error is that when the bind function is called (on Windows NT 4.0 with SP4 and later), another application, service, or kernel mode driver is bound to the same address with exclusive access. Such exclusive access is a new feature of Windows NT 4.0 with SP4 and later, and is implemented by using the SO_EXCLUSIVEADDRUSE option.\n");
		break;
	case WSAEFAULT:
		printf("WSAEFAULT\n");
		printf("Bad address.\n");
		printf("The system detected an invalid pointer address in attempting to use a pointer argument of a call. This error occurs if an application passes an invalid pointer value, or if the length of the buffer is too small. For instance, if the length of an argument, which is a sockaddr structure, is smaller than the sizeof(sockaddr).\n");
		break;
	case WSAEINVAL:
		printf("WSAEINVAL\n");
		printf("Invalid argument.\n");
		printf("Some invalid argument was supplied (for example, specifying an invalid level to the setsockopt function). In some instances, it also refers to the current state of the socket—for instance, calling accept on a socket that is not listening.\n");
		break;
	case WSAEMFILE:
		printf("WSAEMFILE\n");
		printf("Too many open files.\n");
		printf("Too many open sockets. Each implementation may have a maximum number of socket handles available, either globally, per process, or per thread.\n");
		break;
	case WSAEWOULDBLOCK:
		printf("WSAEWOULDBLOCK\n");
		printf("Resource temporarily unavailable.\n");
		printf("This error is returned from operations on nonblocking sockets that cannot be completed immediately, for example recv when no data is queued to be read from the socket. It is a nonfatal error, and the operation should be retried later. It is normal for WSAEWOULDBLOCK to be reported as the result from calling connect on a nonblocking SOCK_STREAM socket, since some time must elapse for the connection to be established.\n");
		break;
	case WSAEINPROGRESS:
		printf("WSAEINPROGRESS\n");
		printf("Operation now in progress.\n");
		printf("A blocking operation is currently executing. Windows Sockets only allows a single blocking operation—per- task or thread—to be outstanding, and if any other function call is made (whether or not it references that or any other socket) the function fails with the WSAEINPROGRESS error.\n");
		break;
	case WSAEALREADY:
		printf("WSAEALREADY\n");
		printf("Operation already in progress.\n");
		printf("An operation was attempted on a nonblocking socket with an operation already in progress—that is, calling connect a second time on a nonblocking socket that is already connecting, or canceling an asynchronous request (WSAAsyncGetXbyY) that has already been canceled or completed.\n");
		break;
	case WSAENOTSOCK:
		printf("WSAENOTSOCK\n");
		printf("Socket operation on nonsocket.\n");
		printf("An operation was attempted on something that is not a socket. Either the socket handle parameter did not reference a valid socket, or for select, a member of an fd_set was not valid.\n");
		break;
	case WSAEDESTADDRREQ:
		printf("WSAEDESTADDRREQ\n");
		printf("Destination address required.\n");
		printf("A required address was omitted from an operation on a socket. For example, this error is returned if sendto is called with the remote address of ADDR_ANY.\n");
		break;
	case WSAEMSGSIZE:
		printf("WSAEMSGSIZE\n");
		printf("Message too long.\n");
		printf("A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram was smaller than the datagram itself.\n");
		break;
	case WSAEPROTOTYPE:
		printf("WSAEPROTOTYPE\n");
		printf("Protocol wrong type for socket.\n");
		printf("A protocol was specified in the socket function call that does not support the semantics of the socket type requested. For example, the ARPA Internet UDP protocol cannot be specified with a socket type of SOCK_STREAM.\n");
		break;
	case WSAENOPROTOOPT:
		printf("WSAENOPROTOOPT\n");
		printf("Bad protocol option.\n");
		printf("An unknown, invalid or unsupported option or level was specified in a getsockopt or setsockopt call.\n");
		break;
	case WSAEPROTONOSUPPORT:
		printf("WSAEPROTONOSUPPORT\n");
		printf("Protocol not supported.\n");
		printf("The requested protocol has not been configured into the system, or no implementation for it exists. For example, a socket call requests a SOCK_DGRAM socket, but specifies a stream protocol.\n");
		break;
	case WSAESOCKTNOSUPPORT:
		printf("WSAESOCKTNOSUPPORT\n");
		printf("Socket type not supported.\n");
		printf("The support for the specified socket type does not exist in this address family. For example, the optional type SOCK_RAW might be selected in a socket call, and the implementation does not support SOCK_RAW sockets at all.\n");
		break;
	case WSAEOPNOTSUPP:
		printf("WSAEOPNOTSUPP\n");
		printf("Operation not supported.\n");
		printf("The attempted operation is not supported for the type of object referenced. Usually this occurs when a socket descriptor to a socket that cannot support this operation is trying to accept a connection on a datagram socket.\n");
		break;
	case WSAEPFNOSUPPORT:
		printf("WSAEPFNOSUPPORT\n");
		printf("Protocol family not supported.\n");
		printf("The protocol family has not been configured into the system or no implementation for it exists. This message has a slightly different meaning from WSAEAFNOSUPPORT. However, it is interchangeable in most cases, and all Windows Sockets functions that return one of these messages also specify WSAEAFNOSUPPORT.\n");
		break;
	case WSAEAFNOSUPPORT:
		printf("WSAEAFNOSUPPORT\n");
		printf("Address family not supported by protocol family.\n");
		printf("An address incompatible with the requested protocol was used. All sockets are created with an associated address family (that is, AF_INET for Internet Protocols) and a generic protocol type (that is, SOCK_STREAM). This error is returned if an incorrect protocol is explicitly requested in the socket call, or if an address of the wrong family is used for a socket, for example, in sendto.\n");
		break;
	case WSAEADDRINUSE:
		printf("WSAEADDRINUSE\n");
		printf("Address already in use.\n");
		printf("Typically, only one usage of each socket address (protocol/IP address/port) is permitted. This error occurs if an application attempts to bind a socket to an IP address/port that has already been used for an existing socket, or a socket that was not closed properly, or one that is still in the process of closing. For server applications that need to bind multiple sockets to the same port number, consider using setsockopt (SO_REUSEADDR). Client applications usually need not call bind at all— connect chooses an unused port automatically. When bind is called with a wildcard address (involving ADDR_ANY), a WSAEADDRINUSE error could be delayed until the specific address is committed. This could happen with a call to another function later, including connect, listen, WSAConnect, or WSAJoinLeaf.\n");
		break;
	case WSAEADDRNOTAVAIL:
		printf("WSAEADDRNOTAVAIL\n");
		printf("Cannot assign requested address.\n");
		printf("The requested address is not valid in its context. This normally results from an attempt to bind to an address that is not valid for the local computer. This can also result from connect, sendto, WSAConnect, WSAJoinLeaf, or WSASendTo when the remote address or port is not valid for a remote computer (for example, address or port 0).\n");
		break;
	case WSAENETDOWN:
		printf("WSAENETDOWN\n");
		printf("Network is down.\n");
		printf("A socket operation encountered a dead network. This could indicate a serious failure of the network system (that is, the protocol stack that the Windows Sockets DLL runs over), the network interface, or the local network itself.\n");
		break;
	case WSAENETUNREACH:
		printf("WSAENETUNREACH\n");
		printf("Network is unreachable.\n");
		printf("A socket operation was attempted to an unreachable network. This usually means the local software knows no route to reach the remote host.\n");
		break;
	case WSAENETRESET:
		printf("WSAENETRESET\n");
		printf("Network dropped connection on reset.\n");
		printf("The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. It can also be returned by setsockopt if an attempt is made to set SO_KEEPALIVE on a connection that has already failed.\n");
		break;
	case WSAECONNABORTED:
		printf("WSAECONNABORTED\n");
		printf("Software caused connection abort.\n");
		printf("An established connection was aborted by the software in your host computer, possibly due to a data transmission time-out or protocol error.\n");
		break;
	case WSAECONNRESET:
		printf("WSAECONNRESET\n");
		printf("Connection reset by peer.\n");
		printf("An existing connection was forcibly closed by the remote host. This normally results if the peer application on the remote host is suddenly stopped, the host is rebooted, the host or remote network interface is disabled, or the remote host uses a hard close (see setsockopt for more information on the SO_LINGER option on the remote socket). This error may also result if a connection was broken due to keep-alive activity detecting a failure while one or more operations are in progress. Operations that were in progress fail with WSAENETRESET. Subsequent operations fail with WSAECONNRESET.\n");
		break;
	case WSAENOBUFS:
		printf("WSAENOBUFS\n");
		printf("No buffer space available.\n");
		printf("An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.\n");
		break;
	case WSAEISCONN:
		printf("WSAEISCONN\n");
		printf("Socket is already connected.\n");
		printf("A connect request was made on an already-connected socket. Some implementations also return this error if sendto is called on a connected SOCK_DGRAM socket (for SOCK_STREAM sockets, the to parameter in sendto is ignored) although other implementations treat this as a legal occurrence.\n");
		break;
	case WSAENOTCONN:
		printf("WSAENOTCONN\n");
		printf("Socket is not connected.\n");
		printf("A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using sendto) no address was supplied. Any other type of operation might also return this error—for example, setsockopt setting SO_KEEPALIVE if the connection has been reset.\n");
		break;
	case WSAESHUTDOWN:
		printf("WSAESHUTDOWN\n");
		printf("Cannot send after socket shutdown.\n");
		printf("A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call. By calling shutdown a partial close of a socket is requested, which is a signal that sending or receiving, or both have been discontinued.\n");
		break;
	case WSAETOOMANYREFS:
		printf("WSAETOOMANYREFS\n");
		printf("Too many references.\n");
		printf("Too many references to some kernel object.\n");
		break;
	case WSAETIMEDOUT:
		printf("WSAETIMEDOUT\n");
		printf("Connection timed out.\n");
		printf("A connection attempt failed because the connected party did not properly respond after a period of time, or the established connection failed because the connected host has failed to respond.\n");
		break;
	case WSAECONNREFUSED:
		printf("WSAECONNREFUSED\n");
		printf("Connection refused.\n");
		printf("No connection could be made because the target computer actively refused it. This usually results from trying to connect to a service that is inactive on the foreign host—that is, one with no server application running.\n");
		break;
	case WSAELOOP:
		printf("WSAELOOP\n");
		printf("Cannot translate name.\n");
		printf("Cannot translate a name.\n");
		break;
	case WSAENAMETOOLONG:
		printf("WSAENAMETOOLONG\n");
		printf("Name too long.\n");
		printf("A name component or a name was too long.\n");
		break;
	case WSAEHOSTDOWN:
		printf("WSAEHOSTDOWN\n");
		printf("Host is down.\n");
		printf("A socket operation failed because the destination host is down. A socket operation encountered a dead host. Networking activity on the local host has not been initiated. These conditions are more likely to be indicated by the error WSAETIMEDOUT.\n");
		break;
	case WSAEHOSTUNREACH:
		printf("WSAEHOSTUNREACH\n");
		printf("No route to host.\n");
		printf("A socket operation was attempted to an unreachable host. See WSAENETUNREACH.\n");
		break;
	case WSAENOTEMPTY:
		printf("WSAENOTEMPTY\n");
		printf("Directory not empty.\n");
		printf("Cannot remove a directory that is not empty.\n");
		break;
	case WSAEPROCLIM:
		printf("WSAEPROCLIM\n");
		printf("Too many processes.\n");
		printf("A Windows Sockets implementation may have a limit on the number of applications that can use it simultaneously. WSAStartup may fail with this error if the limit has been reached.\n");
		break;
	case WSAEUSERS:
		printf("WSAEUSERS\n");
		printf("User quota exceeded.\n");
		printf("Ran out of user quota.\n");
		break;
	case WSAEDQUOT:
		printf("WSAEDQUOT\n");
		printf("Disk quota exceeded.\n");
		printf("Ran out of disk quota.\n");
		break;
	case WSAESTALE:
		printf("WSAESTALE\n");
		printf("Stale file handle reference.\n");
		printf("The file handle reference is no longer available.\n");
		break;
	case WSAEREMOTE:
		printf("WSAEREMOTE\n");
		printf("Item is remote.\n");
		printf("The item is not available locally.\n");
		break;
	case WSASYSNOTREADY:
		printf("WSASYSNOTREADY\n");
		printf("Network subsystem is unavailable.\n");
		printf("This error is returned by WSAStartup if the Windows Sockets implementation cannot function at this time because the underlying system it uses to provide network services is currently unavailable. Users should check:\n");
		break;
	default:
		printf("Unknown winsock error.....\n");
		//assert(false);
		break;
	}
}