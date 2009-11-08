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
CBuildingInfo::CBuildingInfo(const CBuildingInfo & rhs) : CBuildingProd(rhs)
{
	m_iOwnerOfBuilding = rhs.m_iOwnerOfBuilding;
	m_strBuildingName[0] = rhs.m_strBuildingName[0];
	m_strBuildingName[1] = rhs.m_strBuildingName[1];
	m_strDescription[0] = rhs.m_strDescription[0];
	m_strDescription[1] = rhs.m_strDescription[1];
	m_bUpgradeable = rhs.m_bUpgradeable;
	m_strGraphikFileName = rhs.m_strGraphikFileName;
	m_iNeededSystems = rhs.m_iNeededSystems;
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
	CBuildingProd::operator =(rhs);

	m_iOwnerOfBuilding = rhs.m_iOwnerOfBuilding;
	m_strBuildingName[0] = rhs.m_strBuildingName[0];
	m_strBuildingName[1] = rhs.m_strBuildingName[1];
	m_strDescription[0] = rhs.m_strDescription[0];
	m_strDescription[1] = rhs.m_strDescription[1];
	m_bUpgradeable = rhs.m_bUpgradeable;
	m_strGraphikFileName = rhs.m_strGraphikFileName;
	m_iNeededSystems = rhs.m_iNeededSystems;
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
	CBuildingProd::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_iOwnerOfBuilding;
		ar << m_strBuildingName[0];
		ar << m_strDescription[0];
		ar << m_bUpgradeable;
		ar << m_strGraphikFileName;
		ar << m_iNeededSystems;
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
		ar >> m_strBuildingName[0];
		ar >> m_strDescription[0];
		ar >> m_bUpgradeable;
		ar >> m_strGraphikFileName;
		ar >> m_iNeededSystems;
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
