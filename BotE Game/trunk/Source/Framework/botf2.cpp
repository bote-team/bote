// botf2.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "botf2Doc.h"
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
// CBotf2App

BEGIN_MESSAGE_MAP(CBotf2App, CWinApp)
	//{{AFX_MSG_MAP(CBotf2App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_OPTIONEN, &CBotf2App::OnSettings)
	ON_COMMAND(ID_CHAT, &CBotf2App::OnChat)
	ON_COMMAND(ID_HELP_MANUAL, &CBotf2App::OnOpenManual)
	ON_COMMAND(ID_HELP_WIKI, &CBotf2App::OnHelpWiki)
	ON_COMMAND(ID_HELP_README, &CBotf2App::OnOpenReadme)
	ON_COMMAND(ID_HELP_CREDITS, &CBotf2App::OnOpenCredits)

	ON_THREAD_MESSAGE(WM_INITVIEWS, CBotf2App::InitViews)
	ON_THREAD_MESSAGE(WM_UPDATEVIEWS, CBotf2App::UpdateViews)
	ON_THREAD_MESSAGE(WM_SHOWCHATDLG, CBotf2App::ShowChatDlg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBotf2App Konstruktion

CBotf2App::CBotf2App() :
	gdiplusToken()
{
#ifdef VLD
	VLDEnable();
#endif
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
	m_pChatDlg = NULL;
	ParseCommandLine(*CCommandLineParameters::GetInstance());
}

CBotf2App::~CBotf2App()
{
	if (m_pChatDlg)
		delete m_pChatDlg;
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CBotf2App-Objekt

CBotf2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CBotf2App Initialisierung

BOOL CBotf2App::InitInstance()
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
		RUNTIME_CLASS(CBotf2Doc),
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

int CBotf2App::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	GdiplusShutdown(gdiplusToken);
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CBotf2App-Nachrichtenbehandlungsroutinen
void CBotf2App::InitViews(WPARAM, LPARAM)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	pDoc->LoadViewGraphics();
}

void CBotf2App::UpdateViews(WPARAM, LPARAM)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	pDoc->DoViewWorkOnNewRound();
}

void CBotf2App::ShowChatDlg(WPARAM, LPARAM)
{
	m_pChatDlg->ShowWindow(SW_SHOW);
}

void CBotf2App::OnChat()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
	this->PostThreadMessage(WM_SHOWCHATDLG, 0, 0);
}

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CBotf2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Funktion zum Anzeigen des Einstellungsdialoges
void CBotf2App::OnSettings()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
	CSettingsDlg dlg(true);
	if (dlg.DoModal() == IDOK)
		resources::pDoc->ResetIniSettings();
}

void CBotf2App::OnOpenManual()
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

void CBotf2App::OnHelpWiki()
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

void CBotf2App::OnOpenReadme()
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

void CBotf2App::OnOpenCredits()
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
