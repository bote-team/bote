#pragma once

namespace network
{
	class CMessage;
};

#include "Network.h"
#include "Connection.h"

#include "ReferenceCounter.h"

namespace network
{
	/**
	 * Basisklasse für alle Nachrichten, die zwischen Server und Client ausgetauscht werden.
	 * Repräsentiert die zu übertragende Nutzinformation (also keine Information, die den Vorgang
	 * der Übertragung steuert).
	 *
	 * Für das Versenden wird das Nachrichten-Objekt mit einem entsprechenden Konstruktor erzeugt.
	 * Für das Empfangen wird das Nachrichten-Objekt per Standardkonstruktor erzeugt;
	 * <code>Serialize()</code> setzt anschließend die Attribute des Objekts. Jede abgeleitete Klasse
	 * benötigt damit einen Standardkonstruktor.
	 *
	 * @author CBot
	 * @version 0.0.3
	 */
	class CMessage : public CReferenceCounter
	{
	private:
		BYTE m_nID;							///< ID der Nachricht

	public:
		/**
		 * Erzeugt eine Nachricht mit der angegebenen ID.
		 */
		CMessage(BYTE nID);
		virtual ~CMessage();

		/**
		 * @return ID dieser Nachricht
		 */
		int GetID() const {return m_nID;}

		/**
		 * Serialisiert bzw. deserialisiert die Daten dieser Nachricht.
		 */
		virtual void Serialize(CArchive &ar) =0;

		/**
		 * @return Instanz der zur angegebenen ID passenden, von <code>CMessage</code>
		 * abgeleiteten Klasse oder <code>NULL</code>, wenn keine gültige/bekannte ID
		 * angegeben wurde; das jeweilige Nachrichtenobjekt wird per Standardkonstruktor
		 * erzeugt, die Daten der erzeugten Nachricht müssen nachträglich gesetzt werden
		 * (üblicherweise per Deserialisieren mit <code>Serialize</code>)
		 */
		static CMessage *CreateMessage(BYTE nID);

	};

};
