// NewGamePage.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "IniLoader.h"
#include "NewGamePage.h"
#include "SearchServersDlg.h"

#include "network.h"
#include "BotEServer.h"
#include "BotEClient.h"

#include "LANClient.h"
#include "LANServer.h"

#include "Lmcons.h"
#include "BotEDoc.h"
#include "SettingsDlg.h"

// CNewGamePage dialog

IMPLEMENT_DYNAMIC(CNewGamePage, CPropertyPage)

CNewGamePage::CNewGamePage()
	: CPropertyPage(CNewGamePage::IDD), m_pClientBtn(NULL), m_pServerBtn(NULL),
	m_pLoadBtn(), m_pChooseBtn(), m_pPublish(), m_pSearch(), m_pParent(NULL), m_nMode()
{
}

CNewGamePage::~CNewGamePage()
{
}

void CNewGamePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOSTIP, m_hostIP);
	DDX_Control(pDX, IDC_FILENAME, m_fileName);
	DDX_Control(pDX, IDC_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_HOSTPORT, m_hostPort);
	DDX_Control(pDX, IDC_SERVERPORT, m_serverPort);
}


BEGIN_MESSAGE_MAP(CNewGamePage, CPropertyPage)
	ON_BN_CLICKED(IDC_CLIENT, &CNewGamePage::OnBnClickedClient)
	ON_BN_CLICKED(IDC_SERVER, &CNewGamePage::OnBnClickedServer)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_HOSTIP, &CNewGamePage::OnChangedHostIP)
	ON_EN_CHANGE(IDC_USERNAME, &CNewGamePage::OnEnChangeUsername)
	ON_BN_CLICKED(IDC_LOAD, &CNewGamePage::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_CHOOSEFILE, &CNewGamePage::OnBnClickedChoosefile)
	ON_BN_CLICKED(IDC_PUBLISH, &CNewGamePage::OnBnClickedPublish)
	ON_EN_CHANGE(IDC_DESCRIPTION, &CNewGamePage::OnEnChangeDescription)
	ON_BN_CLICKED(IDC_SEARCHSERVERS, &CNewGamePage::OnBnClickedSearchservers)
	ON_EN_CHANGE(IDC_HOSTPORT, &CNewGamePage::OnEnChangeHostport)
	ON_EN_CHANGE(IDC_SERVERPORT, &CNewGamePage::OnEnChangeServerport)
	ON_BN_CLICKED(ID_SHOWOPTIONSDLG, &CNewGamePage::OnBnClickedShowoptionsdlg)
END_MESSAGE_MAP()


// CNewGamePage message handlers

BOOL CNewGamePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Zeiger auf Steuerelemente
	m_pClientBtn = (CButton *)GetDlgItem(IDC_CLIENT);
	AssertBotE(m_pClientBtn);
	m_pServerBtn = (CButton *)GetDlgItem(IDC_SERVER);
	AssertBotE(m_pServerBtn);
	m_pLoadBtn = (CButton *)GetDlgItem(IDC_LOAD);
	AssertBotE(m_pLoadBtn);
	m_pChooseBtn = (CButton *)GetDlgItem(IDC_CHOOSEFILE);
	AssertBotE(m_pChooseBtn);
	m_pPublish = (CButton *)GetDlgItem(IDC_PUBLISH);
	AssertBotE(m_pPublish);
	m_pSearch = (CButton *)GetDlgItem(IDC_SEARCHSERVERS);
	AssertBotE(m_pSearch);

	m_pParent = dynamic_cast<CMainDlg*>(GetParent());
	AssertBotE(m_pParent);

	// Anfangswerte setzen
	m_pPublish->SetCheck(TRUE);

/*	CString userName = "";
//	userName.Format("user_%u", rand());

	/*
	UINT r;
	for (int i = 0; i < 10; i++) r = (UINT)(((double)rand() / RAND_MAX) * 3);
	switch (r)
	{
	case 0:		userName = "CBot"; break;
	case 1:		userName = "SirPustekuchen"; break;
	case 2:		userName = "Rautzebautze"; break;
	}
	for (int i = 0; i < 10; i++) r = (UINT)(((double)rand() / RAND_MAX) * 100);
	userName.Format(userName + "_%u", r);
	/**/

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

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

#define SHOW_IF(cond)	((cond) ? SW_SHOW : SW_HIDE)

void CNewGamePage::SetMode(MODE mode)
{
	m_nMode = mode;

	BOOL bActive = (mode == MODE_CLIENT);
	m_pClientBtn->SetCheck(bActive);
	GetDlgItem(IDC_LBL_HOSTIP)->ShowWindow(SHOW_IF(bActive));
	m_hostIP.ShowWindow(SHOW_IF(bActive));
	GetDlgItem(IDC_LBL_HOSTPORT)->ShowWindow(SHOW_IF(bActive));
	m_hostPort.ShowWindow(SHOW_IF(bActive));
	m_pSearch->ShowWindow(SHOW_IF(bActive));

	bActive = (mode == MODE_SERVER);
	m_pServerBtn->SetCheck(bActive);

	bActive = (mode == MODE_LOAD);
	m_pLoadBtn->SetCheck(bActive);
	GetDlgItem(IDC_LBL_FILENAME)->ShowWindow(SHOW_IF(bActive));
	m_fileName.ShowWindow(SHOW_IF(bActive));
	m_pChooseBtn->ShowWindow(SHOW_IF(bActive));

	bActive = (mode == MODE_SERVER || mode == MODE_LOAD);
	GetDlgItem(IDC_LBL_SERVERPORT)->ShowWindow(SHOW_IF(bActive));
	m_serverPort.ShowWindow(SHOW_IF(bActive));
	m_pPublish->ShowWindow(SHOW_IF(bActive));
	GetDlgItem(IDC_LBL_DESCRIPTION)->ShowWindow(SHOW_IF(bActive));
	m_description.ShowWindow(SHOW_IF(bActive));

	m_description.EnableWindow(m_pPublish->GetCheck());
}

void CNewGamePage::OnBnClickedClient()
{
	SetMode(MODE_CLIENT);
	EnableWizardButtons();
}

void CNewGamePage::OnBnClickedServer()
{
	SetMode(MODE_SERVER);
	EnableWizardButtons();
}

BOOL CNewGamePage::OnSetActive()
{
	EnableWizardButtons();
	return CPropertyPage::OnSetActive();
}

BOOL CNewGamePage::CheckValues()
{
	// Benutzername
	CString userName;
	GetDlgItemText(IDC_USERNAME, userName);
	userName.Trim();
	if (!userName.GetLength()) return FALSE;

	// Beschreibung
	if ((m_nMode == MODE_SERVER || m_nMode == MODE_LOAD) && m_pPublish->GetCheck())
	{
		CString strDescription;
		m_description.GetWindowText(strDescription);
		strDescription.Trim();
		if (strDescription.IsEmpty()) return FALSE;
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

		CString fileName;
		GetDlgItemText(IDC_FILENAME, fileName);
		return !fileName.IsEmpty();
		}
	}

	return FALSE;
}

void CNewGamePage::EnableWizardButtons()
{
	DWORD dwFlags = 0;
	if (CheckValues()) dwFlags |= PSWIZB_NEXT;
	m_pParent->SetWizardButtons(dwFlags);
}

void CNewGamePage::OnChangedHostIP(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	EnableWizardButtons();
	*pResult = 0;
}

LRESULT CNewGamePage::OnWizardNext()
{
	SetCursor(::LoadCursor(NULL, IDC_WAIT));

	// Werte nochmals prüfen, bei Fehler die Seite nicht wechseln
	if (!CheckValues()) return -1;

	// Dokumentdaten zurücksetzen
//	m_pParent->GetServerDocument()->Reset();
//	m_pParent->GetClientDocument()->Reset();

	// Benutzername ermitteln
	CString name;
	GetDlgItemText(IDC_USERNAME, name);
	name.Trim();
	// Name in Ini-Datei schreiben
	CIniLoader::GetInstance()->WriteValue("General", "USERNAME", name);

	switch (m_nMode)
	{
	case MODE_CLIENT:
		{
		DWORD dwAddr;
		m_hostIP.GetAddress(dwAddr);
		AssertBotE(dwAddr != 0 && (dwAddr & 0xFF) != 0xFF);

		BOOL trans;
		UINT nPort = GetDlgItemInt(IDC_HOSTPORT, &trans, FALSE);
		AssertBotE(trans && nPort && nPort <= 65535);

		m_pParent->SendMessage(WM_USERMSG, UMSG_SETSERVER, 0);

		// Client zum angegebenen Server verbinden
		if (!client.Connect(dwAddr, nPort, name))
		{
			MessageBox("Es konnte keine Verbindung zum Server hergestellt werden.", "Fehler",
				MB_ICONEXCLAMATION | MB_OK);
			SetCursor(::LoadCursor(NULL, IDC_ARROW));
			return -1;
		}
		break;
		}

	case MODE_LOAD:
		{
		// Datei laden
		CString fileName;
		GetDlgItemText(IDC_FILENAME, fileName);
		m_pParent->GetServerDocument()->m_bGameLoaded = true;
		if (!m_pParent->GetServerDocument()->OnOpenDocument(fileName))
		{
			MessageBox("Der angegebene Spielstand konnte nicht geladen werden.", "Fehler",
				MB_ICONEXCLAMATION | MB_OK);
			SetCursor(::LoadCursor(NULL, IDC_ARROW));
			return -1;
		}
		// kein break!
		}

	case MODE_SERVER:
		{
		BOOL trans;
		UINT nPort = GetDlgItemInt(IDC_SERVERPORT, &trans, FALSE);
		AssertBotE(trans && nPort && nPort <= 65535);

		// Server starten
		if (!server.Start(nPort))
		{
			MessageBox("Server konnte nicht gestartet werden.", "Fehler", MB_ICONEXCLAMATION | MB_OK);
			SetCursor(::LoadCursor(NULL, IDC_ARROW));
			return -1;
		}
		m_pParent->SendMessage(WM_USERMSG, UMSG_SETSERVER, 1);

		// Client zu 127.0.0.1:nPort verbinden
		if (!client.Connect(INADDR_LOOPBACK, nPort, name))
		{
			server.Stop();
			MessageBox("Es konnte keine Verbindung zum Server hergestellt werden.", "Fehler",
				MB_ICONEXCLAMATION | MB_OK);
			SetCursor(::LoadCursor(NULL, IDC_ARROW));
			return -1;
		}

		// Veröffentlichung starten
		if (m_pPublish->GetCheck())
		{
			CString strDescription;
			m_description.GetWindowText(strDescription);
			strDescription.Trim();

			if (!serverPublisher.StartPublishing(7777, strDescription, nPort))
			{
				// Vorgang nicht abbrechen, nur Warnhinweis ausgeben
				MessageBox("Das Spiel konnte nicht veröffentlicht werden.", "Warnung",
					MB_ICONWARNING | MB_OK);
			}
		}

		break;
		}

	}

	SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return CPropertyPage::OnWizardNext();
}

void CNewGamePage::OnEnChangeUsername()
{
	EnableWizardButtons();
}

void CNewGamePage::OnBnClickedLoad()
{
	SetMode(MODE_LOAD);
	EnableWizardButtons();
}

void CNewGamePage::OnBnClickedChoosefile()
{
	CFileDialog dlg(TRUE, "sav", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING,
		"BotE-Savegame (*.sav)|*.sav|All Files (*.*)|*.*||", this, 0);
	if (dlg.DoModal() == IDOK)
		SetDlgItemText(IDC_FILENAME, dlg.GetPathName());

	EnableWizardButtons();
}

void CNewGamePage::OnBnClickedPublish()
{
	m_description.EnableWindow((m_nMode == MODE_SERVER || m_nMode == MODE_LOAD) &&
		m_pPublish->GetCheck());
	EnableWizardButtons();
}

void CNewGamePage::OnEnChangeDescription()
{
	EnableWizardButtons();
}

void CNewGamePage::OnBnClickedSearchservers()
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

void CNewGamePage::OnEnChangeHostport()
{
	EnableWizardButtons();
}

void CNewGamePage::OnEnChangeServerport()
{
	EnableWizardButtons();
}

void CNewGamePage::OnBnClickedShowoptionsdlg()
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	CIniLoader* pIni = CIniLoader::GetInstance();
	int nOldSeed = -1;
	pIni->ReadValue("Special", "RANDOMSEED", nOldSeed);

	CSettingsDlg dlg;
	if (dlg.DoModal() == IDOK)
		CBotEDoc::RandomSeed(&nOldSeed);
}
