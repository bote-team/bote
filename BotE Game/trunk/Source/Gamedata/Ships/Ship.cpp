// Ship.cpp: Implementierung der Klasse CShip.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ship.h"
#include "Galaxy\Sector.h"
#include "Fleet.h"
#include "HTMLStringBuilder.h"
#include "GraphicPool.h"

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
	m_Path.RemoveAll();
	for (int i = 0; i < rhs.m_Path.GetSize(); i++)
		m_Path.Add(rhs.m_Path.GetAt(i));
	m_Troops.RemoveAll();
	for (int i = 0; i < rhs.m_Troops.GetSize(); i++)
		m_Troops.Add(rhs.m_Troops.GetAt(i));
	
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
	m_Path.RemoveAll();
	for (int i = 0; i < rhs.m_Path.GetSize(); i++)
		m_Path.Add(rhs.m_Path.GetAt(i));
	m_Troops.RemoveAll();
	for (int i = 0; i < rhs.m_Troops.GetSize(); i++)
		m_Troops.Add(rhs.m_Troops.GetAt(i));
	
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
// Serialisierungsfunktion
//////////////////////////////////////////////////////////////////////
void CShip::Serialize(CArchive &ar)
{
	__super::Serialize(ar);
	
	m_Hull.Serialize(ar);
	m_Shield.Serialize(ar);
	
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_Fleet;
		ar << m_iID;
		ar << m_KO;
		for (int i = 0; i < 4; i++)
			ar << m_TargetKO[i];
		ar << m_sOwnerOfShip;
		ar << m_iMaintenanceCosts;
		ar << m_iShipType;
		ar << m_byShipSize;
		ar << m_byManeuverability;
		ar << m_iSpeed;
		ar << m_iRange;
		ar << m_iScanPower;
		ar << m_iScanRange;
		ar << m_iCrewExperiance;
		ar << m_iStealthPower;
		ar << m_bCloakOn;
		ar << m_iStorageRoom;
		for (int i = TITAN; i <= DERITIUM; i++)
			ar << m_iLoadedResources[i];
		ar << m_iColonizePoints;
		ar << m_iStationBuildPoints;
		ar << m_iCurrentOrder;
		ar << m_bySpecial[0];
		ar << m_bySpecial[1];
		ar << m_nTerraformingPlanet;
		ar << m_strShipName;
		ar << m_strShipDescription;
		ar << m_strShipClass;
		ar << m_bIsFlagShip;
		ar << m_TorpedoWeapons.GetSize();
		for (int i = 0; i < m_TorpedoWeapons.GetSize(); i++)
			m_TorpedoWeapons.GetAt(i).Serialize(ar);
		ar << m_BeamWeapons.GetSize();
		for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
			m_BeamWeapons.GetAt(i).Serialize(ar);
		m_Path.Serialize(ar);
		ar << m_Troops.GetSize();
		for (int i = 0; i < m_Troops.GetSize(); i++)
			m_Troops.GetAt(i).Serialize(ar);
		int nTactic = (int)m_nCombatTactic;
		ar << nTactic;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		ar >> m_Fleet;
		ar >> m_iID;
		ar >> m_KO;
		for (int i = 0; i < 4; i++)
			ar >> m_TargetKO[i];
		ar >> m_sOwnerOfShip;
		ar >> m_iMaintenanceCosts;
		ar >> m_iShipType;
		ar >> m_byShipSize;
		ar >> m_byManeuverability;
		ar >> m_iSpeed;
		ar >> m_iRange;
		ar >> m_iScanPower;
		ar >> m_iScanRange;
		ar >> m_iCrewExperiance;
		ar >> m_iStealthPower;
		ar >> m_bCloakOn;
		ar >> m_iStorageRoom;
		for (int i = TITAN; i <= DERITIUM; i++)
			ar >> m_iLoadedResources[i];
		ar >> m_iColonizePoints;
		ar >> m_iStationBuildPoints;
		ar >> m_iCurrentOrder;
		ar >> m_bySpecial[0];
		ar >> m_bySpecial[1];
		ar >> m_nTerraformingPlanet;
		ar >> m_strShipName;
		ar >> m_strShipDescription;
		ar >> m_strShipClass;
		ar >> m_bIsFlagShip;
		ar >> number;
		m_TorpedoWeapons.RemoveAll();
		m_TorpedoWeapons.SetSize(number);
		for (int i = 0; i < number; i++)
			m_TorpedoWeapons.GetAt(i).Serialize(ar);
		ar >> number;
		m_BeamWeapons.RemoveAll();
		m_BeamWeapons.SetSize(number);
		for (int i = 0; i < number; i++)
			m_BeamWeapons.GetAt(i).Serialize(ar);		
		m_Path.Serialize(ar);
		ar >> number;
		m_Troops.RemoveAll();
		m_Troops.SetSize(number);
		for (int i = 0; i < number; i++)
			m_Troops.GetAt(i).Serialize(ar);
		if (VERSION >= 0.72)
		{
			int nTactic;
			ar >> nTactic;
			m_nCombatTactic = (COMBAT_TACTICS)nTactic;
		}
	}
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

//Funktion gibt aktuelles Ziel als char* zurück
CString CShip::GetCurrentTargetAsString() const
{
CString target;
if (m_TargetKO[0].x == -1) 
	target = "-";
else if (m_TargetKO[0].x == GetKO().x && m_TargetKO[0].y == GetKO().y )
	target = "-";
else 
target.Format("%c%i", (char)(m_TargetKO[0].y+97),m_TargetKO[0].x+1);
return target;
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
/// @param bBeams <code>true</code> wenn Beamwaffen beachtet werden sollen
/// @param bTorpedos <code>true</code> wenn Torpedowaffen beachtet werden sollen
/// @return Wert welcher die Offensivstärke des Schiffes angibt
UINT CShip::GetCompleteOffensivePower(bool bBeams/* = true*/, bool bTorpedos/* = true*/) const
{
	UINT beamDmg = 0;
	if (bBeams)
	{
		for (int i = 0; i < m_BeamWeapons.GetSize(); i++)
		{
			short counter = 0;
			double dTypeBonus = m_BeamWeapons.GetAt(i).GetBeamType() / 10.0;
			dTypeBonus *= (double)(100 + m_BeamWeapons.GetAt(i).GetBonus()) / 100.0;
			
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

			beamDmg = (UINT)((double)beamDmg * dTypeBonus);
		}
		beamDmg /= 2;
	}
	
	UINT torpedoDmg  = 0;
	if (bTorpedos)
	{
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
	}

	// Stationen bekommen einen Bonus, da sie keine Feuerwinkel beachten
	if (m_iShipType == OUTPOST || m_iShipType == STARBASE)
	{
		beamDmg = (UINT)(beamDmg * 1.5);
		torpedoDmg = (UINT)(torpedoDmg * 1.5);
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
/// @param bShields <code>true</code> wenn Schildstärken beachtet werden sollen
/// @param bHull <code>true</code> wenn Hüllenstärke beachtet werden sollen
/// @return Wert welcher die Defensivstärke des Schiffes angibt
UINT CShip::GetCompleteDefensivePower(bool bShields/* = true*/, bool bHull/* = true*/) const
{
	UINT def = 0;
	if (bHull)
	{
		def += m_Hull.GetMaxHull();
		if (m_Hull.GetAblative())
			def = (UINT)(def * 1.3);
		if (m_Hull.GetPolarisation())
			def = (UINT)(def * 1.3);
		// Hülle gilt doppelt soviel wie Schilde
		def *= 2;
	}

	if (bShields)
	{
		def += m_Shield.GetMaxShield() + (m_Shield.GetMaxShield() / 300 + 2 * m_Shield.GetShieldType()) * 100;
	}
	
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

/// Funktion gibt den schon benutzten Lagerraum im Schiff zurück.
USHORT CShip::GetUsedStorageRoom(const CArray<CTroopInfo>* troopInfo)
{
	USHORT usedStorage = 0;
	// benötigten Platz durch Truppen, welche schon im Schiff sind ermitteln
	for (int i = 0; i < this->GetTransportedTroops()->GetSize(); i++)
	{
		BYTE id = this->GetTransportedTroops()->GetAt(i).GetID();
		usedStorage += troopInfo->GetAt(id).GetSize();
	}		
	for (int i = TITAN; i <= DERITIUM; i++)
	{
		// Deritium wird im Verhältnis 1:250 gelagert, die anderen Ressourcen im Verhältnis 1:1
		if (i == DERITIUM)
			usedStorage += this->GetLoadedResources(i) * 250;
		else
			usedStorage += this->GetLoadedResources(i);
	}
	return usedStorage;
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

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @param bShowFleet wenn dieser Parameter <code>true</code> dann werden Informationen über die angeführte Flotte angezeigt, sonst nur über das Schiff
/// @return	der erstellte Tooltip-Text
CString CShip::GetTooltip(bool bShowFleet/*= true*/)
{
	CString sName = this->GetShipName();
	if (sName.IsEmpty() == false)
	{
		if (bShowFleet && this->GetFleet())
		{
			// Schiffsnamen holen und die ersten 4 Zeichen (z.B. USS_) und die lezten 2 Zeichen (z.B. _A) entfernen
			if (sName.GetLength() > 4 && sName.GetAt(3) == ' ')
				sName.Delete(0,4);
			if (sName.GetLength() > 2 && sName.ReverseFind(' ') == sName.GetLength() - 2)
				sName.Delete(sName.GetLength() - 2, 2);
			sName.Append(" " + CResourceManager::GetString("GROUP"));
		}
		sName = CHTMLStringBuilder::GetHTMLColor(sName);
		sName = CHTMLStringBuilder::GetHTMLHeader(sName, _T("h3"));
		sName = CHTMLStringBuilder::GetHTMLCenter(sName);
		sName += CHTMLStringBuilder::GetHTMLStringNewLine();
	}

	CString sType;
	if (bShowFleet && this->GetFleet() && this->GetFleet()->GetFleetShipType(this) == -1)
		sType = _T("(") + CResourceManager::GetString("MIXED_FLEET") + _T(")");
	else if (bShowFleet && this->GetFleet())
		sType = _T("(") + this->GetShipTypeAsString(TRUE) + _T(")");
	else
		sType = _T("(") + this->GetShipTypeAsString() + _T(")");		
	sType = CHTMLStringBuilder::GetHTMLColor(sType, _T("silver"));
	sType = CHTMLStringBuilder::GetHTMLHeader(sType, _T("h4"));
	sType = CHTMLStringBuilder::GetHTMLCenter(sType);
	sType += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	// Größenklasse und Unterhalt anzeigen	
	CString sCosts = "0";
	sCosts.Format("%s: %d    %s: %d", CResourceManager::GetString("SHIPSIZE"), this->GetShipSize(), CResourceManager::GetString("SHIPCOSTS"), this->GetMaintenanceCosts());
	sCosts = CHTMLStringBuilder::GetHTMLColor(sCosts);
	sCosts = CHTMLStringBuilder::GetHTMLHeader(sCosts, _T("h5"));
	sCosts = CHTMLStringBuilder::GetHTMLCenter(sCosts);
	sCosts += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	// Bewegung anzeigen
	CString sMovementHead = "";
	sMovementHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sMovementHead += CResourceManager::GetString("MOVEMENT");
	sMovementHead = CHTMLStringBuilder::GetHTMLColor(sMovementHead, _T("silver"));
	sMovementHead = CHTMLStringBuilder::GetHTMLHeader(sMovementHead, _T("h4"));
	sMovementHead = CHTMLStringBuilder::GetHTMLCenter(sMovementHead);
	sMovementHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sMovementHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sMovementHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	CString sMovement = CResourceManager::GetString("RANGE") + _T(": ");
	BYTE byRange = this->GetRange();
	if (bShowFleet && this->GetFleet())
		byRange = this->GetFleet()->GetFleetRange();
	if (byRange == RANGE_SHORT)
		sMovement += CResourceManager::GetString("SHORT");
	else if (byRange == RANGE_MIDDLE)
		sMovement += CResourceManager::GetString("MIDDLE");
	else if (byRange == RANGE_LONG)
		sMovement += CResourceManager::GetString("LONG");
	sMovement += CHTMLStringBuilder::GetHTMLStringNewLine();
	CString sSpeed;
	BYTE bySpeed = this->GetSpeed();
	if (bShowFleet && this->GetFleet())
		bySpeed = this->GetFleet()->GetFleetSpeed();
	sSpeed.Format("%s: %d\n", CResourceManager::GetString("SPEED"), bySpeed);
	sMovement += sSpeed;
		
	// wenn es eine Flotte ist keine weiteren Infos anzeigen
	if (bShowFleet && this->GetFleet())
	{
		sMovement = CHTMLStringBuilder::GetHTMLColor(sMovement);
		sMovement = CHTMLStringBuilder::GetHTMLHeader(sMovement, _T("h5"));
		sMovement = CHTMLStringBuilder::GetHTMLCenter(sMovement);
		return sName + sType + sMovementHead + sMovement;
	}

	sMovement += CResourceManager::GetString("MANEUVERABILITY") + _T(": ");
	switch (this->GetManeuverability())
	{
	case 9:	sMovement += CResourceManager::GetString("PHENOMENAL");	break;
	case 8:	sMovement += CResourceManager::GetString("EXCELLENT");	break;
	case 7:	sMovement += CResourceManager::GetString("VERYGOOD");	break;
	case 6:	sMovement += CResourceManager::GetString("GOOD");		break;
	case 5:	sMovement += CResourceManager::GetString("NORMAL");		break;
	case 4:	sMovement += CResourceManager::GetString("ADEQUATE");	break;
	case 3:	sMovement += CResourceManager::GetString("BAD");		break;
	case 2:	sMovement += CResourceManager::GetString("VERYBAD");	break;
	case 1:	sMovement += CResourceManager::GetString("MISERABLE");	break;
	default:sMovement += CResourceManager::GetString("NONE");
	}
	sMovement = CHTMLStringBuilder::GetHTMLColor(sMovement);
	sMovement = CHTMLStringBuilder::GetHTMLHeader(sMovement, _T("h5"));
	sMovement = CHTMLStringBuilder::GetHTMLCenter(sMovement);
	sMovement += CHTMLStringBuilder::GetHTMLStringNewLine();
	sMovement += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Bewaffnung anzeigen	
	UINT nOverallDmg = 0;		
	std::map<CString, int> mBeamWeapons;
	for (int i = 0; i < this->GetBeamWeapons()->GetSize(); i++)
	{
		CBeamWeapons* pBeam = &(this->GetBeamWeapons()->GetAt(i));
		CString s;
		s.Format("%s %d %s", CResourceManager::GetString("TYPE"), pBeam->GetBeamType(), pBeam->GetBeamName());
		// Waffen typenrein sammeln		
		mBeamWeapons[s] += pBeam->GetBeamNumber();		
				
		short counter = 0;
		for (int j = 0; j < 100; j++)
		{
			if (counter == 0)
				counter = pBeam->GetBeamLenght() + pBeam->GetRechargeTime();
			if (counter > pBeam->GetRechargeTime())
				nOverallDmg += (UINT)pBeam->GetBeamPower()	* pBeam->GetBeamNumber() * pBeam->GetShootNumber();				
			counter--;			
		}
	}
	
	// Beamwaffenbeschreibungstext zusammenbauen
	CString sBeams = "";
	for (map<CString, int>::const_iterator it = mBeamWeapons.begin(); it != mBeamWeapons.end(); ++it)
	{
		CString s;
		s.Format("%d x %s\n", it->second, it->first);
		sBeams += s;
	}

	if (sBeams.IsEmpty())
		sBeams = CResourceManager::GetString("NONE") + "\n";
	sBeams = CHTMLStringBuilder::GetHTMLColor(sBeams);
	sBeams = CHTMLStringBuilder::GetHTMLHeader(sBeams, _T("h5"));
	sBeams = CHTMLStringBuilder::GetHTMLCenter(sBeams);
	sBeams += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	CString sBeamWeaponHead;
	sBeamWeaponHead.Format("%s (%s: %d)", CResourceManager::GetString("BEAMWEAPONS"), CResourceManager::GetString("DAMAGE"), nOverallDmg);
	sBeamWeaponHead = CHTMLStringBuilder::GetHTMLColor(sBeamWeaponHead, _T("silver"));
	sBeamWeaponHead = CHTMLStringBuilder::GetHTMLHeader(sBeamWeaponHead, _T("h4"));
	sBeamWeaponHead = CHTMLStringBuilder::GetHTMLCenter(sBeamWeaponHead);
	sBeamWeaponHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sBeamWeaponHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sBeamWeaponHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	nOverallDmg = 0;
	std::map<CString, int> mTorpedoWeapons;
	for (int i = 0; i < this->GetTorpedoWeapons()->GetSize(); i++)
	{
		CTorpedoWeapons* pTorp = &(this->GetTorpedoWeapons()->GetAt(i));
		CString s;
		s.Format("%s (%s)", pTorp->GetTupeName(), pTorp->GetTorpedoName());
		mTorpedoWeapons[s] += m_TorpedoWeapons.GetAt(i).GetNumberOfTupes();
		nOverallDmg += (UINT)((pTorp->GetTorpedoPower() * pTorp->GetNumber() * 100 * pTorp->GetNumberOfTupes()) / pTorp->GetTupeFirerate());
	}

	CString sTorps = "";
	for (map<CString, int>::const_iterator it = mTorpedoWeapons.begin(); it != mTorpedoWeapons.end(); ++it)
	{
		CString s;
		s.Format("%d x %s\n", it->second, it->first);
		sTorps += s;
	}

	if (sTorps.IsEmpty())
		sTorps = CResourceManager::GetString("NONE") + "\n";
	sTorps = CHTMLStringBuilder::GetHTMLColor(sTorps);
	sTorps = CHTMLStringBuilder::GetHTMLHeader(sTorps, _T("h5"));
	sTorps = CHTMLStringBuilder::GetHTMLCenter(sTorps);
	sTorps += CHTMLStringBuilder::GetHTMLStringNewLine();
	CString sTupeWeaponHead;
	sTupeWeaponHead.Format("%s (%s: %d)", CResourceManager::GetString("TORPEDOWEAPONS"), CResourceManager::GetString("DAMAGE"), nOverallDmg);
	sTupeWeaponHead = CHTMLStringBuilder::GetHTMLColor(sTupeWeaponHead, _T("silver"));
	sTupeWeaponHead = CHTMLStringBuilder::GetHTMLHeader(sTupeWeaponHead, _T("h4"));
	sTupeWeaponHead = CHTMLStringBuilder::GetHTMLCenter(sTupeWeaponHead);
	sTupeWeaponHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sTupeWeaponHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sTupeWeaponHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sDefensiveHead = CResourceManager::GetString("SHIELDS")+" "+CResourceManager::GetString("AND")+" "+CResourceManager::GetString("HULL");
	sDefensiveHead = CHTMLStringBuilder::GetHTMLColor(sDefensiveHead, _T("silver"));
	sDefensiveHead = CHTMLStringBuilder::GetHTMLHeader(sDefensiveHead, _T("h4"));
	sDefensiveHead = CHTMLStringBuilder::GetHTMLCenter(sDefensiveHead);
	sDefensiveHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sDefensiveHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sDefensiveHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	CShield* pShield = this->GetShield();
	CString sShield;
	sShield.Format("%s %d %s: %s %d/%d", CResourceManager::GetString("TYPE"), pShield->GetShieldType(), CResourceManager::GetString("SHIELDS"), CResourceManager::GetString("CAPACITY"), (UINT)pShield->GetCurrentShield(), (UINT)pShield->GetMaxShield());
	sShield = CHTMLStringBuilder::GetHTMLColor(sShield);
	sShield = CHTMLStringBuilder::GetHTMLHeader(sShield, _T("h5"));
	sShield = CHTMLStringBuilder::GetHTMLCenter(sShield);
	sShield += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	CHull* pHull = this->GetHull();
	CString sMaterial;
	switch (pHull->GetHullMaterial())
	{
	case TITAN:		sMaterial = CResourceManager::GetString("TITAN");	 break;
	case DURANIUM:	sMaterial = CResourceManager::GetString("DURANIUM"); break;
	case IRIDIUM:	sMaterial = CResourceManager::GetString("IRIDIUM");	 break;
	default:		sMaterial = "";
	}
	CString sHull;
	if (pHull->GetDoubleHull() == TRUE)
		sHull.Format("%s%s: %s %d/%d", sMaterial, CResourceManager::GetString("DOUBLE_HULL_ARMOUR"), CResourceManager::GetString("INTEGRITY"), (int)pHull->GetCurrentHull(), (int)pHull->GetMaxHull());
	else
		sHull.Format("%s%s: %s %d/%d", sMaterial, CResourceManager::GetString("HULL_ARMOR"), CResourceManager::GetString("INTEGRITY"), (int)pHull->GetCurrentHull(), (int)pHull->GetMaxHull());		
	sHull = CHTMLStringBuilder::GetHTMLColor(sHull);
	sHull = CHTMLStringBuilder::GetHTMLHeader(sHull, _T("h5"));
	sHull = CHTMLStringBuilder::GetHTMLCenter(sHull);
	sHull += CHTMLStringBuilder::GetHTMLStringNewLine();
	sHull += CHTMLStringBuilder::GetHTMLStringNewLine();

	
	// Manövrierbarkeit anzeigen
	CString sManeuverHead = CResourceManager::GetString("MANEUVERABILITY");
	sManeuverHead = CHTMLStringBuilder::GetHTMLColor(sManeuverHead, _T("silver"));
	sManeuverHead = CHTMLStringBuilder::GetHTMLHeader(sManeuverHead, _T("h4"));
	sManeuverHead = CHTMLStringBuilder::GetHTMLCenter(sManeuverHead);
	sManeuverHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sManeuverHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sManeuverHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	CString sManeuver;
	switch (this->GetManeuverability())
	{
	case 9:	sManeuver = CResourceManager::GetString("PHENOMENAL");	break;
	case 8:	sManeuver = CResourceManager::GetString("EXCELLENT");	break;
	case 7:	sManeuver = CResourceManager::GetString("VERYGOOD");	break;
	case 6:	sManeuver = CResourceManager::GetString("GOOD");		break;
	case 5:	sManeuver = CResourceManager::GetString("NORMAL");		break;
	case 4:	sManeuver = CResourceManager::GetString("ADEQUATE");	break;
	case 3:	sManeuver = CResourceManager::GetString("BAD");			break;
	case 2:	sManeuver = CResourceManager::GetString("VERYBAD");		break;
	case 1:	sManeuver = CResourceManager::GetString("MISERABLE");	break;
	default:sManeuver = CResourceManager::GetString("NONE");
	}
	sManeuver = CHTMLStringBuilder::GetHTMLColor(sManeuver);
	sManeuver = CHTMLStringBuilder::GetHTMLHeader(sManeuver, _T("h5"));
	sManeuver = CHTMLStringBuilder::GetHTMLCenter(sManeuver);
	sManeuver += CHTMLStringBuilder::GetHTMLStringNewLine();
	sManeuver += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	// Scan anzeigen
	CString sScanHead = CResourceManager::GetString("SENSORS");
	sScanHead = CHTMLStringBuilder::GetHTMLColor(sScanHead, _T("silver"));
	sScanHead = CHTMLStringBuilder::GetHTMLHeader(sScanHead, _T("h4"));
	sScanHead = CHTMLStringBuilder::GetHTMLCenter(sScanHead);
	sScanHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sScanHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sScanHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	CString sScan;
	sScan.Format("%s: %d", CResourceManager::GetString("SCANRANGE"), this->GetScanRange());
	sScan = CHTMLStringBuilder::GetHTMLColor(sScan);
	sScan = CHTMLStringBuilder::GetHTMLHeader(sScan, _T("h5"));
	sScan = CHTMLStringBuilder::GetHTMLCenter(sScan);
	sScan += CHTMLStringBuilder::GetHTMLStringNewLine();
	CString sScanpower;
	sScanpower.Format("%s: %d", CResourceManager::GetString("SCANPOWER"), this->GetScanPower());
	sScanpower = CHTMLStringBuilder::GetHTMLColor(sScanpower);
	sScanpower = CHTMLStringBuilder::GetHTMLHeader(sScanpower, _T("h5"));
	sScanpower = CHTMLStringBuilder::GetHTMLCenter(sScanpower);
	sScanpower += CHTMLStringBuilder::GetHTMLStringNewLine();
	sScanpower += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Spezialfähigkeiten anzeigen
	CString sSpecialsHead = CResourceManager::GetString("SPECIAL_ABILITIES");
	sSpecialsHead = CHTMLStringBuilder::GetHTMLColor(sSpecialsHead, _T("silver"));
	sSpecialsHead = CHTMLStringBuilder::GetHTMLHeader(sSpecialsHead, _T("h4"));
	sSpecialsHead = CHTMLStringBuilder::GetHTMLCenter(sSpecialsHead);
	sSpecialsHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sSpecialsHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sSpecialsHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	
	CString sSpecials;
	if (this->HasSpecial(ASSULTSHIP))
		sSpecials += CResourceManager::GetString("ASSAULTSHIP") + "\n";
	if (this->HasSpecial(BLOCKADESHIP))
		sSpecials += CResourceManager::GetString("BLOCKADESHIP") + "\n";
	if (this->HasSpecial(COMMANDSHIP))
		sSpecials += CResourceManager::GetString("COMMANDSHIP") + "\n";
	if (this->HasSpecial(DOGFIGHTER))
		sSpecials += CResourceManager::GetString("DOGFIGHTER") + "\n";
	if (this->HasSpecial(DOGKILLER))
		sSpecials += CResourceManager::GetString("DOGKILLER") + "\n";
	if (this->HasSpecial(PATROLSHIP))
		sSpecials += CResourceManager::GetString("PATROLSHIP") + "\n";
	if (this->HasSpecial(RAIDER))
		sSpecials += CResourceManager::GetString("RAIDER") + "\n";
	if (this->HasSpecial(SCIENCEVESSEL))
		sSpecials += CResourceManager::GetString("SCIENCESHIP") + "\n";
	if (pShield->GetRegenerative())
		sSpecials += CResourceManager::GetString("REGENERATIVE_SHIELDS") + "\n";
	if (pHull->GetAblative())
		sSpecials += CResourceManager::GetString("ABLATIVE_ARMOR") + "\n";
	if  (pHull->GetPolarisation())
		sSpecials += CResourceManager::GetString("HULLPOLARISATION") + "\n";
	if (this->GetStealthPower() > 3)
		sSpecials += CResourceManager::GetString("CAN_CLOAK") + "\n";
	if (sSpecials.IsEmpty())
		sSpecials = CResourceManager::GetString("NONE") + "\n";;
	sSpecials = CHTMLStringBuilder::GetHTMLColor(sSpecials);
	sSpecials = CHTMLStringBuilder::GetHTMLHeader(sSpecials, _T("h5"));
	sSpecials = CHTMLStringBuilder::GetHTMLCenter(sSpecials);
	sSpecials += CHTMLStringBuilder::GetHTMLStringNewLine();
	sSpecials += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sSpecials += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sDesc = this->GetShipDescription();
	sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc);
	sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));

	CString sTip = sName + sType + sCosts + sMovementHead + sMovement + sBeamWeaponHead + sBeams + sTupeWeaponHead + sTorps + sDefensiveHead + sShield + sHull + sScanHead + sScan + sScanpower + sSpecialsHead + sSpecials + sDesc;
	return sTip;
}

void CShip::DrawShip(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt, bool bIsMarked, bool bOwnerUnknown, bool bDrawFleet, const Gdiplus::Color& clrNormal, const Gdiplus::Color& clrMark, const Gdiplus::Font& font) const
{
	StringFormat fontFormat;
	SolidBrush fontBrush(clrNormal);
	bDrawFleet = bDrawFleet && m_Fleet;

	// transparente Ellipse hinter markiertem Schiff zeichnen
	if (bIsMarked)
	{
		// Create a path that consists of a single ellipse.
		GraphicsPath path;
		path.AddEllipse(pt.x + 25, pt.y + 25, 230, 60);

		// Use the path to construct a brush.
		PathGradientBrush pthGrBrush(&path);

		// Set the color at the center of the path .
		pthGrBrush.SetCenterColor(Color(100,clrNormal.GetR(),clrNormal.GetG(),clrNormal.GetB()));

		// Set the color along the entire boundary of the path .
		Color colors[] = {Color(0, 0, 0, 0)};
		int count = 1;
		pthGrBrush.SetSurroundColors(colors, &count);
		g->FillEllipse(&pthGrBrush, pt.x + 25, pt.y + 25, 230, 60);
	}
	
	CString s;
	// ist der Besitzer des Schiffes unbekannt?						
	if (bOwnerUnknown)
		s = _T("Ships\\Unknown.bop");
	else
		s.Format("Ships\\%s.bop", m_strShipClass);

	Bitmap* graphic = pGraphicPool->GetGDIGraphic(s);
	if (graphic == NULL)
		graphic = pGraphicPool->GetGDIGraphic("Ships\\ImageMissing.bop");
	if (graphic)
		g->DrawImage(graphic, pt.x + 37, pt.y + 30, 65, 49);
	
	// Erfahrungsstufen des Schiffes anzeigen
	switch (this->GetExpLevel())
	{
		case 1: graphic = pGraphicPool->GetGDIGraphic("Other\\xp_beginner.bop");break;
		case 2: graphic = pGraphicPool->GetGDIGraphic("Other\\xp_normal.bop");	break;
		case 3: graphic = pGraphicPool->GetGDIGraphic("Other\\xp_profi.bop");	break;
		case 4: graphic = pGraphicPool->GetGDIGraphic("Other\\xp_veteran.bop");	break;
		case 5: graphic = pGraphicPool->GetGDIGraphic("Other\\xp_elite.bop");	break;
		case 6: graphic = pGraphicPool->GetGDIGraphic("Other\\xp_legend.bop");	break;
		default: graphic = NULL;
	}
	if (graphic)
		g->DrawImage(graphic, pt.x + 29, pt.y + 79 - graphic->GetHeight(), 8, graphic->GetHeight());
	
	// Hier die Striche für Schilde und Hülle neben dem Schiffsbild anzeigen (jeweils max. 20)
	int nHullPercent = m_Hull.GetCurrentHull() * 20 / m_Hull.GetMaxHull();
	Gdiplus::Pen pen(Color(240 - nHullPercent * 12, 0 + nHullPercent * 12, 0));
	RectF* rects = new RectF[nHullPercent + 1];
	for (int n = 0; n <= nHullPercent; n++)
		rects[n] = RectF((REAL)pt.x + 102, (REAL)pt.y + 75 - n * 2, 5, 0.5);
	g->DrawRectangles(&pen, rects, nHullPercent + 1);
	delete[] rects;
		
	int nShieldPercent = 0;
	if (m_Shield.GetMaxShield() > 0)
		nShieldPercent = m_Shield.GetCurrentShield() * 20 / m_Shield.GetMaxShield();
	pen.SetColor(Color(240 - nShieldPercent * 12, 80, 0 + nShieldPercent * 12));
	if (nShieldPercent > 0)
	{
		rects = new RectF[nShieldPercent + 1];
		for (int n = 0; n <= nShieldPercent; n++)
			rects[n] = RectF((REAL)pt.x + 109, (REAL)pt.y + 75 - n * 2, 5, 0.5);
		g->DrawRectangles(&pen, rects, nShieldPercent + 1);
		delete[] rects;
	}
	
	// Wenn es das Flagschiff unseres Imperiums ist, dann kleines Zeichen zeichnen
	if (m_bIsFlagShip)
	{		
		graphic = pGraphicPool->GetGDIGraphic("Symbols\\" + m_sOwnerOfShip + ".bop");
		if (graphic)
			g->DrawImage(graphic, pt.x + 37, pt.y + 30, 25, 25);
	}

	// Wenn des Schiff Truppen transportiert, dann kleines Truppensymbol zeichnen
	// Symbole zu Truppen zeichnen
	bool bDrawTroopSymbol = false;
	if (m_Troops.GetSize())
		bDrawTroopSymbol = true;
	// prüfen ob ein Schiff in der Flotte Truppen hat
	else if (bDrawFleet)
		for (int i = 0; i < m_Fleet->GetFleetSize(); i++)
			if (m_Fleet->GetShipFromFleet(i)->GetTransportedTroops()->GetSize())
			{
				bDrawTroopSymbol = true;
				break;
			}

	if (bDrawTroopSymbol)
	{
		graphic = pGraphicPool->GetGDIGraphic("Other\\troopSmall.bop");
		if (graphic)
			g->DrawImage(graphic, pt.x + 75, pt.y + 50, 25, 25);
	}

	if (!bOwnerUnknown)
	{	
		if (bIsMarked)
			fontBrush.SetColor(clrMark);
		// Wenn das Schiff getarnt ist die Schrift etwas dunkler darstellen
		if (m_bCloakOn)
		{
			Color clrCurrent;
			fontBrush.GetColor(&clrCurrent);
			Gdiplus::Color clrCloaked(50, clrCurrent.GetR(), clrCurrent.GetG(), clrCurrent.GetB());	
			fontBrush.SetColor(clrCloaked);
			// kleines Icon für aktivierte Tarnung zeichnen
			graphic = pGraphicPool->GetGDIGraphic("Other\\cloakedSmall.bop");
			if (graphic)
				g->DrawImage(graphic, pt.x + 37, pt.y + 55, graphic->GetWidth(), graphic->GetHeight());
		}

		// normale Infos zum Schiff sollen angezeigt werden
		if (!bDrawFleet)
		{
			if (m_iShipType != ALIEN)
			{
				g->DrawString(m_strShipName.AllocSysString(), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 37), &fontFormat, &fontBrush);
				s = m_strShipClass + "-" + CResourceManager::GetString("CLASS");
				g->DrawString(s.AllocSysString(), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 57), &fontFormat, &fontBrush);
			}
			else
			{
				g->DrawString(m_strShipClass.AllocSysString(), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 37), &fontFormat, &fontBrush);
			}
		}
		// Schiff und dessen Flotteninfos sollen angezeigt werden
		else
		{
			// Schiffsnamen holen und die ersten 4 Zeichen (z.B. USS_) und die lezten 2 Zeichen (z.B. _A) entfernen
			s.Format("%s", m_strShipName);
			if (s.GetLength() > 4 && s.GetAt(3) == ' ')
				s.Delete(0,4);
			if (s.GetLength() > 2 && s.ReverseFind(' ') == s.GetLength() - 2)
				s.Delete(s.GetLength() - 2, 2);
		
			s.Append(" " + CResourceManager::GetString("GROUP"));
			// Hier jetzt Namen und Schiffstype zur Flotte
			g->DrawString(s.AllocSysString(), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 37), &fontFormat, &fontBrush);
			
			if (m_Fleet->GetFleetShipType(this) == -1)
				g->DrawString(CResourceManager::GetString("MIXED_FLEET").AllocSysString(), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 57), &fontFormat, &fontBrush);
			else
				g->DrawString(this->GetShipTypeAsString(TRUE).AllocSysString(), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 57), &fontFormat, &fontBrush);
		}
	}

	if (bDrawFleet)
	{
		// Anzahl der Schiffe in der Flotte (+1 weil das Führerschiff mitgezählt werden muß)
		fontBrush.SetColor(Color::White);
		s.Format("%d", m_Fleet->GetFleetSize() + 1);
		g->DrawString(s.AllocSysString(), -1, &font, PointF((REAL)pt.x + 35, (REAL)pt.y + 30), &fontFormat, &fontBrush);
	}	
}