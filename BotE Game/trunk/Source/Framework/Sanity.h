#pragma once

class CShips;
class CSector;
class CSystem;
class CBotEDoc;

#include <set>

//@file
// sanity checks of game data which are executed at runtime
// combines nicely with the --autoturn command line parameter


class CSanity
{
	private:
		CSanity(void);
		bool notified;
	public:
		static CSanity* GetInstance();
		~CSanity(void);


	void SanityCheckFleet(const CShips& ship);
	static void SanityCheckSectorAndSystem(const CSystem& system);
	//static void ShipInfo(const CArray<CShip, CShip>& shiparray, int index, const CString& indexname);
	void CheckShipUniqueness(const CShips& ship, std::set<CString>& already_encountered);
private:
	void SanityCheckShip(const CShips& ship);
	void CheckShipTargetCoordinates(const CShips& ship);
	void Notify(const CString& s, bool bPopup = true);

};
