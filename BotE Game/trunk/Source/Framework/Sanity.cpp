#include "stdafx.h"
#include "Sanity.h"



//These debugging functions check that all ships which don't move have target CPoint(-1, -1) set
//instead of their current coordinates. Later on, the places that handle the case of
//(target coords)==(current coords) can be removed (filled with assert(false)).
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
		s.Format("The %s from %s at (%u, %u) has target (%u, %u) set and current order is %s. This is a bug, please report.",
			ship.GetShipName(),
			ship.GetOwnerOfShip(),
			co.x, co.y,
			tco.x, tco.y,
			ship.GetCurrentOrderAsString());
		MYTRACE("general")(MT::LEVEL_WARNING, s);
		AfxMessageBox(s);
	}
}

static void SanityCheckShip(const CShip& ship)
{
	CheckShipTargetCoordinates(ship);

	const CPoint& co = ship.GetKO();
	if(ship.GetTerraformingPlanet() != -1 && ship.GetCurrentOrder() != SHIP_ORDER::TERRAFORM)
	{
		CString s;
		s.Format("The %s from %s at (%u, %u) has m_nTerraformingPlanet %i set but current order is %s. This is a bug, please report.",
			ship.GetShipName(),
			ship.GetOwnerOfShip(),
			co.x, co.y,
			ship.GetTerraformingPlanet(),
			ship.GetCurrentOrderAsString());
		MYTRACE("general")(MT::LEVEL_WARNING, s);
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
