// Ship.cpp: Implementierung der Klasse CShip.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ship.h"
#include "Galaxy\Sector.h"
#include "Fleet.h"
#include "HTMLStringBuilder.h"
#include "GraphicPool.h"

#include <cassert>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CShip, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShip::CShip() :
	m_KO(-1, -1),
	m_iID(0),
	m_iMaintenanceCosts(0),
	m_byManeuverability(0),
	m_iSpeed(0),
	m_iScanPower(0),
	m_iScanRange(0),
	m_iStealthPower(0),
	m_iStorageRoom(0),
	m_iColonizePoints(0),
	m_iStationBuildPoints(0)
{
	unsigned length = sizeof(m_TargetKO) / sizeof(m_TargetKO[0]);
	for(unsigned i = 0; i < length; ++i) {
		m_TargetKO[i] = CPoint(-1, -1);
	}
	m_iCrewExperiance = 0;
	m_nTerraformingPlanet = -1;
	m_bIsFlagShip = FALSE;
	m_nSpecial[0] = m_nSpecial[1] = SHIP_SPECIAL::NONE;
	m_Fleet = NULL;
	for (int i = TITAN; i <= DERITIUM; i++)
		m_iLoadedResources[i] = 0;
	m_bCloakOn = false;
	m_nCombatTactic = COMBAT_TACTIC::CT_ATTACK;
	m_nAlienType = ALIEN_TYPE::NONE;
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
CShip::CShip(const CShip & rhs) :
	m_Hull(rhs.m_Hull),
	m_Shield(rhs.m_Shield),
	m_KO(rhs.m_KO),
	m_sOwnerOfShip(rhs.m_sOwnerOfShip),
	m_strShipName(rhs.m_strShipName),
	m_strShipClass(rhs.m_strShipClass)
{
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
	for (int i=0;i<4;i++)
		m_TargetKO[i] = rhs.m_TargetKO[i];
	m_iMaintenanceCosts = rhs.m_iMaintenanceCosts;
	m_iShipType = rhs.m_iShipType;
	m_nShipSize = rhs.m_nShipSize;
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
	m_strShipDescription = rhs.m_strShipDescription;
	m_bIsFlagShip = rhs.m_bIsFlagShip;
	m_nSpecial[0] = rhs.m_nSpecial[0];
	m_nSpecial[1] = rhs.m_nSpecial[1];
	m_nCombatTactic = rhs.m_nCombatTactic;
	m_nAlienType = rhs.m_nAlienType;
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
	m_nShipSize = rhs.m_nShipSize;
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
	m_nSpecial[0] = rhs.m_nSpecial[0];
	m_nSpecial[1] = rhs.m_nSpecial[1];
	m_nCombatTactic = rhs.m_nCombatTactic;
	m_nAlienType = rhs.m_nAlienType;

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
		ar << m_nShipSize;
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
		ar << m_nSpecial[0];
		ar << m_nSpecial[1];
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
		ar << m_nCombatTactic;
		ar << m_nAlienType;
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
		int nType;
		ar >> nType;
		m_iShipType = (SHIP_TYPE::Typ)nType;
		int nSize;
		ar >> nSize;
		m_nShipSize = (SHIP_SIZE::Typ)nSize;
		ar >> m_byManeuverability;
		ar >> m_iSpeed;
		int nRange;
		ar >> nRange;
		m_iRange = (SHIP_RANGE::Typ)nRange;
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
		int nOrder;
		ar >> nOrder;
		m_iCurrentOrder = (SHIP_ORDER::Typ)nOrder;
		int nSpecial;
		ar >> nSpecial;
		m_nSpecial[0] = (SHIP_SPECIAL::Typ)nSpecial;
		ar >> nSpecial;
		m_nSpecial[1] = (SHIP_SPECIAL::Typ)nSpecial;
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
			m_nCombatTactic = (COMBAT_TACTIC::Typ)nTactic;
		}
		int nAlienType;
		ar >> nAlienType;
		m_nAlienType = (ALIEN_TYPE::Typ)nAlienType;
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
	{
		switch (m_iShipType)
		{
		case SHIP_TYPE::TRANSPORTER: shipType = CResourceManager::GetString("TRANSPORTER"); break;
		case SHIP_TYPE::COLONYSHIP: shipType = CResourceManager::GetString("COLONIZESHIP"); break;
		case SHIP_TYPE::PROBE: shipType = CResourceManager::GetString("PROBE"); break;
		case SHIP_TYPE::SCOUT: shipType = CResourceManager::GetString("SCOUT"); break;
		case SHIP_TYPE::FIGHTER: shipType = CResourceManager::GetString("FIGHTER"); break;
		case SHIP_TYPE::FRIGATE: shipType = CResourceManager::GetString("FRIGATE"); break;
		case SHIP_TYPE::DESTROYER: shipType = CResourceManager::GetString("DESTROYER"); break;
		case SHIP_TYPE::CRUISER: shipType = CResourceManager::GetString("CRUISER"); break;
		case SHIP_TYPE::HEAVY_DESTROYER: shipType = CResourceManager::GetString("HEAVY_DESTROYER"); break;
		case SHIP_TYPE::HEAVY_CRUISER: shipType = CResourceManager::GetString("HEAVY_CRUISER"); break;
		case SHIP_TYPE::BATTLESHIP: shipType = CResourceManager::GetString("BATTLESHIP"); break;
		case SHIP_TYPE::DREADNOUGHT: shipType = CResourceManager::GetString("DREADNOUGHT"); break;
		case SHIP_TYPE::OUTPOST: shipType = CResourceManager::GetString("OUTPOST"); break;
		case SHIP_TYPE::STARBASE: shipType = CResourceManager::GetString("STARBASE"); break;
		case SHIP_TYPE::ALIEN: shipType = CResourceManager::GetString("ALIEN"); break;
		}
	}
	else
	{
		switch (m_iShipType)
		{
		case SHIP_TYPE::TRANSPORTER: shipType = CResourceManager::GetString("TRANSPORTERS"); break;
		case SHIP_TYPE::COLONYSHIP: shipType = CResourceManager::GetString("COLONIZESHIPS"); break;
		case SHIP_TYPE::PROBE: shipType = CResourceManager::GetString("PROBES"); break;
		case SHIP_TYPE::SCOUT: shipType = CResourceManager::GetString("SCOUTS"); break;
		case SHIP_TYPE::FIGHTER: shipType = CResourceManager::GetString("FIGHTERS"); break;
		case SHIP_TYPE::FRIGATE: shipType = CResourceManager::GetString("FRIGATES"); break;
		case SHIP_TYPE::DESTROYER: shipType = CResourceManager::GetString("DESTROYERS"); break;
		case SHIP_TYPE::CRUISER: shipType = CResourceManager::GetString("CRUISERS"); break;
		case SHIP_TYPE::HEAVY_DESTROYER: shipType = CResourceManager::GetString("HEAVY_DESTROYERS"); break;
		case SHIP_TYPE::HEAVY_CRUISER: shipType = CResourceManager::GetString("HEAVY_CRUISERS"); break;
		case SHIP_TYPE::BATTLESHIP: shipType = CResourceManager::GetString("BATTLESHIPS"); break;
		case SHIP_TYPE::DREADNOUGHT: shipType = CResourceManager::GetString("DREADNOUGHTS"); break;
		case SHIP_TYPE::OUTPOST: shipType = CResourceManager::GetString("OUTPOSTS"); break;
		case SHIP_TYPE::STARBASE: shipType = CResourceManager::GetString("STARBASES"); break;
		case SHIP_TYPE::ALIEN: shipType = CResourceManager::GetString("ALIENS"); break;
		}
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
	WAIT_SHIP_ORDER				16
	SENTRY_SHIP_ORDER			17
	REPAIR						18
	*/
	CString order;
	switch (m_iCurrentOrder)
	{
	case SHIP_ORDER::AVOID: order = CResourceManager::GetString("AVOID_ORDER"); break;
	case SHIP_ORDER::ATTACK: order = CResourceManager::GetString("ATTACK_ORDER"); break;
	case SHIP_ORDER::CLOAK:
		if (m_bCloakOn)
			order = CResourceManager::GetString("DECLOAK_ORDER");
		else
			order = CResourceManager::GetString("CLOAK_ORDER"); break;
	case SHIP_ORDER::ATTACK_SYSTEM: order = CResourceManager::GetString("ATTACK_SYSTEM_ORDER"); break;
	case SHIP_ORDER::RAID_SYSTEM: order = CResourceManager::GetString("RAID_SYSTEM_ORDER"); break;
	case SHIP_ORDER::BLOCKADE_SYSTEM: order = CResourceManager::GetString("BLOCKADE_SYSTEM_ORDER"); break;
	case SHIP_ORDER::DESTROY_SHIP: order = CResourceManager::GetString("DESTROY_SHIP_ORDER"); break;
	case SHIP_ORDER::COLONIZE: order = CResourceManager::GetString("COLONIZE_ORDER"); break;
	case SHIP_ORDER::TERRAFORM: order = CResourceManager::GetString("TERRAFORM_ORDER"); break;
	case SHIP_ORDER::BUILD_OUTPOST: order = CResourceManager::GetString("BUILD_OUTPOST_ORDER"); break;
	case SHIP_ORDER::BUILD_STARBASE: order = CResourceManager::GetString("BUILD_STARBASE_ORDER"); break;
	case SHIP_ORDER::ASSIGN_FLAGSHIP: order = CResourceManager::GetString("ASSIGN_FLAGSHIP_ORDER"); break;
	case SHIP_ORDER::CREATE_FLEET: order = CResourceManager::GetString("CREATE_FLEET_ORDER"); break;
	case SHIP_ORDER::TRANSPORT: order = CResourceManager::GetString("TRANSPORT_ORDER"); break;
	case SHIP_ORDER::FOLLOW_SHIP: order = CResourceManager::GetString("FOLLOW_SHIP_ORDER"); break;
	case SHIP_ORDER::TRAIN_SHIP: order = CResourceManager::GetString("TRAIN_SHIP_ORDER"); break;
	case SHIP_ORDER::WAIT_SHIP_ORDER: order = CResourceManager::GetString("WAIT_SHIP_ORDER"); break;
	case SHIP_ORDER::SENTRY_SHIP_ORDER: order = CResourceManager::GetString("SENTRY_SHIP_ORDER"); break;
	case SHIP_ORDER::REPAIR: order = CResourceManager::GetString("REPAIR_SHIP_ORDER"); break;
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

void CShip::AdoptOrdersFrom(const CShip& ship, const bool also_flagship_transport)
{
	SHIP_ORDER::Typ order_to_adopt = ship.GetCurrentOrder();

		if (order_to_adopt != SHIP_ORDER::ASSIGN_FLAGSHIP && order_to_adopt != SHIP_ORDER::TRANSPORT || also_flagship_transport)
		{
			SetCurrentOrder(order_to_adopt);
		}
		SetKO(ship.GetKO());

		const CPoint& tko = ship.GetTargetKO();
		if (GetTargetKO() != tko)
			SetTargetKO(tko,0,true);

		//den Terraformingplaneten neu setzen
		SetTerraformingPlanet(ship.GetTerraformingPlanet());
}

void CShip::AddShipToFleet(CShip& ship)
{
	assert(m_Fleet);
	ship.AdoptOrdersFrom(*this, true);
	m_Fleet->AddShipToFleet(&ship);
}

bool CShip::HasSpecial(SHIP_SPECIAL::Typ nAbility) const
{
	if (m_nSpecial[0] == nAbility || m_nSpecial[1] == nAbility)
		return true;
	else
		return false;
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
	if (m_iShipType == SHIP_TYPE::OUTPOST || m_iShipType == SHIP_TYPE::STARBASE)
	{
		beamDmg = (UINT)(beamDmg * 1.5);
		torpedoDmg = (UINT)(torpedoDmg * 1.5);
	}

	// Manövrierfähigkeit geht mit in den Wert ein
	double dMan = 1.0;
	if (m_iShipType != SHIP_TYPE::OUTPOST && m_iShipType != SHIP_TYPE::STARBASE)
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
	if (m_iShipType != SHIP_TYPE::OUTPOST && m_iShipType != SHIP_TYPE::STARBASE)
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
	// Sonden und Aliens sammeln keine Erfahrung
	if (m_iShipType != SHIP_TYPE::PROBE && m_iShipType != SHIP_TYPE::ALIEN)
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
	SHIP_RANGE::Typ nRange = this->GetRange();
	if (bShowFleet && this->GetFleet())
		nRange = this->GetFleet()->GetFleetRange();
	if (nRange == SHIP_RANGE::SHORT)
		sMovement += CResourceManager::GetString("SHORT");
	else if (nRange == SHIP_RANGE::MIDDLE)
		sMovement += CResourceManager::GetString("MIDDLE");
	else if (nRange == SHIP_RANGE::LONG)
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
	if (this->HasSpecial(SHIP_SPECIAL::ASSULTSHIP))
		sSpecials += CResourceManager::GetString("ASSAULTSHIP") + "\n";
	if (this->HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
		sSpecials += CResourceManager::GetString("BLOCKADESHIP") + "\n";
	if (this->HasSpecial(SHIP_SPECIAL::COMMANDSHIP))
		sSpecials += CResourceManager::GetString("COMMANDSHIP") + "\n";
	if (this->HasSpecial(SHIP_SPECIAL::DOGFIGHTER))
		sSpecials += CResourceManager::GetString("DOGFIGHTER") + "\n";
	if (this->HasSpecial(SHIP_SPECIAL::DOGKILLER))
		sSpecials += CResourceManager::GetString("DOGKILLER") + "\n";
	if (this->HasSpecial(SHIP_SPECIAL::PATROLSHIP))
		sSpecials += CResourceManager::GetString("PATROLSHIP") + "\n";
	if (this->HasSpecial(SHIP_SPECIAL::RAIDER))
		sSpecials += CResourceManager::GetString("RAIDER") + "\n";
	if (this->HasSpecial(SHIP_SPECIAL::SCIENCEVESSEL))
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
			if (m_iShipType != SHIP_TYPE::ALIEN)
			{
				g->DrawString(CComBSTR(m_strShipName), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 37), &fontFormat, &fontBrush);
				s = m_strShipClass + "-" + CResourceManager::GetString("CLASS");
				g->DrawString(CComBSTR(s), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 57), &fontFormat, &fontBrush);
			}
			else
			{
				g->DrawString(CComBSTR(m_strShipClass), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 37), &fontFormat, &fontBrush);
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
			g->DrawString(CComBSTR(s), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 37), &fontFormat, &fontBrush);

			if (m_Fleet->GetFleetShipType(this) == -1)
				g->DrawString(CComBSTR(CResourceManager::GetString("MIXED_FLEET")), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 57), &fontFormat, &fontBrush);
			else
				g->DrawString(CComBSTR(this->GetShipTypeAsString(TRUE)), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 57), &fontFormat, &fontBrush);
		}
	}

	if (bDrawFleet)
	{
		// Anzahl der Schiffe in der Flotte (+1 weil das Führerschiff mitgezählt werden muß)
		fontBrush.SetColor(Color::White);
		s.Format("%d", m_Fleet->GetFleetSize() + 1);
		g->DrawString(CComBSTR(s), -1, &font, PointF((REAL)pt.x + 35, (REAL)pt.y + 30), &fontFormat, &fontBrush);
	}
}

void CShip::SetTargetKO(const CPoint& TargetKO, int Index, const bool simple_setter)
{
	m_TargetKO[Index] = TargetKO;
	if(simple_setter)
		return;
	if (m_iCurrentOrder > SHIP_ORDER::AVOID) {
		UnsetCurrentOrder();
	}
	m_nTerraformingPlanet = -1;
}

bool CShip::HasNothingToDo() const {
	return (m_iCurrentOrder == SHIP_ORDER::AVOID || m_iCurrentOrder == SHIP_ORDER::ATTACK)
		&& (GetTargetKO() == GetKO() || GetTargetKO() == CPoint(-1, -1)) && m_iShipType != SHIP_TYPE::OUTPOST && m_iShipType != SHIP_TYPE::STARBASE;
}

bool CShip::NeedsRepair() const {
	if(m_Fleet) {
		const unsigned size = m_Fleet->GetFleetSize();
		for(unsigned i = 0; i < size; ++i) {
			const CHull& hull = *m_Fleet->GetShipFromFleet(i)->GetHull();
			if(hull.GetCurrentHull() < hull.GetMaxHull())
				return true;
		}
	}
	return m_Hull.GetCurrentHull() < m_Hull.GetMaxHull();
}

void CShip::UnsetCurrentOrder() {
	m_iCurrentOrder = IsNonCombat() ? SHIP_ORDER::AVOID : SHIP_ORDER::ATTACK;
}

void CShip::Repair(BOOL bAtShipPort, bool bFasterShieldRecharge) {
	m_Shield.RechargeShields(200 * (bFasterShieldRecharge + 1));
	if (m_Fleet)
		for (int x = 0; x < m_Fleet->GetFleetSize(); x++)
			m_Fleet->GetShipFromFleet(x)->Repair(bAtShipPort, bFasterShieldRecharge);
	// Wenn wir in diesem Sektor einen Shipport haben, dann wird die H?lle repariert
	if(bAtShipPort)
		m_Hull.RepairHull();

	if(m_iCurrentOrder == SHIP_ORDER::REPAIR && (!NeedsRepair() || !bAtShipPort))
		UnsetCurrentOrder();
}

/// Diese Funktion berechnet die Schiffserfahrung in einer neuen Runde. Außer Erfahrung im Kampf, diese werden nach einem
/// Kampf direkt verteilt.
void CShip::CalcExp()
{
/*
Starterfahrung 0 (bis auf Schiffe, die in Systemen mit Akademien gebaut werden, die fangen mit 500 Erfahrung bereits an), Bezeichnung Unerfahren. Erfahrung steigt mit 10 pro Runde kontinuierlich an bis 500.
Erfahrung 500 = Normal. Erfahrung steigt nun mit 5 pro Runde bis 1000.
Erfahrung 1000 = Veteran. Erfahrung steigt nicht mehr.
Erfahrung 2500 = Elite.
Erfahrung 5000 = Legendär.

Für den Erfahrungsgewinn gibt es mehrere Möglichkeiten:

1. random events die pauschal jeweils immer EP geben (Ionensturm überstanden, diplomatische Eskortmission, Anomalie erforscht)
2. Forschungsmissionen (Pulsar, Neutronenstern, schwarzes Loch, Wurmloch, Nebel scannen)
3. Akademien: Bei Anwesenheit eines Veteranen- oder höher -schiffes in der Crewtrainingflotte werden die Erfahrungsgewinne von unerfahrenen und normalen Crews pro Runde verdoppelt. Das Veteranenschiff bekommt die normalen EP-Gewinne.
4. Invasionen: Schiffe bekommen (Bevölkerungsverlust in Mrd.) * 100 + aktive shipdefence EP.
5. Systemblockaden, Überfälle: Verdoppeln pro Runde Gewinn für unerfahrene und normale Crews
6. Systemangriff: (Bevölkerungsverlust in Mrd.) * 100 + aktive shipdefence EP.
7. Schiffskampf: ((Durchschnittscrewerfahrung aller gegnerischen Schiffe)/((Durchschnittscrewerfahrung aller gegnerischen Schiffe)+(Durchschnittscrewerfahrung aller eigenen Schiffe)))x(totalen Hüllenschaden am Gegner)/100. Letzteres sorgt dafür dass Schäden an erfahrenen Schiffen höher gewichtet werden (da sie ja auch seltener sind, weil erfahrenere Schiffe seltener getroffen werden). Distributiert wird dann gleichmäßig auf alle Schiffe.
*/

	int expAdd = 0;
	switch (GetExpLevel())
	{
		case 0:	expAdd = 15;	break;
		case 1: expAdd = 10;	break;
		case 2: expAdd = 5;		break;
	}
	SetCrewExperiance(expAdd);
}
