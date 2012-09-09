#include "StdAfx.h"
#include "BotEClient.h"

#include "network.h"
#include "RequestRace.h"
#include "EndOfRound.h"

#pragma warning (disable: 4267 4018)

/// ruft die Funktion <code>FN</code> (evtl. inkl. Parameter) aller registrierter Listeners auf
#define TO_ALL_LISTENERS(FN) \
	do { for (POSITION pos = m_lListeners.GetHeadPosition(); pos; m_lListeners.GetNext(pos)) \
	m_lListeners.GetAt(pos)->FN; } while (false)


network::CBotEClient client;

namespace network
{
	CBotEClient::CBotEClient() : m_nState(CLT_STATE_DISCONNECTED), m_pConnection(NULL),
		m_nClientRace(RACE_NONE), m_strClientUserName("")
	{
		memset(m_pPlayers, 0, RACE_COUNT * sizeof(*m_pPlayers));		
	}

	CBotEClient::~CBotEClient()
	{
		Disconnect();
		Sleep(100);
	}

	void CBotEClient::AddClientListener(CClientListener *pListener)
	{
		if (pListener && !m_lListeners.Find(pListener)) m_lListeners.AddTail(pListener);
	}

	void CBotEClient::RemoveClientListener(CClientListener *pListener)
	{
		POSITION pos = m_lListeners.Find(pListener);
		if (pos) m_lListeners.RemoveAt(pos);
	}

	void CBotEClient::OnSocketError(int nError)
	{
		TRACE("client: %s\n", network::GetWSAErrorMsg(nError));
		TO_ALL_LISTENERS(OnClientSocketError(nError));
	}

	BOOL CBotEClient::Connect(DWORD dwAddr, int nPort, const CString &strUserName)
	{
		if (m_nState != CLT_STATE_DISCONNECTED) return FALSE;
		ASSERT(m_pConnection == NULL);

		// Socket erzeugen
		CSocket socket;
		if (!socket.Create()) goto error;

		// mit angegebener Adresse/Port verbinden
		SOCKADDR_IN in_addr;
		memset(&in_addr, 0, sizeof(in_addr));
		in_addr.sin_family = AF_INET;
		in_addr.sin_port = htons(nPort);
		in_addr.sin_addr.S_un.S_addr = htonl(dwAddr);

		TRACE("client: connecting to server %s:%u...\n", AddrToString(dwAddr), nPort);

		if (!socket.Connect((SOCKADDR *)&in_addr, sizeof(in_addr))) goto error;

		// Zustand schonmal wechseln, da sofort nach dem Handshake eine CChosenRaces-Nachricht
		// vom Server eintrifft und dieser Thread vor dem Setzen des neuen Zustands unterbrochen
		// werden kˆnnte
		m_nState = CLT_STATE_PREPAREGAME;
		m_strClientUserName = strUserName;

		int nError;
		if (!DoClientHandshake(socket, nError, strUserName))
		{
			m_nState = CLT_STATE_DISCONNECTED; // im Fehlerfall Zustand zur¸cksetzen
			m_strClientUserName = "";
			if (nError) OnSocketError(nError);
			else {TRACE("client: connection interrupted\n");}
			return FALSE;
		}
		
		// Listeners informieren, Erfolg zur¸ckgeben
	//	TO_ALL_LISTENERS(OnClientConnected());
		return TRUE;

	error:
		OnSocketError(socket.GetLastError());
		return FALSE;
	}

	void CBotEClient::Disconnect()
	{
		if (m_nState == CLT_STATE_DISCONNECTED) return;
		ASSERT(m_pConnection != NULL);

		m_pConnection->Interrupt();
	//	m_pConnection->Join();
	//	delete m_pConnection;
		m_pConnection = NULL;
		m_strClientUserName = "";
		m_nState = CLT_STATE_DISCONNECTED;

		TRACE("client: disconnected\n");
	//	TO_ALL_LISTENERS(OnClientDisconnected());
	}

	BOOL CBotEClient::RequestData(CAsyncSocket &socket, UINT nCount, clock_t clkDeadline)
	{
		while (true)
		{
			DWORD dwAvailable = 0;
			if (!socket.IOCtl(FIONREAD, &dwAvailable))
			{
				client.OnSocketError(socket.GetLastError());
				return FALSE;
			}
			if (dwAvailable >= nCount) return TRUE;
			if (clock() > clkDeadline) return FALSE;
		}
	}

	BOOL CBotEClient::DoClientHandshake(CSocket &socket, int &nError, const CString &strUserName)
	{
		nError = 0;
		BYTE *lpBuf = NULL;

		// Client-ID senden
		int nDone = socket.Send(HANDSHAKE_CLIENTID, strlen(HANDSHAKE_CLIENTID));
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < strlen(HANDSHAKE_CLIENTID)) goto error;

		// (1.) Versionsnummer vorbereiten, senden
		int nCount = 1 + sizeof(UINT) + 1;
		lpBuf = new BYTE[nCount];

		lpBuf[0] = lpBuf[nCount - 1] = 0;
		UINT nClientVersion = CLIENT_VERSION;
		memcpy(&lpBuf[1], &nClientVersion, sizeof(UINT));

		nDone = socket.Send(lpBuf, nCount);
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < nCount) goto error;

		delete[] lpBuf;
		lpBuf = NULL;

		// (2.) ID vom Server abwarten
		clock_t end = clock() + HANDSHAKE_TIMEOUT * CLOCKS_PER_SEC;
		nCount = strlen(HANDSHAKE_SERVERID) + 1 + sizeof(UINT) + 1;
		if (!RequestData(socket, nCount, end)) goto error;

		// Daten in Puffer ¸bertragen
		lpBuf = new BYTE[nCount];
		nDone = socket.Receive(lpBuf, nCount);
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < nCount) goto error; // schlieﬂt nDone == 0 mit ein

		// Pufferinhalt pr¸fen
		if (memcmp(lpBuf, HANDSHAKE_SERVERID, strlen(HANDSHAKE_SERVERID)) != 0
			|| lpBuf[strlen(HANDSHAKE_SERVERID)] != 0
			|| lpBuf[nCount - 1] != 0)
		{
			goto error;
		}

		// Server-Version extrahieren
		UINT nServerVersion = 0;
		memcpy(&nServerVersion, lpBuf + (strlen(HANDSHAKE_SERVERID) + 1), sizeof(UINT));
		delete[] lpBuf;
		lpBuf = NULL;
		TRACE("client: connected, server has version %u\n", nServerVersion);

		// (3.) Best‰tigung + Benutzername senden
		UINT nLen = 3 + sizeof(UINT) + strUserName.GetLength();
		lpBuf = new BYTE[nLen];
		BYTE *p = lpBuf;
		memcpy(p, "OK\0", 3);
		p += 3;
		UINT nStrLen = strUserName.GetLength();
		memcpy(p, &nStrLen, sizeof(UINT));
		p += sizeof(UINT);
		for (int i = 0; i < nStrLen; i++)
		{
			char c = strUserName.GetAt(i);
			memcpy(p, &c, 1);
			p++;
		}

		nDone = socket.Send(lpBuf, nLen);
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < nLen) goto error;
		delete[] lpBuf;
		lpBuf = NULL;

		// CConnection-Objekt erzeugen
		ASSERT(m_pConnection == NULL);
		UINT nVersion = min(nServerVersion, nClientVersion);
		m_pConnection = new CConnection(socket.Detach(), nVersion, this);
		TRACE("client: server verified, negotiated version %u\n", nVersion);

		return TRUE;

	sockerror:
		nError = socket.GetLastError();
	error:
		if (lpBuf) delete[] lpBuf;
		return FALSE;
	}

	void CBotEClient::GetRemoteAddr(DWORD &dwIP, UINT &nPort)
	{
		if (m_nState == CLT_STATE_DISCONNECTED) return;
		ASSERT(m_pConnection);

		m_pConnection->GetRemoteAddr(dwIP, nPort);
	}

	void CBotEClient::RequestRace(RACE nRace)
	{
		ASSERT(m_nState == CLT_STATE_PREPAREGAME);

		if (nRace >= RACE_LAST) return;

		CRequestRace msg(nRace);
		Send(m_pConnection, &msg);
	}

	void CBotEClient::Send(CConnection *pConnection, CMessage *pMessage)
	{
		if (!pConnection || !pMessage)
		{
			ASSERT(FALSE);
			return;
		}

		VERIFY(pConnection->Send(new CPacket(pMessage)));
	}

	void CBotEClient::OnPacketReceived(const CReference<CPacket> &packet, CConnection *pConnection)
	{
		ASSERT(packet && pConnection);

		CMessage *pMsg = packet->GetMessage();
		if (!pMsg)
		{
			TRACE("client: packet received, unknown message type\n");
			return;
		}

		// TRACE("client: packet received, message ID %u\n", pMsg->GetID());

		switch (pMsg->GetID())
		{
		case MSGID_CHOSENRACES:
			ASSERT(m_nState == CLT_STATE_PREPAREGAME);
			HandleChosenRaces((CChosenRaces *)pMsg);
			break;

		case MSGID_BEGINGAME:
			ASSERT(m_nState == CLT_STATE_PREPAREGAME);
			HandleBeginGame((CBeginGame *)pMsg);
			break;

		case MSGID_NEXTROUND:
			ASSERT(m_nState == CLT_STATE_WAITFORDATA);
			HandleNextRound((CNextRound *)pMsg);
			break;

		case MSGID_CHAT:
			ASSERT(m_nState != CLT_STATE_DISCONNECTED);
			HandleChatMsg((CChatMsg *)pMsg);
			break;

		default:
			TRACE("client: unhandled message\n");
			ASSERT(FALSE);
		}

		delete pMsg;
	}

	void CBotEClient::OnPacketSent(const CReference<CPacket> &packet, CConnection *pConnection)
	{
		ASSERT(packet && pConnection);
		// TRACE("client: packet sent\n");
	}

	void CBotEClient::OnSocketError(int nWSAError, CConnection *pConnection)
	{
		TRACE("client: socket error %d\n", nWSAError);

		// Listeners benachrichtigen
		OnSocketError(nWSAError);

		// die Verbindung wird anschlieﬂend getrennt; es wird OnConnectionLost() aufgerufen
	}

	void CBotEClient::OnPacketDiscarded(const CReference<CPacket> &packet, CConnection *pConnection)
	{
		ASSERT(packet && pConnection);
		TRACE("client: packet discarded\n");
	}

	void CBotEClient::OnConnectionLost(CConnection *pConnection)
	{
		ASSERT(pConnection);
		TRACE("client: connection to server lost\n");
		Disconnect();
		TO_ALL_LISTENERS(OnClientDisconnected());
	}

	void CBotEClient::HandleChosenRaces(CChosenRaces *pMsg)
	{
		if (m_nState != CLT_STATE_PREPAREGAME) return;

		// das diesem Client zugeordnete Volk merken
		m_nClientRace = pMsg->GetClientRace();

		// merken, welches Volk mitspielt und welches davon von einem Client oder vom Server
		// gespielt wird
		for (int i = 0; i < RACE_COUNT; i++)
		{
			RACE race = (RACE)(RACE_FIRST + i);
			m_pstrUserNames[i] = "";
			if (!pMsg->IsPlayer(race))
				m_pPlayers[i] = PLAYER_NONE;
			else if (pMsg->IsPlayedByClient(race))
			{
				m_pPlayers[i] = PLAYER_CLIENT;
				m_pstrUserNames[i] = pMsg->GetUserName(race);
			}
			else if (pMsg->IsPlayedByServer(race))
				m_pPlayers[i] = PLAYER_SERVER;
			else
			{
				ASSERT(FALSE);
				m_pPlayers[i] = PLAYER_NONE;
			}
		}

		TO_ALL_LISTENERS(OnClientChangedRaces());
	}

	BOOL CBotEClient::IsPlayer(RACE nRace) const
	{
		if (nRace < RACE_FIRST || nRace >= RACE_LAST) return FALSE;
		return (m_pPlayers[nRace - RACE_FIRST] != RACE_NONE);
	}

	void CBotEClient::HandleBeginGame(CBeginGame *pMsg)
	{
		if (m_nState != CLT_STATE_PREPAREGAME) return;
		ASSERT(m_nClientRace != RACE_NONE);
		m_nState = CLT_STATE_WAITFORDATA;
		TRACE("client: OnBeginGame()\n");

		TO_ALL_LISTENERS(OnBeginGame(pMsg));
	}

	void CBotEClient::HandleNextRound(CNextRound *pMsg)
	{
		ASSERT(pMsg);
		if (m_nState != CLT_STATE_WAITFORDATA) return;
		m_nState = CLT_STATE_PLAYING;
		TRACE("client: OnNextRound()\n");

		for (int i = 0; i < RACE_COUNT; i++)
		{
			RACE race = (RACE)(RACE_FIRST + i);
			if (!pMsg->IsPlayer(race))
			{
				m_pPlayers[i] = PLAYER_NONE;
				m_pstrUserNames[i] = "";
			}
			else if (pMsg->IsPlayedByClient(race))
				m_pPlayers[i] = PLAYER_CLIENT;
			else if (pMsg->IsPlayedByServer(race))
			{
				m_pPlayers[i] = PLAYER_SERVER;
				m_pstrUserNames[i] = "";
			}
			else
			{
				ASSERT(FALSE);
				m_pPlayers[i] = PLAYER_NONE;
				m_pstrUserNames[i] = "";
			}
		}

		TO_ALL_LISTENERS(OnNextRound(pMsg));
	}

	void CBotEClient::EndOfRound(CPeerData *pDoc)
	{
		ASSERT(pDoc != NULL && m_nState == CLT_STATE_PLAYING);
		if (m_nState != CLT_STATE_PLAYING || !pDoc) return;

		TRACE("client: EndOfRound()\n");

		CEndOfRound msg(pDoc);
		Send(m_pConnection, &msg);
		m_nState = CLT_STATE_WAITFORDATA;
	}

	void CBotEClient::HandleChatMsg(CChatMsg *pMsg)
	{
		ASSERT(pMsg);
		if (m_nState != CLT_STATE_PREPAREGAME && m_nState != CLT_STATE_WAITFORDATA
			&& m_nState != CLT_STATE_PLAYING)
		{
			return;
		}

		TRACE("client: OnChatMsg()\n");
		TO_ALL_LISTENERS(OnChatMsg(pMsg));
	}

	void CBotEClient::Chat(CString message)
	{
		if (m_nState == CLT_STATE_DISCONNECTED) return;
		
		TRACE("client: Chat() broadcast\n");
		CChatMsg msg(message);
		msg.AddReceiver(RACE_ALL);
		Send(m_pConnection, &msg);
	}

	void CBotEClient::Chat(CString message, BOOL bIsReceiver[RACE_COUNT])
	{
		if (m_nState == CLT_STATE_DISCONNECTED) return;
		
		TRACE("client: Chat()\n");

		// Empf‰nger setzen
		CChatMsg msg(message);
		for (int i = 0; i < RACE_COUNT; i++)
			if (bIsReceiver[i])
				msg.AddReceiver((RACE)(RACE_FIRST + i));
		// abbrechen, wenn keine Empf‰nger oder nur Client selbst der Empf‰nger ist
		if (!msg.GetReceiverCount() || msg.GetReceiver(0) == m_nClientRace)
			return;

		// an Server senden
		Send(m_pConnection, &msg);
	}

	BOOL CBotEClient::IsPlayedByClient(RACE nRace) const
	{
		if (nRace < RACE_FIRST || nRace >= RACE_LAST)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		return (m_pPlayers[nRace - RACE_FIRST] == PLAYER_CLIENT);
	}

	BOOL CBotEClient::IsPlayedByServer(RACE nRace) const
	{
		if (nRace < RACE_FIRST || nRace >= RACE_LAST)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		return (m_pPlayers[nRace - RACE_FIRST] == PLAYER_SERVER);
	}

	CString CBotEClient::GetUserName(RACE nRace) const
	{
		if (nRace < RACE_FIRST || nRace >= RACE_LAST)
		{
			ASSERT(FALSE);
			return "";
		}
		return m_pstrUserNames[nRace - RACE_FIRST];
	}

};
