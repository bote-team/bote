#include "StdAfx.h"
#include "LANConnection.h"
#include "network.h"

#pragma warning (disable: 4244)

namespace network
{
	IMPLEMENT_DYNAMIC(CLANConnection, CThread)

	CLANConnection::CLANConnection(UINT nPort, CLANConnectionListener *pListener)
		: CThread(), m_hSocket(INVALID_SOCKET), m_nPort(nPort), m_dwIP(0), m_pListener(pListener),
		m_bSendLost(true)
	{
		ASSERT(nPort <= 65535);
	}

	CLANConnection::~CLANConnection()
	{
		// verbleibende Nachrichten entfernen (ohne die Listeners zu informieren)
		messages.RemoveAll();
	}

	BOOL CLANConnection::Send(const CReference<CLANMessage> &message)
	{
		if (IsInterrupted()) return FALSE;

		CSingleLock singleLock(&m_CriticalSection);
		singleLock.Lock();

		// Nachricht an Liste anhängen
		messages.AddTail(message);
		return TRUE;
	}

	CReferenceTransfer<CLANMessage> CLANConnection::GetNextMessage()
	{
		CSingleLock singleLock(&m_CriticalSection);
		singleLock.Lock();

		if (messages.GetSize() > 0)
			return CReferenceTransfer<CLANMessage>(messages.RemoveHead());
		else
			return NULL;
	}

	BOOL CLANConnection::CreateSocket(int &nError)
	{
		nError = 0;
		if (m_hSocket != INVALID_SOCKET) return TRUE;

		// Socket erzeugen
		CAsyncSocket socket;
		if (!socket.Socket(SOCK_DGRAM, 0)) goto error;

/*		// mehrfaches Binden an selben Port erlauben
		BOOL bReuseAddr = TRUE;
		if (!socket.SetSockOpt(SO_REUSEADDR, &bReuseAddr, sizeof(bReuseAddr)))
			goto error; */

		// Socket binden
		if (!socket.Bind(m_nPort))	goto error;

		// Broadcast erlauben
		BOOL bBroadcast = TRUE;
		if (!socket.SetSockOpt(SO_BROADCAST, &bBroadcast, sizeof(bBroadcast)))
			goto error;

		m_hSocket = socket.Detach();
		return TRUE;

error:
		nError = socket.GetLastError();
		return FALSE;
	}

	void CLANConnection::Run()
	{
		VERIFY(AfxSocketInit());

		// Socket erzeugen, falls noch nicht geschehen
		int nError;
		if (!CreateSocket(nError))
		{
			if (m_pListener)
			{
				m_pListener->OnSocketError(nError, this);
				m_pListener->OnConnectionLost(this);
			}
			return;
		}

		CAsyncSocket socket;
		ASSERT(m_hSocket != INVALID_SOCKET);
		socket.Attach(m_hSocket);

		// IP-Adresse und tatsächlichen Port ermitteln
		SOCKADDR_IN sockaddr;
		memset(&sockaddr, 0, sizeof(sockaddr));
		int nSockAddrLen = sizeof(SOCKADDR_IN);
		if (!socket.GetSockName((SOCKADDR *)&sockaddr, &nSockAddrLen)) goto error;

		m_nPort = ntohs(sockaddr.sin_port);
		m_dwIP = ntohl(sockaddr.sin_addr.S_un.S_addr);

		// main thread loop
		BYTE buf[LAN_BUFSIZE];
		while (!IsInterrupted())
		{
			// Senden
			{
			CReference<CLANMessage> message;
			if (message = GetNextMessage())
			{
				// Magic Number
				memcpy(buf, "BotE", 4);

				// Nachricht serialisieren
				CMemFile memFile(&buf[4], LAN_BUFSIZE - 4);
				CArchive ar(&memFile, CArchive::store);
				message->Serialize(ar);
				ar.Close();
				UINT nSize = memFile.GetPosition() + 4;
				memFile.Detach();

				// Empfänger setzen
				memset(&sockaddr, 0, sizeof(sockaddr));
				sockaddr.sin_family = AF_INET;
				sockaddr.sin_addr.S_un.S_addr = htonl(message->GetReceiverIP());
				sockaddr.sin_port = htons(message->GetReceiverPort());

				// Nachricht versenden, setzt bei Fehler m_bInterrupted
				if (!SendMessageTo(socket, message, buf, nSize, &sockaddr)) break;

				// Broadcast-Nachricht auch an den lokalen Host senden
				if (message->GetReceiverIP() == INADDR_BROADCAST)
				{
					sockaddr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
					if (!SendMessageTo(socket, message, buf, nSize, &sockaddr))
						break;
				}
			}
			}

			// Empfangen
			memset(&sockaddr, 0, sizeof(sockaddr));
			int nSockAddrLen = sizeof(sockaddr);
			int nCount = socket.ReceiveFrom(&buf, LAN_BUFSIZE, (SOCKADDR *)&sockaddr, &nSockAddrLen);
			if (nCount > 0)
			{
				// Magic Number prüfen
				if (memcmp(&buf, "BotE", 4) != 0) continue;

				// Nachricht deserialisieren
				CMemFile memFile(&buf[4], nCount - 4);
				CArchive ar(&memFile, CArchive::load);
				CReference<CLANMessage> message(new CLANMessage());
				message->Serialize(ar);
				ar.Close();
				memFile.Detach();

				// IP und Port des Absenders
				DWORD dwIP = ntohl(sockaddr.sin_addr.S_un.S_addr);
				UINT nPort = ntohs(sockaddr.sin_port);
				message->SetSenderIP(dwIP);
				message->SetSenderPort(nPort);

				// Ankunft einer Nachricht melden
				if (m_pListener) m_pListener->OnMessageReceived(message, this);
			}
			else if (nCount == 0)
			{
				// "Verbindung" wurde getrennt
				ASSERT(FALSE);
				break;
			}
			else if (nCount == SOCKET_ERROR)
			{
				int nError = socket.GetLastError();
				if (nError != WSAEWOULDBLOCK)
				{
					if (m_pListener) m_pListener->OnSocketError(nError, this);
					// WSAECONNRESET tritt auf, wenn wir zuvor eine Nachricht an eine Adresse gesendet
					// haben, an der kein Server läuft; Thread dann nicht abbrechen
					if (nError != WSAECONNRESET)
					{
						__super::Interrupt();
						break;
					}
				}
			}

			Sleep(50);
		}

		// verbleibende Nachrichten löschen
		{
			CReference<CLANMessage> message;
			while (message = GetNextMessage())
			{
				if (m_pListener) m_pListener->OnMessageDiscarded(message, this);
			}
		}

		// socket schließen, Thread beenden
		socket.Close();
		m_hSocket = INVALID_SOCKET;
		if (m_bSendLost && m_pListener) m_pListener->OnConnectionLost(this);
		return;

error:
		if (m_pListener) m_pListener->OnSocketError(socket.GetLastError(), this);
		m_hSocket = INVALID_SOCKET;
		if (m_pListener) m_pListener->OnConnectionLost(this);
	}

	void CLANConnection::Interrupt()
	{
		__super::Interrupt();
		m_bSendLost = false;
	}

	BOOL CLANConnection::SendMessageTo(CAsyncSocket &socket, const CReference<CLANMessage> &msg,
		const BYTE *lpBuf, UINT nSize, SOCKADDR_IN *sockaddr)
	{
		clock_t deadline = clock() + SEND_TIMEOUT * CLOCKS_PER_SEC;

		// Nachricht versenden
		while (true)
		{
			int nCount = socket.SendTo(lpBuf, nSize, (SOCKADDR *)sockaddr, sizeof(*sockaddr));
			if (nCount == nSize)
			{
				if (m_pListener) m_pListener->OnMessageSent(msg, this);
			}
			else if (nCount == SOCKET_ERROR)
			{
				int nError = socket.GetLastError();

				if (nError == WSAEWOULDBLOCK)
				{
					Sleep(50);
					if (clock() <= deadline) continue;

					TRACE("LAN send timeout\n");
				}

				if (m_pListener)
				{
					m_pListener->OnSocketError(nError, this);
					m_pListener->OnMessageDiscarded(msg, this);
				}

				// bei Broadcast-Nachricht, deren Versenden aufgrund fehlender Verbindung zu einem lokalen
				// Netzwerk fehlschlug (WSAEHOSTUNREACH), den Thread nicht unterbrechen
				if (msg->GetReceiverIP() != INADDR_BROADCAST || nError != WSAEHOSTUNREACH)
				{
					__super::Interrupt();
					return FALSE;
				}
			}
			else
			{
				if (m_pListener) m_pListener->OnMessageDiscarded(msg, this);
			}

			break;
		}

		return TRUE;
	}

};
