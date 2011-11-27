#pragma once

#include "LANConnection.h"

namespace network
{
	/**
	 * Implementiert die Funktionalität zum Veröffentlichen eines Servers/Spieles im lokalen Netz.
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CLANServer : public CLANConnectionListener
	{
	private:
		CLANConnection *m_pConnection;			///< Kommunikationsthread

		CString m_strDescription;				///< Beschreibung, die veröffentlicht werden soll
		UINT m_nTCPPort;						///< zu veröffentlichender TCP-Port, an dem der BotE-Server läuft
		
	public:
		CLANServer();
		virtual ~CLANServer();

		/**
		 * Startet den serverseitigen Kommunikationsthread, der sich um das Veröffentlichen eines Spieles
		 * kümmert.
		 *
		 * @param nServerPort UDP-Port, an dem der Thread läuft
		 * @param strDescription Beschreibung des Servers/Spieles
		 * @param nTCPPort TCP-Port, an dem der BotE-Server läuft
		 * @return <code>TRUE</code> gdw. der Thread erfolgreich erzeugt und gestartet werden konnte;
		 * liefert <code>FALSE</code>, wenn der Thread bereits erzeugt wurde
		 */
		BOOL StartPublishing(UINT nServerPort, const CString &strDescription, UINT nTCPPort);

		/// Hält den serverseitigen Kommunikationsthread an, falls dieser läuft.
		void StopPublishing();

		// Handler für LANConnection-Ereignisse
		virtual void OnMessageReceived(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnMessageSent(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnMessageDiscarded(const CReference<CLANMessage> &message, CLANConnection *pConnection);
		virtual void OnSocketError(int nWSAError, CLANConnection *pConnection);
		virtual void OnConnectionLost(CLANConnection *pConnection);

	};

};

/// globale Instanz
extern network::CLANServer serverPublisher;
