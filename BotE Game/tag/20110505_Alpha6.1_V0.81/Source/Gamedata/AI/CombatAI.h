/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

#include <map>

using namespace std;

// forward declaration
class CShip;
class CRace;
class CAnomaly;

class CCombatAI
{
public:
	/// Konstruktor
	CCombatAI(void);

	/// Destruktor
	virtual ~CCombatAI(void);

	/// Funktion berechnet die Taktik aller im Kampf beteiligten Schiffe.
	/// @param vInvolvedShips Vektor mit allen beteiligten Schiffen
	/// @param pmRaces alle Rassen des Spiels
	/// @param mCombatOrders allgemeiner Kampfbefehl aller beteiligten Rassen
	/// @param pAnomaly eventuell vorhandene Anomalie im Kampfsektor
	/// @return <code>true</code> wenn ein Kampf nach der Taktikberechnung noch stattfinden kann, sonst <code>false</code>
	bool CalcCombatTactics(const CArray<CShip*>& vInvolvedShips, const map<CString, CRace*>* pmRaces, map<CString, int>& mCombatOrders, const CAnomaly* pAnomaly) const;

private:
	/// Funktion legt die allgemeine Taktik der Rasse für den bevorstehenden Kampf fest.
	/// @param vInvolvedShips Vektor mit allen beteiligten Schiffen
	/// @param pmRaces alle Rassen des Spiels
	/// @param mCombatOrders allgemeiner Kampfbefehl aller beteiligten Rassen
	/// @param pAnomaly eventuell vorhandene Anomalie im Kampfsektor
	void ApplyCombatOrders(const CArray<CShip*>& vInvolvedShips, const map<CString, CRace*>* pmRaces, map<CString, int>& mCombatOrders, const CAnomaly* pAnomaly) const;

	/// Funktion erteilt allen im Kampf beteiligten Schiffen ihre zuvor ausgewählte Taktik.
	/// @param vInvolvedShips Vektor mit allen beteiligten Schiffen	
	/// @param mCombatOrders allgemeiner Kampfbefehl aller beteiligten Rassen
	void ApplyShipTactics(const CArray<CShip*>& vInvolvedShips, map<CString, int>& mCombatOrders) const;
};
