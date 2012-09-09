#pragma once

#include "Message.h"
#include "Network.h"

namespace network
{
	/// Werte für <code>CChosenRaces::m_pPlayers</code>.
	typedef enum PLAYER
	{
		PLAYER_NONE,				///< das Volk spielt nicht mit
		PLAYER_SERVER,				///< das Volk wird vom Computer (Server) gespielt
		PLAYER_CLIENT,				///< das Volk wird von einem Menschen (Client) gespielt
	}
	PLAYER;

	/**
	 * Liste von Völkern, die bereits gewählt wurden und das dem Empfänger der Nachricht
	 * zugeordnete Volk.
	 *
	 * Der Server sendet jedem neu verbundenen Client zu Beginn eine solche Nachricht. Ändert sich
	 * die Zuordnung der Clients zu den Völkern (durch <code>CRequestRace</code>-Nachrichten),
	 * werden sämtliche Clients durch diese Nachricht darüber informiert.
	 *
	 * Aufbau:
	 * <pre>
	 * BYTE			clientRace		dem Client zugeordnetes Volk, RACE_NONE wenn keines zugeordnet ist (RACE)
	 * BYTE[]		players			Array der Länge RACE_COUNT, Eintrag i gibt für das Volk RACE_FIRST + i eine
	 *								der PLAYER-Konstanten an
	 * CString[]	userNames		Array der Länge RACE_COUNT, Eintrag i gibt den Benutzernamen des dem Volk
	 *								RACE_FIRST + i zugeordneten Clients an (standardmäßig "")
	 * </pre>
	 *
	 * @author CBot
	 * @version 0.0.4
	 */
	class CChosenRaces : public CMessage
	{
	private:
		RACE m_nClientRace;					///< dem Client zugeordnetes Volk
		PLAYER m_pPlayers[RACE_COUNT];		///< enthält für jedes Volk, ob es mitspielt und ob es vom Server oder von einem Client gespielt wird
		CString m_pstrUserNames[RACE_COUNT];///< enthält die Benutzernamen der Clients, sonst <code>""</code>

	public:
		CChosenRaces();

		/**
		 * @param nClientRace das dem Client zugeordnete Volk, <code>RACE_NONE</code> wenn dem Client
		 * kein Volk zugeordnet ist
		 */
		CChosenRaces(RACE nClientRace);
		virtual ~CChosenRaces();

		virtual void Serialize(CArchive &ar);

		/// Legt fest, dass das angegebene Volk einem Client zugeordnet wurde.
		void SetPlayedByClient(RACE nRace, const CString &strUserName);
		/// Legt fest, dass das angegebene Volk dem Server zugeordnet wurde. Der zugehörige Benutzername
		/// wird auf <code>""</code> gesetzt.
		void SetPlayedByServer(RACE nRace);

		RACE GetClientRace() const {return m_nClientRace;}

		/**
		 * @return <code>TRUE</code> gdw. das angegebene Volk bereits vergeben ist oder vom Server gespielt
		 * wird; liefert für Werte von <code>nRace</code> kleiner als <code>RACE_FIRST</code> oder größer
		 * gleich <code>RACE_LAST</code> immer <code>FALSE</code>
		 */
		BOOL IsPlayer(RACE nRace);
		/// @return <code>TRUE</code> gdw. das angegebene Volk von einem Client gespielt wird
		BOOL IsPlayedByClient(RACE nRace);
		/// @return <code>TRUE</code> gdw. das angegebene Volk vom Server gespielt wird
		BOOL IsPlayedByServer(RACE nRace);

		/// @return Benutzername des dem Volk zugeordneten Clients; <code>""</code>, wenn das Volk vom Server
		/// gespielt wird oder nicht am Spiel teilnimmt
		CString GetUserName(RACE nRace);

	};

};
