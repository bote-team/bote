/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
/*
 *@file
 * properties of a map location with a sun system
 */
#pragma once
#include "MapTile.h"
#include "Planet.h"
#include <map>
#include <set>
#include <vector>
#include <cassert>

#include "BotE.h"

using namespace std;

// forward declaration
class CMajor;
class CAnomaly;
class CRace;
class CShips;
class CShip;

class CSector : public CMapTile
{

public:

//////////////////////////////////////////////////////////////////////
// construction/destruction
//////////////////////////////////////////////////////////////////////

	/// Konstruktor
	CSector(void);
	CSector(int x, int y);

	///Kopierkontruktor
	CSector(const CSector& other);

	CSector& operator=(const CSector&);

	/// Destruktor
	virtual ~CSector(void);

protected:
	/// Resetfunktion für die Klasse CSector
	void Reset(bool call_up);
public:

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

//////////////////////////////////////////////////////////////////////
// iterators
//////////////////////////////////////////////////////////////////////

	//iterators
	typedef std::vector<CPlanet>::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;
private:
	typedef std::vector<CPlanet>::iterator iterator;
	iterator begin();
	iterator end();
public:

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

	/// Diese Funktion gibt zurück, wem dieser Sektor zuerst gehörte. Aber nur, wenn es sich dabei
	/// um eine Kolonie oder um das Heimatsystem handelte.
	CString GetColonyOwner(void) const {return m_sColonyOwner;}

	/// Die Funktion gibt die Farbe der Sonne in diesem System zurück. Der Rückgabewert ist ein BYTE-Wert, also nicht
	///die Farbe als Adjektiv.
	BYTE GetSunColor() const {return m_bySunColor;}

	/// Diese Funktion gibt die Anzahl der Planeten zurück, die in dem Sektor beheimatet sind.
	BYTE GetNumberOfPlanets(void) const {return m_Planets.size();}

	/// Diese Funktion gibt einen Zeiger auf einen Planeten in diesem System zurück.
	const CPlanet* GetPlanet(BYTE nPlanetIndex) const {return &m_Planets[nPlanetIndex];}

	/// Funktion gibt alle Einwohner aller Planeten in dem Sektor zurück.
	float GetCurrentHabitants() const;

	/// Diese Funktion berechnet die vorhandenen Rohstoffe der Planeten im Sektor. Übergebn wird dafür ein Feld für
	/// die Ressourcen <code>res</code>.
	void GetAvailableResources(BOOLEAN bResources[RESOURCES::DERITIUM + 1], BOOLEAN bOnlyColonized = true) const;

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

	/// Funktion legt fest, welcher Majorrace der Sektor zuerst gehörte. Das gilt aber nur für
	/// eigene Kolonien und dem Heimatsektor
	void SetColonyOwner(const CString& sOwner) {m_sColonyOwner = sOwner;}

//////////////////////////////////////////////////////////////////////
// planets
//////////////////////////////////////////////////////////////////////

	/**
	 * Funktion generiert den Sektor. Dabei wird als Parameter die Wahrscheinlichkeit, ob in dem Sektor ein
	 * Sonnensystem ist, im Paramter <code>sunProb</code> in Prozent übergeben. Im Parameter <code>minorProb</code>
	 * wird die Wahrscheinlichkeit in Prozent übergeben, dass sich in dem Sektor eine Minorrace befindet.
	 */
	void GenerateSector(int sunProb, int minorProb);

	/// Diese Funktion generiert die Planeten in dem Sektor.
	/// @param majorNumber Nummer der Majorrace, wenn deren Planeten im Sektor generiert werden soll. Wird kein Wert
	/// angegeben, so werden zufällige Planeten erstellt.
	void CreatePlanets(const CString& sMajorID = "");

	/// Diese Funktion führt das Planetenwachstum für diesen Sektor durch.
	void LetPlanetsGrowth();
	/// Diese Funktion lässt die Bevölkerung auf allen Planeten zusammen um den übergebenen Wert <code>Value</code>
	/// schrumpfen.
	void LetPlanetsShrink(float Value);

	//Sets whether to display planets as being terraformed based on all currently terraforming ships in this sector
	void RecalcPlanetsTerraformingStatus();

	int CountOfTerraformedPlanets() const;

	void DistributeColonists(const float colonists);

	void Terraforming(CShip& ship);

	bool PerhapsMinorExtends(BYTE TechnologicalProgress);

	void CreateDeritiumForSpaceflightMinor();

	bool Terraform(const CShips& ship);

	void SystemEventPlanetMovement(CString& message);
	void SystemEventDemographic(CString& message, CMajor& major);

//////////////////////////////////////////////////////////////////////
// other functions
//////////////////////////////////////////////////////////////////////

	/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
	/// machen, werden diese Variablen später ja wieder korrekt gesetzt. Außerdem werden auch die Besitzerpunkte wieder
	/// gelöscht.
	void ClearAllPoints(bool call_up);

//////////////////////////////////////////////////////////////////////
// members
//////////////////////////////////////////////////////////////////////

protected:

	/// Die Feld mit den einzelnen Planeten in dem Sektor
	std::vector<CPlanet> m_Planets;

	/// Wem gehört/gehörte der Sektor zuerst? Also eigene Kolonie und Heimatsystem
	CString m_sColonyOwner;

private:

	/// Die Farbe der Sonne in diesem Sektor
	BYTE m_bySunColor;

};
