#pragma once

#include "afxmt.h"
#include "Thread.h"
#include "Packet.h"
#include "ReferenceTransfer.h"

namespace network
{
	class CConnection;

	/**
	 * Ein Objekt dieser Klasse kann Ereignisse, die in einem <code>CConnection</code>-Objekt
	 * auftreten behandeln.
	 *
	 * @author CBot
	 * @version 0.0.2
	 */
	class CConnectionListener
	{
	public:
		/// Die angegebene Verbindung hat das angegebene Paket empfangen.
		virtual void OnPacketReceived(const CReference<CPacket> &packet, CConnection *pConnection) =0;

		/// Die angegebene Verbindung hat das angegebene Paket erfolgreich versendet.
		virtual void OnPacketSent(const CReference<CPacket> &packet, CConnection *pConnection) =0;

		/// Das angegebene ursprünglich zu sendende Paket wird ohne versendet worden zu sein verworfen.
		virtual void OnPacketDiscarded(const CReference<CPacket> &packet, CConnection *pConnection) =0;

		/// Auf der angegebenen Verbindung ist der angegebene Socket-Fehler aufgetreten. Die angegebene
		/// Verbindung wird anschließend getrennt (liefert <code>OnConnectionLost()</code>).
		virtual void OnSocketError(int nWSAError, CConnection *pConnection) =0;

		/// Die angegebene Verbindung wurde durch einen Fehler oder vom anderen Ende aus
		/// (also nicht durch das diesseitige <code>CConnection::Interrupt()</code>) getrennt.
		virtual void OnConnectionLost(CConnection *pConnection) =0;

	};

	typedef CList< CReference<CPacket> > PACKET_LIST;

	/**
	 * Thread für die Kommunikation per Socket. Versendet und empfängt <code>CPacket</code>-Objekte.
	 *
	 * Wurde der Thread nicht durch <code>Interrupt()</code> (also durch einen Fehler oder das
	 * Trennen der Verbindung von der anderen Seite aus) unterbrochen, sendet er vor seiner
	 * Beendigung <code>OnConnectionLost()</code> an den zugehörigen Listener.
	 *
	 * Der Thread startet mit der Konstruktion.
	 *
	 * @author CBot
	 * @version 0.0.6
	 */
	class CConnection : public CThread
	{
		DECLARE_DYNAMIC(CConnection)

	private:
		SOCKET m_hSocket;						///< Handle des zur Verbindung gehörenden Sockets
		PACKET_LIST packets;					///< Liste der zu versendenden Pakete
		bool m_bSendLost;						///< <code>true</code> wenn der Thread <code>MSG_CONNECTION_LOST</code> senden soll, sonst <code>false</code>
		CConnectionListener *m_pListener;		///< Listener, der auftretende Ereignisse entgegen nimmt
		UINT m_nVersion;						///< Versionsnummer des zu verwendenden Protokolls

		DWORD m_dwRemoteIP;						///< IP-Adresse des "anderen Endes", in Host-Byte-Order
		UINT m_nRemotePort;						///< Port des "anderen Endes"

		CCriticalSection m_CriticalSection;

	public:
		CConnection(SOCKET hSocket, UINT nVersion, CConnectionListener *pListener);
		virtual ~CConnection();

		virtual void Run();
		virtual void Interrupt();

		/**
		 * Hängt das angegebene Paket an die Warteschlange an. Das Paket wird asynchron bei
		 * nächster Gelegenheit versendet. Wurde die Nachricht erfolgreich versendet, wird
		 * <code>CConnectionListener::OnPacketSent()</code>, sonst
		 * <code>CConnectionListener::OnPacketDiscarded()</code> aufgerufen.
		 *
		 * @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE</code>
		 */
		BOOL Send(const CReference<CPacket> &packet);

		/// Liefert die IP-Adresse und den Port, des "anderen Endes" der Verbindung
		void GetRemoteAddr(DWORD &dwIP, UINT &nPort) const;

	private:
		/// Liefert die erste Nachricht der Warteschlange oder <code>NULL</code>, wenn
		/// die Schlange leer ist.
		CReferenceTransfer<CPacket> GetNextPacket();

/*		/// Hängt das Paket an den Anfang der Warteschlange.
		BOOL ReSend(const CReference<CPacket> &packet); */

	};

};
