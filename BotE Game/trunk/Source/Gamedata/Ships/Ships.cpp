// Ships.cpp: Implementierung der Klasse CShips.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ships.h"
#include "Races/race.h"
#include "Loc.h"
#include "system/System.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CShips::CShips() :
	CShip(),
	m_Fleet(),
	m_Key(0),
	m_bLeaderIsCurrent(true)
{
}

CShips::CShips(const CShip& ship) :
	CShip(ship),
	m_Fleet(),
	m_Key(0),
	m_bLeaderIsCurrent(true)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////

CShips::CShips(const CShips& o) :
	CShip(o),
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

	CShip::operator =(o);

	m_Fleet = o.m_Fleet;
	m_Key = o.m_Key;
	m_bLeaderIsCurrent = o.m_bLeaderIsCurrent;
	return *this;
}

CShips::~CShips()
{
	Reset();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CShips::Serialize(CArchive &ar)
{

	CShip::Serialize(ar);
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
	AssertBotE(!m_bLeaderIsCurrent && HasFleet());
	return m_Fleet.CurrentShip();
}

boost::shared_ptr<const CShips> CShips::at(unsigned key) const
{
	return m_Fleet.at(key);
}

boost::shared_ptr<CShips> CShips::at(unsigned key)
{
	return m_Fleet.at(key);
}

CString CShips::GetRangeAsString() const {
	const SHIP_RANGE::Typ range = GetRange(true);
	switch(range) {
		case SHIP_RANGE::SHORT: return CLoc::GetString("SHORT");
		case SHIP_RANGE::MIDDLE: return CLoc::GetString("MIDDLE");
		case SHIP_RANGE::LONG: return CLoc::GetString("LONG");
	}
	AssertBotE(false);
	return "";
}

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

void CShips::SetKO(int x, int y) {
	CShip::SetCo(x, y);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetKO(x, y);
}

//removes the element pointed to by the passed iterator from this fleet
//@param index: will be updated and point to the new position of the element which followed the erased one
void CShips::RemoveShipFromFleet(CShips::iterator& ship)
{
	m_Fleet.EraseAt(ship);
	if(!HasFleet())
		Reset();
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

		RemoveShipFromFleet(i);
	}

	// Wenn das Schiff selbst zerstört wurde
	// In der Schiffshistoryliste das Schiff als ehemaliges Schiff markieren
	return CShip::RemoveDestroyed(owner, round, sEvent, sStatus, destroyedShips, anomaly);
}

void CShips::Reset() {
	m_Fleet.Reset();
	m_bLeaderIsCurrent = true;
}

// Funktion übernimmt die Befehle des hier als Zeiger übergebenen Schiffsobjektes an alle Mitglieder der Flotte
void CShips::AdoptOrdersFrom(const CShips& ship)
{
	CShip::AdoptOrdersFrom(ship);
	for(CShips::iterator i = begin(); i != end(); ++i) {
		i->second->AdoptOrdersFrom(ship);
	}
}

void CShips::AddShipToFleet(const boost::shared_ptr<CShips>& fleet) {
	CString s;
	if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
		s.Format("CShips: adding ship with leader %s to fleet of %s", fleet->m_sName,
			m_sName);
		MYTRACE("ships")(MT::LEVEL_INFO, s);
	}
	AssertBotE(fleet->OwnerID() == OwnerID());
	const CShipMap::iterator i = m_Fleet.Add(fleet);
	const SHIP_ORDER::Typ order = GetCurrentOrder();
	AssertBotE(order != SHIP_ORDER::ASSIGN_FLAGSHIP);
	if(!i->second->CanHaveOrder(order, false))
		UnsetCurrentOrder();
	i->second->AdoptOrdersFrom(*this);
	if(fleet->HasFleet()) {
		if(MT::CMyTrace::IsLoggingEnabledFor("ships")) {
			s.Format("CShips: adding the fleet of leader %s to fleet of %s", fleet->m_sName,
				m_sName);
			MYTRACE("ships")(MT::LEVEL_INFO, s);
		}
		m_Fleet.Append(i->second->m_Fleet);
		i->second->Reset();
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
	CShip::ApplyTraining(XP, veteran);
	// Wenn das Schiff eine Flotte anführt, Schiffstraining auf alle Schiffe in der Flotte anwenden
	for(CShips::iterator i = begin(); i != end(); ++i)
	{
		const boost::shared_ptr<CShip>& leader = boost::static_pointer_cast<CShip>(i->second);
		leader->ApplyTraining(XP, veteran);
	}
}

bool CShips::ApplyIonstormEffects() {
	//conveniantly set the ship back to idle in case we can not improve further
	//doesn't work for fleets however, since that would require removing the finished ships from the fleet
	//to keep orders consistent, so unset the order for all once the first ship can no longer improve
	bool improvement_finished = false;
	if(CShip::ApplyIonstormEffects() && GetCurrentOrder() == SHIP_ORDER::IMPROVE_SHIELDS) {
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
	return CShip::UnassignFlagship();
}

void CShips::SetCloak(bool bCloakOn) {
	CShip::SetCloak(bCloakOn);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCloak(bCloakOn);
}

void CShips::SetTargetKO(const CPoint& TargetKO, const bool simple_setter) {
	CShip::SetTargetKO(TargetKO, simple_setter);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetTargetKO(TargetKO, simple_setter);
}

void CShips::SetCombatTactic(COMBAT_TACTIC::Typ nTactic, bool bPropagateToFleet) {
	CShip::SetCombatTactic(nTactic);
	if(bPropagateToFleet)
	{
		for(CShips::iterator i = begin(); i != end(); ++i)
			i->second->SetCombatTactic(nTactic);
	}
}

void CShips::SetTerraform(short planetNumber) {
	CShip::SetTerraform(planetNumber);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetTerraform(planetNumber);
}

void CShips::SetCurrentOrder(SHIP_ORDER::Typ nCurrentOrder) {
	CShip::SetCurrentOrder(nCurrentOrder);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCurrentOrder(nCurrentOrder);
}

void CShips::SetCurrentOrderAccordingToType() {
	CShip::SetCurrentOrderAccordingToType();
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCurrentOrderAccordingToType();
}

void CShips::SetCombatTacticAccordingToType() {
	CShip::SetCombatTacticAccordingToType();
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->SetCombatTacticAccordingToType();
}

void CShips::UnsetCurrentOrder() {
	CShip::UnsetCurrentOrder();
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->UnsetCurrentOrder();
}

//////////////////////////////////////////////////////////////////////
// calculated stements about this fleet (should be const functions, non-bool returning)
//////////////////////////////////////////////////////////////////////

// Funktion berechnet die Geschwindigkeit der Flotte.
unsigned CShips::GetSpeed(bool consider_fleet) const
{
	unsigned speed = m_iSpeed;
	//The bool parameter is probably not needed, but can't check the logic of all the calls atm.
	if(!consider_fleet)
		return speed;
	for(CShips::const_iterator i = begin(); i != end(); ++i)
		speed = min(i->second->GetSpeed(true), speed);
	//@todo this assert can probably be removed after enough testing (atm @r78019)
	AssertBotE(speed != 127);
	return speed;
}

// Funktion berechnet die Reichweite der Flotte.
SHIP_RANGE::Typ CShips::GetRange(bool consider_fleet) const
{
	SHIP_RANGE::Typ nRange = min(m_iRange, SHIP_RANGE::LONG);
	//The bool parameter is probably not needed, but can't check the logic of all the calls atm.
	if(!consider_fleet)
		return nRange;
	for(CShips::const_iterator i = begin(); i != end(); ++i)
		nRange = min(i->second->GetRange(true), nRange);

	return nRange;
}

CShips::RETREAT_MODE CShips::CalcRetreatMode() const
{
	//check the leader's order; if it retreats, try a complete retreat, if it stays, try a complete stay
	//otherwise the fleet is disassembled into its single ships
	const RETREAT_MODE result = m_nCombatTactic == COMBAT_TACTIC::CT_RETREAT ?
		RETREAT_MODE_COMPLETE : RETREAT_MODE_STAY;
	for(CShips::const_iterator i = begin(); i != end(); ++i) {
		const COMBAT_TACTIC::Typ tactic = i->second->GetCombatTactic();
		if(result == RETREAT_MODE_COMPLETE)
		{
			if(tactic != COMBAT_TACTIC::CT_RETREAT)
				return RETREAT_MODE_SPLITTED;
		}
		else //result == RETREAT_MODE_STAY
		{
			if(tactic == COMBAT_TACTIC::CT_RETREAT)
				return RETREAT_MODE_SPLITTED;
		}
	}
	return result;
}

// Funktion berechnet den Schiffstyp der Flotte. Wenn hier nur der selbe Schiffstyp in der Flotte vorkommt,
// dann gibt die Funktion diesen Schiffstyp zurück. Wenn verschiedene Schiffstypen in der Flotte vorkommen,
// dann liefert und die Funktion ein -1.
short CShips::GetFleetShipType() const
{
	for(CShips::const_iterator i = begin(); i != end(); ++i)
		if (i->second->GetShipType() != m_iShipType)
			return -1;
	return m_iShipType;
}

// Funktion berechnet die minimale Stealthpower der Flotte. Der Parameter der hier übergeben werden sollte
// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
unsigned CShips::GetStealthPower() const
{
	unsigned stealthPower = CShip::GetStealthPower();
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
	return CShip::CanHaveOrder(order, require_new);
}

bool CShips::HasFleet() const {
	return !m_Fleet.empty();
}

bool CShips::NeedsRepair() const {
	for(CShips::const_iterator i = begin(); i != end(); ++i) {
		if(i->second->NeedsRepair())
			return true;
	}
	return CShip::NeedsRepair();
}

bool CShips::FleetHasTroops() const {
	for(CShips::const_iterator j = begin(); j != end(); ++j)
		if(j->second->FleetHasTroops())
			return true;
	return CShip::HasTroops();
}

bool CShips::HasVeteran() const {
	for(CShips::const_iterator j = begin(); j != end(); ++j)
		if(j->second->HasVeteran())
			return true;
	return CShip::IsVeteran();
}

bool CShips::HasTarget() const {
	//targets should always be the same among the leader and fleet of a Chips
	return CShip::HasTarget();
}

bool CShips::CanCloak(bool consider_fleet) const {
	if(consider_fleet) {
		for(CShips::const_iterator j = begin(); j != end(); ++j)
			if(!j->second->CanCloak(consider_fleet))
				return false;
	}
	return CShip::CanCloak();
}

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

boost::shared_ptr<CShips> CShips::GiveFleetToFleetsFirstShip() {
	AssertBotE(HasFleet());
	// erstes Schiff aus der Flotte holen
	CShips::iterator i = begin();
	const boost::shared_ptr<CShips> new_fleet_ship(i->second);

	while(true)
	{
		++i;
		if(i == end())
			break;
		new_fleet_ship->AddShipToFleet(i->second);
	}
	Reset();
	return new_fleet_ship;
}

CString CShips::GetTooltip(bool bShowFleet) const
{
	if(bShowFleet && HasFleet())
		return CShip::GetTooltip(&CShip::FleetInfoForGetTooltip(
			GetFleetShipType(), GetRange(true), GetSpeed(true))
		);
	return CShip::GetTooltip();
}

void CShips::DrawShip(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt, bool bIsMarked,
	bool bOwnerUnknown, bool bDrawFleet, const Gdiplus::Color& clrNormal,
	const Gdiplus::Color& clrMark, const Gdiplus::Font& font) const {

	const bool draw_troop_symbol = bDrawFleet ? FleetHasTroops() : CShip::HasTroops();
	CShip::DrawShip(g, pGraphicPool, pt, bIsMarked, bOwnerUnknown, bDrawFleet && HasFleet(),
		clrNormal,clrMark, font, draw_troop_symbol, GetFleetShipType(), GetFleetSize());
}

void CShips::TraditionalRepair(BOOL bAtShipPort, bool bFasterShieldRecharge) {
	CShip::Repair(bAtShipPort, bFasterShieldRecharge);
	for(CShips::iterator i = begin(); i != end(); ++i)
		i->second->TraditionalRepair(bAtShipPort, bFasterShieldRecharge);
}

void CShips::RepairCommand(BOOL bAtShipPort, bool bFasterShieldRecharge, CShipMap& ships) {
	AssertBotE(GetCurrentOrder() == SHIP_ORDER::REPAIR);
	if(!bAtShipPort) {
		UnsetCurrentOrder();
		return;
	}
	for(CShips::iterator i = begin(); i != end();) {
		i->second->RepairCommand(bAtShipPort, bFasterShieldRecharge, ships);
		if(!i->second->NeedsRepair()) {
			ships.Add(i->second);
			RemoveShipFromFleet(i);
			continue;
		}
		++i;
	}
	CShip::Repair(bAtShipPort, bFasterShieldRecharge);
	if(!CShip::NeedsRepair()) {
		CShip::UnsetCurrentOrder();
		if(HasFleet())
			ships.Add(GiveFleetToFleetsFirstShip());
	}
}

void CShips::RetreatFleet(const CPoint& RetreatSector, COMBAT_TACTIC::Typ const* NewCombatTactic) {
	for(CShips::iterator j = begin(); j != end(); ++j)
		j->second->Retreat(RetreatSector, NewCombatTactic);
}

void CShips::CalcEffects(CSector& sector, CRace* pRace,
			bool bDeactivatedShipScanner, bool bBetterScanner) {

		CShip::CalcEffectsForSingleShip(sector, pRace, bDeactivatedShipScanner, bBetterScanner, false);
		// wenn das Schiff eine Flotte besitzt, dann die Schiffe in der Flotte auch beachten
		for(CShips::iterator j = begin(); j != end(); ++j)
		{
			const boost::shared_ptr<CShip>& leader = boost::static_pointer_cast<CShip>(j->second);
			leader->CalcEffectsForSingleShip(sector, pRace, bDeactivatedShipScanner, bBetterScanner, true);
		}
}

CShips::StationWorkResult CShips::BuildStation(SHIP_ORDER::Typ order, CSector& sector, CMajor& major, short id) {
	StationWorkResult result;
	// Wenn das Schiff eine Flotte anf?hrt, dann erstmal die Au?enpostenbaupunkte der Schiffe
	// in der Flotte beachten und gegebenfalls das Schiff aus der Flotte entfernen
	for(CShips::iterator j = begin(); j != end(); ++j)
	{
		if(j->second->BuildStation(order, sector, major, id).finished)
		{
			result.finished = true;
			// Das Schiff, welches die Station fertiggestellt hat aus der Flotte entfernen
			RemoveShipFromFleet(j);
			UnsetCurrentOrder();
			return result;
		}
	}
	if(CShip::BuildStation(order, sector, major, id))
	{
		result.finished = true;
		result.remove_leader = true;
		UnsetCurrentOrder();
	}
	return result;
}

void CShips::Scrap(CMajor& major, CSystem& sy, bool disassembly)
{
	for(CShips::const_iterator x = begin(); x != end(); ++x)
		x->second->Scrap(major, sy, disassembly);
	CShip::Scrap(major, sy, disassembly);
}

CString CShips::SanityCheckUniqueness(std::set<CString>& already_encountered) const {
	for(CShips::const_iterator i = begin(); i != end(); ++i) {
		const CString& duplicate = i->second->SanityCheckUniqueness(already_encountered);
		if(!duplicate.IsEmpty())
			return duplicate;
	}
	return CShip::SanityCheckUniqueness(already_encountered);
}

bool CShips::SanityCheckOrdersConsistency() const {
	for(CShips::const_iterator i = begin(); i != end(); ++i)
	{
		const boost::shared_ptr<CShip>& leader = boost::static_pointer_cast<CShip>(i->second);
		if(!leader->SanityCheckOrdersConsistency(*this))
			return false;
	}
	return CShip::SanityCheckOrdersConsistency(*this);
}
