#pragma once
#include "Message.h"
#include "PeerData.h"
#include "ChosenRaces.h"

namespace network
{
	/**
	 * Sendet die Spieldaten der folgenden Runde an einen Client und startet beim Client
	 * diese Runde.
	 *
	 * Aufbau:
	 * <pre>
	 * BYTE[]	players			Array der Länge RACE_COUNT, Volk RACE_FIRST + i wird auf Index i abgebildet;
	 *							enthält für jedes Volk eine der PLAYER-Konstanten
	 * UINT		size			Länge von data
	 * BYTE[]	data			serialisierte Daten des Dokuments
	 * </pre>
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CNextRound : public CMessage
	{
		PLAYER m_pPlayers[RACE_COUNT];				///< Welches Volk spielt mit? Computer oder Mensch?
		UINT m_nSize;								///< Größe des Puffers <code>m_pData</code>
		BYTE *m_pData;								///< serialisierte Daten des Dokuments

	public:
		CNextRound();

		/// Erzeugt die Nachricht und serialisiert die Daten des angegebenen Server-Dokuments per
		/// <code>CPeerData::SerializeNextRoundData()</code>.
		CNextRound(CPeerData *pDoc);
		virtual ~CNextRound();

		virtual void Serialize(CArchive &ar);

		/// Ruft <code>CPeerData::SerializeNextRoundData()</code> vom angegebenen Client-Dokument auf,
		/// um die empfangenen Daten in das Dokument zu deserialisieren.
		void DeserializeToDoc(CPeerData *pDoc);

		/// @return <code>TRUE</code> genau dann, wenn das angegebene Volk am Spiel teilnimmt
		BOOL IsPlayer(RACE race) const;
		/// @return <code>TRUE</code>, wenn das angegebene Volk von einem Menschen (Client)
		/// gespielt wird
		BOOL IsPlayedByClient(RACE race) const;
		/// @return <code>TRUE</code>, wenn das angegebene Volk vom Computer (Server) gespielt wird
		BOOL IsPlayedByServer(RACE race) const;

		/// Spezifiziert das angegebene Volk als vom Menschen (Client) gespielt.
		void SetPlayedByClient(RACE race);
		/// Spezifiziert das angegebene Volk als vom Computer (Server) gespielt.
		void SetPlayedByServer(RACE race);

	};

};
