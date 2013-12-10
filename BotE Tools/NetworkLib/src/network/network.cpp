
#include "stdafx.h"
#include "network.h"

namespace network
{
	CString GetWSAErrorMsg(int nError)
	{
		CString msg;

		switch (nError)
		{
		case WSAEINTR:					msg = "Interrupted function call"; break;
		case WSAEACCES:					msg = "Permission denied"; break;
		case WSAEFAULT:					msg = "Bad address"; break;
		case WSAEINVAL:					msg = "Invalid argument"; break;
		case WSAEMFILE:					msg = "Too many open files"; break;
		case WSAEWOULDBLOCK:			msg = "Resource temporarily unavailable"; break;
		case WSAEINPROGRESS:			msg = "Operation now in progress"; break;
		case WSAEALREADY:				msg = "Operation already in progress"; break;
		case WSAENOTSOCK:				msg = "Socket operation on nonsocket"; break;
		case WSAEDESTADDRREQ:			msg = "Destination address required"; break;
		case WSAEMSGSIZE:				msg = "Message too long"; break;
		case WSAEPROTOTYPE:				msg = "Protocol wrong type for socket"; break;
		case WSAENOPROTOOPT:			msg = "Bad protocol option"; break;
		case WSAEPROTONOSUPPORT:		msg = "Protocol not supported"; break;
		case WSAESOCKTNOSUPPORT:		msg = "Socket type not supported"; break;
		case WSAEOPNOTSUPP:				msg = "Operation not supported"; break;
		case WSAEPFNOSUPPORT:			msg = "Protocol family not supported"; break;
		case WSAEAFNOSUPPORT:			msg = "Address family not supported by protocol family"; break;
		case WSAEADDRINUSE:				msg = "Address already in use"; break;
		case WSAEADDRNOTAVAIL:			msg = "Cannot assign requested address"; break;
		case WSAENETDOWN:				msg = "Network is down"; break;
		case WSAENETUNREACH:			msg = "Network is unreachable"; break;
		case WSAENETRESET:				msg = "Network dropped connection on reset"; break;
		case WSAECONNABORTED:			msg = "Software caused connection abort"; break;
		case WSAECONNRESET:				msg = "Connection reset by peer"; break;
		case WSAENOBUFS:				msg = "No buffer space available"; break;
		case WSAEISCONN:				msg = "Socket is already connected"; break;
		case WSAENOTCONN:				msg = "Socket is not connected"; break;
		case WSAESHUTDOWN:				msg = "Cannot send after socket shutdown"; break;
		case WSAETIMEDOUT:				msg = "Connection timed out"; break;
		case WSAECONNREFUSED:			msg = "Connection refused"; break;
		case WSAEHOSTDOWN:				msg = "Host is down"; break;
		case WSAEHOSTUNREACH:			msg = "No route to host"; break;
		case WSAEPROCLIM:				msg = "Too many processes"; break;
		case WSASYSNOTREADY:			msg = "Network subsystem is unavailable"; break;
		case WSAVERNOTSUPPORTED:		msg = "Winsock.dll version out of range"; break;
		case WSANOTINITIALISED:			msg = "Successful WSAStartup not yet performed"; break;
		case WSAEDISCON:				msg = "Graceful shutdown in progress"; break;
		case WSATYPE_NOT_FOUND:			msg = "Class type not found"; break;
		case WSAHOST_NOT_FOUND:			msg = "Host not found"; break;
		case WSATRY_AGAIN:				msg = "Nonauthoritative host not found"; break;
		case WSANO_RECOVERY:			msg = "This is a nonrecoverable error"; break;
		case WSANO_DATA:				msg = "Valid name, no data record of requested type"; break;
		case WSA_INVALID_HANDLE:		msg = "Specified event object handle is invalid"; break;
		case WSA_INVALID_PARAMETER:		msg = "One or more parameters are invalid"; break;
		case WSA_IO_INCOMPLETE:			msg = "Overlapped I/O event object not in signaled state"; break;
		case WSA_IO_PENDING:			msg = "Overlapped operations will complete later"; break;
		case WSA_NOT_ENOUGH_MEMORY:		msg = "Insufficient memory available"; break;
		case WSA_OPERATION_ABORTED:		msg = "Overlapped operation aborted"; break;
	//	case WSAINVALIDPROCTABLE:		msg = "Invalid procedure table from service provider"; break;
	//	case WSAINVALIDPROVIDER:		msg = "Invalid service provider version number"; break;
	//	case WSAPROVIDERFAILEDINIT:		msg = "Unable to initialize a service provider"; break;
		case WSASYSCALLFAILURE:			msg = "System call failure"; break;

		default:						msg = "unknown error"; break;
		}

		CString s;
		s.Format("socket error %i: %s.", nError, msg);

		return s;
	}

	CString AddrToString(DWORD dwIP)
	{
		CString s;
		s.Format("%u.%u.%u.%u", (dwIP >> 24) & 0xFF, (dwIP >> 16) & 0xFF, (dwIP >> 8) & 0xFF, dwIP & 0xFF);
		return s;
	}
};
