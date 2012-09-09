#include "StdAfx.h"
#include "LANClient.h"

network::CLANClient clientPublisher;

/// ruft die Funktion <code>FN</code> (evtl. inkl. Parameter) aller registrierter Listeners auf
#define TO_ALL_LISTENERS(FN) \
	do { for (POSITION pos = m_lListeners.GetHeadPosition(); pos; m_lListeners.GetNext(pos)) \
	m_lListeners.GetAt(pos)->FN; } while (false)

namespace network
{
	CLANClient::CLANClient() : m_pConnection(NULL), m_nServerPort(0)
	{
	}

	CLANClient::~CLANClient()
	{
	}

	void CLANClient::AddLANClientListener(CLANClientListener *pListener)
	{
		if (pListener && !m_lListeners.Find(pListener)) m_lListeners.AddTail(pListener);
	}

	void CLANClient::RemoveLANClientListener(CLANClientListener *pListener)
	{
		POSITION pos = m_lListeners.Find(pListener);
		if (pos) m_lListeners.RemoveAt(pos);
	}

	BOOL CLANClient::Start(UINT nServerPort)
	{
		if (m_pConnection) return FALSE;

		m_nServerPort = nServerPort;

		m_pConnection = new CLANConnection(0, this);
		int nError;
		if (!m_pConnection->CreateSocket(nError))
		{
			delete m_pConnection;
			m_pConnection = NULL;
			return FALSE;
		}
		VERIFY(m_pConnection->Start());

		return TRUE;
	}

	void CLANClient::Stop()
	{
		if (!m_pConnection) return;

		// Thread anhalten, Objekt zerstören
		m_pConnection->Interrupt();
		m_pConnection->Join();
		delete m_pConnection;

		m_pConnection = NULL;
	}

	void CLANClient::SearchServers()
	{
		if (!m_pConnection || !m_nServerPort)
		{
			ASSERT(FALSE);
			return;
		}

		CReference<CLANMessage> msg(new CLANMessage(LANID_WHOISTHERE));
		msg->SetReceiver(INADDR_BROADCAST, m_nServerPort);
		m_pConnection->Send(msg);
	}

	void CLANClient::OnMessageReceived(const CReference<CLANMessage> &message, CLANConnection *pConnection)
	{
/*		CString msg;
		msg.Format("LANClient: message %u from %X:%u received\n",
			message->GetId(), message->GetSenderIP(), message->GetSenderPort());
		TRACE(msg); */
		//AfxMessageBox(msg);

		switch (message->GetId())
		{
		case LANID_HELLO:
			{
			TRACE("LANClient: HELLO received from %s:%u\n", AddrToString(message->GetSenderIP()),
				message->GetSenderPort());
			CString strDescription = message->GetStringValue("description");
			UINT nTCPPort = message->GetUINTValue("port");
			ASSERT(!strDescription.IsEmpty() && nTCPPort > 0);
			TO_ALL_LISTENERS(OnServerFound(strDescription, message->GetSenderIP(), nTCPPort));
			break;
			}

		default:
			TRACE("LANClient: unhandled message received\n");
			ASSERT(FALSE);
			break;
		}

/*		TRACE("params: ");
		for (UINT i = 0; i < message->GetParamCount(); i++)
		{
			CString key = message->GetParamName(i);
			CString value = message->GetStringValue(key);
			TRACE("%s=\"%s\", ", key, value);
		}
		TRACE("\n"); */
	}

	void CLANClient::OnMessageSent(const CReference<CLANMessage> &message, CLANConnection *pConnection)
	{
		CString msg;
		msg.Format("LANClient: message sent\n");
		TRACE(msg);
		//AfxMessageBox(msg);
	}

	void CLANClient::OnMessageDiscarded(const CReference<CLANMessage> &message, CLANConnection *pConnection)
	{
		CString msg;
		msg.Format("LANClient: message discarded\n");
		TRACE(msg);
		//AfxMessageBox(msg);
	}

	void CLANClient::OnSocketError(int nWSAError, CLANConnection *pConnection)
	{
		CString msg;
		msg.Format("LANClient: %s\n", network::GetWSAErrorMsg(nWSAError));
		TRACE(msg);
		//AfxMessageBox(msg);
	}

	void CLANClient::OnConnectionLost(CLANConnection *pConnection)
	{
		CString msg;
		msg.Format("LANClient: connection lost\n");
		TRACE(msg);
		//AfxMessageBox(msg);
	}

};
