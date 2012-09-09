#pragma once

#include "Message.h"
#include "PeerData.h"

namespace network
{
	/**
	 * Nachricht, die der Server an alle Clients, denen Völker zugeordnet wurden, versendet, wenn das
	 * Spiel beginnen soll. Die Clients versetzen sich daraufhin in den Rundenende-Zustand und warten auf eine
	 * <code>CNextRound</code>-Nachricht vom Server.
	 *
	 * Diese Nachricht enthält Daten, die der Server bei Spielbeginn nur einmal an alle Clients senden
	 * muss.
	 *
	 * Aufbau:
	 * <pre>
	 * UINT		size			Länge von data
	 * BYTE[]	data			serialisierte Daten des Dokuments
	 * </pre>
	 *
	 * @author CBot
	 * @version 0.0.2
	 */
	class CBeginGame : public CMessage
	{
	private:
		UINT m_nSize;									///< Größe des Puffers <code>m_pData</code>
		BYTE *m_pData;									///< serialisierte Daten des Dokuments

	public:
		CBeginGame();

		/**
		 * Erzeugt die Nachricht und serialisiert die Daten des angegebenen Client-Dokuments per
		 * <code>CPeerData::SerializeEndOfRoundData()</code>.
		 */
		CBeginGame(CPeerData *pDoc);
		virtual ~CBeginGame();

		virtual void Serialize(CArchive &ar);

		/// Deserialisiert die in der Nachricht enthaltenen Daten per
		/// <code>CPeerData::SerializeBeginGameData()</code> ins Dokument.
		void DeserializeToDoc(CPeerData *pDoc);

	};

};
