#include "stdafx.h"
#include "SystemProd.h"

#include <cassert>

// statische Variablen initialisieren
std::map<CString, short> CSystemProd::m_iMoralProdEmpireWide;

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

CSystemProd::CSystemProd(const CSystemProd &other) :
	m_iFoodProd(other.m_iFoodProd),
	m_iMaxFoodProd(other.m_iMaxFoodProd),
	m_iIndustryProd(other.m_iIndustryProd),
	m_iPotentialIndustryProd(other.m_iPotentialIndustryProd),
	m_iEnergyProd(other.m_iEnergyProd),
	m_iMaxEnergyProd(other.m_iMaxEnergyProd),
	m_iPotentialEnergyProd(other.m_iPotentialEnergyProd),
	m_iSecurityProd(other.m_iSecurityProd),
	m_iResearchProd(other.m_iResearchProd),
	m_iTitanProd(other.m_iTitanProd),
	m_iDeuteriumProd(other.m_iDeuteriumProd),
	m_iDuraniumProd(other.m_iDuraniumProd),
	m_iCrystalProd(other.m_iCrystalProd),
	m_iIridiumProd(other.m_iIridiumProd),
	m_iDeritiumProd(other.m_iDeritiumProd),
	m_iCreditsProd(other.m_iCreditsProd),
	m_iMoralProd(other.m_iMoralProd),
	m_iBioTechBoni(other.m_iBioTechBoni),
	m_iEnergyTechBoni(other.m_iEnergyTechBoni),
	m_iCompTechBoni(other.m_iCompTechBoni),
	m_iPropulsionTechBoni(other.m_iPropulsionTechBoni),
	m_iConstructionTechBoni(other.m_iConstructionTechBoni),
	m_iWeaponTechBoni(other.m_iWeaponTechBoni),
	m_iInnerSecurityBoni(other.m_iInnerSecurityBoni),
	m_iEconomySpyBoni(other.m_iEconomySpyBoni),
	m_iEconomySabotageBoni(other.m_iEconomySabotageBoni),
	m_iResearchSpyBoni(other.m_iResearchSpyBoni),
	m_iResearchSabotageBoni(other.m_iResearchSabotageBoni),
	m_iMilitarySpyBoni(other.m_iMilitarySpyBoni),
	m_iMilitarySabotageBoni(other.m_iMilitarySabotageBoni),
	m_bShipYard(other.m_bShipYard),
	m_iBuildableShipSizes(other.m_iBuildableShipSizes),
	m_iShipYardEfficiency(other.m_iShipYardEfficiency),
	m_bBarrack(other.m_bBarrack),
	m_iBarracksEfficiency(other.m_iBarracksEfficiency),
	m_iShieldPower(other.m_iShieldPower),
	m_iShieldPowerBoni(other.m_iShieldPowerBoni),
	m_iShipDefend(other.m_iShipDefend),
	m_iShipDefendBoni(other.m_iShipDefendBoni),
	m_iGroundDefend(other.m_iGroundDefend),
	m_iGroundDefendBoni(other.m_iGroundDefendBoni),
	m_iScanPower(other.m_iScanPower),
	m_iScanPowerBoni(other.m_iScanPowerBoni),
	m_iScanRange(other.m_iScanRange),
	m_iScanRangeBoni(other.m_iScanRangeBoni),
	m_iShipTraining(other.m_iShipTraining),
	m_iTroopTraining(other.m_iTroopTraining),
	m_iResistance(other.m_iResistance),
	m_iAddedTradeRouts(other.m_iAddedTradeRouts),
	m_iIncomeOnTradeRoutes(other.m_iIncomeOnTradeRoutes),
	m_iShipRecycling(other.m_iShipRecycling),
	m_iBuildingBuildSpeed(other.m_iBuildingBuildSpeed),
	m_iUpdateBuildSpeed(other.m_iUpdateBuildSpeed),
	m_iShipBuildSpeed(other.m_iShipBuildSpeed),
	m_iTroopBuildSpeed(other.m_iTroopBuildSpeed)
{
	const unsigned size = sizeof(m_bResourceDistributor) / sizeof(m_bResourceDistributor[0]);
	for(unsigned i = 0; i < size; ++i)
		m_bResourceDistributor[i] = other.m_bResourceDistributor[i];
}

CSystemProd& CSystemProd::operator=(const CSystemProd& other)
{
	m_iFoodProd = other.m_iFoodProd;
	m_iMaxFoodProd = other.m_iMaxFoodProd;
	m_iIndustryProd = other.m_iIndustryProd;
	m_iPotentialIndustryProd = other.m_iPotentialIndustryProd;
	m_iEnergyProd = other.m_iEnergyProd;
	m_iMaxEnergyProd = other.m_iMaxEnergyProd;
	m_iPotentialEnergyProd = other.m_iPotentialEnergyProd;
	m_iSecurityProd = other.m_iSecurityProd;
	m_iResearchProd = other.m_iResearchProd;
	m_iTitanProd = other.m_iTitanProd;
	m_iDeuteriumProd = other.m_iDeuteriumProd;
	m_iDuraniumProd = other.m_iDuraniumProd;
	m_iCrystalProd = other.m_iCrystalProd;
	m_iIridiumProd = other.m_iIridiumProd;
	m_iDeritiumProd = other.m_iDeritiumProd;
	m_iCreditsProd = other.m_iCreditsProd;
	m_iMoralProd = other.m_iMoralProd;
	m_iBioTechBoni = other.m_iBioTechBoni;
	m_iEnergyTechBoni = other.m_iEnergyTechBoni;
	m_iCompTechBoni = other.m_iCompTechBoni;
	m_iPropulsionTechBoni = other.m_iPropulsionTechBoni;
	m_iConstructionTechBoni = other.m_iConstructionTechBoni;
	m_iWeaponTechBoni = other.m_iWeaponTechBoni;
	m_iInnerSecurityBoni = other.m_iInnerSecurityBoni;
	m_iEconomySpyBoni = other.m_iEconomySpyBoni;
	m_iEconomySabotageBoni = other.m_iEconomySabotageBoni;
	m_iResearchSpyBoni = other.m_iResearchSpyBoni;
	m_iResearchSabotageBoni = other.m_iResearchSabotageBoni;
	m_iMilitarySpyBoni = other.m_iMilitarySpyBoni;
	m_iMilitarySabotageBoni = other.m_iMilitarySabotageBoni;
	m_bShipYard = other.m_bShipYard;
	m_iBuildableShipSizes = other.m_iBuildableShipSizes;
	m_iShipYardEfficiency = other.m_iShipYardEfficiency;
	m_bBarrack = other.m_bBarrack;
	m_iBarracksEfficiency = other.m_iBarracksEfficiency;
	m_iShieldPower = other.m_iShieldPower;
	m_iShieldPowerBoni = other.m_iShieldPowerBoni;
	m_iShipDefend = other.m_iShipDefend;
	m_iShipDefendBoni = other.m_iShipDefendBoni;
	m_iGroundDefend = other.m_iGroundDefend;
	m_iGroundDefendBoni = other.m_iGroundDefendBoni;
	m_iScanPower = other.m_iScanPower;
	m_iScanPowerBoni = other.m_iScanPowerBoni;
	m_iScanRange = other.m_iScanRange;
	m_iScanRangeBoni = other.m_iScanRangeBoni;
	m_iShipTraining = other.m_iShipTraining;
	m_iTroopTraining = other.m_iTroopTraining;
	m_iResistance = other.m_iResistance;
	m_iAddedTradeRouts = other.m_iAddedTradeRouts;
	m_iIncomeOnTradeRoutes = other.m_iIncomeOnTradeRoutes;
	m_iShipRecycling = other.m_iShipRecycling;
	m_iBuildingBuildSpeed = other.m_iBuildingBuildSpeed;
	m_iUpdateBuildSpeed = other.m_iUpdateBuildSpeed;
	m_iShipBuildSpeed = other.m_iShipBuildSpeed;
	m_iTroopBuildSpeed = other.m_iTroopBuildSpeed;

	const unsigned size = sizeof(m_bResourceDistributor) / sizeof(m_bResourceDistributor[0]);
	for(unsigned i = 0; i < size; ++i)
		m_bResourceDistributor[i] = other.m_bResourceDistributor[i];

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
		ar << m_iPotentialIndustryProd;
		ar << m_iEnergyProd;
		ar << m_iMaxEnergyProd;
		ar << m_iPotentialEnergyProd;
		ar << m_iSecurityProd;
		ar << m_iResearchProd;
		ar << m_iTitanProd;
		ar << m_iDeuteriumProd;
		ar << m_iDuraniumProd;
		ar << m_iCrystalProd;
		ar << m_iIridiumProd;
		ar << m_iDeritiumProd;
		ar << m_iCreditsProd;
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
		for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
			ar << m_bResourceDistributor[res];
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iFoodProd;
		ar >> m_iMaxFoodProd;
		ar >> m_iIndustryProd;
		ar >> m_iPotentialIndustryProd;
		ar >> m_iEnergyProd;
		ar >> m_iMaxEnergyProd;
		ar >> m_iPotentialEnergyProd;
		ar >> m_iSecurityProd;
		ar >> m_iResearchProd;
		ar >> m_iTitanProd;
		ar >> m_iDeuteriumProd;
		ar >> m_iDuraniumProd;
		ar >> m_iCrystalProd;
		ar >> m_iIridiumProd;
		ar >> m_iDeritiumProd;
		ar >> m_iCreditsProd;
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
		int nBuildableShipSize;
		ar >> nBuildableShipSize;
		m_iBuildableShipSizes = (SHIP_SIZE::Typ)nBuildableShipSize;
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
		for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
			ar >> m_bResourceDistributor[res];
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
// Funktion berechnet die neuen Produktionen im System die sich durch das übergebene Gebäude ergeben
void CSystemProd::CalculateProduction(const CBuildingInfo* building)
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
	m_iDeritiumProd	+= building->GetDeritiumProd();
	m_iCreditsProd		+= building->GetCredits();
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
	// Ressourcenverteiler
	for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
		m_bResourceDistributor[res] |= building->GetResourceDistributor(res);
}

void CSystemProd::DisableProductions(const bool* vDisabledProductions)
{
	// deaktiverte Produktionen auf 0 setzen
	if (vDisabledProductions[WORKER::FOOD_WORKER])
		m_iFoodProd = m_iMaxFoodProd = 0;

	if (vDisabledProductions[WORKER::INDUSTRY_WORKER])
		m_iIndustryProd = 0;

	if (vDisabledProductions[WORKER::ENERGY_WORKER])
		m_iEnergyProd = m_iMaxEnergyProd = 0;

	if (vDisabledProductions[WORKER::SECURITY_WORKER])
		m_iSecurityProd = 0;

	if (vDisabledProductions[WORKER::RESEARCH_WORKER])
		m_iResearchProd = 0;

	if (vDisabledProductions[WORKER::TITAN_WORKER])
		m_iTitanProd = 0;

	if (vDisabledProductions[WORKER::DEUTERIUM_WORKER])
		m_iDeuteriumProd = 0;

	if (vDisabledProductions[WORKER::DURANIUM_WORKER])
		m_iDuraniumProd = 0;

	if (vDisabledProductions[WORKER::CRYSTAL_WORKER])
		m_iCrystalProd = 0;

	if (vDisabledProductions[WORKER::IRIDIUM_WORKER])
		m_iIridiumProd = 0;
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
	m_iCreditsProd	= (short)(m_iCreditsProd * moral/100);
}

int CSystemProd::GetResourceProd(BYTE res) const
{
	switch (res)
	{
	case RESOURCES::TITAN:
		return m_iTitanProd;
	case RESOURCES::DEUTERIUM:
		return m_iDeuteriumProd;
	case RESOURCES::DURANIUM:
		return m_iDuraniumProd;
	case RESOURCES::CRYSTAL:
		return m_iCrystalProd;
	case RESOURCES::IRIDIUM:
		return m_iIridiumProd;
	case RESOURCES::DERITIUM:
		return m_iDeritiumProd;
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
	m_iDeritiumProd = 0;			// Deritiumproduktion
	m_iCreditsProd = 0;				// Creditsproduktion in dem System, abhängig von der Bevölkerung
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
	m_iBuildableShipSizes = SHIP_SIZE::SMALL;
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
	for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
		m_bResourceDistributor[res] = false;
}

CSystemProd::RESEARCHBONI CSystemProd::GetResearchBoni() const
{
	return RESEARCHBONI(
			m_iBioTechBoni,
			m_iEnergyTechBoni,
			m_iCompTechBoni,
			m_iPropulsionTechBoni,
			m_iConstructionTechBoni,
			m_iWeaponTechBoni
		);
}

int CSystemProd::GetXProd(WORKER::Typ x) const
{
	switch(x)
	{
		case WORKER::FOOD_WORKER: return m_iFoodProd;
		case WORKER::INDUSTRY_WORKER: return m_iIndustryProd;
		case WORKER::ENERGY_WORKER: return m_iEnergyProd;
		case WORKER::SECURITY_WORKER: return m_iSecurityProd;
		case WORKER::RESEARCH_WORKER: return m_iResearchProd;
		case WORKER::TITAN_WORKER: return m_iTitanProd;
		case WORKER::DEUTERIUM_WORKER: return m_iDeuteriumProd;
		case WORKER::DURANIUM_WORKER: return m_iDuraniumProd;
		case WORKER::CRYSTAL_WORKER: return m_iCrystalProd;
		case WORKER::IRIDIUM_WORKER: return m_iIridiumProd;
	}
	assert(false);
	return 0;
}
