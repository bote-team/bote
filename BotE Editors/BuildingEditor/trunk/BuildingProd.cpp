#include "stdafx.h"
#include "BuildingProd.h"

IMPLEMENT_SERIAL (CBuildingProd, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CBuildingProd::CBuildingProd(void)
{
}

CBuildingProd::~CBuildingProd(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CBuildingProd::CBuildingProd(const CBuildingProd & rhs) : CBuilding(rhs)
{
	m_iNeededEnergy = rhs.m_iNeededEnergy;
	m_iFood = rhs.m_iFood;
	m_iIP = rhs.m_iIP;
	m_iEnergy = rhs.m_iEnergy;
	m_iSP = rhs.m_iSP;
	m_iFP = rhs.m_iFP;
	m_iTitan = rhs.m_iTitan;
	m_iDeuterium = rhs.m_iDeuterium;
	m_iDuranium = rhs.m_iDuranium;
	m_iCrystal = rhs.m_iCrystal;
	m_iIridium = rhs.m_iIridium;
	m_iDilithium = rhs.m_iDilithium;
	m_iLatinum = rhs.m_iLatinum;
	m_iMoral = rhs.m_iMoral;
	m_iMoralEmpire = rhs.m_iMoralEmpire;
	
	m_iFoodBoni = rhs.m_iFoodBoni;
	m_iIndustryBoni = rhs.m_iIndustryBoni;
	m_iEnergyBoni = rhs.m_iEnergyBoni;
	m_iSecurityBoni = rhs.m_iSecurityBoni;
	m_iResearchBoni = rhs.m_iResearchBoni;
	m_iTitanBoni = rhs.m_iTitanBoni;
	m_iDeuteriumBoni = rhs.m_iDeuteriumBoni;
	m_iDuraniumBoni = rhs.m_iDuraniumBoni;
	m_iCrystalBoni = rhs.m_iCrystalBoni;
	m_iIridiumBoni = rhs.m_iIridiumBoni;
	m_iDilithiumBoni = rhs.m_iDilithiumBoni;
	m_iAllRessourceBoni = rhs.m_iAllRessourceBoni;
	m_iLatinumBoni = rhs.m_iLatinumBoni;
	
	m_iBioTechBoni = rhs.m_iBioTechBoni;
	m_iEnergyTechBoni = rhs.m_iEnergyTechBoni;
	m_iCompTechBoni = rhs.m_iCompTechBoni;
	m_iPropulsionTechBoni = rhs.m_iPropulsionTechBoni;
	m_iConstructionTechBoni = rhs.m_iConstructionTechBoni;
	m_iWeaponTechBoni = rhs.m_iWeaponTechBoni;
	
	m_iInnerSecurityBoni = rhs.m_iInnerSecurityBoni;
	m_iEconomySpyBoni = rhs.m_iEconomySpyBoni;
	m_iEconomySabotageBoni = rhs.m_iEconomySabotageBoni;
	m_iResearchSpyBoni = rhs.m_iResearchSpyBoni;
	m_iResearchSabotageBoni = rhs.m_iResearchSabotageBoni;
	m_iMilitarySpyBoni = rhs.m_iMilitarySpyBoni;
	m_iMilitarySabotageBoni = rhs.m_iMilitarySabotageBoni;
	
	m_bShipYard = rhs.m_bShipYard;
	m_iBuildableShipSizes = rhs.m_iBuildableShipSizes;
	m_iShipYardSpeed = rhs.m_iShipYardSpeed;
	m_bBarrack = rhs.m_bBarrack;
	m_iBarrackSpeed = rhs.m_iBarrackSpeed;
	m_iHitPoints = rhs.m_iHitPoints;
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
	m_iAddedTradeRoutes = rhs.m_iAddedTradeRoutes;
	m_iIncomeOnTradeRoutes = rhs.m_iIncomeOnTradeRoutes;
	m_iShipRecycling = rhs.m_iShipRecycling;
	m_iBuildingBuildSpeed = rhs.m_iBuildingBuildSpeed;
	m_iUpdateBuildSpeed = rhs.m_iUpdateBuildSpeed;
	m_iShipBuildSpeed = rhs.m_iShipBuildSpeed;
	m_iTroopBuildSpeed = rhs.m_iTroopBuildSpeed;
	m_bWorker = rhs.m_bWorker;
	m_bNeverReady = rhs.m_bNeverReady;

	for (int res = TITAN; res <= DILITHIUM; res++)
		m_bResourceDistributor[res] = rhs.m_bResourceDistributor[res];
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CBuildingProd & CBuildingProd::operator=(const CBuildingProd & rhs)
{
	if (this == &rhs)
		return *this;
	
	CBuilding::operator =(rhs);

	m_iNeededEnergy = rhs.m_iNeededEnergy;
	m_iFood = rhs.m_iFood;
	m_iIP = rhs.m_iIP;
	m_iEnergy = rhs.m_iEnergy;
	m_iSP = rhs.m_iSP;
	m_iFP = rhs.m_iFP;
	m_iTitan = rhs.m_iTitan;
	m_iDeuterium = rhs.m_iDeuterium;
	m_iDuranium = rhs.m_iDuranium;
	m_iCrystal = rhs.m_iCrystal;
	m_iIridium = rhs.m_iIridium;
	m_iDilithium = rhs.m_iDilithium;
	m_iLatinum = rhs.m_iLatinum;
	m_iMoral = rhs.m_iMoral;
	m_iMoralEmpire = rhs.m_iMoralEmpire;
	
	m_iFoodBoni = rhs.m_iFoodBoni;
	m_iIndustryBoni = rhs.m_iIndustryBoni;
	m_iEnergyBoni = rhs.m_iEnergyBoni;
	m_iSecurityBoni = rhs.m_iSecurityBoni;
	m_iResearchBoni = rhs.m_iResearchBoni;
	m_iTitanBoni = rhs.m_iTitanBoni;
	m_iDeuteriumBoni = rhs.m_iDeuteriumBoni;
	m_iDuraniumBoni = rhs.m_iDuraniumBoni;
	m_iCrystalBoni = rhs.m_iCrystalBoni;
	m_iIridiumBoni = rhs.m_iIridiumBoni;
	m_iDilithiumBoni = rhs.m_iDilithiumBoni;
	m_iAllRessourceBoni = rhs.m_iAllRessourceBoni;
	m_iLatinumBoni = rhs.m_iLatinumBoni;
	
	m_iBioTechBoni = rhs.m_iBioTechBoni;
	m_iEnergyTechBoni = rhs.m_iEnergyTechBoni;
	m_iCompTechBoni = rhs.m_iCompTechBoni;
	m_iPropulsionTechBoni = rhs.m_iPropulsionTechBoni;
	m_iConstructionTechBoni = rhs.m_iConstructionTechBoni;
	m_iWeaponTechBoni = rhs.m_iWeaponTechBoni;
	
	m_iInnerSecurityBoni = rhs.m_iInnerSecurityBoni;
	m_iEconomySpyBoni = rhs.m_iEconomySpyBoni;
	m_iEconomySabotageBoni = rhs.m_iEconomySabotageBoni;
	m_iResearchSpyBoni = rhs.m_iResearchSpyBoni;
	m_iResearchSabotageBoni = rhs.m_iResearchSabotageBoni;
	m_iMilitarySpyBoni = rhs.m_iMilitarySpyBoni;
	m_iMilitarySabotageBoni = rhs.m_iMilitarySabotageBoni;
	
	m_bShipYard = rhs.m_bShipYard;
	m_iBuildableShipSizes = rhs.m_iBuildableShipSizes;
	m_iShipYardSpeed = rhs.m_iShipYardSpeed;
	m_bBarrack = rhs.m_bBarrack;
	m_iBarrackSpeed = rhs.m_iBarrackSpeed;
	m_iHitPoints = rhs.m_iHitPoints;
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
	m_iAddedTradeRoutes = rhs.m_iAddedTradeRoutes;
	m_iIncomeOnTradeRoutes = rhs.m_iIncomeOnTradeRoutes;
	m_iShipRecycling = rhs.m_iShipRecycling;
	m_iBuildingBuildSpeed = rhs.m_iBuildingBuildSpeed;
	m_iUpdateBuildSpeed = rhs.m_iUpdateBuildSpeed;
	m_iShipBuildSpeed = rhs.m_iShipBuildSpeed;
	m_iTroopBuildSpeed = rhs.m_iTroopBuildSpeed;
		
	m_bWorker = rhs.m_bWorker;
	m_bNeverReady = rhs.m_bNeverReady;

	for (int res = TITAN; res <= DILITHIUM; res++)
		m_bResourceDistributor[res] = rhs.m_bResourceDistributor[res];

	return *this;
}
///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CBuildingProd::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	
	CBuilding::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		// Die nötige Energie zum Betrieb des Gebäudes
		ar << m_iNeededEnergy;	
		// Produktion -> überprüfen ob diese auch ar << bleiben können?
		ar << m_iFood;
		ar << m_iIP;
		ar << m_iEnergy;
		ar << m_iSP;
		ar << m_iFP;
		ar << m_iTitan;
		ar << m_iDeuterium;
		ar << m_iDuranium;
		ar << m_iCrystal;
		ar << m_iIridium;
		ar << m_iDilithium;
		ar << m_iLatinum;
		ar << m_iMoral;
		ar << m_iMoralEmpire;
		// Boni / Mali, das sind alles Prozentangaben zu irgendwas
		// Produktionsboni
		ar << m_iFoodBoni;
		ar << m_iIndustryBoni;
		ar << m_iEnergyBoni;
		ar << m_iSecurityBoni;
		ar << m_iResearchBoni;
		ar << m_iTitanBoni;
		ar << m_iDeuteriumBoni;
		ar << m_iDuraniumBoni;
		ar << m_iCrystalBoni;
		ar << m_iIridiumBoni;
		ar << m_iDilithiumBoni;
		ar << m_iAllRessourceBoni;	
		ar << m_iLatinumBoni;
		// Forschungstechboni
		ar << m_iBioTechBoni;
		ar << m_iEnergyTechBoni;
		ar << m_iCompTechBoni;
		ar << m_iPropulsionTechBoni;
		ar << m_iConstructionTechBoni;
		ar << m_iWeaponTechBoni;
		// Sicherheitsboni
		ar << m_iInnerSecurityBoni;
		ar << m_iEconomySpyBoni;
		ar << m_iEconomySabotageBoni;
		ar << m_iResearchSpyBoni;
		ar << m_iResearchSabotageBoni;
		ar << m_iMilitarySpyBoni;
		ar << m_iMilitarySabotageBoni;
		// sonstige Informationen
		ar << m_bShipYard;
		ar << m_iBuildableShipSizes;
		ar << m_iShipYardSpeed;
		ar << m_bBarrack;
		ar << m_iBarrackSpeed;
		ar << m_iHitPoints;
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
		ar << m_iResistance;				// Bestechungsresistenz
		ar << m_iAddedTradeRoutes;
		ar << m_iIncomeOnTradeRoutes;
		ar << m_iShipRecycling;
		ar << m_iBuildingBuildSpeed;
		ar << m_iUpdateBuildSpeed;
		ar << m_iShipBuildSpeed;
		ar << m_iTroopBuildSpeed;
		// benötigt das Gebäude Arbeiter?
		ar << m_bWorker;
		// ist das Gebäude niemals fertig (z.B. ein Tribunal)
		ar << m_bNeverReady;
		for (int res = TITAN; res <= DILITHIUM; res++)
			ar << m_bResourceDistributor[res];
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		// Die nötige Energie zum Betrieb des Gebäudes
		ar >> m_iNeededEnergy;	
		// Produktion -> überprüfen ob diese auch ar >> bleiben können?
		ar >> m_iFood;
		ar >> m_iIP;
		ar >> m_iEnergy;
		ar >> m_iSP;
		ar >> m_iFP;
		ar >> m_iTitan;
		ar >> m_iDeuterium;
		ar >> m_iDuranium;
		ar >> m_iCrystal;
		ar >> m_iIridium;
		ar >> m_iDilithium;
		ar >> m_iLatinum;
		ar >> m_iMoral;
		ar >> m_iMoralEmpire;
		// Boni / Mali, das sind alles Prozentangaben zu irgendwas
		// Produktionsboni
		ar >> m_iFoodBoni;
		ar >> m_iIndustryBoni;
		ar >> m_iEnergyBoni;
		ar >> m_iSecurityBoni;
		ar >> m_iResearchBoni;
		ar >> m_iTitanBoni;
		ar >> m_iDeuteriumBoni;
		ar >> m_iDuraniumBoni;
		ar >> m_iCrystalBoni;
		ar >> m_iIridiumBoni;
		ar >> m_iDilithiumBoni;
		ar >> m_iAllRessourceBoni;	
		ar >> m_iLatinumBoni;
		// Forschungstechboni
		ar >> m_iBioTechBoni;
		ar >> m_iEnergyTechBoni;
		ar >> m_iCompTechBoni;
		ar >> m_iPropulsionTechBoni;
		ar >> m_iConstructionTechBoni;
		ar >> m_iWeaponTechBoni;
		// Sicherheitsboni
		ar >> m_iInnerSecurityBoni;
		ar >> m_iEconomySpyBoni;
		ar >> m_iEconomySabotageBoni;
		ar >> m_iResearchSpyBoni;
		ar >> m_iResearchSabotageBoni;
		ar >> m_iMilitarySpyBoni;
		ar >> m_iMilitarySabotageBoni;
		// sonstige Informationen
		ar >> m_bShipYard;
		ar >> m_iBuildableShipSizes;
		ar >> m_iShipYardSpeed;
		ar >> m_bBarrack;
		ar >> m_iBarrackSpeed;
		ar >> m_iHitPoints;
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
		ar >> m_iResistance;				// Bestechungsresistenz
		ar >> m_iAddedTradeRoutes;
		ar >> m_iIncomeOnTradeRoutes;
		ar >> m_iShipRecycling;
		ar >> m_iBuildingBuildSpeed;
		ar >> m_iUpdateBuildSpeed;
		ar >> m_iShipBuildSpeed;
		ar >> m_iTroopBuildSpeed;
		// benötigt das Gebäude Arbeiter?
		ar >> m_bWorker;
		// ist das Gebäude niemals fertig (z.B. ein Tribunal)
		ar >> m_bNeverReady;
		for (int res = TITAN; res <= DILITHIUM; res++)
			ar >> m_bResourceDistributor[res];
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion gibt die Produktion des übergebenen Parameters zurück.
USHORT CBuildingProd::GetResourceProd(BYTE res) const
{
	switch (res)
	{
	case TITAN:		return GetTitanProd();
	case DEUTERIUM: return GetDeuteriumProd();
	case DURANIUM:	return GetDuraniumProd();
	case CRYSTAL:	return GetCrystalProd();
	case IRIDIUM:	return GetIridiumProd();
	case DILITHIUM:	return GetDilithiumProd();
	default: {
		TRACE("CBuildingProd::GetResourceProd(BYTE res) -> wrong res");
		return 0;
			 }
	}
}
