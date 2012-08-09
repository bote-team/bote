// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Botf2.h"
#include "MainDlg.h"
#include "Botf2Doc.h"
#include "LANClient.h"
#include "LANServer.h"


IMPLEMENT_DYNAMIC(CMainDlg, CPropertySheet)

BEGIN_MESSAGE_MAP(CMainDlg, CPropertySheet)
	ON_MESSAGE(WM_USERMSG, CMainDlg::OnUserMsg)
END_MESSAGE_MAP()

// CMainDlg

CMainDlg::CMainDlg(CBotf2Doc *pDoc)
{
	m_pDoc = pDoc;
	ASSERT(m_pDoc);
	m_pNewGame = new CNewGamePage();
	ASSERT(m_pNewGame);
	AddPage(m_pNewGame);

	m_pChooseRace = new CChooseRacePage();
	ASSERT(m_pChooseRace);
	AddPage(m_pChooseRace);

//	m_pPlayGame = new CPlayGamePage();
//	ASSERT(m_pPlayGame);
//	AddPage(m_pPlayGame);

	SetWizardMode();

	// wollen Benachrichtigungen über Netzwerkereignisse erhalten
	server.AddServerListener(this);
	client.AddClientListener(this);

	clientPublisher.Start(7777);
}

CMainDlg::~CMainDlg()
{
	if (m_pNewGame) delete m_pNewGame;
	if (m_pChooseRace) delete m_pChooseRace;
//	if (m_pPlayGame) delete m_pPlayGame;

	client.RemoveClientListener(this);
	server.RemoveServerListener(this);
}

// CMainDlg message handlers

BOOL CMainDlg::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	// CChooseRacePage einmal öffnen, damit die Steuerelemente existieren
	SetActivePage(1);
	SetActivePage(0);

	// Hilfe-Button wieder entfernen, andere Buttons verschieben
/*	CWnd *helpBtn = GetDlgItem(IDHELP);
	if (helpBtn)
	{
		CWnd *cancelBtn = GetDlgItem(IDCANCEL);
		ASSERT(cancelBtn);

		CRect rect1, rect2;
		cancelBtn->GetWindowRect(&rect1);
		helpBtn->GetWindowRect(&rect2);

		int dx = rect2.right - rect1.right;

		helpBtn->ShowWindow(SW_HIDE);
		rect1.MoveToX(rect1.left + dx);
		ScreenToClient(&rect1);
		cancelBtn->MoveWindow(rect1);

		CWnd *backBtn = GetDlgItem(ID_WIZBACK);
		if (backBtn)
		{
			backBtn->GetWindowRect(&rect1);
			rect1.MoveToX(rect1.left + dx);
			ScreenToClient(&rect1);
			backBtn->MoveWindow(rect1);
		}

		CWnd *nextBtn = GetDlgItem(ID_WIZNEXT);
		if (nextBtn)
		{
			nextBtn->GetWindowRect(&rect1);
			rect1.MoveToX(rect1.left + dx);
			ScreenToClient(&rect1);
			nextBtn->MoveWindow(rect1);
		}

		CWnd *finishBtn = GetDlgItem(ID_WIZFINISH);
		if (finishBtn)
		{
			finishBtn->GetWindowRect(&rect1);
			rect1.MoveToX(rect1.left + dx);
			ScreenToClient(&rect1);
			finishBtn->MoveWindow(rect1);
		}
	} */

	return bResult;
}

LRESULT CMainDlg::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case UMSG_SETSERVER:
		m_pChooseRace->SetServer(lParam != 0);
		break;
	}
	return 0;
}

void CMainDlg::OnServerSocketError(int /*nError*/)
{
//	MessageBox(network::GetWSAErrorMsg(nError), "Server-Fehler", MB_ICONEXCLAMATION | MB_OK);
}

void CMainDlg::OnClientSocketError(int /*nError*/)
{
//	MessageBox(network::GetWSAErrorMsg(nError), "Client-Fehler", MB_ICONEXCLAMATION | MB_OK);
}

void CMainDlg::OnClientChangedRaces()
{
	// Client: Zuordnung der Völker hat sich geändert
	m_pChooseRace->EnableRaceButtons();
}

void CMainDlg::OnClientDisconnected()
{
	// Client: die Verbindung zum Server wurde unterbrochen

	// evtl. Server anhalten
	serverPublisher.StopPublishing();
	server.Stop();

	// zur ersten Seite zurückkehren
	SetActivePage(0);
	MessageBox("Verbindung zum Server verloren.", "Fehler", MB_ICONEXCLAMATION | MB_OK);
}

void CMainDlg::OnBeginGame(network::CBeginGame *pMsg)
{
	// Client: der Server hat das Spiel begonnen
	// bei Clients, auf denen nicht der Server läuft, von selbst auf die nächste Page wechseln
	pMsg->DeserializeToDoc((network::CPeerData*)GetClientDocument());
	if (!m_pChooseRace->IsServer())
	{
		GetClientDocument()->m_bDontExit = true;
		PostMessage(WM_CLOSE);
	}
}

void CMainDlg::OnPrepareGame()
{
}

void CMainDlg::OnNextRound(network::CNextRound* /*pMsg*/)
{
}

void CMainDlg::OnCalculateGame()
{
}

void CMainDlg::OnChatMsg(network::CChatMsg *pMsg)
{
	ASSERT(pMsg);
	switch (GetActiveIndex())
	{
	case 1:
		m_pChooseRace->AddChatMsg(pMsg->GetSenderName(), pMsg->GetMessage());
		break;

	case 2:
//		m_pPlayGame->AddChatMsg(pMsg);
		break;
	}
}
