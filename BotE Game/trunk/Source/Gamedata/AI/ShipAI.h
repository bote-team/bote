/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "Options.h"
#include <map>

using namespace std;

class CBotf2Doc;
class CSectorAI;
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
	/// Funktion erteilt einen Terraformbefehl, sofern dies auch möglich ist. Übergebn wird dafür der index
	/// <code>index</code> des Schiffes aus dem Schiffsarray des Documents. Der Rückgabewert der Funktion ist
	/// <code>TRUE</code>, wenn ein Terraformbefehl gegeben werden könnte.
	BOOLEAN DoTerraform(int index);

	/// Funktion erteilt einen Kolonisierungsbefehl, sofern dies auch möglich ist. Übergeben wird dafür der index
	/// <code>index</code> des Schiffes aus dem Schiffsarray des Dokuments. Der Rückgabewert der Funktion ist
	/// <code>TRUE</code>, wenn ein Kolonisierungsbefehl gegeben werden könnte.
	BOOLEAN DoColonize(int index);

	/// Funktion schickt Kriegsschiffe zu einem möglichen Offensivziel. Unter Umständen wird auch der Befehl zum
	/// Angriff automatisch erteilt. Auch kann es passieren, das diese Funktion die Kriegsschiffe zu einem
	/// gemeinsamen Sammelpunkt schickt. Übergebn wird dafür der index <code>index</code> des Schiffes aus dem
	/// Schiffsarray des Documents. Der Rückgabewert der Funktion ist <code>TRUE</code>, wenn ein Bewegungsbefehl
	/// gegeben werden könnte.
	BOOLEAN DoAttackMove(int index);

	/// Funktion schickt Kriegsschiffe zu einem möglichen System, welches Bombardiert werden könnte. Übergeben wird
	/// dafür der index <code>index</code> des Schiffes aus dem Schiffsarray des Documents. Der Rückgabewert der
	/// Funktion ist <code>TRUE</code>, wenn ein Bewegungsbefehl gegeben werden konnte oder eine Bombardierung
	/// befohlen wurde oder das Schiff auf Verstärkung zur Bombardierung im System wartet.
	BOOLEAN DoBombardSystem(int index);

	/// Funktion erteilt einen Tarnbefehl, wenn <code>camouflage</code> wahr ist. Ist <code>camouflage</code> falsch,
	/// wird ein Enttarnbefehl gegeben. Übergebn wird dafür der index <code>index</code> des Schiffes aus dem
	/// Schiffsarray des Documents. Der Rückgabewert der Funktion ist <code>TRUE</code>, wenn der Befehl gegeben wurde.
	BOOLEAN DoCamouflage(int index, BOOLEAN camouflage = TRUE);

	/// Funktion erteilt einen Außenpostenbaubefehl, sofern dies auch möglich ist. Übergeben wird dafür der index
	/// <code>index</code> des Schiffes aus dem Schiffsarray des Dokuments. Der Rückgabewert der Funktion ist
	/// <code>TRUE</code>, wenn ein Außenpostenbaubefehl gegeben werden könnte.
	BOOLEAN DoStationBuild(int index);

	/// Funktion erstellt eine Flotte. Schiffe werden der Flotte nur hinzugefügt, wenn diese bestimmte Voraussetzungen erfüllen.
	/// So muss der ungefähre Schiffstyp übereinstimmen (Combat <-> NonCombat) sowie die Geschwindigkeit des Schiffes.
	/// @param index Index des aktuellen Schiffes im Array.
	void DoMakeFleet(int index);
	
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
