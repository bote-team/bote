/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "System.h"
#include "Ships\Ship.h"
#include "Races\RaceController.h"
#include <map>

using namespace std;

class CAttackSystem : public CObject
{
public:
	/// Konstruktor
	CAttackSystem(void);

	/// Destruktor
	~CAttackSystem(void);

	/// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	
	/// Diese Funktion gibt das Feld der News zurück
	CStringArray* GetNews() {return &m_strNews;}

	/// Diese Funktion gibt die Anzahl der vernichteten Bevölkerung beim Systemangriff zurück.
	/// @return vernichtete Bevölkerung in Mrd.
	float GetKilledPop() const {return m_fKilledPop;}

	/// Funktion gibt die Anzahl der beim Systemangriff zerstörten Gebäude zurück.
	/// @return Anzahl der zerstörtden Gebäude
	short GetDestroyedBuildings() const {return m_iDestroyedBuildings;}

	/// Diese Funktion gibt zurück, ob Truppen an der Invasion beteiligt waren und diese auch erfolgreich verlief.
	BOOLEAN IsTroopsInvolved() const {return m_bTroopsInvolved;}

	// sonstige Funktionen
	/**
	 * Diese Funktion initiiert das CAttackSystem Objekt mit den entsprechenden Variablen. Dabei übernimmt sie als
	 * Parameter einen Zeiger auf die verteidigende Rasse <code>pDefender</code>, einen Zeiger auf das System <code>system</code>,
	 * welches angegriffen wird, einen Zeiger auf das komplette Feld aller Schiffe <code>ships</code>, einen Zeiger auf den
	 * zum System gehörenden Sektor <code>sector</code>, einen Zeiger auf die Gebäudeinformationen <code>buildingInfos</code>
	 * und das Feld mit den Monopolbesitzern <code>monopolOwner</code>.
	 */
	void Init(CRace* pDefender, CSystem* system, ShipArray* ships, CSector* sector, BuildingInfoArray* buildingInfos, const CString* monopolOwner);

	/**
	 * Diese Funktion führt den Angriff durch. Außerdem werden alle Berechnungen der Auswirkungen des Angriffs 
	 * durchgeführt. Der Rückgabewert ist <code>TRUE</code>, wenn der Angriff erfolgreich war, bei Misserfolg
	 * ist der Rückgabewert <code>FALSE</code>.
	 */
	BOOLEAN Calculate();

	/// Diese Funktion gibt zurück, ob der Verteidiger ungleich dem/den Angreifer/n ist.
	/// @param defender Verteidiger des Systems
	/// @param attacker Feld mit allen eingreifern auf das Systems (meist nur einer)
	/// @return <code>TRUE</code> wenn Verteidiger ungleich allen Angreifern, ansonsten <code>FALSE</code>
	BOOLEAN IsDefenderNotAttacker(const CString& sDefender, const set<CString>* attacker) const;

private:
	// Attribute

	/// Dieses dynamische Feld beinhaltet alle Schiffe die an dem Systemangriff beteiligt sind.
	CArray<CShip*> m_pShips;

	/// Dieses dynamische Feld beinhaltet alle Truppen, die das System angreifen. Diese mussten
	/// mittels Schiffen herbeigebracht wurden sein.
	CArray<CTroop*> m_pTroops;

	/// Ein Zeiger auf die verteidigende Rasse.
	CRace* m_pDefender;

	/// Ein Zeiger auf das System, welches angegriffen wird.
	CSystem* m_pSystem;

	/// Ein Zeiger auf den Sektor, in welchem der Angriff stattfindet.
	CSector* m_pSector;

	/// Ein Zeiger auf die Gebäudeinformationen
	BuildingInfoArray* m_pBuildingInfos;

	/// Die Koordinate des Systems auf der Map
	CPoint m_KO;

	/// Ein Zeiger auf das Feld der Monopolbesitzer
	const CString* m_sMonopolOwner;

	/// StringFeld zur Nachrichtenrückgabe über einen Angriff. Muss nicht serialisiert werden
	CStringArray m_strNews;

	/// Wahrheitswert, ob beim Angriff auf das System Truppen mit dabei sind. Muss nicht serialisiert werden
	BOOLEAN m_bTroopsInvolved;

	/// Wahrheitswert, ob die Truppen bei ihrem Angriff von einem Assaultschiff unterstützt werden. Dies gibt ihnen
	/// einen Angriffsbonus von 20%. Muss nicht serialisiert werden.
	BOOLEAN m_bAssultShipInvolved;

	/// Die beim Angriff vernichtete Bevölkerung in Mrd.
	float m_fKilledPop;

	/// Anzahl der vernichteten Gebäude beim Systemangriff.
	short m_iDestroyedBuildings;

	// Funktionen

	/// Private Funktion, die allein die Schiffsabwehr berechnet
	void CalculateShipDefence();

	/// Private Funktion, die allein die Systembombardierung beachtet
	void CalculateBombAttack();

	/// Private Funktion, die allein den Angriff durch Truppen berechnet
	void CalculateTroopAttack();

};
