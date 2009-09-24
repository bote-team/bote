#pragma once
#include "thread.h"

namespace network
{
	/**
	 * Thread, der serverseitig den Handshake mit dem Client ausführt. Informiert die
	 * <code>CBotEServer</code>-Instanz per <code>OnNewClient()</code> über die
	 * Erfolgreiche Durchführung eines Handshakes.
	 *
	 * <code>CServerHandshake</code>-Objekte zerstören sich nach Beendigung des Threads
	 * von selbst.
	 *
	 * Handshake (three-way):
	 *
	 * 1. Client sendet		HANDSHAKE_CLIENTID + "\0" + <UINT Versionsnummer> + "\0"
	 * 2. Server sendet		HANDSHAKE_SERVERID + "\0" + <UINT Versionsnummer> + "\0"
	 * 3. Client sendet		"OK\0" + <UINT Länge des Benutzernamens> + <BYTE[] Benutzername>
	 *
	 * Der Partner muss die Nachricht innerhalb von HANDSHAKE_TIMEOUT Sekunden erhalten,
	 * sonst wird die Verbindung getrennt.
	 *
	 * Geben Server und Client verschiedene Versionsnummern an, wird die jeweils kleinere
	 * Versionsnummer verwendet (unter der Annahme, dass zukünftige Versionen abwärtskompatibel
	 * sind).
	 *
	 * @author CBot
	 * @version 0.0.5
	 */
	class CServerHandshake : public CThread
	{
		DECLARE_DYNAMIC(CServerHandshake)

	private:
		SOCKET m_hSocket;
		UINT m_nServerVersion;

	public:
		CServerHandshake(SOCKET hSocket, UINT nServerVersion);
		virtual ~CServerHandshake();

		/// @return <code>0</code> bei Erfolg, sonst <code>1</code>
		virtual void Run();

	private:
		/// Wartet, bis die angegebene Anzahl Bytes zur Verfügung steht oder die gegebene
		/// Deadline abläuft.
		static BOOL RequestData(CAsyncSocket &socket, UINT nCount, clock_t clkDeadline);

	};

};
