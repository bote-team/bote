// ProductionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BuildingEditor.h"
#include "ProductionDlg.h"


// CProductionDlg dialog

IMPLEMENT_DYNAMIC(CProductionDlg, CDialog)

CProductionDlg::CProductionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProductionDlg::IDD, pParent)
{
	m_iCrystalProd = 0;
	m_iDeuteriumProd = 0;
	m_iDilithiumProd = 0;
	m_iDuraniumProd = 0;
	m_iEnergyProd = 0;
	m_iFoodProd = 0;
	m_iFPProd = 0;
	m_iIPProd = 0;
	m_iIridiumProd = 0;
	m_iLatinumProd = 0;
	m_iSPProd = 0;
	m_iTitanProd = 0;
	m_iAllResBoni = 0;
	m_iCrystalBoni = 0;
	m_iDeuteriumBoni = 0;
	m_iDilithiumBoni = 0;
	m_iDuraniumBoni = 0;
	m_iEnergyBoni = 0;
	m_iFoodBoni = 0;
	m_iFPBoni = 0;
	m_iIPBoni = 0;
	m_iIridiumBoni = 0;
	m_iLatinumBoni = 0;
	m_iSPBoni = 0;
	m_iTitanBoni = 0;
	m_iBioTechBoni = 0;
	m_iCompTechBoni = 0;
	m_iConstructionTechBoni = 0;
	m_iEnergyTechBoni = 0;
	m_iPropulsionTechBoni = 0;
	m_iWeaponTechBoni = 0;
	m_iEconomySabotageBoni = 0;
	m_iEconomySpyBoni = 0;
	m_iInnerSecurityBoni = 0;
	m_iMilitarySabotageBoni = 0;
	m_iMilitarySpyBoni = 0;
	m_iResearchSabotageBoni = 0;
	m_iResearchSpyBoni = 0;
	m_iMoralSys = 0;
	m_iMoralEmpirewide = 0;
}

CProductionDlg::~CProductionDlg()
{
}

void CProductionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_CRYSTALPROD, m_iCrystalProd);
	DDV_MinMaxUInt(pDX, m_iCrystalProd, 0, 64000);
	DDX_Text(pDX, IDC_DEUTERIUMPROD, m_iDeuteriumProd);
	DDV_MinMaxUInt(pDX, m_iDeuteriumProd, 0, 64000);
	DDX_Text(pDX, IDC_DILITHIUMPROD, m_iDilithiumProd);
	DDV_MinMaxUInt(pDX, m_iDilithiumProd, 0, 64000);
	DDX_Text(pDX, IDC_DURANIUMPROD, m_iDuraniumProd);
	DDV_MinMaxUInt(pDX, m_iDuraniumProd, 0, 64000);
	DDX_Text(pDX, IDC_ENERGYPROD, m_iEnergyProd);
	DDV_MinMaxUInt(pDX, m_iEnergyProd, 0, 64000);
	DDX_Text(pDX, IDC_FOODPROD, m_iFoodProd);
	DDX_Text(pDX, IDC_FPPROD, m_iFPProd);
	DDV_MinMaxUInt(pDX, m_iFPProd, 0, 64000);
	DDX_Text(pDX, IDC_IPPROD, m_iIPProd);
	DDV_MinMaxUInt(pDX, m_iIPProd, 0, 64000);
	DDX_Text(pDX, IDC_IRIDIUMPROD, m_iIridiumProd);
	DDV_MinMaxUInt(pDX, m_iIridiumProd, 0, 64000);
	DDX_Text(pDX, IDC_LATINUMPROD, m_iLatinumProd);
	DDX_Text(pDX, IDC_SPPROD, m_iSPProd);
	DDV_MinMaxUInt(pDX, m_iSPProd, 0, 64000);
	DDX_Text(pDX, IDC_TITANPROD, m_iTitanProd);
	DDV_MinMaxUInt(pDX, m_iTitanProd, 0, 64000);
	DDX_Text(pDX, IDC_ALLRESBONI, m_iAllResBoni);
	DDX_Text(pDX, IDC_CRYSTALBONI, m_iCrystalBoni);
	DDX_Text(pDX, IDC_DEUTERIUMBONI, m_iDeuteriumBoni);
	DDX_Text(pDX, IDC_DILITHIUMBONI, m_iDilithiumBoni);
	DDX_Text(pDX, IDC_DURANIUMBONI, m_iDuraniumBoni);
	DDX_Text(pDX, IDC_ENERGYBONI, m_iEnergyBoni);
	DDX_Text(pDX, IDC_FOODBONI, m_iFoodBoni);
	DDX_Text(pDX, IDC_FPBONI, m_iFPBoni);
	DDX_Text(pDX, IDC_IPBONI, m_iIPBoni);
	DDX_Text(pDX, IDC_IRIDIUMBONI, m_iIridiumBoni);
	DDX_Text(pDX, IDC_LATINUMBONI, m_iLatinumBoni);
	DDX_Text(pDX, IDC_SPBONI, m_iSPBoni);
	DDX_Text(pDX, IDC_TITANBONI, m_iTitanBoni);
	DDX_Text(pDX, IDC_BIOTECHBONI, m_iBioTechBoni);
	DDX_Text(pDX, IDC_COMPTECHBONI, m_iCompTechBoni);
	DDX_Text(pDX, IDC_CONSTRUCTIONTECHBONI, m_iConstructionTechBoni);
	DDX_Text(pDX, IDC_ENERGYTECHBONI, m_iEnergyTechBoni);
	DDX_Text(pDX, IDC_PROPULSIONTECHBONI, m_iPropulsionTechBoni);
	DDX_Text(pDX, IDC_WEAPONTECHBONI, m_iWeaponTechBoni);
	DDX_Text(pDX, IDC_ECONOMYSABOTAGEBONI, m_iEconomySabotageBoni);
	DDX_Text(pDX, IDC_ECONOMYSPYBONI, m_iEconomySpyBoni);
	DDX_Text(pDX, IDC_INNERSECURITYBONI, m_iInnerSecurityBoni);
	DDX_Text(pDX, IDC_MILITARYSABOTAGEBONI, m_iMilitarySabotageBoni);
	DDX_Text(pDX, IDC_MILITARYSPYBONI, m_iMilitarySpyBoni);
	DDX_Text(pDX, IDC_RESEARCHSABOTAGEBONI, m_iResearchSabotageBoni);
	DDX_Text(pDX, IDC_RESEARCHSPYBONI, m_iResearchSpyBoni);
	DDX_Text(pDX, IDC_MORALINSYS, m_iMoralSys);
	DDX_Text(pDX, IDC_MORALEMPIREWIDE, m_iMoralEmpirewide);
}


BEGIN_MESSAGE_MAP(CProductionDlg, CDialog)
END_MESSAGE_MAP()


// CProductionDlg message handlers
void CProductionDlg::DataToDialog(CBuildingInfo* building, int language)
{	
	// ab hier die einzelnen Produktionswerte
	m_iFoodProd = building->GetFoodProd();
	m_iIPProd = building->GetIPProd();
	m_iEnergyProd = building->GetEnergyProd();
	m_iSPProd = building->GetSPProd();
	m_iFPProd = building->GetFPProd();
	m_iTitanProd = building->GetTitanProd();
	m_iDeuteriumProd = building->GetDeuteriumProd();
	m_iDuraniumProd = building->GetDuraniumProd();
	m_iCrystalProd = building->GetCrystalProd();
	m_iIridiumProd = building->GetIridiumProd();
	m_iDilithiumProd = building->GetDilithiumProd();
	m_iLatinumProd = building->GetLatinum();
	m_iMoralSys = building->GetMoralProd();
	m_iMoralEmpirewide = building->GetMoralProdEmpire();

	// ab hier die Boni/Mali auf die einzelnen Produktionswerte
	m_iFoodBoni = building->GetFoodBoni();
	m_iIPBoni = building->GetIndustryBoni();
	m_iEnergyBoni = building->GetEnergyBoni();
	m_iSPBoni = building->GetSecurityBoni();
	m_iFPBoni = building->GetResearchBoni();
	m_iTitanBoni = building->GetTitanBoni();
	m_iDeuteriumBoni = building->GetDeuteriumBoni();
	m_iDuraniumBoni = building->GetDuraniumBoni();
	m_iCrystalBoni = building->GetCrystalBoni();
	m_iIridiumBoni = building->GetIridiumBoni();
	m_iDilithiumBoni = building->GetDilithiumBoni();
	m_iLatinumBoni = building->GetLatinumBoni();
	m_iAllResBoni = building->GetAllRessourcesBoni();	// 81

	// ab hier Boni auf die einzelnen Techs
	m_iBioTechBoni = building->GetBioTechBoni();
	m_iEnergyTechBoni = building->GetEnergyTechBoni();
	m_iCompTechBoni = building->GetCompTechBoni();
	m_iPropulsionTechBoni = building->GetPropulsionTechBoni();
	m_iConstructionTechBoni = building->GetConstructionTechBoni();
	m_iWeaponTechBoni = building->GetWeaponTechBoni();

	// ab hier Boni auf das ganze Geheimdienstzeugs
	m_iInnerSecurityBoni = building->GetInnerSecurityBoni();
	m_iEconomySpyBoni = building->GetEconomySpyBoni();
	m_iEconomySabotageBoni = building->GetEconomySabotageBoni();
	m_iResearchSpyBoni = building->GetResearchSpyBoni();
	m_iResearchSabotageBoni = building->GetResearchSabotageBoni();
	m_iMilitarySpyBoni = building->GetMilitarySpyBoni();
	m_iMilitarySabotageBoni = building->GetMilitarySabotageBoni();

	// Daten in den Dialog bringen
	this->UpdateData(FALSE);
}

void CProductionDlg::DialogToData(CBuildingInfo* building, int language)
{
	this->UpdateData(TRUE);

	building->SetFoodProd(m_iFoodProd);
	building->SetIPProd(m_iIPProd);
	building->SetEnergyProd(m_iEnergyProd);
	building->SetSPProd(m_iSPProd);
	building->SetFPProd(m_iFPProd);
	building->SetTitanProd(m_iTitanProd);
	building->SetDeuteriumProd(m_iDeuteriumProd);
	building->SetDuraniumProd(m_iDuraniumProd);
	building->SetCrystalProd(m_iCrystalProd);
	building->SetIridiumProd(m_iIridiumProd);
	building->SetDilithiumProd(m_iDilithiumProd);
	building->SetLatinumProd(m_iLatinumProd);
	building->SetMoralProd(m_iMoralSys);
	building->SetMoralProdEmpire(m_iMoralEmpirewide);
	building->SetFoodBoni(m_iFoodBoni);
	building->SetIndustryBoni(m_iIPBoni);
	building->SetEnergyBoni(m_iEnergyBoni);
	building->SetSecurityBoni(m_iSPBoni);
	building->SetResearchBoni(m_iFPBoni);
	building->SetTitanBoni(m_iTitanBoni);
	building->SetDeuteriumBoni(m_iDeuteriumBoni);
	building->SetDuraniumBoni(m_iDuraniumBoni);
	building->SetCrystalBoni(m_iCrystalBoni);
	building->SetIridiumBoni(m_iIridiumBoni);
	building->SetDilithiumBoni(m_iDilithiumBoni);
	building->SetAllRessourceBoni(m_iAllResBoni);
	building->SetLatinumBoni(m_iLatinumBoni);
	building->SetBioTechBoni(m_iBioTechBoni);
	building->SetEnergyTechBoni(m_iEnergyTechBoni);
	building->SetCompTechBoni(m_iCompTechBoni);
	building->SetPropulsionTechBoni(m_iPropulsionTechBoni);
	building->SetConstructionTechBoni(m_iConstructionTechBoni);
	building->SetWeaponTechBoni(m_iWeaponTechBoni);
	building->SetInnerSecurityBoni(m_iInnerSecurityBoni);
	building->SetEconomySpyBoni(m_iEconomySpyBoni);
	building->SetEconomySabotageBoni(m_iEconomySabotageBoni);
	building->SetResearchSpyBoni(m_iResearchSpyBoni);
	building->SetResearchSabotageBoni(m_iResearchSabotageBoni);
	building->SetMilitarySpyBoni(m_iMilitarySpyBoni);
	building->SetMilitarySabotageBoni(m_iMilitarySabotageBoni);
}