// Ship.cpp: Implementierung der Klasse CShip.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Ship.h"
#include "Galaxy\Sector.h"
#include "HTMLStringBuilder.h"
#include "GraphicPool.h"
#include "Races/race.h"
#include "Races/major.h"
#include "General/Loc.h"
#include "Ships/Ships.h"
#include "ClientWorker.h"
#include "Races/RaceController.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CShip::CShip() :
	CInGameEntity(),
	m_iID(0),
	m_iMaintenanceCosts(0),
	m_byManeuverability(0),
	m_iSpeed(0),
	m_iScanPower(0),
	m_iScanRange(0),
	m_iStorageRoom(0),
	m_iColonizePoints(0),
	m_iStationBuildPoints(0),
	m_iStealthGrade(0),
	m_TargetKO(-1, -1)
{
	m_iCrewExperiance = 0;
	m_nTerraformingPlanet = -1;
	m_bIsFlagShip = FALSE;
	m_nSpecial[0] = m_nSpecial[1] = SHIP_SPECIAL::NONE;
	for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
		m_iLoadedResources[i] = 0;
	m_bCloakOn = false;
	m_iShipType = SHIP_TYPE::PROBE;
	m_iCurrentOrder = SHIP_ORDER::NONE;
	m_nCombatTactic = COMBAT_TACTIC::CT_ATTACK;
}

CShip::~CShip()
{
}


//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CShip::CShip(const CShip & rhs) :
	CInGameEntity(rhs),
	m_Hull(rhs.m_Hull),
	m_Shield(rhs.m_Shield),
	m_strShipClass(rhs.m_strShipClass),
	m_TargetKO(rhs.m_TargetKO)
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

	m_iID = rhs.m_iID;
	m_iMaintenanceCosts = rhs.m_iMaintenanceCosts;
	m_iShipType = rhs.m_iShipType;
	m_nShipSize = rhs.m_nShipSize;
	m_byManeuverability = rhs.m_byManeuverability;
	m_iSpeed = rhs.m_iSpeed;
	m_iRange = rhs.m_iRange;
	m_iScanPower = rhs.m_iScanPower;
	m_iScanRange = rhs.m_iScanRange;
	m_iCrewExperiance = rhs.m_iCrewExperiance;
	m_iStealthGrade = rhs.m_iStealthGrade;
	m_bCloakOn = rhs.m_bCloakOn;
	m_iStorageRoom = rhs.m_iStorageRoom;
	for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
		m_iLoadedResources[i] = rhs.m_iLoadedResources[i];
	m_iColonizePoints = rhs.m_iColonizePoints;
	m_iStationBuildPoints = rhs.m_iStationBuildPoints;
	m_iCurrentOrder = rhs.m_iCurrentOrder;
	m_nTerraformingPlanet = rhs.m_nTerraformingPlanet;
	m_bIsFlagShip = rhs.m_bIsFlagShip;
	m_nSpecial[0] = rhs.m_nSpecial[0];
	m_nSpecial[1] = rhs.m_nSpecial[1];
	m_nCombatTactic = rhs.m_nCombatTactic;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CShip & CShip::operator=(const CShip & rhs)
{
	if (this == &rhs)
		return *this;

	CInGameEntity::operator=(rhs);

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


	m_iID = rhs.m_iID;
	m_TargetKO = rhs.m_TargetKO;
	m_iMaintenanceCosts = rhs.m_iMaintenanceCosts;
	m_iShipType = rhs.m_iShipType;
	m_nShipSize = rhs.m_nShipSize;
	m_byManeuverability = rhs.m_byManeuverability;
	m_iSpeed = rhs.m_iSpeed;
	m_iRange = rhs.m_iRange;
	m_iScanPower = rhs.m_iScanPower;
	m_iScanRange = rhs.m_iScanRange;
	m_iCrewExperiance = rhs.m_iCrewExperiance;
	m_iStealthGrade = rhs.m_iStealthGrade;
	m_bCloakOn = rhs.m_bCloakOn;
	m_iStorageRoom = rhs.m_iStorageRoom;
	for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
		m_iLoadedResources[i] = rhs.m_iLoadedResources[i];
	m_iColonizePoints = rhs.m_iColonizePoints;
	m_iStationBuildPoints = rhs.m_iStationBuildPoints;
	m_iCurrentOrder = rhs.m_iCurrentOrder;
	m_nTerraformingPlanet = rhs.m_nTerraformingPlanet;
	m_strShipClass = rhs.m_strShipClass;
	m_bIsFlagShip = rhs.m_bIsFlagShip;
	m_nSpecial[0] = rhs.m_nSpecial[0];
	m_nSpecial[1] = rhs.m_nSpecial[1];
	m_nCombatTactic = rhs.m_nCombatTactic;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Serialisierungsfunktion
//////////////////////////////////////////////////////////////////////
void CShip::Serialize(CArchive &ar)
{
	CInGameEntity::Serialize(ar);

	m_Hull.Serialize(ar);
	m_Shield.Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iID;
		ar << m_TargetKO;
		ar << m_iMaintenanceCosts;
		ar << m_iShipType;
		ar << m_nShipSize;
		ar << m_byManeuverability;
		ar << (BYTE)(m_iSpeed);
		ar << m_iRange;
		ar << m_iScanPower;
		ar << m_iScanRange;
		ar << m_iCrewExperiance;
		ar << m_iStealthGrade;
		ar << m_bCloakOn;
		ar << m_iStorageRoom;
		for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
			ar << m_iLoadedResources[i];
		ar << m_iColonizePoints;
		ar << m_iStationBuildPoints;
		ar << m_iCurrentOrder;
		ar << m_nSpecial[0];
		ar << m_nSpecial[1];
		ar << m_nTerraformingPlanet;
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
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		ar >> m_iID;
		ar >> m_TargetKO;
		ar >> m_iMaintenanceCosts;
		int nType;
		ar >> nType;
		m_iShipType = (SHIP_TYPE::Typ)nType;
		int nSize;
		ar >> nSize;
		m_nShipSize = (SHIP_SIZE::Typ)nSize;
		ar >> m_byManeuverability;
		BYTE speed;
		ar >> speed;
		m_iSpeed = static_cast<unsigned>(speed);
		int nRange;
		ar >> nRange;
		m_iRange = (SHIP_RANGE::Typ)nRange;
		ar >> m_iScanPower;
		ar >> m_iScanRange;
		ar >> m_iCrewExperiance;
		ar >> m_iStealthGrade;
		ar >> m_bCloakOn;
		ar >> m_iStorageRoom;
		for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
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
	}
}


//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

const boost::shared_ptr<CRace> CShip::Owner() const
{
	const boost::shared_ptr<CRace>& owner = CInGameEntity::Owner();
	AssertBotE(owner);
	return owner;
}

CString CShip::OwnerID() const
{
	const CString& owner = CInGameEntity::OwnerID();
	AssertBotE(!owner.IsEmpty());
	return owner;
}

// Funktion gibt den Schiffstyp als char* zurück
CString CShip::GetShipTypeAsString(BOOL plural) const
{
	CString shipType;
	if (plural == FALSE)
	{
		switch (m_iShipType)
		{
		case SHIP_TYPE::TRANSPORTER: shipType = CLoc::GetString("TRANSPORTER"); break;
		case SHIP_TYPE::COLONYSHIP: shipType = CLoc::GetString("COLONIZESHIP"); break;
		case SHIP_TYPE::PROBE: shipType = CLoc::GetString("PROBE"); break;
		case SHIP_TYPE::SCOUT: shipType = CLoc::GetString("SCOUT"); break;
		case SHIP_TYPE::FIGHTER: shipType = CLoc::GetString("FIGHTER"); break;
		case SHIP_TYPE::FRIGATE: shipType = CLoc::GetString("FRIGATE"); break;
		case SHIP_TYPE::DESTROYER: shipType = CLoc::GetString("DESTROYER"); break;
		case SHIP_TYPE::CRUISER: shipType = CLoc::GetString("CRUISER"); break;
		case SHIP_TYPE::HEAVY_DESTROYER: shipType = CLoc::GetString("HEAVY_DESTROYER"); break;
		case SHIP_TYPE::HEAVY_CRUISER: shipType = CLoc::GetString("HEAVY_CRUISER"); break;
		case SHIP_TYPE::BATTLESHIP: shipType = CLoc::GetString("BATTLESHIP"); break;
		case SHIP_TYPE::DREADNOUGHT: shipType = CLoc::GetString("DREADNOUGHT"); break;
		case SHIP_TYPE::OUTPOST: shipType = CLoc::GetString("OUTPOST"); break;
		case SHIP_TYPE::STARBASE: shipType = CLoc::GetString("STARBASE"); break;
		case SHIP_TYPE::ALIEN: shipType = CLoc::GetString("ALIEN"); break;
		}
	}
	else
	{
		switch (m_iShipType)
		{
		case SHIP_TYPE::TRANSPORTER: shipType = CLoc::GetString("TRANSPORTERS"); break;
		case SHIP_TYPE::COLONYSHIP: shipType = CLoc::GetString("COLONIZESHIPS"); break;
		case SHIP_TYPE::PROBE: shipType = CLoc::GetString("PROBES"); break;
		case SHIP_TYPE::SCOUT: shipType = CLoc::GetString("SCOUTS"); break;
		case SHIP_TYPE::FIGHTER: shipType = CLoc::GetString("FIGHTERS"); break;
		case SHIP_TYPE::FRIGATE: shipType = CLoc::GetString("FRIGATES"); break;
		case SHIP_TYPE::DESTROYER: shipType = CLoc::GetString("DESTROYERS"); break;
		case SHIP_TYPE::CRUISER: shipType = CLoc::GetString("CRUISERS"); break;
		case SHIP_TYPE::HEAVY_DESTROYER: shipType = CLoc::GetString("HEAVY_DESTROYERS"); break;
		case SHIP_TYPE::HEAVY_CRUISER: shipType = CLoc::GetString("HEAVY_CRUISERS"); break;
		case SHIP_TYPE::BATTLESHIP: shipType = CLoc::GetString("BATTLESHIPS"); break;
		case SHIP_TYPE::DREADNOUGHT: shipType = CLoc::GetString("DREADNOUGHTS"); break;
		case SHIP_TYPE::OUTPOST: shipType = CLoc::GetString("OUTPOSTS"); break;
		case SHIP_TYPE::STARBASE: shipType = CLoc::GetString("STARBASES"); break;
		case SHIP_TYPE::ALIEN: shipType = CLoc::GetString("ALIENS"); break;
		}
	}

	return shipType;
}

// Funktion gibt den aktuellen Schiffsauftrag als char* zurück
CString CShip::GetCurrentOrderAsString() const
{
	CString order;
	switch (m_iCurrentOrder)
	{
	case SHIP_ORDER::AVOID: order = CLoc::GetString("AVOID_ORDER"); break;
	case SHIP_ORDER::ATTACK: order = CLoc::GetString("ATTACK_ORDER"); break;
	case SHIP_ORDER::ENCLOAK: order = CLoc::GetString("CLOAK_ORDER"); break;
	case SHIP_ORDER::DECLOAK: order = CLoc::GetString("DECLOAK_ORDER"); break;
	case SHIP_ORDER::ATTACK_SYSTEM: order = CLoc::GetString("ATTACK_SYSTEM_ORDER"); break;
	case SHIP_ORDER::RAID_SYSTEM: order = CLoc::GetString("RAID_SYSTEM_ORDER"); break;
	case SHIP_ORDER::BLOCKADE_SYSTEM: order = CLoc::GetString("BLOCKADE_SYSTEM_ORDER"); break;
	case SHIP_ORDER::DESTROY_SHIP: order = CLoc::GetString("DESTROY_SHIP_ORDER"); break;
	case SHIP_ORDER::COLONIZE: order = CLoc::GetString("COLONIZE_ORDER"); break;
	case SHIP_ORDER::TERRAFORM: order = CLoc::GetString("TERRAFORM_ORDER"); break;
	case SHIP_ORDER::BUILD_OUTPOST: order = CLoc::GetString("BUILD_OUTPOST_ORDER"); break;
	case SHIP_ORDER::BUILD_STARBASE: order = CLoc::GetString("BUILD_STARBASE_ORDER"); break;
	case SHIP_ORDER::UPGRADE_OUTPOST: order = CLoc::GetString("UPGRADE_OUTPOST_ORDER"); break;
	case SHIP_ORDER::UPGRADE_STARBASE: order = CLoc::GetString("UPGRADE_STARBASE_ORDER"); break;
	case SHIP_ORDER::ASSIGN_FLAGSHIP: order = CLoc::GetString("ASSIGN_FLAGSHIP_ORDER"); break;
	case SHIP_ORDER::CREATE_FLEET: order = CLoc::GetString("CREATE_FLEET_ORDER"); break;
	case SHIP_ORDER::TRANSPORT: order = CLoc::GetString("TRANSPORT_ORDER"); break;
	case SHIP_ORDER::FOLLOW_SHIP: order = CLoc::GetString("FOLLOW_SHIP_ORDER"); break;
	case SHIP_ORDER::TRAIN_SHIP: order = CLoc::GetString("TRAIN_SHIP_ORDER"); break;
	case SHIP_ORDER::WAIT_SHIP_ORDER: order = CLoc::GetString("WAIT_SHIP_ORDER"); break;
	case SHIP_ORDER::SENTRY_SHIP_ORDER: order = CLoc::GetString("SENTRY_SHIP_ORDER"); break;
	case SHIP_ORDER::REPAIR: order = CLoc::GetString("REPAIR_SHIP_ORDER"); break;
	case SHIP_ORDER::IMPROVE_SHIELDS: order = CLoc::GetString("IMPROVE_SHIELDS_SHIP_ORDER"); break;
	case SHIP_ORDER::EXTRACT_DEUTERIUM: order = CLoc::GetString("EXTRACT_DEUTERIUM_SHIP_ORDER"); break;
	case SHIP_ORDER::NONE: order = CLoc::GetString("NO_SHIP_ORDER"); break;
	default: AssertBotE(false); break;
	}
	return order;
}

CString CShip::GetCombatTacticAsString() const
{

	CString tactic;
	switch (m_nCombatTactic)
	{
		case COMBAT_TACTIC::CT_AVOID:
			tactic = CLoc::GetString("AVOID_ORDER");
		break;
		case COMBAT_TACTIC::CT_ATTACK:
			tactic = CLoc::GetString("ATTACK_ORDER");
		break;
		default:
			AssertBotE(false);
		break;
	}
	return tactic;
}

//Funktion gibt aktuelles Ziel als char* zurück
CString CShip::GetCurrentTargetAsString(const boost::shared_ptr<const CRace>& for_race) const
{
	AssertBotE(for_race);
	CString target;
	if(for_race == m_Owner || for_race->GetAgreement(m_Owner->GetRaceID()) >= DIPLOMATIC_AGREEMENT::ALLIANCE)
	{
		if(HasTarget())
		{
			const CSystem& system= resources::pDoc->GetSystem(m_TargetKO.x, m_TargetKO.y);
			target = system.CoordsName(system.GetKnown(for_race->GetRaceID()));
		}
		else
			target = CLoc::GetString("NO_SHIP_ORDER");
	}
	else
		target = CLoc::GetString("UNKNOWN");
	return target;
}

bool CShip::GetCloak() const {
	if(m_bCloakOn)
		AssertBotE(CanCloak());
	return m_bCloakOn;
}

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

void CShip::SetCo(int x, int y)
{
	m_Co = CPoint(x, y);
}

void CShip::AdoptOrdersFrom(const CShip& ship)
{
	SHIP_ORDER::Typ order = ship.GetCurrentOrder();
	AssertBotE(order != SHIP_ORDER::ASSIGN_FLAGSHIP && CanHaveOrder(order, false));
	//den Terraformingplaneten neu setzen
	SetTerraform(ship.m_nTerraformingPlanet);
	m_iCurrentOrder = order;
	m_nCombatTactic = ship.GetCombatTactic();
	AssertBotE(m_Co == ship.m_Co);
	m_TargetKO = ship.GetTargetKO();
}

void CShip::SetCrewExperiance(int nAdd)
{
	// Sonden und Aliens sammeln keine Erfahrung
	if (m_iShipType != SHIP_TYPE::PROBE && !IsAlien())
		m_iCrewExperiance = min(64000, m_iCrewExperiance + nAdd);
}

void CShip::ApplyTraining(int XP, bool veteran) {
	if (!veteran || GetExpLevel() >= 4)
		SetCrewExperiance(XP);
	else
		SetCrewExperiance(XP * 2);
}

bool CShip::ApplyIonstormEffects() {
	// Verlust aller Crewerfahrung bei Ionensturm
	m_iCrewExperiance = 0;
	// maximale Schildkapazität um 3% erhöhen
	// maximal die doppelte Anzahl der Schildstärke können erreicht werden
	const unsigned max_max_shield = GetMaxMaxShield();
	const unsigned old_max_shield = m_Shield.GetMaxShield();
	if(old_max_shield >= max_max_shield)
		return true;
	unsigned new_max_shield = old_max_shield * 1.03;
	bool improvement_finished = false;
	if(new_max_shield >= max_max_shield) {
		new_max_shield = max_max_shield;
		improvement_finished = true;
	}
	m_Shield.ModifyShield(new_max_shield, m_Shield.GetShieldType(), m_Shield.GetRegenerative());
	return improvement_finished;
}

bool CShip::UnassignFlagship() {
	const bool is = GetIsShipFlagShip();
	SetIsShipFlagShip(false);
	return is;
}

void CShip::SetCloak(bool bCloakOn) {
	if(!CanCloak()) {
		AssertBotE(!m_bCloakOn);
		return;
	}
	m_bCloakOn = bCloakOn;
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

//when setting a ship target, some running orders need to be unset if they're active
//this function calculates whether that's needed
static bool ShouldUnsetOrder(SHIP_ORDER::Typ order) {
	switch(order) {
		case SHIP_ORDER::ATTACK_SYSTEM:
		case SHIP_ORDER::BLOCKADE_SYSTEM:
		case SHIP_ORDER::DESTROY_SHIP:
		case SHIP_ORDER::COLONIZE:
		case SHIP_ORDER::TERRAFORM:
		case SHIP_ORDER::BUILD_OUTPOST:
		case SHIP_ORDER::BUILD_STARBASE:
		case SHIP_ORDER::UPGRADE_OUTPOST:
		case SHIP_ORDER::UPGRADE_STARBASE:
		case SHIP_ORDER::WAIT_SHIP_ORDER:
		case SHIP_ORDER::SENTRY_SHIP_ORDER:
		case SHIP_ORDER::REPAIR:
		case SHIP_ORDER::IMPROVE_SHIELDS:
		case SHIP_ORDER::EXTRACT_DEUTERIUM:
			return true;
	}
	return false;
}

void CShip::SetTargetKO(const CPoint& TargetKO, const bool simple_setter)
{
	m_TargetKO = TargetKO;
	// Wenn kein Ziel mehr vorhanden, dann den Pfad löschen
	if (!HasTarget())
		m_Path.RemoveAll();

	if(simple_setter)
		return;
	if(TargetKO != CPoint(-1, -1) && ShouldUnsetOrder(m_iCurrentOrder)) {
		SetTerraform(-1);
		UnsetCurrentOrder();
	}
}

void CShip::SetCombatTactic(COMBAT_TACTIC::Typ nTactic, bool also_if_retreat)
{
	if(!also_if_retreat && m_nCombatTactic == COMBAT_TACTIC::CT_RETREAT)
		return;
	m_nCombatTactic = nTactic;
}

void CShip::SetCurrentOrderAccordingToType() {
	UnsetCurrentOrder();
	SetCombatTacticAccordingToType();
}

void CShip::SetCurrentOrder(SHIP_ORDER::Typ nCurrentOrder) {
	AssertBotE(nCurrentOrder != SHIP_ORDER::TERRAFORM
		&& nCurrentOrder != SHIP_ORDER::ATTACK
		&& nCurrentOrder != SHIP_ORDER::AVOID
		&& nCurrentOrder != SHIP_ORDER::ASSIGN_FLAGSHIP);
	if(m_iCurrentOrder == SHIP_ORDER::TERRAFORM)
		SetTerraform(-1);
	m_iCurrentOrder = nCurrentOrder;
}

void CShip::SetCombatTacticAccordingToType() {
	m_nCombatTactic = IsNonCombat() ? COMBAT_TACTIC::CT_AVOID : COMBAT_TACTIC::CT_ATTACK;
}

void CShip::UnsetCurrentOrder() {
	m_nTerraformingPlanet = -1;
	const bool was_terraform = m_iCurrentOrder == SHIP_ORDER::TERRAFORM;
	m_iCurrentOrder = SHIP_ORDER::NONE;
	if(was_terraform)
		resources::pDoc->GetSystem(m_Co.x, m_Co.y).RecalcPlanetsTerraformingStatus();
}

bool CShip::RemoveDestroyed(CRace& owner, unsigned short round, const CString& sEvent, const CString& sStatus, CStringArray* destroyedShips, const CString& anomaly) {

	if(IsAlive())
		return true;
	// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
	owner.AddToLostShipHistory(ShipHistoryInfo(), sEvent, sStatus, round, m_Co);
	if(destroyedShips)
		destroyedShips->Add(m_sName + " (" + GetShipTypeAsString() + ", " + m_strShipClass + ")");
	if(m_bIsFlagShip)
		owner.LostFlagShip(*this);
	if(IsStation())
		owner.LostStation(m_iShipType);
	if(!anomaly.IsEmpty()) {
		owner.LostShipToAnomaly(CShips(*this), anomaly);
	}

	return false;
}

void CShip::SetTerraform(short planetNumber) {
	if(planetNumber == -1) {
		if(m_iCurrentOrder == SHIP_ORDER::TERRAFORM)
			m_iCurrentOrder = SHIP_ORDER::NONE;
	}
	else
		m_iCurrentOrder = SHIP_ORDER::TERRAFORM;
	m_nTerraformingPlanet = planetNumber;
	resources::pDoc->GetSystem(m_Co.x, m_Co.y).RecalcPlanetsTerraformingStatus();
}

//////////////////////////////////////////////////////////////////////
// calculated stements about this ship (should be const functions, non-bool returning)
//////////////////////////////////////////////////////////////////////

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

					// Wenn kein Schaden direkt auf die Hülle geht, weil der Schaden zu klein ist, um einen
					// prozentualen Anteil zu haben, dann den Beamschaden halbieren (nur wenn nicht immer schildbrechend)
					bool bHalfDmg = m_BeamWeapons.GetAt(i).GetBeamPower() * DAMAGE_TO_HULL < 1;

					// besondere Beamfähigkeiten erhöhen den BeamDmg um einen selbst gewählten Mulitplikator
					// der dadurch erhaltende Schaden entspricht nicht dem wirklichen Schaden!
					if (m_BeamWeapons.GetAt(i).GetPiercing())
						tempBeamDmg = (UINT)(tempBeamDmg * 1.5);
					if (m_BeamWeapons.GetAt(i).GetModulating())
						tempBeamDmg *= 3;
					else if (bHalfDmg)
						tempBeamDmg /= 2;

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

			// Wenn kein Schaden direkt auf die Hülle geht, weil der Schaden zu klein ist, um einen
			// prozentualen Anteil zu haben, dann den Torpedoschaden halbieren (nur wenn nicht immer schildbrechend)
			bool bHalfDmg = m_TorpedoWeapons.GetAt(i).GetTorpedoPower() * DAMAGE_TO_HULL < 1;

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

			if (bHalfDmg && !CTorpedoInfo::GetIgnoreAllShields(type))
				tempTorpedoDmg /= 2;

			torpedoDmg += tempTorpedoDmg;
		}
	}

	// Stationen bekommen einen Bonus, da sie keine Feuerwinkel beachten
	if (IsStation())
	{
		beamDmg = (UINT)(beamDmg * 1.5);
		torpedoDmg = (UINT)(torpedoDmg * 1.5);
	}

	// Manövrierfähigkeit geht mit in den Wert ein
	double dMan = 1.0;
	if (!IsStation())
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
	if (!IsStation())
		dMan = ((int)m_byManeuverability - 4.0) / 10.0 * 1.75 + 1.0;

	// Tarnung geht mit in den Wert ein
	double dCloak = 1.0;
	if (m_bCloakOn)
		dCloak = 1.25;

	return (UINT)(def * dMan * dCloak);
}

/// Funktion gibt den schon benutzten Lagerraum im Schiff zurück.
USHORT CShip::GetUsedStorageRoom(const CArray<CTroopInfo>* troopInfo) const
{
	USHORT usedStorage = 0;
	// benötigten Platz durch Truppen, welche schon im Schiff sind ermitteln
	for (int i = 0; i < this->GetTransportedTroops()->GetSize(); i++)
	{
		BYTE id = this->GetTransportedTroops()->GetAt(i).GetID();
		usedStorage += troopInfo->GetAt(id).GetSize();
	}
	for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
	{
		// Deritium wird im Verhältnis 1:250 gelagert, die anderen Ressourcen im Verhältnis 1:1
		if (i == RESOURCES::DERITIUM)
			usedStorage += this->GetLoadedResources(i) * 250;
		else
			usedStorage += this->GetLoadedResources(i);
	}
	return usedStorage;
}

int CShip::GetFreeStorageRoom() const
{
	const int result = m_iStorageRoom - GetUsedStorageRoom(&resources::pDoc->m_TroopInfo);
	AssertBotE(result >= 0);
	return result;
}

unsigned CShip::GetStealthPower() const {
	unsigned level = m_iStealthGrade;
	if(!GetCloak())
		level = min(level, 3);
	return level * 20;
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
	// Anfänger
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

unsigned CShip::GetMaxMaxShield() const {
	return resources::pDoc->GetShipInfos()->GetAt(m_iID - 10000).GetShield()->GetMaxShield() * 2;
}

//////////////////////////////////////////////////////////////////////
// bool statements about this ship
//////////////////////////////////////////////////////////////////////

/*
 * Judge whether this ship is "worse" than the passed one.
 * Currently only used for special techs which can improve stations,
 * extend with missing things if needed.
 */
bool CShip::IsWorseThan(const CShip& o) const
{
	if(m_Shield.GetMaxShield() < o.m_Shield.GetMaxShield() ||
		m_Hull.GetMaxHull() < o.m_Hull.GetMaxHull() ||
		m_iScanPower < o.m_iScanPower)
	return true;

	if(m_BeamWeapons.GetSize() < o.m_BeamWeapons.GetSize())
		return true;
	if(!m_BeamWeapons.IsEmpty())
	{
		const CBeamWeapons& ours = m_BeamWeapons.GetAt(0);
		const CBeamWeapons& theirs = o.m_BeamWeapons.GetAt(0);
		if(ours.GetBeamPower() < theirs.GetBeamPower() || ours.GetRechargeTime() > theirs.GetRechargeTime())
			return true;
	}

	if(m_TorpedoWeapons.GetSize() < o.m_TorpedoWeapons.GetSize())
		return true;
	if(!m_TorpedoWeapons.IsEmpty())
	{
		const CTorpedoWeapons& ours = m_TorpedoWeapons.GetAt(0);
		const CTorpedoWeapons& theirs = o.m_TorpedoWeapons.GetAt(0);
		if(ours.GetAccuracy() < theirs.GetAccuracy() || ours.GetTupeFirerate() > theirs.GetTupeFirerate())
			return true;
	}

	return false;
}

bool CShip::IonstormCanImproveShields() const {
	return GetMaxMaxShield() > m_Shield.GetMaxShield();
}

bool CShip::HasSpecial(SHIP_SPECIAL::Typ nAbility) const
{
	if (m_nSpecial[0] == nAbility || m_nSpecial[1] == nAbility)
		return true;
	else
		return false;
}

bool CShip::HasNothingToDo() const {
	return m_iCurrentOrder == SHIP_ORDER::NONE && !HasTarget() && !IsStation();
}

bool CShip::NeedsRepair() const {
	return m_Hull.GetCurrentHull() < m_Hull.GetMaxHull() || m_Shield.GetCurrentShield() < m_Shield.GetMaxShield();
}

bool CShip::CanExtractDeuterium() const
{
	return m_iShipType == SHIP_TYPE::TRANSPORTER && GetFreeStorageRoom() > 0;
}

bool CShip::IsNonCombat() const {
	return m_iShipType <= SHIP_TYPE::PROBE;
}

bool CShip::IsVeteran() const {
	return GetExpLevel() >= 4;
}

static bool CheckNew(SHIP_ORDER::Typ order, SHIP_ORDER::Typ co, COMBAT_TACTIC::Typ ct) {
	if(order == SHIP_ORDER::AVOID)
		return ct != COMBAT_TACTIC::CT_AVOID;
	else if (order == SHIP_ORDER::ATTACK)
		return ct != COMBAT_TACTIC::CT_ATTACK;
	return co != order;
}

bool CShip::CanHaveOrder(SHIP_ORDER::Typ order, bool require_new) const {
	if(require_new && !CheckNew(order, m_iCurrentOrder, m_nCombatTactic))
		return false;
	switch(order) {
		case SHIP_ORDER::NONE:
		case SHIP_ORDER::AVOID:
		case SHIP_ORDER::ATTACK:
		case SHIP_ORDER::DESTROY_SHIP:
		case SHIP_ORDER::FOLLOW_SHIP:
		case SHIP_ORDER::CANCEL:
		case SHIP_ORDER::TRAIN_SHIP:
			return true;
		case SHIP_ORDER::IMPROVE_SHIELDS:
			return IonstormCanImproveShields();
		case SHIP_ORDER::EXTRACT_DEUTERIUM:
			return CanExtractDeuterium();
		case SHIP_ORDER::WAIT_SHIP_ORDER:
		case SHIP_ORDER::SENTRY_SHIP_ORDER:
		case SHIP_ORDER::CREATE_FLEET:
			return !IsStation();
		case SHIP_ORDER::REPAIR:
			return NeedsRepair();
		case SHIP_ORDER::ASSIGN_FLAGSHIP:
			return !m_bIsFlagShip && !IsStation();
		case SHIP_ORDER::ENCLOAK:
			return CanCloak() && !m_bCloakOn;
		case SHIP_ORDER::DECLOAK:
			return m_bCloakOn;
		case SHIP_ORDER::COLONIZE:
		case SHIP_ORDER::TERRAFORM:
			return GetColonizePoints() >= 1;
		case SHIP_ORDER::BUILD_OUTPOST:
			return m_iStationBuildPoints >= 1 && !IsDoingStationWork(SHIP_ORDER::BUILD_OUTPOST);
		case SHIP_ORDER::BUILD_STARBASE:
			return m_iStationBuildPoints >= 1 && !IsDoingStationWork(SHIP_ORDER::BUILD_STARBASE);
		case SHIP_ORDER::UPGRADE_OUTPOST:
			return m_iStationBuildPoints >= 1 && !IsDoingStationWork(SHIP_ORDER::UPGRADE_OUTPOST);
		case SHIP_ORDER::UPGRADE_STARBASE:
			return m_iStationBuildPoints >= 1 && !IsDoingStationWork(SHIP_ORDER::UPGRADE_STARBASE);
		case SHIP_ORDER::BLOCKADE_SYSTEM:
			return HasSpecial(SHIP_SPECIAL::BLOCKADESHIP);
		case SHIP_ORDER::ATTACK_SYSTEM:
			return !GetCloak() && !IsStation();
		case SHIP_ORDER::TRANSPORT:
			return GetStorageRoom() >= 1;
		case SHIP_ORDER::RAID_SYSTEM:
			return HasSpecial(SHIP_SPECIAL::RAIDER);
		default: //possibly added commands
			AssertBotE(false);
	}
	return false;
}

bool CShip::CanCloak() const {
	return m_iStealthGrade >= 4 && !IsStation();
}

bool CShip::IsDoingStationWork(SHIP_ORDER::Typ ignore) const
{
	const SHIP_ORDER::Typ values[] = { SHIP_ORDER::BUILD_OUTPOST, SHIP_ORDER::BUILD_STARBASE,
		SHIP_ORDER::UPGRADE_OUTPOST, SHIP_ORDER::UPGRADE_STARBASE };
	std::set<SHIP_ORDER::Typ> station_orders(values, values + sizeof(values)/sizeof(SHIP_ORDER::Typ));
	if(ignore != SHIP_ORDER::NONE)
		station_orders.erase(ignore);
	for(std::set<SHIP_ORDER::Typ>::const_iterator it = station_orders.begin(); it != station_orders.end(); ++it)
		if(m_iCurrentOrder == *it)
			return true;
	return false;
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

/// Funktion erstellt eine Tooltipinfo vom Schiff
/// @param info wenn dieser Parameter nicht NULL ist dann werden Informationen über die angeführte Flotte angezeigt, sonst nur über das Schiff
/// @return	der erstellte Tooltip-Text
CString CShip::GetTooltip(const FleetInfoForGetTooltip* const info) const
{
	CString sName = m_sName;
	if (sName.IsEmpty() == false)
	{
		if (info)
		{
			// Schiffsnamen holen und die ersten 4 Zeichen (z.B. USS_) und die lezten 2 Zeichen (z.B. _A) entfernen
			if (sName.GetLength() > 4 && sName.GetAt(3) == ' ')
				sName.Delete(0,4);
			if (sName.GetLength() > 2 && sName.ReverseFind(' ') == sName.GetLength() - 2)
				sName.Delete(sName.GetLength() - 2, 2);
			sName.Append(" " + CLoc::GetString("GROUP"));
		}
		sName = CHTMLStringBuilder::GetHTMLColor(sName);
		sName = CHTMLStringBuilder::GetHTMLHeader(sName, _T("h3"));
		sName = CHTMLStringBuilder::GetHTMLCenter(sName);
		sName += CHTMLStringBuilder::GetHTMLStringNewLine();
	}

	CString sType;
	if (info && info->FleetShipType == -1)
		sType = _T("(") + CLoc::GetString("MIXED_FLEET") + _T(")");
	else if (info)
		sType = _T("(") + GetShipTypeAsString(TRUE) + _T(")");
	else
		sType = _T("(") + GetShipTypeAsString() + _T(")");
	sType = CHTMLStringBuilder::GetHTMLColor(sType, _T("silver"));
	sType = CHTMLStringBuilder::GetHTMLHeader(sType, _T("h4"));
	sType = CHTMLStringBuilder::GetHTMLCenter(sType);
	sType += CHTMLStringBuilder::GetHTMLStringNewLine();

	// anzeigen von Größenklasse + Unterhalt + Crewerfahrung
	CString sCosts = "0";
	sCosts.Format("%s: %d    %s: %d    %s: %d", CLoc::GetString("SHIPSIZE"), GetShipSize(), CLoc::GetString("SHIPCOSTS"), GetMaintenanceCosts(), CLoc::GetString("EXPERIANCE"), GetCrewExperience());
	sCosts = CHTMLStringBuilder::GetHTMLColor(sCosts);
	sCosts = CHTMLStringBuilder::GetHTMLHeader(sCosts, _T("h5"));
	sCosts = CHTMLStringBuilder::GetHTMLCenter(sCosts);
	sCosts += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Bewegung anzeigen
	CString sMovementHead = "";
	sMovementHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sMovementHead += CLoc::GetString("MOVEMENT");
	sMovementHead = CHTMLStringBuilder::GetHTMLColor(sMovementHead, _T("silver"));
	sMovementHead = CHTMLStringBuilder::GetHTMLHeader(sMovementHead, _T("h4"));
	sMovementHead = CHTMLStringBuilder::GetHTMLCenter(sMovementHead);
	sMovementHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sMovementHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sMovementHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sMovement = CLoc::GetString("RANGE") + _T(": ");
	SHIP_RANGE::Typ nRange = GetRange();
	if (info)
		nRange = info->FleetRange;
	if (nRange == SHIP_RANGE::SHORT)
		sMovement += CLoc::GetString("SHORT");
	else if (nRange == SHIP_RANGE::MIDDLE)
		sMovement += CLoc::GetString("MIDDLE");
	else if (nRange == SHIP_RANGE::LONG)
		sMovement += CLoc::GetString("LONG");
	sMovement += CHTMLStringBuilder::GetHTMLStringNewLine();
	CString sSpeed;
	BYTE bySpeed = GetSpeed();
	if (info)
		bySpeed = info->FleetSpeed;
	sSpeed.Format("%s: %d\n", CLoc::GetString("SPEED"), bySpeed);
	sMovement += sSpeed;

	// wenn es eine Flotte ist keine weiteren Infos anzeigen
	if (info)
	{
		sMovement = CHTMLStringBuilder::GetHTMLColor(sMovement);
		sMovement = CHTMLStringBuilder::GetHTMLHeader(sMovement, _T("h5"));
		sMovement = CHTMLStringBuilder::GetHTMLCenter(sMovement);
		return sName + sType + sMovementHead + sMovement;
	}

	sMovement += CLoc::GetString("MANEUVERABILITY") + _T(": ");
	switch (GetManeuverability())
	{
	case 9:	sMovement += CLoc::GetString("PHENOMENAL");	break;
	case 8:	sMovement += CLoc::GetString("EXCELLENT");	break;
	case 7:	sMovement += CLoc::GetString("VERYGOOD");	break;
	case 6:	sMovement += CLoc::GetString("GOOD");		break;
	case 5:	sMovement += CLoc::GetString("NORMAL");		break;
	case 4:	sMovement += CLoc::GetString("ADEQUATE");	break;
	case 3:	sMovement += CLoc::GetString("BAD");		break;
	case 2:	sMovement += CLoc::GetString("VERYBAD");	break;
	case 1:	sMovement += CLoc::GetString("MISERABLE");	break;
	default:sMovement += CLoc::GetString("NONE");
	}

	//show capacity
	if(m_iStorageRoom > 0)
	{
		sMovement += CHTMLStringBuilder::GetHTMLStringNewLine();
		CString s;
		s.Format("%s: %u", CLoc::GetString("PLACE"), m_iStorageRoom);
		sMovement += s;
	}
	sMovement = CHTMLStringBuilder::GetHTMLColor(sMovement);
	sMovement = CHTMLStringBuilder::GetHTMLHeader(sMovement, _T("h5"));
	sMovement = CHTMLStringBuilder::GetHTMLCenter(sMovement);
	sMovement += CHTMLStringBuilder::GetHTMLStringNewLine();
	sMovement += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Bewaffnung anzeigen
	UINT nOverallDmg = 0;
	std::map<CString, int> mBeamWeapons;
	for (int i = 0; i < GetBeamWeapons()->GetSize(); i++)
	{
		const CBeamWeapons* pBeam = &(GetBeamWeapons()->GetAt(i));
		CString s;
		s.Format("%s %d %s", CLoc::GetString("TYPE"), pBeam->GetBeamType(), pBeam->GetBeamName());
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
		sBeams = CLoc::GetString("NONE") + "\n";
	sBeams = CHTMLStringBuilder::GetHTMLColor(sBeams);
	sBeams = CHTMLStringBuilder::GetHTMLHeader(sBeams, _T("h5"));
	sBeams = CHTMLStringBuilder::GetHTMLCenter(sBeams);
	sBeams += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sBeamWeaponHead;
	sBeamWeaponHead.Format("%s (%s: %u)", CLoc::GetString("BEAMWEAPONS"), CLoc::GetString("DAMAGE"), nOverallDmg);
	sBeamWeaponHead = CHTMLStringBuilder::GetHTMLColor(sBeamWeaponHead, _T("silver"));
	sBeamWeaponHead = CHTMLStringBuilder::GetHTMLHeader(sBeamWeaponHead, _T("h4"));
	sBeamWeaponHead = CHTMLStringBuilder::GetHTMLCenter(sBeamWeaponHead);
	sBeamWeaponHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sBeamWeaponHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sBeamWeaponHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	nOverallDmg = 0;
	std::map<CString, int> mTorpedoWeapons;
	for (int i = 0; i < GetTorpedoWeapons()->GetSize(); i++)
	{
		const CTorpedoWeapons* pTorp = &(GetTorpedoWeapons()->GetAt(i));
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
		sTorps = CLoc::GetString("NONE") + "\n";
	sTorps = CHTMLStringBuilder::GetHTMLColor(sTorps);
	sTorps = CHTMLStringBuilder::GetHTMLHeader(sTorps, _T("h5"));
	sTorps = CHTMLStringBuilder::GetHTMLCenter(sTorps);
	sTorps += CHTMLStringBuilder::GetHTMLStringNewLine();
	CString sTupeWeaponHead;
	sTupeWeaponHead.Format("%s (%s: %u)", CLoc::GetString("TORPEDOWEAPONS"), CLoc::GetString("DAMAGE"), nOverallDmg);
	sTupeWeaponHead = CHTMLStringBuilder::GetHTMLColor(sTupeWeaponHead, _T("silver"));
	sTupeWeaponHead = CHTMLStringBuilder::GetHTMLHeader(sTupeWeaponHead, _T("h4"));
	sTupeWeaponHead = CHTMLStringBuilder::GetHTMLCenter(sTupeWeaponHead);
	sTupeWeaponHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sTupeWeaponHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sTupeWeaponHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sDefensiveHead = CLoc::GetString("SHIELDS")+" "+CLoc::GetString("AND")+" "+CLoc::GetString("HULL");
	sDefensiveHead = CHTMLStringBuilder::GetHTMLColor(sDefensiveHead, _T("silver"));
	sDefensiveHead = CHTMLStringBuilder::GetHTMLHeader(sDefensiveHead, _T("h4"));
	sDefensiveHead = CHTMLStringBuilder::GetHTMLCenter(sDefensiveHead);
	sDefensiveHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sDefensiveHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sDefensiveHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	const CShield* pShield = GetShield();
	CString sShield;
	sShield.Format("%s %d %s: %s %d/%d", CLoc::GetString("TYPE"), pShield->GetShieldType(), CLoc::GetString("SHIELDS"), CLoc::GetString("CAPACITY"), (UINT)pShield->GetCurrentShield(), (UINT)pShield->GetMaxShield());
	sShield = CHTMLStringBuilder::GetHTMLColor(sShield);
	sShield = CHTMLStringBuilder::GetHTMLHeader(sShield, _T("h5"));
	sShield = CHTMLStringBuilder::GetHTMLCenter(sShield);
	sShield += CHTMLStringBuilder::GetHTMLStringNewLine();

	const CHull* pHull = GetHull();
	CString sMaterial;
	switch (pHull->GetHullMaterial())
	{
	case RESOURCES::TITAN:		sMaterial = CLoc::GetString("TITAN");	 break;
	case RESOURCES::DURANIUM:	sMaterial = CLoc::GetString("DURANIUM"); break;
	case RESOURCES::IRIDIUM:	sMaterial = CLoc::GetString("IRIDIUM");	 break;
	default:		sMaterial = "";
	}
	CString sHull;
	if (pHull->GetDoubleHull() == TRUE)
		sHull.Format("%s%s: %s %d/%d", sMaterial, CLoc::GetString("DOUBLE_HULL_ARMOUR"), CLoc::GetString("INTEGRITY"), (int)pHull->GetCurrentHull(), (int)pHull->GetMaxHull());
	else
		sHull.Format("%s%s: %s %d/%d", sMaterial, CLoc::GetString("HULL_ARMOR"), CLoc::GetString("INTEGRITY"), (int)pHull->GetCurrentHull(), (int)pHull->GetMaxHull());
	sHull = CHTMLStringBuilder::GetHTMLColor(sHull);
	sHull = CHTMLStringBuilder::GetHTMLHeader(sHull, _T("h5"));
	sHull = CHTMLStringBuilder::GetHTMLCenter(sHull);
	sHull += CHTMLStringBuilder::GetHTMLStringNewLine();
	sHull += CHTMLStringBuilder::GetHTMLStringNewLine();


	// Manövrierbarkeit anzeigen
	CString sManeuverHead = CLoc::GetString("MANEUVERABILITY");
	sManeuverHead = CHTMLStringBuilder::GetHTMLColor(sManeuverHead, _T("silver"));
	sManeuverHead = CHTMLStringBuilder::GetHTMLHeader(sManeuverHead, _T("h4"));
	sManeuverHead = CHTMLStringBuilder::GetHTMLCenter(sManeuverHead);
	sManeuverHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sManeuverHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sManeuverHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sManeuver;
	switch (GetManeuverability())
	{
	case 9:	sManeuver = CLoc::GetString("PHENOMENAL");	break;
	case 8:	sManeuver = CLoc::GetString("EXCELLENT");	break;
	case 7:	sManeuver = CLoc::GetString("VERYGOOD");	break;
	case 6:	sManeuver = CLoc::GetString("GOOD");		break;
	case 5:	sManeuver = CLoc::GetString("NORMAL");		break;
	case 4:	sManeuver = CLoc::GetString("ADEQUATE");	break;
	case 3:	sManeuver = CLoc::GetString("BAD");			break;
	case 2:	sManeuver = CLoc::GetString("VERYBAD");		break;
	case 1:	sManeuver = CLoc::GetString("MISERABLE");	break;
	default:sManeuver = CLoc::GetString("NONE");
	}
	sManeuver = CHTMLStringBuilder::GetHTMLColor(sManeuver);
	sManeuver = CHTMLStringBuilder::GetHTMLHeader(sManeuver, _T("h5"));
	sManeuver = CHTMLStringBuilder::GetHTMLCenter(sManeuver);
	sManeuver += CHTMLStringBuilder::GetHTMLStringNewLine();
	sManeuver += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Scan anzeigen
	CString sScanHead = CLoc::GetString("SENSORS");
	sScanHead = CHTMLStringBuilder::GetHTMLColor(sScanHead, _T("silver"));
	sScanHead = CHTMLStringBuilder::GetHTMLHeader(sScanHead, _T("h4"));
	sScanHead = CHTMLStringBuilder::GetHTMLCenter(sScanHead);
	sScanHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sScanHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sScanHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sScan;
	sScan.Format("%s: %d", CLoc::GetString("SCANRANGE"), GetScanRange());
	sScan = CHTMLStringBuilder::GetHTMLColor(sScan);
	sScan = CHTMLStringBuilder::GetHTMLHeader(sScan, _T("h5"));
	sScan = CHTMLStringBuilder::GetHTMLCenter(sScan);
	sScan += CHTMLStringBuilder::GetHTMLStringNewLine();
	CString sScanpower;
	sScanpower.Format("%s: %d", CLoc::GetString("SCANPOWER"), GetScanPower());
	sScanpower = CHTMLStringBuilder::GetHTMLColor(sScanpower);
	sScanpower = CHTMLStringBuilder::GetHTMLHeader(sScanpower, _T("h5"));
	sScanpower = CHTMLStringBuilder::GetHTMLCenter(sScanpower);
	sScanpower += CHTMLStringBuilder::GetHTMLStringNewLine();
	sScanpower += CHTMLStringBuilder::GetHTMLStringNewLine();

	// Spezialfähigkeiten anzeigen
	CString sSpecialsHead = CLoc::GetString("SPECIAL_ABILITIES");
	sSpecialsHead = CHTMLStringBuilder::GetHTMLColor(sSpecialsHead, _T("silver"));
	sSpecialsHead = CHTMLStringBuilder::GetHTMLHeader(sSpecialsHead, _T("h4"));
	sSpecialsHead = CHTMLStringBuilder::GetHTMLCenter(sSpecialsHead);
	sSpecialsHead += CHTMLStringBuilder::GetHTMLStringNewLine();
	sSpecialsHead += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sSpecialsHead += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sSpecials;
	if (HasSpecial(SHIP_SPECIAL::ASSULTSHIP))
		sSpecials += CLoc::GetString("ASSAULTSHIP") + "\n";
	if (HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
		sSpecials += CLoc::GetString("BLOCKADESHIP") + "\n";
	if (HasSpecial(SHIP_SPECIAL::COMMANDSHIP))
		sSpecials += CLoc::GetString("COMMANDSHIP") + "\n";
	if (HasSpecial(SHIP_SPECIAL::DOGFIGHTER))
		sSpecials += CLoc::GetString("DOGFIGHTER") + "\n";
	if (HasSpecial(SHIP_SPECIAL::DOGKILLER))
		sSpecials += CLoc::GetString("DOGKILLER") + "\n";
	if (HasSpecial(SHIP_SPECIAL::PATROLSHIP))
		sSpecials += CLoc::GetString("PATROLSHIP") + "\n";
	if (HasSpecial(SHIP_SPECIAL::RAIDER))
		sSpecials += CLoc::GetString("RAIDER") + "\n";
	if (HasSpecial(SHIP_SPECIAL::SCIENCEVESSEL))
		sSpecials += CLoc::GetString("SCIENCESHIP") + "\n";
	if (pShield->GetRegenerative())
		sSpecials += CLoc::GetString("REGENERATIVE_SHIELDS") + "\n";
	if (pHull->GetAblative())
		sSpecials += CLoc::GetString("ABLATIVE_ARMOR") + "\n";
	if  (pHull->GetPolarisation())
		sSpecials += CLoc::GetString("HULLPOLARISATION") + "\n";
	if (CanCloak())
		sSpecials += CLoc::GetString("CAN_CLOAK") + "\n";
	if (sSpecials.IsEmpty())
		sSpecials = CLoc::GetString("NONE") + "\n";;
	sSpecials = CHTMLStringBuilder::GetHTMLColor(sSpecials);
	sSpecials = CHTMLStringBuilder::GetHTMLHeader(sSpecials, _T("h5"));
	sSpecials = CHTMLStringBuilder::GetHTMLCenter(sSpecials);
	sSpecials += CHTMLStringBuilder::GetHTMLStringNewLine();
	sSpecials += CHTMLStringBuilder::GetHTMLStringHorzLine();
	sSpecials += CHTMLStringBuilder::GetHTMLStringNewLine();

	CString sDesc = m_sDescription;
	sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc);
	sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));

	CString sTip = sName + sType + sCosts + sMovementHead + sMovement + sBeamWeaponHead + sBeams + sTupeWeaponHead + sTorps + sDefensiveHead + sShield + sHull + sScanHead + sScan + sScanpower + sSpecialsHead + sSpecials + sDesc;
	return sTip;
}

void CShip::DrawOrderTerraform(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt) const {
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (pDoc->m_bDataReceived) {
		const CSystem& sec = pDoc->GetSystem(m_Co.x, m_Co.y);
		CString s = sec.GetPlanet(GetTerraform())->GetPlanetGraphicFile();
		Bitmap* graphic = pGraphicPool->GetGDIGraphic(s);

		// Planeten zeichnen, der gerade terraformt wird
		if( graphic )
			g->DrawImage(graphic, pt.x + 85, pt.y + 60, 15, 15);
	}
}

void CShip::DrawOrderColonize(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt) {
	Bitmap* graphic = pGraphicPool->GetGDIGraphic("Other\\populationSmall.bop");

	// Planeten zeichnen, der gerade terraformt wird
	if( graphic )
		g->DrawImage(graphic, pt.x + 85, pt.y + 60, 15, 15);
}

void CShip::DrawShip(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt, bool bIsMarked,
		bool bOwnerUnknown, bool bDrawFleet, const Gdiplus::Color& clrNormal,
		const Gdiplus::Color& clrMark, const Gdiplus::Font& font, bool bDrawTroopSymbol,
		short FleetShipType, int FleetSize) const
{
	StringFormat fontFormat;
	SolidBrush fontBrush(clrNormal);

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
		graphic = pGraphicPool->GetGDIGraphic("Symbols\\" + OwnerID() + ".bop");
		if (graphic)
			g->DrawImage(graphic, pt.x + 37, pt.y + 30, 25, 25);
	}

	// Wenn des Schiff Truppen transportiert, dann kleines Truppensymbol zeichnen
	// Symbole zu Truppen zeichnen
	// prüfen ob ein Schiff in der Flotte Truppen hat
	if (bDrawTroopSymbol || HasTroops())
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

		if( m_iCurrentOrder == SHIP_ORDER::TERRAFORM ) {
			DrawOrderTerraform(g, pGraphicPool, pt);
		} else if( m_iCurrentOrder == SHIP_ORDER::COLONIZE ) {
			DrawOrderColonize(g, pGraphicPool, pt);
		}

		// normale Infos zum Schiff sollen angezeigt werden
		if (!bDrawFleet)
		{
			if (!IsAlien())
			{
				g->DrawString(CComBSTR(m_sName), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 37), &fontFormat, &fontBrush);
				s = m_strShipClass + "-" + CLoc::GetString("CLASS");
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
			s.Format("%s", m_sName);
			if (s.GetLength() > 4 && s.GetAt(3) == ' ')
				s.Delete(0,4);
			if (s.GetLength() > 2 && s.ReverseFind(' ') == s.GetLength() - 2)
				s.Delete(s.GetLength() - 2, 2);

			s.Append(" " + CLoc::GetString("GROUP"));
			// Hier jetzt Namen und Schiffstype zur Flotte
			g->DrawString(CComBSTR(s), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 37), &fontFormat, &fontBrush);

			if (FleetShipType == -1)
				g->DrawString(CComBSTR(CLoc::GetString("MIXED_FLEET")), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 57), &fontFormat, &fontBrush);
			else
				g->DrawString(CComBSTR(this->GetShipTypeAsString(TRUE)), -1, &font, PointF((REAL)pt.x + 120, (REAL)pt.y + 57), &fontFormat, &fontBrush);
		}
	}

	if (bDrawFleet)
	{
		// Anzahl der Schiffe in der Flotte (+1 weil das Führerschiff mitgezählt werden muß)
		fontBrush.SetColor(static_cast<Gdiplus::ARGB>(Color::White));
		s.Format("%d", FleetSize + 1);
		g->DrawString(CComBSTR(s), -1, &font, PointF((REAL)pt.x + 35, (REAL)pt.y + 30), &fontFormat, &fontBrush);
	}
}

void CShip::Repair(BOOL bAtShipPort, bool bFasterShieldRecharge) {
	m_Shield.RechargeShields(200 * (bFasterShieldRecharge + 1));
	// Wenn wir in diesem Sektor einen Shipport haben, dann wird die H?lle repariert
	if(bAtShipPort)
		m_Hull.RepairHull();
}

void CShip::ExtractDeuterium()
{
	int deut = 50;
	const CResearch& research = *m_Owner->GetEmpire()->GetResearch();
	deut += deut * min(research.GetEnergyTech(), research.GetPropulsionTech());
	if(CTrade::GetMonopolOwner(RESOURCES::DEUTERIUM) == OwnerID())
		deut *= 2;
	if(const int bonus = research.GetResearchInfo()->IsResearchedThenGetBonus(RESEARCH_COMPLEX::PRODUCTION, 2))
		deut += bonus * deut / 100;
	deut = min(deut, GetFreeStorageRoom());
	m_iLoadedResources[RESOURCES::DEUTERIUM] += deut;
}

void CShip::Retreat(const CPoint& ptRetreatSector, COMBAT_TACTIC::Typ const* NewCombatTactic)
{
	// womögicher Terraformplanet oder Stationsbau zurücknehmen
	UnsetCurrentOrder();
	// Rückzugsbefehl zurücknehmen
	if(NewCombatTactic)
		m_nCombatTactic = *NewCombatTactic;
	else
		SetCombatTacticAccordingToType();
	// Kann das Schiff überhaupt fliegen?
	if (m_iSpeed > 0)
	{
		m_Co = ptRetreatSector;
		// aktuell eingestellten Kurs löschen (nicht dass das Schiff wieder in den Gefahrensektor fliegt)
		m_TargetKO = CPoint(-1, -1);
	}
}

//most of the stuff from CalcShipEffects() for either a ship from the shiparray or a ship of its fleet
void CShip::CalcEffectsForSingleShip(CSector& sector, CRace* pRace,
			bool bDeactivatedShipScanner, bool bBetterScanner, bool fleetship) {
	const CString& sRace = pRace->GetRaceID();
	const bool major = pRace->IsMajor();
	if(!fleetship && major)
		sector.SetFullKnown(sRace);
	if (!bDeactivatedShipScanner) {
		// Scanstärke auf die Sektoren abhängig von der Scanrange übertragen
		sector.PutScannedSquare(GetScanRange(), GetScanPower(),
			*pRace, bBetterScanner, HasSpecial(SHIP_SPECIAL::PATROLSHIP));
	}
	// Schiffe, wenn wir dort nicht eine ausreichend hohe Scanpower haben. Ab Stealthstufe 4 muss das Schiff getarnt
	// sein, ansonsten gilt dort nur Stufe 3.
	if (!IsStation()) {
		// Im Sektor die NeededScanPower setzen, die wir brauchen um dort Schiffe zu sehen. Wir sehen ja keine getarnten
		// Schiffe, wenn wir dort nicht eine ausreichend hohe Scanpower haben. Ab Stealthstufe 4 muss das Schiff getarnt
		// sein, ansonsten gilt dort nur Stufe 3.
		short stealthPower = m_iStealthGrade;
		if(!GetCloak() && stealthPower > 3)
			stealthPower = 3;
		const short NeededScanPower = stealthPower * 20;
		if (NeededScanPower < sector.GetNeededScanPower(sRace))
			sector.SetNeededScanPower(NeededScanPower, sRace);
	}
	if(!fleetship) {
		// Wenn das Schiff gerade eine Station baut, so dies dem Sektor mitteilen
		const SHIP_ORDER::Typ current_order = GetCurrentOrder();
		if (IsDoingStationWork())
			sector.SetIsStationBuilding(current_order, sRace);
		// Wenn das Schiff gerade Terraform, so dies dem Planeten mitteilen
		else if (current_order == SHIP_ORDER::TERRAFORM)
			sector.Terraforming(*this);
	}
	if (major) {
		CMajor* pMajor = dynamic_cast<CMajor*>(pRace);
		// Schiffunterstützungkosten dem jeweiligen Imperium hinzufügen.
		pMajor->GetEmpire()->AddShipCosts(GetMaintenanceCosts());
		// die Schiffe in der Flotte beim modifizieren der Schiffslisten der einzelnen Imperien beachten
		pMajor->GetShipHistory()->ModifyShip(ShipHistoryInfo(), sector.CoordsName(true));
	}
	// Erfahrungspunkte der Schiffe anpassen
	CalcExp();
}

/// Funktion beachtet die erforschten Spezialforschungen einer Rasse und verbessert die
/// Eigenschaften der übergebenen Schiffes.
/// @param pShip Schiff welches durch Spezialforschungen eventuell verbessert wird
/// @param pShipOwner Zeiger auf den Besitzer des Schiffes
void CShip::AddSpecialResearchBoni(const boost::shared_ptr<const CRace> owner)
{
	const CResearchInfo* pInfo = NULL;
	if(!owner)
		pInfo = m_Owner->GetEmpire()->GetResearch()->GetResearchInfo();
	else
		pInfo = owner->GetEmpire()->GetResearch()->GetResearchInfo();
	if (!pInfo)
		return;

	// mögliche Verbesserungen durch die Spezialforschung werden hier beachtet
	// Spezialforschung #0: "Waffentechnik"
	if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
	{
		// 20% erhoehter Phaserschaden
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			for (int i = 0; i < GetBeamWeapons()->GetSize(); i++)
			{
				USHORT oldPower = GetBeamWeapons()->GetAt(i).GetBeamPower();
				GetBeamWeapons()->GetAt(i).SetBeamPower(oldPower + (oldPower * pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetBonus(1) / 100));
			}
		}
		// 20% erhoehte Torpedogenauigkeit
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			for (int i = 0; i < GetTorpedoWeapons()->GetSize(); i++)
			{
				BYTE oldAcc = GetTorpedoWeapons()->GetAt(i).GetAccuracy();
				GetTorpedoWeapons()->GetAt(i).SetAccuracy(oldAcc + (oldAcc * pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetBonus(2) / 100));
			}
		}
		// 20% erhoehte Schussfreuquenz
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		{
			for (int i = 0; i < GetBeamWeapons()->GetSize(); i++)
			{
				BYTE oldRate = GetBeamWeapons()->GetAt(i).GetRechargeTime();
				GetBeamWeapons()->GetAt(i).SetRechargeTime(oldRate	- (oldRate * pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetBonus(3) / 100));
			}
			for (int i = 0; i < GetTorpedoWeapons()->GetSize(); i++)
			{
				BYTE oldRate = GetTorpedoWeapons()->GetAt(i).GetTupeFirerate();
				GetTorpedoWeapons()->GetAt(i).SetTubeFirerate(oldRate - (oldRate * pInfo->GetResearchComplex(RESEARCH_COMPLEX::WEAPONS_TECHNOLOGY)->GetBonus(3) / 100));
			}
		}
	}
	// Spezialforschung #1: "Konstruktionstechnik"
	if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
	{
		// 20% bessere Schilde
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			UINT maxShield = GetShield()->GetMaxShield();
			BYTE shieldType = GetShield()->GetShieldType();
			BOOLEAN regenerative = GetShield()->GetRegenerative();
			GetShield()->ModifyShield((maxShield + (maxShield * pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetBonus(1) / 100)), shieldType, regenerative);
		}
		// 20% bessere Hülle
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			BOOLEAN doubleHull = GetHull()->GetDoubleHull();
			BOOLEAN ablative = GetHull()->GetAblative();
			BOOLEAN polarisation = GetHull()->GetPolarisation();
			UINT baseHull = GetHull()->GetBaseHull();
			BYTE hullMaterial = GetHull()->GetHullMaterial();
			GetHull()->ModifyHull(doubleHull, (baseHull + (baseHull * pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetBonus(2) / 100)), hullMaterial,ablative,polarisation);
		}
		// 50% stärkere Scanner
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		{
			USHORT scanPower = GetScanPower();
			SetScanPower(scanPower + (scanPower * pInfo->GetResearchComplex(RESEARCH_COMPLEX::CONSTRUCTION_TECHNOLOGY)->GetBonus(3) / 100));
		}
	}
	// Spezialforschung #2: "allgemeine Schiffstechnik"
	if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED)
	{
		// erhoehte Reichweite für Schiffe mit zuvor kurzer Reichweite
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			if (m_iRange == SHIP_RANGE::SHORT)
				SetRange((SHIP_RANGE::Typ)(pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(1)));
		}
		// erhoehte Geschwindigkeit für Schiffe mit Geschwindigkeit 1
		else if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			if (m_iSpeed == 1)
				SetSpeed((BYTE)(pInfo->GetResearchComplex(RESEARCH_COMPLEX::GENERAL_SHIP_TECHNOLOGY)->GetBonus(2)));
		}
	}
	// Spezialforschung #3: "friedliche Schiffstechnik"
	if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetComplexStatus() == RESEARCH_STATUS::RESEARCHED && GetShipType() <= SHIP_TYPE::COLONYSHIP)
	{
		// 25% erhoehte Transportkapazitaet
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
		{
			USHORT storage = GetStorageRoom();
			SetStorageRoom(storage + (storage * pInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetBonus(1) / 100));
		}
		// keine Unterhaltskosten
		if (pInfo->GetResearchComplex(RESEARCH_COMPLEX::PEACEFUL_SHIP_TECHNOLOGY)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
		{
			SetMaintenanceCosts(0);
		}
	}
}

bool CShip::BuildStation(SHIP_ORDER::Typ order, CSector& sector, CMajor& major, short id) {
	if(!sector.SetNeededStationPoints(m_iStationBuildPoints, OwnerID()))
		return false;
	SHIP_TYPE::Typ type = (order == SHIP_ORDER::BUILD_OUTPOST
		|| order == SHIP_ORDER::UPGRADE_OUTPOST) ? SHIP_TYPE::OUTPOST : SHIP_TYPE::STARBASE;
	sector.BuildStation(type, OwnerID());
	CEmpireNews message;
	const CString& s1 = (order == SHIP_ORDER::BUILD_OUTPOST) ? "OUTPOST_FINISHED"
		: ((order == SHIP_ORDER::BUILD_STARBASE) ? "STARBASE_FINISHED"
		: ((order == SHIP_ORDER::UPGRADE_OUTPOST) ? "OUTPOST_UPGRADE_FINISHED"
		: "STARBASE_UPGRADE_FINISHED" ));
	message.CreateNews(CLoc::GetString(s1),EMPIRE_NEWS_TYPE::MILITARY,"",sector.GetCo());
	major.GetEmpire()->AddMsg(message);
	// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
	const CString& s2 = (order == SHIP_ORDER::BUILD_OUTPOST) ? "OUTPOST_CONSTRUCTION"
		: ((order == SHIP_ORDER::BUILD_STARBASE) ? "STARBASE_CONSTRUCTION"
		: ((order == SHIP_ORDER::UPGRADE_OUTPOST) ? "OUTPOST_UPGRADE"
		: "STARBASE_UPGRADE" ));
	CBotEDoc* pDoc = resources::pDoc;
	major.AddToLostShipHistory(ShipHistoryInfo(), CLoc::GetString(s2),
		CLoc::GetString("DESTROYED"), pDoc->GetCurrentRound(), m_Co);
	resources::pClientWorker->CalcStationReady(type, major);
	pDoc->BuildShip(id, sector.GetCo(), OwnerID());
	// Wenn hier ein Au?enposten gebaut wurde den Befehl f?r die Flotte auf Meiden stellen
	UnsetCurrentOrder();
	return true;
}

void CShip::Scrap(CMajor& major, CSystem& sy, bool disassembly) {
	// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
	major.GetShipHistory()->ModifyShip(ShipHistoryInfo(), sy.CoordsName(true),
		resources::pDoc->GetCurrentRound(), CLoc::GetString(disassembly ?
		"DISASSEMBLY" : "UPGRADE"), CLoc::GetString("DESTROYED"));
	if(sy.OwnerID() != OwnerID())
		return;
	// wenn wir in dem Sector wo wir das Schiff demoniteren ein uns gehörendes System haben,
	// dann bekommen wir teilweise Rohstoffe aus der Demontage zurück (vlt. auch ein paar Credits)
	double proz = rand()%26 + 50;	// Wert zwischen 50 und 75 ausw?hlen
	// Wenn in dem System Gebäude stehen, wodurch der Prozentsatz erhöht wird, dann hier addieren
	proz += sy.GetProduction()->GetShipRecycling();
	USHORT id = m_iID - 10000;
	const CShipInfo& si = resources::pDoc-> m_ShipInfoArray.GetAt(id);
	GameResources res = si.GetNeededResources();
	const double factor = static_cast<double>(proz) / 100;
	res.Deritium = 0;
	res *= factor;
	sy.SetStores(res);
	major.GetEmpire()->SetCredits(si.GetNeededIndustry() * factor);
}

CShipHistoryStruct CShip::ShipHistoryInfo() const
{
	return CShipHistoryStruct(m_sName, GetShipTypeAsString(), m_strShipClass, "", "", GetCurrentOrderAsString(), "",
		GetCurrentTargetAsString(m_Owner), 0, 0, m_iCrewExperiance);
}

CString CShip::SanityCheckUniqueness(std::set<CString>& already_encountered) const {
	const std::set<CString>::const_iterator found = already_encountered.find(m_sName);
	if(found == already_encountered.end()) {
		already_encountered.insert(m_sName);
		return CString();
	}
	return m_sName;
}

bool CShip::SanityCheckOrdersConsistency(const CShip& with) const {
	//These orders are no longer allowed to be set;
	//they are executed immediately instead of at turn change
	if(m_iCurrentOrder == SHIP_ORDER::ATTACK
		|| m_iCurrentOrder == SHIP_ORDER::AVOID
		|| m_iCurrentOrder == SHIP_ORDER::ASSIGN_FLAGSHIP
		|| with.m_iCurrentOrder == SHIP_ORDER::ATTACK
		|| with.m_iCurrentOrder == SHIP_ORDER::AVOID
		|| with.m_iCurrentOrder == SHIP_ORDER::ASSIGN_FLAGSHIP)
		return false;
	AssertBotE(CanHaveOrder(with.m_iCurrentOrder, false));
	if(m_iCurrentOrder == SHIP_ORDER::TERRAFORM) {
		AssertBotE(0 <= m_nTerraformingPlanet &&
			m_nTerraformingPlanet < static_cast<int>(resources::pDoc->GetSystem(m_Co.x, m_Co.y).GetNumberOfPlanets()));
	}
	return m_iCurrentOrder == with.m_iCurrentOrder
		&& m_nCombatTactic == with.m_nCombatTactic
		&& m_Co == with.m_Co
		&& m_TargetKO == with.m_TargetKO
		&& m_nTerraformingPlanet == with.m_nTerraformingPlanet;
}
