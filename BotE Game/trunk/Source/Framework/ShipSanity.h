#pragma once

#include "Ships/Fleet.h"

class CShipSanity
{
	private:
		CShipSanity(void);
	public:
		~CShipSanity(void);


		static void CShipSanity::SanityCheckFleet(const CShip& ship);

};
