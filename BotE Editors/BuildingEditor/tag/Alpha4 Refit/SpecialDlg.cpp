// SpecialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BuildingEditor.h"
#include "SpecialDlg.h"


// CSpecialDlg dialog

IMPLEMENT_DYNAMIC(CSpecialDlg, CDialog)

CSpecialDlg::CSpecialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecialDlg::IDD, pParent)
{
	m_iAddedTradeRoutes = 0;
	m_bBarracks = FALSE;
	m_iBarrackSpeed = 0;
	m_iBuildingBuildSpeed = 0;
	m_iGroundDefend = 0;
	m_iGroundDefendBoni = 0;
	m_iHitPoints = 0;
	m_iIncomeOnTradeRoutes = 0;
	m_iResistance = 0;
	m_iScanPower = 0;
	m_iScanPowerBoni = 0;
	m_iScanRange = 0;
	m_iScanRangeBoni = 0;
	m_iShieldPower = 0;
	m_iShieldPowerBoni = 0;
	m_iShipDefend = 0;
	m_iShipDefendBoni = 0;
	m_iShipRecycling = 0;
	m_iShipTraining = 0;
	m_bShipyard = FALSE;
	m_iShipyardSpeed = 0;
	m_iTroopbuildSpeed = 0;
	m_iTroopTraining = 0;
	m_iUpgradeBuildSpeed = 0;
	m_iShipbuildSpeed = 0;
}

CSpecialDlg::~CSpecialDlg()
{
}

void CSpecialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUILDABLESHIPTYPES, m_BuildableShipTypesComboBox);
	DDX_Text(pDX, IDC_ADDEDTRADEROUTES, m_iAddedTradeRoutes);
	DDX_Check(pDX, IDC_BARRACKS, m_bBarracks);
	DDX_Text(pDX, IDC_BARRACKSBUILDSPEED, m_iBarrackSpeed);
	DDX_Text(pDX, IDC_BUILDINGSPEEDBONI, m_iBuildingBuildSpeed);
	DDX_Text(pDX, IDC_GROUNDDEFENCE, m_iGroundDefend);
	DDV_MinMaxUInt(pDX, m_iGroundDefend, 0, 64000);
	DDX_Text(pDX, IDC_GROUNDDEFENCEBONI, m_iGroundDefendBoni);
	DDX_Text(pDX, IDC_HITPOINTS, m_iHitPoints);
	DDV_MinMaxUInt(pDX, m_iHitPoints, 0, 64000);
	DDX_Text(pDX, IDC_INCOMEONTRADEROUTES, m_iIncomeOnTradeRoutes);
	DDX_Text(pDX, IDC_RESISTANCE, m_iResistance);
	DDX_Text(pDX, IDC_SCANPOWER, m_iScanPower);
	DDV_MinMaxUInt(pDX, m_iScanPower, 0, 64000);
	DDX_Text(pDX, IDC_SCANPOWERBONI, m_iScanPowerBoni);
	DDX_Text(pDX, IDC_SCANRANGE, m_iScanRange);
	DDX_Text(pDX, IDC_SCANRANGEBONI, m_iScanRangeBoni);
	DDX_Text(pDX, IDC_SHIELDPOWER, m_iShieldPower);
	DDV_MinMaxUInt(pDX, m_iShieldPower, 0, 64000);
	DDX_Text(pDX, IDC_SHIELDPOWERBONI, m_iShieldPowerBoni);
	DDX_Text(pDX, IDC_SHIPDEFENCE, m_iShipDefend);
	DDV_MinMaxUInt(pDX, m_iShipDefend, 0, 64000);
	DDX_Text(pDX, IDC_SHIPDEFENCEBONI, m_iShipDefendBoni);
	DDX_Text(pDX, IDC_SHIPRECYCLING, m_iShipRecycling);
	DDX_Text(pDX, IDC_SHIPTRAINING, m_iShipTraining);
	DDX_Check(pDX, IDC_SHIPYARD, m_bShipyard);
	DDX_Text(pDX, IDC_SHIPYARDBUILDSPEED, m_iShipyardSpeed);
	DDX_Text(pDX, IDC_TROOPSPEEDBONI, m_iTroopbuildSpeed);
	DDX_Text(pDX, IDC_TROOPTRAINING, m_iTroopTraining);
	DDX_Text(pDX, IDC_UPGRADESPEEDBONI, m_iUpgradeBuildSpeed);
	DDX_Text(pDX, IDC_SHIPSPEEDBONI, m_iShipbuildSpeed);
}


BEGIN_MESSAGE_MAP(CSpecialDlg, CDialog)
END_MESSAGE_MAP()


// CSpecialDlg message handlers
void CSpecialDlg::DataToDialog(CBuildingInfo* building, int language)
{
	// sonstige Informationen
	m_bShipyard = building->GetShipYard();
	m_BuildableShipTypesComboBox.SetCurSel(building->GetMaxBuildableShipSize());
	m_iShipyardSpeed = building->GetShipYardSpeed();
	m_bBarracks = building->GetBarrack();
	m_iBarrackSpeed = building->GetBarrackSpeed();
	m_iHitPoints = building->GetHitPoints();
	m_iShieldPower = building->GetShieldPower();
	m_iShieldPowerBoni = building->GetShieldPowerBoni();
	m_iShipDefend = building->GetShipDefend();
	m_iShipDefendBoni = building->GetShipDefendBoni();
	m_iGroundDefend = building->GetGroundDefend();
	m_iGroundDefendBoni = building->GetGroundDefendBoni();
	m_iScanPower = building->GetScanPower();
	m_iScanPowerBoni = building->GetScanPowerBoni();
	m_iScanRange = building->GetScanRange();
	m_iScanRangeBoni = building->GetScanRangeBoni();
	m_iShipTraining = building->GetShipTraining();
	m_iTroopTraining = building->GetTroopTraining();
	m_iResistance = building->GetResistance();
	m_iAddedTradeRoutes = building->GetAddedTradeRoutes();
	m_iIncomeOnTradeRoutes = building->GetIncomeOnTradeRoutes();
	m_iShipRecycling = building->GetShipRecycling();
	m_iBuildingBuildSpeed = building->GetBuildingBuildSpeed();
	m_iUpgradeBuildSpeed = building->GetUpdateBuildSpeed();
	m_iShipbuildSpeed = building->GetShipBuildSpeed();
	m_iTroopbuildSpeed = building->GetTroopBuildSpeed();

	// Daten in den Dialog bringen
	this->UpdateData(FALSE);
}

void CSpecialDlg::DialogToData(CBuildingInfo* building, int language)
{
	this->UpdateData(TRUE);

	building->SetShipYard(m_bShipyard);
	building->SetBuildableShipTypes(m_BuildableShipTypesComboBox.GetCurSel());
	building->SetShipYardSpeed(m_iShipyardSpeed);
	building->SetBarrack(m_bBarracks);
	building->SetBarrackSpeed(m_iBarrackSpeed);
	building->SetHitpoints(m_iHitPoints);
	building->SetShieldPower(m_iShieldPower);
	building->SetShieldPowerBoni(m_iShieldPowerBoni);
	building->SetShipDefend(m_iShipDefend);
	building->SetShipDefendBoni(m_iShipDefendBoni);
	building->SetGroundDefend(m_iGroundDefend);
	building->SetGroundDefendBoni(m_iGroundDefendBoni);
	building->SetScanPower(m_iScanPower);
	building->SetScanPowerBoni(m_iScanPowerBoni);
	building->SetScanRange(m_iScanRange);
	building->SetScanRangeBoni(m_iScanRangeBoni);
	building->SetShipTraining(m_iShipTraining);
	building->SetTroopTraining(m_iTroopTraining);
	building->SetResistance(m_iResistance);
	building->SetAddedTradeRoutes(m_iAddedTradeRoutes);
	building->SetIncomeOnTradeRoutes(m_iIncomeOnTradeRoutes);
	building->SetShipRecycling(m_iShipRecycling);
	building->SetBuildingBuildSpeed(m_iBuildingBuildSpeed);
	building->SetUpdateBuildSpeed(m_iUpgradeBuildSpeed);
	building->SetShipBuildSpeed(m_iShipbuildSpeed);
	building->SetTroopBuildSpeed(m_iTroopbuildSpeed);	
}