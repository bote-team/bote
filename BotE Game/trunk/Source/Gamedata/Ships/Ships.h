/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Ships.h: Schnittstelle für die Klasse CShips.
//
//////////////////////////////////////////////////////////////////////

/*
 *@file
 * combination of a ship, which is the leading one, and its fleet
 * It has "1 plus 0 till n" semantics, where n is the number of ships in this CShip's fleet. While those
 * objects are of type CShip, they must always lack a fleet themselves. The fleet of this CShip is said
 * to not exist if the container holding it is empty (the container always exists); the CShip just represents
 * the leading ship then.
 * Everything which is calculated from or affects "a leading ship and its fleet", or the fleet only,
 * should be placed into this file.
 * Many functions are direct calls to the leading ship, some affect the fleet and the leader, and some
 * only the fleet.
 * Iterating should be done using CShips::const_iterator, which iterates over this CShip's fleet (without
 * the leader.)
 */

#if !defined(SHIPS_H_INCLUDED)
#define SHIPS_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ship.h"
#include "ShipMap.h"

class CBotEDoc;
class CMajor;

class CShips : public CShip, public boost::enable_shared_from_this<CShips>
{
public:
	enum RETREAT_MODE
	{
		RETREAT_MODE_STAY, //none of the ships in this fleet (including leader) retreats
		RETREAT_MODE_SPLITTED, //some do, some don't
		RETREAT_MODE_COMPLETE //all, including the leader, retreat
	};

	//////////////////////////////////////////////////////////////////////
	// Konstruktion/Destruktion
	//////////////////////////////////////////////////////////////////////

	// Standardkonstruktor
	CShips();
	CShips(const CShip&);

	// Destruktor
	virtual ~CShips();

	// Kopierkonstruktor
	CShips(const CShips& o);

	// Zuweisungsoperatur
	CShips& operator=(const CShips& o);

	// Die Serialisierungsfunktion
	void Serialize(CArchive &ar);

	//////////////////////////////////////////////////////////////////////
	// iterators
	//////////////////////////////////////////////////////////////////////
	typedef CShipMap::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	typedef CShipMap::iterator iterator;
	iterator begin();
	iterator end();

	const_iterator find(unsigned key) const;
	iterator find(unsigned key);

	const_iterator iterator_at(int index) const;
	iterator iterator_at(int index);

	boost::shared_ptr<const CShips> at(unsigned key) const;
	boost::shared_ptr<CShips> at(unsigned key);

	//////////////////////////////////////////////////////////////////////
	// getting
	//////////////////////////////////////////////////////////////////////

	const CShips::const_iterator& CurrentShip() const;
	// Funktion liefert die Anzahl der Schiffe in der Flotte
	int GetFleetSize() const { return m_Fleet.GetSize(); }
	const CShipMap& Fleet() const { return m_Fleet; }
	unsigned Key() const { return m_Key; }
	unsigned MapTileKey() const { return m_MapTileKey; }

	// zum Lesen der Membervariablen
	bool LeaderIsCurrent() const { return m_bLeaderIsCurrent; }

	float GetStartHabitants() const { return GetColonizePoints() * 4; }
	CString GetRangeAsString() const;

	//////////////////////////////////////////////////////////////////////
	// setting
	//////////////////////////////////////////////////////////////////////

private:
	void AdoptOrdersFrom(const CShips& ship);
public:
	/*
	 * Adds the given CShips to this CShips' fleet and propagates this CShips' leader's orders to the given
	 * CShips' leader and fleet (both are in this CShips' fleet now).
	**/
	void AddShipToFleet(const boost::shared_ptr<CShips>& fleet);
	void SetCurrentShip(const CShips::iterator& position);
	// Funktion um ein Schiff aus der Flotte zu entfernen.
	void RemoveShipFromFleet(CShips::iterator& ship);

	//strip this CShips from destroyed ships
	//@ return true in case the leading ship is still alive, false in case the leader is dead and
	//possibly ships in the fleet remain
	bool RemoveDestroyed(CRace& owner, unsigned short round, const CString& sEvent,	const CString& sStatus, CStringArray* destroyedShips = NULL, const CString& anomaly = "");

	//// Funktion löscht die gesamte Flotte
	void Reset();

	//Affects leader and fleet
	void ApplyTraining(int XP);

	bool ApplyIonstormEffects();

	//remove any possible flagship status of the leader and the fleet in this CShips
	//@return was it found and removed (process terminates, as only one flagship can exist)
	bool UnassignFlagship();

	void SetKey(unsigned key) { m_Key = key; }
	void SetMapTileKey(unsigned key) { m_MapTileKey = key; }

	// zum Schreiben der Membervariablen
	//simple setting
	struct NotifySector;
	void SetKO(int x, int y);
	void SetOwner(const CString& id);
	void SetCloak(bool bCloakOn);
	void SetCurrentOrder(SHIP_ORDER::Typ nCurrentOrder);
	void SetTerraform(short planetNumber = -1);

	void SetCombatTactic(COMBAT_TACTIC::Typ nTactic, bool bPropagateToFleet = true);

	//more complex setting
	void SetTargetKO(const CPoint& TargetKO, const bool simple_setter = false);
	//Sets this ship's m_iCurrentOrder to AVOID if it's a civil ship and to ATTACK otherwise.
	void SetCurrentOrderAccordingToType();
	//Sets this ship's m_nCombatTactic to AVOID if it's a civil ship and to ATTACK otherwise.
	void SetCombatTacticAccordingToType();
	//Sets the current order according to m_nCombatTactic
	void UnsetCurrentOrder();

	//////////////////////////////////////////////////////////////////////
	// calculated stements about this fleet (should be const functions, non-bool returning)
	//////////////////////////////////////////////////////////////////////

	//// Funktion berechnet die Geschwindigkeit der Flotte.
	unsigned GetSpeed(bool consider_fleet) const;

	//// Funktion berechnet die Reichweite der Flotte.
	SHIP_RANGE::Typ GetRange(bool consider_fleet) const;

	RETREAT_MODE CalcRetreatMode() const;

	//// Funktion berechnet den Schiffstyp der Flotte. Wenn hier nur der selbe Schiffstyp in der Flotte vorkommt,
	//// dann gibt die Funktion diesen Schiffstyp zurück. Wenn verschiedene Schiffstypen in der Flotte vorkommen,
	//// dann liefert und die Funktion ein -1. Der Parameter der hier übergeben werden sollte ist der this-Zeiger
	//// des Schiffsobjektes, welches die Flotte besitzt
	short GetFleetShipType() const;

	//// Funktion berechnet die minimale Stealthpower der Flotte.
	unsigned GetStealthPower() const;

	//////////////////////////////////////////////////////////////////////
	// bool statements about this fleet or the ship leading it
	//////////////////////////////////////////////////////////////////////
	//Are the hull of this ship and all the hulls of the ships in its fleet at their maximums ?
	//@return false if yes
	bool NeedsRepair() const;

	//// Diese Funktion liefert true wenn die Flotte den "order" ausführen kann.
	//// Kann die Flotte den Befehl nicht befolgen liefert die Funktion false zurück
	bool CanHaveOrder(SHIP_ORDER::Typ order, bool require_new, bool require_all_can = true) const;

	//has this ship a fleet with at least one ship ?
	bool HasFleet() const;
	bool FleetHasTroops() const;

	//Includes this CShip a CShip with experience >= 4 ?
	//Covers leader and fleet.
	bool HasVeteran() const;

	bool HasTarget() const;

	bool CanCloak(bool consider_fleet) const;

	//////////////////////////////////////////////////////////////////////
	// other functions
	//////////////////////////////////////////////////////////////////////

	/// Funktion erstellt eine Tooltipinfo vom Schiff
	/// @param bShowFleet wenn dieser Parameter <code>true</code> dann werden Informationen über die angeführte Flotte angezeigt, sonst nur über das Schiff
	/// @return	der erstellte Tooltip-Text
	CString GetTooltip(bool bShowFleet = true) const;

	//Execute a 1-turn shield (always) and hull (if bAtShipPort == TRUE) repairing step
	void TraditionalRepair(BOOL bAtShipPort, bool bFasterShieldRecharge);
	void RepairCommand(BOOL bAtShipPort, bool bFasterShieldRecharge, CShipMap& ships);


	//Perform actions to retreat this ship to the given sector.
	void RetreatFleet(const CPoint& RetreatSector, COMBAT_TACTIC::Typ const* NewCombatTactic = NULL);
	void Retreat(const CPoint& ptRetreatSector, COMBAT_TACTIC::Typ const* NewCombatTactic = NULL);

	void CalcEffects(CSector& sector, CRace* pRace,
			bool bDeactivatedShipScanner, bool bBetterScanner);

	struct StationWorkResult
	{
		StationWorkResult() :
			remove_leader(false),
			finished(false)
		{};
		bool remove_leader;
		bool finished;
	};

	StationWorkResult BuildStation(SHIP_ORDER::Typ order, CSector& sector, CMajor& major, short id);


	void Scrap(CMajor& major, CSystem& sy, bool disassembly);


	CString SanityCheckUniqueness(std::set<CString>& already_encountered) const;
	bool SanityCheckOrdersConsistency() const;

	////uses this CShips's fleet's first ship to make a leading ship, which has the remaining ships of this CShips's
	////fleet as its fleet, and returns the new CShips
	boost::shared_ptr<CShips> GiveFleetToFleetsFirstShip();

	/// Funktion zum Zeichnen des Schiffes in der Schiffsansicht.
	/// @param g Zeiger auf Zeichenkontext
	/// @param pGraphicPool Zeiger auf Grafikpool
	/// @param pt Linke obere Ecke der Zeichenkoordinate
	/// @param bIsMarked Ist das Schiff aktuell markiert
	/// @param bOwnerUnknown Ist der Besitzer unbekannt
	/// @param bDrawFleet Sollen Flotteninformationen angezeigt werden, falls das Schiff eine Flotte anführt
	/// This parameter is/should supposedly be true if this function is called from shipbottomview (where ship
	/// groups are shown as grouped) and false if called from elsewhere (where the ships in a group are shown
	//	alone)
	/// @param clrNormal Schriftfarbe
	/// @param clrMark Schriftfarbe falls markiert
	/// @param font zu benutzende Schrift
	void DrawShip(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt, bool bIsMarked, bool bOwnerUnknown, bool bDrawFleet, const Gdiplus::Color& clrNormal, const Gdiplus::Color& clrMark, const Gdiplus::Font& font) const;


//////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////
private:
	// Wenn wir eine Gruppe bilden und dieses Schiff hier Gruppenleader ist, dann werden die anderen Schiffe in die Fleet genommen
	CShipMap m_Fleet;//other ships in this fleet
	unsigned m_Key; //index of this CShip in the shipmap
	unsigned m_MapTileKey;
	bool m_bLeaderIsCurrent;
};

#endif // !defined(SHIPS_H_INCLUDED)
