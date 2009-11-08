// BuildingEditor.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "BuildingEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBuildingEditorApp

BEGIN_MESSAGE_MAP(CBuildingEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CBuildingEditorApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuildingEditorApp Konstruktion

CBuildingEditorApp::CBuildingEditorApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
	m_pDlg = NULL;
}

// CBuildingEditorApp Destruktion
CBuildingEditorApp::~CBuildingEditorApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
	if (m_pDlg != NULL)
	{
		delete m_pDlg;
		m_pDlg = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CBuildingEditorApp-Objekt

CBuildingEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBuildingEditorApp Initialisierung

BOOL CBuildingEditorApp::InitInstance()
{
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

	m_pDlg = new CBuildingEditorDlg();
	m_pMainWnd = m_pDlg;
	int nResponse = m_pDlg->DoModal();
	if (nResponse == IDOK)
	{
		// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über OK zu steuern
	}
	else if (nResponse == IDCANCEL)
	{
		// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über "Abbrechen" zu steuern
	}

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}
