#pragma once

#include "network.h"

namespace network
{
	/**
	 * Stellt Methoden für das Serialisieren und Deserialisieren der Spieldaten, die im Netzwerk
	 * übertragen werden müssen, zur Verfügung. Wird üblicherweise von der Dokumentenklasse des
	 * Servers und Clients implementiert.
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CPeerData
	{
	public:
		/// Serialisiert die Daten, die zu Beginn eines Spieles vom Server einmal an alle Clients
		/// übertragen werden (<code>CBeginGame</code>-Nachricht). (store: Server, load: Client)
		virtual void SerializeBeginGameData(CArchive &ar) =0;

		/// Serialisiert die Daten für die <code>CNextRound</code>-Nachricht. (store: Server, load: Client)
		virtual void SerializeNextRoundData(CArchive &ar) =0;

		/// Serialisiert die Daten für die <code>CEndOfRound</code>-Nachricht über das angegebene Volk.
		/// <code>race</code> wird beim Schreiben ignoriert. (store: Client, load: Server)
		virtual void SerializeEndOfRoundData(CArchive &ar, RACE race) =0;

	};

};
