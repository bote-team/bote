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

void CSanity::Notify(const CString& s, bool bPopup, bool force_popup) {
	CString sMessage;
	sMessage.Format("%s This is a bug, please report.", s);
	MYTRACE("general")(MT::LEVEL_WARNING, sMessage);
	if(bPopup && !notified || force_popup) {
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
			ship.OwnerID(),
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
			ship.OwnerID(),
			co.x, co.y,
			ship.GetTerraform(),
			ship.GetCurrentOrderAsString());
		Notify(s);
	}
	if(!ship.IsAlive()) {
		CString s;
		s.Format("The %s from %s at (%u, %u) has hull < 1 but wasn't removed!",
			ship.GetName(),
			ship.OwnerID(),
			co.x, co.y);
		Notify(s);
	}
	if(!ship.CanHaveOrder(ship.GetCurrentOrder(), false)) {
		CString s;
		s.Format("The %s from %s at (%u, %u) has order %s which it should not be allowed to have!",
			ship.GetName(),
			ship.OwnerID(),
			co.x, co.y,
			ship.GetCurrentOrderAsString());
		Notify(s);
	}
}

void CSanity::SanityCheckFleet(const CShips& ship)
{
	SanityCheckShip(ship);
	AssertBotE(resources::pDoc->m_ShipMap.at(ship.Key()).get() == &ship);
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
	const bool ok = system.CheckSanity();
	AssertBotE(ok);
}

void CSanity::SanityCheckRacePtrUseCounts(const CBotEDoc& doc)
{
	const CRaceController& race_ctrl = *doc.GetRaceCtrl();

	std::map<CString, int> use_counts;

	for(CRaceController::const_iterator it = race_ctrl.begin(); it != race_ctrl.end(); ++it)
	{
		if(it->second->IsAlien())
			continue;

		std::map<CString, int>::iterator count = use_counts.find(it->first);
		if(count == use_counts.end())
			use_counts[it->first] = 3;//the race itself in race controller, in diplo ai object in CRace
			//and HomeOf member in home system
		else
			count->second = count->second + 3;
		if(it->second->IsMinor())
		{
			const boost::shared_ptr<CRace>& owner = it->second->Owner();
			if(owner)
			{
				AssertBotE(owner->IsMajor());
				std::map<CString, int>::iterator found = use_counts.find(owner->GetRaceID());
				if(found == use_counts.end())
					use_counts[owner->GetRaceID()] = 1;
				else
					found->second++;
			}
		}
	}

	for(std::vector<CSystem>::const_iterator it = doc.m_Systems.begin(); it != doc.m_Systems.end(); ++it)
	{
		const boost::shared_ptr<CRace>& owner = it->Owner();
		if(owner)
		{
			std::map<CString, int>::iterator count = use_counts.find(owner->GetRaceID());
			AssertBotE(count != use_counts.end());
			count->second++;
		}
	}

	for(CShipMap::const_iterator it = doc.m_ShipMap.begin(); it != doc.m_ShipMap.end(); ++it)
	{
		if(it->second->IsAlien())
			continue;
		std::map<CString, int>::iterator found = use_counts.find(it->second->OwnerID());
		AssertBotE(found != use_counts.end());
		found->second++;
		found->second = found->second + it->second->Fleet().GetSize();
	}

	std::map<CString, int>::iterator count = use_counts.begin();
	for(CRaceController::const_iterator it = race_ctrl.begin(); it != race_ctrl.end(); ++it)
	{
		if(it->second->IsAlien())
			continue;
		AssertBotE(count->first == it->first);
		int use_count = it->second.use_count();
		if(use_count != count->second)
		{
			CString s;
			s.Format("race pointer use count for %s inconsistent; is: %i, should be: %i", it->first, use_count, count->second);
			Notify(s, true);
		}
		++count;
	}
	AssertBotE(count == use_counts.end());
}

void CSanity::SanityCheckShipsInSectors(const CBotEDoc& doc)
{
	const std::vector<CSystem>& systems = doc.m_Systems;
	const CShipMap& shipmap = doc.m_ShipMap;

	for(std::vector<CSystem>::const_iterator it = systems.begin(); it != systems.end(); ++it)
	{
		const std::map<CString, CShipMap>& ships = it->ShipsInSector();
		for(std::map<CString, CShipMap>::const_iterator itt = ships.begin(); itt != ships.end(); ++itt)
			for(CShipMap::const_iterator ittt = itt->second.begin(); ittt != itt->second.end(); ++ittt)
			{
				const CShipMap::const_iterator found = shipmap.find(ittt->second->Key());
				AssertBotE(found != shipmap.end());
				AssertBotE(found->second->GetCo() == it->GetCo());
				AssertBotE(found->second == ittt->second);
			}
	}

	for(CShipMap::const_iterator it = shipmap.begin(); it != shipmap.end(); ++it)
	{
		const CPoint& co = it->second->GetCo();
		const std::map<CString, CShipMap> ships = systems.at(CoordsToIndex(co.x, co.y)).ShipsInSector();
		const std::map<CString, CShipMap>::const_iterator found_race = ships.find(it->second->OwnerID());
		AssertBotE(found_race != ships.end());
		const CShipMap::const_iterator found_ship = found_race->second.find(it->second->MapTileKey());
		AssertBotE(found_ship != found_race->second.end());
		AssertBotE(found_ship->second == it->second);
	}

}

