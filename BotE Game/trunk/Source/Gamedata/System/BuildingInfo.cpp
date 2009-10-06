// BuildingInfo.cpp: Implementierung der Klasse CBuildingInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BuildingInfo.h"

IMPLEMENT_SERIAL (CBuildingInfo, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CBuildingInfo::CBuildingInfo()
{	
}

CBuildingInfo::~CBuildingInfo()
{
}
//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CBuildingInfo::CBuildingInfo(const CBuildingInfo & rhs)
{
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
	m_bAllwaysOnline = rhs.m_bAllwaysOnline;

	m_iRunningNumber = rhs.m_iRunningNumber;
	m_iOwnerOfBuilding = rhs.m_iOwnerOfBuilding;
	m_strBuildingName = rhs.m_strBuildingName;
	m_strDescription = rhs.m_strDescription;
	m_bUpgradeable = rhs.m_bUpgradeable;
	m_strGraphikFileName = rhs.m_strGraphikFileName;
	m_MaxInSystem = rhs.m_MaxInSystem;
	m_MaxInEmpire = rhs.m_MaxInEmpire;
	m_bOnlyHomePlanet = rhs.m_bOnlyHomePlanet;
	m_bOnlyOwnColony = rhs.m_bOnlyOwnColony;
	m_bOnlyMinorRace = rhs.m_bOnlyMinorRace;
	m_bOnlyTakenSystem = rhs.m_bOnlyTakenSystem;
	m_strOnlyInSystemWithName = rhs.m_strOnlyInSystemWithName;
	m_iMinHabitants = rhs.m_iMinHabitants;
	m_MinInSystem = rhs.m_MinInSystem;
	m_MinInEmpire = rhs.m_MinInEmpire;
	m_bOnlyRace = rhs.m_bOnlyRace;
	
	m_PlanetTypes = rhs.m_PlanetTypes;
	
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;
	
	m_iNeededIndustry = rhs.m_iNeededIndustry;
	m_iNeededEnergy = rhs.m_iNeededEnergy;
	m_iNeededTitan = rhs.m_iNeededTitan;
	m_iNeededDeuterium = rhs.m_iNeededDeuterium;
	m_iNeededDuranium = rhs.m_iNeededDuranium;
	m_iNeededCrystal = rhs.m_iNeededCrystal;
	m_iNeededIridium = rhs.m_iNeededIridium;
	m_iNeededDilithium = rhs.m_iNeededDilithium;

	m_iPredecessorID = rhs.m_iPredecessorID;

	for (int i = 0; i < 7; i++)
		m_iBuildingEquivalent[i] = rhs.m_iBuildingEquivalent[i];
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CBuildingInfo & CBuildingInfo::operator=(const CBuildingInfo & rhs)
{
	if (this == &rhs)
		return *this;
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
	m_bAllwaysOnline = rhs.m_bAllwaysOnline;

	m_iRunningNumber = rhs.m_iRunningNumber;
	m_iOwnerOfBuilding = rhs.m_iOwnerOfBuilding;
	m_strBuildingName = rhs.m_strBuildingName;
	m_strDescription = rhs.m_strDescription;
	m_bUpgradeable = rhs.m_bUpgradeable;
	m_strGraphikFileName = rhs.m_strGraphikFileName;
	m_MaxInSystem = rhs.m_MaxInSystem;
	m_MaxInEmpire = rhs.m_MaxInEmpire;
	m_bOnlyHomePlanet = rhs.m_bOnlyHomePlanet;
	m_bOnlyOwnColony = rhs.m_bOnlyOwnColony;
	m_bOnlyMinorRace = rhs.m_bOnlyMinorRace;
	m_bOnlyTakenSystem = rhs.m_bOnlyTakenSystem;
	m_strOnlyInSystemWithName = rhs.m_strOnlyInSystemWithName;
	m_iMinHabitants = rhs.m_iMinHabitants;
	m_MinInSystem = rhs.m_MinInSystem;
	m_MinInEmpire = rhs.m_MinInEmpire;
	m_bOnlyRace = rhs.m_bOnlyRace;
	
	m_PlanetTypes = rhs.m_PlanetTypes;
	
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;
	
	m_iNeededIndustry = rhs.m_iNeededIndustry;
	m_iNeededEnergy = rhs.m_iNeededEnergy;
	m_iNeededTitan = rhs.m_iNeededTitan;
	m_iNeededDeuterium = rhs.m_iNeededDeuterium;
	m_iNeededDuranium = rhs.m_iNeededDuranium;
	m_iNeededCrystal = rhs.m_iNeededCrystal;
	m_iNeededIridium = rhs.m_iNeededIridium;
	m_iNeededDilithium = rhs.m_iNeededDilithium;

	m_iPredecessorID = rhs.m_iPredecessorID;

	for (int i = 0; i < 7; i++)
		m_iBuildingEquivalent[i] = rhs.m_iBuildingEquivalent[i];
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CBuildingInfo::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iRunningNumber;
		ar << m_iOwnerOfBuilding;
		ar << m_strBuildingName;
		ar << m_strDescription;
		ar << m_bUpgradeable;
		ar << m_strGraphikFileName;
		ar << m_MaxInSystem.Number;
		ar << m_MaxInSystem.RunningNumber;
		ar << m_MaxInEmpire.Number;
		ar << m_MaxInEmpire.RunningNumber;
		ar << m_bOnlyHomePlanet;
		ar << m_bOnlyOwnColony;
		ar << m_bOnlyMinorRace;
		ar << m_bOnlyTakenSystem;
		ar << m_strOnlyInSystemWithName;
		ar << m_iMinHabitants;
		ar << m_MinInSystem.Number;
		ar << m_MinInSystem.RunningNumber;
		ar << m_MinInEmpire.Number;
		ar << m_MinInEmpire.RunningNumber;
		ar << m_bOnlyRace;
		ar << m_PlanetTypes;
		ar << m_iBioTech;
		ar << m_iEnergyTech;
		ar << m_iCompTech;
		ar << m_iPropulsionTech;
		ar << m_iConstructionTech;
		ar << m_iWeaponTech;	
		ar << m_iNeededIndustry;
		ar << m_iNeededTitan;
		ar << m_iNeededDeuterium;
		ar << m_iNeededDuranium;
		ar << m_iNeededCrystal;
		ar << m_iNeededIridium;
		ar << m_iNeededDilithium;
		ar << m_iPredecessorID;
		for (int i = 0; i < 7; i++)
			ar << m_iBuildingEquivalent[i];

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
		ar << m_bAllwaysOnline;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iRunningNumber;
		ar >> m_iOwnerOfBuilding;
		ar >> m_strBuildingName;
		ar >> m_strDescription;
		ar >> m_bUpgradeable;
		ar >> m_strGraphikFileName;
		ar >> m_MaxInSystem.Number;
		ar >> m_MaxInSystem.RunningNumber;
		ar >> m_MaxInEmpire.Number;
		ar >> m_MaxInEmpire.RunningNumber;
		ar >> m_bOnlyHomePlanet;
		ar >> m_bOnlyOwnColony;
		ar >> m_bOnlyMinorRace;
		ar >> m_bOnlyTakenSystem;
		ar >> m_strOnlyInSystemWithName;
		ar >> m_iMinHabitants;
		ar >> m_MinInSystem.Number;
		ar >> m_MinInSystem.RunningNumber;
		ar >> m_MinInEmpire.Number;
		ar >> m_MinInEmpire.RunningNumber;
		ar >> m_bOnlyRace;
		ar >> m_PlanetTypes;
		ar >> m_iBioTech;
		ar >> m_iEnergyTech;
		ar >> m_iCompTech;
		ar >> m_iPropulsionTech;
		ar >> m_iConstructionTech;
		ar >> m_iWeaponTech;	
		ar >> m_iNeededIndustry;
		ar >> m_iNeededTitan;
		ar >> m_iNeededDeuterium;
		ar >> m_iNeededDuranium;
		ar >> m_iNeededCrystal;
		ar >> m_iNeededIridium;
		ar >> m_iNeededDilithium;
		ar >> m_iPredecessorID;
		for (int i = 0; i < 7; i++)
			ar >> m_iBuildingEquivalent[i];

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
		ar >> m_bAllwaysOnline;
	}
}

USHORT CBuildingInfo::GetNeededResource(BYTE res) const
{
	switch (res)
	{
	case TITAN:
		return m_iNeededTitan;
	case DEUTERIUM:
		return m_iNeededDeuterium;
	case DURANIUM:
		return m_iNeededDuranium;
	case CRYSTAL:
		return m_iNeededCrystal;
	case IRIDIUM:
		return m_iNeededIridium;
	case DILITHIUM:
		return m_iNeededDilithium;
	default:
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion gibt die Produktion des übergebenen Parameters zurück.
USHORT CBuildingInfo::GetResourceProd(BYTE res) const
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
		MYTRACE(MT::LEVEL_WARNING, "CBuildingInfo::GetResourceProd(): Resource %d doesn't exists!");
		return 0;
			 }
	}
}

/// Diese Funktion erstellt einen String über die komplette Produktion des Gebäudes. Übergeben wird dabei
/// die Anzahl dieser Gebäude.
CString CBuildingInfo::GetProductionAsString(USHORT number) const
{
	CString s = "";
	if (this->GetFoodProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("FOOD"), this->GetFoodProd() * number);
	if (this->GetIPProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("INDUSTRY"), this->GetIPProd() * number);
	if (this->GetEnergyProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("ENERGY"), this->GetEnergyProd() * number);
	if (this->GetSPProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("SECURITY"), this->GetSPProd() * number);
	if (this->GetFPProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("RESEARCH"), this->GetFPProd() * number);
	if (this->GetTitanProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("TITAN"), this->GetTitanProd() * number);
	if (this->GetDeuteriumProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("DEUTERIUM"), this->GetDeuteriumProd() * number);
	if (this->GetDuraniumProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("DURANIUM"), this->GetDuraniumProd() * number);
	if (this->GetCrystalProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("CRYSTAL"), this->GetCrystalProd() * number);
	if (this->GetIridiumProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("IRIDIUM"), this->GetIridiumProd() * number);
	if (this->GetDilithiumProd() > 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("DILITHIUM"), this->GetDilithiumProd() * number);
	if (this->GetLatinum() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("LATINUM"), this->GetLatinum() * number);
	if (this->GetMoralProd() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("MORAL"), this->GetMoralProd() * number);
	if (this->GetMoralProdEmpire() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("MORAL_EMPIREWIDE"), this->GetMoralProdEmpire() * number);
	
	// Ab hier die Boni
	if (this->GetFoodBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("FOOD_BONUS"), this->GetFoodBoni() * number);
	if (this->GetIndustryBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("INDUSTRY_BONUS"), this->GetIndustryBoni() * number);
	if (this->GetEnergyBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("ENERGY_BONUS"), this->GetEnergyBoni() * number);
	if (this->GetSecurityBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SECURITY_BONUS"), this->GetSecurityBoni() * number);
	if (this->GetResearchBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("RESEARCH_BONUS"), this->GetResearchBoni() * number);
	if (this->GetTitanBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("TITAN_BONUS"), this->GetTitanBoni() * number);
	if (this->GetDeuteriumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("DEUTERIUM_BONUS"), this->GetDeuteriumBoni() * number);
	if (this->GetDuraniumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("DURANIUM_BONUS"), this->GetDuraniumBoni() * number);
	if (this->GetCrystalBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("CRYSTAL_BONUS"), this->GetCrystalBoni() * number);
	if (this->GetIridiumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("IRIDIUM_BONUS"), this->GetIridiumBoni() * number);
	if (this->GetDilithiumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("DILITHIUM_BONUS"), this->GetDilithiumBoni() * number);
	if (this->GetAllRessourcesBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("BONUS_TO_ALL_RES"), this->GetAllRessourcesBoni() * number);
	if (this->GetLatinumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("LATINUM_BONUS"), this->GetLatinumBoni() * number);
	
	// Wenn alle Forschungsboni belegt sind, sprich Bonus auf alles
	if (this->GetBioTechBoni() != 0 && this->GetBioTechBoni() == this->GetEnergyTechBoni() && this->GetBioTechBoni() == this->GetCompTechBoni()
		&& this->GetBioTechBoni() == this->GetPropulsionTechBoni() && this->GetBioTechBoni() == this->GetConstructionTechBoni()
		&& this->GetBioTechBoni() == this->GetWeaponTechBoni())
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("TECHNIC_BONUS"), this->GetBioTechBoni() * number);
	else // bei einzelnen Gebieten
	{
		if (this->GetBioTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("BIOTECH_BONUS"), this->GetBioTechBoni() * number);
		if (this->GetEnergyTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("ENERGYTECH_BONUS"), this->GetEnergyTechBoni() * number);
		if (this->GetCompTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("COMPUTERTECH_BONUS"), this->GetCompTechBoni() * number);
		if (this->GetPropulsionTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("PROPULSIONTECH_BONUS"), this->GetPropulsionTechBoni() * number);
		if (this->GetConstructionTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("CONSTRUCTIONTECH_BONUS"), this->GetConstructionTechBoni() * number);
		if (this->GetWeaponTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("WEAPONTECH_BONUS"), this->GetWeaponTechBoni() * number);
	}
	// Wenn alle Geheimdienstdinger belegt sind -> Geheimdienst auf alles
	if (this->GetInnerSecurityBoni() != 0 && this->GetInnerSecurityBoni() == this->GetEconomySpyBoni() 
		&& this->GetInnerSecurityBoni() ==	this->GetEconomySabotageBoni() && this->GetInnerSecurityBoni() == this->GetResearchSpyBoni()
		&& this->GetInnerSecurityBoni() == this->GetResearchSabotageBoni() && this->GetInnerSecurityBoni() == this->GetMilitarySpyBoni()
		&& this->GetInnerSecurityBoni() == this->GetMilitarySabotageBoni())
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("COMPLETE_SECURITY_BONUS"), this->GetInnerSecurityBoni() * number);
	else
	{
		if (this->GetEconomySpyBoni() != 0 && this->GetEconomySpyBoni() == this->GetResearchSpyBoni()
			&& this->GetEconomySpyBoni() == this->GetMilitarySpyBoni())
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SPY_BONUS"), this->GetEconomySpyBoni() * number);
		else
		{
			if (this->GetEconomySpyBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("ECONOMY_SPY_BONUS"), this->GetEconomySpyBoni() * number);
			if (this->GetResearchSpyBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("RESEARCH_SPY_BONUS"), this->GetResearchSpyBoni() * number);
			if (this->GetMilitarySpyBoni() != 0)
				s.AppendFormat("%s: %i%%",CResourceManager::GetString("MILITARY_SPY_BONUS"), this->GetMilitarySpyBoni() * number);
		}
		if (this->GetEconomySabotageBoni() != 0 && this->GetEconomySabotageBoni() == this->GetResearchSabotageBoni()
			&& this->GetEconomySabotageBoni() == this->GetMilitarySabotageBoni())
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SABOTAGE_BONUS"), this->GetEconomySabotageBoni() * number);
		else
		{
			if (this->GetEconomySabotageBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("ECONOMY_SABOTAGE_BONUS"), this->GetEconomySabotageBoni() * number);
			if (this->GetResearchSabotageBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("RESEARCH_SABOTAGE_BONUS"), this->GetResearchSabotageBoni() * number);
			if (this->GetMilitarySabotageBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("MILITARY_SABOTAGE_BONUS"), this->GetMilitarySabotageBoni() * number);
		}
		if (this->GetInnerSecurityBoni() != 0)
			s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("INNER_SECURITY_BONUS"), this->GetInnerSecurityBoni() * number);
	}
	// Ab hier zusätzliche Eigenschaften des Gebäudes
	if (this->GetShipYard() == TRUE)
	{
		s.AppendFormat("%s\n",CResourceManager::GetString("SHIPYARD"));
		CString ss;
		switch (this->GetMaxBuildableShipSize())
		{
			case 0: ss = CResourceManager::GetString("SMALL"); break;
			case 1: ss = CResourceManager::GetString("MIDDLE"); break;
			case 2: ss = CResourceManager::GetString("BIG"); break;
			case 3: ss = CResourceManager::GetString("HUGE"); break;
			default: ss = CResourceManager::GetString("HUGE");
		}
		s.AppendFormat("%s: %s\n",CResourceManager::GetString("MAX_SHIPSIZE"), ss);
	}
	if (this->GetShipYardSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SHIPYARD_EFFICIENCY"), this->GetShipYardSpeed());
	if (this->GetBarrack() == TRUE)
		s.AppendFormat("%s\n",CResourceManager::GetString("BARRACK"));
	if (this->GetBarrackSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("BARRACK_EFFICIENCY"), this->GetBarrackSpeed());
	if (this->GetShieldPower() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("SHIELDPOWER"), this->GetShieldPower() * number);
	if (this->GetShieldPowerBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SHIELDPOWER_BONUS"), this->GetShieldPowerBoni() * number);
	if (this->GetShipDefend() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("SHIPDEFEND"), this->GetShipDefend() * number);
	if (this->GetShipDefendBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SHIPDEFEND_BONUS"), this->GetShipDefendBoni() * number);
	if (this->GetGroundDefend() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("GROUNDDEFEND"), this->GetGroundDefend() * number);
	if (this->GetGroundDefendBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("GROUNDDEFEND_BONUS"), this->GetGroundDefendBoni() * number);
	if (this->GetScanPower() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("SCANPOWER"), this->GetScanPower() * number);
	if (this->GetScanPowerBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SCANPOWER_BONUS"), this->GetScanPowerBoni() * number);
	if (this->GetScanRange() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("SCANRANGE"), this->GetScanRange() * number);
	if (this->GetScanRangeBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SCANRANGE_BONUS"), this->GetScanRangeBoni() * number);
	if (this->GetShipTraining() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("SHIPTRAINING"), this->GetShipTraining() * number);
	if (this->GetTroopTraining() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("TROOPTRAINING"), this->GetTroopTraining() * number);
	if (this->GetResistance() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("RESISTANCE"), this->GetResistance() * number);
	if (this->GetAddedTradeRoutes() != 0)
		s.AppendFormat("%s: %i\n",CResourceManager::GetString("ADDED_TRADEROUTES"), this->GetAddedTradeRoutes() * number);
	if (this->GetIncomeOnTradeRoutes() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("INCOME_ON_TRADEROUTES"), this->GetIncomeOnTradeRoutes() * number);
	if (this->GetShipRecycling() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SHIPRECYCLING"), this->GetShipRecycling() * number);
	if (this->GetBuildingBuildSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("BUILDING_BUILDSPEED"), this->GetBuildingBuildSpeed() * number);
	if (this->GetUpdateBuildSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("UPGRADE_BUILDSPEED"), this->GetUpdateBuildSpeed() * number);
	if (this->GetShipBuildSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("SHIP_BUILDSPEED"), this->GetShipBuildSpeed() * number);
	if (this->GetTroopBuildSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CResourceManager::GetString("TROOP_BUILDSPEED"), this->GetTroopBuildSpeed() * number);
	return s;
}

/// Funktion gibt zurück, ob das Gebäude mit der aktuellen Forschung einer Rasse baubar ist.
/// @param pResearch Forschungsobjekt der Rasse
/// @return Wahrheitswert
bool CBuildingInfo::IsBuildingBuildableNow(const BYTE reserachLevels[6]) const
{
	// zuerstmal die Forschungsstufen checken
	if (reserachLevels[0] < this->GetBioTech())
		return false;
	if (reserachLevels[1] < this->GetEnergyTech())
		return false;
	if (reserachLevels[2] < this->GetCompTech())
		return false;
	if (reserachLevels[3] < this->GetPropulsionTech())
		return false;
	if (reserachLevels[4] < this->GetConstructionTech())
		return false;
	if (reserachLevels[5] < this->GetWeaponTech())
		return false;
	
	return true;
}