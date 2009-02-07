#include "stdafx.h"
#include "SystemProd.h"
short CSystemProd::m_iMoralProdEmpireWide[7] = {0,0,0,0,0,0,0};

IMPLEMENT_SERIAL (CSystemProd, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CSystemProd::CSystemProd(void)
{
	Reset();
}

CSystemProd::~CSystemProd(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CSystemProd::CSystemProd(const CSystemProd &rhs)
{
	m_iFoodProd = rhs.m_iFoodProd;
	m_iMaxFoodProd = rhs.m_iMaxFoodProd;
	m_iIndustryProd = rhs.m_iIndustryProd;
	m_iEnergyProd = rhs.m_iEnergyProd;
	m_iMaxEnergyProd = rhs.m_iMaxEnergyProd;
	m_iSecurityProd = rhs.m_iSecurityProd;
	m_iResearchProd = rhs.m_iResearchProd;
	m_iTitanProd = rhs.m_iTitanProd;
	m_iDeuteriumProd = rhs.m_iDeuteriumProd;
	m_iDuraniumProd = rhs.m_iDuraniumProd;
	m_iCrystalProd = rhs.m_iCrystalProd;
	m_iIridiumProd = rhs.m_iIridiumProd;
	m_iDilithiumProd = rhs.m_iDilithiumProd;
	m_iLatinumProd = rhs.m_iLatinumProd;
	m_iMoralProd = rhs.m_iMoralProd;
	// Forschungstechboni
	m_iBioTechBoni = rhs.m_iBioTechBoni;
	m_iEnergyTechBoni = rhs.m_iEnergyTechBoni;
	m_iCompTechBoni = rhs.m_iCompTechBoni;
	m_iPropulsionTechBoni = rhs.m_iPropulsionTechBoni;
	m_iConstructionTechBoni = rhs.m_iConstructionTechBoni;
	m_iWeaponTechBoni = rhs.m_iWeaponTechBoni;
	// Geheimdienstboni
	m_iInnerSecurityBoni = rhs.m_iInnerSecurityBoni;
	m_iEconomySpyBoni = rhs.m_iEconomySpyBoni;
	m_iEconomySabotageBoni = rhs.m_iEconomySabotageBoni;
	m_iResearchSpyBoni = rhs.m_iResearchSpyBoni;
	m_iResearchSabotageBoni = rhs.m_iResearchSabotageBoni;
	m_iMilitarySpyBoni = rhs.m_iMilitarySpyBoni;
	m_iMilitarySabotageBoni = rhs.m_iMilitarySabotageBoni;
	// sonstige Informationen
	m_bShipYard = rhs.m_bShipYard;
	m_iBuildableShipSizes = rhs.m_iBuildableShipSizes;
	m_iShipYardEfficiency = rhs.m_iShipYardEfficiency;
	m_bBarrack = rhs.m_bBarrack;
	m_iBarracksEfficiency = rhs.m_iBarracksEfficiency;
	m_iShieldPower = rhs.m_iShieldPower;
	m_iShieldPowerBoni = rhs.m_iShieldPowerBoni;
	m_iShipDefend = rhs.m_iShipDefend;
	m_iShipDefendBoni = rhs.m_iShipDefendBoni;
	m_iGroundDefend = rhs.m_iGroundDefend;
	m_iGroundDefendBoni = rhs.m_iGroundDefendBoni;
	m_iScanPower = rhs.m_iScanPower;
	m_iScanPowerBoni = rhs.m_iScanPowerBoni;
	m_iScanRange = rhs.m_iScanRange;
	m_iScanRangeBoni = rhs.m_iScanRangeBoni;
	m_iShipTraining = rhs.m_iShipTraining;
	m_iTroopTraining = rhs.m_iTroopTraining;
	m_iResistance = rhs.m_iResistance;
	m_iAddedTradeRouts = rhs.m_iAddedTradeRouts;
	m_iIncomeOnTradeRoutes = rhs.m_iIncomeOnTradeRoutes;
	m_iShipRecycling = rhs.m_iShipRecycling;
	m_iBuildingBuildSpeed = rhs.m_iBuildingBuildSpeed;
	m_iUpdateBuildSpeed = rhs.m_iUpdateBuildSpeed;
	m_iShipBuildSpeed = rhs.m_iShipBuildSpeed;
	m_iTroopBuildSpeed = rhs.m_iTroopBuildSpeed;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CSystemProd & CSystemProd::operator=(const CSystemProd & rhs)
{
	if (this == &rhs)
		return *this;
	m_iFoodProd = rhs.m_iFoodProd;
	m_iMaxFoodProd = rhs.m_iMaxFoodProd;
	m_iIndustryProd = rhs.m_iIndustryProd;
	m_iEnergyProd = rhs.m_iEnergyProd;
	m_iMaxEnergyProd = rhs.m_iMaxEnergyProd;
	m_iSecurityProd = rhs.m_iSecurityProd;
	m_iResearchProd = rhs.m_iResearchProd;
	m_iTitanProd = rhs.m_iTitanProd;
	m_iDeuteriumProd = rhs.m_iDeuteriumProd;
	m_iDuraniumProd = rhs.m_iDuraniumProd;
	m_iCrystalProd = rhs.m_iCrystalProd;
	m_iIridiumProd = rhs.m_iIridiumProd;
	m_iDilithiumProd = rhs.m_iDilithiumProd;
	m_iLatinumProd = rhs.m_iLatinumProd;
	m_iMoralProd = rhs.m_iMoralProd;
	// Forschungstechboni
	m_iBioTechBoni = rhs.m_iBioTechBoni;
	m_iEnergyTechBoni = rhs.m_iEnergyTechBoni;
	m_iCompTechBoni = rhs.m_iCompTechBoni;
	m_iPropulsionTechBoni = rhs.m_iPropulsionTechBoni;
	m_iConstructionTechBoni = rhs.m_iConstructionTechBoni;
	m_iWeaponTechBoni = rhs.m_iWeaponTechBoni;
	// Geheimdienstboni
	m_iInnerSecurityBoni = rhs.m_iInnerSecurityBoni;
	m_iEconomySpyBoni = rhs.m_iEconomySpyBoni;
	m_iEconomySabotageBoni = rhs.m_iEconomySabotageBoni;
	m_iResearchSpyBoni = rhs.m_iResearchSpyBoni;
	m_iResearchSabotageBoni = rhs.m_iResearchSabotageBoni;
	m_iMilitarySpyBoni = rhs.m_iMilitarySpyBoni;
	m_iMilitarySabotageBoni = rhs.m_iMilitarySabotageBoni;
	// sonstige Informationen
	m_bShipYard = rhs.m_bShipYard;
	m_iBuildableShipSizes = rhs.m_iBuildableShipSizes;
	m_iShipYardEfficiency = rhs.m_iShipYardEfficiency;
	m_bBarrack = rhs.m_bBarrack;
	m_iBarracksEfficiency = rhs.m_iBarracksEfficiency;
	m_iShieldPower = rhs.m_iShieldPower;
	m_iShieldPowerBoni = rhs.m_iShieldPowerBoni;
	m_iShipDefend = rhs.m_iShipDefend;
	m_iShipDefendBoni = rhs.m_iShipDefendBoni;
	m_iGroundDefend = rhs.m_iGroundDefend;
	m_iGroundDefendBoni = rhs.m_iGroundDefendBoni;
	m_iScanPower = rhs.m_iScanPower;
	m_iScanPowerBoni = rhs.m_iScanPowerBoni;
	m_iScanRange = rhs.m_iScanRange;
	m_iScanRangeBoni = rhs.m_iScanRangeBoni;
	m_iShipTraining = rhs.m_iShipTraining;
	m_iTroopTraining = rhs.m_iTroopTraining;
	m_iResistance = rhs.m_iResistance;
	m_iAddedTradeRouts = rhs.m_iAddedTradeRouts;
	m_iIncomeOnTradeRoutes = rhs.m_iIncomeOnTradeRoutes;
	m_iShipRecycling = rhs.m_iShipRecycling;
	m_iBuildingBuildSpeed = rhs.m_iBuildingBuildSpeed;
	m_iUpdateBuildSpeed = rhs.m_iUpdateBuildSpeed;
	m_iShipBuildSpeed = rhs.m_iShipBuildSpeed;
	m_iTroopBuildSpeed = rhs.m_iTroopBuildSpeed;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CSystemProd::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iFoodProd;
		ar << m_iMaxFoodProd;	
		ar << m_iIndustryProd;
		ar << m_iEnergyProd;
		ar << m_iMaxEnergyProd;
		ar << m_iSecurityProd;
		ar << m_iResearchProd;
		ar << m_iTitanProd;
		ar << m_iDeuteriumProd;
		ar << m_iDuraniumProd;
		ar << m_iCrystalProd;
		ar << m_iIridiumProd;
		ar << m_iDilithiumProd;
		ar << m_iLatinumProd;
		ar << m_iMoralProd;
		ar << m_iBioTechBoni;
		ar << m_iEnergyTechBoni;
		ar << m_iCompTechBoni;
		ar << m_iPropulsionTechBoni;
		ar << m_iConstructionTechBoni;
		ar << m_iWeaponTechBoni;
		ar << m_iInnerSecurityBoni;
		ar << m_iEconomySpyBoni;
		ar << m_iEconomySabotageBoni;
		ar << m_iResearchSpyBoni;
		ar << m_iResearchSabotageBoni;
		ar << m_iMilitarySpyBoni;
		ar << m_iMilitarySabotageBoni;
		ar << m_bShipYard;
		ar << m_iBuildableShipSizes;
		ar << m_iShipYardEfficiency;
		ar << m_bBarrack;
		ar << m_iBarracksEfficiency;
		ar << m_iShieldPower;
		ar << m_iShieldPowerBoni;
		ar << m_iShipDefend;
		ar << m_iShipDefendBoni;
		ar << m_iGroundDefend;
		ar << m_iGroundDefendBoni;
		ar << m_iScanPower;
		ar << m_iScanPowerBoni;
		ar << m_iScanRange;
		ar << m_iScanRangeBoni;
		ar << m_iShipTraining;
		ar << m_iTroopTraining;
		ar << m_iResistance;
		ar << m_iAddedTradeRouts;
		ar << m_iIncomeOnTradeRoutes;
		ar << m_iShipRecycling;
		ar << m_iBuildingBuildSpeed;
		ar << m_iUpdateBuildSpeed;
		ar << m_iShipBuildSpeed;
		ar << m_iTroopBuildSpeed;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iFoodProd;
		ar >> m_iMaxFoodProd;	
		ar >> m_iIndustryProd;
		ar >> m_iEnergyProd;
		ar >> m_iMaxEnergyProd;
		ar >> m_iSecurityProd;
		ar >> m_iResearchProd;
		ar >> m_iTitanProd;
		ar >> m_iDeuteriumProd;
		ar >> m_iDuraniumProd;
		ar >> m_iCrystalProd;
		ar >> m_iIridiumProd;
		ar >> m_iDilithiumProd;
		ar >> m_iLatinumProd;
		ar >> m_iMoralProd;
		ar >> m_iBioTechBoni;
		ar >> m_iEnergyTechBoni;
		ar >> m_iCompTechBoni;
		ar >> m_iPropulsionTechBoni;
		ar >> m_iConstructionTechBoni;
		ar >> m_iWeaponTechBoni;
		ar >> m_iInnerSecurityBoni;
		ar >> m_iEconomySpyBoni;
		ar >> m_iEconomySabotageBoni;
		ar >> m_iResearchSpyBoni;
		ar >> m_iResearchSabotageBoni;
		ar >> m_iMilitarySpyBoni;
		ar >> m_iMilitarySabotageBoni;
		ar >> m_bShipYard;
		ar >> m_iBuildableShipSizes;
		ar >> m_iShipYardEfficiency;
		ar >> m_bBarrack;
		ar >> m_iBarracksEfficiency;
		ar >> m_iShieldPower;
		ar >> m_iShieldPowerBoni;
		ar >> m_iShipDefend;
		ar >> m_iShipDefendBoni;
		ar >> m_iGroundDefend;
		ar >> m_iGroundDefendBoni;
		ar >> m_iScanPower;
		ar >> m_iScanPowerBoni;
		ar >> m_iScanRange;
		ar >> m_iScanRangeBoni;
		ar >> m_iShipTraining;
		ar >> m_iTroopTraining;
		ar >> m_iResistance;
		ar >> m_iAddedTradeRouts;
		ar >> m_iIncomeOnTradeRoutes;
		ar >> m_iShipRecycling;
		ar >> m_iBuildingBuildSpeed;
		ar >> m_iUpdateBuildSpeed;
		ar >> m_iShipBuildSpeed;
		ar >> m_iTroopBuildSpeed;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
// Funktion berechnet die neuen Produktionen im System die sich durch das übergebene Gebäude ergeben
void CSystemProd::CalculateProduction(CBuilding* building)
{
	m_iFoodProd			+= building->GetFoodProd();
	m_iIndustryProd		+= building->GetIPProd();
	m_iEnergyProd		+= building->GetEnergyProd();
	m_iSecurityProd		+= building->GetSPProd();
	m_iResearchProd		+= building->GetFPProd();
	m_iTitanProd		+= building->GetTitanProd();
	m_iDeuteriumProd	+= building->GetDeuteriumProd();
	m_iDuraniumProd		+= building->GetDuraniumProd();
	m_iCrystalProd		+= building->GetCrystalProd();
	m_iIridiumProd		+= building->GetIridiumProd();
	m_iDilithiumProd	+= building->GetDilithiumProd();
	m_iLatinumProd		+= building->GetLatinum();
	m_iMoralProd		+= building->GetMoralProd();
	// Forschungstechboni
	m_iBioTechBoni		+= building->GetBioTechBoni();
	m_iEnergyTechBoni	+= building->GetEnergyTechBoni();
	m_iCompTechBoni		+= building->GetCompTechBoni();
	m_iPropulsionTechBoni	+= building->GetPropulsionTechBoni();
	m_iConstructionTechBoni += building->GetConstructionTechBoni();
	m_iWeaponTechBoni	+= building->GetWeaponTechBoni();
	// Geheimdienstboni
	m_iInnerSecurityBoni	+= building->GetInnerSecurityBoni();
	m_iEconomySpyBoni		+= building->GetEconomySpyBoni();
	m_iEconomySabotageBoni	+= building->GetEconomySabotageBoni();
	m_iResearchSpyBoni		+= building->GetResearchSpyBoni();
	m_iResearchSabotageBoni += building->GetResearchSabotageBoni();
	m_iMilitarySpyBoni		+= building->GetMilitarySpyBoni();
	m_iMilitarySabotageBoni += building->GetMilitarySabotageBoni();
	// sonstige Informationen
	m_bShipYard			|= building->GetShipYard();
	if (building->GetMaxBuildableShipSize() > m_iBuildableShipSizes)
		m_iBuildableShipSizes = building->GetMaxBuildableShipSize();
	if (building->GetShipYardSpeed() > m_iShipYardEfficiency)
		m_iShipYardEfficiency = building->GetShipYardSpeed();
	m_bBarrack			|= building->GetBarrack();
	if (building->GetBarrackSpeed() > m_iBarracksEfficiency)
		m_iBarracksEfficiency = building->GetBarrackSpeed();
	m_iShieldPower		+= building->GetShieldPower();
	m_iShieldPowerBoni	+= building->GetShieldPowerBoni();
	m_iShipDefend		+= building->GetShipDefend();
	m_iShipDefendBoni	+= building->GetShipDefendBoni();
	m_iGroundDefend		+= building->GetGroundDefend();
	m_iGroundDefendBoni += building->GetGroundDefendBoni();
	if (building->GetScanPower() > m_iScanPower)
		m_iScanPower		= building->GetScanPower();
	m_iScanPowerBoni	+= building->GetScanPowerBoni();
	if (building->GetScanRange() > m_iScanRange)
		m_iScanRange		= building->GetScanRange();
	m_iScanRangeBoni	+= building->GetScanRangeBoni();
	m_iShipTraining		+= building->GetShipTraining();
	m_iTroopTraining	+= building->GetTroopTraining();
	m_iResistance		+= building->GetResistance();
	m_iAddedTradeRouts	+= building->GetAddedTradeRoutes();
	m_iIncomeOnTradeRoutes  += building->GetIncomeOnTradeRoutes();
	m_iShipRecycling	+= building->GetShipRecycling();
	m_iBuildingBuildSpeed += building->GetBuildingBuildSpeed();
	m_iUpdateBuildSpeed	+= building->GetUpdateBuildSpeed();
	m_iShipBuildSpeed	+= building->GetShipBuildSpeed();
	m_iTroopBuildSpeed	+= building->GetTroopBuildSpeed();
}

void CSystemProd::IncludeSystemMoral(short moral)
{
	m_iFoodProd		= (int)(m_iFoodProd * moral/100);
	m_iMaxFoodProd	= m_iFoodProd;
	m_iIndustryProd = (int)(m_iIndustryProd * moral/100);	// Energie bis jetzt ausgeschlossen
	m_iSecurityProd = (int)(m_iSecurityProd * moral/100);
	m_iResearchProd = (int)(m_iResearchProd * moral/100);
	m_iTitanProd	= (int)(m_iTitanProd * moral/100);
	m_iDeuteriumProd= (int)(m_iDeuteriumProd * moral/100);
	m_iDuraniumProd = (int)(m_iDuraniumProd * moral/100);
	m_iCrystalProd	= (int)(m_iCrystalProd * moral/100);
	m_iIridiumProd	= (int)(m_iIridiumProd * moral/100);
	m_iLatinumProd	= (short)(m_iLatinumProd * moral/100);
}

int CSystemProd::GetResourceProd(BYTE res) const
{
	switch (res)
	{
	case TITAN:
		return m_iTitanProd;
	case DEUTERIUM:
		return m_iDeuteriumProd;
	case DURANIUM:
		return m_iDuraniumProd;
	case CRYSTAL:
		return m_iCrystalProd;
	case IRIDIUM:
		return m_iIridiumProd;
	case DILITHIUM:
		return m_iDilithiumProd;
	default:
		return 0;
	}
}


// Resetfunktion für die Klasse CSystemProd
void CSystemProd::Reset()
{
	m_iFoodProd = 10;				// Nahrungsproduktion in dem System
	m_iMaxFoodProd = 0;				// Nahrungsproduktion in dem System ohne Bevölkerungsabzug
	m_iIndustryProd = 5;			// Industrieproduktion in dem System
	m_iEnergyProd = 0;				// Energyproduktion in dem System
	m_iMaxEnergyProd = 0;			// Energieproduktion in dem System ohne Abzug des Energieverbrauchs
	m_iSecurityProd = 0;			// Geheimdienstproduktion in dem System
	m_iResearchProd = 0;			// Forschungspunkteproduktion in dem System
	m_iTitanProd = 0;				// Titanproduktion in dem System
	m_iDeuteriumProd = 0;			// Deuteriumproduktion in dem System
	m_iDuraniumProd = 0;			// Duraniumproduktion in dem System
	m_iCrystalProd = 0;				// Crystalproduktion in dem System
	m_iIridiumProd = 0;				// Iridiumproduktion in dem System
	m_iDilithiumProd = 0;			// Dilithiumproduktion
	m_iLatinumProd = 0;				// Latinumproduktion in dem System, abhängig von der Bevölkerung
	m_iMoralProd = 0;				// Moralproduktion auf dem System
	// Forschungstechboni
	m_iBioTechBoni = 0;
	m_iEnergyTechBoni = 0;
	m_iCompTechBoni = 0;
	m_iPropulsionTechBoni = 0;
	m_iConstructionTechBoni = 0;
	m_iWeaponTechBoni = 0;
	// Geheimdienstboni
	m_iInnerSecurityBoni = 0;
	m_iEconomySpyBoni = 0;
	m_iEconomySabotageBoni = 0;
	m_iResearchSpyBoni = 0;
	m_iResearchSabotageBoni = 0;
	m_iMilitarySpyBoni = 0;
	m_iMilitarySabotageBoni = 0;
	// sonstige Informationen
	m_bShipYard = FALSE;
	m_iBuildableShipSizes = TRANSPORTER;
	m_iShipYardEfficiency = 0;
	m_bBarrack = FALSE;
	m_iBarracksEfficiency = 0;
	m_iShieldPower = 0;
	m_iShieldPowerBoni = 0;
	m_iShipDefend = 0;
	m_iShipDefendBoni = 0;
	m_iGroundDefend = 0;
	m_iGroundDefendBoni = 0;
	m_iScanPower = 0;
	m_iScanPowerBoni = 0;
	m_iScanRange = 0;
	m_iScanRangeBoni = 0;
	m_iShipTraining = 0;
	m_iTroopTraining = 0;
	m_iResistance = 0;
	m_iAddedTradeRouts = 0;
	m_iIncomeOnTradeRoutes = 0;
	m_iShipRecycling = 0;
	m_iBuildingBuildSpeed = 0;
	m_iUpdateBuildSpeed = 0;
	m_iShipBuildSpeed = 0;
	m_iTroopBuildSpeed = 0;
}