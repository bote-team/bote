#pragma once

#include "Ships/Fleet.h"

class CShipSanity
{
	private:
		CShipSanity(void);
	public:
		~CShipSanity(void);


		static void SanityCheckFleet(const CShip& ship);

};
