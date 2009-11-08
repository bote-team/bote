#pragma once
#include "BuildingInfo.h"

// CSpecialDlg dialog

class CSpecialDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpecialDlg)

public:
	CSpecialDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpecialDlg();
	// Memberfunktionen
	virtual void DialogToData(CBuildingInfo* building, int language);
	virtual void DataToDialog(CBuildingInfo* building, int language);

// Dialog Data
	enum { IDD = IDD_SPECIALDLG };
	CComboBox	m_BuildableShipTypesComboBox;
	BYTE	m_iAddedTradeRoutes;
	BOOL	m_bBarracks;
	short	m_iBarrackSpeed;
	short	m_iBuildingBuildSpeed;
	UINT	m_iGroundDefend;
	short	m_iGroundDefendBoni;
	UINT	m_iHitPoints;
	short	m_iIncomeOnTradeRoutes;
	short	m_iResistance;
	UINT	m_iScanPower;
	short	m_iScanPowerBoni;
	BYTE	m_iScanRange;
	short	m_iScanRangeBoni;
	UINT	m_iShieldPower;
	short	m_iShieldPowerBoni;
	UINT	m_iShipDefend;
	short	m_iShipDefendBoni;
	short	m_iShipRecycling;
	short	m_iShipTraining;
	BOOL	m_bShipyard;
	short	m_iShipyardSpeed;
	short	m_iTroopbuildSpeed;
	short	m_iTroopTraining;
	short	m_iUpgradeBuildSpeed;
	short	m_iShipbuildSpeed;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
