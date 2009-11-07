// ShipEditorDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "ShipEditor.h"
#include "ShipEditorDlg.h"
#include "ModifyTubeDlg.h"
#include "ModifyBeamDlg.h"

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
// CShipEditorDlg Dialogfeld

CShipEditorDlg::CShipEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShipEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShipEditorDlg)
	m_strShipDescription = _T("");
	m_strShipType = _T("");
	m_iBioTech = 0;
	m_iComputerTech = 0;
	m_iConstructionTech = 0;
	m_iEnergyTech = 0;
	m_iPropulsionTech = 0;
	m_iWeaponsTech = 0;
	m_iIndustry = 0;
	m_iIridium = 0;
	m_iDeuterium = 0;
	m_iCrystal = 0;
	m_iDilithium = 0;
	m_iDuranium = 0;
	m_iTitan = 0;
	m_iHullMaterial = -1;
	m_iBaseHull = 0;
	m_bDoubleHull = FALSE;
	m_iMaxShield = 0;
	m_iShieldType = 0;
	m_iRange = -1;
	m_iSpeed = 0;
	m_iScanpower = 0;
	m_iScanRange = 0;
	m_iStorageRoom = 0;
	m_iStationbuildPoints = 0;
	m_iColonizationPoints = 0;
	m_iCamouflagePower = 0;
	m_iMaintenanceCosts = 0;
	m_iRace = -1;
	m_strShipClass = _T("");
	m_strOnlyOnSystem = _T("");
	m_byShipSize = 0;
	m_byManeuverability = 0;
	m_bAblative = FALSE;
	m_bRegenerative = FALSE;
	m_strObsolete = _T("");
	m_bPolarisation = FALSE;
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShipEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShipEditorDlg)
	DDX_Control(pDX, IDC_SPECIAL2, m_Special2);
	DDX_Control(pDX, IDC_SPECIAL1, m_Special1);
	DDX_Control(pDX, IDC_TORPEDOLIST, m_TorpedoList);
	DDX_Control(pDX, IDC_BEAMLIST, m_BeamList);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_SHIPTYPE, m_iComboShipType);
	DDX_Control(pDX, IDC_OBSOLETE, m_CtrlObsolets);
	DDX_Text(pDX, IDC_SHIPDESCRIPTION, m_strShipDescription);
	DDV_MaxChars(pDX, m_strShipDescription, 1000);
	DDX_CBString(pDX, IDC_SHIPTYPE, m_strShipType);
	DDX_Text(pDX, IDC_BIOTECH, m_iBioTech);
	DDV_MinMaxUInt(pDX, m_iBioTech, 0, 255);
	DDX_Text(pDX, IDC_COMPUTERTECH, m_iComputerTech);
	DDV_MinMaxUInt(pDX, m_iComputerTech, 0, 255);
	DDX_Text(pDX, IDC_CONSTRUCTIONTECH, m_iConstructionTech);
	DDV_MinMaxUInt(pDX, m_iConstructionTech, 0, 255);
	DDX_Text(pDX, IDC_ENERGYTECH, m_iEnergyTech);
	DDV_MinMaxUInt(pDX, m_iEnergyTech, 0, 255);
	DDX_Text(pDX, IDC_PROPULSIONTECH, m_iPropulsionTech);
	DDV_MinMaxUInt(pDX, m_iPropulsionTech, 0, 255);
	DDX_Text(pDX, IDC_WEAPONTECH, m_iWeaponsTech);
	DDV_MinMaxUInt(pDX, m_iWeaponsTech, 0, 255);
	DDX_Text(pDX, IDC_INDUSTRY, m_iIndustry);
	DDX_Text(pDX, IDC_IRIDIUM, m_iIridium);
	DDX_Text(pDX, IDC_DEUTERIUM, m_iDeuterium);
	DDX_Text(pDX, IDC_CRYSTAL, m_iCrystal);
	DDX_Text(pDX, IDC_DILITHIUM, m_iDilithium);
	DDX_Text(pDX, IDC_DURANIUM, m_iDuranium);
	DDX_Text(pDX, IDC_TITAN, m_iTitan);
	DDX_Radio(pDX, IDC_MATERIAL, m_iHullMaterial);
	DDX_Text(pDX, IDC_BASEHULL, m_iBaseHull);
	DDX_Check(pDX, IDC_DOUBLEHULL, m_bDoubleHull);
	DDX_Text(pDX, IDC_MAXSHIELDS, m_iMaxShield);
	DDV_MinMaxUInt(pDX, m_iMaxShield, 0, 64000);
	DDX_Text(pDX, IDC_SHIELDTYPE, m_iShieldType);
	DDV_MinMaxUInt(pDX, m_iShieldType, 0, 255);
	DDX_Radio(pDX, IDC_RANGE, m_iRange);
	DDX_Text(pDX, IDC_SPEED, m_iSpeed);
	DDV_MinMaxUInt(pDX, m_iSpeed, 0, 255);
	DDX_Text(pDX, IDC_SCANPOWER, m_iScanpower);
	DDV_MinMaxUInt(pDX, m_iScanpower, 0, 64000);
	DDX_Text(pDX, IDC_SCANRANGE, m_iScanRange);
	DDV_MinMaxUInt(pDX, m_iScanRange, 0, 255);
	DDX_Text(pDX, IDC_STORAGEROOM, m_iStorageRoom);
	DDV_MinMaxUInt(pDX, m_iStorageRoom, 0, 64000);
	DDX_Text(pDX, IDC_STATIONBUILDPOINTS, m_iStationbuildPoints);
	DDV_MinMaxUInt(pDX, m_iStationbuildPoints, 0, 255);
	DDX_Text(pDX, IDC_COLONIZATIONPOINTS, m_iColonizationPoints);
	DDV_MinMaxUInt(pDX, m_iColonizationPoints, 0, 255);
	DDX_Text(pDX, IDC_CAMOUFLAGEPOWER, m_iCamouflagePower);
	DDV_MinMaxUInt(pDX, m_iCamouflagePower, 0, 6);
	DDX_Text(pDX, IDC_MAINTENANCECOSTS, m_iMaintenanceCosts);
	DDV_MinMaxUInt(pDX, m_iMaintenanceCosts, 0, 64000);
	DDX_Radio(pDX, IDC_FEDERATION, m_iRace);
	DDX_Text(pDX, IDC_SHIPCLASS, m_strShipClass);
	DDX_Text(pDX, IDC_ONLYONSYSTEM, m_strOnlyOnSystem);
	DDX_Text(pDX, IDC_SHIPSIZE, m_byShipSize);
	DDV_MinMaxByte(pDX, m_byShipSize, 0, 3);
	DDX_Text(pDX, IDC_MANEUVER, m_byManeuverability);
	DDV_MinMaxByte(pDX, m_byManeuverability, 0, 8);
	DDX_Check(pDX, IDC_ABLATIVE, m_bAblative);
	DDX_Check(pDX, IDC_REGENERATIVE, m_bRegenerative);
	DDX_CBString(pDX, IDC_OBSOLETE, m_strObsolete);
	DDX_Check(pDX, IDC_POLARISATION, m_bPolarisation);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShipEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CShipEditorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeShipClassList)
	ON_LBN_SELCHANGE(IDC_BEAMLIST, OnSelchangeBeamlist)
	ON_BN_CLICKED(IDC_MODIFYBEAM, OnModifybeam)
	ON_BN_CLICKED(IDC_ADDBEAM, OnAddbeam)
	ON_BN_CLICKED(IDC_DELETEBEAM, OnDeletebeam)
	ON_BN_CLICKED(IDC_MODIFYTUBE, OnModifytube)
	ON_LBN_SELCHANGE(IDC_TORPEDOLIST, OnSelchangeTorpedolist)
	ON_BN_CLICKED(IDC_DELETETUBE, OnDeletetube)
	ON_BN_CLICKED(IDC_ADDTUBE, OnAddtube)
	ON_BN_CLICKED(IDC_NEWCLASS, OnNewclass)
	ON_BN_CLICKED(IDC_DELETECLASS, OnDeleteclass)
	ON_BN_CLICKED(IDC_SHOWSTATS, OnShowstats)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShipEditorDlg Nachrichten-Handler

BOOL CShipEditorDlg::OnInitDialog()
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
	fileReader->ReadDataFromFile(&m_ShipInfo);
	
	color[HUMAN]		= RGB(0,0,255);
	color[FERENGI]		= RGB(248,211,5);
	color[KLINGON]		= RGB(255,0,0);
	color[ROMULAN]		= RGB(0,140,0);
	color[CARDASSIAN]	= RGB(125,0,125);
	color[DOMINION]		= RGB(73,240,240);
	color[UNKNOWN]		= RGB(0,0,0);

	for (int i = 0; i < m_ShipInfo.GetSize(); i++)
		m_ListBox.AddString(m_ShipInfo.GetAt(i)->GetShipClass(), color[m_ShipInfo.GetAt(i)->GetRace()]);
	if (m_ShipInfo.GetSize() > 0)
	{
		m_Special1.SetCurSel(0);
		m_Special2.SetCurSel(0);
		m_ListBox.SetCurSel(0);
		m_iClick = 0;
	}
	this->OnSelchangeShipClassList();

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CShipEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CShipEditorDlg::OnDestroy()
{
	delete fileReader;

	for (int i = 0; i < m_ShipInfo.GetSize(); i++)
	{
		delete m_ShipInfo[i];
		m_ShipInfo[i] = NULL;
	}
	m_ShipInfo.RemoveAll();

	CDialog::OnDestroy();
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CShipEditorDlg::OnPaint() 
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
		UpdateData(TRUE);
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CShipEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CShipEditorDlg::DataToDialog()
{
	if (m_ShipInfo.GetSize() > 0)
	{
		m_strShipClass = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShipClass(); 
		m_strShipType = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShipTypeAsString();
		m_iShipType = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShipType();
		m_iComboShipType.SetCurSel(m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShipType());		
		m_iRace = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetRace()-1;
		m_strShipDescription = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShipDescription();
		m_iBioTech = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetBioTech();
		m_iEnergyTech = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetEnergyTech();
		m_iComputerTech = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetComputerTech();
		m_iPropulsionTech = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetPropulsionTech();
		m_iConstructionTech = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetConstructionTech();
		m_iWeaponsTech = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetWeaponTech();
		m_iIndustry = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetNeededIndustry();
		m_iTitan = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetNeededTitan();
		m_iDeuterium = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetNeededDeuterium();
		m_iDuranium = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetNeededDuranium();
		m_iCrystal = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetNeededCrystal();
		m_iIridium = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetNeededIridium();
		m_iDilithium = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetNeededDilithium();
		switch (m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetHull()->GetHullMaterial())
		{
			case TITAN:		m_iHullMaterial = 0; break;
			case DURANIUM:  m_iHullMaterial = 1; break;
			case IRIDIUM:	m_iHullMaterial = 2; break;
			default:		m_iHullMaterial = 0; break;
		}
		m_iBaseHull = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetHull()->GetBaseHull();
		m_bDoubleHull = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetHull()->GetDoubleHull();
		m_bAblative = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetHull()->GetAblative();
		m_bPolarisation = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetHull()->GetPolarisation();
		m_iMaxShield = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShield()->GetMaxShield();
		m_iShieldType = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShield()->GetShieldType();
		m_bRegenerative = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShield()->GetRegenerative();
		m_iRange = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetRange();
		m_iSpeed = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetSpeed();
		m_iScanpower = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetScanPower();
		m_iScanRange = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetScanRange();
		m_iStorageRoom = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetStorageRoom();
		m_iCamouflagePower = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetStealthPower();
		m_iColonizationPoints = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetColonizePoints();
		m_iStationbuildPoints = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetStationBuildPoints();
		m_iMaintenanceCosts = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetMaintenanceCosts();
		for (int i = 0; i < m_BeamList.GetCount();)
			m_BeamList.DeleteString(i);
		for (int i = 0; i < m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetBeamWeapons()->GetSize(); i++)
		{
			CString s;
			s.Format("%d x %s Type %d",m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetBeamWeapons()->GetAt(i).GetBeamNumber(),
									m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetBeamWeapons()->GetAt(i).GetBeamName(),
									m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetBeamWeapons()->GetAt(i).GetBeamType());
			m_BeamList.AddString(s);
		}
		for (int i = 0; i < m_TorpedoList.GetCount();)
			m_TorpedoList.DeleteString(i);
		for (int i = 0; i < m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetTorpedoWeapons()->GetSize(); i++)
			{
				CString s;
				s.Format("%d x %s",m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetTorpedoWeapons()->GetAt(i).GetNumberOfTupes(),
									m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetTorpedoWeapons()->GetAt(i).GetTupeName());
				m_TorpedoList.AddString(s);
			}
		m_BeamList.SetCurSel(-1);
		m_TorpedoList.SetCurSel(-1);
		this->GetDlgItem(IDC_MODIFYBEAM)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_DELETEBEAM)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_MODIFYTUBE)->EnableWindow(FALSE);
		this->GetDlgItem(IDC_DELETETUBE)->EnableWindow(FALSE);

		m_Special1.SetCurSel(m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetSpecial(0));
		m_Special2.SetCurSel(m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetSpecial(1));

		m_strOnlyOnSystem = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetOnlyInSystem();
		m_strObsolete = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetObsoleteShipClass();
		m_byShipSize = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetShipSize();
		m_byManeuverability = m_ShipInfo.GetAt(m_ListBox.GetCurSel())->GetManeuverability();
		// Daten in den Dialog bringen
		this->UpdateData(FALSE);
	}	
}

void CShipEditorDlg::DialogToData()
{
	if (m_ShipInfo.GetSize() > 0)
	{
		this->UpdateData(TRUE);
		// überprüfen, das in keinem String ein "newline" auftritt
		int newline = -1;
		newline = m_strShipClass.Find("\n");
		if (newline != -1)
		{
			AfxMessageBox("There is a newline in the ShipClassName!");
			m_strShipClass = "ShipClassError";
			newline = -1;
		}
		newline = m_strShipDescription.Find("\n");
		if (newline != -1)
		{
			AfxMessageBox("There is a newline in the ShipDescription!");
			m_strShipDescription = "no description";
			newline = -1;
		}	
		newline = m_strOnlyOnSystem.Find("\n");
		if (newline != -1)
		{
			AfxMessageBox("There is a newline in the OnlyInSystemName!");
			m_strOnlyOnSystem = "";
			newline = -1;
		}		
		newline = m_strObsolete.Find("\n");
		if (newline != -1)
		{
			AfxMessageBox("There is a newline in the ObsoleteClassName!");
			m_strObsolete = "";
			newline = -1;
		}
		m_ShipInfo.ElementAt(m_iClick)->SetShipClass(m_strShipClass);
		m_ShipInfo.ElementAt(m_iClick)->SetShipDescription(m_strShipDescription);
		m_ShipInfo.ElementAt(m_iClick)->SetShipType(m_iComboShipType.GetCurSel());
		m_ShipInfo.ElementAt(m_iClick)->SetRace(m_iRace+1);
		m_ShipInfo.ElementAt(m_iClick)->SetBioTech(m_iBioTech);
		m_ShipInfo.ElementAt(m_iClick)->SetEnergyTech(m_iEnergyTech);
		m_ShipInfo.ElementAt(m_iClick)->SetComputerTech(m_iComputerTech);
		m_ShipInfo.ElementAt(m_iClick)->SetPropulsionTech(m_iPropulsionTech);
		m_ShipInfo.ElementAt(m_iClick)->SetConstructionTech(m_iConstructionTech);
		m_ShipInfo.ElementAt(m_iClick)->SetWeaponTech(m_iWeaponsTech);
		m_ShipInfo.ElementAt(m_iClick)->SetNeededIndustry(m_iIndustry);
		m_ShipInfo.ElementAt(m_iClick)->SetNeededTitan(m_iTitan);
		m_ShipInfo.ElementAt(m_iClick)->SetNeededDeuterium(m_iDeuterium);
		m_ShipInfo.ElementAt(m_iClick)->SetNeededDuranium(m_iDuranium);
		m_ShipInfo.ElementAt(m_iClick)->SetNeededCrystal(m_iCrystal);
		m_ShipInfo.ElementAt(m_iClick)->SetNeededIridium(m_iIridium);
		m_ShipInfo.ElementAt(m_iClick)->SetNeededDilithium(m_iDilithium);
		switch (m_iHullMaterial)
		{
			case 0:	m_ShipInfo.ElementAt(m_iClick)->GetHull()->ModifyHull(m_bDoubleHull,m_iBaseHull,TITAN,m_bAblative,m_bPolarisation); break;
			case 1:	m_ShipInfo.ElementAt(m_iClick)->GetHull()->ModifyHull(m_bDoubleHull,m_iBaseHull,DURANIUM,m_bAblative,m_bPolarisation); break;
			case 2:	m_ShipInfo.ElementAt(m_iClick)->GetHull()->ModifyHull(m_bDoubleHull,m_iBaseHull,IRIDIUM,m_bAblative,m_bPolarisation); break;
			default:m_ShipInfo.ElementAt(m_iClick)->GetHull()->ModifyHull(m_bDoubleHull,m_iBaseHull,TITAN,m_bAblative,m_bPolarisation); break;
		}
		m_ShipInfo.ElementAt(m_iClick)->GetShield()->ModifyShield(m_iMaxShield,m_iShieldType,m_bRegenerative);
		m_ShipInfo.ElementAt(m_iClick)->SetRange(m_iRange);
		m_ShipInfo.ElementAt(m_iClick)->SetSpeed(m_iSpeed);
		m_ShipInfo.ElementAt(m_iClick)->SetScanPower(m_iScanpower);
		m_ShipInfo.ElementAt(m_iClick)->SetScanRange(m_iScanRange);
		m_ShipInfo.ElementAt(m_iClick)->SetStorageRoom(m_iStorageRoom);
		m_ShipInfo.ElementAt(m_iClick)->SetStealthPower(m_iCamouflagePower);
		m_ShipInfo.ElementAt(m_iClick)->SetColonizePoints(m_iColonizationPoints);
		m_ShipInfo.ElementAt(m_iClick)->SetStationBuildPoints(m_iStationbuildPoints);
		m_ShipInfo.ElementAt(m_iClick)->SetMaintenanceCosts(m_iMaintenanceCosts);

		m_ShipInfo.ElementAt(m_iClick)->SetSpecial(0, m_Special1.GetCurSel());
		m_ShipInfo.ElementAt(m_iClick)->SetSpecial(1, m_Special2.GetCurSel());

		m_ShipInfo.ElementAt(m_iClick)->SetOnlyInSystem(m_strOnlyOnSystem);
		m_ShipInfo.ElementAt(m_iClick)->SetObsoleteShipClass(m_strObsolete);
		m_ShipInfo.ElementAt(m_iClick)->SetShipSize(m_byShipSize);
		m_ShipInfo.ElementAt(m_iClick)->SetManeuverability(m_byManeuverability);
	}
}

void CShipEditorDlg::OnSave() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	// aktuelle Daten noch speichern
	DialogToData();
	fileReader->WriteDataToFile(&m_ShipInfo);
	// Nach speichern Schiffsliste neu berechnen...damit wir bei geänderten Namen auch ne aktuelle Auswahlliste bekommen
	CString s;
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);
	for (int i = 0; i < m_ShipInfo.GetSize(); i++)
		m_ListBox.AddString(m_ShipInfo.GetAt(i)->GetShipClass(), color[m_ShipInfo.GetAt(i)->GetRace()]);
	m_ListBox.SetCurSel(m_iClick);
	this->OnSelchangeShipClassList();
}

void CShipEditorDlg::OnLoad() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	fileReader->ReadDataFromFile(&m_ShipInfo);
	
	// aktuelle Schiffsliste löschen
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);

	for (int i = 0; i < m_ShipInfo.GetSize(); i++)
		m_ListBox.AddString(m_ShipInfo.GetAt(i)->GetShipClass(), color[m_ShipInfo.GetAt(i)->GetRace()]);
	
	if (m_ShipInfo.GetSize() > 0)
	{
		m_ListBox.SetCurSel(0);
		m_iClick = 0;
	}

	this->OnSelchangeShipClassList();	
}

void CShipEditorDlg::OnSelchangeShipClassList() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	// Daten des alten angeklickten Schiffes speichern
	if (m_iClick != m_ListBox.GetCurSel())
		DialogToData();	
	
	// Obsolete Schiffsklassen anzeigen
	int n = m_ListBox.GetCurSel();
	if (n != CB_ERR)
	{
		for (int i = 0; i < m_CtrlObsolets.GetCount(); )
			m_CtrlObsolets.DeleteString(i);
		CShipInfo* pShip = m_ShipInfo[n];
		for (int i = 0; i < m_ShipInfo.GetSize(); i++)
			if (pShip->GetRace() == m_ShipInfo[i]->GetRace() && i != n)
				m_CtrlObsolets.AddString(m_ShipInfo[i]->GetShipClass());
	}
	
	// Daten des neuen (jetzt markierten) Schiffes anzeigen
	DataToDialog();
	m_iClick = m_ListBox.GetCurSel();
}

void CShipEditorDlg::OnSelchangeBeamlist() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	this->GetDlgItem(IDC_MODIFYBEAM)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_DELETEBEAM)->EnableWindow(TRUE);
}

void CShipEditorDlg::OnModifybeam() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	int n = m_ListBox.GetCurSel();
	if (n == CB_ERR)
		return;

	int m = m_BeamList.GetCurSel();
	if (m == CB_ERR)
		return;
	
	CModifyBeamDlg* pDlg = new CModifyBeamDlg(&m_ShipInfo, n, m);

	if (pDlg->DoModal() == IDOK)
	{
		CBeamWeapons* weaponMod = &(m_ShipInfo.GetAt(n)->GetBeamWeapons()->GetAt(m));
		// Torpedowerfer anpassen
		weaponMod->ModifyBeamWeapon(
			pDlg->m_iBeamType,
			pDlg->m_iBeamDamage,
			pDlg->m_iBeamNumber,
			pDlg->m_strBeamName,
			pDlg->m_bModulating,
			pDlg->m_bPiercing,
			pDlg->m_byBonus,
			pDlg->m_byBeamtime,
			pDlg->m_byRechargeTime,
			pDlg->m_byShootNumber);

		// nun jedes Schiff, welches diese Waffe drauf hat anpassen, da sich Werte geändert haben könnten
		for (int i = 0; i < m_ShipInfo.GetSize(); i++)
		{
			for (int j = 0; j < m_ShipInfo.GetAt(i)->GetBeamWeapons()->GetSize(); j++)
			{
				CBeamWeapons* weaponNew = &(m_ShipInfo.GetAt(i)->GetBeamWeapons()->GetAt(j));
				if (weaponNew->GetBeamName() == weaponMod->GetBeamName())
				{	
					weaponNew->ModifyBeamWeapon(
						weaponNew->GetBeamType(),		// bleibt gleich
						weaponMod->GetBeamPower(),		// anpassen
						weaponNew->GetBeamNumber(),		// bleibt gleich
						weaponNew->GetBeamName(),		// bleibt gleich
						weaponMod->GetModulating(),		// anpassen
						weaponMod->GetPiercing(),		// anpassen
						weaponMod->GetBonus(),			// anpassen
						weaponMod->GetBeamLenght(),		// anpassen
						weaponMod->GetRechargeTime(),	// anpassen
						weaponMod->GetShootNumber());	// anpassen
				}
			}
		}
		DataToDialog();
	}

	delete pDlg;
	pDlg = NULL;
}

void CShipEditorDlg::OnAddbeam() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_ListBox.GetCurSel() != CB_ERR)
	{
		CModifyBeamDlg* pDlg = new CModifyBeamDlg(&m_ShipInfo, m_ListBox.GetCurSel(), CB_ERR);
		if (pDlg->DoModal() == IDOK)
		{
			CBeamWeapons new_beam;
			new_beam.ModifyBeamWeapon(
				pDlg->m_iBeamType,
				pDlg->m_iBeamDamage,
				pDlg->m_iBeamNumber,
				pDlg->m_strBeamName,
				pDlg->m_bModulating,
				pDlg->m_bPiercing,
				pDlg->m_byBonus,
				pDlg->m_byBeamtime,
				pDlg->m_byRechargeTime,
				pDlg->m_byShootNumber);
			m_ShipInfo.ElementAt(m_ListBox.GetCurSel())->GetBeamWeapons()->Add(new_beam);
			DataToDialog();
		}
		delete pDlg;
		pDlg = NULL;		
	}
}

void CShipEditorDlg::OnDeletebeam() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_ListBox.GetCurSel() != -1 && m_BeamList.GetCurSel() != -1)
	{
		m_ShipInfo.ElementAt(m_ListBox.GetCurSel())->GetBeamWeapons()->RemoveAt(m_BeamList.GetCurSel());
		DataToDialog();
	}
}

void CShipEditorDlg::OnSelchangeTorpedolist() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	this->GetDlgItem(IDC_MODIFYTUBE)->EnableWindow(TRUE);
	this->GetDlgItem(IDC_DELETETUBE)->EnableWindow(TRUE);
}

void CShipEditorDlg::OnAddtube() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_ListBox.GetCurSel() != CB_ERR)
	{
		CModifyTubeDlg* pDlg = new CModifyTubeDlg(&m_ShipInfo, m_ListBox.GetCurSel(), CB_ERR);
		if (pDlg->DoModal() == IDOK)
		{
			CTorpedoWeapons new_torpedo;
			new_torpedo.ModifyTorpedoWeapon(
				pDlg->m_iTorpedoType,
				pDlg->m_iTorpedoNumber,
				pDlg->m_iTubeFirerate,
				pDlg->m_iTubeNumber,
				pDlg->m_strTubeName,
				pDlg->m_bOnlyMicroPhoton,
				pDlg->m_byAccuracy);
			m_ShipInfo.ElementAt(m_ListBox.GetCurSel())->GetTorpedoWeapons()->Add(new_torpedo);
			DataToDialog();
		}
		delete pDlg;
		pDlg = NULL;
	}
}

void CShipEditorDlg::OnModifytube() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	int n = m_ListBox.GetCurSel();
	if (n == CB_ERR)
		return;

	int m = m_TorpedoList.GetCurSel();
	if (m == CB_ERR)
		return;
	
	CModifyTubeDlg* pDlg = new CModifyTubeDlg(&m_ShipInfo, n, m);

	if (pDlg->DoModal() == IDOK)
	{
		CTorpedoWeapons* weaponMod = &(m_ShipInfo.GetAt(n)->GetTorpedoWeapons()->GetAt(m));
		// Torpedowerfer anpassen
		weaponMod->ModifyTorpedoWeapon(
			pDlg->m_iTorpedoType,
			pDlg->m_iTorpedoNumber,
			pDlg->m_iTubeFirerate,
			pDlg->m_iTubeNumber,
			pDlg->m_strTubeName,
			pDlg->m_bOnlyMicroPhoton,
			pDlg->m_byAccuracy);
		// nun jedes Schiff, welches diese Waffe drauf hat anpassen, da sich Werte geändert haben könnten
		for (int i = 0; i < m_ShipInfo.GetSize(); i++)
		{
			for (int j = 0; j < m_ShipInfo.GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
			{
				CTorpedoWeapons* weaponNew = &(m_ShipInfo.GetAt(i)->GetTorpedoWeapons()->GetAt(j));
				if (weaponNew->GetTupeName() == weaponMod->GetTupeName())
				{				
					weaponNew->ModifyTorpedoWeapon(
						weaponNew->GetTorpedoType(),	// bleibt gleich
						weaponMod->GetNumber(),			// anpassen
						weaponMod->GetTupeFirerate(),	// anpassen
						weaponNew->GetNumberOfTupes(),	// bleibt gleich
						weaponNew->GetTupeName(),		// bleibt gleich
						weaponMod->GetOnlyMicroPhoton(),// anpassen
						weaponNew->GetAccuracy());		// bleibt gleich
				}
			}
		}	

		DataToDialog();
	}

	delete pDlg;
	pDlg = NULL;
}

void CShipEditorDlg::OnDeletetube() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_ListBox.GetCurSel() != -1 && m_TorpedoList.GetCurSel() != -1)
	{
		m_ShipInfo.ElementAt(m_ListBox.GetCurSel())->GetTorpedoWeapons()->RemoveAt(m_TorpedoList.GetCurSel());
		DataToDialog();
	}
	
}

void CShipEditorDlg::OnNewclass() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_NewClassNameDlg.DoModal() == IDOK)
	{
		DialogToData();
		CShipInfo* newClass = new CShipInfo();
		newClass->SetRace(UNKNOWN);
		newClass->SetShipClass(m_NewClassNameDlg.m_strNewClassName);
		newClass->SetShipDescription("Shipdescription is missing");
		newClass->SetShipType(SCOUT);
		newClass->SetShipSize(NORMAL);
		newClass->SetManeuverability(1);
		newClass->SetBioTech(0);
		newClass->SetEnergyTech(0);
		newClass->SetComputerTech(0);
		newClass->SetPropulsionTech(0);
		newClass->SetConstructionTech(0);
		newClass->SetWeaponTech(0);
		newClass->SetNeededIndustry(0);
		newClass->SetNeededTitan(0);
		newClass->SetNeededDeuterium(0);
		newClass->SetNeededDuranium(0);
		newClass->SetNeededCrystal(0);
		newClass->SetNeededIridium(0);
		newClass->SetNeededDilithium(1);
		newClass->SetOnlyInSystem("");
		newClass->SetObsoleteShipClass("");
		newClass->GetHull()->ModifyHull(0,0,TITAN,0,0);
		newClass->GetShield()->ModifyShield(0,1,0);
		newClass->SetSpeed(1);
		newClass->SetRange(0);
		newClass->SetScanPower(0);
		newClass->SetScanRange(0);
		newClass->SetStealthPower(0);
		newClass->SetStorageRoom(0);
		newClass->SetColonizePoints(0);
		newClass->SetStationBuildPoints(0);
		newClass->SetMaintenanceCosts(0);
		
		m_ShipInfo.Add(newClass);
		m_ListBox.AddString(m_NewClassNameDlg.m_strNewClassName, color[newClass->GetRace()]);	
		m_ListBox.SetCurSel(m_ShipInfo.GetUpperBound());
		m_iClick = m_ShipInfo.GetUpperBound();
		m_Special1.SetCurSel(0);
		m_Special2.SetCurSel(0);
		this->OnSelchangeShipClassList();
	}
}

void CShipEditorDlg::OnDeleteclass() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_ShipInfo.GetSize() > 0)
	{
		delete m_ShipInfo[m_iClick];
		m_ShipInfo[m_iClick] = NULL;
		m_ShipInfo.RemoveAt(m_iClick);
	}
	// aktuelle Rassenliste löschen
	for (int i = 0; i < m_ListBox.GetCount(); )
		m_ListBox.DeleteString(i);

	for (int i = 0; i < m_ShipInfo.GetSize(); i++)
		m_ListBox.AddString(m_ShipInfo.GetAt(i)->GetShipClass(), color[m_ShipInfo.GetAt(i)->GetRace()]);
	if (m_ShipInfo.GetSize() > m_iClick)
	{
		m_ListBox.SetCurSel(m_iClick);
	}
	else if (m_ShipInfo.GetSize() > 0)
	{
		m_iClick--;
		m_ListBox.SetCurSel(m_iClick);
	}
	this->OnSelchangeShipClassList();
}

void CShipEditorDlg::OnShowstats() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (m_ListBox.GetCurSel() != -1)
	{

		m_ShowStatsDlg.SetShowStatsShip(m_ShipInfo.GetAt(m_ListBox.GetCurSel()));
		m_ShowStatsDlg.DoModal();
	}
}
