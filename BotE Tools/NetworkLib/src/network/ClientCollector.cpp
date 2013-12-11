#include "StdAfx.h"
#include "ClientCollector.h"

namespace network
{
	IMPLEMENT_DYNAMIC(CClientCollector, CThread)

	CClientCollector::CClientCollector(SOCKET hServer) : m_hServer(hServer)
	{
		ASSERT(hServer != INVALID_SOCKET);
	}

	CClientCollector::~CClientCollector()
	{
	}

	void CClientCollector::Run()
	{
		VERIFY(AfxSocketInit());

		TRACE("server: client collector is running\n");

		CAsyncSocket srvSocket;
		srvSocket.Attach(m_hServer);

		while (!IsInterrupted())
		{
			CSocket socket;
			if (srvSocket.Accept(socket))
			{
				// eingehende Verbindung
				server.OnClientCollected(socket.Detach());
			}
			else
			{
				int nError = srvSocket.GetLastError();
				if (nError != WSAEWOULDBLOCK) server.OnSocketError(nError);
			}

			Sleep(50);
		}

		srvSocket.Detach();

		TRACE("server: client collector stopped\n");
	}

};
