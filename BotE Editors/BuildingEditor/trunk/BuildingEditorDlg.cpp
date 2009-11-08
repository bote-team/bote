// BuildingEditorDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "BuildingEditor.h"
#include "BuildingEditorDlg.h"

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
// CBuildingEditorDlg Dialogfeld

CBuildingEditorDlg::CBuildingEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBuildingEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBuildingEditorDlg)
	m_iRunningNumber = 0;
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CBuildingEditorDlg::~CBuildingEditorDlg()
{
}

void CBuildingEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBuildingEditorDlg)
	DDX_Control(pDX, IDC_LANGUAGE, m_Language);
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	DDX_Text(pDX, IDC_RUNNINGNUMBER, m_iRunningNumber);
	DDV_MinMaxUInt(pDX, m_iRunningNumber, 0, 64000);
	DDX_Control(pDX, IDC_TAB1, m_dynTabCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBuildingEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CBuildingEditorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_MOVEUP, OnMoveup)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnMovedown)
	ON_CBN_SELCHANGE(IDC_LANGUAGE, OnSelchangeLanguage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuildingEditorDlg Nachrichten-Handler

BOOL CBuildingEditorDlg::OnInitDialog()
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
	m_dynTabCtrl.InsertItem(new CInfoDlg(), "Information", IDD_INFODLG);
	m_dynTabCtrl.InsertItem(new CPrerequisitesDlg, "Prerequisites", IDD_PREREQUISITESDLG);
	m_dynTabCtrl.InsertItem(new CProductionDlg, "Production", IDD_PRODUCTIONDLG);
	m_dynTabCtrl.InsertItem(new CSpecialDlg, "Specials", IDD_SPECIALDLG);
	m_dynTabCtrl.InsertItem(new CAboutDlg, "About", IDD_ABOUTBOX);
		
	m_bAutoSorting = FALSE;
	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
	// ZU ERLEDIGEN: Hier zusätzliche Initialisierung einfügen
	fileReader = new CFileReader();
	fileReader->ReadDataFromFile(&m_BuildingInfo);
	//fileReader->WriteDataToFile(&m_BuildingInfo);

	color[NOBODY]	= RGB(1,1,1);
	color[HUMAN]	= RGB(0,0,255);
	color[FERENGI]	= RGB(255,180,0);
	color[KLINGON]	= RGB(255,0,0);
	color[ROMULAN]	= RGB(0,140,0);
	color[CARDASSIAN] = RGB(125,0,125);
	color[DOMINION]	= RGB(90,225,255);

	CString s;
	for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
	{
		s.Format("%d: %s",m_BuildingInfo.GetAt(i).GetRunningNumber(),m_BuildingInfo.GetAt(i).GetBuildingName(0));
		m_ListBox.AddString(s,color[m_BuildingInfo.GetAt(i).GetOwnerOfBuilding()]);
	}
	if (m_BuildingInfo.GetSize() > 0)
	{
		m_ListBox.SetCurSel(0);
		m_iClick = 0;
	}
//	m_RaceComboBox.SetCurSel(0);
	m_Language.SetCurSel(0);
	this->OnSelchangeList();
		
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CBuildingEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBuildingEditorDlg::OnPaint() 
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
HCURSOR CBuildingEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CBuildingEditorDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen

	delete fileReader;
	fileReader = NULL;	

	m_dynTabCtrl.DeleteItem(4);
	m_dynTabCtrl.DeleteItem(3);
	m_dynTabCtrl.DeleteItem(2);
	m_dynTabCtrl.DeleteItem(1);
	m_dynTabCtrl.DeleteItem(0);
}

void CBuildingEditorDlg::OnSelchangeList() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
	// Daten des alten angeklickten Gebäudes speichern
	if (m_iClick != m_ListBox.GetCurSel())
		DialogToData();			
	
	// Daten des neuen (jetzt markierten) Gebäudes anzeigen
	DataToDialog();	
	m_iClick = m_ListBox.GetCurSel();
}

void CBuildingEditorDlg::OnSave() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	// aktuelle Daten noch speichern
	DialogToData();
	AutoSort();
	fileReader->WriteDataToFile(&m_BuildingInfo, m_Language.GetCurSel());
	// Nach speichern Gebäudeliste neu berechnen...damit wir bei geänderten Namen auch ne aktuelle Auswahlliste bekommen
	CString s;
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);
	for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
	{
		s.Format("%d: %s",m_BuildingInfo.GetAt(i).GetRunningNumber(),m_BuildingInfo.GetAt(i).GetBuildingName(m_Language.GetCurSel()));
		m_ListBox.AddString(s,color[m_BuildingInfo.GetAt(i).GetOwnerOfBuilding()]);
	}
	m_ListBox.SetCurSel(m_iClick);
	this->OnSelchangeList();
}

void CBuildingEditorDlg::OnLoad() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	fileReader->ReadDataFromFile(&m_BuildingInfo);
	// Nach laden Gebäudeliste neu berechnen...damit wir bei geänderten Namen auch ne aktuelle Auswahlliste bekommen
	CString s;
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);
	for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
	{
		s.Format("%d: %s",m_BuildingInfo.GetAt(i).GetRunningNumber(),m_BuildingInfo.GetAt(i).GetBuildingName(m_Language.GetCurSel()));
		m_ListBox.AddString(s,color[m_BuildingInfo.GetAt(i).GetOwnerOfBuilding()]);
	}
	if (m_BuildingInfo.GetSize() > 0)
	{
		m_ListBox.SetCurSel(0);
		m_iClick = 0;
	}
	this->OnSelchangeList();
}

void CBuildingEditorDlg::OnNew() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CBuildingInfo bi;
	m_BuildingInfo.Add(bi);
	m_iClick = m_BuildingInfo.GetUpperBound();
	m_BuildingInfo.ElementAt(m_iClick).SetRunningNumber(m_iClick+1);
	m_BuildingInfo.ElementAt(m_iClick).SetOwnerOfBuilding(0);
	m_BuildingInfo.ElementAt(m_iClick).SetBuildingName("new building", 0);
	m_BuildingInfo.ElementAt(m_iClick).SetBuildingName("new building", 1);
	m_BuildingInfo.ElementAt(m_iClick).SetBuildingDescription("no description available", 0);
	m_BuildingInfo.ElementAt(m_iClick).SetBuildingDescription("no description available", 1);
	m_BuildingInfo.ElementAt(m_iClick).SetGraphikFileName("");
	m_BuildingInfo.ElementAt(m_iClick).SetMaxInSystem(0,0);
	m_BuildingInfo.ElementAt(m_iClick).SetMaxInEmpire(0,0);
	m_BuildingInfo.ElementAt(m_iClick).SetOnlyHomePlanet(0);
	m_BuildingInfo.ElementAt(m_iClick).SetOnlyOwnColony(0);
	m_BuildingInfo.ElementAt(m_iClick).SetOnlyMinorRace(0);
	m_BuildingInfo.ElementAt(m_iClick).SetOnlyTakenSystem(0);
	m_BuildingInfo.ElementAt(m_iClick).SetOnlyInSystemWithName("");
	m_BuildingInfo.ElementAt(m_iClick).SetMinHabitants(0);
	m_BuildingInfo.ElementAt(m_iClick).SetMinInSystem(0,0);
	m_BuildingInfo.ElementAt(m_iClick).SetMinInEmpire(0,0);
	m_BuildingInfo.ElementAt(m_iClick).SetOnlyRace(0);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(A,0);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(B,0);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(C,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(E,0);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(F,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(G,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(H,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(I,0);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(J,0);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(K,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(L,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(M,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(N,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(O,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(P,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(Q,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(R,1);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(S,0);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(T,0);
	m_BuildingInfo.ElementAt(m_iClick).SetPlanetTypes(Y,0);
	m_BuildingInfo.ElementAt(m_iClick).SetBioTech(0);
	m_BuildingInfo.ElementAt(m_iClick).SetEnergyTech(0);
	m_BuildingInfo.ElementAt(m_iClick).SetCompTech(0);
	m_BuildingInfo.ElementAt(m_iClick).SetPropulsionTech(0);
	m_BuildingInfo.ElementAt(m_iClick).SetConstructionTech(0);
	m_BuildingInfo.ElementAt(m_iClick).SetWeaponTech(0);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededIndustry(1);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededEnergy(0);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededTitan(0);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededDeuterium(0);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededDuranium(0);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededCrystal(0);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededIridium(0);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededDilithium(0);
	m_BuildingInfo.ElementAt(m_iClick).SetFoodProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetIPProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetEnergyProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetSPProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetFPProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetTitanProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetDeuteriumProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetDuraniumProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetCrystalProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetIridiumProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetDilithiumProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetLatinumProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetMoralProd(0);
	m_BuildingInfo.ElementAt(m_iClick).SetMoralProdEmpire(0);
	m_BuildingInfo.ElementAt(m_iClick).SetFoodBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetIndustryBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetEnergyBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetSecurityBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetResearchBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetTitanBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetDeuteriumBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetDuraniumBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetCrystalBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetIridiumBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetDilithiumBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetAllRessourceBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetLatinumBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetBioTechBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetEnergyTechBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetCompTechBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetPropulsionTechBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetConstructionTechBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetWeaponTechBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetInnerSecurityBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetEconomySpyBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetEconomySabotageBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetResearchSpyBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetResearchSabotageBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetMilitarySpyBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetMilitarySabotageBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShipYard(0);
	m_BuildingInfo.ElementAt(m_iClick).SetBuildableShipTypes(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShipYardSpeed(0);
	m_BuildingInfo.ElementAt(m_iClick).SetBarrack(0);
	m_BuildingInfo.ElementAt(m_iClick).SetBarrackSpeed(0);
	m_BuildingInfo.ElementAt(m_iClick).SetHitpoints(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShieldPower(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShieldPowerBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShipDefend(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShipDefendBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetGroundDefend(0);
	m_BuildingInfo.ElementAt(m_iClick).SetGroundDefendBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetScanPower(0);
	m_BuildingInfo.ElementAt(m_iClick).SetScanPowerBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetScanRange(0);
	m_BuildingInfo.ElementAt(m_iClick).SetScanRangeBoni(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShipTraining(0);
	m_BuildingInfo.ElementAt(m_iClick).SetTroopTraining(0);
	m_BuildingInfo.ElementAt(m_iClick).SetResistance(0);
	m_BuildingInfo.ElementAt(m_iClick).SetAddedTradeRoutes(0);
	m_BuildingInfo.ElementAt(m_iClick).SetIncomeOnTradeRoutes(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShipRecycling(0);
	m_BuildingInfo.ElementAt(m_iClick).SetBuildingBuildSpeed(0);
	m_BuildingInfo.ElementAt(m_iClick).SetUpdateBuildSpeed(0);
	m_BuildingInfo.ElementAt(m_iClick).SetShipBuildSpeed(0);
	m_BuildingInfo.ElementAt(m_iClick).SetTroopBuildSpeed(0);
	m_BuildingInfo.ElementAt(m_iClick).SetPredecessor(0);
	m_BuildingInfo.ElementAt(m_iClick).SetIsBuildingOnline(0);
	m_BuildingInfo.ElementAt(m_iClick).SetWorker(0);
	m_BuildingInfo.ElementAt(m_iClick).SetNeverReady(0);
	m_BuildingInfo.ElementAt(m_iClick).SetEquivalent(NOBODY,0);// niemand-index immer auf NULL setzen
	m_BuildingInfo.ElementAt(m_iClick).SetEquivalent(HUMAN,0);
	m_BuildingInfo.ElementAt(m_iClick).SetEquivalent(FERENGI,0);
	m_BuildingInfo.ElementAt(m_iClick).SetEquivalent(KLINGON,0);
	m_BuildingInfo.ElementAt(m_iClick).SetEquivalent(ROMULAN,0);
	m_BuildingInfo.ElementAt(m_iClick).SetEquivalent(CARDASSIAN,0);
	m_BuildingInfo.ElementAt(m_iClick).SetEquivalent(DOMINION,0);
	for (int res = TITAN; res <= DILITHIUM; res++)
		m_BuildingInfo.ElementAt(m_iClick).SetResourceDistributor(res, false);
	m_BuildingInfo.ElementAt(m_iClick).SetNeededSystems(0);
	
	CString s;
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);
	for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
	{
		s.Format("%d: %s",m_BuildingInfo.GetAt(i).GetRunningNumber(),m_BuildingInfo.GetAt(i).GetBuildingName(m_Language.GetCurSel()));
		m_ListBox.AddString(s,color[m_BuildingInfo.GetAt(i).GetOwnerOfBuilding()]);
	}
	m_ListBox.SetCurSel(m_iClick);
	this->OnSelchangeList();
}

void CBuildingEditorDlg::OnCopy() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CBuildingInfo bi;
	m_BuildingInfo.Add(bi);
	m_iClick = m_BuildingInfo.GetUpperBound();
	m_BuildingInfo.ElementAt(m_iClick).SetRunningNumber(m_iClick+1);
	DialogToData();	
	CString s;
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);
	for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
	{
		s.Format("%d: %s",m_BuildingInfo.GetAt(i).GetRunningNumber(),m_BuildingInfo.GetAt(i).GetBuildingName(m_Language.GetCurSel()));
		m_ListBox.AddString(s,color[m_BuildingInfo.GetAt(i).GetOwnerOfBuilding()]);
	}
	m_ListBox.SetCurSel(m_iClick);
	this->OnSelchangeList();	
}


void CBuildingEditorDlg::OnDelete() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_BuildingInfo.GetSize() > 0)
	{
		m_iClick = m_ListBox.GetCurSel();
		// alle Gebäude in der DB durchgehen. Wenn ein Gebäude bei den Äquivalenzgebäuden eine ID besitzt, welche größer
		// ist als die ID unseres Gebäudes, dann müssen wir diese dekrementieren
		for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
		{
			// ID´s, welche größer als unser markiertes Gebäude sind müssen dekrementiert werden
			if (m_BuildingInfo.GetAt(i).GetRunningNumber() > m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetRunningNumber(m_BuildingInfo.GetAt(i).GetRunningNumber()-1);
			for (int j = HUMAN; j <= DOMINION; j++)
			{
				// bei größer müssen wir dekrementieren
				if (m_BuildingInfo.GetAt(i).GetEquivalent(j) > m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				{
					m_BuildingInfo.ElementAt(i).SetEquivalent(j,m_BuildingInfo.GetAt(i).GetEquivalent(j)-1);
				}
				// bei gleich müssen wir sie auf NULL setzen
				else if (m_BuildingInfo.GetAt(i).GetEquivalent(j) == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				{
					m_BuildingInfo.ElementAt(i).SetEquivalent(j,0);
				}
			}
			// PredecessorID´s welche größer als die markierte ID sind werden dekrementiert. Sind sie gleich, dann wird
			// die PredecessorID gelöscht, sprich es gibt keinen Vorgänger mehr
			if (m_BuildingInfo.GetAt(i).GetPredecessorID() > m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
			{
				m_BuildingInfo.ElementAt(i).SetPredecessor(m_BuildingInfo.GetAt(i).GetPredecessorID()-1);
			}
			else if (m_BuildingInfo.GetAt(i).GetPredecessorID() == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
			{
				m_BuildingInfo.ElementAt(i).SetPredecessor(0);
			}
			// Genau wie bei den PredecessorID´s verhält es sich auch bei den ID´s von Min und Max
			if (m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber > m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInEmpire(m_BuildingInfo.GetAt(i).GetMaxInEmpire().Number,m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber-1);
			else if (m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInEmpire(0,0);
			if (m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber > m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInEmpire(m_BuildingInfo.GetAt(i).GetMinInEmpire().Number,m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber-1);
			else if (m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInEmpire(0,0);
			if (m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber > m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInSystem(m_BuildingInfo.GetAt(i).GetMaxInSystem().Number,m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber-1);
			else if (m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInSystem(0,0);
			if (m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber > m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInSystem(m_BuildingInfo.GetAt(i).GetMinInSystem().Number,m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber-1);
			else if (m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInSystem(0,0);
		}
		// markiertes Gebäude aus der DB löschen
		m_BuildingInfo.RemoveAt(m_iClick);
		CString s;
		for (int i = 0; i < m_ListBox.GetCount(); )
			m_ListBox.DeleteString(i);
		for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
		{
			s.Format("%d: %s",m_BuildingInfo.GetAt(i).GetRunningNumber(),m_BuildingInfo.GetAt(i).GetBuildingName(m_Language.GetCurSel()));
			m_ListBox.AddString(s,color[m_BuildingInfo.GetAt(i).GetOwnerOfBuilding()]);
		}
		if (m_BuildingInfo.GetSize() > 0)
		{
			m_ListBox.SetCurSel(0);
			m_iClick = 0;
		}
		this->OnSelchangeList();
	}
}

void CBuildingEditorDlg::OnMoveup() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_BuildingInfo.GetSize() > 0 && m_ListBox.GetCurSel() != 0)
	{
		DialogToData();
		m_iClick = m_ListBox.GetCurSel();
		// alle Gebäude in der DB durchgehen. Wenn ein Gebäude bei den Äquivalenzgebäuden unsere ID besitzt, 
		// dann müssen wir diese dekrementieren. Finden wir die ID vom Gebäude über uns, dann müssen wir diese
		// inkrementieren
		for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
		{
			for (int j = HUMAN; j <= DOMINION; j++)
			{
				if (m_BuildingInfo.GetAt(i).GetEquivalent(j) == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
					m_BuildingInfo.ElementAt(i).SetEquivalent(j,m_BuildingInfo.GetAt(i).GetEquivalent(j)-1);
				else if (m_BuildingInfo.GetAt(i).GetEquivalent(j) == m_BuildingInfo.GetAt(m_iClick-1).GetRunningNumber())
					m_BuildingInfo.ElementAt(i).SetEquivalent(j,m_BuildingInfo.GetAt(i).GetEquivalent(j)+1);
			}
			// PredecessorID´s welche gleich der markierten ID sind werden dekrementiert.
			// PredecessorID´s welche gleich der ID des oberen Gebäudes sind werden inkrementiert.
			if (m_BuildingInfo.GetAt(i).GetPredecessorID() == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetPredecessor(m_BuildingInfo.GetAt(i).GetPredecessorID()-1);
			else if (m_BuildingInfo.GetAt(i).GetPredecessorID() == m_BuildingInfo.GetAt(m_iClick-1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetPredecessor(m_BuildingInfo.GetAt(i).GetPredecessorID()+1);

			// Genau wie bei den PredecessorID´s verhält es sich auch bei den ID´s von Min und Max
			if (m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInEmpire(m_BuildingInfo.GetAt(i).GetMaxInEmpire().Number,m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber-1);
			else if (m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick-1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInEmpire(m_BuildingInfo.GetAt(i).GetMaxInEmpire().Number,m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber+1);
			if (m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInEmpire(m_BuildingInfo.GetAt(i).GetMinInEmpire().Number,m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber-1);
			else if (m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick-1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInEmpire(m_BuildingInfo.GetAt(i).GetMinInEmpire().Number,m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber+1);
			if (m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInSystem(m_BuildingInfo.GetAt(i).GetMaxInSystem().Number,m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber-1);
			else if (m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick-1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInSystem(m_BuildingInfo.GetAt(i).GetMaxInSystem().Number,m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber+1);
			if (m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInSystem(m_BuildingInfo.GetAt(i).GetMinInSystem().Number,m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber-1);
			else if (m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick-1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInSystem(m_BuildingInfo.GetAt(i).GetMinInSystem().Number,m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber+1);
		}
		// markiertes Gebäude mit dem drüber vertauschen
		CBuildingInfo temp = m_BuildingInfo.GetAt(m_iClick-1);
		m_BuildingInfo.SetAt(m_iClick-1,m_BuildingInfo.GetAt(m_iClick));
		m_BuildingInfo.SetAt(m_iClick,temp);
		// nun neue ID´s zuweisen
		m_BuildingInfo.ElementAt(m_iClick-1).SetRunningNumber(m_BuildingInfo.GetAt(m_iClick-1).GetRunningNumber()-1);
		m_BuildingInfo.ElementAt(m_iClick).SetRunningNumber(m_BuildingInfo.GetAt(m_iClick).GetRunningNumber()+1);

		if (!m_bAutoSorting)
		{
			CString s;
			for (int i = 0; i < m_ListBox.GetCount(); )
				m_ListBox.DeleteString(i);
			for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
			{
				s.Format("%d: %s",m_BuildingInfo.GetAt(i).GetRunningNumber(),m_BuildingInfo.GetAt(i).GetBuildingName(m_Language.GetCurSel()));
				m_ListBox.AddString(s,color[m_BuildingInfo.GetAt(i).GetOwnerOfBuilding()]);
			}
		}
		if (m_BuildingInfo.GetSize() > 0)
		{
			m_iClick--;
			m_ListBox.SetCurSel(m_iClick);
		}
		this->OnSelchangeList();
	}
}


void CBuildingEditorDlg::OnMovedown() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_BuildingInfo.GetSize() > 0 && m_ListBox.GetCurSel() != m_BuildingInfo.GetUpperBound())
	{
		DialogToData();
		m_iClick = m_ListBox.GetCurSel();
		// alle Gebäude in der DB durchgehen. Wenn ein Gebäude bei den Äquivalenzgebäuden unsere ID besitzt, 
		// dann müssen wir diese inkrementieren. Finden wir die ID vom Gebäude unter uns, dann müssen wir diese
		// dekrementieren
		for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
		{
			for (int j = HUMAN; j <= DOMINION; j++)
			{
				if (m_BuildingInfo.GetAt(i).GetEquivalent(j) == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
					m_BuildingInfo.ElementAt(i).SetEquivalent(j,m_BuildingInfo.GetAt(i).GetEquivalent(j)+1);
				else if (m_BuildingInfo.GetAt(i).GetEquivalent(j) == m_BuildingInfo.GetAt(m_iClick+1).GetRunningNumber())
					m_BuildingInfo.ElementAt(i).SetEquivalent(j,m_BuildingInfo.GetAt(i).GetEquivalent(j)-1);
			}
			// PredecessorID´s welche gleich der markierten ID sind werden inkrementiert.
			// PredecessorID´s welche gleich der ID des unteren Gebäudes sind werden derementiert.
			if (m_BuildingInfo.GetAt(i).GetPredecessorID() == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetPredecessor(m_BuildingInfo.GetAt(i).GetPredecessorID()+1);
			else if (m_BuildingInfo.GetAt(i).GetPredecessorID() == m_BuildingInfo.GetAt(m_iClick+1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetPredecessor(m_BuildingInfo.GetAt(i).GetPredecessorID()-1);

			// Genau wie bei den PredecessorID´s verhält es sich auch bei den ID´s von Min und Max
			if (m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInEmpire(m_BuildingInfo.GetAt(i).GetMaxInEmpire().Number,m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber+1);
			else if (m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick+1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInEmpire(m_BuildingInfo.GetAt(i).GetMaxInEmpire().Number,m_BuildingInfo.GetAt(i).GetMaxInEmpire().RunningNumber-1);
			if (m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInEmpire(m_BuildingInfo.GetAt(i).GetMinInEmpire().Number,m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber+1);
			else if (m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber == m_BuildingInfo.GetAt(m_iClick+1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInEmpire(m_BuildingInfo.GetAt(i).GetMinInEmpire().Number,m_BuildingInfo.GetAt(i).GetMinInEmpire().RunningNumber-1);
			if (m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInSystem(m_BuildingInfo.GetAt(i).GetMaxInSystem().Number,m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber+1);
			else if (m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick+1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMaxInSystem(m_BuildingInfo.GetAt(i).GetMaxInSystem().Number,m_BuildingInfo.GetAt(i).GetMaxInSystem().RunningNumber-1);
			if (m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInSystem(m_BuildingInfo.GetAt(i).GetMinInSystem().Number,m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber+1);
			else if (m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber == m_BuildingInfo.GetAt(m_iClick+1).GetRunningNumber())
				m_BuildingInfo.ElementAt(i).SetMinInSystem(m_BuildingInfo.GetAt(i).GetMinInSystem().Number,m_BuildingInfo.GetAt(i).GetMinInSystem().RunningNumber-1);
			
		}
		// markiertes Gebäude mit dem darunter vertauschen
		CBuildingInfo temp = m_BuildingInfo.GetAt(m_iClick+1);
		m_BuildingInfo.SetAt(m_iClick+1,m_BuildingInfo.GetAt(m_iClick));
		m_BuildingInfo.SetAt(m_iClick,temp);
		// nun neue ID´s zuweisen
		m_BuildingInfo.ElementAt(m_iClick+1).SetRunningNumber(m_BuildingInfo.GetAt(m_iClick+1).GetRunningNumber()+1);
		m_BuildingInfo.ElementAt(m_iClick).SetRunningNumber(m_BuildingInfo.GetAt(m_iClick).GetRunningNumber()-1);

		CString s;
		for (int i = 0; i < m_ListBox.GetCount(); )
			m_ListBox.DeleteString(i);
		for (int i = 0; i < m_BuildingInfo.GetSize(); i++)
		{
			s.Format("%d: %s",m_BuildingInfo.GetAt(i).GetRunningNumber(),m_BuildingInfo.GetAt(i).GetBuildingName(m_Language.GetCurSel()));
			m_ListBox.AddString(s,color[m_BuildingInfo.GetAt(i).GetOwnerOfBuilding()]);
		}
		if (m_BuildingInfo.GetSize() > 0)
		{
			m_iClick++;
			m_ListBox.SetCurSel(m_iClick);
		}
		this->OnSelchangeList();
	}
}


void CBuildingEditorDlg::DataToDialog()
{
	if (m_BuildingInfo.GetSize() > 0)
	{
		((CInfoDlg*)m_dynTabCtrl.GetTab(0))->DataToDialog(&m_BuildingInfo[m_ListBox.GetCurSel()], m_Language.GetCurSel());
		((CInfoDlg*)m_dynTabCtrl.GetTab(1))->DataToDialog(&m_BuildingInfo[m_ListBox.GetCurSel()], m_Language.GetCurSel());
		((CInfoDlg*)m_dynTabCtrl.GetTab(2))->DataToDialog(&m_BuildingInfo[m_ListBox.GetCurSel()], m_Language.GetCurSel());
		((CInfoDlg*)m_dynTabCtrl.GetTab(3))->DataToDialog(&m_BuildingInfo[m_ListBox.GetCurSel()], m_Language.GetCurSel());

		// Allgemeine Informationen
		m_iRunningNumber = m_BuildingInfo.GetAt(m_ListBox.GetCurSel()).GetRunningNumber();
					
		// Daten in den Dialog bringen
		this->UpdateData(FALSE);
	}
}

void CBuildingEditorDlg::DialogToData()
{
	if (m_BuildingInfo.GetSize() > 0)
	{
		((CInfoDlg*)m_dynTabCtrl.GetTab(0))->DialogToData(&m_BuildingInfo[m_iClick], m_Language.GetCurSel());
		((CInfoDlg*)m_dynTabCtrl.GetTab(1))->DialogToData(&m_BuildingInfo[m_iClick], m_Language.GetCurSel());
		((CInfoDlg*)m_dynTabCtrl.GetTab(2))->DialogToData(&m_BuildingInfo[m_iClick], m_Language.GetCurSel());
		((CInfoDlg*)m_dynTabCtrl.GetTab(3))->DialogToData(&m_BuildingInfo[m_iClick], m_Language.GetCurSel());

		this->UpdateData(TRUE);			
	}
}

void CBuildingEditorDlg::AutoSort()
{
	// Die Gebäudeliste von hinten durchgehen. Wenn eine zugehörige Racenummer kleiner
	// als die Racenummer des vorangegangen Gebäudes ist, dann soll dieses Gebäude
	// in der Liste soweit vorgerückt werden, bis es an letzter Stelle seiner richtigen
	// Racenummer steht.
	for (int i = m_BuildingInfo.GetUpperBound(); i > 0; i--)
	{
		int lastID = m_BuildingInfo.GetAt(i).GetOwnerOfBuilding();
		if (lastID < m_BuildingInfo.GetAt(i-1).GetOwnerOfBuilding() || lastID != 0 && m_BuildingInfo.GetAt(i-1).GetOwnerOfBuilding() == 0)
		{
			// jetzt in der Liste weiter voranrücken um rauszubekommen, um wieviel Positionen das
			// Gebäude vorgerückt werden muss
			int counter = 0;
			for (int j = i-1; j > 0; j--)
			{
				if (m_BuildingInfo.GetAt(j).GetOwnerOfBuilding() == lastID)
				{
					m_bAutoSorting = TRUE;
					m_ListBox.SetCurSel(i);
					for (int k = 0; k < counter; k++)
					{
						this->OnMoveup();
					//	AfxMessageBox(m_BuildingInfo.GetAt(m_ListBox.GetCurSel()).GetBuildingName());
					}
					i = m_BuildingInfo.GetSize();
					m_bAutoSorting = FALSE;
					break;
				}
				counter++;
			}
		}
	}
}


void CBuildingEditorDlg::OnSelchangeLanguage() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
	// Daten des alten angeklickten Gebäudes speichern
	BOOLEAN sel = !m_Language.GetCurSel();
	m_Language.SetCurSel(sel);
	DialogToData();	
	// Daten des neuen (jetzt markierten) Gebäudes anzeigen
	m_Language.SetCurSel(!sel);
	DataToDialog();
}
