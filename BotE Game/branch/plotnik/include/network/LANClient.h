#pragma once

#include "LANConnection.h"

namespace network
{
	/**
	 * Stellt Handler für Ereignisse des <code>CLANClient</code>s zur Verfügung.
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CLANClientListener
	{
	public:
		/**
		 * Wird aufgerufen, wenn sich nach dem Aufruf von <code>CLANClient::SearchServers()</code> ein
		 * Server gemeldet hat.
		 */
		virtual void OnServerFound(const CString &strDescription, DWORD dwIP, int nTCPPort) =0;

	};

	typedef CList<CLANClientListener *> LANCLIENTLISTENER_LIST;

	/**
	 * Implementiert die Funktionalität zum Auffinden eines Servers/Spieles im lokalen Netz.
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CLANClient : public CLANConnectionListener
	{
	private:
		LANCLIENTLISTENER_LIST m_lListeners;	///< Liste der registrierten Listeners

		CLANConnection *m_pConnection;			///< Kommunikationsthread
		UINT m_nServerPort;						///< UDP-Port, an dem der serverseitige Thread erreichbar ist
		
	public:
		CLANClient();
		virtual ~CLANClient();

		void AddLANClientListener(CLANClientListener *pListener);
		void RemoveLANClientListener(CLANClientListener *pListener);

		/**
		 * Startet den clientseitigen Kommunikationsthread, der sich um das Auffinden eines Spieles/Servers
		 * kümmert.
		 *
		 * @param nServerPort UDP-Port, an dem der serverseitige Kommunikationsthread erreichbar ist
		 * @return <code>TRUE</code> gdw. der Thread erfolgreich erzeugt und gestartet werden konnte; immer
		 * <code>FALSE</code>, wenn der Thread bereits erzeugt wurde
		 */
		BOOL Start(UINT nServerPort);

		/// Hält den clientseitigen Kommunikationsthread an.
		void Stop();

		/// Sucht im lokalen Netz nach veröffentlichten Spielen/Servern. Antwortet ein Server, werden die
		/// Listeners per <code>CLANClientListener::OnServerFound()</code> informiert.
		void SearchServers();

		// Handler für LANConnection-Ereignisse
		virtual void OnMessageReceived(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnMessageSent(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnMessageDiscarded(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnSocketError(int nWSAError, CLANConnection *pConnection);
		virtual void OnConnectionLost(CLANConnection *pConnection);

	};


};

/// globale Instanz
extern network::CLANClient clientPublisher;
