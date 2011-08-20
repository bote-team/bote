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
	m_iRelCar = 0;
	m_iRelDom = 0;
	m_iRelFed = 0;
	m_iRelFer = 0;
	m_iRelKli = 0;
	m_iRelRom = 0;
	for(int i=0;i<11;i++) m_iPropertyRel[i]=50;
	m_iBaseRel=50;
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
	DDX_Check(pDX, IDC_CHECKAGRARIAN, m_bProperty[2]);
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
	DDX_Text(pDX, IDC_BASEVALUE, m_iBaseRel);
	DDX_Text(pDX, IDC_HOMESYSNAME, m_strHomeSysName);
	DDX_Text(pDX, IDC_GRAPHICNAME, m_strGraphicName);
	DDX_Text(pDX, IDC_RELFED, m_iRelFed);
	DDX_Text(pDX, IDC_RELFER, m_iRelFer);
	DDX_Text(pDX, IDC_RELKLI, m_iRelKli);
	DDX_Text(pDX, IDC_RELROM, m_iRelRom);
	DDX_Text(pDX, IDC_RELCAR, m_iRelCar);
	DDX_Text(pDX, IDC_RELDOM, m_iRelDom);
	DDX_Text(pDX, IDC_RELFINANCIAL, m_iPropertyRel[0]);
	DDX_Text(pDX, IDC_RELWARLIKE, m_iPropertyRel[1]);
	DDX_Text(pDX, IDC_RELAGRARIAN, m_iPropertyRel[2]);
	DDX_Text(pDX, IDC_RELINDUSTRIAL, m_iPropertyRel[3]);
	DDX_Text(pDX, IDC_RELSECRET, m_iPropertyRel[4]);
	DDX_Text(pDX, IDC_RELSCIENTIFIC, m_iPropertyRel[5]);
	DDX_Text(pDX, IDC_RELPRODUCER, m_iPropertyRel[6]);
	DDX_Text(pDX, IDC_RELPACIFIST, m_iPropertyRel[7]);
	DDX_Text(pDX, IDC_RELSNEAKY, m_iPropertyRel[8]);
	DDX_Text(pDX, IDC_RELSOLOING, m_iPropertyRel[9]);
	DDX_Text(pDX, IDC_RELHOSTILE, m_iPropertyRel[10]);
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
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RELTYPE1, &CMinorRaceEditorDlg::OnBnClickedReltype1)
	ON_BN_CLICKED(IDC_RELTYPE2, &CMinorRaceEditorDlg::OnBnClickedReltype2)
	ON_BN_CLICKED(IDC_CALC, &CMinorRaceEditorDlg::OnBnClickedCalc)
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

	for(int i=0;i<11;i++) m_iPropertyRel[i]=50;//Werden nicht automatisch zurückgesetzt
	m_iBaseRel=50;
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
		m_iRelFed = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetRelationshipToMajorRace(HUMAN);
		m_iRelFer = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetRelationshipToMajorRace(FERENGI);
		m_iRelKli = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetRelationshipToMajorRace(KLINGON);
		m_iRelRom = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetRelationshipToMajorRace(ROMULAN);
		m_iRelCar = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetRelationshipToMajorRace(CARDASSIAN);
		m_iRelDom = m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetRelationshipToMajorRace(DOMINION);
		for(int i=0;i<11;i++) m_bProperty[i]=m_MinorInfo.GetAt(m_ListBox.GetCurSel()).GetProperty(i);
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
		if(m_iRelFed>100) m_iRelFed=100;//Um Werte über 100 zu verhindern
		m_MinorInfo.ElementAt(m_iClick).SetRelationship(HUMAN,m_iRelFed);
		if(m_iRelFer>100) m_iRelFer=100;
		m_MinorInfo.ElementAt(m_iClick).SetRelationship(FERENGI,m_iRelFer);
		if(m_iRelKli>100) m_iRelKli=100;
		m_MinorInfo.ElementAt(m_iClick).SetRelationship(KLINGON,m_iRelKli);
		if(m_iRelRom>100) m_iRelRom=100;
		m_MinorInfo.ElementAt(m_iClick).SetRelationship(ROMULAN,m_iRelRom);
		if(m_iRelCar>100) m_iRelCar=100;
		m_MinorInfo.ElementAt(m_iClick).SetRelationship(CARDASSIAN,m_iRelCar);
		if(m_iRelDom>100) m_iRelDom=100;
		m_MinorInfo.ElementAt(m_iClick).SetRelationship(DOMINION,m_iRelDom);
		m_MinorInfo.ElementAt(m_iClick).SetTechnologicalProgress(m_TechProgress.GetCurSel());
		for(int i=0;i<11;i++) m_MinorInfo.ElementAt(m_iClick).SetProperty(i,m_bProperty[i]);
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



void CMinorRaceEditorDlg::OnBnClickedReltype1()
{
	GetDlgItem(IDC_BASEVALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELAGRARIAN)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELWARLIKE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELFINANCIAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELINDUSTRIAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELSCIENTIFIC)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELSOLOING)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELSECRET)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELSNEAKY)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELPACIFIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELPRODUCER)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELHOSTILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CALC)->EnableWindow(FALSE);

	GetDlgItem(IDC_RELFED)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELFER)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELKLI)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELROM)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELDOM)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELCAR)->EnableWindow(TRUE);

	// TODO: Add your control notification handler code here
}

void CMinorRaceEditorDlg::OnBnClickedReltype2()
{
	GetDlgItem(IDC_BASEVALUE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELAGRARIAN)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELWARLIKE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELFINANCIAL)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELINDUSTRIAL)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELSCIENTIFIC)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELSOLOING)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELSECRET)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELSNEAKY)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELPACIFIST)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELPRODUCER)->EnableWindow(TRUE);
	GetDlgItem(IDC_RELHOSTILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_CALC)->EnableWindow(TRUE);

	GetDlgItem(IDC_RELFED)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELFER)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELKLI)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELROM)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELDOM)->EnableWindow(FALSE);
	GetDlgItem(IDC_RELCAR)->EnableWindow(FALSE);
	// TODO: Add your control notification handler code here
}


void CMinorRaceEditorDlg::OnBnClickedCalc()
{
	this->UpdateData(TRUE);
	if(m_iPropertyRel[5]+m_iBaseRel-50<0) m_iRelFed=0;
	else m_iRelFed= m_iPropertyRel[5]+m_iBaseRel-50;
	//CString t;
	//t.Format("%d",m_iRelFed);
	//GetDlgItem(IDC_RELFED)->SetWindowTextA(t);

	if(m_iPropertyRel[0]+m_iBaseRel-50<0) m_iRelFer=0;
	else m_iRelFer= m_iPropertyRel[0]+m_iBaseRel-50;
	//t.Format("%d",m_iRelFer);
	//GetDlgItem(IDC_RELFER)->SetWindowTextA(t);

	if(m_iPropertyRel[1]+m_iBaseRel-50<0) m_iRelKli=0;
	else m_iRelKli= m_iPropertyRel[1]+m_iBaseRel-50;
	//t.Format("%d",m_iRelKli);
	//GetDlgItem(IDC_RELKLI)->SetWindowTextA(t);

	if(m_iPropertyRel[4]+m_iBaseRel-50<0) m_iRelRom=0;
	else m_iRelRom= m_iPropertyRel[4]+m_iBaseRel-50;
	//t.Format("%d",m_iRelRom);
	//GetDlgItem(IDC_RELROM)->SetWindowTextA(t);

	if(m_iPropertyRel[8]+m_iBaseRel-50<0) m_iRelCar=0;
	else m_iRelCar= m_iPropertyRel[8]+m_iBaseRel-50;
	//t.Format("%d",m_iRelCar);
	//GetDlgItem(IDC_RELCAR)->SetWindowTextA(t);

	m_iRelDom= m_iBaseRel;
	//t.Format("%d",m_iRelDom);
	//GetDlgItem(IDC_RELDOM)->SetWindowTextA(t);
	
	
	this->UpdateData(FALSE);
	// TODO: Add your control notification handler code here
}

