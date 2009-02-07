#pragma once

namespace network
{
	class CBotEServer;
};

#include "Connection.h"
#include "ClientCollector.h"
#include "ServerHandshake.h"
#include "PeerData.h"

#include "RequestRace.h"
#include "EndOfRound.h"
#include "ChatMsg.h"

#include "Reference.h"

namespace network
{
	/**
	 * Zustände für <code>CBotEServer</code>.
	 */
	typedef enum SRV_STATE
	{
		SRV_STATE_STOPPED,				///< Server läuft nicht
		SRV_STATE_COLLECTING,			///< Server wartet auf teilnehmende Spieler
		SRV_STATE_PLAYING,				///< ein Spiel läuft
	}
	SRV_STATE;

	/**
	 * Stellt Handler für Server-Ereignisse zur Verfügung.
	 *
	 * @author CBot
	 * @version 0.0.2
	 */
	class CServerListener
	{
	public:
		/// Es ist ein Socket-Fehler aufgetreten.
		virtual void OnServerSocketError(int nError) =0;

		/// Wird von <code>CBotEServer::BeginGame()</code> aufgerufen; erzeugt sämtliche Spieldaten.
		/// Sollte <code>CBotEServer::NextRound()</code> aufrufen, um die erste Runde zu starten.
		virtual void OnPrepareGame() =0;

		/// Wird vom Server aufgerufen, nachdem sämtliche Clients ihre Rundendaten gesendet haben
		/// (<code>CEndOfRound</code>-Nachricht), um die Spieldaten der nächsten Runde zu berechnen.
		/// Kann per <code>CBotEServer::DeserializeEndOfRoundToDoc()</code> die Daten eines bestimmten
		/// Volkes erhalten. Ermittelt mit <code>CBotEServer::IsPlayedByServer()</code>, ob ein Volk
		/// vom Server gespielt wird. Sollte nach der Berechnung <code>CBotEServer::NextRound()</code>
		/// aufrufen, um die nächste Runde zu starten.
		virtual void OnCalculateGame() =0;

		/// Ein Client mit dem angegebenen Benutzernamen hat seine Verbindung zum Server getrennt.
		virtual void OnClientLost(const CString &strUserName) =0;

	};

	/**
	 * Eintrag für die Liste der Verbindungen des Servers.
	 */
	typedef struct CONNECTION
	{
		CConnection *pConnection;					///< Verbindung
		CString strUserName;						///< Benutzername des zugehörigen Clients
	}
	CONNECTION;

	/**
	 * Eintrag in der Liste der Requests.
	 */
	typedef struct REQUEST
	{
		int nId;									///< ID des Requests, <code>&gt;= 0</code>
		CConnection *pConnection;					///< Empfänger des Requests == Sender der Response (TODO wird evtl. nicht benötigt)

		HANDLE hEvent;								///< Handle des Events, mit dem auf die Response gewartet wird
		CReference<CMessage> refResponse;			///< die eingetroffene Response
	}
	REQUEST;

	typedef CList<CServerListener *> SERVERLISTENER_LIST;
	typedef CList<CONNECTION> CONNECTION_LIST;
	typedef CList<REQUEST> REQUEST_LIST;

	/**
	 * Kapselt die komplette Server-Funktionalität.
	 *
	 * @author CBot
	 * @version 0.0.3
	 */
	class CBotEServer : public CConnectionListener
	{
		friend class CClientCollector;
		friend class CServerHandshake;

	private:
		SERVERLISTENER_LIST m_lListeners;			///< Liste der ServerListeners
		SRV_STATE m_nState;							///< aktueller Server-Zustand
		SOCKET m_hSocket;							///< Handle des Server-Sockets

		CClientCollector *m_pCollector;				///< Thread, der Verbindungen von Clients annimmt
		CONNECTION_LIST m_lConnections;				///< Liste aller Verbindungen zu Clients

		/// Enthält für jedes Volk den Zeiger auf das <code>CConnection</code>-Objekt der Verbindung
		/// zu dem Client, der das entsprechende Volk gewählt hat. Index <code>i</code> entspricht dem
		/// Volk <code>RACE_FIRST + i</code>
		CConnection* m_pChosenRaces[RACE_COUNT];
		/// Der Eintrag <code>i</code> ist genau dann <code>TRUE</code>, wenn das Volk
		/// <code>RACE_FIRST + i</code> vom Computer (Server) gespielt werden soll.
		BOOL m_pbPlayByServer[RACE_COUNT];

		/// <code>TRUE</code> genau dann, wenn dem zugehörigen Volk eine <code>CNextRound</code>-Nachricht
		/// gesendet wurde, die durch eine <code>CEndOfRound</code>-Nachricht beantwortet werden muss.
		BOOL m_pbNextRoundSent[RACE_COUNT];
		/// Zeiger auf den Puffer, der die serialisierten Daten des Client-Dokuments enthält; <code>NULL</code>,
		/// wenn der zugehörige Client keine <code>CEndOfRound</code>-Nachricht gesendet hat.
		BYTE* m_pEndOfRoundBuf[RACE_COUNT];
		/// Eintrag <code>i</code> gibt die Länge des Puffers <code>m_pEndOfRoundBuf[i]</code> an,
		/// <code>0</code>, wenn der zum Volk <code>RACE_FIRST + i</code> gehörige Client keine
		/// <code>CEndOfRound</code>-Nachricht gesendet hat.
		UINT m_pnEndOfRoundSize[RACE_COUNT];

		/// Müssen Methoden des <code>CConnectionListener</code>-Interface synchronisieren, da diese
		/// von mehreren <code>CConnection</code>-Objekten (Threads) genutzt werden.
		CCriticalSection m_HandlerCriticalSection;

		// Request/Response-Verwaltung
		int m_nNextRequestId;						///< die für den nächsten Request zu vergebende Id
		REQUEST_LIST m_lRequests;					///< Liste der aktuellen Requests

		/// Synchronisierungsobjekt für die Verwaltung von Requests und Responses.
		CCriticalSection m_RequestCriticalSection;

	public:
		CBotEServer();
		virtual ~CBotEServer();

		void AddServerListener(CServerListener *pListener);
		void RemoveServerListener(CServerListener *pListener);

		/// Startet den Server am angegebenen Port.
		/// @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE<code>
		BOOL Start(UINT nPort);

		/// Trennt die Verbindungen zu den Clients, stoppt den Server.
		void Stop();

		/// Setzt, ob das angegebene Volk vom Server gespielt werden soll. Kann nur vor
		/// <code>BeginGame()</code> aufgerufen werden. Der Aufruf vor <code>Start()</code> ist möglich.
		/// Nach <code>Start()</code> werden bei Änderung <code>CChosenRaces</code>-Nachrichten an die
		/// Clients versendet, wenn <code>bUpdateClients = TRUE</code> ist. Die Information geht bei
		/// <code>Stop()</code> verloren und muss neu gesetzt werden.
		/// @return <code>TRUE</code> bei Erfolg; <code>FALSE</code>, wenn bei <code>bPlayByServer == TRUE</code>
		/// das gewünschte Volk bereits vergeben ist (nach <code>Start()</code>) oder nach <code>BeginGame()</code>
		BOOL SetPlayByServer(RACE nRace, BOOL bPlayByServer, BOOL bUpdateClients);

		/// @return <code>TRUE</code> gdw. das angegebene Volk am Spiel teilnimmt
		BOOL IsPlayer(RACE nRace) const;

		/// Gibt an, ob das angegebene Volk vom Server gespielt wird. Sollte im Handler
		/// <code>CServerListener::OnCalculateGame()</code> aufgerufen werden, um zu ermitteln, ob der
		/// Server Spielzüge für das Volk vornehmen muss.
		/// @return <code>TRUE</code> gdw. das angegebene Volk vom Computer (Server) gespielt wird.
		BOOL IsPlayedByServer(RACE nRace) const;

		/// @return <code>TRUE</code> gdw. das angegebene Volk von einem Client gespielt wird
		BOOL IsPlayedByClient(RACE nRace) const;

		/// Sendet jeweils eine <code>CChosenRaces<code>-Nachricht an alle verbundenen Clients.
		void SendChosenRaces();

		/// Trennt die Clients, denen kein Volk zugeordnet wurde und teilt den anderen Clients mit,
		/// dass das Spiel beginnt (<code>CBeginGame</code>-Nachricht). Löst das
		/// <code>OnPrepareGame()</code>-Ereignis aus und überträgt initiale Daten an die Clients.
		/// @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE<code>
		BOOL BeginGame(CPeerData *pDoc);

		/// Sendet eine <code>CNextRound</code>-Nachricht an alle Clients.
		void NextRound(CPeerData *pDoc);

		/// Deserialisiert die Daten einer <code>CEndOfRound</code>-Nachricht in das angegebene
		/// Server-Dokument. Kann nur vom Handler <code>CServerListener::OnCalculateGame()</code> aus
		/// aufgerufen werden.
		/// @return <code>TRUE</code> genau dann, wenn zum angegebenen Volk Daten vorhanden waren, die ins
		/// Dokument deserialisiert wurden
		BOOL DeserializeEndOfRoundToDoc(RACE race, CPeerData *pDoc);

		/// @return <code>TRUE</code> genau dann, wenn der Server läuft
		BOOL IsRunning() const {return m_nState != SRV_STATE_STOPPED;}

		// CConnectionListener-Interface
		virtual void OnPacketReceived(const CReference<CPacket> &packet, CConnection *pConnection);
		virtual void OnPacketSent(const CReference<CPacket> &packet, CConnection *pConnection);
		virtual void OnSocketError(int nWSAError, CConnection *pConnection);
		virtual void OnPacketDiscarded(const CReference<CPacket> &packet, CConnection *pConnection);
		virtual void OnConnectionLost(CConnection *pConnection);

	private:
		/// Ruft <code>OnServerSocketError()</code> aller Listeners auf.
		void OnSocketError(int nError);

		/// Wird aufgerufen, wenn sich ein neuer Client (noch ohne Handshake) zum Server verbunden hat.
		void OnClientCollected(SOCKET hSocket);

		/// Wird aufgerufen, wenn sich einer neuer BotE-Client erfolgreich zum Server verbunden hat
		/// (nach erfolgreichem Handshake).
		void OnNewClient(SOCKET hSocket, UINT nVersion, const CString &strUserName);

		/// Beendet den Thread, der neue Verbindungen von Clients annimmt.
		void StopCollector();

		/// Versendet die angegebene Nachricht über die angegebene Verbindung.
		void Send(CConnection *pConnection, CMessage *pMessage);

		/// Verpackt die angegebene Nachricht einmal und sendet sie an alle verbundenen Clients außer des
		/// angegebenen Clients. Bei Angabe von <code>NULL</code> wird die Nachricht an alle Clients gesendet.
		void SendToAll(CMessage *pMessage, CConnection *pFromConnection = NULL);

		/// @return das dem Client der angegebenen Verbindung zugeordnete Volk; <code>RACE_NONE</code>, wenn
		/// kein Volk zugeordnet ist
		RACE GetRace(CConnection *pConnection) const;

		/// Sendet über die angegebene Verbindung eine <code>CChosenRaces<code>-Nachricht.
		void SendChosenRaces(CConnection *pConnection);

		/// Von einem Client eingetroffene <code>CRequestRace</code>-Nachricht behandeln.
		void HandleRequestRace(CRequestRace *pMsg, CConnection *pConnection);

		/// Von einem Client eingetroffene <code>CEndOfRound</code>-Nachricht behandeln. Sind die
		/// Rundenende-Nachrichten aller Clients eingetroffen, wird das
		/// <code>OnCalculateGame()</code>-Ereignis ausgelöst.
		void HandleEndOfRound(CEndOfRound *pMsg, CConnection *pConnection);

		/// Leitet eine Chat-Nachricht an die angegebenen Empfänger weiter.
		void HandleChatMsg(CChatMsg *pMsg, CConnection *pConnection);

		/// Prüft, ob alle <code>EndOfRound</code>-Nachrichten eingetroffen sind. Wenn ja, wird der
		/// <code>CServerListener::OnCalculateGame()</code>-Handler aufgerufen; anschließend werden
		/// belegte Puffer wieder freigegeben.
		void DoCalculateGame(RACE race);

		/// Gibt die Puffer in <code>m_pEndOfRoundBuf</code> frei.
		void DeleteEndOfRoundBufs();

		/**
		 * Sendet die angegebene Nachricht als Request an den angegebenen Client. Als Response wird vom Empfänger
		 * des Requests eine Nachricht vom Typ <code>nResponseId</code> erwartet. Die asynchron eintreffende
		 * Antwort kann mit <code>GetResponse()</code> abgeholt werden.
		 *
		 * @return ID des erzeugten Requests; <code>-1</code>, wenn kein Request erzeugt werden konnte
		 */
		int SendRequest(CConnection *pConnection, CMessage *pMessage);

		/**
		 * Entfernt den angegebenen Request. Falls später eine Response eintrifft, wird diese verworfen.
		 */
		void CancelRequest(int nRequestId);

		/**
		 * Wartet auf das Eintreffen einer zum angegebenen Request gehörigen Response und liefert die als
		 * Antwort gesendete Nachricht. Bei Erfolg wird die Antwort aus der internen Liste entfernt, so dass
		 * die Methode in folgenden Aufrufen mit derselben Request-ID immer <code>NULL</code> liefert.
		 *
		 * @param nRequestId die von <code>SendRequest</code> gelieferte ID des erzeugten Requests
		 * @param dwMilliseconds Timeout
		 * @return als Antwort gesendete Nachricht; <code>NULL</code>, wenn ein Fehler aufgetreten ist (z. B. wenn
		 * der Request nicht gesendet wurde, der Empfänger keine Response sendet oder der Empfänger nicht mehr
		 * erreichbar ist) bzw. der Timeout abgelaufen ist
		 */
		CReferenceTransfer<CMessage> GetResponse(int nRequestId, DWORD dwMilliseconds = INFINITE);

		/// Erzeugt einen Eintrag in der Liste der Requests.
		/// @return ID des erzeugten Requests; <code>-1</code>, wenn kein Request erzeugt wurde
		int AddRequest(CConnection *pConnection);

		/**
		 * Sorgt dafür, dass <code>GetResponse()</code> mit Rückgabe von <code>NULL</code> abbricht,
		 * woraufhin der Eintrag aus der Liste der Requests entfernt wird.
		 */
		BOOL RemoveRequest(int nRequestId);

		/**
		 * Setzt die Response eines Requests und löst das zugehörige Ereignis aus, so dass
		 * <code>GetResponse()</code> (mit der gegebenen ID aufgerufen) zum Aufrufer zurückkehrt.
		 *
		 * @param nRequestId ID des Requests, für den eine Response eingetroffen ist
		 * @param pConnection Verbindung, auf der die Response eingetroffen ist
		 * @param refMessage Nachricht der eingetroffenen Response
		 * @return <code>TRUE</code> gdw. ein unbeantworteter Request mit der gegebenen ID vorhanden war
		 */
		BOOL ReceiveResponse(int nRequestId, CConnection *pConnection, CReference<CMessage> refMessage);

	};

};

/// globale Instanz
extern network::CBotEServer server;
