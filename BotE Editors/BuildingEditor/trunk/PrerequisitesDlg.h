#pragma once
#include "BuildingInfo.h"

// CPrerequisitesDlg dialog

class CPrerequisitesDlg : public CDialog
{
	DECLARE_DYNAMIC(CPrerequisitesDlg)

public:
	CPrerequisitesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPrerequisitesDlg();
	
	// Memberfunktionen
	virtual void DialogToData(CBuildingInfo* building, int language);
	virtual void DataToDialog(CBuildingInfo* building, int language);

// Dialog Data
	enum { IDD = IDD_PREREQUISITESDLG };
	BOOL	m_bType_A;
	BOOL	m_bType_B;
	BOOL	m_bType_C;
	BOOL	m_bType_E;
	BOOL	m_bType_F;
	BOOL	m_bType_G;
	BOOL	m_bType_H;
	BOOL	m_bType_I;
	BOOL	m_bType_J;
	BOOL	m_bType_K;
	BOOL	m_bType_L;
	BOOL	m_bType_M;
	BOOL	m_bType_N;
	BOOL	m_bType_O;
	BOOL	m_bType_P;
	BOOL	m_bType_Q;
	BOOL	m_bType_R;
	BOOL	m_bType_S;
	BOOL	m_bType_T;
	BOOL	m_bType_Y;
	UINT	m_iMaxSysID;
	UINT	m_iMaxSysNumber;
	UINT	m_iMaxEmpireID;
	UINT	m_iMaxEmpireNumber;
	UINT	m_iMinEmpireID;
	UINT	m_iMinEmpireNumber;
	UINT	m_iMinSysID;
	UINT	m_iMinSysNumber;
	UINT	m_iNeededSystems;
	BOOL	m_bOnlyConqueredSys;
	BOOL	m_bOnlyHomeSys;
	BOOL	m_bOnlyMinorSys;
	BOOL	m_bOnlyOwnColony;
	BOOL	m_bReallyOnlyRace;
	UINT	m_iMinHabitants;
	CString	m_strOnlySystemWithName;
	BYTE	m_iNeededBioTech;
	BYTE	m_iNeededCompTech;
	BYTE	m_iNeededConstructionTech;
	BYTE	m_iNeededEnergyTech;
	BYTE	m_iNeededPropulsionTech;
	BYTE	m_iNeededWeaponTech;
	UINT	m_iNeededCrystals;
	UINT	m_iNeededDeuterium;
	UINT	m_iNeededDilithium;
	UINT	m_iNeededDuranium;
	UINT	m_iNeededEnergy;
	UINT	m_NeededIP;
	UINT	m_iNeededIridium;
	UINT	m_iNeededTitan;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
