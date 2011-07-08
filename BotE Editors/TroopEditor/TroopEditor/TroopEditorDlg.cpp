// TroopEditorDlg.cpp : Implementierungsdatei
//
//
//TroopEditor by Revisor 2011
//
//based on classes and functions by SirPustekuchen
//
//
#include "stdafx.h"
#include "TroopEditor.h"
#include "TroopEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTroopEditorDlg-Dialogfeld




CTroopEditorDlg::CTroopEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTroopEditorDlg::IDD, pParent)
{
	m_strName="";
	m_strDescription="";
	m_strOwner="";
	m_byPower=0;
	m_byMaintenanceCosts=0;
	for(int i=0;i<6;i++)
	m_byNeededTechs[i]=0;
	for(int i=0;i<5;i++)
	m_iNeededResources[i]=0;
	m_iNeededIndustry=0;
	m_byID=0;
	m_iSize=0;
	m_byMoralValue=0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTroopEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_DESC, m_strDescription);
	DDX_Text(pDX, IDC_NAME2, m_strName2);
	DDX_Text(pDX, IDC_DESC2, m_strDescription2);
	DDX_Text(pDX, IDC_OWNER, m_strOwner);
	DDX_Text(pDX, IDC_POWER, m_byPower);
	DDX_Text(pDX, IDC_MCOSTS, m_byMaintenanceCosts);
	DDX_Text(pDX, IDC_TECH1, m_byNeededTechs[0]);
	DDX_Text(pDX, IDC_TECH2, m_byNeededTechs[1]);
	DDX_Text(pDX, IDC_TECH3, m_byNeededTechs[2]);
	DDX_Text(pDX, IDC_TECH4, m_byNeededTechs[3]);
	DDX_Text(pDX, IDC_TECH5, m_byNeededTechs[4]);
	DDX_Text(pDX, IDC_TECH6, m_byNeededTechs[5]);
	DDX_Text(pDX, IDC_REST, m_iNeededResources[0]);
	DDX_Text(pDX, IDC_RES2, m_iNeededResources[1]);
	DDX_Text(pDX, IDC_RES3, m_iNeededResources[2]);
	DDX_Text(pDX, IDC_RES4, m_iNeededResources[3]);
	DDX_Text(pDX, IDC_RES5, m_iNeededResources[4]);
	DDX_Text(pDX, IDC_IP, m_iNeededIndustry);
	DDX_Text(pDX, IDC_ID, m_byID);
	DDX_Text(pDX, IDC_BSIZE, m_iSize);
	DDX_Text(pDX, IDC_MORAL, m_byMoralValue);
	DDX_Check(pDX, IDC_STRTABEL, m_Stringtablelookup);
}

BEGIN_MESSAGE_MAP(CTroopEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON4, &CTroopEditorDlg::OnNew)
	ON_LBN_SELCHANGE(IDC_LIST, &CTroopEditorDlg::OnSelchangeList)
	ON_BN_CLICKED(IDC_BUTTON3, &CTroopEditorDlg::OnDelete)
	ON_BN_CLICKED(IDC_BUTTON1, &CTroopEditorDlg::OnLoad)
	ON_BN_CLICKED(IDC_BUTTON2, &CTroopEditorDlg::OnSave)
	ON_BN_CLICKED(IDC_STRTABEL, &CTroopEditorDlg::OnClickedStrtabel)
END_MESSAGE_MAP()


// CTroopEditorDlg-Meldungshandler

BOOL CTroopEditorDlg::OnInitDialog()
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

	// TODO: Hier zusätzliche Initialisierung einfügen
	fileReader = new CFileReader();
	fileReader->ReadDataFromFile(&m_TroopInfo);
	for (int i = 0; i < m_TroopInfo.GetSize(); i++)
		m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName());
	if (m_TroopInfo.GetSize() > 0)
	{
		m_ListBox.SetCurSel(0);
		m_iClick = 0;
	}
	this->OnSelchangeList();
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CTroopEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CTroopEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
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

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CTroopEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTroopEditorDlg::OnNew()
{
	CTroopInfo mr;
	mr.SetID(m_TroopInfo.GetAt(m_TroopInfo.GetUpperBound()).GetID()+1);//Automatische ID-Vergabe
	m_TroopInfo.Add(mr);
	

	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);

	if(m_Stringtablelookup==FALSE)//Liste neu generieren
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName());
	else
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName2());

	m_ListBox.SetCurSel(m_TroopInfo.GetUpperBound());
	m_iClick = m_TroopInfo.GetUpperBound();
	this->OnSelchangeList();
	// TODO: Add your control notification handler code here
}

void CTroopEditorDlg::OnSelchangeList()
{
	// TODO: Add your control notification handler code here
	if (m_iClick != m_ListBox.GetCurSel())
		DialogToData();	
	// Daten der neuen (jetzt markierten) Truppe anzeigen
	DataToDialog();
	m_iClick = m_ListBox.GetCurSel();
}

void CTroopEditorDlg::DialogToData()
{
	if (m_TroopInfo.GetSize() > 0)
	{
		this->UpdateData(TRUE);
		m_TroopInfo.ElementAt(m_iClick).SetName(m_strName);
		m_TroopInfo.ElementAt(m_iClick).SetDescription(m_strDescription);
		m_TroopInfo.ElementAt(m_iClick).SetOwner(m_strOwner);
		m_TroopInfo.ElementAt(m_iClick).SetPower(m_byPower);
		m_TroopInfo.ElementAt(m_iClick).SetMaintenanceCosts(m_byMaintenanceCosts);
		for(int i=0;i<6;i++) m_TroopInfo.ElementAt(m_iClick).SetNeededTechlevel(i,m_byNeededTechs[i]);
		for(int i=0;i<5;i++) m_TroopInfo.ElementAt(m_iClick).SetNeededRessource(i,m_iNeededResources[i]);
		m_TroopInfo.ElementAt(m_iClick).SetNeededIndustry(m_iNeededIndustry);
		m_TroopInfo.ElementAt(m_iClick).SetID(m_byID);
		m_TroopInfo.ElementAt(m_iClick).SetSize(m_iSize);
		m_TroopInfo.ElementAt(m_iClick).SetMoralValue(m_byMoralValue);
		
	}
	
}

void CTroopEditorDlg::DataToDialog()
{
	if(m_TroopInfo.GetSize() > 0)
	{
		m_strName = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetName();
		m_strDescription = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetDescription();
		m_strName2 = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetName2();
		m_strDescription2 = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetDescription2();
		m_strOwner = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetOwner();
		m_byPower = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetPower();
		m_byMaintenanceCosts = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetMaintenanceCosts();
		for(int i=0;i<6;i++) m_byNeededTechs[i] = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetNeededTechlevel(i);
		for(int i=0;i<5;i++) m_iNeededResources[i] = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetNeededRessource(i);
		m_iNeededIndustry = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetNeededIndustry();
		m_byID = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetID();
		m_iSize = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetSize();
		m_byMoralValue = m_TroopInfo.GetAt(m_ListBox.GetCurSel()).GetMoralValue();

		this->UpdateData(FALSE);
	}	

}

void CTroopEditorDlg::OnDelete()
{
	if (m_TroopInfo.GetSize() > 0)
		m_TroopInfo.RemoveAt(m_iClick);
	
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);

	if(m_Stringtablelookup==FALSE)//Liste neu generieren
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName());
	else
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName2());

	
	if (m_TroopInfo.GetSize() > m_iClick)
	{
		m_ListBox.SetCurSel(m_iClick);
	}
	else if (m_TroopInfo.GetSize() > 0)
	{
		m_iClick--;
		m_ListBox.SetCurSel(m_iClick);
	}
	this->OnSelchangeList();
	// TODO: Add your control notification handler code here
}

void CTroopEditorDlg::OnLoad()
{
	// TODO: Add your control notification handler code here
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	fileReader->ReadDataFromFile(&m_TroopInfo);
	
	// aktuelle Truppenliste löschen
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);

	if(m_Stringtablelookup==FALSE)
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName());
	else
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
		{	
			m_TroopInfo.GetAt(i).SetName2(fileReader->GetString(m_TroopInfo.GetAt(i).GetName()));
			m_TroopInfo.GetAt(i).SetDescription2(fileReader->GetString(m_TroopInfo.GetAt(i).GetDescription()));
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName2());
		}

	if (m_TroopInfo.GetSize() > 0)
	{
		m_ListBox.SetCurSel(0);
		m_iClick = 0;
	}
	
	this->OnSelchangeList();	

}

void CTroopEditorDlg::OnSave()
{
	DialogToData();
	fileReader->WriteDataToFile(&m_TroopInfo);
	CString s;
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);

	if(m_Stringtablelookup==FALSE)//Liste neu generieren
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName());
	else
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
		{
			
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName2());
		}
	m_ListBox.SetCurSel(m_iClick);
	this->OnSelchangeList();// TODO: Add your control notification handler code here
}

void CTroopEditorDlg::OnDestroy()
{
	CDialog::OnDestroy();
	delete(fileReader);
	fileReader = NULL;

}

void CTroopEditorDlg::OnClickedStrtabel()//Wenn die Checkbox betätigt wird, werden alle Namen in der Stringtable nachgeschlagen
{
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);
	if(m_Stringtablelookup==FALSE)
	{
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
		{
			m_TroopInfo.GetAt(i).SetName2(fileReader->GetString(m_TroopInfo.GetAt(i).GetName()));
			m_TroopInfo.GetAt(i).SetDescription2(fileReader->GetString(m_TroopInfo.GetAt(i).GetDescription()));
			// TODO: Add your control notification handler code here
		}
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName2());

	}
	else
	{
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
		{
			m_TroopInfo.GetAt(i).SetName2("");
			m_TroopInfo.GetAt(i).SetDescription2("");
			// TODO: Add your control notification handler code here
		}
		for (int i = 0; i < m_TroopInfo.GetSize(); i++)
			m_ListBox.AddString(m_TroopInfo.GetAt(i).GetName());

	}

}
