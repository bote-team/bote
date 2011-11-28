// Ship.cpp: Implementierung der Klasse CShip.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ship.h"
#include "Fleet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CShip, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShip::CShip()
{
	m_TargetKO[0] = CPoint(-1,-1);
	m_iCrewExperiance = 0;
	m_nTerraformingPlanet = -1;
	m_bIsFlagShip = FALSE;
	m_bySpecial[0] = m_bySpecial[1] = NONE;
	m_Fleet = NULL;
	for (int i = TITAN; i <= DERITIUM; i++)
		m_iLoadedResources[i] = 0;
	m_bCloakOn = FALSE;
	m_nCombatTactic = COMBAT_TACTIC_ATTACK;
}

CShip::~CShip()
{
	if (m_Fleet)
	{		
		delete m_Fleet;
		m_Fleet = NULL;
	}
}


//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CShip::CShip(const CShip & rhs)
{
	m_Hull = rhs.m_Hull;
	m_Shield = rhs.m_Shield;
	m_TorpedoWeapons.RemoveAll();
	for (int i = 0; i < rhs.m_TorpedoWeapons.GetSize(); i++)
		m_TorpedoWeapons.Add(rhs.m_TorpedoWeapons.GetAt(i));
	m_BeamWeapons.RemoveAll();
	for (int i = 0; i < rhs.m_BeamWeapons.GetSize(); i++)
		m_BeamWeapons.Add(rhs.m_BeamWeapons.GetAt(i));
	
	// Zeiger auf Fleet speziell behandeln
	if (rhs.m_Fleet)
	{
		m_Fleet = new CFleet();
		*m_Fleet = *(rhs.m_Fleet);
	}
	else
		m_Fleet = NULL;
	
	//m_Fleet = rhs.&m_Fleet;
	m_iID = rhs.m_iID;
	m_KO = rhs.m_KO;
	for (int i=0;i<4;i++)
		m_TargetKO[i] = rhs.m_TargetKO[i];
	m_sOwnerOfShip = rhs.m_sOwnerOfShip;
	m_iMaintenanceCosts = rhs.m_iMaintenanceCosts;
	m_iShipType = rhs.m_iShipType;
	m_byShipSize = rhs.m_byShipSize;
	m_byManeuverability = rhs.m_byManeuverability;
	m_iSpeed = rhs.m_iSpeed;
	m_iRange = rhs.m_iRange;
	m_iScanPower = rhs.m_iScanPower;
	m_iScanRange = rhs.m_iScanRange;
	m_iCrewExperiance = rhs.m_iCrewExperiance;
	m_iStealthPower = rhs.m_iStealthPower;
	m_bCloakOn = rhs.m_bCloakOn;
	m_iStorageRoom = rhs.m_iStorageRoom;
	for (int i = TITAN; i <= DERITIUM; i++)
		m_iLoadedResources[i] = rhs.m_iLoadedResources[i];
	m_iColonizePoints = rhs.m_iColonizePoints;
	m_iStationBuildPoints = rhs.m_iStationBuildPoints;
	m_iCurrentOrder = rhs.m_iCurrentOrder;
	m_nTerraformingPlanet = rhs.m_nTerraformingPlanet;
	m_strShipName = rhs.m_strShipName;
	m_strShipDescription = rhs.m_strShipDescription;
	m_strShipClass = rhs.m_strShipClass;
	m_bIsFlagShip = rhs.m_bIsFlagShip;
	m_bySpecial[0] = rhs.m_bySpecial[0];
	m_bySpecial[1] = rhs.m_bySpecial[1];
	m_nCombatTactic = rhs.m_nCombatTactic;						
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CShip & CShip::operator=(const CShip & rhs)
{
	if (this == &rhs)
		return *this;
	
	m_Hull = rhs.m_Hull;
	m_Shield = rhs.m_Shield;
	m_TorpedoWeapons.RemoveAll();
	for (int i = 0; i < rhs.m_TorpedoWeapons.GetSize(); i++)
		m_TorpedoWeapons.Add(rhs.m_TorpedoWeapons.GetAt(i));
	m_BeamWeapons.RemoveAll();
	for (int i = 0; i < rhs.m_BeamWeapons.GetSize(); i++)
		m_BeamWeapons.Add(rhs.m_BeamWeapons.GetAt(i));
		
	// Zeiger auf Fleet speziell behandeln
	if (rhs.m_Fleet)
	{
		m_Fleet = new CFleet();
		*m_Fleet = *(rhs.m_Fleet);
	}
	else
		m_Fleet = NULL;
	
	m_iID = rhs.m_iID;
	m_KO = rhs.m_KO;
	for (int i=0;i<4;i++)
		m_TargetKO[i] = rhs.m_TargetKO[i];
	m_sOwnerOfShip = rhs.m_sOwnerOfShip;
	m_iMaintenanceCosts = rhs.m_iMaintenanceCosts;
	m_iShipType = rhs.m_iShipType;
	m_byShipSize = rhs.m_byShipSize;
	m_byManeuverability = rhs.m_byManeuverability;
	m_iSpeed = rhs.m_iSpeed;
	m_iRange = rhs.m_iRange;
	m_iScanPower = rhs.m_iScanPower;
	m_iScanRange = rhs.m_iScanRange;
	m_iCrewExperiance = rhs.m_iCrewExperiance;
	m_iStealthPower = rhs.m_iStealthPower;
	m_bCloakOn = rhs.m_bCloakOn;
	m_iStorageRoom = rhs.m_iStorageRoom;
	for (int i = TITAN; i <= DERITIUM; i++)
		m_iLoadedResources[i] = rhs.m_iLoadedResources[i];
	m_iColonizePoints = rhs.m_iColonizePoints;
	m_iStationBuildPoints = rhs.m_iStationBuildPoints;
	m_iCurrentOrder = rhs.m_iCurrentOrder;
	m_nTerraformingPlanet = rhs.m_nTerraformingPlanet;
	m_strShipName = rhs.m_strShipName;
	m_strShipDescription = rhs.m_strShipDescription;
	m_strShipClass = rhs.m_strShipClass;
	m_bIsFlagShip = rhs.m_bIsFlagShip;
	m_bySpecial[0] = rhs.m_bySpecial[0];
	m_bySpecial[1] = rhs.m_bySpecial[1];
	m_nCombatTactic = rhs.m_nCombatTactic;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Zugriffsfunktion
//////////////////////////////////////////////////////////////////////

// Funktion gibt den Schiffstyp als char* zurück
CString CShip::GetShipTypeAsString(BOOL plural) const
{
	CString shipType;
	if (plural == FALSE)
		switch (m_iShipType)
		{
		case TRANSPORTER: shipType = CResourceManager::GetString("TRANSPORTER"); break;
		case COLONYSHIP: shipType = CResourceManager::GetString("COLONIZESHIP"); break;
		case PROBE: shipType = CResourceManager::GetString("PROBE"); break;
		case SCOUT: shipType = CResourceManager::GetString("SCOUT"); break;
		case FIGHTER: shipType = CResourceManager::GetString("FIGHTER"); break;
		case FRIGATE: shipType = CResourceManager::GetString("FRIGATE"); break;
		case DESTROYER: shipType = CResourceManager::GetString("DESTROYER"); break;
		case CRUISER: shipType = CResourceManager::GetString("CRUISER"); break;
		case HEAVY_DESTROYER: shipType = CResourceManager::GetString("HEAVY_DESTROYER"); break;
		case HEAVY_CRUISER: shipType = CResourceManager::GetString("HEAVY_CRUISER"); break;
		case BATTLESHIP: shipType = CResourceManager::GetString("BATTLESHIP"); break;
		case DREADNOUGHT: shipType = CResourceManager::GetString("DREADNOUGHT"); break;
		case OUTPOST: shipType = CResourceManager::GetString("OUTPOST"); break;
		case STARBASE: shipType = CResourceManager::GetString("STARBASE"); break;
		case ALIEN: shipType = CResourceManager::GetString("ALIEN"); break;
		}
	else
		switch (m_iShipType)
		{
		case TRANSPORTER: shipType = CResourceManager::GetString("TRANSPORTERS"); break;
		case COLONYSHIP: shipType = CResourceManager::GetString("COLONIZESHIPS"); break;
		case PROBE: shipType = CResourceManager::GetString("PROBES"); break;
		case SCOUT: shipType = CResourceManager::GetString("SCOUTS"); break;
		case FIGHTER: shipType = CResourceManager::GetString("FIGHTERS"); break;
		case FRIGATE: shipType = CResourceManager::GetString("FRIGATES"); break;
		case DESTROYER: shipType = CResourceManager::GetString("DESTROYERS"); break;
		case CRUISER: shipType = CResourceManager::GetString("CRUISERS"); break;
		case HEAVY_DESTROYER: shipType = CResourceManager::GetString("HEAVY_DESTROYERS"); break;
		case HEAVY_CRUISER: shipType = CResourceManager::GetString("HEAVY_CRUISERS"); break;
		case BATTLESHIP: shipType = CResourceManager::GetString("BATTLESHIPS"); break;
		case DREADNOUGHT: shipType = CResourceManager::GetString("DREADNOUGHTS"); break;
		case OUTPOST: shipType = CResourceManager::GetString("OUTPOSTS"); break;
		case STARBASE: shipType = CResourceManager::GetString("STARBASES"); break;
		case ALIEN: shipType = CResourceManager::GetString("ALIENS"); break;
		}
	
	return shipType;
}

// Funktion gibt den aktuellen Schiffsauftrag als char* zurück
CString CShip::GetCurrentOrderAsString() const
{
	/*
	#define AVOID               0
	#define ATTACK              1
	#define CLOAK               2
	#define ATTACK_SYSTEM       3
	#define RAID_SYSTEM         4
	#define BLOCKADE_SYSTEM		5
	#define DESTROY_SHIP        6
	#define COLONIZE            7
	#define TERRAFORM           8
	#define BUILD_OUTPOST       9
	#define BUILD_STARBASE		10
	#define ASSIGN_FLAGSHIP     11
	#define CREATE_FLEET        12
	#define TRANSPORT			13
	#define FOLLOW_SHIP			14
	#define TRAIN_SHIP			15
	*/
	CString order;
	switch (m_iCurrentOrder)
	{
	case AVOID: order = CResourceManager::GetString("AVOID_ORDER"); break;
	case ATTACK: order = CResourceManager::GetString("ATTACK_ORDER"); break;
	case CLOAK: 
		if (m_bCloakOn)
			order = CResourceManager::GetString("DECLOAK_ORDER");
		else
			order = CResourceManager::GetString("CLOAK_ORDER"); break;
	case ATTACK_SYSTEM: order = CResourceManager::GetString("ATTACK_SYSTEM_ORDER"); break;
	case RAID_SYSTEM: order = CResourceManager::GetString("RAID_SYSTEM_ORDER"); break;
	case BLOCKADE_SYSTEM: order = CResourceManager::GetString("BLOCKADE_SYSTEM_ORDER"); break;
	case DESTROY_SHIP: order = CResourceManager::GetString("DESTROY_SHIP_ORDER"); break;
	case COLONIZE: order = CResourceManager::GetString("COLONIZE_ORDER"); break;
	case TERRAFORM: order = CResourceManager::GetString("TERRAFORM_ORDER"); break;
	case BUILD_OUTPOST: order = CResourceManager::GetString("BUILD_OUTPOST_ORDER"); break;
	case BUILD_STARBASE: order = CResourceManager::GetString("BUILD_STARBASE_ORDER"); break;
	case ASSIGN_FLAGSHIP: order = CResourceManager::GetString("ASSIGN_FLAGSHIP_ORDER"); break;
	case CREATE_FLEET: order = CResourceManager::GetString("CREATE_FLEET_ORDER"); break;
	case TRANSPORT: order = CResourceManager::GetString("TRANSPORT_ORDER"); break;
	case FOLLOW_SHIP: order = CResourceManager::GetString("FOLLOW_SHIP_ORDER"); break;
	case TRAIN_SHIP: order = CResourceManager::GetString("TRAIN_SHIP_ORDER"); break;
	default: order = "nothing"; break;
	}
	return order;
}

void CShip::CreateFleet()
{
	if (!m_Fleet)
		m_Fleet = new CFleet();
}

void CShip::CheckFleet()
{
	if (m_Fleet)
	{
		if (m_Fleet->GetFleetSize() == 0)
		{
			m_Fleet->DeleteFleet();
			delete m_Fleet;
			m_Fleet = NULL;
		}
	}
}

void CShip::DeleteFleet()
{
	if (m_Fleet)
	{
		m_Fleet->DeleteFleet();
		delete m_Fleet;
		m_Fleet = NULL;
	}
}

BOOLEAN CShip::HasSpecial(BYTE ability) const
{
	if (m_bySpecial[0] == ability || m_bySpecial[1] == ability)
		return TRUE;
	else
		return FALSE;
}

/// Funktion gibt die gesamte Offensivpower des Schiffes zurück, welches es in 100s anrichten würde.
/// Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum Vergleich heranzuziehen.
UINT CShip::GetCompleteOffensivePower() const
{
	UINT beamDmg	 = 0;
	UINT torpedoDmg  = 0;
	for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
	{
		short counter = 0;
		for (int j = 0; j < 100; j++)
		{
			if (counter == 0)
				counter = m_BeamWeapons.GetAt(i).GetBeamLenght() 
							+ m_BeamWeapons.GetAt(i).GetRechargeTime();
			if (counter > m_BeamWeapons.GetAt(i).GetRechargeTime())
			{
				UINT tempBeamDmg = (UINT)m_BeamWeapons.GetAt(i).GetBeamPower()
							* m_BeamWeapons.GetAt(i).GetBeamNumber()
							* m_BeamWeapons.GetAt(i).GetShootNumber();
				// besondere Beamfähigkeiten erhöhen den BeamDmg um einen selbst gewählten Mulitplikator
				// der dadurch erhaltende Schaden entspricht nicht dem wirklichen Schaden!
				if (m_BeamWeapons.GetAt(i).GetPiercing())
					tempBeamDmg = (UINT)(tempBeamDmg * 1.5);
				if (m_BeamWeapons.GetAt(i).GetModulating())
					tempBeamDmg *= 3;
				beamDmg += tempBeamDmg;
			}			
			counter--;
		}
	}
	beamDmg /= 3;
	for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
	{
		UINT tempTorpedoDmg = (UINT)(m_TorpedoWeapons.GetAt(i).GetTorpedoPower() *
									m_TorpedoWeapons.GetAt(i).GetNumber() * 100 *
									m_TorpedoWeapons.GetAt(i).GetNumberOfTupes() /
									m_TorpedoWeapons.GetAt(i).GetTupeFirerate());
		// besondere Torpedofähigkeiten erhöhen den Torpedoschaden um einen selbst gewählten Mulitplikator
		// der dadurch erhaltende Schaden entspricht nicht dem wirklichen Schaden!
		BYTE type = m_TorpedoWeapons.GetAt(i).GetTorpedoType();
		if (CTorpedoInfo::GetPenetrating(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 1.5);
		if (CTorpedoInfo::GetIgnoreAllShields(type))
			tempTorpedoDmg *= 3;
		if (CTorpedoInfo::GetCollapseShields(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 1.25);
		if (CTorpedoInfo::GetDoubleShieldDmg(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 1.5);
		if (CTorpedoInfo::GetDoubleHullDmg(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 2);
		if (CTorpedoInfo::GetReduceManeuver(type))
			tempTorpedoDmg = (UINT)(tempTorpedoDmg * 1.1);
		torpedoDmg += tempTorpedoDmg;
	}

	// Manövrierfähigkeit geht mit in den Wert ein
	double dMan = 1.0;
	if (m_iShipType != OUTPOST && m_iShipType != STARBASE)
		dMan = ((int)m_byManeuverability - 4.0) / 10.0 * 1.75 + 1.0;

	// Tarnung geht mit in den Wert ein
	double dCloak = 1.0;
	if (m_bCloakOn)
		dCloak = 1.25;
		
	return (UINT)((beamDmg + torpedoDmg) * dMan * dCloak);
}

/// Funktion gibt die gesamte Defensivstärke des Schiffes zurück. Dabei wird die maximale Hülle, die maximalen
/// Schilde und die Schildaufladezeit beachtet. Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum
/// Vergleich heranzuziehen.
UINT CShip::GetCompleteDefensivePower() const
{
	UINT def = m_Hull.GetMaxHull() + m_Shield.GetMaxShield()
		+ (m_Shield.GetMaxShield() / 300 + 2 * m_Shield.GetShieldType()) * 100;
	if (m_Hull.GetAblative())
		def = (UINT)(def * 1.1);
	if (m_Hull.GetPolarisation())
		def = (UINT)(def * 1.1);

	// Manövrierfähigkeit geht mit in den Wert ein
	double dMan = 1.0;
	if (m_iShipType != OUTPOST && m_iShipType != STARBASE)
		dMan = ((int)m_byManeuverability - 4.0) / 10.0 * 1.75 + 1.0;
		
	// Tarnung geht mit in den Wert ein
	double dCloak = 1.0;
	if (m_bCloakOn)
		dCloak = 1.25;

	return (UINT)(def * dMan * dCloak);
}

/// Funktion gibt das Erfahrungslevel des Schiffes zurück. Damit sind nicht die genauen Erfahrungspunkte gemeint, sondern das erreichte
/// Level aufgrund der Erfahrungspunkte.
/// @return Erfahrungstufe
BYTE CShip::GetExpLevel() const
{
	USHORT exp = m_iCrewExperiance;
	// Greenhorn
	if (exp < 250)
		return 0;
	// Angänger
	else if (exp < 500)
		return 1;
	// Normal
	else if (exp < 1000)
		return 2;
	// Profi
	else if (exp < 2500)
		return 3;
	// Veteran
	else if (exp < 5000)
		return 4;
	// Elite
	else if (exp < 10000)
		return 5;
	// Legende
	else
		return 6;
}

void CShip::SetCrewExperiance(int nAdd)
{
	// Sonden sammeln keine Erfahrung
	if (m_iShipType != PROBE)
		m_iCrewExperiance = min(64000, m_iCrewExperiance + nAdd);
}

