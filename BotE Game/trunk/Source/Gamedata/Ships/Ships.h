/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
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

class CShips/* : public CObject*/
{
public:
	//DECLARE_SERIAL (CShips)

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

	//get the CShips from the fleet of this CShips with the given key
	//complexity: logarithmic
	const CShips& at(unsigned key) const;

	//get the CShips from the fleet of this CShips with the given key
	//complexity: logarithmic
	CShips& at(unsigned key);

	//////////////////////////////////////////////////////////////////////
	// getting
	//////////////////////////////////////////////////////////////////////

	const CShips::const_iterator& CurrentShip() const;
	// Funktion liefert die Anzahl der Schiffe in der Flotte
	int GetFleetSize() const { return m_Fleet.GetSize(); }
	const CShip& Leader() const { return m_Leader; }
	CShip& Leader() { return m_Leader; }
	const CShipMap& Fleet() const { return m_Fleet; }
	unsigned Key() const { return m_Key; }


		//////////////////////////////////////////////////////////////////////
		// LEADER ACCESS
		//////////////////////////////////////////////////////////////////////
		// zum Lesen der Membervariablen
		bool LeaderIsCurrent() const { return m_bLeaderIsCurrent; }
		CHull* GetHull(void) {return m_Leader.GetHull();}
		const CHull* GetHull(void) const {return m_Leader.GetHull();}
		CShield* GetShield(void) {return m_Leader.GetShield();}
		const CShield* GetShield(void) const {return m_Leader.GetShield();}
		CArray<CTorpedoWeapons, CTorpedoWeapons>* GetTorpedoWeapons(void) {return m_Leader.GetTorpedoWeapons();}
		CArray<CBeamWeapons, CBeamWeapons>* GetBeamWeapons(void) {return m_Leader.GetBeamWeapons();}
		CArray<CTroop>* GetTransportedTroops(void) {return m_Leader.GetTransportedTroops();}
		const CArray<CTroop>* GetTransportedTroops(void) const {return m_Leader.GetTransportedTroops();}
		USHORT GetID() const {return m_Leader.GetID();}
		CPoint GetKO() const {return m_Leader.GetKO();}
		CPoint GetTargetKO() const {return m_Leader.GetTargetKO();}
		CArray<Sector>* GetPath() {return m_Leader.GetPath();}
		const CArray<Sector>* GetPath() const {return m_Leader.GetPath();}
		const CString& GetOwnerOfShip(void) const {return m_Leader.GetOwnerOfShip();}
		USHORT GetMaintenanceCosts() const {return m_Leader.GetMaintenanceCosts();}
		BYTE GetStealthPower() const {return m_Leader.GetStealthPower(); }
		bool GetCloak() const {return m_Leader.GetCloak(); }
		SHIP_TYPE::Typ GetShipType() const {return m_Leader.GetShipType(); }
		SHIP_SIZE::Typ GetShipSize() const {return m_Leader.GetShipSize(); }
		BYTE GetManeuverability() const {return m_Leader.GetManeuverability(); }
		SHIP_RANGE::Typ GetRange() const {return m_Leader.GetRange(); }
		BYTE GetSpeed() const {return m_Leader.GetSpeed(); }
		USHORT GetScanPower() const {return m_Leader.GetScanPower(); }
		BYTE GetScanRange() const {return m_Leader.GetScanRange(); }
		BYTE GetColonizePoints() const {return m_Leader.GetColonizePoints(); }
		BYTE GetStationBuildPoints() const {return m_Leader.GetStationBuildPoints(); }
		SHIP_ORDER::Typ GetCurrentOrder() const {return m_Leader.GetCurrentOrder(); }
		short GetTerraformingPlanet() const {return m_Leader.GetTerraformingPlanet(); }
		const CString& GetShipName() const {return m_Leader.GetShipName(); }
		const CString& GetShipClass() const {return m_Leader.GetShipClass(); }
		const CString& GetShipDescription() const {return m_Leader.GetShipDescription(); }
		CString GetShipTypeAsString(BOOL plural = FALSE) const { return m_Leader.GetShipTypeAsString(plural); }
		CString GetCurrentOrderAsString() const { return m_Leader.GetCurrentOrderAsString(); }
		CString GetCombatTacticAsString() const { return m_Leader.GetCombatTacticAsString(); }
		CString GetCurrentTargetAsString() const { return m_Leader.GetCurrentTargetAsString(); }
		bool GetIsShipFlagShip() const {return m_Leader.GetIsShipFlagShip(); }
		USHORT GetCrewExperience() const {return m_Leader.GetCrewExperience(); }
		USHORT GetStorageRoom() const {return m_Leader.GetStorageRoom(); }
		USHORT GetLoadedResources(BYTE res) const {return m_Leader.GetLoadedResources(res); }
		COMBAT_TACTIC::Typ GetCombatTactic() const {return m_Leader.GetCombatTactic(); }
	//////////////////////////////////////////////////////////////////////
	// setting
	//////////////////////////////////////////////////////////////////////

	void AdoptOrdersFrom(const CShips& ship);
	/*
	 * Adds the given CShips to this CShips' fleet and propagates this CShips' leader's orders to the given
	 * CShips' leader and fleet (both are in this CShips' fleet now).
	**/
	void AddShipToFleet(CShips& fleet);
	void SetCurrentShip(const CShips::iterator& position);
	// Funktion um ein Schiff aus der Flotte zu entfernen.
	void RemoveShipFromFleet(CShips::iterator& ship);
	//// Funktion löscht die gesamte Flotte
	void Reset(void);

	//Affects leader and fleet
	void ApplyTraining(int XP);

	//remove any possible flagship status of the leader and the fleet in this CShips
	//@return was it found and removed (process terminates, as only one flagship can exist)
	bool UnassignFlagship();

	void SetKey(unsigned key) { m_Key = key; }

		//////////////////////////////////////////////////////////////////////
		// LEADER ACCESS
		//////////////////////////////////////////////////////////////////////
		// zum Schreiben der Membervariablen
		//simple setting
		void SetID(USHORT ID) { m_Leader.SetID(ID);}
		void SetKO(int x, int y);
		void SetOwnerOfShip(const CString& sOwnerOfShip) { m_Leader.SetOwnerOfShip(sOwnerOfShip); }
		void SetMaintenanceCosts(USHORT MaintenanceCosts) { m_Leader.SetMaintenanceCosts(MaintenanceCosts); }
		void SetShipType(SHIP_TYPE::Typ nShipType) { m_Leader.SetShipType(nShipType); }
		void SetShipSize(SHIP_SIZE::Typ nSize) { m_Leader.SetShipSize(nSize); }
		void SetManeuverability(BYTE value) { m_Leader.SetManeuverability(value); }
		void SetSpeed(BYTE Speed) { m_Leader.SetSpeed(Speed); }
		void SetRange(SHIP_RANGE::Typ Range) { m_Leader.SetRange(Range); }
		void SetScanPower(USHORT ScanPower) { m_Leader.SetScanPower(ScanPower); }
		void SetScanRange(BYTE ScanRange) { m_Leader.SetScanRange(ScanRange); }
		void SetCrewExperiance(int nAdd) { m_Leader.SetCrewExperiance(nAdd); }
		void SetStealthPower(BYTE StealthPower) { m_Leader.SetStealthPower(StealthPower); }
		void SetCloak(bool bCloakOn);
		void SetStorageRoom(USHORT StorageRoom) { m_Leader.SetStorageRoom(StorageRoom); }
		void SetLoadedResources(USHORT add, BYTE res) { m_Leader.SetLoadedResources(add, res); }
		void SetColonizePoints(BYTE ColonizePoints) { m_Leader.SetColonizePoints(ColonizePoints); }
		void SetStationBuildPoints(BYTE StationBuildPoints) { m_Leader.SetStationBuildPoints(StationBuildPoints); }
		void SetCurrentOrder(SHIP_ORDER::Typ nCurrentOrder);
		void SetSpecial(int n, SHIP_SPECIAL::Typ nAbility) { m_Leader.SetSpecial(n, nAbility); }
		void SetTerraformingPlanet(short planetNumber);
		void SetShipName(const CString& ShipName) { m_Leader.SetShipName(ShipName); }
		void SetShipDescription(const CString& ShipDescription) { m_Leader.SetShipDescription(ShipDescription); }
		void SetShipClass(const CString& ShipClass) { m_Leader.SetShipClass(ShipClass); }
		void SetIsShipFlagShip(bool bIs) { m_Leader.SetIsShipFlagShip(bIs); }
		void SetCombatTactic(COMBAT_TACTIC::Typ nTactic);

		//more complex setting
		void SetTargetKO(const CPoint& TargetKO, int Index, const bool simple_setter = false);
		//Sets this ship's m_iCurrentOrder to AVOID if it's a civil ship and to ATTACK otherwise.
		void SetCurrentOrderAccordingToType();
		//Sets this ship's m_nCombatTactic to AVOID if it's a civil ship and to ATTACK otherwise.
		void SetCombatTacticAccordingToType();
		//Sets the current order according to m_nCombatTactic
		void UnsetCurrentOrder();

	//////////////////////////////////////////////////////////////////////
	// calculated stements about this fleet (should be const functions, non-bool returning)
	//////////////////////////////////////////////////////////////////////

	//// Funktion berechnet die Geschwindigkeit der Flotte. Der Parameter der hier übergeben werden sollte
	//// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
	unsigned GetFleetSpeed(const CShip* pShip = NULL) const;

	//// Funktion berechnet die Reichweite der Flotte. Der Parameter der hier übergeben werden sollte
	//// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
	SHIP_RANGE::Typ GetFleetRange(const CShip* ship = NULL) const;

	//// Funktion berechnet den Schiffstyp der Flotte. Wenn hier nur der selbe Schiffstyp in der Flotte vorkommt,
	//// dann gibt die Funktion diesen Schiffstyp zurück. Wenn verschiedene Schiffstypen in der Flotte vorkommen,
	//// dann liefert und die Funktion ein -1. Der Parameter der hier übergeben werden sollte ist der this-Zeiger
	//// des Schiffsobjektes, welches die Flotte besitzt
	short GetFleetShipType() const;

	//// Funktion berechnet die minimale Stealthpower der Flotte. Der Parameter der hier übergeben werden sollte
	//// ist der this-Zeiger bzw. die Adresse des Schiffsobjektes, welches die Flotte besitzt
	BYTE GetFleetStealthPower(const CShip* ship = NULL) const;

		//////////////////////////////////////////////////////////////////////
		// LEADER ACCESS
		//////////////////////////////////////////////////////////////////////

		/// Funktion gibt die gesamte Offensivpower des Schiffes zurück, welches es in 100s anrichten würde. Dieser
		/// Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum Vergleich heranzuziehen.
		/// @param bBeams <code>true</code> wenn Beamwaffen beachtet werden sollen
		/// @param bTorpedos <code>true</code> wenn Torpedowaffen beachtet werden sollen
		/// @return Wert welcher die Offensivstärke des Schiffes angibt
		UINT GetCompleteOffensivePower(bool bBeams = true, bool bTorpedos = true) const {
			return m_Leader.GetCompleteOffensivePower(bBeams, bTorpedos);
		}

		/// Funktion gibt die gesamte Defensivstärke des Schiffes zurück. Dabei wird die maximale Hülle, die maximalen
		/// Schilde und die Schildaufladezeit beachtet. Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum
		/// Vergleich heranzuziehen.
		/// @param bShields <code>true</code> wenn Schildstärken beachtet werden sollen
		/// @param bHull <code>true</code> wenn Hüllenstärke beachtet werden sollen
		/// @return Wert welcher die Defensivstärke des Schiffes angibt
		UINT GetCompleteDefensivePower(bool bShields = true, bool bHull = true) const {
			return m_Leader.GetCompleteDefensivePower(bShields, bHull);
		}

		/// Funktion gibt das Erfahrungslevel des Schiffes zurück. Damit sind nicht die genauen Erfahrungspunkte gemeint, sondern das erreichte
		/// Level aufgrund der Erfahrungspunkte.
		/// @return Erfahrungstufe
		BYTE GetExpLevel() const { return m_Leader.GetExpLevel(); }

		USHORT GetUsedStorageRoom(const CArray<CTroopInfo>* troopInfo) const {
			return m_Leader.GetUsedStorageRoom(troopInfo);
		}

	//////////////////////////////////////////////////////////////////////
	// bool statements about this fleet or the ship leading it
	//////////////////////////////////////////////////////////////////////
	//Are the hull of this ship and all the hulls of the ships in its fleet at their maximums ?
	//@return false if yes
	bool NeedsRepair() const;

	//// Diese Funktion liefert true wenn die Flotte den "order" ausführen kann.
	//// Kann die Flotte den Befehl nicht befolgen liefert die Funktion false zurück
	bool CanHaveOrder(SHIP_ORDER::Typ order) const;

	////Do all ships in this fleet have the given tactic ?
	bool AllOnTactic(COMBAT_TACTIC::Typ tactic) const;

	//has this ship a fleet with at least one ship ?
	bool HasFleet() const;
	bool FleetHasTroops() const;

	//Includes this CShip a CShip with experience >= 4 ?
	//Covers leader and fleet.
	bool HasVeteran() const;

		//////////////////////////////////////////////////////////////////////
		// LEADER ACCESS
		//////////////////////////////////////////////////////////////////////

		bool IsNonCombat() const { return m_Leader.IsNonCombat(); }
		//Is this ship an outpost or a starbase ?
		bool IsStation() const { return m_Leader.IsStation(); }
		bool IsAlien() const {	return m_Leader.IsAlien(); }
		//Is this ship in need for a player command input in this round ?
		//Does not cover "self-renewing" orders without a turn limit
		//such as ATTACK_SYSTEM; player is expected to look after such
		//fleets manually.
		//@return true if yes
		bool HasNothingToDo() const { return m_Leader.HasNothingToDo(); };
		/// Funktion gibt einen Wahrheitswert zurück, ob das Schiffsobjekt eine bestimmte Spezialfähigkeit besitzt.
		/// @param ability Spezialfähigkeit
		/// @return <code>true</code> wenn es diese Fähigkeit besitzt, ansonsten <code>false</code>
		bool HasSpecial(SHIP_SPECIAL::Typ nAbility) const { return m_Leader.HasSpecial(nAbility); }

	//////////////////////////////////////////////////////////////////////
	// other functions
	//////////////////////////////////////////////////////////////////////

	/// Funktion erstellt eine Tooltipinfo vom Schiff
	/// @param bShowFleet wenn dieser Parameter <code>true</code> dann werden Informationen über die angeführte Flotte angezeigt, sonst nur über das Schiff
	/// @return	der erstellte Tooltip-Text
	CString GetTooltip(bool bShowFleet = true);

	//Execute a 1-turn shield (always) and hull (if bAtShipPort == TRUE) repairing step
	void Repair(BOOL bAtShipPort, bool bFasterShieldRecharge);

	//Perform actions to retreat this ship to the given sector.
	void Retreat(const CPoint& ptRetreatSector) { m_Leader.Retreat(ptRetreatSector); };
	void RetreatFleet(const CPoint& RetreatSector);

	void CalcEffects(CSector& sector, CRace* pRace,
			bool bDeactivatedShipScanner, bool bBetterScanner);

	/// Diese Funktion berechnet die Schiffserfahrung in einer neuen Runde. Außer Erfahrung im Kampf, diese werden nach einem
	/// Kampf direkt verteilt.
	void CalcExp() { m_Leader.CalcExp(); };

	CString SanityCheckUniqueness(std::set<CString>& already_encountered) const;

	////uses this CShips's fleet's first ship to make a leading ship, which has the remaining ships of this CShips's
	////fleet as its fleet, and returns the new CShips
	CShips GiveFleetToFleetsFirstShip();

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
	CShip m_Leader;//the ship leading this fleet
	CShipMap m_Fleet;//other ships in this fleet
	unsigned m_Key; //index of this CShip in the shipmap
	bool m_bLeaderIsCurrent;
};

#endif // !defined(SHIPS_H_INCLUDED)
