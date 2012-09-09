#include "StdAfx.h"
#include "LANServer.h"

network::CLANServer serverPublisher;

namespace network
{
	CLANServer::CLANServer() : m_pConnection(NULL), m_strDescription(""), m_nTCPPort(0)
	{
	}

	CLANServer::~CLANServer()
	{
	}

	BOOL CLANServer::StartPublishing(UINT nServerPort, const CString &strDescription, UINT nTCPPort)
	{
		if (m_pConnection) return FALSE;

		m_strDescription = strDescription;
		m_nTCPPort = nTCPPort;

		m_pConnection = new CLANConnection(nServerPort, this);
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

	void CLANServer::StopPublishing()
	{
		if (!m_pConnection) return;

		// Thread anhalten, Objekt zerstören
		m_pConnection->Interrupt();
		m_pConnection->Join();
		delete m_pConnection;

		m_pConnection = NULL;
	}

	void CLANServer::OnMessageReceived(const CReference<CLANMessage> &message, CLANConnection *pConnection)
	{
/*		CString msg;
		msg.Format("LANServer: message %u from %X:%u received\n",
			message->GetId(), message->GetSenderIP(), message->GetSenderPort());
		TRACE(msg);
		AfxMessageBox(msg); */

		switch (message->GetId())
		{
		case LANID_WHOISTHERE:
			{
			TRACE("LANServer: WHOISTHERE received, sending HELLO to %s:%u\n",
				AddrToString(message->GetSenderIP()), message->GetSenderPort());

			// dem anfragenden Client mit HELLO antworten
			CReference<CLANMessage> response(new CLANMessage(LANID_HELLO));
			response->SetValue("description", m_strDescription);
			response->SetValue("port", m_nTCPPort);

			response->SetReceiver(message->GetSenderIP(), message->GetSenderPort());
			ASSERT(m_pConnection);
			m_pConnection->Send(response);
			break;
			}

		default:
			TRACE("LANServer: unhandled message received\n");
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

	void CLANServer::OnMessageSent(const CReference<CLANMessage> &message, CLANConnection *pConnection)
	{
		CString msg;
		msg.Format("LANServer: message sent\n");
		TRACE(msg);
		//AfxMessageBox(msg);
	}

	void CLANServer::OnMessageDiscarded(const CReference<CLANMessage> &message, CLANConnection *pConnection)
	{
		CString msg;
		msg.Format("LANServer: message discarded\n");
		TRACE(msg);
		//AfxMessageBox(msg);
	}

	void CLANServer::OnSocketError(int nWSAError, CLANConnection *pConnection)
	{
		CString msg;
		msg.Format("LANServer: %s\n", network::GetWSAErrorMsg(nWSAError));
		TRACE(msg);
		//AfxMessageBox(msg);
	}

	void CLANServer::OnConnectionLost(CLANConnection *pConnection)
	{
		CString msg;
		msg.Format("LANServer: connection lost\n");
		TRACE(msg);
		//AfxMessageBox(msg);
		//AfxMessageBox("Veröffentlichung nicht möglich.");
	}

};
