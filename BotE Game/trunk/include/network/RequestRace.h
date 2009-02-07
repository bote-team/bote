#pragma once

namespace network
{
	class CRequestRace;
	class CMessage;
};

#include "Message.h"

namespace network
{
	/**
	 * Nachricht, die ein gewünschtes Volk beim Server anfordert. Der Server antwortet mit
	 * einer <code>CChosenRaces</code>-Nachricht, die das dem Anforderer zugeordnete Volk
	 * enthält (entspricht dem angeforderten Volk, wenn dieses noch frei war, sonst dem
	 * vorher zugeordneten Volk). Hat sich dadurch die Zuordnung der Clients zu den Völkern
	 * geändert, werden alle verbundenen Clients per <code>CChosenRaces</code>-Nachricht
	 * informiert.
	 *
	 * Aufbau:
	 * <pre>
	 * BYTE		race			angefordertes Volk, RACE_NONE zur Aufhebung der Zuordnung
	 * </pre>
	 *
	 * @author CBot
	 * @version 0.0.3
	 */
	class CRequestRace : public CMessage
	{
	private:
		RACE m_nRace;	///< das angeforderte Volk

	public:
		CRequestRace();
		CRequestRace(RACE nRace);
		virtual ~CRequestRace();

		virtual void Serialize(CArchive &ar);

		RACE GetRequestedRace() const {return m_nRace;}

	};

};
