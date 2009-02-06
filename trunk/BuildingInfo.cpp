// BuildingInfo.cpp: Implementierung der Klasse CBuildingInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "BuildingInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
// Werte aus der Basisklasse
	m_iRunningNumber = rhs.m_iRunningNumber;
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
	
	m_bOnline = rhs.m_bOnline;
	m_bWorker = rhs.m_bWorker;
	m_bNeverReady = rhs.m_bNeverReady;
// Ende aus der Basisklasse
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
	
	for (int i = 0; i < 20; i++)
		m_bPlanetTypes[i] = rhs.m_bPlanetTypes[i];
	
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;
	
	m_iNeededIndustry = rhs.m_iNeededIndustry;
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
// Werte aus der Basisklasse
	m_iRunningNumber = rhs.m_iRunningNumber;
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
	
	m_bOnline = rhs.m_bOnline;
	m_bWorker = rhs.m_bWorker;
	m_bNeverReady = rhs.m_bNeverReady;
// Ende aus der Basisklasse
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
	
	for (int i = 0; i < 20; i++)
		m_bPlanetTypes[i] = rhs.m_bPlanetTypes[i];
	
	m_iBioTech = rhs.m_iBioTech;
	m_iEnergyTech = rhs.m_iEnergyTech;
	m_iCompTech = rhs.m_iCompTech;
	m_iPropulsionTech = rhs.m_iPropulsionTech;
	m_iConstructionTech = rhs.m_iConstructionTech;
	m_iWeaponTech = rhs.m_iWeaponTech;
	
	m_iNeededIndustry = rhs.m_iNeededIndustry;
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
	CBuilding::Serialize(ar);
	if (ar.IsStoring())
	{
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
		for (int i = 0; i < 20; i++)
			ar << m_bPlanetTypes[i];
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
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
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
		for (int i = 0; i < 20; i++)
			ar >> m_bPlanetTypes[i];
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
