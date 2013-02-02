// Ships.cpp: Implementierung der Klasse CShips.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ships.h"
#include "Races/race.h"
#include "Loc.h"

#include <cassert>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//IMPLEMENT_SERIAL (CShips, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShips::CShips() :
	m_Leader(),
	m_Fleet(),
	m_Key(0),
	m_bLeaderIsCurrent(true)
{
}

CShips::CShips(const CShip& ship) :
	m_Leader(ship),
	m_Fleet(),
	m_Key(0),
	m_bLeaderIsCurrent(true)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////

CShips::CShips(const CShips& o) :
	m_Leader(o.m_Leader),
	m_Fleet(o.m_Fleet),
	m_Key(o.m_Key),
	m_bLeaderIsCurrent(o.m_bLeaderIsCurrent)
{
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////

CShips& CShips::operator=(const CShips& o)
{
	if(this == &o)
		return *this;
	m_Leader = o.m_Leader;
	m_Fleet = o.m_Fleet;
	m_Key = o.m_Key;
	m_bLeaderIsCurrent = o.m_bLeaderIsCurrent;
	return *this;
}

CShips::~CShips()
{
	Reset(true);
}

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

CShips::const_iterator CShips::find(unsigned key) const {
	return m_Fleet.find(key);
}
CShips::iterator CShips::find(unsigned key) {
	return m_Fleet.find(key);
}

CShips::const_iterator CShips::iterator_at(int index) const{
	return m_Fleet.iterator_at(index);
}
CShips::iterator CShips::iterator_at(int index) {
	return m_Fleet.iterator_at(index);
}

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

const CShips::const_iterator& CShips::CurrentShip() const {
	assert(!m_bLeaderIsCurrent && HasFleet());
	return m_Fleet.CurrentShip();
}

const CShips& CShips::at(unsigned key) const {
	return m_Fleet.at(key);
}
CShips& CShips::at(unsigned key) {
	return m_Fleet.at(key);
}

CString CShips::GetRangeAsString() const {
	const SHIP_RANGE::Typ range = GetRange(true);
	switch(range) {
		case SHIP_RANGE::SHORT: return CLoc::GetString("SHORT");
		case SHIP_RANGE::MIDDLE: return CLoc::GetString("MIDDLE");
		case SHIP_RANGE::LONG: return CLoc::GetString("LONG");
	}
	assert(false);
	return "";
}

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

void CShips::SetKO(int x, int y) {
	m_Leader.SetKO(x, y);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetKO(x, y);
}

//removes the element pointed to by the passed iterator from this fleet
//@param index: will be updated and point to the new position of the element which followed the erased one
void CShips::RemoveShipFromFleet(CShips::iterator& ship, bool destroy)
{
	//if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
	//	CString s;
	//	s.Format("CShips: removing ship %s from fleet of %s", ship->m_Leader.GetShipName(),
	//		m_Leader.GetShipName());
	//	MYTRACE("ships")(MT::LEVEL_INFO, s);
	//}
	m_Fleet.EraseAt(ship, destroy);
	if(!HasFleet())
		Reset(destroy);
}

bool CShips::RemoveDestroyed(CRace& owner, unsigned short round, const CString& sEvent, const CString& sStatus, CStringArray* destroyedShips, const CString& anomaly)
{
	// Wenn das Schiff eine Flotte hatte, dann erstmal nur die Schiffe in der Flotte beachten
	// Wenn davon welche zerstört wurden diese aus der Flotte nehmen
	for(CShips::iterator i = begin(); i != end();)
	{
		if (i->second->RemoveDestroyed(owner, round, sEvent, sStatus, destroyedShips, anomaly))
		{
			++i;
			continue;
		}

		RemoveShipFromFleet(i, true);
	}

	// Wenn das Schiff selbst zerstört wurde
	// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
	return m_Leader.RemoveDestroyed(owner, round, sEvent, sStatus, destroyedShips, anomaly);
}

void CShips::Reset(bool destroy) {
	m_Fleet.Reset(destroy);
	m_bLeaderIsCurrent = true;
}

// Funktion übernimmt die Befehle des hier als Zeiger übergebenen Schiffsobjektes an alle Mitglieder der Flotte
void CShips::AdoptOrdersFrom(const CShips& ship)
{
	m_Leader.AdoptOrdersFrom(ship.m_Leader);
	for(CShips::iterator i = begin(); i != end(); ++i) {
		i->second->AdoptOrdersFrom(ship);
	}
}

void CShips::AddShipToFleet(CShips* fleet) {
	CString s;
	if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
		s.Format("CShips: adding ship with leader %s to fleet of %s", fleet->m_Leader.GetShipName(),
			m_Leader.GetShipName());
		MYTRACE("ships")(MT::LEVEL_INFO, s);
	}
	assert(fleet->GetOwnerOfShip() == GetOwnerOfShip());
	const CShipMap::iterator i = m_Fleet.Add(fleet);
	const SHIP_ORDER::Typ order = GetCurrentOrder();
	assert(order != SHIP_ORDER::ASSIGN_FLAGSHIP);
	if(!i->second->CanHaveOrder(order, false))
		UnsetCurrentOrder();
	i->second->AdoptOrdersFrom(*this);
	if(fleet->HasFleet()) {
		if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
			s.Format("CShips: adding the fleet of leader %s to fleet of %s", fleet->m_Leader.GetShipName(),
				m_Leader.GetShipName());
			MYTRACE("ships")(MT::LEVEL_INFO, s);
		}
		m_Fleet.Append(i->second->m_Fleet);
		i->second->Reset(false);
	}
}

void CShips::SetCurrentShip(const CShips::iterator& position)
{
	if(position == end())
	{
		m_bLeaderIsCurrent = true;
		return;
	}
	m_bLeaderIsCurrent = false;
	m_Fleet.SetCurrentShip(position);
}

void CShips::ApplyTraining(int XP) {
	const bool veteran = HasVeteran();
	m_Leader.ApplyTraining(XP, veteran);
	// Wenn das Schiff eine Flotte anführt, Schiffstraining auf alle Schiffe in der Flotte anwenden
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->m_Leader.ApplyTraining(XP, veteran);
}

bool CShips::ApplyIonstormEffects() {
	//conveniantly set the ship back to idle in case we can not improve further
	//doesn't work for fleets however, since that would require removing the finished ships from the fleet
	//to keep orders consistent, so unset the order for all once the first ship can no longer improve
	bool improvement_finished = false;
	if(m_Leader.ApplyIonstormEffects() && GetCurrentOrder() == SHIP_ORDER::IMPROVE_SHIELDS) {
		UnsetCurrentOrder();
		improvement_finished = true;
	}
	for(CShips::iterator i = begin(); i != end(); ++i)
		if(i->second->ApplyIonstormEffects() && GetCurrentOrder() == SHIP_ORDER::IMPROVE_SHIELDS) {
			UnsetCurrentOrder();
			improvement_finished = true;
		}
	return improvement_finished;
}

bool CShips::UnassignFlagship() {
	for(CShips::iterator i = begin(); i != end(); ++i) {
		if(i->second->UnassignFlagship())
			return true;
	}
	return m_Leader.UnassignFlagship();
}

void CShips::SetCloak(bool bCloakOn) {
	m_Leader.SetCloak(bCloakOn);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCloak(bCloakOn);
}

void CShips::SetTargetKO(const CPoint& TargetKO, const bool simple_setter) {
	m_Leader.SetTargetKO(TargetKO, simple_setter);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetTargetKO(TargetKO, simple_setter);
}

void CShips::SetCombatTactic(COMBAT_TACTIC::Typ nTactic) {
	m_Leader.SetCombatTactic(nTactic);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCombatTactic(nTactic);
}

void CShips::SetTerraform(short planetNumber) {
	m_Leader.SetTerraform(planetNumber);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetTerraform(planetNumber);
}

void CShips::SetCurrentOrder(SHIP_ORDER::Typ nCurrentOrder) {
	m_Leader.SetCurrentOrder(nCurrentOrder);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCurrentOrder(nCurrentOrder);
}

void CShips::SetCurrentOrderAccordingToType() {
	m_Leader.SetCurrentOrderAccordingToType();
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCurrentOrderAccordingToType();
}

void CShips::SetCombatTacticAccordingToType() {
	m_Leader.SetCombatTacticAccordingToType();
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCombatTacticAccordingToType();
}

void CShips::UnsetCurrentOrder() {
	m_Leader.UnsetCurrentOrder();
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->UnsetCurrentOrder();
}

//////////////////////////////////////////////////////////////////////
// calculated stements about this fleet (should be const functions, non-bool returning)
//////////////////////////////////////////////////////////////////////

// Funktion berechnet die Geschwindigkeit der Flotte.
unsigned CShips::GetSpeed(bool consider_fleet) const
{
	unsigned speed = m_Leader.GetSpeed();
	//The bool parameter is probably not needed, but can't check the logic of all the calls atm.
	if(!consider_fleet)
		return speed;
	for(CShips::const_iterator i = begin(); i != end(); ++i)
		speed = min(i->second->GetSpeed(true), speed);
	//@todo this assert can probably be removed after enough testing (atm @r78019)
	assert(speed != 127);
	return speed;
}

// Funktion berechnet die Reichweite der Flotte.
SHIP_RANGE::Typ CShips::GetRange(bool consider_fleet) const
{
	SHIP_RANGE::Typ nRange = min(m_Leader.GetRange(), SHIP_RANGE::LONG);
	//The bool parameter is probably not needed, but can't check the logic of all the calls atm.
	if(!consider_fleet)
		return nRange;
	for(CShips::const_iterator i = begin(); i != end(); ++i)
		nRange = min(i->second->GetRange(true), nRange);

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
		if (i->second->GetShipType() != type)
			return -1;
	return type;
}

// Funktion berechnet die minimale Stealthpower der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
unsigned CShips::GetStealthPower() const
{
	unsigned stealthPower = m_Leader.GetStealthPower();
	for(CShips::const_iterator i = begin(); i != end(); ++i)
	{
		if (stealthPower == 0)
			return 0;
		stealthPower = min(stealthPower, i->second->GetStealthPower());
	}
	return stealthPower;
}

//////////////////////////////////////////////////////////////////////
// bool statements about this fleet or the ship leading it, should be const
//////////////////////////////////////////////////////////////////////

//// Diese Funktion liefert true wenn die Flotte den "order" ausführen kann.
//// Kann die Flotte den Befehl nicht befolgen liefert die Funktion false zurück
bool CShips::CanHaveOrder(SHIP_ORDER::Typ order, bool require_new, bool require_all_can) const {
	if(HasFleet())
	{
		if (order == SHIP_ORDER::ASSIGN_FLAGSHIP)
			return false;
		if(require_all_can) {
			for(CShips::const_iterator i = m_Fleet.begin(); i != m_Fleet.end(); ++i)
				if(!i->second->CanHaveOrder(order, require_new))
					return false;
		}
		else
			for(CShips::const_iterator i = m_Fleet.begin(); i != m_Fleet.end(); ++i)
				if(i->second->CanHaveOrder(order, require_new, false))
					return true;
	}
	return m_Leader.CanHaveOrder(order, require_new);
}

bool CShips::AllOnTactic(COMBAT_TACTIC::Typ tactic) const {
	for(CShips::const_iterator i = begin(); i != end(); ++i)
	{
		if(i->second->GetCombatTactic() != tactic)
			return false;
	}
	return true;
}

bool CShips::HasFleet() const {
	return !m_Fleet.empty();
}

bool CShips::NeedsRepair() const {
	for(CShips::const_iterator i = begin(); i != end(); ++i) {
		if(i->second->NeedsRepair())
			return true;
	}
	return m_Leader.NeedsRepair();
}

bool CShips::FleetHasTroops() const {
	for(CShips::const_iterator j = begin(); j != end(); ++j)
		if(j->second->FleetHasTroops())
			return true;
	return m_Leader.HasTroops();
}

bool CShips::HasVeteran() const {
	for(CShips::const_iterator j = begin(); j != end(); ++j)
		if(j->second->HasVeteran())
			return true;
	return m_Leader.IsVeteran();
}

bool CShips::HasTarget() const {
	//targets should always be the same among the leader and fleet of a Chips
	return m_Leader.HasTarget();
}

bool CShips::CanCloak(bool consider_fleet) const {
	if(consider_fleet) {
		for(CShips::const_iterator j = begin(); j != end(); ++j)
			if(!j->second->CanCloak(consider_fleet))
				return false;
	}
	return m_Leader.CanCloak();
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

CShips* CShips::GiveFleetToFleetsFirstShip() {
	assert(HasFleet());
	// erstes Schiff aus der Flotte holen
	CShips::iterator i = begin();
	CShips* new_fleet_ship = i->second;

	while(true)
	{
		++i;
		if(i == end())
			break;
		new_fleet_ship->AddShipToFleet(i->second);
	}
	Reset(false);
	return new_fleet_ship;
}

CString CShips::GetTooltip(bool bShowFleet)
{
	if(bShowFleet && HasFleet())
		return m_Leader.GetTooltip(&CShip::FleetInfoForGetTooltip(
			GetFleetShipType(), GetRange(true), GetSpeed(true))
		);
	return m_Leader.GetTooltip();
}

void CShips::DrawShip(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt, bool bIsMarked,
	bool bOwnerUnknown, bool bDrawFleet, const Gdiplus::Color& clrNormal,
	const Gdiplus::Color& clrMark, const Gdiplus::Font& font) const {

	const bool draw_troop_symbol = bDrawFleet ? FleetHasTroops() : m_Leader.HasTroops();
	m_Leader.DrawShip(g, pGraphicPool, pt, bIsMarked, bOwnerUnknown, bDrawFleet && HasFleet(),
		clrNormal,clrMark, font, draw_troop_symbol, GetFleetShipType(), GetFleetSize());
}

void CShips::TraditionalRepair(BOOL bAtShipPort, bool bFasterShieldRecharge) {
	m_Leader.Repair(bAtShipPort, bFasterShieldRecharge);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->TraditionalRepair(bAtShipPort, bFasterShieldRecharge);
}

void CShips::RepairCommand(BOOL bAtShipPort, bool bFasterShieldRecharge, CShipMap& ships) {
	assert(GetCurrentOrder() == SHIP_ORDER::REPAIR);
	if(!bAtShipPort) {
		UnsetCurrentOrder();
		return;
	}
	for(CShips::iterator i = begin(); i != end();) {
		i->second->RepairCommand(bAtShipPort, bFasterShieldRecharge, ships);
		if(!i->second->NeedsRepair()) {
			ships.Add(i->second);
			RemoveShipFromFleet(i, false);
			continue;
		}
		++i;
	}
	m_Leader.Repair(bAtShipPort, bFasterShieldRecharge);
	if(!m_Leader.NeedsRepair()) {
		m_Leader.UnsetCurrentOrder();
		if(HasFleet())
			ships.Add(GiveFleetToFleetsFirstShip());
	}
}

void CShips::RetreatFleet(const CPoint& RetreatSector) {
	for(CShips::iterator j = begin(); j != end(); ++j)
		j->second->Retreat(RetreatSector);
}

void CShips::CalcEffects(CSector& sector, CRace* pRace,
			bool bDeactivatedShipScanner, bool bBetterScanner) {

		m_Leader.CalcEffectsForSingleShip(sector, pRace, bDeactivatedShipScanner, bBetterScanner, false);
		// wenn das Schiff eine Flotte besitzt, dann die Schiffe in der Flotte auch beachten
		for(CShips::iterator j = begin(); j != end(); ++j)
			j->second->m_Leader.CalcEffectsForSingleShip(sector, pRace, bDeactivatedShipScanner, bBetterScanner, true);
}

CString CShips::SanityCheckUniqueness(std::set<CString>& already_encountered) const {
	for(CShips::const_iterator i = begin(); i != end(); ++i) {
		const CString& duplicate = i->second->SanityCheckUniqueness(already_encountered);
		if(!duplicate.IsEmpty())
			return duplicate;
	}
	return m_Leader.SanityCheckUniqueness(already_encountered);
}

bool CShips::SanityCheckOrdersConsistency() const {
	for(CShips::const_iterator i = begin(); i != end(); ++i)
		if(!i->second->m_Leader.SanityCheckOrdersConsistency(m_Leader))
			return false;
	return m_Leader.SanityCheckOrdersConsistency(m_Leader);
}
