#pragma once

namespace network
{
	// IDs der Völker
	typedef enum RACE
	{
		RACE_NONE,											///< Volk nicht gesetzt
		RACE_1,
		RACE_2,
		RACE_3,
		RACE_4,
		RACE_5,
		RACE_6,
		RACE_ALL,											///< für Broadcast-Nachrichten (auch an Clients, die kein Volk gewählt haben)
	}
	RACE;

};

#define RACE_FIRST				RACE_1
#define RACE_LAST				(RACE_6 + 1)
#define RACE_COUNT				(RACE_LAST - RACE_FIRST)

// verschiedene Konstanten
#define HANDSHAKE_CLIENTID		"BotE-Client"				///< Identifizierungs-String des Clients
#define HANDSHAKE_SERVERID		"BotE-Server"				///< Identifizierungs-String des Servers
#define HANDSHAKE_TIMEOUT		5							///< in Sekunden
#define SERVER_VERSION			1							///< Protokoll-Version, die der Server verwendet
#define CLIENT_VERSION			1							///< Protokoll-Version, die der Client verwendet
#define PACKET_COMPRESS			512							///< Paket wird komprimiert, wenn unkomprimierte Daten länger als dieser Wert sind
#define RECEIVE_TIMEOUT			5							///< Timeout beim Empfangen von Daten (s)
#define SEND_TIMEOUT			8							///< Timeout beim Senden von Daten (s)
#define PACKET_CAPTURE_PATTERN	"BotE"						///< Capture Pattern für den Paketanfang
#define PACKET_CAPTURE_LEN		4							///< Länge des Capture Pattern Strings
#define LAN_BUFSIZE				512							///< Größe des Puffers, der UDP-Pakete aufnimmt

// benutzerdefinierte Fenster-Nachrichten
#define WM_USERMSG				((WM_USER) + 200)			///< universelle Nachricht, <code>wParam</code>: eine der <code>UMSG</code>-Konstanten

#define UMSG_SETSERVER			1							///< in CMainDlg; lParam != 0: Server läuft auf lokalem Rechner

// IDs für die verschiedenen Nachrichten
#define MSGID_NONE				0

#define MSGID_CHOSENRACES		1							///< SRV-CLT Liste der bereits gewählten Völker
#define MSGID_REQUESTRACE		2							///< CLT-SRV Aus-/Abwahl eines Volkes
#define MSGID_BEGINGAME			3							///< SRV-CLT Spiel starten, Volkszuordnungphase ist vorüber
#define MSGID_NEXTROUND			4							///< SRV-CLT Spieldaten der nächsten Runde, startet die angegebene Runde
#define MSGID_ENDOFROUND		5							///< CLT-SRV Rundendaten an Server senden, beendet beim Client die Runde
#define MSGID_CHAT				6							///< (CLT-)SRV-CLT Chat-Nachricht

// Flags für CPacket
#define PCK_COMPRESSED			1							///< gesetzt: Daten der Nachricht sind komprimiert
#define PCK_REQUEST				2							///< gesetzt: Paket ist ein Request, nicht gesetzt: Paket ist eine Response, wenn die Request-ID <code>&gt;-1</code> ist

// IDs für LAN-Nachrichten
#define LANID_NONE				0

#define LANID_WHOISTHERE		1							///< CLT-SRV Welche Server sind vorhanden? (broadcast)
#define LANID_HELLO				2							///< SRV-CLT Antwort eines Servers auf WHOISTHERE
//#define LANID_BYE				3							///< SRV-CLT, CLT-SRV Server oder Client wird beendet


// Funktionen
namespace network
{
	/// @return eine zum gegebenen Fehlercode gehörige Beschreibung; als Parameter können alle Codes dienen,
	/// die <code>CAsyncSocket::GetLastError()</code> geliefert hat.
	CString GetWSAErrorMsg(int nError);

	/// @return String-Darstellung der (in host byte order) gegebenen IP-Adresse
	CString AddrToString(DWORD dwIP);

};
