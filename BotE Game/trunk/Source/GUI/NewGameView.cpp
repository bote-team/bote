// NewGameView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "resource.h"
#include "NewGameView.h"
#include "ChooseRaceView.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "MainFrm.h"

#include "Graphic\memdc.h"
#include "GraphicPool.h"
#include "IniLoader.h"

#include "BotEServer.h"
#include "LANServer.h"

#include "SearchServersDlg.h"
// CNewGameView

#define BTN_NEXT	1
#define BTN_BACK	2

IMPLEMENT_DYNCREATE(CNewGameView, CFormView)

CNewGameView::CNewGameView()
: CFormView(CNewGameView::IDD), m_nMode(MODE_SERVER), m_pBkgndImg()
{

}

CNewGameView::~CNewGameView()
{
}

void CNewGameView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_HOSTIP, m_hostIP);
	DDX_Control(pDX, IDC_HOSTPORT, m_hostPort);
	DDX_Control(pDX, IDC_SERVERPORT, m_serverPort);
	DDX_Control(pDX, IDC_USERNAME, m_username);
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_PUBLISH, m_btPublish);
	DDX_Control(pDX, IDC_SEARCHSERVERS, m_btSearchServers);
}

BEGIN_MESSAGE_MAP(CNewGameView, CFormView)
	ON_BN_CLICKED(BTN_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(BTN_BACK, OnBnClickedBack)
	ON_EN_CHANGE(IDC_HOSTPORT, OnEnChangeHostport)
	ON_EN_CHANGE(IDC_SERVERPORT, OnEnChangeServerport)
	ON_BN_CLICKED(IDC_PUBLISH, OnBnClickedPublish)
	ON_EN_CHANGE(IDC_USERNAME, OnEnChangeUsername)
	ON_EN_CHANGE(IDC_DESCRIPTION, OnEnChangeDescription)
	ON_BN_CLICKED(IDC_SEARCHSERVERS, OnBnClickedSearchservers)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CNewGameView-Diagnose

#ifdef _DEBUG
void CNewGameView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CNewGameView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNewGameView-Meldungshandler
void CNewGameView::OnDraw(CDC* dc)
{
	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	ASSERT((CBotf2Doc*)GetDocument());

	CMyMemDC pDC(dc);
	CRect clientRect;
	GetClientRect(&clientRect);

	double dSizeModX = m_TotalSize.cx / (double)clientRect.Width();
	double dSizeModY = m_TotalSize.cy / (double)clientRect.Height();

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());

	g.Clear(Color::Black);
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)clientRect.Width() / (REAL)m_TotalSize.cx, (REAL)clientRect.Height() / (REAL)m_TotalSize.cy);

	g.DrawImage(m_pBkgndImg, 0, 0, m_TotalSize.cx, m_TotalSize.cy);

	Gdiplus::Font font(L"Calibri", 14 * min(dSizeModX, dSizeModY), FontStyleBold);
	StringFormat format;
	format.SetAlignment(StringAlignmentFar);
	format.SetLineAlignment(StringAlignmentCenter);

	// dunklen Bereich um die Buttons zeichnen
	CRect rect;
	m_username.GetWindowRect(rect);
	int nRectTop = (rect.top - 25) * dSizeModY;
	m_btNext.GetWindowRect(rect);
	int nRectBottom = (rect.bottom + 25) * dSizeModY - nRectTop;
	Color clr(130, 0, 0, 0);
	g.FillRectangle(&Gdiplus::SolidBrush(clr), RectF(0, nRectTop, m_TotalSize.cx, nRectBottom));
	g.DrawRectangle(&Pen(Color(200,200,200), 1), RectF(0, nRectTop, m_TotalSize.cx, nRectBottom));

	// Spielername schreiben
	m_username.GetWindowRect(rect);
	CString sUserName = CResourceManager::GetString("PLAYERNAME");
	g.DrawString(CComBSTR(sUserName.AllocSysString()), -1, &font, RectF((rect.left - 400) * dSizeModX, rect.top * dSizeModY, 375 * dSizeModX, rect.Height() * dSizeModY), &format, &SolidBrush(Color::WhiteSmoke));

	if (m_nMode == MODE_SERVER)
	{
		// Servername schreiben
		CString sServerName = CResourceManager::GetString("SERVERNAME");
		m_description.GetWindowRect(rect);
		g.DrawString(CComBSTR(sServerName.AllocSysString()), -1, &font, RectF((rect.left - 400) * dSizeModX, rect.top * dSizeModY, 375 * dSizeModX, rect.Height() * dSizeModY), &format, &SolidBrush(Color::WhiteSmoke));

		// Serverport schreiben
		CString sServerPort = CResourceManager::GetString("PORT");
		m_serverPort.GetWindowRect(rect);
		g.DrawString(CComBSTR(sServerPort.AllocSysString()), -1, &font, RectF((rect.left - 400) * dSizeModX, rect.top * dSizeModY, 375 * dSizeModX, rect.Height() * dSizeModY), &format, &SolidBrush(Color::WhiteSmoke));

		// publish in local area network schreiben
		CString sPublishInLAN = CResourceManager::GetString("PUBLISHLAN");
		m_btPublish.GetWindowRect(rect);
		g.DrawString(CComBSTR(sPublishInLAN.AllocSysString()), -1, &font, RectF((rect.left - 400) * dSizeModX, rect.top * dSizeModY, 375 * dSizeModX, rect.Height() * dSizeModY), &format, &SolidBrush(Color::WhiteSmoke));
	}
	else if (m_nMode == MODE_CLIENT)
	{
		// IP-Adresse schreiben
		CString sIPAddr = CResourceManager::GetString("SERVERIP");
		m_hostIP.GetWindowRect(rect);
		g.DrawString(CComBSTR(sIPAddr.AllocSysString()), -1, &font, RectF((rect.left - 400) * dSizeModX, rect.top * dSizeModY, 375 * dSizeModX, rect.Height() * dSizeModY), &format, &SolidBrush(Color::WhiteSmoke));

		// Hostport schreiben
		CString sHostPort = CResourceManager::GetString("PORT");
		m_hostPort.GetWindowRect(rect);
		g.DrawString(CComBSTR(sHostPort.AllocSysString()), -1, &font, RectF((rect.left - 400) * dSizeModX, rect.top * dSizeModY, 375 * dSizeModX, rect.Height() * dSizeModY), &format, &SolidBrush(Color::WhiteSmoke));
	}

	Gdiplus::Font font2(L"Calibri", 10, FontStyleBold);
	format.SetAlignment(StringAlignmentFar);
	format.SetLineAlignment(StringAlignmentFar);

	CString sVersion = "Birth of the Empires Pre-Alpha7 V";
	sVersion += VERSION_INFORMATION;
	sVersion += "\n© by Sir Pustekuchen 2011";
	g.DrawString(CComBSTR(sVersion.AllocSysString()), -1, &font2, RectF(0, 0, m_TotalSize.cx, m_TotalSize.cy), &format, &SolidBrush(Color::WhiteSmoke));

	g.ReleaseHDC(pDC->GetSafeHdc());
}

void CNewGameView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	m_pBkgndImg = pDoc->GetGraphicPool()->GetGDIGraphic("Events\\Startmenu.boj");
	m_TotalSize = CSize(1280, 1024);

	CRect clientRect;
	GetDesktopWindow()->GetClientRect(clientRect);

	//double nButtonSizeX = 300.0;
	//double nButtonSizeY = 40.0;

	//double nXPos = clientRect.Width() / 2.0 - nButtonSizeX / 2.0;
	double nYPos = clientRect.Height() / 2.0 - 175.0;

	//double nGab = 10.0;

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

	// auflösungsabhängige Font erstellen
	CFont fnt;
	fnt.CreateFont(17, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Calibri");

	// Set control positions
	m_username.SetWindowPos(NULL, clientRect.Width() / 2.0 - 100, nYPos, 200, 25, SWP_FRAMECHANGED);
	m_username.SetFont(&fnt);

	// Controls nur für Server
	m_description.SetWindowPos(NULL, clientRect.Width() / 2.0 - 100, nYPos + 35, 200, 25, SWP_FRAMECHANGED);
	m_description.SetFont(&fnt);

	m_serverPort.SetWindowPos(NULL, clientRect.Width() / 2.0 - 100, nYPos + 70, 200, 25, SWP_FRAMECHANGED);
	m_serverPort.SetFont(&fnt);

	m_btPublish.SetWindowPos(NULL, clientRect.Width() / 2.0 - 100, nYPos + 105, 14, 14, SWP_FRAMECHANGED);
	m_btPublish.SetFont(&fnt);

	// Controls nur für Clients
	m_hostIP.SetWindowPos(NULL, clientRect.Width() / 2.0 - 100, nYPos + 35, 200, 25, SWP_FRAMECHANGED);
	m_hostIP.SetFont(&fnt);

	m_btSearchServers.SetWindowPos(NULL, clientRect.Width() / 2.0 + 110, nYPos + 35, 175, 25, SWP_FRAMECHANGED);
	SetButtonStyle(&m_btSearchServers);
	m_btSearchServers.SetWindowText(CResourceManager::GetString("SEARCHLANSERVER"));

	m_hostPort.SetWindowPos(NULL, clientRect.Width() / 2.0 - 100, nYPos + 70, 200, 25, SWP_FRAMECHANGED);
	m_hostPort.SetFont(&fnt);

	// Create push buttons
	m_btBack.Create(_T(CResourceManager::GetString("BTN_BACK")), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(), this, BTN_BACK);
	m_btBack.SetWindowPos(NULL, clientRect.Width() / 2.0 - 160, nYPos + 175, 150, 30, SWP_FRAMECHANGED);
	SetButtonStyle(&m_btBack);

	m_btNext.Create(_T(CResourceManager::GetString("BTN_FURTHER")), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(), this, BTN_NEXT);
	m_btNext.SetWindowPos(NULL, clientRect.Width() / 2.0 + 10, nYPos + 175, 150, 30, SWP_FRAMECHANGED);
	SetButtonStyle(&m_btNext);

	// Handle auf Font freigeben
	fnt.Detach();

	// Anfangswerte setzen
	m_btPublish.SetCheck(TRUE);

	CString userName = "";
	CIniLoader::GetInstance()->ReadValue("General", "USERNAME", userName);
	if (!userName.IsEmpty())
		SetDlgItemText(IDC_USERNAME, userName);
	else
	{
		TCHAR userName[UNLEN + 1];
		DWORD nLen = UNLEN + 1;
		VERIFY(GetUserName(userName, &nLen));
		SetDlgItemText(IDC_USERNAME, userName);
	}
	GetDlgItemText(IDC_USERNAME, userName);
	CIniLoader::GetInstance()->WriteValue("General", "USERNAME", userName);

	m_hostIP.SetAddress(INADDR_LOOPBACK);
	m_hostPort.SetWindowText("7777");
	m_serverPort.SetWindowText("7777");
	CString strDesc;
	strDesc.Format("%ss BotE-Server", userName);
	m_description.SetWindowText(strDesc);

	SetMode(MODE_SERVER);

	EnableNextButton();
}

BOOL CNewGameView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	return FALSE;
//	return CFormView::OnEraseBkgnd(pDC);
}

void CNewGameView::SetButtonStyle(CRoundButton2* pBtn)
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

void CNewGameView::StartServer(bool bPublish)
{
	CString sName = "";
	CIniLoader::GetInstance()->ReadValue("General", "USERNAME", sName);

	BOOL trans;
	UINT nPort = GetDlgItemInt(IDC_SERVERPORT, &trans, FALSE);
	ASSERT(trans && nPort && nPort <= 65535);

	// Server starten
	if (!server.Start(nPort))
	{
		MessageBox(CResourceManager::GetString("SERVERERROR3"), CResourceManager::GetString("ERROR"), MB_ICONEXCLAMATION | MB_OK);
		SetCursor(::LoadCursor(NULL, IDC_ARROW));
		return;
	}

	// Client zu 127.0.0.1:nPort verbinden
	if (!client.Connect(INADDR_LOOPBACK, nPort, sName))
	{
		server.Stop();
		MessageBox(CResourceManager::GetString("SERVERERROR4"), CResourceManager::GetString("ERROR"), MB_ICONEXCLAMATION | MB_OK);
		SetCursor(::LoadCursor(NULL, IDC_ARROW));
		return;
	}

	// Veröffentlichung starten
	if (bPublish)
	{
		CString strDescription;
		m_description.GetWindowText(strDescription);
		strDescription.Trim();

		if (!serverPublisher.StartPublishing(7777, strDescription, nPort))
		{
			// Vorgang nicht abbrechen, nur Warnhinweis ausgeben
			MessageBox(CResourceManager::GetString("SERVERERROR5"), CResourceManager::GetString("WARNING"), MB_ICONWARNING | MB_OK);
		}
	}
	else
		serverPublisher.StopPublishing();

	// Umschalten zur Rassenauswahlansicht
	ShowChooseRaceView(true);
}

void CNewGameView::LoadGame(const CString& sPath)
{
	SetMode(MODE_LOAD);

	m_sFileName = sPath;
	if (CheckValues())
	{
		// Datei laden
		CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
		ASSERT(pDoc);

		if (!pDoc->OnOpenDocument(m_sFileName))
		{
			MessageBox(CResourceManager::GetString("LOADERROR"), CResourceManager::GetString("ERROR"),	MB_ICONEXCLAMATION | MB_OK);
			m_sFileName = "";
			return;
		}

		pDoc->m_bGameLoaded = true;
		Sleep(250);
		StartServer(true);
	}
}

#define SHOW_IF(cond)	((cond) ? SW_SHOW : SW_HIDE)

void CNewGameView::SetMode(MODE mode)
{
	m_nMode = mode;

	BOOL bActive = (mode == MODE_CLIENT);
	m_hostIP.ShowWindow(SHOW_IF(bActive));
	m_hostPort.ShowWindow(SHOW_IF(bActive));
	m_btSearchServers.ShowWindow(SHOW_IF(bActive));

	bActive = (mode == MODE_SERVER || mode == MODE_LOAD);
	m_serverPort.ShowWindow(SHOW_IF(bActive));
	m_btPublish.ShowWindow(SHOW_IF(bActive));
	m_description.ShowWindow(SHOW_IF(bActive));

	m_description.EnableWindow(m_btPublish.GetCheck());
}

BOOL CNewGameView::CheckValues()
{
	// Benutzername
	CString userName;
	m_username.GetWindowText(userName);
	userName.Trim();
	if (!userName.GetLength())
		return FALSE;

	// Beschreibung
	if ((m_nMode == MODE_SERVER || m_nMode == MODE_LOAD) && m_btPublish.GetCheck())
	{
		CString strDescription;
		m_description.GetWindowText(strDescription);
		strDescription.Trim();
		if (strDescription.IsEmpty())
			return FALSE;
	}

	switch (m_nMode)
	{
	case MODE_CLIENT:
		{
		// Port
		BOOL trans;
		UINT nPort = GetDlgItemInt(IDC_HOSTPORT, &trans, FALSE);
		if (!trans || !nPort || nPort > 65535) return FALSE;

		DWORD dwAddr;
		m_hostIP.GetAddress(dwAddr);
		return (dwAddr != 0 && (dwAddr & 0xFF) != 0xFF);
		}

	case MODE_SERVER:
		{
		// Port
		BOOL trans;
		UINT nPort = GetDlgItemInt(IDC_SERVERPORT, &trans, FALSE);
		return (trans && nPort && nPort <= 65535);
		}

	case MODE_LOAD:
		{
		// Port
		BOOL trans;
		UINT nPort = GetDlgItemInt(IDC_SERVERPORT, &trans, FALSE);
		if (!trans || !nPort || nPort > 65535) return FALSE;

		return !m_sFileName.IsEmpty();
		}
	}

	return FALSE;
}

void CNewGameView::EnableNextButton()
{
	m_btNext.EnableWindow(CheckValues());
}

void CNewGameView::OnEnChangeHostport()
{
	EnableNextButton();
}

void CNewGameView::OnEnChangeServerport()
{
	EnableNextButton();
}

void CNewGameView::OnBnClickedPublish()
{
	m_description.EnableWindow((m_nMode == MODE_SERVER || m_nMode == MODE_LOAD) && m_btPublish.GetCheck());
	EnableNextButton();
}

void CNewGameView::OnEnChangeUsername()
{
	// Name in Ini-Datei schreiben
	CString userName;
	m_username.GetWindowText(userName);
	userName.Trim();
	CIniLoader::GetInstance()->WriteValue("General", "USERNAME", userName);

	EnableNextButton();
}

void CNewGameView::OnEnChangeDescription()
{
	EnableNextButton();
}

void CNewGameView::OnBnClickedSearchservers()
{
	CSearchServersDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_hostIP.SetAddress(dlg.m_dwIP);
		CString strPort;
		strPort.Format("%u", dlg.m_nPort);
		m_hostPort.SetWindowText(strPort);
	}
}

void CNewGameView::OnBnClickedNext()
{
	if (m_nMode == MODE_SERVER)
	{
		StartServer(m_btPublish.GetCheck() == BST_CHECKED);
		return;
	}

	if (m_nMode == MODE_CLIENT)
	{
		// Benutzername
		CString userName;
		m_username.GetWindowText(userName);
		userName.Trim();
		if (!userName.GetLength())
			return;

		DWORD dwAddr;
		m_hostIP.GetAddress(dwAddr);
		ASSERT(dwAddr != 0 && (dwAddr & 0xFF) != 0xFF);

		BOOL trans;
		UINT nPort = GetDlgItemInt(IDC_HOSTPORT, &trans, FALSE);
		ASSERT(trans && nPort && nPort <= 65535);

		// Client zum angegebenen Server verbinden
		if (!client.Connect(dwAddr, nPort, userName))
		{
			MessageBox(CResourceManager::GetString("SERVERERROR4"), CResourceManager::GetString("ERROR"), MB_ICONEXCLAMATION | MB_OK);
			SetCursor(::LoadCursor(NULL, IDC_ARROW));
			return;
		}
		else
		{
			// Umschalten zur Rassenauswahlansicht
			ShowChooseRaceView(false);
		}
	}
}

void CNewGameView::OnBnClickedBack()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	if (!pDoc)
		return;

	pDoc->GetMainFrame()->SelectMainView(START_VIEW);
}

void CNewGameView::ShowChooseRaceView(bool bIsServer) const
{
	// Umschalten zur Rassenauswahlansicht
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	if (!pDoc)
		return;

	ASSERT(pDoc->GetMainFrame());
	if (!pDoc->GetMainFrame())
		return;

	pDoc->GetMainFrame()->SelectMainView(CHOOSERACE_VIEW);

	CChooseRaceView* m_pChooseRaceView = dynamic_cast<CChooseRaceView*>(pDoc->GetMainFrame()->GetView(RUNTIME_CLASS(CChooseRaceView)));
	ASSERT(m_pChooseRaceView);
	if (m_pChooseRaceView)
	{
		m_pChooseRaceView->SetServer(bIsServer);
		m_pChooseRaceView->EnableRaceButtons();
	}
}
