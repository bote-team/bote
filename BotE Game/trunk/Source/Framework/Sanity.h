#pragma once

#include "Ships/Fleet.h"

class CSanity
{
	private:
		CSanity(void);
	public:
		~CSanity(void);


		static void SanityCheckFleet(const CShip& ship);

};
