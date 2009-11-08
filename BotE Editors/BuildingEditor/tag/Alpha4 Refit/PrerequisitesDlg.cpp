// PrerequisitesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BuildingEditor.h"
#include "PrerequisitesDlg.h"


// CPrerequisitesDlg dialog

IMPLEMENT_DYNAMIC(CPrerequisitesDlg, CDialog)

CPrerequisitesDlg::CPrerequisitesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrerequisitesDlg::IDD, pParent)
{
	m_bType_A = FALSE;
	m_bType_B = FALSE;
	m_bType_C = FALSE;
	m_bType_E = FALSE;
	m_bType_F = FALSE;
	m_bType_G = FALSE;
	m_bType_H = FALSE;
	m_bType_I = FALSE;
	m_bType_J = FALSE;
	m_bType_K = FALSE;
	m_bType_L = FALSE;
	m_bType_M = FALSE;
	m_bType_N = FALSE;
	m_bType_O = FALSE;
	m_bType_P = FALSE;
	m_bType_Q = FALSE;
	m_bType_R = FALSE;
	m_bType_S = FALSE;
	m_bType_T = FALSE;
	m_bType_Y = FALSE;
	m_iMaxSysID = 0;
	m_iMaxSysNumber = 0;
	m_iMaxEmpireID = 0;
	m_iMaxEmpireNumber = 0;
	m_iMinEmpireID = 0;
	m_iMinEmpireNumber = 0;
	m_iMinSysID = 0;
	m_iMinSysNumber = 0;
	m_bOnlyConqueredSys = FALSE;
	m_bOnlyHomeSys = FALSE;
	m_bOnlyMinorSys = FALSE;
	m_bOnlyOwnColony = FALSE;
	m_bReallyOnlyRace = FALSE;
	m_iMinHabitants = 0;
	m_strOnlySystemWithName = _T("");
	m_iNeededBioTech = 0;
	m_iNeededCompTech = 0;
	m_iNeededConstructionTech = 0;
	m_iNeededEnergyTech = 0;
	m_iNeededPropulsionTech = 0;
	m_iNeededWeaponTech = 0;
	m_iNeededCrystals = 0;
	m_iNeededDeuterium = 0;
	m_iNeededDilithium = 0;
	m_iNeededDuranium = 0;
	m_iNeededEnergy = 0;
	m_NeededIP = 0;
	m_iNeededIridium = 0;
	m_iNeededTitan = 0;
}

CPrerequisitesDlg::~CPrerequisitesDlg()
{
}

void CPrerequisitesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_TYPE_A, m_bType_A);
	DDX_Check(pDX, IDC_TYPE_B, m_bType_B);
	DDX_Check(pDX, IDC_TYPE_C, m_bType_C);
	DDX_Check(pDX, IDC_TYPE_E, m_bType_E);
	DDX_Check(pDX, IDC_TYPE_F, m_bType_F);
	DDX_Check(pDX, IDC_TYPE_G, m_bType_G);
	DDX_Check(pDX, IDC_TYPE_H, m_bType_H);
	DDX_Check(pDX, IDC_TYPE_I, m_bType_I);
	DDX_Check(pDX, IDC_TYPE_J, m_bType_J);
	DDX_Check(pDX, IDC_TYPE_K, m_bType_K);
	DDX_Check(pDX, IDC_TYPE_L, m_bType_L);
	DDX_Check(pDX, IDC_TYPE_M, m_bType_M);
	DDX_Check(pDX, IDC_TYPE_N, m_bType_N);
	DDX_Check(pDX, IDC_TYPE_O, m_bType_O);
	DDX_Check(pDX, IDC_TYPE_P, m_bType_P);
	DDX_Check(pDX, IDC_TYPE_Q, m_bType_Q);
	DDX_Check(pDX, IDC_TYPE_R, m_bType_R);
	DDX_Check(pDX, IDC_TYPE_S, m_bType_S);
	DDX_Check(pDX, IDC_TYPE_T, m_bType_T);
	DDX_Check(pDX, IDC_TYPE_Y, m_bType_Y);
	DDX_Text(pDX, IDC_MAXSYSID, m_iMaxSysID);
	DDV_MinMaxUInt(pDX, m_iMaxSysID, 0, 64000);
	DDX_Text(pDX, IDC_MAXSYSNUMBER, m_iMaxSysNumber);
	DDV_MinMaxByte(pDX, m_iMaxSysNumber, 0, 127);
	DDX_Text(pDX, IDC_MAXEMPIREID, m_iMaxEmpireID);
	DDV_MinMaxUInt(pDX, m_iMaxEmpireID, 0, 64000);
	DDX_Text(pDX, IDC_MAXEMPIRENUMBER, m_iMaxEmpireNumber);
	DDV_MinMaxByte(pDX, m_iMaxEmpireNumber, 0, 127);
	DDX_Text(pDX, IDC_MINEMPIREID, m_iMinEmpireID);
	DDV_MinMaxUInt(pDX, m_iMinEmpireID, 0, 64000);
	DDX_Text(pDX, IDC_MINEMPIRENUMBER, m_iMinEmpireNumber);
	DDV_MinMaxUInt(pDX, m_iMinEmpireNumber, 0, 127);
	DDX_Text(pDX, IDC_MINSYSID, m_iMinSysID);
	DDV_MinMaxUInt(pDX, m_iMinSysID, 0, 64000);
	DDX_Text(pDX, IDC_MINSYSNUMBER, m_iMinSysNumber);
	DDV_MinMaxUInt(pDX, m_iMinSysNumber, 0, 127);
	DDX_Check(pDX, IDC_ONLYCONQUEREDSYS, m_bOnlyConqueredSys);
	DDX_Check(pDX, IDC_ONLYHOMESYSTEM, m_bOnlyHomeSys);
	DDX_Check(pDX, IDC_ONLYMINORRACESYS, m_bOnlyMinorSys);
	DDX_Check(pDX, IDC_ONLYOWNCOLONY, m_bOnlyOwnColony);
	DDX_Check(pDX, IDC_REALLYONLYRACE, m_bReallyOnlyRace);
	DDX_Text(pDX, IDC_MINHABITANTS, m_iMinHabitants);
	DDV_MinMaxUInt(pDX, m_iMinHabitants, 0, 1000);
	DDX_Text(pDX, IDC_ONLYINSYSWITHNAME, m_strOnlySystemWithName);
	DDX_Text(pDX, IDC_BIOTECH, m_iNeededBioTech);
	DDV_MinMaxByte(pDX, m_iNeededBioTech, 0, 127);
	DDX_Text(pDX, IDC_COMPTECH, m_iNeededCompTech);
	DDV_MinMaxByte(pDX, m_iNeededCompTech, 0, 127);
	DDX_Text(pDX, IDC_CONSTRUCTIONTECH, m_iNeededConstructionTech);
	DDV_MinMaxByte(pDX, m_iNeededConstructionTech, 0, 127);
	DDX_Text(pDX, IDC_ENERGYTECH, m_iNeededEnergyTech);
	DDV_MinMaxByte(pDX, m_iNeededEnergyTech, 0, 127);
	DDX_Text(pDX, IDC_PROPULSIONTECH, m_iNeededPropulsionTech);
	DDV_MinMaxByte(pDX, m_iNeededPropulsionTech, 0, 127);
	DDX_Text(pDX, IDC_WEAPONTECH, m_iNeededWeaponTech);
	DDV_MinMaxByte(pDX, m_iNeededWeaponTech, 0, 127);
	DDX_Text(pDX, IDC_NEEDEDCRYSTALS, m_iNeededCrystals);
	DDV_MinMaxUInt(pDX, m_iNeededCrystals, 0, 64000);
	DDX_Text(pDX, IDC_NEEDEDDEUTERIUM, m_iNeededDeuterium);
	DDV_MinMaxUInt(pDX, m_iNeededDeuterium, 0, 64000);
	DDX_Text(pDX, IDC_NEEDEDDILITHIUM, m_iNeededDilithium);
	DDV_MinMaxUInt(pDX, m_iNeededDilithium, 0, 64000);
	DDX_Text(pDX, IDC_NEEDEDDURANIUM, m_iNeededDuranium);
	DDV_MinMaxUInt(pDX, m_iNeededDuranium, 0, 64000);
	DDX_Text(pDX, IDC_NEEDEDENERGY, m_iNeededEnergy);
	DDV_MinMaxUInt(pDX, m_iNeededEnergy, 0, 64000);
	DDX_Text(pDX, IDC_NEEDEDIP, m_NeededIP);
	DDV_MinMaxUInt(pDX, m_NeededIP, 1, 64000);
	DDX_Text(pDX, IDC_NEEDEDIRIDIUM, m_iNeededIridium);
	DDV_MinMaxUInt(pDX, m_iNeededIridium, 0, 64000);
	DDX_Text(pDX, IDC_NEEDEDTITAN, m_iNeededTitan);
	DDV_MinMaxUInt(pDX, m_iNeededTitan, 0, 64000);
}


BEGIN_MESSAGE_MAP(CPrerequisitesDlg, CDialog)
END_MESSAGE_MAP()


// CPrerequisitesDlg message handlers
void CPrerequisitesDlg::DataToDialog(CBuildingInfo* building, int language)
{	
	// Planetentypen, welche vorausgesetzt werden
	m_bType_A = building->GetPlanetTypes(A);
	m_bType_B = building->GetPlanetTypes(B);
	m_bType_C = building->GetPlanetTypes(C);
	m_bType_E = building->GetPlanetTypes(E);
	m_bType_F = building->GetPlanetTypes(F);
	m_bType_G = building->GetPlanetTypes(G);
	m_bType_H = building->GetPlanetTypes(H);
	m_bType_I = building->GetPlanetTypes(I);
	m_bType_J = building->GetPlanetTypes(J);
	m_bType_K = building->GetPlanetTypes(K);
	m_bType_L = building->GetPlanetTypes(L);
	m_bType_M = building->GetPlanetTypes(M);
	m_bType_N = building->GetPlanetTypes(N);
	m_bType_O = building->GetPlanetTypes(O);
	m_bType_P = building->GetPlanetTypes(P);
	m_bType_Q = building->GetPlanetTypes(Q);
	m_bType_R = building->GetPlanetTypes(R);
	m_bType_S = building->GetPlanetTypes(S);
	m_bType_T = building->GetPlanetTypes(T);
	m_bType_Y = building->GetPlanetTypes(Y);	// 25
	
	// Min und Max der Gebäude pro System und Imperium
	m_iMaxSysNumber = building->GetMaxInSystem().Number;
	m_iMaxSysID = building->GetMaxInSystem().RunningNumber;
	m_iMaxEmpireNumber = building->GetMaxInEmpire().Number;
	m_iMaxEmpireID = building->GetMaxInEmpire().RunningNumber;
	m_iMinSysNumber = building->GetMinInSystem().Number;
	m_iMinSysID = building->GetMinInSystem().RunningNumber;
	m_iMinEmpireNumber = building->GetMinInEmpire().Number;
	m_iMinEmpireID = building->GetMinInEmpire().RunningNumber;
	
	// weitere Voraussetzungen
	m_bOnlyHomeSys = building->GetOnlyHomePlanet();
	m_bOnlyOwnColony = building->GetOnlyOwnColony();
	m_bOnlyMinorSys = building->GetOnlyMinorRace();
	m_bOnlyConqueredSys = building->GetOnlyTakenSystem();
	m_bReallyOnlyRace = building->GetOnlyRace();
	m_strOnlySystemWithName = building->GetOnlyInSystemWithName();
	m_iMinHabitants = building->GetMinHabitants();

	// benötigte Forschungsstufen
	m_iNeededBioTech = building->GetBioTech();
	m_iNeededEnergyTech = building->GetEnergyTech();
	m_iNeededCompTech = building->GetCompTech();
	m_iNeededPropulsionTech = building->GetPropulsionTech();
	m_iNeededConstructionTech = building->GetConstructionTech();
	m_iNeededWeaponTech = building->GetWeaponTech();

	// benötigte Ressourcen
	m_NeededIP = building->GetNeededIndustry();
	m_iNeededEnergy = building->GetNeededEnergy();
	m_iNeededTitan = building->GetNeededTitan();
	m_iNeededDeuterium = building->GetNeededDeuterium();
	m_iNeededDuranium = building->GetNeededDuranium();
	m_iNeededCrystals = building->GetNeededCrystal();
	m_iNeededIridium = building->GetNeededIridium();
	m_iNeededDilithium = building->GetNeededDilithium();	// 54

	// Daten in den Dialog bringen
	this->UpdateData(FALSE);
}

void CPrerequisitesDlg::DialogToData(CBuildingInfo* building, int language)
{
	this->UpdateData(TRUE);

	// Wenn wir bei Max und Min bei System oder Empire einen Wert eingegeben haben, dann darf die ID
	// nicht NULL sein.
	if (m_iMaxEmpireNumber != 0 && m_iMaxEmpireID == 0)
	{
		CString s;
		s.Format("Error ... the ID of max per empire is NULL!\n\nAll your changes for the building: %s were canceled", building->GetBuildingName(language));
		AfxMessageBox(s);
		return;
	}
	else if (m_iMinEmpireNumber != 0 && m_iMinEmpireID == 0)
	{
		CString s;
		s.Format("Error ... the ID of min per empire is NULL!\n\nAll your changes for the building: %s were canceled",building->GetBuildingName(language));
		AfxMessageBox(s);
		return;
	}
	else if (m_iMaxSysNumber != 0 && m_iMaxSysID == 0)
	{
		CString s;
		s.Format("Error ... the ID of max per system is NULL!\n\nAll your changes for the building: %s were canceled",building->GetBuildingName(language));
		AfxMessageBox(s);
		return;
	}
	else if (m_iMinSysNumber != 0 && m_iMinSysID == 0)
	{
		CString s;
		s.Format("Error ... the ID of min per system is NULL!\n\nAll your changes for the building: %s were canceled",building->GetBuildingName(language));
		AfxMessageBox(s);
		return;
	}				
	// Wenn ich eine ID für Min und Max pro Imperium oder System eingegeben habe, so muß diese Idee auch in der
	// Liste vorkommen
	/*if ((int)m_iMaxEmpireID > m_BuildingInfo.GetSize() || (int)m_iMaxSysID > m_BuildingInfo.GetSize()
		|| (int)m_iMinEmpireID > m_BuildingInfo.GetSize() || (int)m_iMinSysID > m_BuildingInfo.GetSize())
	{
		AfxMessageBox("You chose an ID which isn't in the buildinglist");
		return;
	}
	*/
	int newline = -1;
	newline = m_strOnlySystemWithName.Find("\n");
	if (newline != -1)
	{
		AfxMessageBox("There is a newline in the only in system-name!");
		return;
	}
	building->SetMaxInSystem(m_iMaxSysNumber,m_iMaxSysID);
	building->SetMaxInEmpire(m_iMaxEmpireNumber,m_iMaxEmpireID);
	building->SetOnlyHomePlanet(m_bOnlyHomeSys);
	building->SetOnlyOwnColony(m_bOnlyOwnColony);
	building->SetOnlyMinorRace(m_bOnlyMinorSys);
	building->SetOnlyTakenSystem(m_bOnlyConqueredSys);
	building->SetOnlyInSystemWithName(m_strOnlySystemWithName);
	building->SetMinHabitants(m_iMinHabitants);
	building->SetMinInSystem(m_iMinSysNumber,m_iMinSysID);
	building->SetMinInEmpire(m_iMinEmpireNumber,m_iMinEmpireID);
	building->SetOnlyRace(m_bReallyOnlyRace);
	building->SetPlanetTypes(A,m_bType_A);
	building->SetPlanetTypes(B,m_bType_B);
	building->SetPlanetTypes(C,m_bType_C);
	building->SetPlanetTypes(E,m_bType_E);
	building->SetPlanetTypes(F,m_bType_F);
	building->SetPlanetTypes(G,m_bType_G);
	building->SetPlanetTypes(H,m_bType_H);
	building->SetPlanetTypes(I,m_bType_I);
	building->SetPlanetTypes(J,m_bType_J);
	building->SetPlanetTypes(K,m_bType_K);
	building->SetPlanetTypes(L,m_bType_L);
	building->SetPlanetTypes(M,m_bType_M);
	building->SetPlanetTypes(N,m_bType_N);
	building->SetPlanetTypes(O,m_bType_O);
	building->SetPlanetTypes(P,m_bType_P);
	building->SetPlanetTypes(Q,m_bType_Q);
	building->SetPlanetTypes(R,m_bType_R);
	building->SetPlanetTypes(S,m_bType_S);
	building->SetPlanetTypes(T,m_bType_T);
	building->SetPlanetTypes(Y,m_bType_Y);
	building->SetBioTech(m_iNeededBioTech);
	building->SetEnergyTech(m_iNeededEnergyTech);
	building->SetCompTech(m_iNeededCompTech);
	building->SetPropulsionTech(m_iNeededPropulsionTech);
	building->SetConstructionTech(m_iNeededConstructionTech);
	building->SetWeaponTech(m_iNeededWeaponTech);
	building->SetNeededIndustry(m_NeededIP);
	building->SetNeededEnergy(m_iNeededEnergy);
	building->SetNeededTitan(m_iNeededTitan);
	building->SetNeededDeuterium(m_iNeededDeuterium);
	building->SetNeededDuranium(m_iNeededDuranium);
	building->SetNeededCrystal(m_iNeededCrystals);
	building->SetNeededIridium(m_iNeededIridium);
	building->SetNeededDilithium(m_iNeededDilithium);
}