/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Ship.h: Schnittstelle für die Klasse CShip.
//
//////////////////////////////////////////////////////////////////////

/*
 *@file
 * ships
 * A ship knows nothing about fleets, whether it is alone or contained within some fleet. Everything which
 * affects or is calculated from only a single ship should be placed into this file.
 */

#if !defined(AFX_SHIP_H__C9FE4406_A0D7_4AE0_A5D0_0070FCBC45C1__INCLUDED_)
#define AFX_SHIP_H__C9FE4406_A0D7_4AE0_A5D0_0070FCBC45C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Options.h"
#include "TorpedoWeapons.h"
#include "BeamWeapons.h"
#include "Shield.h"
#include "Hull.h"
#include "Races/Starmap.h"
#include "Troops/TroopInfo.h"

// forward declaration
class CGraphicPool;
class CRace;
class CBotf2Doc;

/// Klasse beschreibt ein Schiff in BotE
class CShip : public CObject
{
public:
	DECLARE_SERIAL (CShip)
// Konstruktion & Destruktion
	CShip();
	virtual ~CShip();
// Kopierkonstruktor
	CShip(const CShip & rhs);
// Zuweisungsoperatur
	CShip & operator=(const CShip &);
// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	struct FleetInfoForGetTooltip {
		FleetInfoForGetTooltip() :
			FleetShipType(-1),
			FleetRange(SHIP_RANGE::LONG),
			FleetSpeed(0)
			{}
		FleetInfoForGetTooltip(short FleetShipType, SHIP_RANGE::Typ FleetRange, BYTE FleetSpeed) :
			FleetShipType(FleetShipType),
			FleetRange(FleetRange),
			FleetSpeed(FleetSpeed)
			{}

		short FleetShipType;
		SHIP_RANGE::Typ FleetRange;
		BYTE FleetSpeed;
	};

	//////////////////////////////////////////////////////////////////////
	// getting
	//////////////////////////////////////////////////////////////////////
	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	CHull* GetHull(void) {return &m_Hull;}
	const CHull* GetHull(void) const {return &m_Hull;}
	CShield* GetShield(void) {return &m_Shield;}
	const CShield* GetShield(void) const {return &m_Shield;}
	CArray<CTorpedoWeapons, CTorpedoWeapons>* GetTorpedoWeapons(void) {return &m_TorpedoWeapons;}
	CArray<CBeamWeapons, CBeamWeapons>* GetBeamWeapons(void) {return &m_BeamWeapons;}
	CArray<CTroop>* GetTransportedTroops(void) {return &m_Troops;}
	const CArray<CTroop>* GetTransportedTroops(void) const {return &m_Troops;}

	USHORT GetID() const {return m_iID;}
	CPoint GetKO() const {return m_KO;}
	CPoint GetTargetKO() const {return m_TargetKO;}
	CArray<Sector>* GetPath() {return &m_Path;}
	const CArray<Sector>* GetPath() const {return &m_Path;}
	const CString& GetOwnerOfShip(void) const {return m_sOwnerOfShip;}
	USHORT GetMaintenanceCosts() const {return m_iMaintenanceCosts;}
	unsigned GetStealthGrade() const {return m_iStealthGrade;}
	bool GetCloak() const;
	SHIP_TYPE::Typ GetShipType() const {return m_iShipType;}
	SHIP_SIZE::Typ GetShipSize() const {return m_nShipSize;}
	BYTE GetManeuverability() const {return m_byManeuverability;}
	SHIP_RANGE::Typ GetRange() const {return m_iRange;}
	BYTE GetSpeed() const {return m_iSpeed;}
	USHORT GetScanPower() const {return m_iScanPower;}
	BYTE GetScanRange() const {return m_iScanRange;}
	BYTE GetColonizePoints() const {return m_iColonizePoints;}
	BYTE GetStationBuildPoints() const {return m_iStationBuildPoints;}
	SHIP_ORDER::Typ GetCurrentOrder() const {return m_iCurrentOrder;}
	short GetTerraform() const {return m_nTerraformingPlanet;}
	const CString& GetShipName() const {return m_strShipName;}
	const CString& GetShipClass() const {return m_strShipClass;}
	const CString& GetShipDescription() const {return m_strShipDescription;}
	CString GetShipTypeAsString(BOOL plural = FALSE) const;
	CString GetCurrentOrderAsString() const;
	CString GetCombatTacticAsString() const;
	CString GetCurrentTargetAsString() const;
	bool GetIsShipFlagShip() const {return m_bIsFlagShip;}
	USHORT GetCrewExperience() const {return m_iCrewExperiance;}
	USHORT GetStorageRoom() const {return m_iStorageRoom;}
	USHORT GetLoadedResources(BYTE res) const {return m_iLoadedResources[res];}
	COMBAT_TACTIC::Typ GetCombatTactic() const {return m_nCombatTactic;}


	//////////////////////////////////////////////////////////////////////
	// setting
	//////////////////////////////////////////////////////////////////////

	// zum Schreiben der Membervariablen
	void SetID(USHORT ID) {m_iID = ID+10000;}
	void SetKO(int x, int y) {m_KO = CPoint(x, y);}
	void SetTargetKO(const CPoint& TargetKO, const bool simple_setter = false);
	void SetOwnerOfShip(const CString& sOwnerOfShip) {m_sOwnerOfShip = sOwnerOfShip;}
	void SetMaintenanceCosts(USHORT MaintenanceCosts) {m_iMaintenanceCosts = MaintenanceCosts;}
	void SetShipType(SHIP_TYPE::Typ nShipType) {m_iShipType = nShipType;}
	void SetShipSize(SHIP_SIZE::Typ nSize) {m_nShipSize = nSize;}
	void SetManeuverability(BYTE value) {m_byManeuverability = value;}
	void SetSpeed(BYTE Speed) {m_iSpeed = Speed;}
	void SetRange(SHIP_RANGE::Typ Range) {m_iRange = Range;}
	void SetScanPower(USHORT ScanPower) {m_iScanPower = ScanPower;}
	void SetScanRange(BYTE ScanRange) {m_iScanRange = ScanRange;}
	void SetStealthGrade(unsigned StealthGrade) {m_iStealthGrade = StealthGrade;}
	void SetCloak(bool bCloakOn);
	void SetStorageRoom(USHORT StorageRoom) {m_iStorageRoom = StorageRoom;}
	void SetLoadedResources(USHORT add, BYTE res) {m_iLoadedResources[res] += add;}
	void SetColonizePoints(BYTE ColonizePoints) {m_iColonizePoints = ColonizePoints;}
	void SetStationBuildPoints(BYTE StationBuildPoints) {m_iStationBuildPoints = StationBuildPoints;}
	void SetCurrentOrder(SHIP_ORDER::Typ nCurrentOrder);
	void SetSpecial(int n, SHIP_SPECIAL::Typ nAbility) {m_nSpecial[n] = nAbility;}
	void SetTerraform(short planetNumber = -1);
	void SetShipName(const CString& ShipName) {m_strShipName = ShipName;}
	void SetShipDescription(const CString& ShipDescription) {m_strShipDescription = ShipDescription;}
	void SetShipClass(const CString& ShipClass) {m_strShipClass = ShipClass;}
	void SetIsShipFlagShip(bool bIs) {m_bIsFlagShip = bIs;}
	void SetCombatTactic(COMBAT_TACTIC::Typ nTactic) {m_nCombatTactic = nTactic;}

	//more complex setting

	void SetCurrentOrderAccordingToType();
	//Sets this ship's m_nCombatTactic to AVOID if it's a civil ship and to ATTACK otherwise.
	void SetCombatTacticAccordingToType();
	//Sets the current oder according to m_nCombatTactic
	void UnsetCurrentOrder();

	bool RemoveDestroyed(CRace& owner, unsigned short round, const CString& sEvent,
		const CString& sStatus, CStringArray* destroyedShips, const CString& anomaly = "");

	/// Diese Funktion berechnet die Schiffserfahrung in einer neuen Runde. Außer Erfahrung im Kampf, diese werden nach einem
	/// Kampf direkt verteilt.
	void CalcExp();

	//sets this ship's orders to the one of the passed ship
	void AdoptOrdersFrom(const CShip& ship);

	void SetCrewExperiance(int nAdd);

	void ApplyTraining(int xp, bool veteran);
	bool ApplyIonstormEffects();

	enum UNASSIGN_FLAGSHIP_MODE { UNASSIGN_FLAGSHIP_MODE_STATUS, UNASSIGN_FLAGSHIP_MODE_ORDER };

	//take away flagship title or assign flagship order
	//@return whether it was taken away
	bool UnassignFlagship(UNASSIGN_FLAGSHIP_MODE mode);

	//////////////////////////////////////////////////////////////////////
	// calculated stements about this ship (should be const functions, non-bool returning)
	//////////////////////////////////////////////////////////////////////

	/// Funktion gibt die gesamte Offensivpower des Schiffes zurück, welches es in 100s anrichten würde. Dieser
	/// Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum Vergleich heranzuziehen.
	/// @param bBeams <code>true</code> wenn Beamwaffen beachtet werden sollen
	/// @param bTorpedos <code>true</code> wenn Torpedowaffen beachtet werden sollen
	/// @return Wert welcher die Offensivstärke des Schiffes angibt
	UINT GetCompleteOffensivePower(bool bBeams = true, bool bTorpedos = true) const;

	/// Funktion gibt die gesamte Defensivstärke des Schiffes zurück. Dabei wird die maximale Hülle, die maximalen
	/// Schilde und die Schildaufladezeit beachtet. Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum
	/// Vergleich heranzuziehen.
	/// @param bShields <code>true</code> wenn Schildstärken beachtet werden sollen
	/// @param bHull <code>true</code> wenn Hüllenstärke beachtet werden sollen
	/// @return Wert welcher die Defensivstärke des Schiffes angibt
	UINT GetCompleteDefensivePower(bool bShields = true, bool bHull = true) const;

	/// Funktion gibt das Erfahrungslevel des Schiffes zurück. Damit sind nicht die genauen Erfahrungspunkte gemeint, sondern das erreichte
	/// Level aufgrund der Erfahrungspunkte.
	/// @return Erfahrungstufe
	BYTE GetExpLevel() const;

	USHORT GetUsedStorageRoom(const CArray<CTroopInfo>* troopInfo) const;

	unsigned GetStealthPower() const;

	unsigned GetMaxMaxShield() const;

	//////////////////////////////////////////////////////////////////////
	// bool statements about this ship
	//////////////////////////////////////////////////////////////////////

	bool IonstormCanImproveShields() const;

	//is this a civil ship (colony, transport, probe)
	bool IsNonCombat() const;
	//Is this ship an outpost or a starbase ?
	bool IsStation() const {
		return m_iShipType == SHIP_TYPE::OUTPOST || m_iShipType == SHIP_TYPE::STARBASE;
	}
	bool IsAlien() const {
		return m_iShipType == SHIP_TYPE::ALIEN;
	}
	bool HasTroops() const { return !GetTransportedTroops()->IsEmpty(); }

	bool HasTarget() const {
		return m_TargetKO.x != -1 && m_TargetKO.y != -1;
	}

	//Is this ship in need for a player command input in this round ?
	//Does not cover "self-renewing" orders without a turn limit
	//such as ATTACK_SYSTEM; player is expected to look after such
	//fleets manually.
	//@return true if yes
	bool HasNothingToDo() const;
	//Are the hull of this ship and all the hulls of the ships in its fleet at their maximums ?
	//@return false if yes
	bool NeedsRepair() const;

	/// Funktion gibt einen Wahrheitswert zurück, ob das Schiffsobjekt eine bestimmte Spezialfähigkeit besitzt.
	/// @param ability Spezialfähigkeit
	/// @return <code>true</code> wenn es diese Fähigkeit besitzt, ansonsten <code>false</code>
	bool HasSpecial(SHIP_SPECIAL::Typ nAbility) const;

	//experience >= 4 ?
	bool IsVeteran() const;

	//Is this ship allowed to have the order order according to its type and physical capabilities ?
	bool CanHaveOrder(SHIP_ORDER::Typ order, bool require_new) const;

	bool CanCloak() const;

public:

	bool IsAlive() const {
		return m_Hull.GetCurrentHull() >= 1;
	}

	//////////////////////////////////////////////////////////////////////
	// other functions
	//////////////////////////////////////////////////////////////////////

	//Execute a 1-turn shield (always) and hull (if bAtShipPort == TRUE) repairing step
	void Repair(BOOL bAtShipPort, bool bFasterShieldRecharge);

	//Perform actions to retreat this ship to the given sector.
	void Retreat(const CPoint& ptRetreatSector);

	//calculate effects this ship has onto its sector
	void CalcEffectsForSingleShip(CSector& sector, CRace* pRace,
			bool bDeactivatedShipScanner, bool bBetterScanner, bool fleetship);

	CString SanityCheckUniqueness(std::set<CString>& already_encountered) const;
	bool SanityCheckOrdersConsistency(const CShip& with) const;

	/// Funktion erstellt eine Tooltipinfo vom Schiff
	/// @param info wenn dieser Parameter nicht NULL ist dann werden Informationen über die angeführte Flotte angezeigt, sonst nur über das Schiff
	/// @return	der erstellte Tooltip-Text
	CString GetTooltip(const FleetInfoForGetTooltip* const info = NULL);

	/// Funktion zum Zeichnen des Schiffes in der Schiffsansicht.
	/// @param g Zeiger auf Zeichenkontext
	/// @param pGraphicPool Zeiger auf Grafikpool
	/// @param pt Linke obere Ecke der Zeichenkoordinate
	/// @param bIsMarked Ist das Schiff aktuell markiert
	/// @param bOwnerUnknown Ist der Besitzer unbekannt
	/// @param bDrawFleet Sollen Flotteninformationen angezeigt werden, falls das Schiff eine Flotte anführt
	/// @param clrNormal Schriftfarbe
	/// @param clrMark Schriftfarbe falls markiert
	/// @param font zu benutzende Schrift
	void DrawShip(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt, bool bIsMarked,
		bool bOwnerUnknown, bool bDrawFleet, const Gdiplus::Color& clrNormal,
		const Gdiplus::Color& clrMark, const Gdiplus::Font& font, bool bDrawTroopSymbol,
		short FleetShipType, int FleetSize) const;

private:
	void DrawOrderTerraform(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt) const;
	void DrawOrderColonize(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt) const;

protected:
	CHull	m_Hull;												// die Hülle des Schiffes
	CShield m_Shield;											// die Schilde des Schiffes
	CArray<CTorpedoWeapons,CTorpedoWeapons> m_TorpedoWeapons;	// die Torpedobewaffnung des Schiffes
	CArray<CBeamWeapons,CBeamWeapons>		m_BeamWeapons;		// die Beamfirebewaffnung des Schiffes

	USHORT m_iID;						// ID des Schiffes
	USHORT m_iMaintenanceCosts;			// Unterhaltskosten des Schiffes pro Runde
	SHIP_TYPE::Typ m_iShipType;			// Schiffstype, siehe Options.h
	SHIP_SIZE::Typ m_nShipSize;			// Die Größe des Schiffes
	BYTE m_byManeuverability;			// Die Manövriebarkeit des Schiffes im Kampf
	BYTE m_iSpeed;						// Geschwindigkeit des Schiffes in der CMenuChooseView
	SHIP_RANGE::Typ m_iRange;			// Reichweite des Schiffes
	USHORT m_iScanPower;				// Scankraft des Schiffes
	BYTE m_iScanRange;					// Die Reichweite der Scanner
	USHORT m_iStorageRoom;				// Laderaum des Schiffes
	BYTE m_iColonizePoints;				// Kolonisierungs/Terraform-Punkte -> Dauer
	BYTE m_iStationBuildPoints;			// Außenposten/Stations-baupunkte -> Dauer
	SHIP_SPECIAL::Typ m_nSpecial[2];	// Die beiden möglichen Spezialfähigkeiten des Schiffes
	CString m_strShipName;				// Der Name des Schiffes
	CString m_strShipDescription;		// Die Beschreibung des Schiffes
	CString m_strShipClass;				// Der Name der Schiffsklasse

private:
	unsigned m_iStealthGrade;			// Tarnstärke des Schiffes: 0 = keine Tarnung, 1 = bissl ...
	SHIP_ORDER::Typ m_iCurrentOrder;	// Aktueller Befehl des Schiffes, nutze dazu enum
	CString m_sOwnerOfShip;				// Besitzer des Schiffes
	CPoint m_KO;						// Koordinate des Schiffes im Raum (welcher Sector?)
	CPoint m_TargetKO;					// Der Zielkurs des Schiffes
	CArray<Sector> m_Path;				// Der Kurs des Schiffes zum Ziel
	bool m_bCloakOn;					// ist die Tarnung eingeschaltet
	short m_nTerraformingPlanet;		// Nummer des Planeten der terraformt wird
	bool m_bIsFlagShip;					// Ist dieses Schiff ein Flagschiff (es kann immer nur ein Schiff eines Imperiums Flagschiff sein)
	USHORT m_iCrewExperiance;			// Crewerfahrung des Schiffes
	// Laderaum
	CArray<CTroop> m_Troops;				// Die Truppen, welche auf dem Schiff transportiert werden.
	USHORT m_iLoadedResources[DERITIUM+1];	// Die geladenen Ressourcen auf dem Schiff
	// Kampftaktik
	COMBAT_TACTIC::Typ m_nCombatTactic;///< Taktik des Schiffes im Kampf
};

#endif // !defined(AFX_SHIP_H__C9FE4406_A0D7_4AE0_A5D0_0070FCBC45C1__INCLUDED_)

