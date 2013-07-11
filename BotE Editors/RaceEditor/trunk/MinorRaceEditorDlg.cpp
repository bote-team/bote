// MinorRaceEditorDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "RaceEditor.h"
#include "MinorRaceEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMinorRaceEditorDlg Dialogfeld

CMinorRaceEditorDlg::CMinorRaceEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMinorRaceEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMinorRaceEditorDlg)
	m_strDescription = _T("");
	m_strRaceName = _T("");
	m_bSpaceflightNation = FALSE;
	m_strHomeSysName = _T("");
	m_strGraphicName = _T("");
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMinorRaceEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMinorRaceEditorDlg)
	DDX_Control(pDX, IDC_TECHPROGRESS, m_TechProgress);
	DDX_Control(pDX, IDC_CORRUPTIBILITY, m_Corruptibiliy);
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	DDX_Text(pDX, IDC_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_RACENAME, m_strRaceName);
	DDX_Check(pDX, IDC_SPACEFLIGHT, m_bSpaceflightNation);
	/*DDX_Check(pDX, IDC_CHECKAGRARIAN, m_bProperty[2]);
	DDX_Check(pDX,  IDC_CHECKFINANCIAL, m_bProperty[0]);
	DDX_Check(pDX,IDC_CHECKHOSTILE, m_bProperty[10]);
	DDX_Check(pDX, IDC_CHECKINDUSTRIAL, m_bProperty[3]);
	DDX_Check(pDX, IDC_CHECPACIFIST, m_bProperty[7]);
	DDX_Check(pDX, IDC_CHECKPRODUCTIV, m_bProperty[6]);
	DDX_Check(pDX, IDC_CHECKSOLOING, m_bProperty[9]);
	DDX_Check(pDX, IDC_CHECKSCIENTIFIC, m_bProperty[5]);
	DDX_Check(pDX, IDC_CHECKSECRET, m_bProperty[4]);
	DDX_Check(pDX, IDC_CHECKSNEAKY, m_bProperty[8]);
	DDX_Check(pDX, IDC_CHECKWARLIKE, m_bProperty[1]);
	*/

	//Names are not correct
	DDX_Check(pDX, IDC_CHECKAGRARIAN, m_bProperty[0]);   //value 1 = financial
	DDX_Check(pDX,  IDC_CHECKFINANCIAL, m_bProperty[1]); //value 2 = warlike
	DDX_Check(pDX,IDC_CHECKHOSTILE, m_bProperty[2]);     //value 3 = AGRARIAN
	DDX_Check(pDX, IDC_CHECKINDUSTRIAL, m_bProperty[3]); //value 4 = INDUSTRIAL
	DDX_Check(pDX, IDC_CHECPACIFIST, m_bProperty[4]);    //value 5 = SECRET
	DDX_Check(pDX, IDC_CHECKPRODUCTIV, m_bProperty[5]);	 //value 6 = SCIENTIFIC
	DDX_Check(pDX, IDC_CHECKSOLOING, m_bProperty[6]);    //value 7 = PRODUCER
	DDX_Check(pDX, IDC_CHECKSCIENTIFIC, m_bProperty[7]); //value 8 = PACIFIST
	DDX_Check(pDX, IDC_CHECKSECRET, m_bProperty[8]);     //value 9 = SNEAKY
	//DDX_Check(pDX, IDC_CHECKSNEAKY, m_bProperty[9]);	//value 10 = SOLOING - not for minors
	//DDX_Check(pDX, IDC_CHECKWARLIKE, m_bProperty[10]);//value 11 = HOSTILE - not for minors

	DDX_Text(pDX, IDC_HOMESYSNAME, m_strHomeSysName);
	DDX_Text(pDX, IDC_GRAPHICNAME, m_strGraphicName);
	
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMinorRaceEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CMinorRaceEditorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_NEW, OnNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMinorRaceEditorDlg Nachrichten-Handler

BOOL CMinorRaceEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
	// ZU ERLEDIGEN: Hier zusätzliche Initialisierung einfügen
	fileReader = new CFileReader();
	fileReader->ReadDataFromFile(&m_MinorInfo);

	for (int i = 0; i < m_MinorInfo.GetSize(); i++)
		m_ListBox.AddString(m_MinorInfo.GetAt(i).GetRaceName());
	if (m_MinorInfo.GetSize() > 0)
	{
		m_ListBox.SetCurSel(0);
		m_iClick = 0;
	}
	m_TechProgress.SetCurSel(2);
	m_Corruptibiliy.SetCurSel(2);

	this->OnSelchangeList();
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CMinorRaceEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CMinorRaceEditorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}	
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CMinorRaceEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMinorRaceEditorDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
	//delete(fileReader);
	fileReader = NULL;
	
}

void CMinorRaceEditorDlg::OnSelchangeList() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	// Daten des alten angeklickten Gebäudes speichern
	if (m_iClick != m_ListBox.GetCurSel())
		DialogToData();	

	// Daten der neuen (jetzt markierten) Rasse anzeigen
	DataToDialog();
	m_iClick = m_ListBox.GetCurSel();

}

void CMinorRaceEditorDlg::DataToDialog()
{
	if (m_MinorInfo.GetSize() > 0)
	{
		m_strHomeSysName = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetHomeplanetName();
		m_strRaceName = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetRaceName();
		m_strDescription = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetRaceDescription();
		m_strGraphicName = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetGraphicName();
		for(int i=0;i<NUMBEROFKINDS;i++) m_bProperty[i]=m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetProperty(i);
		m_TechProgress.SetCurSel(m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetTechnologicalProgress());
		m_Corruptibiliy.SetCurSel(m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetCorruptibility());
		m_bSpaceflightNation = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetSpaceflightNation();
		// Daten in den Dialog bringen
		this->UpdateData(FALSE);
	}
}

void CMinorRaceEditorDlg::DialogToData()
{
	if (m_MinorInfo.GetSize() > 0)
	{
		this->UpdateData(TRUE);
		m_MinorInfo.ElementAt(m_iClick).SetHomePlanet(m_strHomeSysName);
		m_MinorInfo.ElementAt(m_iClick).SetRaceName(m_strRaceName);
		m_MinorInfo.ElementAt(m_iClick).SetRaceDescription(m_strDescription);
		m_MinorInfo.ElementAt(m_iClick).SetGraphicName(m_strGraphicName);
		m_MinorInfo.ElementAt(m_iClick).SetTechnologicalProgress(m_TechProgress.GetCurSel());
		for(int i=0;i<NUMBEROFKINDS;i++) m_MinorInfo.ElementAt(m_iClick).SetProperty(i,m_bProperty[i]);
		m_MinorInfo.ElementAt(m_iClick).SetCorruptibility(m_Corruptibiliy.GetCurSel());
		m_MinorInfo.ElementAt(m_iClick).SetSpaceflightNation(m_bSpaceflightNation);
	}
}



void CMinorRaceEditorDlg::OnLoad() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	fileReader->ReadDataFromFile(&m_MinorInfo);
	
	// aktuelle Rassenliste löschen
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);

	for (int i = 0; i < m_MinorInfo.GetSize(); i++)
		m_ListBox.AddString(m_MinorInfo.GetAt(i).GetRaceName());
	if (m_MinorInfo.GetSize() > 0)
	{
		m_ListBox.SetCurSel(0);
		m_iClick = 0;
	}
	m_TechProgress.SetCurSel(2);
	
	m_Corruptibiliy.SetCurSel(2);

	this->OnSelchangeList();	
}

void CMinorRaceEditorDlg::OnSave() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	// aktuelle Daten noch speichern
	DialogToData();
	fileReader->WriteDataToFile(&m_MinorInfo);
	// Nach speichern Gebäudeliste neu berechnen...damit wir bei geänderten Namen auch ne aktuelle Auswahlliste bekommen
	CString s;
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);
	for (int i = 0; i < m_MinorInfo.GetSize(); i++)
		m_ListBox.AddString(m_MinorInfo.GetAt(i).GetRaceName());
	m_ListBox.SetCurSel(m_iClick);
	this->OnSelchangeList();
}

void CMinorRaceEditorDlg::OnDelete() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_MinorInfo.GetSize() > 0)
		m_MinorInfo.RemoveAt(m_iClick);
	// aktuelle Rassenliste löschen
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);

	for (int i = 0; i < m_MinorInfo.GetSize(); i++)
		m_ListBox.AddString(m_MinorInfo.GetAt(i).GetRaceName());
	if (m_MinorInfo.GetSize() > m_iClick)
	{
		m_ListBox.SetCurSel(m_iClick);
	}
	else if (m_MinorInfo.GetSize() > 0)
	{
		m_iClick--;
		m_ListBox.SetCurSel(m_iClick);
	}
	m_TechProgress.SetCurSel(2);
	
	m_Corruptibiliy.SetCurSel(2);

	this->OnSelchangeList();
}

void CMinorRaceEditorDlg::OnNew() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CMinorRace mr;
	mr.Reset();
	m_MinorInfo.Add(mr);
	m_ListBox.AddString(mr.GetRaceName());	
	m_ListBox.SetCurSel(m_MinorInfo.GetUpperBound());
	m_iClick = m_MinorInfo.GetUpperBound();
	this->OnSelchangeList();
}




