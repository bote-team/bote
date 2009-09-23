#include "stdafx.h"
#include "NetworkHandler.h"
#include "Botf2.h"

CNetworkHandler::CNetworkHandler(CBotf2Doc *pDoc) : m_pDoc(pDoc)
{
	ASSERT(pDoc);
}

CNetworkHandler::~CNetworkHandler()
{
}

void CNetworkHandler::OnPrepareGame()
{
	// Server: Spieldaten vorbereiten, an Clients senden
	server.NextRound(m_pDoc);
}

void CNetworkHandler::OnCalculateGame()
{
	using namespace network;
	// Server: alle EndOfRound-Nachrichten eingetroffen

	// Daten aller Clients ins Server-Dokument deserialisieren
	BOOL bSomethingDone = FALSE;
	for (int i = RACE_FIRST; i < RACE_LAST; i++)
		if (server.DeserializeEndOfRoundToDoc((RACE)i, m_pDoc))
			bSomethingDone = TRUE;

	ASSERT(bSomethingDone);

	// wenn Daten vorhanden waren, nächste Runde berechnen, Daten an Clients senden
	if (!bSomethingDone) return;
	m_pDoc->NextRound();
	server.NextRound(m_pDoc);
}

void CNetworkHandler::OnNextRound(network::CNextRound *pMsg)
{
	ASSERT(pMsg);

	// Client: Rundendaten vom Server sind eingetroffen, in Dokument übernehmen,
	// Oberfläche zum Spielen wieder aktivieren
	pMsg->DeserializeToDoc(m_pDoc);
	// ...
	m_pDoc->m_bDataReceived = true;

	// wurde Rundenende geklickt zurücksetzen
	m_pDoc->m_bRoundEndPressed = false;
	AfxGetApp()->PostThreadMessage(WM_INITVIEWS, 0, 0);
	AfxGetApp()->PostThreadMessage(WM_UPDATEVIEWS, 0, 0);
	if (m_pDoc->m_iRound > 1)
		m_pDoc->m_pSoundManager->PlaySound(SNDMGR_SOUND_ENDOFROUND, SNDMGR_PRIO_HIGH);
	m_pDoc->m_pSoundManager->PlayMessages(600, 200);	
}

void CNetworkHandler::OnChatMsg(network::CChatMsg *pMsg)
{
	if (m_pDoc->m_bDataReceived)
	{
		((CBotf2App*)AfxGetApp())->GetChatDlg()->AddChatMsg(pMsg->GetSenderName(), pMsg->GetMessage());
		AfxGetApp()->PostThreadMessage(WM_SHOWCHATDLG, 0, 0);
	}
}

void CNetworkHandler::OnClientLost(const CString &strUserName)
{
	CString msg;
	msg.Format("Connection to client \"%s\" lost.", strUserName);
	AfxMessageBox(msg, MB_ICONINFORMATION | MB_OK);
}
