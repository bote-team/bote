// Ships.cpp: Implementierung der Klasse CShips.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ships.h"

#include <cassert>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//IMPLEMENT_SERIAL (CShips, CObject, 1)

//////////////////////////////////////////////////////////////////////
// iterators
//////////////////////////////////////////////////////////////////////
CShips::const_iterator CShips::begin() const {
	return m_Fleet.begin();
}
CShips::const_iterator CShips::end() const {
	return m_Fleet.end();
}
CShips::iterator CShips::begin() {
	return m_Fleet.begin();
}
CShips::iterator CShips::end() {
	return m_Fleet.end();
}

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShips::CShips() :
	m_Leader(),
	m_Fleet()
{
}

CShips::CShips(const CShip& ship) :
	m_Leader(ship),
	m_Fleet()
{
}

CShips::~CShips()
{
	m_Fleet.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
//CShips::CShips(const CShips& rhs)
//{
//	if (this == &rhs)
//		return;
//
//	m_vShips.RemoveAll();
//	const CShipArray& shiparray = rhs.m_vShips;
//	for(CShipArray::const_iterator it = shiparray.begin(); it != shiparray.end(); ++it)
//		m_vShips.Add(m_vShips.end(), *it);
//}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
//CFleet & CFleet::operator=(const CFleet & rhs)
//{
//	if (this == &rhs)
//		return *this;
//
//	m_vShips.RemoveAll();
//	const CShipArray& shiparray = rhs.m_vShips;
//	for(CShipArray::const_iterator it = shiparray.begin(); it != shiparray.end(); ++it)
//		m_vShips.Add(m_vShips.end(), *it);
//
//	return *this;
//}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CShips::Serialize(CArchive &ar)
{
	/*CObject::Serialize(ar);*/

	m_Leader.Serialize(ar);
	m_Fleet.Serialize(ar);
}

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

//(none)

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

// Funktion um ein Schiff aus der Flotte zu entfernen. Das n-te Schiff in der Flotte wird entfernt
void CShips::RemoveShipFromFleet(UINT nIndex)
{
	RemoveShipFromFleet(begin() + nIndex);
}

//removes the element pointed to by the passed iterator from this fleet
//@param index: will be updated and point to the new position of the element which followed the erased one
void CShips::RemoveShipFromFleet(CShips::iterator& ship)
{
	if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
		CString s;
		s.Format("CShips: removing ship %s from fleet of %s", ship->m_Leader.GetShipName(),
			m_Leader.GetShipName());
		MYTRACE("ships")(MT::LEVEL_INFO, s);
	}
	const unsigned index = ship - begin();
	assert(index < static_cast<unsigned>(m_Fleet.GetSize()));
	m_Fleet.RemoveAt(ship);
}

// Funktion übernimmt die Befehle des hier als Zeiger übergebenen Schiffsobjektes an alle Mitglieder der Flotte
void CShips::AdoptCurrentOrders(const CShip* ship)
{
	for(CShips::iterator i = begin(); i != end(); ++i) {
		i->m_Leader.AdoptOrdersFrom(*ship);
	}
}

void CShips::AddShipToFleet(CShips& fleet) {
	CString s;
	if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
		s.Format("CShips: adding ship with leader %s to fleet of %s", fleet.m_Leader.GetShipName(),
			m_Leader.GetShipName());
		MYTRACE("ships")(MT::LEVEL_INFO, s);
	}
	CShip leader = fleet.m_Leader;
	leader.AdoptOrdersFrom(m_Leader);
	m_Fleet.Add(end(), fleet.m_Leader);
	if(fleet.HasFleet()) {
		if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
			s.Format("CShips: adding the fleet of leader %s to fleet of %s", fleet.m_Leader.GetShipName(),
				m_Leader.GetShipName());
			MYTRACE("ships")(MT::LEVEL_INFO, s);
		}
		m_Fleet.Append(end(), fleet.m_Fleet);
		PropagateOrdersToFleet();
		fleet.DeleteFleet();
	}
}

void CShips::PropagateOrdersToFleet()
{
	AdoptCurrentOrders(&m_Leader);
}

//////////////////////////////////////////////////////////////////////
// calculated stements about this fleet (should be const functions, non-bool returning)
//////////////////////////////////////////////////////////////////////

// Funktion berechnet die Geschwindigkeit der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger des Schiffsobjektes, welches die Flotte besitzt
unsigned CShips::GetFleetSpeed(const CShip* ship) const
{
	BYTE speed = 127;
	if (ship != NULL)
		speed = ship->GetSpeed();
	for(CShips::const_iterator i = begin(); i != end(); ++i)
		if(i->GetSpeed() < speed)
			speed = i->GetSpeed();
	if (speed == 127)
		speed = 0;
	return speed;
}

// Funktion berechnet die Reichweite der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger des Schiffsobjektes, welches die Flotte besitzt
SHIP_RANGE::Typ CShips::GetFleetRange(const CShip* pShip) const
{
	SHIP_RANGE::Typ nRange = SHIP_RANGE::LONG;
	if (pShip)
		nRange = min(pShip->GetRange(), nRange);

	for(CShips::const_iterator i = begin(); i != end(); ++i)
		nRange = min(i->GetRange(), nRange);

	return nRange;
}

// Funktion berechnet den Schiffstyp der Flotte. Wenn hier nur der selbe Schiffstyp in der Flotte vorkommt,
// dann gibt die Funktion diesen Schiffstyp zurück. Wenn verschiedene Schiffstypen in der Flotte vorkommen,
// dann liefert und die Funktion ein -1. Der Parameter der hier übergeben werden sollte ist der this-Zeiger
// des Schiffsobjektes, welches die Flotte besitzt
short CShips::GetFleetShipType() const
{
	short type = m_Leader.GetShipType();
	for(CShips::const_iterator i = begin(); i != end(); ++i)
		if (i->GetShipType() != type)
			return -1;
	return type;
}

// Funktion berechnet die minimale Stealthpower der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
BYTE CShips::GetFleetStealthPower(const CShip* ship) const
{
	BYTE stealthPower = MAXBYTE;
	if (ship != NULL)
	{
		stealthPower = ship->GetStealthPower() * 20;
		if (ship->GetStealthPower() > 3 && ship->GetCloak() == false)
			stealthPower = 3 * 20;
	}
	if (stealthPower == 0)
		return 0;

	for(CShips::const_iterator i = begin(); i != end(); ++i)
	{
		if (stealthPower == 0)
			return 0;

		BYTE fleetStealthPower = i->GetStealthPower() * 20;
		if (i->GetStealthPower() > 3  && !i->GetCloak())
			fleetStealthPower = 3 * 20;
		if (fleetStealthPower < stealthPower)
			stealthPower = fleetStealthPower;
	}

	return stealthPower;
}

//////////////////////////////////////////////////////////////////////
// bool statements about this fleet or the ship leading it, should be const
//////////////////////////////////////////////////////////////////////

// Diese Funktion liefert TRUE wenn die Flotte den "order" ausführen kann. Als Schiffszeiger muß das Schiff
// übergeben werden, welches die Flotte beinhaltet. Kann die Flotte den Befehl nicht befolgen liefert die
// Funktion FALSE zurück
BOOLEAN CShips::CheckOrder(SHIP_ORDER::Typ nOrder) const
{
//
//	Schiffsbefehle
//	AVOID               0
//	ATTACK              1
//	CLOAK               2
//	ATTACK_SYSTEM       3
//	RAID_SYSTEM         4
//	BLOCKADE_SYSTEM		5
//	DESTROY_SHIP        6
//	COLONIZE            7
//	TERRAFORM           8
//	BUILD_OUTPOST       9
//	BUILD_STARBASE		10
//	ASSIGN_FLAGSHIP     11
//	CREATE_FLEET        12
//	TRANSPORT			13
//	FOLLOW_SHIP			14
//	TRAIN_SHIP			15
//	WAIT_SHIP_ORDER		16
//	SENTRY_SHIP_ORDER	17
//	REPAIR				18
//
	// AVOID, ATTACK, RAID_SYSTEM, DESTROY_SHIP, CREATE_FLEET, FOLLOW_SHIP, TRAIN_SHIP, WAIT_SHIP_ORDER, SENTRY_SHIP_ORDER, REPAIR
	// können wenn möglich immer von jedem Schiff ausgeführt werden. Deshalb können wir hier immer
	// ein TRUE zurückgeben bzw. es ist erst gar nicht nötig die "CheckOrder" Funktion aufzurufen
	if (nOrder == SHIP_ORDER::AVOID || nOrder == SHIP_ORDER::ATTACK || nOrder == SHIP_ORDER::RAID_SYSTEM || nOrder == SHIP_ORDER::DESTROY_SHIP
		|| nOrder == SHIP_ORDER::CREATE_FLEET || nOrder == SHIP_ORDER::FOLLOW_SHIP || nOrder == SHIP_ORDER::TRAIN_SHIP
		|| nOrder == SHIP_ORDER::WAIT_SHIP_ORDER || nOrder == SHIP_ORDER::SENTRY_SHIP_ORDER
		|| nOrder == SHIP_ORDER::REPAIR)
		return TRUE;

	// ASSIGN_FLAGSHIP und TRANSPORT können nicht als Befehl an eine Flotte gegeben werden, daher wird hier
	// ein FALSE zurückgegeben
	else if (nOrder == SHIP_ORDER::ASSIGN_FLAGSHIP || nOrder == SHIP_ORDER::TRANSPORT)
		return FALSE;

	// bei den restlichen Befehlen müssen wir einige Berechnungen anstellen.
	else
	{
		// Tarnen können wir die gesamte Flotte nur, wenn in ihr auch nur Schiffe vorkommen, die die Tarn-
		// fähigkeit besitzen
		if (nOrder == SHIP_ORDER::CLOAK)
		{
			if (m_Leader.GetStealthPower() < 4)
				return FALSE;
			for(CShips::const_iterator i = begin(); i != end(); ++i)
			{
				if (i->GetStealthPower() < 4)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben kann sich jedes Schiff in der Flotte tarnen
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Wenn der Befehl kolonisieren oder terraformen lautet kann das die gesamte Flotte nur, wenn jedes
		// Schiff in der Flotte "ColonizePoints" beitzt
		else if (nOrder == SHIP_ORDER::COLONIZE || nOrder == SHIP_ORDER::TERRAFORM)
		{
			if (m_Leader.GetColonizePoints() < 1)
				return FALSE;
			for(CShips::const_iterator i = begin(); i != end(); ++i)
			{
				if (i->GetColonizePoints() < 1)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben besitzt jedes Schiff in der Flotte "ColonizePoints"
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Wenn der Befehl Außenposten oder Sternbasis bauen lautet kann das die gesamte Flotte nur, wenn jedes
		// Schiff in der Flotte "StationBuildPoints" beitzt
		else if (nOrder == SHIP_ORDER::BUILD_OUTPOST || nOrder == SHIP_ORDER::BUILD_STARBASE)
		{
			if (m_Leader.GetStationBuildPoints() < 1)
				return FALSE;
			for(CShips::const_iterator i = begin(); i != end(); ++i)
			{
				if (i->GetStationBuildPoints() < 1)
					return FALSE;
			}
			// Haben wir bis jetzt kein FALSE zurückgegeben besitzt jedes Schiff in der Flotte "StationBuildPoints"
			// und wir können ein TRUE zurückgeben
			return TRUE;
		}
		// Bei einem Blockadebefehl müssen alle Schiffe in der Flotte die Eigenschaft "Blockadeschiff" besitzen
		else if (nOrder == SHIP_ORDER::BLOCKADE_SYSTEM)
		{
			if (!m_Leader.HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
				return FALSE;
			for(CShips::const_iterator i = begin(); i != end(); ++i)
			{
				if (!i->HasSpecial(SHIP_SPECIAL::BLOCKADESHIP))
					return FALSE;
			}
			return TRUE;
		}
		// Bei einem Systemangriff müssen alle Schiffe in der Flotte ungetarnt sein
		else if (nOrder == SHIP_ORDER::ATTACK_SYSTEM)
		{
			if (m_Leader.GetCloak())
				return FALSE;
			for(CShips::const_iterator i = begin(); i != end(); ++i)
			{
				if (i->GetCloak())
					return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

bool CShips::AllOnTactic(COMBAT_TACTIC::Typ tactic) const {
	for(CShips::const_iterator i = begin(); i != end(); ++i)
	{
		if(i->GetCombatTactic() != tactic)
			return false;
	}
	return true;
}

bool CShips::HasFleet() const {
	return !m_Fleet.empty();
}

bool CShips::NeedsRepair() const {
	for(CShips::const_iterator i = begin(); i != end(); ++i) {
		if(i->NeedsRepair())
			return true;
	}
	return m_Leader.NeedsRepair();
}

bool CShips::FleetHasTroops() const {
	for(CShips::const_iterator j = begin(); j != end(); ++j)
		if(j->FleetHasTroops())
			return true;
	return m_Leader.HasTroops();
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

CShips CShips::GiveFleetToFleetsFirstShip() {
	assert(HasFleet());
	// erstes Schiff aus der Flotte holen
	CShips::iterator i = begin();
	CShips new_fleet_ship = *i;

	while(true)
	{
		++i;
		if(i == end())
			break;
		new_fleet_ship.AddShipToFleet(*i);
	}
	DeleteFleet();
	return new_fleet_ship;
}

CString CShips::GetTooltip(bool bShowFleet)
{
	if(bShowFleet && HasFleet())
		return m_Leader.GetTooltip(&CShip::FleetInfoForGetTooltip(
			GetFleetShipType(), GetFleetRange(&m_Leader), GetFleetSpeed(&m_Leader))
		);
	return m_Leader.GetTooltip();
}

void CShips::DrawShip(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt, bool bIsMarked,
	bool bOwnerUnknown, bool bDrawFleet, const Gdiplus::Color& clrNormal,
	const Gdiplus::Color& clrMark, const Gdiplus::Font& font) const {

	m_Leader.DrawShip(g, pGraphicPool, pt, bIsMarked, bOwnerUnknown, bDrawFleet && HasFleet(),
		clrNormal,clrMark, font, FleetHasTroops(), GetFleetShipType(), GetFleetSize());
}

void CShips::Repair(BOOL bAtShipPort, bool bFasterShieldRecharge) {
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->Repair(bAtShipPort, bFasterShieldRecharge);

	if(m_Leader.GetCurrentOrder() == SHIP_ORDER::REPAIR && (!NeedsRepair() || !bAtShipPort))
		m_Leader.UnsetCurrentOrder();
}

void CShips::RetreatFleet(const CPoint& RetreatSector) {
	for(CShips::iterator j = begin(); j != end(); ++j)
		j->Retreat(RetreatSector);
}

void CShips::CalcEffects(CSector& sector, CRace* pRace,
			bool bDeactivatedShipScanner, bool bBetterScanner) {

		m_Leader.CalcEffectsForSingleShip(sector, pRace, bDeactivatedShipScanner, bBetterScanner, false);
		// wenn das Schiff eine Flotte besitzt, dann die Schiffe in der Flotte auch beachten
		for(CShips::iterator j = begin(); j != end(); ++j)
			j->m_Leader.CalcEffectsForSingleShip(sector, pRace, bDeactivatedShipScanner, bBetterScanner, true);
}

CString CShips::SanityCheckUniqueness(std::set<CString>& already_encountered) const {
	for(CShips::const_iterator i = begin(); i != end(); ++i) {
		const CString& duplicate = i->SanityCheckUniqueness(already_encountered);
		if(!duplicate.IsEmpty())
			return duplicate;
	}
	return m_Leader.SanityCheckUniqueness(already_encountered);
}