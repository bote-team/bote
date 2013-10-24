// BuildingInfo.cpp: Implementierung der Klasse CBuildingInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BuildingInfo.h"
#include "General/Loc.h"

#include <cassert>

IMPLEMENT_SERIAL (CBuildingInfo, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CBuildingInfo::CBuildingInfo() :
	m_iRunningNumber(0),
	m_iOwnerOfBuilding(0),
	m_bUpgradeable(FALSE),
	m_MaxInSystem(),
	m_MaxInEmpire(),
	m_bOnlyHomePlanet(FALSE),
	m_bOnlyOwnColony(FALSE),
	m_bOnlyMinorRace(FALSE),
	m_bOnlyTakenSystem(FALSE),
	m_iMinHabitants(0),
	m_MinInSystem(),
	m_MinInEmpire(),
	m_bOnlyRace(FALSE),
	m_PlanetTypes(0),
	m_iBioTech(0),
	m_iEnergyTech(0),
	m_iCompTech(0),
	m_iPropulsionTech(0),
	m_iConstructionTech(0),
	m_iWeaponTech(0),
	m_iNeededIndustry(0),
	m_iNeededTitan(0),
	m_iNeededDeuterium(0),
	m_iNeededDuranium(0),
	m_iNeededCrystal(0),
	m_iNeededIridium(0),
	m_iNeededDeritium(0),
	m_iPredecessorID(0),
	m_iNeededEnergy(0),
	m_bAllwaysOnline(FALSE),
	m_iFood(0),
	m_iIP(0),
	m_iEnergy(0),
	m_iSP(0),
	m_iFP(0),
	m_iTitan(0),
	m_iDeuterium(0),
	m_iDuranium(0),
	m_iCrystal(0),
	m_iIridium(0),
	m_iDeritium(0),
	m_iCredits(0),
	m_iMoral(0),
	m_iMoralEmpire(0),
	m_iFoodBoni(0),
	m_iIndustryBoni(0),
	m_iEnergyBoni(0),
	m_iSecurityBoni(0),
	m_iResearchBoni(0),
	m_iTitanBoni(0),
	m_iDeuteriumBoni(0),
	m_iDuraniumBoni(0),
	m_iCrystalBoni(0),
	m_iIridiumBoni(0),
	m_iDeritiumBoni(0),
	m_iAllRessourceBoni(0),
	m_iCreditsBoni(0),
	m_iBioTechBoni(0),
	m_iEnergyTechBoni(0),
	m_iCompTechBoni(0),
	m_iPropulsionTechBoni(0),
	m_iConstructionTechBoni(0),
	m_iWeaponTechBoni(0),
	m_iInnerSecurityBoni(0),
	m_iEconomySpyBoni(0),
	m_iEconomySabotageBoni(0),
	m_iResearchSpyBoni(0),
	m_iResearchSabotageBoni(0),
	m_iMilitarySpyBoni(0),
	m_iMilitarySabotageBoni(0),
	m_bShipYard(FALSE),
	m_iShipYardSpeed(0),
	m_bBarrack(FALSE),
	m_iBarrackSpeed(0),
	m_iHitPoints(0),
	m_iShieldPower(0),
	m_iShieldPowerBoni(0),
	m_iShipDefend(0),
	m_iShipDefendBoni(0),
	m_iGroundDefend(0),
	m_iGroundDefendBoni(0),
	m_iScanPower(0),
	m_iScanPowerBoni(0),
	m_iScanRange(0),
	m_iScanRangeBoni(0),
	m_iShipTraining(0),
	m_iTroopTraining(0),
	m_iResistance(0),
	m_iAddedTradeRoutes(0),
	m_iIncomeOnTradeRoutes(0),
	m_iShipRecycling(0),
	m_iBuildingBuildSpeed(0),
	m_iUpdateBuildSpeed(0),
	m_iShipBuildSpeed(0),
	m_iTroopBuildSpeed(0),
	m_bWorker(FALSE),
	m_bNeverReady(FALSE),
	m_iNeededSystems(0)
{
	memset(m_iBuildingEquivalent, 0, sizeof(m_iBuildingEquivalent));
	memset(m_bResourceDistributor, FALSE, sizeof(m_bResourceDistributor));
}

CBuildingInfo::~CBuildingInfo()
{
}
//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CBuildingInfo::CBuildingInfo(const CBuildingInfo & rhs) :
	m_MaxInSystem(rhs.m_MaxInSystem),
	m_MaxInEmpire(rhs.m_MaxInEmpire),
	m_MinInSystem(rhs.m_MinInSystem),
	m_MinInEmpire(rhs.m_MinInEmpire)
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
	m_iDeritium = rhs.m_iDeritium;
	m_iCredits = rhs.m_iCredits;
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
	m_iDeritiumBoni = rhs.m_iDeritiumBoni;
	m_iAllRessourceBoni = rhs.m_iAllRessourceBoni;
	m_iCreditsBoni = rhs.m_iCreditsBoni;

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
	m_bOnlyHomePlanet = rhs.m_bOnlyHomePlanet;
	m_bOnlyOwnColony = rhs.m_bOnlyOwnColony;
	m_bOnlyMinorRace = rhs.m_bOnlyMinorRace;
	m_bOnlyTakenSystem = rhs.m_bOnlyTakenSystem;
	m_strOnlyInSystemWithName = rhs.m_strOnlyInSystemWithName;
	m_iMinHabitants = rhs.m_iMinHabitants;
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
	m_iNeededDeritium = rhs.m_iNeededDeritium;

	m_iPredecessorID = rhs.m_iPredecessorID;

	for (int i = 0; i < 7; i++)
		m_iBuildingEquivalent[i] = rhs.m_iBuildingEquivalent[i];

	for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
		m_bResourceDistributor[res] = rhs.m_bResourceDistributor[res];
	m_iNeededSystems = rhs.m_iNeededSystems;
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
	m_iDeritium = rhs.m_iDeritium;
	m_iCredits = rhs.m_iCredits;
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
	m_iDeritiumBoni = rhs.m_iDeritiumBoni;
	m_iAllRessourceBoni = rhs.m_iAllRessourceBoni;
	m_iCreditsBoni = rhs.m_iCreditsBoni;

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
	m_iNeededDeritium = rhs.m_iNeededDeritium;

	m_iPredecessorID = rhs.m_iPredecessorID;

	for (int i = 0; i < 7; i++)
		m_iBuildingEquivalent[i] = rhs.m_iBuildingEquivalent[i];

	for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
		m_bResourceDistributor[res] = rhs.m_bResourceDistributor[res];
	m_iNeededSystems = rhs.m_iNeededSystems;

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
		ar << m_iNeededDeritium;
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
		ar << m_iDeritium;
		ar << m_iCredits;
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
		ar << m_iDeritiumBoni;
		ar << m_iAllRessourceBoni;
		ar << m_iCreditsBoni;
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
		for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
			ar << m_bResourceDistributor[res];
		ar << m_iNeededSystems;
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
		ar >> m_iNeededDeritium;
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
		ar >> m_iDeritium;
		ar >> m_iCredits;
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
		ar >> m_iDeritiumBoni;
		ar >> m_iAllRessourceBoni;
		ar >> m_iCreditsBoni;
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
		int nBuildableShipSizes;
		ar >> nBuildableShipSizes;
		m_iBuildableShipSizes = (SHIP_SIZE::Typ)nBuildableShipSizes;
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
		for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
			ar >> m_bResourceDistributor[res];
		ar >> m_iNeededSystems;
	}
}

USHORT CBuildingInfo::GetNeededResource(BYTE res) const
{
	switch (res)
	{
	case RESOURCES::TITAN:
		return m_iNeededTitan;
	case RESOURCES::DEUTERIUM:
		return m_iNeededDeuterium;
	case RESOURCES::DURANIUM:
		return m_iNeededDuranium;
	case RESOURCES::CRYSTAL:
		return m_iNeededCrystal;
	case RESOURCES::IRIDIUM:
		return m_iNeededIridium;
	case RESOURCES::DERITIUM:
		return m_iNeededDeritium;
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
	case RESOURCES::TITAN:		return GetTitanProd();
	case RESOURCES::DEUTERIUM: return GetDeuteriumProd();
	case RESOURCES::DURANIUM:	return GetDuraniumProd();
	case RESOURCES::CRYSTAL:	return GetCrystalProd();
	case RESOURCES::IRIDIUM:	return GetIridiumProd();
	case RESOURCES::DERITIUM:	return GetDeritiumProd();
	default: {
		MYTRACE("general")(MT::LEVEL_WARNING, "CBuildingInfo::GetResourceProd(): Resource %d doesn't exists!");
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
		s.AppendFormat("%s: %i\n",CLoc::GetString("FOOD"), this->GetFoodProd() * number);
	if (this->GetIPProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("INDUSTRY"), this->GetIPProd() * number);
	if (this->GetEnergyProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("ENERGY"), this->GetEnergyProd() * number);
	if (this->GetSPProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("SECURITY"), this->GetSPProd() * number);
	if (this->GetFPProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("RESEARCH"), this->GetFPProd() * number);
	if (this->GetTitanProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("TITAN"), this->GetTitanProd() * number);
	if (this->GetDeuteriumProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("DEUTERIUM"), this->GetDeuteriumProd() * number);
	if (this->GetDuraniumProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("DURANIUM"), this->GetDuraniumProd() * number);
	if (this->GetCrystalProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("CRYSTAL"), this->GetCrystalProd() * number);
	if (this->GetIridiumProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("IRIDIUM"), this->GetIridiumProd() * number);
	if (this->GetDeritiumProd() > 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("DERITIUM"), this->GetDeritiumProd() * number);
	if (this->GetCredits() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("CREDITS"), this->GetCredits() * number);
	if (this->GetMoralProd() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("MORAL"), this->GetMoralProd() * number);
	if (this->GetMoralProdEmpire() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("MORAL_EMPIREWIDE"), this->GetMoralProdEmpire() * number);

	// Ab hier die Boni
	if (this->GetFoodBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("FOOD_BONUS"), this->GetFoodBoni() * number);
	if (this->GetIndustryBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("INDUSTRY_BONUS"), this->GetIndustryBoni() * number);
	if (this->GetEnergyBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("ENERGY_BONUS"), this->GetEnergyBoni() * number);
	if (this->GetSecurityBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("SECURITY_BONUS"), this->GetSecurityBoni() * number);
	if (this->GetResearchBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("RESEARCH_BONUS"), this->GetResearchBoni() * number);
	if (this->GetTitanBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("TITAN_BONUS"), this->GetTitanBoni() * number);
	if (this->GetDeuteriumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("DEUTERIUM_BONUS"), this->GetDeuteriumBoni() * number);
	if (this->GetDuraniumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("DURANIUM_BONUS"), this->GetDuraniumBoni() * number);
	if (this->GetCrystalBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("CRYSTAL_BONUS"), this->GetCrystalBoni() * number);
	if (this->GetIridiumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("IRIDIUM_BONUS"), this->GetIridiumBoni() * number);
	if (this->GetDeritiumBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("DERITIUM_BONUS"), this->GetDeritiumBoni() * number);
	if (this->GetAllRessourcesBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("BONUS_TO_ALL_RES"), this->GetAllRessourcesBoni() * number);
	if (this->GetCreditsBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("CREDITS_BONUS"), this->GetCreditsBoni() * number);

	// Wenn alle Forschungsboni belegt sind, sprich Bonus auf alles
	if (this->GetBioTechBoni() != 0 && this->GetBioTechBoni() == this->GetEnergyTechBoni() && this->GetBioTechBoni() == this->GetCompTechBoni()
		&& this->GetBioTechBoni() == this->GetPropulsionTechBoni() && this->GetBioTechBoni() == this->GetConstructionTechBoni()
		&& this->GetBioTechBoni() == this->GetWeaponTechBoni())
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("TECHNIC_BONUS"), this->GetBioTechBoni() * number);
	else // bei einzelnen Gebieten
	{
		if (this->GetBioTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("BIOTECH_BONUS"), this->GetBioTechBoni() * number);
		if (this->GetEnergyTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("ENERGYTECH_BONUS"), this->GetEnergyTechBoni() * number);
		if (this->GetCompTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("COMPUTERTECH_BONUS"), this->GetCompTechBoni() * number);
		if (this->GetPropulsionTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("PROPULSIONTECH_BONUS"), this->GetPropulsionTechBoni() * number);
		if (this->GetConstructionTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("CONSTRUCTIONTECH_BONUS"), this->GetConstructionTechBoni() * number);
		if (this->GetWeaponTechBoni() > 0)
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("WEAPONTECH_BONUS"), this->GetWeaponTechBoni() * number);
	}
	// Wenn alle Geheimdienstdinger belegt sind -> Geheimdienst auf alles
	if (this->GetInnerSecurityBoni() != 0 && this->GetInnerSecurityBoni() == this->GetEconomySpyBoni()
		&& this->GetInnerSecurityBoni() ==	this->GetEconomySabotageBoni() && this->GetInnerSecurityBoni() == this->GetResearchSpyBoni()
		&& this->GetInnerSecurityBoni() == this->GetResearchSabotageBoni() && this->GetInnerSecurityBoni() == this->GetMilitarySpyBoni()
		&& this->GetInnerSecurityBoni() == this->GetMilitarySabotageBoni())
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("COMPLETE_SECURITY_BONUS"), this->GetInnerSecurityBoni() * number);
	else
	{
		if (this->GetEconomySpyBoni() != 0 && this->GetEconomySpyBoni() == this->GetResearchSpyBoni()
			&& this->GetEconomySpyBoni() == this->GetMilitarySpyBoni())
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("SPY_BONUS"), this->GetEconomySpyBoni() * number);
		else
		{
			if (this->GetEconomySpyBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CLoc::GetString("ECONOMY_SPY_BONUS"), this->GetEconomySpyBoni() * number);
			if (this->GetResearchSpyBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CLoc::GetString("RESEARCH_SPY_BONUS"), this->GetResearchSpyBoni() * number);
			if (this->GetMilitarySpyBoni() != 0)
				s.AppendFormat("%s: %i%%",CLoc::GetString("MILITARY_SPY_BONUS"), this->GetMilitarySpyBoni() * number);
		}
		if (this->GetEconomySabotageBoni() != 0 && this->GetEconomySabotageBoni() == this->GetResearchSabotageBoni()
			&& this->GetEconomySabotageBoni() == this->GetMilitarySabotageBoni())
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("SABOTAGE_BONUS"), this->GetEconomySabotageBoni() * number);
		else
		{
			if (this->GetEconomySabotageBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CLoc::GetString("ECONOMY_SABOTAGE_BONUS"), this->GetEconomySabotageBoni() * number);
			if (this->GetResearchSabotageBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CLoc::GetString("RESEARCH_SABOTAGE_BONUS"), this->GetResearchSabotageBoni() * number);
			if (this->GetMilitarySabotageBoni() != 0)
				s.AppendFormat("%s: %i%%\n",CLoc::GetString("MILITARY_SABOTAGE_BONUS"), this->GetMilitarySabotageBoni() * number);
		}
		if (this->GetInnerSecurityBoni() != 0)
			s.AppendFormat("%s: %i%%\n",CLoc::GetString("INNER_SECURITY_BONUS"), this->GetInnerSecurityBoni() * number);
	}
	// Ab hier zusätzliche Eigenschaften des Gebäudes
	if (this->GetShipYard() == TRUE)
	{
		s.AppendFormat("%s\n",CLoc::GetString("SHIPYARD"));
		CString ss;
		switch (this->GetMaxBuildableShipSize())
		{
			case 0: ss = CLoc::GetString("SMALL"); break;
			case 1: ss = CLoc::GetString("MIDDLE"); break;
			case 2: ss = CLoc::GetString("BIG"); break;
			case 3: ss = CLoc::GetString("HUGE"); break;
			default: ss = CLoc::GetString("HUGE");
		}
		s.AppendFormat("%s: %s\n",CLoc::GetString("MAX_SHIPSIZE"), ss);
	}
	if (this->GetShipYardSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("SHIPYARD_EFFICIENCY"), this->GetShipYardSpeed());
	if (this->GetBarrack() == TRUE)
		s.AppendFormat("%s\n",CLoc::GetString("BARRACK"));
	if (this->GetBarrackSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("BARRACK_EFFICIENCY"), this->GetBarrackSpeed());
	if (this->GetShieldPower() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("SHIELDPOWER"), this->GetShieldPower() * number);
	if (this->GetShieldPowerBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("SHIELDPOWER_BONUS"), this->GetShieldPowerBoni() * number);
	if (this->GetShipDefend() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("SHIPDEFEND"), this->GetShipDefend() * number);
	if (this->GetShipDefendBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("SHIPDEFEND_BONUS"), this->GetShipDefendBoni() * number);
	if (this->GetGroundDefend() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("GROUNDDEFEND"), this->GetGroundDefend() * number);
	if (this->GetGroundDefendBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("GROUNDDEFEND_BONUS"), this->GetGroundDefendBoni() * number);
	if (this->GetScanPower() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("SCANPOWER"), this->GetScanPower() * number);
	if (this->GetScanPowerBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("SCANPOWER_BONUS"), this->GetScanPowerBoni() * number);
	if (this->GetScanRange() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("SCANRANGE"), this->GetScanRange() * number);
	if (this->GetScanRangeBoni() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("SCANRANGE_BONUS"), this->GetScanRangeBoni() * number);
	if (this->GetShipTraining() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("SHIPTRAINING"), this->GetShipTraining() * number);
	if (this->GetTroopTraining() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("TROOPTRAINING"), this->GetTroopTraining() * number);
	if (this->GetResistance() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("RESISTANCE"), this->GetResistance() * number);
	if (this->GetAddedTradeRoutes() != 0)
		s.AppendFormat("%s: %i\n",CLoc::GetString("ADDED_TRADEROUTES"), this->GetAddedTradeRoutes() * number);
	if (this->GetIncomeOnTradeRoutes() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("INCOME_ON_TRADEROUTES"), this->GetIncomeOnTradeRoutes() * number);
	if (this->GetShipRecycling() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("SHIPRECYCLING"), this->GetShipRecycling() * number);
	if (this->GetBuildingBuildSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("BUILDING_BUILDSPEED"), this->GetBuildingBuildSpeed() * number);
	if (this->GetUpdateBuildSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("UPGRADE_BUILDSPEED"), this->GetUpdateBuildSpeed() * number);
	if (this->GetShipBuildSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("SHIP_BUILDSPEED"), this->GetShipBuildSpeed() * number);
	if (this->GetTroopBuildSpeed() != 0)
		s.AppendFormat("%s: %i%%\n",CLoc::GetString("TROOP_BUILDSPEED"), this->GetTroopBuildSpeed() * number);

	if (this->GetResourceDistributor(RESOURCES::TITAN))
		s.AppendFormat("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("TITAN"));
	if (this->GetResourceDistributor(RESOURCES::DEUTERIUM))
		s.AppendFormat("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("DEUTERIUM"));
	if (this->GetResourceDistributor(RESOURCES::DURANIUM))
		s.AppendFormat("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("DURANIUM"));
	if (this->GetResourceDistributor(RESOURCES::CRYSTAL))
		s.AppendFormat("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("CRYSTAL"));
	if (this->GetResourceDistributor(RESOURCES::IRIDIUM))
		s.AppendFormat("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("IRIDIUM"));
	if (this->GetResourceDistributor(RESOURCES::DERITIUM))
		s.AppendFormat("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("DERITIUM"));

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

bool CBuildingInfo::IsDefenseBuilding() const
{
	return
		m_iShieldPower > 0 ||
		m_iShieldPowerBoni > 0 ||
		m_iGroundDefend > 0 ||
		m_iGroundDefendBoni > 0 ||
		m_iShipDefend > 0 ||
		m_iShipDefendBoni > 0;
}

bool CBuildingInfo::IsDeritiumRefinery() const
{
	return m_iDeritium > 0;
}

static bool GreaterThanEqualToZero(int value)
{
	return value >= 0;
}

bool CBuildingInfo::IsAcceptableMinusMoral() const
{
	return m_iMoral < 0 &&
		GreaterThanEqualToZero(m_iAllRessourceBoni) &&
		GreaterThanEqualToZero(m_iMoralEmpire) &&
		GreaterThanEqualToZero(m_iFoodBoni) &&
		GreaterThanEqualToZero(m_iIndustryBoni) &&
		GreaterThanEqualToZero(m_iEnergyBoni) &&
		GreaterThanEqualToZero(m_iSecurityBoni) &&
		GreaterThanEqualToZero(m_iResearchBoni) &&
		GreaterThanEqualToZero(m_iTitanBoni) &&
		GreaterThanEqualToZero(m_iDeuteriumBoni) &&
		GreaterThanEqualToZero(m_iDuraniumBoni) &&
		GreaterThanEqualToZero(m_iCrystalBoni) &&
		GreaterThanEqualToZero(m_iIridiumBoni) &&
		GreaterThanEqualToZero(m_iDeritiumBoni) &&
		GreaterThanEqualToZero(m_iCreditsBoni) &&
		GreaterThanEqualToZero(m_iInnerSecurityBoni) &&
		GreaterThanEqualToZero(m_iEconomySpyBoni) &&
		GreaterThanEqualToZero(m_iEconomySabotageBoni) &&
		GreaterThanEqualToZero(m_iResearchSpyBoni) &&
		GreaterThanEqualToZero(m_iResearchSabotageBoni) &&
		GreaterThanEqualToZero(m_iMilitarySpyBoni) &&
		GreaterThanEqualToZero(m_iMilitarySabotageBoni) &&
		GreaterThanEqualToZero(m_iBioTechBoni) &&
		GreaterThanEqualToZero(m_iEnergyBoni) &&
		GreaterThanEqualToZero(m_iCompTechBoni) &&
		GreaterThanEqualToZero(m_iPropulsionTechBoni) &&
		GreaterThanEqualToZero(m_iConstructionTechBoni) &&
		GreaterThanEqualToZero(m_iWeaponTechBoni);
}

int CBuildingInfo::GetXProd(WORKER::Typ x) const
{
	switch(x)
	{
		case WORKER::FOOD_WORKER: return m_iFood;
		case WORKER::INDUSTRY_WORKER: return m_iIP;
		case WORKER::ENERGY_WORKER: return m_iEnergy;
		case WORKER::SECURITY_WORKER: return m_iSP;
		case WORKER::RESEARCH_WORKER: return m_iFP;
		case WORKER::TITAN_WORKER: return m_iTitan;
		case WORKER::DEUTERIUM_WORKER: return m_iDeuterium;
		case WORKER::DURANIUM_WORKER: return m_iDuranium;
		case WORKER::CRYSTAL_WORKER: return m_iCrystal;
		case WORKER::IRIDIUM_WORKER: return m_iIridium;
	}
	assert(false);
	return 0;
}
