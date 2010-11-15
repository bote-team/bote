/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Ship.h: Schnittstelle für die Klasse CShip.
//
//////////////////////////////////////////////////////////////////////

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
#include "..\Races\Starmap.h"
#include "..\Troops\TroopInfo.h"

// forward declaration
class CFleet;
class CGraphicPool;

/// Mögliche Taktiken im Kampf
typedef enum COMBAT_TACTICS
{
	COMBAT_TACTIC_ATTACK,	///< Angriff
	COMBAT_TACTIC_AVOID,	///< Meiden
	COMBAT_TACTIC_RETREAT	///< Rückzug
}
COMBAT_TACTICS;

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
	
// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	CFleet*	GetFleet(void) {return m_Fleet;}
	CHull* GetHull(void) {return &m_Hull;}
	CShield* GetShield(void) {return &m_Shield;}	
	CArray<CTorpedoWeapons, CTorpedoWeapons>* GetTorpedoWeapons(void) {return &m_TorpedoWeapons;}
	CArray<CBeamWeapons, CBeamWeapons>* GetBeamWeapons(void) {return &m_BeamWeapons;}
	CArray<CTroop>* GetTransportedTroops(void) {return &m_Troops;}
	
	/// Funktion gibt die gesamte Offensivpower des Schiffes zurück, welches es in 100s anrichten würde. Dieser
	/// Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum Vergleich heranzuziehen.
	UINT GetCompleteOffensivePower() const;

	/// Funktion gibt die gesamte Defensivstärke des Schiffes zurück. Dabei wird die maximale Hülle, die maximalen
	/// Schilde und die Schildaufladezeit beachtet. Dieser Wert hat keinen direkten Kampfeinfluss, er ist nur zum
	/// Vergleich heranzuziehen.
	UINT GetCompleteDefensivePower() const;

	/// Funktion gibt das Erfahrungslevel des Schiffes zurück. Damit sind nicht die genauen Erfahrungspunkte gemeint, sondern das erreichte
	/// Level aufgrund der Erfahrungspunkte.
	/// @return Erfahrungstufe
	BYTE GetExpLevel() const;

	USHORT GetID() const {return m_iID;}
	CPoint GetKO() const {return m_KO;}
	CPoint GetTargetKO() const {return m_TargetKO[0];}
	CArray<Sector>* GetPath() {return &m_Path;}
	const CString& GetOwnerOfShip(void) const {return m_sOwnerOfShip;}
	USHORT GetMaintenanceCosts() const {return m_iMaintenanceCosts;}
	BYTE GetStealthPower() const {return m_iStealthPower;}
	BOOLEAN GetCloak() const {return m_bCloakOn;}
	BYTE GetShipType() const {return m_iShipType;}
	BYTE GetShipSize() const {return m_byShipSize;}
	BYTE GetManeuverability() const {return m_byManeuverability;}
	BYTE GetRange() const {return m_iRange;}
	BYTE GetSpeed() const {return m_iSpeed;}
	USHORT GetScanPower() const {return m_iScanPower;}
	BYTE GetScanRange() const {return m_iScanRange;}
	BYTE GetColonizePoints() const {return m_iColonizePoints;}
	BYTE GetStationBuildPoints() const {return m_iStationBuildPoints;}
	BYTE GetCurrentOrder() const {return m_iCurrentOrder;}
	short GetTerraformingPlanet() const {return m_nTerraformingPlanet;}
	const CString& GetShipName() const {return m_strShipName;}
	const CString& GetShipClass() const {return m_strShipClass;}
	const CString& GetShipDescription() const {return m_strShipDescription;}
	CString GetShipTypeAsString(BOOL plural = FALSE) const;
	CString GetCurrentOrderAsString() const;
	BOOLEAN GetIsShipFlagShip() const {return m_bIsFlagShip;}
	USHORT GetCrewExperience() const {return m_iCrewExperiance;}
	USHORT GetStorageRoom() const {return m_iStorageRoom;}
	USHORT GetUsedStorageRoom(const CArray<CTroopInfo>* troopInfo);
	USHORT GetLoadedResources(BYTE res) const {return m_iLoadedResources[res];}
	COMBAT_TACTICS GetCombatTactic() const {return m_nCombatTactic;}
		
	// zum Schreiben der Membervariablen
	void SetID(USHORT ID) {m_iID = ID+10000;}
	void SetKO(const CPoint& KO) {m_KO = KO;}
	void SetTargetKO(const CPoint& TargetKO, int Index) {m_TargetKO[Index] = TargetKO; if (m_iCurrentOrder > AVOID) m_iCurrentOrder = ATTACK; m_nTerraformingPlanet = -1;}
	void SetOwnerOfShip(const CString& sOwnerOfShip) {m_sOwnerOfShip = sOwnerOfShip;}
	void SetMaintenanceCosts(USHORT MaintenanceCosts) {m_iMaintenanceCosts = MaintenanceCosts;}
	void SetShipType(BYTE ShipType) {m_iShipType = ShipType;}
	void SetShipSize(BYTE size) {m_byShipSize = size;}
	void SetManeuverability(BYTE value) {m_byManeuverability = value;}
	void SetSpeed(BYTE Speed) {m_iSpeed = Speed;}
	void SetRange(BYTE Range) {m_iRange = Range;}
	void SetScanPower(USHORT ScanPower) {m_iScanPower = ScanPower;}
	void SetScanRange(BYTE ScanRange) {m_iScanRange = ScanRange;}
	void SetCrewExperiance(int nAdd);
	void SetStealthPower(BYTE StealthPower) {m_iStealthPower = StealthPower;}
	void SetCloak() {m_bCloakOn = !m_bCloakOn;}
	void SetStorageRoom(USHORT StorageRoom) {m_iStorageRoom = StorageRoom;}
	void SetLoadedResources(USHORT add, BYTE res) {m_iLoadedResources[res] += add;}
	void SetColonizePoints(BYTE ColonizePoints) {m_iColonizePoints = ColonizePoints;}
	void SetStationBuildPoints(BYTE StationBuildPoints) {m_iStationBuildPoints = StationBuildPoints;}
	void SetCurrentOrder(BYTE CurrentOrder) {m_iCurrentOrder = CurrentOrder;}
	void SetSpecial(BOOLEAN n, BYTE value) {m_bySpecial[n] = value;}
	void SetTerraformingPlanet(short planetNumber) {m_nTerraformingPlanet = planetNumber;}
	void SetShipName(const CString& ShipName) {m_strShipName = ShipName;}
	void SetShipDescription(const CString& ShipDescription) {m_strShipDescription = ShipDescription;}
	void SetShipClass(const CString& ShipClass) {m_strShipClass = ShipClass;}
	void SetIsShipFlagShip(BOOLEAN is) {m_bIsFlagShip = is;}
	void SetCombatTactic(COMBAT_TACTICS nTactic) {m_nCombatTactic = nTactic;}
	
	// sonstige Funktionen
	void CreateFleet();	// Bevor wir mit der Flotte arbeiten können muß diese erst created werden
	void CheckFleet();	// am besten in jeder neuen Runde aufrufen, säubert die Flotte (aber nicht unbedingt notwendig)
	void DeleteFleet();	// wie es der Name schon sagt wird hier die Flotte gelöscht

	/// Funktion gibt einen Wahrheitswert zurück, ob das Schiffsobjekt eine bestimmte Spezialfähigkeit besitzt.
	/// @param ability Spezialfähigkeit
	/// @return <code>TRUE</code> wenn es diese Fähigkeit besitzt, ansonsten <code>FALSE</code>
	BOOLEAN HasSpecial(BYTE ability) const;

	/// Funktion erstellt eine Tooltipinfo vom Schiff
	/// @param bShowFleet wenn dieser Parameter <code>true</code> dann werden Informationen über die angeführte Flotte angezeigt, sonst nur über das Schiff
	/// @return	der erstellte Tooltip-Text
	CString GetTooltip(bool bShowFleet = true);

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
	void DrawShip(Gdiplus::Graphics* g, CGraphicPool* pGraphicPool, const CPoint& pt, bool bIsMarked, bool bOwnerUnknown, bool bDrawFleet, const Gdiplus::Color& clrNormal, const Gdiplus::Color& clrMark, const Gdiplus::Font& font) const;
	
protected:
	CHull	m_Hull;												// die Hülle des Schiffes
	CShield m_Shield;											// die Schilde des Schiffes
	CArray<CTorpedoWeapons,CTorpedoWeapons> m_TorpedoWeapons;	// die Torpedobewaffnung des Schiffes
	CArray<CBeamWeapons,CBeamWeapons>		m_BeamWeapons;		// die Beamfirebewaffnung des Schiffes
	
	USHORT m_iID;						// ID des Schiffes
	BYTE m_iCurrentOrder;				// Aktueller Befehl des Schiffes, nutze dazu enum
	USHORT m_iMaintenanceCosts;			// Unterhaltskosten des Schiffes pro Runde
	BYTE m_iShipType;					// Schiffstype, siehe Options.h
	BYTE m_byShipSize;					// Die Größe des Schiffes
	BYTE m_byManeuverability;			// Die Manövriebarkeit des Schiffes im Kampf
	BYTE m_iSpeed;						// Geschwindigkeit des Schiffes in der CMenuChooseView
	BYTE m_iRange;						// Reichweite des Schiffes
	USHORT m_iScanPower;				// Scankraft des Schiffes
	BYTE m_iScanRange;					// Die Reichweite der Scanner
	BYTE m_iStealthPower;				// Tarnstärke des Schiffes: 0 = keine Tarnung, 1 = bissl ...
	USHORT m_iStorageRoom;				// Laderaum des Schiffes
	BYTE m_iColonizePoints;				// Kolonisierungs/Terraform-Punkte -> Dauer
	BYTE m_iStationBuildPoints;			// Außenposten/Stations-baupunkte -> Dauer
	BYTE m_bySpecial[2];				// Die beiden möglichen Spezialfähigkeiten des Schiffes
	CString m_strShipName;				// Der Name des Schiffes
	CString m_strShipDescription;		// Die Beschreibung des Schiffes
	CString m_strShipClass;				// Der Name der Schiffsklasse	

private:
	CString m_sOwnerOfShip;				// Besitzer des Schiffes
	CFleet*	m_Fleet;					// Wenn wir eine Gruppe bilden und dieses Schiff hier Gruppenleader ist, dann werden die anderen Schiffe in die Fleet genommen
	CPoint m_KO;						// Koordinate des Schiffes im Raum (welcher Sector?)
	CPoint m_TargetKO[4];				// Der Zielkurs des Schiffes
	CArray<Sector> m_Path;				// Der Kurs des Schiffes zum Ziel
	BOOLEAN m_bCloakOn;					// ist die Tarnung eingeschaltet
	short m_nTerraformingPlanet;		// Nummer des Planeten der kolonisiert werden soll
	BOOLEAN m_bIsFlagShip;				// Ist dieses Schiff ein Flagschiff (es kann immer nur ein Schiff eines Imperiums Flagschiff sein)
	USHORT m_iCrewExperiance;			// Crewerfahrung des Schiffes
	// Laderaum
	CArray<CTroop> m_Troops;				// Die Truppen, welche auf dem Schiff transportiert werden.
	USHORT m_iLoadedResources[DILITHIUM+1];	// Die geladenen Ressourcen auf dem Schiff
	// Kampftaktik
	COMBAT_TACTICS m_nCombatTactic;		///< Taktik des Schiffes im Kampf
};

typedef CArray<CShip, CShip> ShipArray;	// Das dynamische Feld wird vereinfacht als ShipArray angegeben

#endif // !defined(AFX_SHIP_H__C9FE4406_A0D7_4AE0_A5D0_0070FCBC45C1__INCLUDED_)
 