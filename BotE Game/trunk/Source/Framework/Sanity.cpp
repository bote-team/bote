#include "stdafx.h"
#include "Sanity.h"

#include "Galaxy/Sector.h"
#include "System/System.h"
#include "Races/race.h"
#include "BotEDoc.h"
#include "Races/RaceController.h"
#include "Ships/Ships.h"



//@file
// sanity checks of game data which are executed at runtime
// combines nicely with the --autoturn command line parameter

CSanity::CSanity(void) : notified(false) {}
CSanity::~CSanity(void) {};

CSanity* CSanity::GetInstance() {
	static CSanity instance;
	return &instance;
}

//These debugging functions check that all ships which don't move have target CPoint(-1, -1) set
//instead of their current coordinates. Later on, the places that handle the case of
//(target coords)==(current coords) can be removed (filled with AssertBotE(false)).

void CSanity::Notify(const CString& s, bool bPopup) {
	CString sMessage;
	sMessage.Format("%s This is a bug, please report.", s);
	MYTRACE("general")(MT::LEVEL_WARNING, sMessage);
	if(bPopup && !notified) {
		notified = true;
		AfxMessageBox(sMessage);
	}
}

void CSanity::CheckShipTargetCoordinates(const CShips& ship)
{
	SHIP_ORDER::Typ order = ship.GetCurrentOrder();
	const CPoint& co = ship.GetCo();
	const CPoint& tco = ship.GetTargetKO();
	//orders which match the fact that the ship has a target != CPoint(-1, -1) set
	if(order == SHIP_ORDER::NONE || order == SHIP_ORDER::ENCLOAK || order == SHIP_ORDER::DECLOAK || order == SHIP_ORDER::DESTROY_SHIP) {
		//It still should be an actual target and not the ship's coordinates
		if(tco != co)
			return;
	}

	//we no longer allow to unset a target by setting it to the current coordinates
	if(tco != CPoint(-1, -1)) {
		CString s;
		s.Format("The %s from %s at (%u, %u) has target (%u, %u) set and current order is %s.",
			ship.GetName(),
			ship.GetOwnerOfShip(),
			co.x, co.y,
			tco.x, tco.y,
			ship.GetCurrentOrderAsString());
		Notify(s);
	}
}

void CSanity::SanityCheckShip(const CShips& ship)
{
	CheckShipTargetCoordinates(ship);

	const CPoint& co = ship.GetCo();
	if(ship.GetTerraform() != -1 && ship.GetCurrentOrder() != SHIP_ORDER::TERRAFORM
		|| ship.GetTerraform() == -1 && ship.GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
	{
		CString s;
		s.Format("The %s from %s at (%u, %u) has m_nTerraformingPlanet %i set but current order is %s.",
			ship.GetName(),
			ship.GetOwnerOfShip(),
			co.x, co.y,
			ship.GetTerraform(),
			ship.GetCurrentOrderAsString());
		Notify(s);
	}
	if(!ship.IsAlive()) {
		CString s;
		s.Format("The %s from %s at (%u, %u) has hull < 1 but wasn't removed!",
			ship.GetName(),
			ship.GetOwnerOfShip(),
			co.x, co.y);
		Notify(s);
	}
	if(!ship.CanHaveOrder(ship.GetCurrentOrder(), false)) {
		CString s;
		s.Format("The %s from %s at (%u, %u) has order %s which it should not be allowed to have!",
			ship.GetName(),
			ship.GetOwnerOfShip(),
			co.x, co.y,
			ship.GetCurrentOrderAsString());
		Notify(s);
	}
}

void CSanity::SanityCheckFleet(const CShips& ship)
{
	SanityCheckShip(ship);
	for(CShips::const_iterator i = ship.begin(); i != ship.end(); ++i) {
		AssertBotE(!i->second->HasFleet());
		SanityCheckShip(*i->second);
	}
	if(!ship.SanityCheckOrdersConsistency())
		Notify("inconsistent orders!");
}

void CSanity::CheckShipUniqueness(const CShips& ship, std::set<CString>& already_encountered) {
	const CString& duplicate = ship.SanityCheckUniqueness(already_encountered);
	if(duplicate.IsEmpty())
		return;
	CString s;
	s.Format("Duplicate ship named %s in fleet of %s!", duplicate, ship.GetName());
	Notify(s);
}

void CSanity::SanityCheckSectorAndSystem(const CSystem& system)
{
	AssertBotE(system.CheckSanity());
}

//void CSanity::ShipInfo(const CArray<CShip, CShip>& shiparray, int index, const CString& indexname) {
//	if(!MT::CMyTrace::IsLoggingEnabledFor("shipindices"))
//		return;
//	CString s;
//	s.Format("%s: %i", indexname, index);
//	const int size = shiparray.GetSize();
//	if(0 <= index && index < size) {
//		const CShip& ship = shiparray.GetAt(index);
//		const CPoint& p = ship.GetCo();
//		CString sector;
//		sector.Format("%c%i",(char)(p.y+97),p.x+1);
//		s.Format("%s; %s; %s", s, ship.GetName(), sector);
//	}
//	MYTRACE_CHECKED("shipindices")(MT::LEVEL_INFO, s);
//}
