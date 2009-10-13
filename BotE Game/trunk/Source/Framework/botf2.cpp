// botf2.cpp : Legt das Klassenverhalten f�r die Anwendung fest.
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "botf2Doc.h"
#include "Races\RaceController.h"
#include "GalaxyMenuView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialog f�r Info �ber Anwendung
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// �berladungen f�r virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult);
public:
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

void CAboutDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
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
	ON_THREAD_MESSAGE(WM_UPDATEVIEWS, CBotf2App::UpdateViews)
	ON_THREAD_MESSAGE(WM_SHOWCHATDLG, CBotf2App::ShowChatDlg)
	ON_THREAD_MESSAGE(WM_INITVIEWS, CBotf2App::InitViews)
	ON_COMMAND(ID_CHAT, &CBotf2App::OnChat)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBotf2App Konstruktion

CBotf2App::CBotf2App()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einf�gen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
	m_pChatDlg = NULL;
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

	// CG: Der folgende Block wurde von der Komponente "Windows-Sockets" hinzugef�gt.
	{
		if (!AfxSocketInit())
		{
			AfxMessageBox(CG_IDS_SOCKETS_INIT_FAILED);
			return FALSE;
		}
	}
	
	//AfxEnableControlContainer();	
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Gr��e Ihrer fertigen 
	//  ausf�hrbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht ben�tigen, entfernen.
/*
#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif*/

	// �ndern des Registrierungsschl�ssels, unter dem unsere Einstellungen gespeichert sind.
	// ZU ERLEDIGEN: Sie sollten dieser Zeichenfolge einen geeigneten Inhalt geben
	// wie z.B. den Namen Ihrer Firma oder Organisation.
	SetRegistryKey(_T("Birth of the Empires Project"));
	LoadStdProfileSettings();  // Standard INI-Dateioptionen laden (einschlie�lich MRU)

	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CBotf2Doc),
		RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CGalaxyMenuView));
	AddDocTemplate(pDocTemplate);

	// Befehlszeile parsen, um zu pr�fen auf Standard-Umgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Verteilung der in der Befehlszeile angegebenen Befehle
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	m_pMainWnd->ModifyStyle(WS_CAPTION|WS_THICKFRAME|WS_SYSMENU|WS_OVERLAPPED|FWS_ADDTOTITLE ,0);
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		
	// Chatdialog erzeugen
	m_pChatDlg = new CChatDlg(GetDocument());
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
void CBotf2App::UpdateViews(WPARAM, LPARAM)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CString sID = pDoc->GetPlayersRaceID();
	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sID));
	ASSERT(pMajor);

	// anzuzeigende View in neuer Runde ausw�hlen
	// Wenn EventScreens f�r den Spieler vorhanden sind, so werden diese angezeigt.
	if (pMajor->GetEmpire()->GetEventMessages()->GetSize() > 0)
	{
		pDoc->GetMainFrame()->FullScreenMainView(true);
		pDoc->GetMainFrame()->SelectMainView(EVENT_VIEW, pMajor->GetRaceID());
	}
	else
	{
		network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pMajor->GetRaceID());
		pDoc->GetMainFrame()->FullScreenMainView(false);
		pDoc->GetMainFrame()->SelectMainView(pDoc->m_iSelectedView[client], pMajor->GetRaceID());
		pDoc->m_iSelectedView[client] = 0;
	}
	
	// alle angezeigten Views neu zeichnen lassen
	pDoc->UpdateAllViews(NULL);	
}

void CBotf2App::InitViews(WPARAM, LPARAM)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	// alles was zu jeder neuen Runde von den Views erledigt werden muss
	pDoc->DoViewWorkOnNewRound();	
}

void CBotf2App::ShowChatDlg(WPARAM, LPARAM)
{
	m_pChatDlg->ShowWindow(SW_SHOW);
}

CBotf2Doc* CBotf2App::GetDocument() const
{
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos)
	{
		CDocTemplate *docTemplate = GetNextDocTemplate(pos);
		POSITION docpos = docTemplate->GetFirstDocPosition();
		while (docpos)
		{
			CDocument *pDoc = docTemplate->GetNextDoc(docpos);
			if (pDoc->IsKindOf(RUNTIME_CLASS(CBotf2Doc)))
				return ((CBotf2Doc *)pDoc);
		}
	}
	return NULL;
}

void CBotf2App::OnChat()
{
	// TODO: F�gen Sie hier Ihren Befehlsbehandlungscode ein.
	this->PostThreadMessage(WM_SHOWCHATDLG, 0, 0);
}

// Anwendungsbefehl zum Ausf�hren des Dialogfelds
void CBotf2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}