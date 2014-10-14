// BotE.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "BotE.h"
#include "MainFrm.h"
#include "BotEDoc.h"
#include "SettingsDlg.h"
#include "IOData.h"

#ifdef VLD
#include "vld.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialog für Info über Anwendung
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Überladungen für virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CAboutDlg::OnIpnFieldchangedIpaddress1(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);*/
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CAboutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CBotEApp

BEGIN_MESSAGE_MAP(CBotEApp, CWinApp)
	//{{AFX_MSG_MAP(CBotEApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_OPTIONEN, &CBotEApp::OnSettings)
	ON_COMMAND(ID_CHAT, &CBotEApp::OnChat)
	ON_COMMAND(ID_HELP_MANUAL, &CBotEApp::OnOpenManual)
	ON_COMMAND(ID_HELP_WIKI, &CBotEApp::OnHelpWiki)
	ON_COMMAND(ID_HELP_README, &CBotEApp::OnOpenReadme)
	ON_COMMAND(ID_HELP_CREDITS, &CBotEApp::OnOpenCredits)
	ON_COMMAND(ID_HELP_CONTROLS, &CBotEApp::OnOpenControls)

	ON_THREAD_MESSAGE(WM_INITVIEWS, CBotEApp::InitViews)
	ON_THREAD_MESSAGE(WM_UPDATEVIEWS, CBotEApp::UpdateViews)
	ON_THREAD_MESSAGE(WM_SHOWCHATDLG, CBotEApp::ShowChatDlg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBotEApp Konstruktion

CBotEApp::CBotEApp() :
	gdiplusToken()
{
	InitConstants();

#ifdef VLD
	VLDEnable();
#endif
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
	m_pChatDlg = NULL;
	ParseCommandLine(*CCommandLineParameters::GetInstance());
}

CBotEApp::~CBotEApp()
{
	if (m_pChatDlg)
		delete m_pChatDlg;
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CBotEApp-Objekt

CBotEApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBotEApp Initialisierung

BOOL CBotEApp::InitInstance()
{
	CWinApp::InitInstance();
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// CG: Der folgende Block wurde von der Komponente "Windows-Sockets" hinzugefügt.
	{
		if (!AfxSocketInit())
		{
			AfxMessageBox(CG_IDS_SOCKETS_INIT_FAILED);
			return FALSE;
		}
	}

	//AfxEnableControlContainer();
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.
/*
#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif*/

	// Ändern des Registrierungsschlüssels, unter dem unsere Einstellungen gespeichert sind.
	// ZU ERLEDIGEN: Sie sollten dieser Zeichenfolge einen geeigneten Inhalt geben
	// wie z.B. den Namen Ihrer Firma oder Organisation.
	SetRegistryKey(_T("Birth of the Empires Project"));
	LoadStdProfileSettings();  // Standard INI-Dateioptionen laden (einschließlich MRU)

	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CBotEDoc),
		RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
		NULL);

	AddDocTemplate(pDocTemplate);

	// Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Verteilung der in der Befehlszeile angegebenen Befehle
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	m_pMainWnd->ModifyStyle(WS_CAPTION|WS_THICKFRAME|WS_SYSMENU|WS_OVERLAPPED|FWS_ADDTOTITLE ,0);
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);

	// Chatdialog erzeugen
	m_pChatDlg = new CChatDlg(resources::pDoc);
	m_pChatDlg->Create(IDD_CHATDLG);

	return TRUE;
}

int CBotEApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	GdiplusShutdown(gdiplusToken);
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CBotEApp-Nachrichtenbehandlungsroutinen
void CBotEApp::InitViews(WPARAM, LPARAM)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	pDoc->LoadViewGraphics();
}

void CBotEApp::UpdateViews(WPARAM, LPARAM)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	pDoc->DoViewWorkOnNewRound();
}

void CBotEApp::ShowChatDlg(WPARAM, LPARAM)
{
	m_pChatDlg->ShowWindow(SW_SHOW);
}

void CBotEApp::OnChat()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
	this->PostThreadMessage(WM_SHOWCHATDLG, 0, 0);
}

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CBotEApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Funktion zum Anzeigen des Einstellungsdialoges
void CBotEApp::OnSettings()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
	CSettingsDlg dlg(true);
	if (dlg.DoModal() == IDOK)
		resources::pDoc->ResetIniSettings();
}

void CBotEApp::OnOpenManual()
{
	// TODO: Add your command handler code here
	CString sFile = CIOData::GetInstance()->GetManualPath();

	/*HINSTANCE hInst =*/ ShellExecute(0,
		"open",		// Operation to perform
		sFile,		// Application name
        "",			// Additional parameters
        0,			// Default directory
        SW_SHOW);

}

void CBotEApp::OnHelpWiki()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
	CString sFile =  "http://www.wiki.birth-of-the-empires.de";

	/*HINSTANCE hInst =*/ ShellExecute(0,
		"open",		// Operation to perform
		sFile,		// Application name
        "",			// Additional parameters
        0,			// Default directory
        SW_SHOW);
}

void CBotEApp::OnOpenReadme()
{
	// TODO: Add your command handler code here
	CString sFile = CIOData::GetInstance()->GetAppPath() + "Liesmich.txt";

	/*HINSTANCE hInst =*/ ShellExecute(0,
		"open",		// Operation to perform
		sFile,		// Application name
        "",			// Additional parameters
        0,			// Default directory
        SW_SHOW);
}

void CBotEApp::OnOpenControls()
{
	// TODO: Add your command handler code here
	CString msg;
	msg = "Use mousewheel to zoom in and out of galaxy map. With pressed-down left mouse button you can move around the map in all directions. \n\nTo move ships around click on the race symbol on the upper right corner of a sector in question. Then click on a ship at the bottom view and hover again over a sector on galaxy map you wish to fly into. Click on it and course is plotted. \nUse 'n' key to select next unoccupied ship of yours. \n\nIn system view use the arrow keys left and right to quickly switch systems when you have more than one colony. You can alternatively use your xtra mouse buttons when you have a gamer mouse. \nDouble-clicking on the build queue list on the bottom right part of the system buildmenu screen will erase that entry from the build queue. \n\nOn empire menu screen clicking on a message and pressing 'del' key will delete the message from the list to make it more clear.";
	AfxMessageBox(msg, MB_ICONINFORMATION | MB_OK);
}
void CBotEApp::OnOpenCredits()
{
	// TODO: Add your command handler code here
	CString sFile = CIOData::GetInstance()->GetAppPath() + "\\Graphics\\Buildings\\CreditsBuildings.txt";
	/*HINSTANCE hInst =*/ ShellExecute(0,
		"open",		// Operation to perform
		sFile,		// Application name
        "",			// Additional parameters
        0,			// Default directory
        SW_SHOW);

	sFile = CIOData::GetInstance()->GetAppPath() + "\\Graphics\\Ships\\CreditsShips.txt";
	/*hInst =*/ ShellExecute(0,
		"open",		// Operation to perform
		sFile,		// Application name
        "",			// Additional parameters
        0,			// Default directory
        SW_SHOW);

	sFile = CIOData::GetInstance()->GetAppPath() + "\\Graphics\\Troops\\CreditsTroops.txt";
	/*hInst =*/ ShellExecute(0,
		"open",		// Operation to perform
		sFile,		// Application name
        "",			// Additional parameters
        0,			// Default directory
        SW_SHOW);

	sFile = CIOData::GetInstance()->GetAppPath() + "\\Graphics\\Planets\\CreditsPlanets.txt";
	/*hInst =*/ ShellExecute(0,
		"open",		// Operation to perform
		sFile,		// Application name
        "",			// Additional parameters
        0,			// Default directory
        SW_SHOW);
}
