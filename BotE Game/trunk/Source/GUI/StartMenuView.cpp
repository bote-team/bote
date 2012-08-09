// StartMenuView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "resource.h"

#include "StartMenuView.h"
#include "ChooseRaceView.h"
#include "NewGameView.h"

#include "Botf2Doc.h"
#include "MainFrm.h"
#include "IniLoader.h"
#include "SettingsDlg.h"

#include "Graphic\memdc.h"
#include "GraphicPool.h"
#include "SoundManager.h"
#include "IniLoader.h"
#include "HTMLStringBuilder.h"

#include "LANClient.h"
#include "LANServer.h"


#define NEWGAME		1
#define	MULTIPLAYER	2
#define LOADGAME	3
#define	OPTIONS		4
#define	EXITGAME	5

#define MP_JOIN		6
#define MP_CREATE	7

// CStartMenuView

IMPLEMENT_DYNCREATE(CStartMenuView, CFormView)

CStartMenuView::CStartMenuView() : CFormView(CStartMenuView::IDD), m_pChooseRaceView(NULL), m_pNewGameView(NULL)
{
	// wollen Benachrichtigungen über Netzwerkereignisse erhalten
	server.AddServerListener(this);
	client.AddClientListener(this);

	clientPublisher.Start(7777);

	m_nTimeCounter	= 0;
	m_nLogoTimer	= 0;
}

CStartMenuView::~CStartMenuView()
{
	client.RemoveClientListener(this);
	server.RemoveServerListener(this);

	clientPublisher.Stop();
}

void CStartMenuView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStartMenuView, CFormView)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(NEWGAME, OnBnClickedNewGame)
	ON_BN_CLICKED(MULTIPLAYER, OnBnClickedMultiplayer)
	ON_BN_CLICKED(LOADGAME, OnBnClickedLoadGame)
	ON_BN_CLICKED(OPTIONS, OnBnClickedOptions)
	ON_BN_CLICKED(EXITGAME, OnBnClickedExit)
	ON_BN_CLICKED(MP_CREATE, OnBnClickedCreateMP)
	ON_BN_CLICKED(MP_JOIN, OnBnClickedJoinMP)

	ON_MESSAGE(WM_USERMSG, CStartMenuView::OnUserMsg)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CStartMenuView-Diagnose

#ifdef _DEBUG
void CStartMenuView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CStartMenuView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CStartMenuView-Meldungshandler

void CStartMenuView::OnDraw(CDC* dc)
{
	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	ASSERT((CBotf2Doc*)GetDocument());

	CMyMemDC pDC(dc);
	CRect client;
	GetClientRect(&client);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());

	g.Clear(Color::Black);
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);

	// zum Start nur einen schwarzen Hintergrund mit Welcome Nachricht anzeigen
	if (m_nLogoTimer < 2)
	{
		Gdiplus::Font font(L"Calibri", 25, FontStyleBold);
		StringFormat format;
		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentCenter);

		CString s= CResourceManager::GetString("PRESENTEDBY")+"\n\n"+CResourceManager::GetString("PRESENTED");
		//CString s = "§www.birth-of-the-empires.de\n\npräsentiert ...";
		g.DrawString(CComBSTR(s), -1, &font, RectF(0, 0, m_TotalSize.cx, m_TotalSize.cy), &format, &SolidBrush(Color::WhiteSmoke));

		g.ReleaseHDC(pDC->GetSafeHdc());
		// nichts weiter Zeichnen
		return;
	}

	// Hintergrundbild anzeigen
	g.DrawImage(m_pBkgndImg, 0, 0, m_TotalSize.cx, m_TotalSize.cy);

	Gdiplus::Font font(L"Calibri", 10, FontStyleBold);
	StringFormat format;
	format.SetAlignment(StringAlignmentFar);
	format.SetLineAlignment(StringAlignmentFar);

	CString sVersion = "Birth of the Empires Pre-Alpha7 V";
	sVersion += VERSION_INFORMATION;
	sVersion += "\n© by Sir Pustekuchen 2011";
	g.DrawString(CComBSTR(sVersion), -1, &font, RectF(0, 0, m_TotalSize.cx, m_TotalSize.cy), &format, &SolidBrush(Color::WhiteSmoke));

	// Hintergrundbild langsam einblenden
	if (m_nTimeCounter < 255)
	{
		Color clr(255 - m_nTimeCounter, 0, 0, 0);
		g.FillRectangle(&Gdiplus::SolidBrush(clr), RectF(0, 0, m_TotalSize.cx, m_TotalSize.cy));

		Gdiplus::Font font(L"Calibri", 25, FontStyleBold);
		StringFormat format;
		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentCenter);

		Color clr2(max(0, 255 - m_nTimeCounter * 1.2), 245,245,245);
		CString s= CResourceManager::GetString("PRESENTEDBY")+"\n\n"+CResourceManager::GetString("PRESENTED");
		g.DrawString(CComBSTR(s), -1, &font, RectF(0, 0, m_TotalSize.cx, m_TotalSize.cy), &format, &SolidBrush(clr2));
	}

	g.ReleaseHDC(pDC->GetSafeHdc());
}

void CStartMenuView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	m_pBkgndImg = pDoc->GetGraphicPool()->GetGDIGraphic("Events\\Startmenu.boj");
	ASSERT(m_pBkgndImg);

	m_pChooseRaceView = dynamic_cast<CChooseRaceView*>(pDoc->GetMainFrame()->GetView(RUNTIME_CLASS(CChooseRaceView)));
	ASSERT(m_pChooseRaceView);

	m_pNewGameView = dynamic_cast<CNewGameView*>(pDoc->GetMainFrame()->GetView(RUNTIME_CLASS(CNewGameView)));
	ASSERT(m_pNewGameView);

	// Immer im Vollbild anzeigen
	pDoc->GetMainFrame()->FullScreenMainView(true);

	m_TotalSize = CSize(1280, 1024);

	CRect clientRect;
	GetDesktopWindow()->GetClientRect(clientRect);

	double nButtonSizeX = 300.0;
	double nButtonSizeY = 40.0;

	double nXPos = clientRect.Width() / 2.0 - nButtonSizeX / 2.0;
	double nYPos = clientRect.Height() / 2.0 - 150.0;

	double nGab = 10.0;

	// Structure containing Style
	tButtonStyle tStyle;
	//////////////////////////////////////////////////////////////////////////

	// Get default Style
	m_tButtonStyle.GetButtonStyle(&tStyle);
	// Change Radius of Button
	tStyle.m_dRadius = 1.0;

	// Change Color Schema of Button
	tStyle.m_tColorFace.m_tEnabled		= RGB(125, 125, 125);
	tStyle.m_tColorBorder.m_tEnabled	= RGB(220, 220, 220);

	tStyle.m_tColorFace.m_tClicked		= RGB(75, 75, 75);
	tStyle.m_tColorBorder.m_tClicked	= RGB(25, 25, 25);

	tStyle.m_tColorFace.m_tPressed		= RGB(75, 75, 75);
	tStyle.m_tColorBorder.m_tPressed	= RGB(25, 25, 25);

	tStyle.m_tColorFace.m_tHot			= RGB(0x80, 0x80, 0xFF);
	tStyle.m_tColorBorder.m_tHot		= RGB(0x40, 0x40, 0xFF);

	// Set Style again
	m_tButtonStyle.SetButtonStyle(&tStyle);

	// Create a push button.
	m_btNewGame.Create(_T(CResourceManager::GetString("NEWGAME")), WS_CHILD|BS_PUSHBUTTON|WS_DISABLED, CRect(nXPos, nYPos, nXPos + nButtonSizeX, nYPos + nButtonSizeY), this, NEWGAME);
	m_btMultiplayer.Create(_T(CResourceManager::GetString("MULTIPLAYER")), WS_CHILD|BS_PUSHBUTTON|WS_DISABLED, CRect(nXPos, nYPos + nGab + nButtonSizeY, nXPos + nButtonSizeX, nYPos + nGab + nButtonSizeY + nButtonSizeY), this, MULTIPLAYER);
	m_btLoadGame.Create(_T(CResourceManager::GetString("LOADGAME")), WS_CHILD|BS_PUSHBUTTON|WS_DISABLED, CRect(nXPos, nYPos + (nGab + nButtonSizeY) * 2, nXPos + nButtonSizeX, nYPos + (nGab + nButtonSizeY) * 2 + nButtonSizeY), this, LOADGAME);
	m_btOptions.Create(_T(CResourceManager::GetString("SETTINGS")), WS_CHILD|BS_PUSHBUTTON|WS_DISABLED, CRect(nXPos, nYPos + (nGab + nButtonSizeY) * 3, nXPos + nButtonSizeX, nYPos + (nGab + nButtonSizeY) * 3 + nButtonSizeY), this, OPTIONS);
	m_btExit.Create(_T(CResourceManager::GetString("LEAVE")), WS_CHILD|BS_PUSHBUTTON|WS_DISABLED, CRect(nXPos, nYPos + (nGab + nButtonSizeY) * 4, nXPos + nButtonSizeX, nYPos + (nGab + nButtonSizeY) * 4 + nButtonSizeY), this, EXITGAME);

	m_btMPServer.Create(_T(CResourceManager::GetString("CREATEGAME")), WS_CHILD|BS_PUSHBUTTON|WS_DISABLED, CRect(nXPos, nYPos + (nGab + nButtonSizeY) * 1, nXPos + nButtonSizeX, nYPos + (nGab + nButtonSizeY) * 1 + nButtonSizeY), this, MP_CREATE);
	m_btMPClient.Create(_T(CResourceManager::GetString("JOINGAME")), WS_CHILD|BS_PUSHBUTTON|WS_DISABLED, CRect(nXPos, nYPos + (nGab + nButtonSizeY) * 2, nXPos + nButtonSizeX, nYPos + (nGab + nButtonSizeY) * 2 + nButtonSizeY), this, MP_JOIN);

	// Buttonstyle zuweisen
	for (int i = NEWGAME; i <= MP_CREATE; i++)
	{
		CRoundButton2* pBtn = dynamic_cast<CRoundButton2*>(GetDlgItem(i));
		if (!pBtn)
			continue;

		SetButtonStyle(pBtn);
	}

	// Für Tooltips registrieren
	CString sTooltip = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("NEWGAMETT"), _T("silver"));
	pDoc->GetMainFrame()->AddToTooltip(GetDlgItem(NEWGAME), sTooltip);
	sTooltip = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("MULTIPLAYERTT"), _T("silver"));
	pDoc->GetMainFrame()->AddToTooltip(GetDlgItem(MULTIPLAYER), sTooltip);
	sTooltip = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("LOADGAMETT"), _T("silver"));
	pDoc->GetMainFrame()->AddToTooltip(GetDlgItem(LOADGAME), sTooltip);
	sTooltip = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("SETTINGSTT"), _T("silver"));
	pDoc->GetMainFrame()->AddToTooltip(GetDlgItem(OPTIONS), sTooltip);
	sTooltip = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("LEAVETT"), _T("silver"));
	pDoc->GetMainFrame()->AddToTooltip(GetDlgItem(EXITGAME), sTooltip);

	sTooltip = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("CREATEGAMETT"), _T("silver"));
	pDoc->GetMainFrame()->AddToTooltip(GetDlgItem(MP_CREATE), sTooltip);
	sTooltip = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("JOINGAMETT"), _T("silver"));
	pDoc->GetMainFrame()->AddToTooltip(GetDlgItem(MP_JOIN), sTooltip);

	this->SetTimer(1,4000,NULL);
}

BOOL CStartMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	return FALSE;
	//return __super::OnEraseBkgnd(pDC);
}

void CStartMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	if (m_nTimeCounter < 255)
	{
		m_nTimeCounter = 255;
		this->SetTimer(1, 1, NULL);
	}

	CFormView::OnLButtonDown(nFlags, point);
}

void CStartMenuView::OnBnClickedNewGame()
{
	m_pNewGameView->SetMode(MODE_SERVER);
	m_pNewGameView->StartServer(false);
}

void CStartMenuView::OnBnClickedMultiplayer()
{
	ShowMPButtons(true);
}

void CStartMenuView::OnBnClickedCreateMP()
{
	// Umschalten zur Spiel starten Ansicht
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	pDoc->GetMainFrame()->SelectMainView(NEWGAME_VIEW);
	m_pNewGameView->SetMode(MODE_SERVER);

	ShowMPButtons(false);
}

void CStartMenuView::OnBnClickedJoinMP()
{
	// Umschalten zur Spiel starten Ansicht
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	pDoc->GetMainFrame()->SelectMainView(NEWGAME_VIEW);
	m_pNewGameView->SetMode(MODE_CLIENT);

	ShowMPButtons(false);
}

void CStartMenuView::OnBnClickedLoadGame()
{
	CFileDialog dlg(TRUE, "sav", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING,
		"BotE-Savegame (*.sav)|*.sav|All Files (*.*)|*.*||", this, 0);

	if (dlg.DoModal() == IDOK)
	{
		SetCursor(::LoadCursor(NULL, IDC_WAIT));
		m_pNewGameView->LoadGame(dlg.GetPathName());
		SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
}

void CStartMenuView::OnBnClickedOptions()
{
	CIniLoader* pIni = CIniLoader::GetInstance();
	int nOldSeed = -1;
	pIni->ReadValue("Special", "RANDOMSEED", nOldSeed);

	CSettingsDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		int nSeed = -1;
		pIni->ReadValue("Special", "RANDOMSEED", nSeed);

		if (nSeed != nOldSeed)
		{
			// festen vorgegeben Seed verwenden
			if (nSeed >= 0)
				srand(nSeed);
			// zufälligen Seed verwenden
			else
			{
				nSeed = (unsigned)time(NULL);
				srand(nSeed);
			}
			MYTRACE(MT::LEVEL_INFO, "Used seed for randomgenerator: %i", nSeed);
		}
	}
}

void CStartMenuView::OnBnClickedExit()
{
	// evtl. Server anhalten
	serverPublisher.StopPublishing();
	server.Stop();

	AfxGetApp()->GetMainWnd()->PostMessage(WM_CLOSE);
}

LRESULT CStartMenuView::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case UMSG_SETSERVER:
		m_pChooseRaceView->SetServer(lParam != 0);
		break;
	}
	return 0;
}

void CStartMenuView::OnServerSocketError(int /*nError*/)
{
//	MessageBox(network::GetWSAErrorMsg(nError), "Server-Fehler", MB_ICONEXCLAMATION | MB_OK);
}

void CStartMenuView::OnClientSocketError(int /*nError*/)
{
//	MessageBox(network::GetWSAErrorMsg(nError), "Client-Fehler", MB_ICONEXCLAMATION | MB_OK);
}

void CStartMenuView::OnClientChangedRaces()
{
	// Client: Zuordnung der Völker hat sich geändert
	m_pChooseRaceView->EnableRaceButtons();
}

void CStartMenuView::OnClientDisconnected()
{
	// Client: die Verbindung zum Server wurde unterbrochen
	// evtl. Server anhalten
	serverPublisher.StopPublishing();
	server.Stop();

	// zur ersten Seite zurückkehren
	if (!m_pChooseRaceView->IsServer())
	{
		CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
		if (!pDoc->m_bDontExit)
		{
			MessageBox(CResourceManager::GetString("SERVERERROR1"), CResourceManager::GetString("ERROR"), MB_ICONEXCLAMATION | MB_OK);
			pDoc->GetMainFrame()->SelectMainView(CHOOSERACE_VIEW);
		}
		else
		{
			MessageBox(CResourceManager::GetString("SERVERERROR2"), CResourceManager::GetString("ERROR"), MB_ICONEXCLAMATION | MB_OK);
			AfxGetApp()->GetMainWnd()->PostMessage(WM_CLOSE);
		}
	}
}

void CStartMenuView::OnBeginGame(network::CBeginGame *pMsg)
{
	// Client: der Server hat das Spiel begonnen
	// bei Clients, auf denen nicht der Server läuft, von selbst auf die nächste Page wechseln

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	if (!pDoc)
		return;

	pMsg->DeserializeToDoc((network::CPeerData*)pDoc);

	if (!m_pChooseRaceView->IsServer())
	{
		pDoc->m_bDontExit = true;
		PostMessage(WM_CLOSE);
	}
}

void CStartMenuView::OnPrepareGame()
{
}

void CStartMenuView::OnNextRound(network::CNextRound* /*pMsg*/)
{
}

void CStartMenuView::OnCalculateGame()
{
}

void CStartMenuView::OnChatMsg(network::CChatMsg *pMsg)
{
	ASSERT(pMsg);
	if (!pMsg)
		return;

	m_pChooseRaceView->AddChatMsg(pMsg->GetSenderName(), pMsg->GetMessage());
}

void CStartMenuView::SetButtonStyle(CRoundButton2* pBtn)
{
	if (!pBtn)
	{
		ASSERT(pBtn);
		return;
	}

	// Initialize the Buttons with global style
	pBtn->SetRoundButtonStyle(&m_tButtonStyle);
	/************************************************************************/
	/* Set Font size in big Button to 48                                    */
	/************************************************************************/
	LOGFONT tFont;
	pBtn->GetFont(&tFont);
	strcpy(tFont.lfFaceName, "Calibri");
	tFont.lfHeight = 18;
	tFont.lfWeight = FW_BLACK;
	tFont.lfQuality = CLEARTYPE_QUALITY;
	pBtn->SetFont(&tFont);

	tColorScheme tColor;
	pBtn->GetTextColor(&tColor);

	// Change Color
	tColor.m_tEnabled	= RGB(255, 255, 255);
	tColor.m_tClicked	= RGB(0x00, 0xAF, 0x00);
	tColor.m_tPressed	= RGB(50, 50, 50);

	// Set Text-Color
	pBtn->SetTextColor(&tColor);

	pBtn->SetHotButton(true);
}

void CStartMenuView::ShowMPButtons(bool bShow)
{
	if (bShow)
	{
		m_btNewGame.ShowWindow(SW_HIDE);
		m_btMultiplayer.ShowWindow(SW_HIDE);
		m_btLoadGame.ShowWindow(SW_HIDE);
		m_btOptions.ShowWindow(SW_HIDE);
		m_btExit.ShowWindow(SW_HIDE);

		m_btNewGame.EnableWindow(FALSE);
		m_btMultiplayer.EnableWindow(FALSE);
		m_btLoadGame.EnableWindow(FALSE);
		m_btOptions.EnableWindow(FALSE);
		m_btExit.EnableWindow(FALSE);

		m_btMPServer.ShowWindow(SW_SHOW);
		m_btMPClient.ShowWindow(SW_SHOW);

		m_btMPServer.EnableWindow(TRUE);
		m_btMPClient.EnableWindow(TRUE);
	}
	else
	{
		m_btNewGame.ShowWindow(SW_SHOW);
		m_btMultiplayer.ShowWindow(SW_SHOW);
		m_btLoadGame.ShowWindow(SW_SHOW);
		m_btOptions.ShowWindow(SW_SHOW);
		m_btExit.ShowWindow(SW_SHOW);

		m_btNewGame.EnableWindow(TRUE);
		m_btMultiplayer.EnableWindow(TRUE);
		m_btLoadGame.EnableWindow(TRUE);
		m_btOptions.EnableWindow(TRUE);
		m_btExit.EnableWindow(TRUE);

		m_btMPServer.ShowWindow(SW_HIDE);
		m_btMPClient.ShowWindow(SW_HIDE);

		m_btMPServer.EnableWindow(FALSE);
		m_btMPClient.EnableWindow(FALSE);
	}
}

void CStartMenuView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	m_nLogoTimer++;
	if (m_nLogoTimer < 2)
	{
		this->SetTimer(1,25,NULL);
		CIniLoader* pIni = CIniLoader::GetInstance();
		ASSERT(pIni);
		bool bUseMusic;
		pIni->ReadValue("Audio", "MUSIC", bUseMusic);
		if (bUseMusic)
		{
			CSoundManager* pSoundManager = CSoundManager::GetInstance();
			ASSERT(pSoundManager);
			float fMusicVolume;
			pIni->ReadValue("Audio", "MUSICVOLUME", fMusicVolume);
			pSoundManager->StartMusic(network::RACE_1, fMusicVolume);
		}
		return;
	}

	m_nTimeCounter++;
	m_nTimeCounter *= 1.025;
	if (m_nTimeCounter >= 255)
	{
		this->KillTimer(1);

		ShowMPButtons(false);
	}

	Invalidate(false);

	__super::OnTimer(nIDEvent);
}
