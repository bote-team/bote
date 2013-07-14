// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BuildingEditor.h"
#include "InfoDlg.h"
#include "BuildingEditorDlg.h"


// CInfoDlg dialog

IMPLEMENT_DYNAMIC(CInfoDlg, CDialog)

CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)	
{
	m_strName = _T("");
	m_strDescription = _T("");
	m_strGraphicfile = _T("");
	m_bAllwayOnline = FALSE;
	m_iCarEqu = 0;
	m_iDomEqu = 0;
	m_iFedEqu = 0;
	m_iFerEqu = 0;
	m_iKliEqu = 0;
	m_iRomEqu = 0;
	m_strEquiName1 = _T("");
	m_strEquiName2 = _T("");
	m_strEquiName3 = _T("");
	m_strEquiName4 = _T("");
	m_strEquiName5 = _T("");
	m_strEquiName6 = _T("");
	m_bNeedWorker = FALSE;
	m_bNeverReady = FALSE;
	m_iPredecessorID = 0;
	m_bUpgradeable = FALSE;
}

CInfoDlg::~CInfoDlg()
{
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RACE, m_RaceComboBox);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_NAME2, m_strName2);
	DDX_Text(pDX, IDC_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_DESCRIPTION2, m_strDescription2);
	DDX_Text(pDX, IDC_GRAPHICFILE, m_strGraphicfile);
	DDX_Check(pDX, IDC_ALLWAYS_ONLINE, m_bAllwayOnline);
	DDX_Text(pDX, IDC_CAR_EQU, m_iCarEqu);
	DDV_MinMaxUInt(pDX, m_iCarEqu, 0, 9999);
	DDX_Text(pDX, IDC_DOM_EQU, m_iDomEqu);
	DDV_MinMaxUInt(pDX, m_iDomEqu, 0, 9999);
	DDX_Text(pDX, IDC_FED_EQU, m_iFedEqu);
	DDV_MinMaxUInt(pDX, m_iFedEqu, 0, 9999);
	DDX_Text(pDX, IDC_FER_EQU, m_iFerEqu);
	DDV_MinMaxUInt(pDX, m_iFerEqu, 0, 9999);
	DDX_Text(pDX, IDC_KLI_EQU, m_iKliEqu);
	DDV_MinMaxUInt(pDX, m_iKliEqu, 0, 9999);
	DDX_Text(pDX, IDC_ROM_EQU, m_iRomEqu);
	DDV_MinMaxUInt(pDX, m_iRomEqu, 0, 9999);
	DDX_Check(pDX, IDC_NEEDWORKER, m_bNeedWorker);
	DDX_Check(pDX, IDC_NEVERREADY, m_bNeverReady);
	DDX_Text(pDX, IDC_PREDECESSORID, m_iPredecessorID);
	DDV_MinMaxUInt(pDX, m_iPredecessorID, 0, 9999);
	DDX_Check(pDX, IDC_UPGRADEABLE, m_bUpgradeable);
	DDX_Text(pDX, IDC_EQUI1NAME, m_strEquiName1);
	DDX_Text(pDX, IDC_EQUI2NAME2, m_strEquiName2);
	DDX_Text(pDX, IDC_EQUI3NAME3, m_strEquiName3);
	DDX_Text(pDX, IDC_EQUI4NAME4, m_strEquiName4);
	DDX_Text(pDX, IDC_EQUI5NAME5, m_strEquiName5);
	DDX_Text(pDX, IDC_EQUI6NAME6, m_strEquiName6);
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_RACE, OnSelchangeRace)
END_MESSAGE_MAP()


// CInfoDlg message handlers
void CInfoDlg::DataToDialog(CBuildingInfo* building, int language)
{	
	// Allgemeine Informationen
	m_RaceComboBox.SetCurSel(building->GetOwnerOfBuilding());
	m_strName = building->GetBuildingName(language);
	m_strName2 = building->GetBuildingName(!language);
	m_strDescription = building->GetBuildingDescription(language);
	m_strDescription2 = building->GetBuildingDescription(!language);
	m_strGraphicfile = building->GetGraphikFileName();

	// programmiertechnische Werte
	m_bUpgradeable = building->GetUpgradeable();
	m_iPredecessorID = building->GetPredecessorID();
	m_bAllwayOnline = building->GetIsBuildingOnline();
	m_bNeedWorker = building->GetWorker();
	m_bNeverReady = building->GetNeverReady();
	m_iFedEqu = building->GetEquivalent(HUMAN);
	m_iFerEqu = building->GetEquivalent(FERENGI);
	m_iKliEqu = building->GetEquivalent(KLINGON);
	m_iRomEqu = building->GetEquivalent(ROMULAN);
	m_iCarEqu = building->GetEquivalent(CARDASSIAN);
	m_iDomEqu = building->GetEquivalent(DOMINION);

	CBuildingEditorDlg* buildDlg = ((CBuildingEditorApp*)AfxGetApp())->GetMainDlg();
	m_strEquiName1 = _T("");
	m_strEquiName2 = _T("");
	m_strEquiName3 = _T("");
	m_strEquiName4 = _T("");
	m_strEquiName5 = _T("");
	m_strEquiName6 = _T("");
	try
	{
		if (building->GetEquivalent(HUMAN) != NULL && building->GetOwnerOfBuilding() != HUMAN)
			m_strEquiName1 = buildDlg->GetBuildings()->GetAt(building->GetEquivalent(HUMAN) - 1).GetBuildingName(language);
		if (building->GetEquivalent(FERENGI) != NULL && building->GetOwnerOfBuilding() != FERENGI)
			m_strEquiName2 = buildDlg->GetBuildings()->GetAt(building->GetEquivalent(FERENGI) - 1).GetBuildingName(language);
		if (building->GetEquivalent(KLINGON) != NULL && building->GetOwnerOfBuilding() != KLINGON)
			m_strEquiName3 = buildDlg->GetBuildings()->GetAt(building->GetEquivalent(KLINGON) - 1).GetBuildingName(language);
		if (building->GetEquivalent(ROMULAN) != NULL && building->GetOwnerOfBuilding() != ROMULAN)
			m_strEquiName4 = buildDlg->GetBuildings()->GetAt(building->GetEquivalent(ROMULAN) - 1).GetBuildingName(language);
		if (building->GetEquivalent(CARDASSIAN) != NULL && building->GetOwnerOfBuilding() != CARDASSIAN)
			m_strEquiName5 = buildDlg->GetBuildings()->GetAt(building->GetEquivalent(CARDASSIAN) - 1).GetBuildingName(language);
		if (building->GetEquivalent(DOMINION) != NULL && building->GetOwnerOfBuilding() != DOMINION)
			m_strEquiName6 = buildDlg->GetBuildings()->GetAt(building->GetEquivalent(DOMINION) - 1).GetBuildingName(language);
		

	}
	catch (...)
	{
		AfxMessageBox("This building has a wrong equivalnt ID. You have to fix it!");
	}

	// Daten in den Dialog bringen
	this->UpdateData(FALSE);
}

void CInfoDlg::DialogToData(CBuildingInfo* building, int language)
{
	this->UpdateData(TRUE);

	if (m_iPredecessorID == building->GetRunningNumber())
	{
		CString s;
		s.Format("Error ... the predecessor ID is the same like the ID of the building!\n\nAll your changes for the building: %s (ID: %d) were canceled", building->GetBuildingName(language), building->GetRunningNumber());
		AfxMessageBox(s);
		return;
	}
	
	int newline = -1;
	newline = m_strName.Find("\n");
	if (newline != -1)
	{
		AfxMessageBox("There is a newline in the name of the building!");
		return;
	}
	newline = -1;
	newline = m_strDescription.Find("\n");
	if (newline != -1)
	{
		AfxMessageBox("There is a newline in the buildingdescription!");
		return;
	}
	newline = -1;
	newline = m_strGraphicfile.Find("\n");
	if (newline != -1)
	{
		AfxMessageBox("There is a newline in the graphicfilename!");
		return;
	};		
			
	BOOLEAN EXITSAVE = TRUE;  // something should be saved

	building->SetOwnerOfBuilding(m_RaceComboBox.GetCurSel());
	building->SetBuildingName(m_strName, language);
	//building->SetBuildingName(m_strName2, language+1);
	building->SetBuildingDescription(m_strDescription, language);
	//building->SetBuildingDescription(m_strDescription2, language+1);
	building->SetGraphikFileName(m_strGraphicfile);
	building->SetUpgradeable(m_bUpgradeable);
	building->SetPredecessor(m_iPredecessorID);
	building->SetIsBuildingOnline(m_bAllwayOnline);
	building->SetWorker(m_bNeedWorker);
	building->SetNeverReady(m_bNeverReady);
	building->SetEquivalent(NOBODY,0);		// niemand-index immer auf NULL setzen
	building->SetEquivalent(HUMAN,m_iFedEqu);
	building->SetEquivalent(FERENGI,m_iFerEqu);
	building->SetEquivalent(KLINGON,m_iKliEqu);
	building->SetEquivalent(ROMULAN,m_iRomEqu);
	building->SetEquivalent(CARDASSIAN,m_iCarEqu);
	building->SetEquivalent(DOMINION,m_iDomEqu);
}

void CInfoDlg::OnSelchangeRace() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CPrerequisitesDlg* dlg = (CPrerequisitesDlg*)(((CBuildingEditorApp*)AfxGetApp())->GetMainDlg()->m_dynTabCtrl.GetTab(1));

	if (m_RaceComboBox.GetCurSel() == 0)
	{
		CBuildingEditorDlg* buildDlg = ((CBuildingEditorApp*)AfxGetApp())->GetMainDlg();
		CBuildingInfo* building = buildDlg->GetClickedBuilding();
		building->SetMaxInEmpire(0,0);
		building->SetMinInEmpire(0,0);
		
		dlg->m_iMaxEmpireID = 0;
		dlg->m_iMaxEmpireNumber = 0;
		dlg->m_iMinEmpireID = 0;
		dlg->m_iMinEmpireNumber = 0;

		dlg->GetDlgItem(IDC_MAXEMPIRENUMBER)->EnableWindow(FALSE);
		dlg->GetDlgItem(IDC_MAXEMPIREID)->EnableWindow(FALSE);
		dlg->GetDlgItem(IDC_MINEMPIRENUMBER)->EnableWindow(FALSE);
		dlg->GetDlgItem(IDC_MINEMPIREID)->EnableWindow(FALSE);
		
		UpdateData(FALSE);
		
	}
	else
	{		
		dlg->GetDlgItem(IDC_MAXEMPIRENUMBER)->EnableWindow(TRUE);
		dlg->GetDlgItem(IDC_MAXEMPIREID)->EnableWindow(TRUE);
		dlg->GetDlgItem(IDC_MINEMPIRENUMBER)->EnableWindow(TRUE);
		dlg->GetDlgItem(IDC_MINEMPIREID)->EnableWindow(TRUE);		
	}
}
