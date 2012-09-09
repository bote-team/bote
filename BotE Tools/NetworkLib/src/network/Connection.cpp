#include "StdAfx.h"
#include "Connection.h"

namespace network
{
	IMPLEMENT_DYNAMIC(CConnection, CThread)

	CConnection::CConnection(SOCKET hSocket, UINT nVersion, CConnectionListener *pListener)
		: CThread(TRUE), m_hSocket(hSocket), m_bSendLost(true), m_pListener(pListener), m_nVersion(nVersion),
		m_dwRemoteIP(0), m_nRemotePort(0)
	{
		ASSERT(m_hSocket != INVALID_SOCKET);

		// IP und Port des anderen Endes ermitteln, merken
		CAsyncSocket socket;
		socket.Attach(m_hSocket);

		SOCKADDR_IN in_addr;
		memset(&in_addr, 0, sizeof(in_addr));
		int len = sizeof(in_addr);
		socket.GetPeerName((SOCKADDR *)&in_addr, &len);
		m_dwRemoteIP = ntohl(in_addr.sin_addr.S_un.S_addr);
		m_nRemotePort = ntohs(in_addr.sin_port);

		socket.Detach();

		// Thread starten
		Start();
	}

	CConnection::~CConnection()
	{
		// verbleibende Pakete entfernen (ohne die Listeners zu informieren)
		packets.RemoveAll();
	}

	BOOL CConnection::Send(const CReference<CPacket> &packet)
	{
		if (IsInterrupted()) return FALSE;

		CSingleLock singleLock(&m_CriticalSection);
		singleLock.Lock();

		// Paket an Liste anhängen
		packets.AddTail(packet);
		return TRUE;
	}

/*	BOOL CConnection::ReSend(const CReference<CPacket> &packet)
	{
		if (IsInterrupted()) return FALSE;

		CSingleLock singleLock(&m_CriticalSection);
		singleLock.Lock();

		packets.AddHead(packet);
		return TRUE;
	} */

	CReferenceTransfer<CPacket> CConnection::GetNextPacket()
	{
		CSingleLock singleLock(&m_CriticalSection);
		singleLock.Lock();

		if (packets.GetSize() > 0)
			return CReferenceTransfer<CPacket>(packets.RemoveHead());
		else
			return NULL;
	}

	void CConnection::Run()
	{
		VERIFY(AfxSocketInit());

		CAsyncSocket socket;
		socket.Attach(m_hSocket);

		while (!IsInterrupted())
		{
			// Paket empfangen
			int nError = 0;
			BOOL bClosed = FALSE;
			CReference<CPacket> packet(CPacket::Receive(socket, nError, bClosed));
			if (packet)
			{
				if (m_pListener) m_pListener->OnPacketReceived(packet, this);
				packet = NULL;
			}
			else if (nError)
			{
				// WSAEWOULDBLOCK: es wurde nichts empfangen
				if (nError != WSAEWOULDBLOCK)
				{
					if (m_pListener) m_pListener->OnSocketError(nError, this);
					__super::Interrupt();
					break;
				}
			}
			else if (bClosed)
			{
				__super::Interrupt();
				break;
			}
			else
			{
				ASSERT(FALSE);
			}

			// Paket versenden
			if (packet = GetNextPacket())
			{
				BOOL bResult = packet->Send(socket, nError);
				if (bResult)
				{
					if (m_pListener) m_pListener->OnPacketSent(packet, this);
				}
				else if (nError)
				{
					// wenn ein Timeout auftritt, ist nError == WSAEWOULDBLOCK;
					// sollte im Normalfall nicht passieren

					if (m_pListener)
					{
						m_pListener->OnSocketError(nError, this);
						m_pListener->OnPacketDiscarded(packet, this);
					}
					__super::Interrupt();
					break;
				}
			}

			Sleep(50);
		}

		// verbleibende zu sendende Paketobjekte zerstören
		CReference<CPacket> packet;
		while (packet = GetNextPacket())
		{
			if (m_pListener) m_pListener->OnPacketDiscarded(packet, this);
		}

		socket.Close();
		if (m_bSendLost && m_pListener) m_pListener->OnConnectionLost(this);

		return;
	}

	void CConnection::Interrupt()
	{
		__super::Interrupt();
		m_bSendLost = false;
	}

	void CConnection::GetRemoteAddr(DWORD &dwIP, UINT &nPort) const
	{
		dwIP = m_dwRemoteIP;
		nPort = m_nRemotePort;
	}

};
