#pragma once

class CShips;
class CShip;
class CSector;
class CSystem;
class CBotf2Doc;

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
	void SanityCheckSectorAndSystem(
		const CSector& sector,
		const CSystem& system,
		const CBotf2Doc& doc);
	//static void ShipInfo(const CArray<CShip, CShip>& shiparray, int index, const CString& indexname);
	void CheckShipUniqueness(const CShips& ship, std::set<CString>& already_encountered);
private:
	void SanityCheckShip(const CShip& ship);
	void CheckShipTargetCoordinates(const CShip& ship);
	void Notify(const CString& s, bool bPopup = true);

};
