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
	m_pDoc->m_bDataReceived = false;
	pMsg->DeserializeToDoc(m_pDoc);

	int nMsg = WM_UPDATEVIEWS;
	// findet ein Kampf statt oder wurde die Runde ganz normal beendet
	if (m_pDoc->m_bCombatCalc)
		nMsg = WM_COMBATVIEW;
	else if (m_pDoc->m_bNewGame)
		nMsg = WM_INITVIEWS;

	// wurde schon ein Fenster erstellt, so wird die Nachricht der Aktualisierung an das Hauptfenster
	// geschickt
	CWnd* pWnd = ((CBotf2App*)AfxGetApp())->GetMainWnd();
	if (pWnd)
		pWnd->PostMessage(nMsg, 0, 0);
	// gibt es noch kein Fenster (direkt beim Spielstart bzw. Laden), so geht die Message
	// an die Application und wird automatisch geschickt, sobald das Fenster da ist
	else
	{
		// zu Beginn etwas warten, so dass Zeit bleibt die Nachrichten zu empfangen
		Sleep(100);
		AfxGetApp()->PostThreadMessage(nMsg, 0, 0);
		// hier schon setzen, da die Funktion erst später aufgerufen wird und man sonst nicht
		// beginnen kann
		m_pDoc->m_bDataReceived = true;
	}

	// warten bis die Views fertig sind bDataReceived auf True gesetzt haben
	while (!m_pDoc->m_bDataReceived)
		Sleep(50);

	// bei einer Kampfansicht werden nicht die Nachrichten abgespielt, die kommen nur, wenn
	// die neue Runde komplett beginnt
	if (nMsg == WM_UPDATEVIEWS)
	{
		CSoundManager* pSoundManager = CSoundManager::GetInstance();
		ASSERT(pSoundManager);

		if (m_pDoc->GetCurrentRound() > 1)
			pSoundManager->PlaySound(SNDMGR_SOUND_ENDOFROUND, SNDMGR_PRIO_HIGH);
		pSoundManager->PlayMessages(600, 200);

		MYTRACE("general")(MT::LEVEL_INFO, "CNetworkHandler::OnNextRound ready\n");
	}
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
