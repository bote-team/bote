#pragma once

#include "Thread.h"
#include "LANMessage.h"
#include "Reference.h"

namespace network
{
	class CLANConnection;

	/**
	 * Stellt Handler für Ereignisse, die in einem <code>CLANConnection</code>-Thread auftreten, zur Verfügung.
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CLANConnectionListener
	{
	public:
		/// Die angegebene Verbindung hat das angegebene Paket empfangen.
		virtual void OnMessageReceived(const CReference<CLANMessage> &message, CLANConnection *pConnection) =0;

		/// Die angegebene Verbindung hat das angegebene Paket erfolgreich versendet.
		virtual void OnMessageSent(const CReference<CLANMessage> &message, CLANConnection *pConnection) =0;

		/// Das angegebene ursprünglich zu sendende Paket wird ohne vollständig versendet worden zu sein verworfen.
		virtual void OnMessageDiscarded(const CReference<CLANMessage> &message, CLANConnection *pConnection) =0;

		/// Auf der angegebenen Verbindung ist der angegebene Socket-Fehler aufgetreten. Die angegebene
		/// Verbindung wird anschließend getrennt (liefert <code>OnConnectionLost()</code>).
		virtual void OnSocketError(int nWSAError, CLANConnection *pConnection) =0;

		/// Die angegebene Verbindung wurde durch einen Fehler oder vom anderen Ende aus
		/// (also nicht durch das diesseitige <code>CLANConnection::Interrupt()</code>) getrennt.
		virtual void OnConnectionLost(CLANConnection *pConnection) =0;

	};

	typedef CList< CReference<CLANMessage> > MSG_LIST;

	/**
	 * Thread, der per UDP im lokalen Netzwerk Nachrichten vom Typ <code>CLANMessage</code>
	 * versendet und empfängt.
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CLANConnection : public CThread
	{
		DECLARE_DYNAMIC(CLANConnection)

	private:
		CLANConnectionListener *m_pListener;	///< Listener, der über Ereignisse informiert wird
		bool m_bSendLost;						///< <code>TRUE</code> gdw. bei Beendigung des Threads <code>CLANConnectionListener::OnConnectionLost()</code> aufgerufen werden soll

		SOCKET m_hSocket;						///< Socket-Handle
		UINT m_nPort;							///< Port, an dem dieser Thread lauscht
		DWORD m_dwIP;							///< lokale IP-Adresse

		MSG_LIST messages;						///< Liste der zu versendenden Nachrichten
		CCriticalSection m_CriticalSection;

	public:
		/**
		 * Erzeugt diesen Thread. Der Thread muss per <code>Start()</code> gestartet werden. Der angegebene Listener
		 * wird über auftretende Ereignisse wie z. B. eintreffende Nachrichten informiert.
		 *
		 * @param nPort Port, an dem Nachrichten entgegen genommen werden sollen; bei Angabe von <code>0</code>
		 * wird ein freier Port gewählt; <code>GetPort()</code> liefert anschließend den gewählten Port
		 */
		CLANConnection(UINT nPort, CLANConnectionListener *pListener);
		virtual ~CLANConnection();

		/**
		 * Erzeugt den Socket für die Verbindung. Kann vor <code>Start()</code> aufgerufen werden, um zu prüfen,
		 * ob der Socket erfolgreich erzeugt werden kann. Wird vom Thread selbst aufgerufen, um den Socket zu
		 * erzeugen. Tut nichts, wenn der Socket bereits erzeugt wurde (liefert <code>TRUE</code>).
		 *
		 * @return <code>TRUE</code> gdw. der Socket erfolgreich erzeugt werden konnte; <code>nError</code> enthält
		 * bei einem Fehler den Fehlercode
		 */
		BOOL CreateSocket(int &nError);

		virtual void Run();
		virtual void Interrupt();

		/// @return liefert den Port, an dem dieser Thread tatsächlich lauscht
		UINT GetPort() const {return m_nPort;}
		/// @return lokale IP-Adresse des erzeugten Sockets
		DWORD GetIP() const {return m_dwIP;}

		/**
		 * Fügt eine Nachricht zur Warteschlange hinzu. Die Nachricht wird asynchron versendet.
		 * Wurde die Nachricht erfolgreich versendet, wird <code>CLANConnectionListener::OnMessageSent()</code>,
		 * sonst <code>CLANConnectionListener::OnMessageDiscarded()</code> aufgerufen.
		 *
		 * Im Gegensatz zum üblichen Verhalten werden Broadcast-Nachrichten in Kopie auch an den lokalen Host
		 * gesendet. Fehler beim Versenden der Broadcast-Nachricht aufgrund einer fehlenden Verbindung zu einem
		 * lokalen Netz führen nicht zum Abbruch des Threads. <code>CLANConnectionListener::OnSocketError()</code>
		 * und <code>CLANConnectionListener::OnMessageDiscarded()</code> werden dennoch aufgerufen. Tritt beim
		 * Versenden der Kopie an den lokalen Host ein Fehler auf, wird eine Referenz auf die ursprüngliche
		 * Nachricht an den jeweiligen Handler übergeben, d. h. deren Empfängeradresse ist nach wie vor
		 * <code>INADDR_BROADCAST</code>.
		 * 
		 * @return <code>TRUE</code> gdw. die Nachricht erfolgreich an die Warteschlange angehängt
		 * werden konnte
		 */
		BOOL Send(const CReference<CLANMessage> &message);

	private:
		/// @return die nächste Nachricht der Nachrichten-Warteschlange; <code>NULL</code>, wenn die Warteschlange#
		/// keine Nachrichten enthält
		CReferenceTransfer<CLANMessage> GetNextMessage();

		/// Hilfsfunktion zum Versenden von Nachrichten. Ermöglicht das mehrfache Versenden einer Nachricht mit
		/// veränderter Zielangabe in <code>sockaddr</code> ohne die Nachricht erneut zu serialisieren.
		BOOL SendMessageTo(CAsyncSocket &socket, const CReference<CLANMessage> &msg,
			const BYTE *lpBuf, UINT nSize, SOCKADDR_IN *sockaddr);

	};

};
