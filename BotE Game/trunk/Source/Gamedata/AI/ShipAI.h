/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Options.h"
#include <map>

using namespace std;

// forward declaration
class CBotf2Doc;
class CSectorAI;
class CMajor;
class CShips;

class CShipAI
{
public:
	/// Konstruktor
	CShipAI(CBotf2Doc* pDoc);

	/// Destruktor
	~CShipAI(void);

	/// Diese Funktion erteilt allen Schiffen aller computergesteuerten Rassen Befehle.
	void CalculateShipOrders(CSectorAI* SectorAI);

private:
	/// Funktion erteilt einen Terraformbefehl, sofern dies auch möglich ist.
	/// @param pShip Zeiger des Schiffes
	/// @return <code>true</code> wenn ein Terraformbefehl gegeben werden könnte
	bool DoTerraform(CShips* pShip);

	/// Funktion erteilt einen Kolonisierungsbefehl, sofern dies auch möglich ist.
	/// @param pShip Zeiger des Schiffes
	/// @return <code>true</code> wenn ein Kolonisierungsbefehl gegeben werden könnte
	bool DoColonize(CShips* pShip);

	/// Funktion schickt Kriegsschiffe zu einem möglichen Offensivziel. Unter Umständen wird auch der Befehl zum
	/// Angriff automatisch erteilt. Auch kann es passieren, das diese Funktion die Kriegsschiffe zu einem
	/// gemeinsamen Sammelpunkt schickt.
	/// @param pShip Zeiger des Schiffes
	/// @param pMajor Zeiger auf den Besitzer des Schiffes
	/// @return <code>true</code> wenn ein Bewegungsbefehl gegeben werden könnte
	bool DoAttackMove(CShips* pShip, const CMajor* pMajor);

	/// Funktion schickt Kriegsschiffe zu einem möglichen System, welches Bombardiert werden könnte.
	/// @param pShip Zeiger des Schiffes
	/// @return <code>true</code> wenn ein Bewegungsbefehl gegeben werden konnte, eine Bombardierung befohlen wurde oder das Schiff auf Verstärkung zur Bombardierung im System wartet
	bool DoBombardSystem(CShips* pShip);

	/// Funktion erteilt einen Tarnbefehl oder Enttarnbefehl.
	/// @param pShip Zeiger des Schiffes
	/// @param bCamouflage <code>true</code> für Tarnen, <code>false</code> für Enttarnen
	/// @return <code>true</code> wenn der Befehl gegeben wurde
	bool DoCamouflage(CShips* pShip, bool bCamouflage = true);

	/// Funktion erteilt einen Außenpostenbaubefehl, sofern dies auch möglich ist.
	/// @param pShip Zeiger des Schiffes
	/// @return <code>true</code> wenn ein Außenpostenbaubefehl gegeben werden könnte
	bool DoStationBuild(CShips* pShip);

	/// Funktion erstellt eine Flotte. Schiffe werden der Flotte nur hinzugefügt, wenn diese bestimmte Voraussetzungen erfüllen.
	/// So muss der ungefähre Schiffstyp übereinstimmen (Combat <-> NonCombat) sowie die Geschwindigkeit des Schiffes.
	/// @param pShip Zeiger des Schiffes
	/// @param nIndex Index des aktuellen Schiffes im Array.
	void DoMakeFleet(CShips* pShip, int nIndex);

	/// Funkion berechnet einen möglichen Angriffssektor, welcher später gesammelt angegriffen werden kann.
	void CalcAttackSector(void);

	/// Funktion berechnet einen möglich besten Sektor für eine Bombardierung. Wurde solch ein Sektor ermittelt hat dieser
	/// die allerhöchste Priorität.
	void CalcBombardSector(void);

	/// Ein Zeiger auf das Document.
	CBotf2Doc* m_pDoc;

	/// Ein Zeiger auf das SectorAI-Objekt
	CSectorAI* m_pSectorAI;

	map<CString, CPoint> m_AttackSector;	///< der globale Angriffssektor der einzelnen Rassen

	map<CString, CPoint> m_BombardSector;	///< der globale Bombardierungssektor der einzelnen Rassen

	map<CString, int> m_iAverageMoral;		///< die durchschnittliche Moral in allen Systemen der einzelnen Rassen
};
