#include "StdAfx.h"
#include "BotEServer.h"

#include "ChosenRaces.h"
#include "BeginGame.h"
#include "NextRound.h"

/// ruft die Funktion <code>FN</code> (evtl. inkl. Parameter) aller registrierter Listeners auf
#define TO_ALL_LISTENERS(FN) \
	do { for (POSITION pos = m_lListeners.GetHeadPosition(); pos; m_lListeners.GetNext(pos)) \
	m_lListeners.GetAt(pos)->FN; } while (false)

network::CBotEServer server;

namespace network
{
	CBotEServer::CBotEServer() : m_nState(SRV_STATE_STOPPED), m_hSocket(INVALID_SOCKET),
		m_nNextRequestId(0)
	{
		memset(m_pChosenRaces, 0, RACE_COUNT * sizeof(*m_pChosenRaces));
		memset(m_pbPlayByServer, 0, RACE_COUNT * sizeof(*m_pbPlayByServer));

		memset(m_pbNextRoundSent, 0, RACE_COUNT * sizeof(*m_pbNextRoundSent));
		memset(m_pEndOfRoundBuf, 0, RACE_COUNT * sizeof(*m_pEndOfRoundBuf));
		memset(m_pnEndOfRoundSize, 0, RACE_COUNT * sizeof(*m_pnEndOfRoundSize));
	}

	CBotEServer::~CBotEServer()
	{
		Stop();
		DeleteEndOfRoundBufs();
		Sleep(100);
	}

	void CBotEServer::AddServerListener(CServerListener *pListener)
	{
		if (pListener && !m_lListeners.Find(pListener)) m_lListeners.AddTail(pListener);
	}

	void CBotEServer::RemoveServerListener(CServerListener *pListener)
	{
		POSITION pos = m_lListeners.Find(pListener);
		if (pos) m_lListeners.RemoveAt(pos);
	}

	BOOL CBotEServer::Start(UINT nPort)
	{
		if (m_nState != SRV_STATE_STOPPED) return FALSE;
		ASSERT(m_hSocket == INVALID_SOCKET);

		// Socket erzeugen
		CAsyncSocket socket;
		if (!socket.Create(nPort) || !socket.Listen())
		{
			OnSocketError(socket.GetLastError());
			return FALSE;
		}
		TRACE("server: started, port %u, version %u\n", nPort, SERVER_VERSION);

		// Handle des Sockets merken, Zustand wechseln
		m_hSocket = socket.Detach();
		ASSERT(m_hSocket != INVALID_SOCKET);
		m_nState = SRV_STATE_COLLECTING;

		// Collector starten
		ASSERT(m_pCollector == NULL);
		m_pCollector = new CClientCollector(m_hSocket);
		m_pCollector->Start();

	//	TO_ALL_LISTENERS(OnServerStarted());
		return TRUE;
	}

	void CBotEServer::Stop()
	{
		if (m_nState == SRV_STATE_STOPPED) return;

		// Collector anhalten, falls dieser noch läuft
		StopCollector();

		// Array mit den Völkern zugeordneten Verbindungen zurücksetzen, Computerspieler
		// zurücksetzen
		memset(m_pChosenRaces, 0, RACE_COUNT * sizeof(*m_pChosenRaces));
		memset(m_pbPlayByServer, 0, RACE_COUNT * sizeof(*m_pbPlayByServer));

		// Verbindungen zu Clients trennen, Objekte zerstören, Liste leeren
		for (POSITION pos = m_lConnections.GetHeadPosition(); pos; m_lConnections.GetNext(pos))
		{
			CConnection *pConnection = m_lConnections.GetAt(pos).pConnection;
			pConnection->Interrupt();
	//		pConnection->Join();
	//		delete pConnection;
		}
		m_lConnections.RemoveAll();

		// Server beenden
		CSocket socket;
		socket.Attach(m_hSocket);
		socket.Close();
		m_hSocket = INVALID_SOCKET;
		m_nState = SRV_STATE_STOPPED;

		TRACE("server: stopped\n");
	//	TO_ALL_LISTENERS(OnServerStopped());
	}

	void CBotEServer::OnClientCollected(SOCKET hSocket)
	{
		ASSERT(hSocket != INVALID_SOCKET);
		ASSERT(m_nState == SRV_STATE_COLLECTING);
	#ifdef _DEBUG
		CAsyncSocket socket;
		socket.Attach(hSocket);
		CString ip;
		UINT port;
		socket.GetPeerName(ip, port);
		TRACE("server: new client from %s:%u connected\n", ip, port);
		socket.Detach();
	#endif

		// Client per Handshake als BotE-Client identifizieren
		CThread *pThread = new CServerHandshake(hSocket, SERVER_VERSION);
		pThread->Start();
	}

	void CBotEServer::OnNewClient(SOCKET hSocket, UINT nVersion, const CString &strUserName)
	{
		ASSERT(hSocket != INVALID_SOCKET);
		ASSERT(m_nState == SRV_STATE_COLLECTING);
		TRACE("server: new client \"%s\" verified, negotiated version %u\n", strUserName, nVersion);

		// CConnection-Objekt erzeugen, in Liste aufnehmen
		CConnection *pConnection = new CConnection(hSocket, nVersion, this);
		CONNECTION entry = {pConnection, strUserName};
		m_lConnections.AddTail(entry);

		// Liste der bereits vergebenen Völker senden
		SendChosenRaces(pConnection);
	}

	void CBotEServer::SendChosenRaces()
	{
		for (POSITION pos = m_lConnections.GetHeadPosition(); pos; m_lConnections.GetNext(pos))
		{
			CConnection *pConnection = m_lConnections.GetAt(pos).pConnection;
			SendChosenRaces(pConnection);
		}
	}

	void CBotEServer::SendChosenRaces(CConnection *pConnection)
	{
		ASSERT(pConnection);
		ASSERT(sizeof(CConnection *) == sizeof(BOOL));

		ASSERT(m_nState == SRV_STATE_COLLECTING);

		// Nachricht erzeugen
		CChosenRaces msg(GetRace(pConnection));

		// zusammenstellen, welche Völker an Clients vergeben wurden
		// oder vom Server gespielt werden
		for (int i = 0; i < RACE_COUNT; i++)
		{
			if (m_pChosenRaces[i] != NULL)
			{
				for (POSITION pos = m_lConnections.GetHeadPosition(); pos; m_lConnections.GetNext(pos))
					if (m_lConnections.GetAt(pos).pConnection == m_pChosenRaces[i])
					{
						msg.SetPlayedByClient((RACE)(RACE_FIRST + i), m_lConnections.GetAt(pos).strUserName);
					}
			}
			else if (m_pbPlayByServer[i])
				msg.SetPlayedByServer((RACE)(RACE_FIRST + i));
		}

		// senden
		Send(pConnection, &msg);
	}

	void CBotEServer::Send(CConnection *pConnection, CMessage *pMessage)
	{
		if (!pConnection || !pMessage)
		{
			ASSERT(FALSE);
			return;
		}

		VERIFY(pConnection->Send(new CPacket(pMessage)));
	}

	void CBotEServer::SendToAll(CMessage *pMessage, CConnection *pFromConnection)
	{
		if (!pMessage)
		{
			ASSERT(FALSE);
			return;
		}

		CReference<CPacket> packet(new CPacket(pMessage));
		for (POSITION pos = m_lConnections.GetHeadPosition(); pos; m_lConnections.GetNext(pos))
		{
			CConnection *pConnection = m_lConnections.GetAt(pos).pConnection;
			if (pConnection != pFromConnection) VERIFY(pConnection->Send(packet));
		}
	}

	void CBotEServer::StopCollector()
	{
		if (!m_pCollector) return;

		m_pCollector->Interrupt();
		m_pCollector->Join();

		delete m_pCollector;
		m_pCollector = NULL;
	}

	void CBotEServer::OnSocketError(int nError)
	{
		TRACE("server: socket error %d\n", nError);
		TO_ALL_LISTENERS(OnServerSocketError(nError));
	}

	void CBotEServer::OnPacketReceived(const CReference<CPacket> &packet, CConnection *pConnection)
	{
		CSingleLock singleLock(&m_HandlerCriticalSection);
		singleLock.Lock();

		ASSERT(packet && pConnection);

		// TODO Struktur überarbeiten; Handler auch bei Response ausführen? Handhabung von Requests?
		// bisherige Nachrichten um Reques/Response erweitern? (z. B. REQUESTRACE)

		// haben wir eine Response erhalten?
		if (packet->IsResponse())
		{
			// zugehörigen Request heraussuchen, Antwort dort unterbringen
			ReceiveResponse(packet->GetRequestId(), pConnection, packet->GetMessage());

			// TODO erstmal abbrechen
			return;
		}

		CMessage *pMsg = packet->GetMessage();
		if (!pMsg)
		{
			TRACE("server: packet received, unknown message type\n");
			return;
		}

		// TRACE("server: packet received, message ID %u\n", pMsg->GetID());

		switch (pMsg->GetID())
		{
		case MSGID_REQUESTRACE:
			ASSERT(m_nState == SRV_STATE_COLLECTING);
			HandleRequestRace((CRequestRace *)pMsg, pConnection);
			break;

		case MSGID_ENDOFROUND:
			ASSERT(m_nState == SRV_STATE_PLAYING);
			HandleEndOfRound((CEndOfRound *)pMsg, pConnection);
			break;

		case MSGID_CHAT:
			ASSERT(m_nState != SRV_STATE_STOPPED);
			HandleChatMsg((CChatMsg *)pMsg, pConnection);
			break;

		default:
			TRACE("server: unhandled message\n");
			ASSERT(FALSE);
		}

		delete pMsg;
	}

	void CBotEServer::OnPacketSent(const CReference<CPacket> &packet, CConnection *pConnection)
	{
		//CSingleLock singleLock(&m_HandlerCriticalSection);
		//singleLock.Lock();

		ASSERT(packet && pConnection);
		//TRACE("server: packet sent\n");
	}

	void CBotEServer::OnSocketError(int nWSAError, CConnection *pConnection)
	{
		CSingleLock singleLock(&m_HandlerCriticalSection);
		singleLock.Lock();

		// Listeners benachrichtigen
		OnSocketError(nWSAError);

		// die Verbindung wird anschließend getrennt, es dürfte OnConnectionLost() aufgerufen werden,
		// wo wir die Verbindung aus m_lConnections entfernen
	}

	void CBotEServer::OnPacketDiscarded(const CReference<CPacket> &packet, CConnection *pConnection)
	{
		//CSingleLock singleLock(&m_HandlerCriticalSection);
		//singleLock.Lock();

		ASSERT(packet && pConnection);
		TRACE("server: packet discarded\n");

		// falls Request nicht gesendet werden konnte, das Warten abbrechen
		if (packet->IsRequest())
		{
			VERIFY(RemoveRequest(packet->GetRequestId()));
		}
	}

	void CBotEServer::OnConnectionLost(CConnection *pConnection)
	{
		CSingleLock singleLock(&m_HandlerCriticalSection);
		singleLock.Lock();

		// Verbindung aus Liste entfernen
		CString strUserName = "";
		for (POSITION pos = m_lConnections.GetHeadPosition(); pos; m_lConnections.GetNext(pos))
			if (m_lConnections.GetAt(pos).pConnection == pConnection)
			{
				strUserName = m_lConnections.GetAt(pos).strUserName;
				TRACE("server: connection to \"%s\" lost\n", strUserName);
				m_lConnections.RemoveAt(pos);
				break;
			}

		if (m_nState == SRV_STATE_COLLECTING)
		{
			// während der Zuordnungsphase
			// Volk freigeben, falls dem Client eines zugeordnet war
			for (int i = 0; i < RACE_COUNT; i++)
			{
				if (m_pChosenRaces[i] == pConnection)
				{
					m_pChosenRaces[i] = NULL;
					SendChosenRaces(); // übrige Clients benachrichtigen
					break;
				}
			}
		}
		else if (m_nState == SRV_STATE_PLAYING)
		{
			// während der Spielphase
			RACE race = GetRace(pConnection);
			ASSERT(RACE_FIRST <= race && race < RACE_LAST);
			int idx = race - RACE_FIRST;

			// das Volk vom Computer ab dieser Runde weiterspielen lassen, Verbindung aus
			// m_pChosenRaces entfernen
			m_pbPlayByServer[idx] = TRUE;
			m_pChosenRaces[idx] = NULL;

			if (m_pbNextRoundSent[idx])
			{
				// falls wir auf EndOfRound-Nachricht des Clients warten, ab jetzt nicht mehr warten
				DoCalculateGame(race);
			}
			else if (m_pEndOfRoundBuf[idx])
			{
				// falls EndOfRound-Nachricht schon eingetroffen, die Verbindung vom Client
				// aber während des Wartens auf andere EndOfRound-Nachrichten getrennt wird,
				// die gepufferten EndOfRound-Daten freigeben
				delete[] m_pEndOfRoundBuf[idx];
				m_pEndOfRoundBuf[idx] = NULL;
				m_pnEndOfRoundSize[idx] = 0;
			}

			// TODO Spiel abbrechen, wenn kein menschlicher Spieler mehr dabei ist; evtl. zuvor speichern
			// ...
		}

		TO_ALL_LISTENERS(OnClientLost(strUserName));

		// Objekt zerstören
		//	pConnection->Join();
		//	delete pConnection;
	}

	void CBotEServer::HandleRequestRace(CRequestRace *pMsg, CConnection *pConnection)
	{
		if (m_nState != SRV_STATE_COLLECTING) return;
		ASSERT(pMsg && pMsg->GetID() == MSGID_REQUESTRACE && pConnection);

		// bisheriges Volk des Clients bestimmen
		RACE nOldRace = GetRace(pConnection);

		// neues Volk des Clients bestimmen
		RACE nNewRace = pMsg->GetRequestedRace();
		if ((nNewRace >= RACE_FIRST && nNewRace < RACE_LAST && (m_pChosenRaces[nNewRace - RACE_FIRST] != NULL
			|| m_pbPlayByServer[nNewRace - RACE_FIRST])) || nNewRace >= RACE_LAST)
		{
			// Client fordert Volk, aber schon vergeben, oder hat einen ungültigen Wert für das Volk angegeben;
			// er behält damit sein bisheriges Volk (die Forderung nach RACE_NONE wird immer erfüllt)
			nNewRace = nOldRace;
		}

		if (nNewRace == nOldRace)
		{
			// keine Veränderung, Antwort nur an den fordernden Client senden
			SendChosenRaces(pConnection);
		}
		else
		{
			// altes Volk freigeben
			if (nOldRace != RACE_NONE)
				m_pChosenRaces[nOldRace - RACE_FIRST] = NULL;

			// neues Volk belegen
			if (nNewRace != RACE_NONE)
				m_pChosenRaces[nNewRace - RACE_FIRST] = pConnection;

			// die Vergabe der Völker hat sich geändert, alle Clients (einschl. Anforderer)
			// benachrichtigen
			SendChosenRaces();
		}
	}

	RACE CBotEServer::GetRace(CConnection *pConnection) const
	{
		for (int i = 0; i < RACE_COUNT; i++)
			if (m_pChosenRaces[i] == pConnection)
				return (RACE)(RACE_FIRST + i);
		return RACE_NONE;
	}

	BOOL CBotEServer::BeginGame(CPeerData *pDoc)
	{
		ASSERT(m_nState == SRV_STATE_COLLECTING);

		if (m_nState != SRV_STATE_COLLECTING) return FALSE;
		TRACE("server: beginning game...\n");

		// keine neuen Verbindungen mehr annehmen
		StopCollector();

		// Clients ohne zugeordnetem Volk trennen
		for (POSITION pos = m_lConnections.GetHeadPosition(); pos; m_lConnections.GetNext(pos))
		{
			CConnection *pConnection = m_lConnections.GetAt(pos).pConnection;
			ASSERT(pConnection);

			// wenn kein Volk zugeordnet, dann trennen
			if (GetRace(pConnection) == RACE_NONE)
			{
				// CConnection-Objekt aus Liste entfernen
				m_lConnections.RemoveAt(pos);
				pos = m_lConnections.GetHeadPosition(); // Iteration neu starten!

				// Verbindung trennen
				TRACE("server: disconnecting client without race...\n");
				pConnection->Interrupt();
	//			pConnection->Join();
	//			delete pConnection;
			}
		}

		// mindestens ein Client sollte übrig bleiben!
		TRACE("server: %d clients with race\n", m_lConnections.GetCount());
		if (m_lConnections.GetCount() == 0)
		{
			ASSERT(FALSE); // die Oberfläche sollte verhindern, dass dies passiert
			return FALSE;
		}

#ifdef _DEBUG
		int nServerPlayerCount = 0;
		for (int i = 0; i < RACE_COUNT; i++)
			if (m_pbPlayByServer[i]) nServerPlayerCount++;
		TRACE("server: %d computer players\n", nServerPlayerCount);
#endif

		// einige Puffer zurücksetzen
		memset(m_pbNextRoundSent, 0, RACE_COUNT * sizeof(*m_pbNextRoundSent));
		memset(m_pEndOfRoundBuf, 0, RACE_COUNT * sizeof(*m_pEndOfRoundBuf));
		memset(m_pnEndOfRoundSize, 0, RACE_COUNT * sizeof(*m_pnEndOfRoundSize));

		// BeginGame-Nachricht an restliche Clients senden
		CBeginGame msg(pDoc);
		SendToAll(&msg);

		// Zustand wechseln
		m_nState = SRV_STATE_PLAYING;

		// OnPrepareGame-Ereignis, dort wird CBotEServer::NextRound() aufgerufen
		TRACE("server: OnPrepareGame()\n");
		TO_ALL_LISTENERS(OnPrepareGame());

		return TRUE;
	}

	void CBotEServer::NextRound(CPeerData *pDoc)
	{
		ASSERT(m_nState == SRV_STATE_PLAYING && pDoc != NULL);
		if (m_nState != SRV_STATE_PLAYING || !pDoc) return;

		TRACE("server: NextRound()\n");

		// Nachricht erzeugen
		// TODO später spielspezifische Nachrichten zusammenstellen; senden erstmal sämtliche Daten
		// an alle Clients
		CNextRound msg(pDoc);

		// menschliche Mitspieler (von einem Client gespielt) setzen
		for (int i = 0; i < RACE_COUNT; i++)
			if (m_pChosenRaces[i])
				msg.SetPlayedByClient((RACE)(RACE_FIRST + i));

		// Computerspieler (vom Server gespielt) setzen
		for (int i = 0; i < RACE_COUNT; i++)
			if (m_pbPlayByServer[i])
				msg.SetPlayedByServer((RACE)(RACE_FIRST + i));

		// Nachricht packen, an alle Clients senden (wird nur einmal verpackt)
		CReference<CPacket> packet(new CPacket(&msg));
		for (POSITION pos = m_lConnections.GetHeadPosition(); pos; m_lConnections.GetNext(pos))
		{
			if (m_lConnections.GetAt(pos).pConnection->Send(packet))
			{
				// merken, dass CNextRound-Nachricht gesendet wurde, damit wir eine CEndOfRound-Nachricht
				// vom zugehörigen Client entgegen nehmen
				RACE race = GetRace(m_lConnections.GetAt(pos).pConnection);
				ASSERT(RACE_FIRST <= race && race < RACE_LAST);
				m_pbNextRoundSent[race - RACE_FIRST] = TRUE;
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}

	void CBotEServer::HandleEndOfRound(CEndOfRound *pMsg, CConnection *pConnection)
	{
		ASSERT(pMsg && pConnection && m_nState == SRV_STATE_PLAYING);
		if (m_nState != SRV_STATE_PLAYING) return;

		TRACE("server: EndOfRound message received\n");

		// prüfen, ob dem Client zuvor eine CNextRound-Nachricht geschickt wurde
		RACE race = GetRace(pConnection);
		ASSERT(RACE_FIRST <= race && race < RACE_LAST);
		int idx = race - RACE_FIRST;
		if (!m_pbNextRoundSent[idx])
		{
			TRACE("server: no matching NextRound message\n");
			ASSERT(FALSE);
			return;
		}

		// Inhalt der Nachricht unter zugehörigem Volk merken
		ASSERT(m_pEndOfRoundBuf[idx] == NULL);
		m_pnEndOfRoundSize[idx] = pMsg->GetSize();
		m_pEndOfRoundBuf[idx] = pMsg->Detach();

		// wenn alle EndOfRound-Nachrichten erhalten, dann Handler CServerListener::OnCalculateGame()
		// aufrufen und Puffer wieder freigeben
		DoCalculateGame(race);
	}

	void CBotEServer::DoCalculateGame(RACE race)
	{
		ASSERT(RACE_FIRST <= race && race < RACE_LAST);
		int idx = race - RACE_FIRST;

		// alle CEndOfRound-Nachrichten erhalten?
		m_pbNextRoundSent[idx] = FALSE;
		BOOL bReceivedAll = TRUE;
		for (int i = 0; i < RACE_COUNT; i++)
		{
			if (m_pbNextRoundSent[i])
			{
				bReceivedAll = FALSE;
				break;
			}
		}

		// Ereignis
		if (bReceivedAll)
		{
			// Listeners informieren; diese erhalten mit DeserializeEndOfRoundToDoc() die empfangenen
			// EndOfRound-Daten eines bestimmten Volkes
			TRACE("server: OnCalculateGame()\n");
			TO_ALL_LISTENERS(OnCalculateGame());

			// gemerkte EndOfRound-Nachrichten löschen
			DeleteEndOfRoundBufs();
		}
	}

	void CBotEServer::DeleteEndOfRoundBufs()
	{
		for (int i = 0; i < RACE_COUNT; i++)
		{
			if (m_pEndOfRoundBuf[i])
			{
				delete[] m_pEndOfRoundBuf[i];
				m_pEndOfRoundBuf[i] = NULL;
				m_pnEndOfRoundSize[i] = 0;
			}
		}
	}

	BOOL CBotEServer::DeserializeEndOfRoundToDoc(RACE race, CPeerData *pDoc)
	{
		ASSERT(RACE_FIRST <= race && race < RACE_LAST);
		ASSERT(pDoc);
		if (race < RACE_FIRST || race >= RACE_LAST || !pDoc) return FALSE;

		int idx = race - RACE_FIRST;
		if (!m_pEndOfRoundBuf[idx]) return FALSE;

		CMemFile memFile(m_pEndOfRoundBuf[idx], m_pnEndOfRoundSize[idx]);
		CArchive ar(&memFile, CArchive::load);
		pDoc->SerializeEndOfRoundData(ar, race);
		ar.Flush();
		memFile.Detach();

		return TRUE;
	}

	void CBotEServer::HandleChatMsg(CChatMsg *pMsg, CConnection *pConnection)
	{
		if (m_nState != SRV_STATE_COLLECTING && m_nState != SRV_STATE_PLAYING) return;

		// Benutzernamen des sendenden Clients ermitteln
		CString strUserName = "";
		for (POSITION pos = m_lConnections.GetHeadPosition(); pos; m_lConnections.GetNext(pos))
			if (m_lConnections.GetAt(pos).pConnection == pConnection)
			{
				strUserName = m_lConnections.GetAt(pos).strUserName;
				break;
			}

		// Absender-Volk und Benutzername korrigieren
		switch (m_nState)
		{
		case SRV_STATE_COLLECTING:
			pMsg->SetSender(RACE_NONE);
			pMsg->SetSenderName(strUserName);
			ASSERT(pMsg->IsForAllClients()); // in der Volkzuordnungsphase sollten Chat-Nachrichten an alle gehen
			break;

		case SRV_STATE_PLAYING:
			{
			RACE race = GetRace(pConnection);
			if (race == RACE_NONE)
			{
				// während des Spiels sind nur Chat-Nachrichten von Mitspielern erlaubt
				ASSERT(FALSE);
				return;
			}
			pMsg->SetSender(race);
			pMsg->SetSenderName(strUserName);
			break;
			}
		}

		// Nachricht weiterleiten
		if (pMsg->IsForAllClients())
		{
			// Broadcast, an alle außer dem ursprünglichen Absender senden
			TRACE("server: broadcasting CChatMsg message...\n");
			SendToAll(pMsg, pConnection);
		}
		else
		{
			// Empfänger prüfen; bei Fehler nichts weiterleiten
			for (UINT i = 0; i < pMsg->GetReceiverCount(); i++)
			{
				RACE recv = pMsg->GetReceiver(i);
				if (recv < RACE_FIRST || recv >= RACE_LAST)
				{
					ASSERT(FALSE);
					return;
				}
			}

			// an angegebene Empfänger weiterleiten, wenn diese mitspielen; die Nachricht
			// geht nicht an den Absender zurück
			TRACE("server: forwarding CChatMsg message...\n");
			CReference<CPacket> packet(new CPacket(pMsg));
			for (UINT i = 0; i < pMsg->GetReceiverCount(); i++)
			{
				RACE receiver = pMsg->GetReceiver(i);
				ASSERT(RACE_FIRST <= receiver && receiver < RACE_LAST);
				int idx = receiver - RACE_FIRST;

				CConnection *pRecvConnection = m_pChosenRaces[idx];
				// pRecvConnection ist für Nichtspieler und Computerspieler auf jeden Fall NULL
				if (pRecvConnection != NULL && pRecvConnection != pConnection)
					VERIFY(pRecvConnection->Send(packet));
			}
		}
	}

	BOOL CBotEServer::SetPlayByServer(RACE nRace, BOOL bPlayByServer, BOOL bUpdateClients)
	{
		if (nRace < RACE_FIRST || nRace >= RACE_LAST)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		ASSERT(m_nState == SRV_STATE_STOPPED || m_nState == SRV_STATE_COLLECTING);
		if (m_nState != SRV_STATE_STOPPED && m_nState != SRV_STATE_COLLECTING)
			return FALSE;
		int idx = nRace - RACE_FIRST;

		// erfolgreich abbrechen, wenn gewünschter Zustand bereits existiert
		if (m_pbPlayByServer[idx] == bPlayByServer) return TRUE;

		// Volk schon vergeben?, vor Start() sind alle Einträge von m_pChosenRaces immer NULL
		if (bPlayByServer && m_pChosenRaces[idx])
			return FALSE;

		// Volk dem Server zuteilen bzw. freigeben
		m_pbPlayByServer[idx] = bPlayByServer;

		// während der Volkzuordnungsphase alle Clients über Änderungen informieren,
		// wenn gewünscht
		if (m_nState == SRV_STATE_COLLECTING && bUpdateClients)
			SendChosenRaces();

		return TRUE;
	}

	BOOL CBotEServer::IsPlayer(RACE nRace) const
	{
		if (nRace < RACE_FIRST || nRace >= RACE_LAST)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		return (m_pChosenRaces[nRace - RACE_FIRST] != NULL || m_pbPlayByServer[nRace - RACE_FIRST]);
	}

	BOOL CBotEServer::IsPlayedByServer(RACE nRace) const
	{
		if (nRace < RACE_FIRST || nRace >= RACE_LAST)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		return m_pbPlayByServer[nRace - RACE_FIRST];
	}

	BOOL CBotEServer::IsPlayedByClient(RACE nRace) const
	{
		if (nRace < RACE_FIRST || nRace >= RACE_LAST)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		return (m_pChosenRaces[nRace - RACE_FIRST] != NULL);
	}

	int CBotEServer::AddRequest(CConnection *pConnection)
	{
		// Struktur befüllen
		REQUEST request;
		request.nId = m_nNextRequestId++;
		ASSERT(request.nId >= 0);
		request.pConnection = pConnection;

		// Ereignis erzeugen (ohne Namen)
		request.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (request.hEvent == NULL) return -1;

		// alles korrekt erzeugt, Eintrag zur Liste der Requests hinzufügen
		// (Zugriff auf m_lRequests synchronisieren)
		CSingleLock singleLock(&m_RequestCriticalSection);
		singleLock.Lock();
		m_lRequests.AddTail(request);
		singleLock.Unlock();

		// Flag setzen, ID des Requests zurückgeben
		return request.nId;
	}

	int CBotEServer::SendRequest(CConnection *pConnection, CMessage *pMessage)
	{
		// Request erzeugen
		int id = AddRequest(pConnection);
		if (id > -1)
		{
			// Nachricht als Request versenden (Response erhält dieselbe Request-Id)
			VERIFY(pConnection->Send(new CPacket(pMessage, id, TRUE)));
		}
		return id;
	}

	CReferenceTransfer<CMessage> CBotEServer::GetResponse(int nRequestId, DWORD dwMilliseconds)
	{
		CSingleLock singleLock(&m_RequestCriticalSection);
		singleLock.Lock();

		// zugehörigen Eintrag suchen
		for (POSITION pos = m_lRequests.GetHeadPosition(); pos; m_lRequests.GetNext(pos))
		{
			REQUEST &request = m_lRequests.GetAt(pos);
			if (request.nId == nRequestId)
			{
				singleLock.Unlock();

				// warten, bis Ereignis auslöst
				if (WaitForSingleObject(request.hEvent, dwMilliseconds) == WAIT_OBJECT_0)
				{
					// Referenz auf Nachricht merken
					CReferenceTransfer<CMessage> result(request.refResponse);

					// Eintrag aus Liste entfernen
					singleLock.Lock();
					m_lRequests.RemoveAt(pos);

					// Nachricht zurückgeben
					return result;
				}
				else
					return NULL;
			}
		}

		return NULL;
	}

	BOOL CBotEServer::ReceiveResponse(int nRequestId, CConnection *pConnection, CReference<CMessage> refMessage)
	{
		CSingleLock singleLock(&m_RequestCriticalSection);
		singleLock.Lock();

		// zugehörigen Eintrag suchen
		for (POSITION pos = m_lRequests.GetHeadPosition(); pos; m_lRequests.GetNext(pos))
		{
			REQUEST &request = m_lRequests.GetAt(pos);
			if (request.nId == nRequestId)
			{
				ASSERT(pConnection == NULL || request.pConnection == pConnection);
				ASSERT(!request.refResponse);

				// Antwort merken, wenn bisher keine eingetroffen
				request.refResponse = refMessage;

				// Ereignis auslösen
				singleLock.Unlock();
				VERIFY(SetEvent(request.hEvent));

				return TRUE;
			}
		}

		return FALSE;
	}

	BOOL CBotEServer::RemoveRequest(int nRequestId)
	{
		return ReceiveResponse(nRequestId, NULL, NULL);
	}

	void CBotEServer::CancelRequest(int nRequestId)
	{
		if (!RemoveRequest(nRequestId)) return;
		GetResponse(nRequestId, 0);
	}

};
