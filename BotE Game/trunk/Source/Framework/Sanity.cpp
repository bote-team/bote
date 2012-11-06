#include "stdafx.h"
#include "Sanity.h"

#include "Ships/Fleet.h"
#include "Galaxy/Sector.h"
#include "System/System.h"
#include "Races/race.h"
#include "Botf2Doc.h"
#include "Races/RaceController.h"

#include <cassert>

//These debugging functions check that all ships which don't move have target CPoint(-1, -1) set
//instead of their current coordinates. Later on, the places that handle the case of
//(target coords)==(current coords) can be removed (filled with assert(false)).

static void Notify(const CString& s, bool bPopup = true) {
	CString sMessage;
	sMessage.Format("%s This is a bug, please report.", s);
	MYTRACE("general")(MT::LEVEL_WARNING, sMessage);
	if(bPopup)
		AfxMessageBox(sMessage);
}

static void CheckShipTargetCoordinates(const CShip& ship)
{
	SHIP_ORDER::Typ order = ship.GetCurrentOrder();
	const CPoint& co = ship.GetKO();
	const CPoint& tco = ship.GetTargetKO();
	//orders which match the fact that the ship has a target != CPoint(-1, -1) set
	if(order == SHIP_ORDER::AVOID || order == SHIP_ORDER::ATTACK || order == SHIP_ORDER::CLOAK
		|| order == SHIP_ORDER::ASSIGN_FLAGSHIP) {
		//It still should be an actual target and not the ship's coordinates
		if(tco != co)
			return;
	}

	//we no longer allow to unset a target by setting it to the current coordinates
	if(tco != CPoint(-1, -1)) {
		CString s;
		s.Format("The %s from %s at (%u, %u) has target (%u, %u) set and current order is %s.",
			ship.GetShipName(),
			ship.GetOwnerOfShip(),
			co.x, co.y,
			tco.x, tco.y,
			ship.GetCurrentOrderAsString());
		Notify(s);
	}
}

static void SanityCheckShip(const CShip& ship)
{
	CheckShipTargetCoordinates(ship);

	const CPoint& co = ship.GetKO();
	if(ship.GetTerraformingPlanet() != -1 && ship.GetCurrentOrder() != SHIP_ORDER::TERRAFORM)
	{
		CString s;
		s.Format("The %s from %s at (%u, %u) has m_nTerraformingPlanet %i set but current order is %s.",
			ship.GetShipName(),
			ship.GetOwnerOfShip(),
			co.x, co.y,
			ship.GetTerraformingPlanet(),
			ship.GetCurrentOrderAsString());
		Notify(s);
	}
}

void CSanity::SanityCheckFleet(const CShip& ship)
{
	SanityCheckShip(ship);
	if(ship.GetFleet() != NULL) {
		CFleet const* const fleet = ship.GetFleet();
		for(unsigned i = 0; i < fleet->GetFleetSize(); ++i) {
			SanityCheckShip(*fleet->GetShipFromFleet(i));
		}
	}
}

void CSanity::SanityCheckSectorAndSystem(const CSector& sector, const CSystem& system, const CBotf2Doc& doc)
{
	CString s;
	if(!sector.GetSunSystem())
		return;
	const CString& sOwnerOfSystem = system.GetOwnerOfSystem();
	if(sOwnerOfSystem.IsEmpty())
		return;
	const CString& sOwnerOfSector = sector.GetOwnerOfSector();
	const CPoint& co = sector.GetKO();
	if(sOwnerOfSector != sOwnerOfSystem) {
		s.Format("OwnerOfSector != OwnerOfSystem in sector %s (%u,%u).",
			sector.GetName(TRUE),
			co.x, co.y
			);
		Notify(s);
		return;
	}
	const CRaceController& RaceCtrl = *doc.GetRaceCtrl();
	if(sector.GetMinorRace()) {
		const CMinor* pMinor = RaceCtrl.GetMinorRace(sector.GetName());
		assert(pMinor);
		assert(pMinor->GetRaceKO() == co);
		if(!pMinor->IsMemberTo() && !pMinor->GetSubjugated()) {
			if(sOwnerOfSystem != pMinor->GetRaceID()) {
				s.Format("Independent minor doesn't own system in sector (%u,%u).", co.x, co.y);
				Notify(s);
			}
			return;
		}
	}
	const CRace* pRace = RaceCtrl.GetRace(sOwnerOfSector);
	assert(pRace->IsMajor());
	const CMajor* pMajor = dynamic_cast<const CMajor*>(pRace);
	assert(pMajor);
}
