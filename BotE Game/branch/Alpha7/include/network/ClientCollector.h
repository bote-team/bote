#pragma once
#include "thread.h"

namespace network
{
	class CClientCollector;
};

#include "BotEServer.h"

namespace network
{
	/**
	 * Thread, der Verbindungen von Clients annimmt. Informiert die <code>CBotEServer</code>-Instanz
	 * per <code>OnClientCollected()</code> über das Eintreffen neuer Verbindungen.
	 *
	 * @author CBot
	 * @version 0.0.2
	 */
	class CClientCollector : public CThread
	{
		DECLARE_DYNAMIC(CClientCollector)

	private:
		SOCKET m_hServer;

	public:
		CClientCollector(SOCKET hServer);
		virtual ~CClientCollector();

		virtual void Run();

	};

};
