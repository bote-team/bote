/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// Sector.h: Schnittstelle für die Klasse CSector.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "Planet.h"
#include "GenSectorName.h"

/// Liefert verschiedene Attributswerte der Sektorklasse.
enum SectorAttributes
{
	/// Ist ein Sonnensystem in diesem Sektor?
	SECTOR_SUNSYSTEM	= 1,
	/// Gehört der Sektor einer irgendeiner Rasse?
	SECTOR_OWNED		= 2,
	/// Gibt es eine kleine Rasse in dem Sector
	SECTOR_MINORRACE	= 4,
	/// Wurde der Sector militärisch eingenommen, also keine eigene Kolonie oder Heimatsystem
	SECTOR_CONQUERED	= 8
};

class CSector : public CObject  
{
public:
	// Klasse serialisierbar machen
	DECLARE_SERIAL (CSector)
	
	/// Konstruktor
	CSector(void);
	
	/// Destruktor
	virtual ~CSector(void);								
	
	/// Kopierkonstruktor
	CSector(const CSector & rhs);
	
	/// Zuweisungsoperator
	CSector & operator=(const CSector &);
	
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

// Zugriffsfunktionen zum Lesen der Membervariablen
	/// Diese Funktion gibt die Koordinaten des Sektors zurück.
	CPoint GetKO() const {return m_KO;}

	/**
	 * Funktion gibt den Namen des Sektors zurück. Wenn in ihm kein Sonnensystem ist, dann wird "" zurückgegeben.
	 * Wenn man aber den Parameter <code>longName<code> beim Aufruf der Funktion auf <code>TRUE<code> setzt, wird
	 * versucht ein genauerer Sektorname zu generieren.
	 */
	CString GetName(BOOLEAN longName = FALSE) const;
	
	/// Diese Funktion gibt zurück, ob sich in diesem Sektor ein Sonnensystem befindet.
	BOOLEAN GetSunSystem(void) const {return (m_Attributes & SECTOR_SUNSYSTEM) == SECTOR_SUNSYSTEM;}

	/// Diese Funktion gibt zurück, ob der Sektor irgendeiner Majorrasse gehört.
	BOOLEAN GetOwned(void) const {return (m_Attributes & SECTOR_OWNED) == SECTOR_OWNED;}
	
	/// Diese Funktion gibt zurück, ob sich in diesem Sektor eine Minorrace befindet.
	BOOLEAN GetMinorRace(void) const {return (m_Attributes & SECTOR_MINORRACE) == SECTOR_MINORRACE;}
	
	/// Diese Funktion gibt zurück, ob der Sektor militärisch erobert wurde.
	BOOLEAN GetTakenSector(void) const {return (m_Attributes & SECTOR_CONQUERED) == SECTOR_CONQUERED;}
	
	/// Diese Funktion gibt zurück, wer im Besitz dieses Sektors ist.
	BYTE GetOwnerOfSector(void) const {return m_byOwnerOfSector;}
	
	/// Diese Funktion gibt zurück, wem dieser Sektor zuerst gehörte. Aber nur, wenn es sich dabei
	/// um eine Kolonie oder um das Heimatsystem handelte.
	BYTE GetColonyOwner(void) const {return m_byColonyOwner;}
	
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob dieser Sektor von der Majorrace
	/// <code>Race</code> gescannt wurde.
	BOOLEAN GetScanned(int Race) const {return (m_byStatus[Race] >= 1);}
	
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob der Name dieses Sektor der
	/// Majorrace <code>Race</code> bekannt ist.
	BOOLEAN GetKnown(int Race) const {return (m_byStatus[Race] >= 2);}

	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// den kompletten Sektor (inkl. der Planeten) kennt.
	BOOLEAN GetFullKnown(int Race) const {return (m_byStatus[Race] >= 3);}
	
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// eine online Werft (bzw. kann auch eine Station sein) in diesem Sektor besitzt.
	BOOLEAN GetShipPort(int Race) const {return m_bShipPort[Race];}
	
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// einen Aussenposten in diesem Sektor besitzt.
	BOOLEAN GetOutpost(int Race) const {return m_bOutpost[Race];}
	
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// eine Sternbasis in diesem Sektor besitzt.
	BOOLEAN GetStarbase(int Race) const {return m_bStarbase[Race];}
	
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Rasse <code>Race</code> ein
	/// bzw. mehrere Schiffe in diesem Sektor hat.
	BOOLEAN GetOwnerOfShip(int Race) {return m_bWhoIsOwnerOfShip[Race];}
	
	/// Diese Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Majorrace <code>Race</code>
	/// gerade eine Station in diesem Sektor baut.
	BOOLEAN GetIsStationBuilding(int Race) const {return m_bIsStationBuild[Race];}
	
	/// Diese Funktion gibt die Scanpower zurück, die die Majorrace <code>Race</code> in diesem Sektor hat.
	short GetScanPower(BYTE Race) const {return m_iScanPower[Race];}
	
	/// Diese Funktion gibt die Scanpower zurück, die man benötigt, um das Schiff/die Schiffe der
	/// Majorrace <code>Race</code> in diesem Sektor zu entdecken.
	short GetNeededScanPower(BYTE Race) const {return m_iNeededScanPower[Race];}
	
	/// Diese Funktion gibt die benötigten Punkte zum Stationenbau zurück, die die Majorrace
	/// <code>Race</code> in diesem Sektor noch zur Fertigstellung einer Station benötigt.
	short GetNeededStationPoints(int Race) const {return m_iNeededStationPoints[Race];}
	
	/// Diese Funktion gibt die Punkte zum Stationenbau zurück, die die Majorrace
	/// <code>Race</code> zu Beginn des Stationenbaus benötigt hat.
	short GetStartStationPoints(int Race) const {return m_iStartStationPoints[Race];}
		
	/// Die Funktion gibt die Farbe der Sonne in diesem System zurück. Der Rückgabewert ist ein BYTE-Wert, also nicht
	///die Farbe als Adjektiv.
	BYTE GetSunColor() const {return m_bySunColor;}
	
	/// Diese Funktion gibt die Anzahl der Planeten zurück, die in dem Sektor beheimatet sind.
	BYTE GetNumberOfPlanets(void) const {return m_Planets.GetSize();}	
	
	/// Diese Funktion gibt einen Zeiger auf einen Planeten in diesem System zurück.
	CPlanet* GetPlanet(short NumberOfPlanet) {return &m_Planets[NumberOfPlanet];}

	/// Diese Funktion gibt einen Zeiger auf das Feld mit den Planeten in diesem System zurück.
	CArray<CPlanet>* GetPlanets() {return &m_Planets;}

	/// Funktion gibt alle Einwohner aller Planeten in dem Sektor zurück.
	float GetCurrentHabitants() const;

	/// Diese Funktion berechnet die vorhandenen Rohstoffe der Planeten im Sektor. Übergebn wird dafür ein Feld für
	/// die Ressourcen <code>res</code>.
	void GetAvailableResources(BOOLEAN res[5]);

	/// Diese Funktion gibt die Schiffspfadpunkte zurück.
	short GetShipPathPoints() const {return m_iShipPathPoints;}
	
// zum Schreiben der Membervariablen
	/// Diese Funktion setzt die Koordinaten des Sektors und übernimmt dabei die beiden Koordinatenwerte.
	void SetKO(int x, int y) {m_KO = CPoint(x,y);}

	/// Funktion legt den Namen des Sektors fest.
	void SetSectorsName(CString nameOfSunSystem) {m_strSectorName = nameOfSunSystem;}
	
	/// Diese Funktion legt fest, ob sich ein Sonnensystem in diesem Sektor befindet.
	void SetSunSystem(BOOLEAN is) {SetAttributes(is, SECTOR_SUNSYSTEM, m_Attributes);}
	
	/// Funktion legt fest, ob der Sektor irgendwem gehört.
	void SetOwned(BOOLEAN is) {SetAttributes(is, SECTOR_OWNED, m_Attributes);}
	
	/// Funktion legt fest, ob sich eine Minorrace in diesem Sektor befindet.
	void SetMinorRace(BOOLEAN is) {SetAttributes(is, SECTOR_MINORRACE, m_Attributes);}
	
	/// Funktion legt fest, ob der Sektor militärisch erobert wurde.
	void SetTakenSector(BOOLEAN is) {SetAttributes(is, SECTOR_CONQUERED, m_Attributes);}
	
	/// Funktion legt fest, wem der Sektor gehört.
	void SetOwnerOfSector(BYTE owner) {m_byOwnerOfSector = owner;}
	
	/// Funktion legt fest, welcher Majorrace der Sektor zuerst gehörte. Das gilt aber nur für
	/// eigene Kolonien und dem Heimatsektor
	void SetColonyOwner(BYTE owner) {m_byColonyOwner = owner;}
	
	/// Diese Funktion legt fest, ob der Sektor von der Majorrace <code>Race</code> gescannt ist.
	void SetScanned(BYTE Race) {if (this->GetScanned(Race) == FALSE) m_byStatus[Race] = 1;}
	
	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> den Namen des Sektor kennt.
	void SetKnown(BYTE Race) {if (this->GetKnown(Race) == FALSE) m_byStatus[Race] = 2;}

	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> den Sektor kompletten Sektor
	/// (inkl. der ganzen Planeten) kennt.
	void SetFullKnown(BYTE Race) {m_byStatus[Race] = 3;}
	
	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> eine online Werft (bzw. auch durch eine
	/// Station erhalten) in diesem Sektor besitzt.
	void SetShipPort(BOOLEAN is, BYTE Race) {m_bShipPort[Race] = is;}
	
	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> einen Aussenposten in diesem Sektor unterhält.
	void SetOutpost(BOOLEAN is, BYTE Race) {m_bOutpost[Race] = is;}
	
	/// Diese Funktion legt fest, ob die Majorrace <code>Race</code> eine Sternbasis in diesem Sektor unterhält.
	void SetStarbase(BOOLEAN is, BYTE Race) {m_bStarbase[Race] = is;}
	
	/// Funktion legt fest, ob die Majorrace <code>Race</code> ein oder auch mehrer Schiffe in diesem Sektor hat.
	void SetOwnerOfShip(BOOLEAN is, BYTE owner) {m_bWhoIsOwnerOfShip[owner] = is;}
	
	/// Funktion legt fest, ob die Majorrace <code>Race</code> gerade eine Station in diesem Sektor baut.
	void SetIsStationBuilding(BOOLEAN is, BYTE Race) {m_bIsStationBuild[Race] = is;}
	
	/// Funktion legt die Scanpower <code>scanpower</code>, welche die Majorrace <code>Race</code> 
	/// in diesem Sektor hat, fest.
	void SetScanPower(short scanpower, BYTE Race) {m_iScanPower[Race] = scanpower;}
	
	/// Funktion legt die Scanpower <code>scanpower</code> fest, welche benötigt wird, um ein Schiff der
	/// Majorrace <code>Race</code> in diesem Sektor zu erkennen.
	void SetNeededScanPower(short scanpower, BYTE Race) {m_iNeededScanPower[Race] = scanpower;}
	
	/// Diese Funktion zieht <code>sub</code> Punkte von den noch benötigten Stationbaupunkten der Majorrace
	/// <code>Race</code> ab.
	BOOLEAN SetNeededStationPoints(int sub, BYTE Race) {m_iNeededStationPoints[Race] -= sub; if (m_iNeededStationPoints[Race] <= 0) return TRUE; else return FALSE;}
	
	/// Diese Funktion legt die Stationsbaupunkte <code>value</code> der Majorrace <code>Race</code> fest,
	/// welche zu Beginn des Baus benötigt werden.
	void SetStartStationPoints(int value, BYTE Race) {m_iStartStationPoints[Race] = value; m_iNeededStationPoints[Race] = value;}

	/// Diese Funktion addiert Besitzerpunkte <code>ownerPoints</code> zu den Punkten des Besitzers <code>owner</owner>
	/// dazu.
	void AddOwnerPoints(BYTE ownerPoints, BYTE owner) {m_byOwnerPoints[owner] += ownerPoints;}

	/// Diese Funktion addiert den übergebenen Wert <code>value</code> zu den Schiffspfadpunkten, anhand derer man
	/// erkennen kann, ob ein Schiff in dieser Runde durch diesen Sektor fliegt.
	void AddShipPathPoints(short value) {m_iShipPathPoints += value;}
	
// sonstige Funktionen
	/**
	 * Funktion initialisiert den Namensgenerator für die CSector Klasse. Muss zu Beginn aufgerufen werden.
	 */
	static void InitNameGenerator();
	
	/**
	 * Funktion generiert den Sektor. Dabei wird als Parameter die Wahrscheinlichkeit, ob in dem Sektor ein
	 * Sonnensystem ist, im Paramter <code>sunProb</code> in Prozent übergeben. Im Parameter <code>minorProb</code>
	 * wird die Wahrscheinlichkeit in Prozent übergeben, dass sich in dem Sektor eine Minorrace befindet.
	 */
	void GenerateSector(int sunProb, int minorProb);
	
	/// Diese Funktion generiert die Planeten in dem Sektor.
	/// @param majorNumber Nummer der Majorrace, wenn deren Planeten im Sektor generiert werden soll. Wird kein Wert
	/// angegeben, so werden zufällige Planeten erstellt.
	void CreatePlanets(BYTE majorNumber = NOBODY);
	
	/// Diese Funktion führt das Planetenwachstum für diesen Sektor durch.
	void LetPlanetsGrowth();
	
	/// Diese Funktion lässt die Bevölkerung auf allen Planeten zusammen um den übergebenen Wert <code>Value</code>
	/// schrumpfen.
	void LetPlanetsShrink(float Value);
	
	/// In jeder neuen Runde die IsTerraforming und IsStationBuilding Variablen auf FALSE setzen, wenn Schiffe eine Aktion
	/// machen, werden diese Variablen später ja wieder korrekt gesetzt. Außerdem werden auch die Besitzerpunkte wieder
	/// gelöscht.
	void ClearAllPoints();

	/// Diese Funktion berechnet anhand der Besitzerpunkte und anderen Enflüssen, wem dieser Sektor schlussendlich
	/// gehört. Übergeben wird dafür auch der mögliche Besitzer des Systems in diesem Sektor.
	void CalculateOwner(BYTE systemOwner);

	/// Resetfunktion für die Klasse CSector
	void Reset();

// Zeichenfunktionen für diese Klasse
	/// Diese Funktion zeichnet den Namen des Sektors.
	void DrawSectorsName(CDC *pDC, BYTE ownerOfSystem, int playersRace, BOOLEAN knowOwner) const;
	
	/// Diese Funktion zeichnet die entsprechenden Schiffssymbole in den Sektor
	void DrawShipSymbolInSector(Graphics *g, int PlayersRace) const;

// statische Membervariablen
	/// Die Schrift, mit welcher die Sektornamen gezeichnet werden.
	static CFont* m_Font;
	
	/// Die Schriftfarbe, mit der die Sektornamen gezeichnet werden.
	static COLORREF m_TextColor;
	
	/// Statische Variable, die alle möglichen Namen für die Sektoren beinhaltet
	static CGenSectorName* m_NameGenerator;
private:
	/// Die Koordinate des Sektors auf der Map
	CPoint m_KO;

	/// Der Name des Sectors
	CString m_strSectorName;

	/// Diese Variable beinhaltet alle Sektorattribute.
	int m_Attributes;		
	
	/// Wem gehört der Sektor?
	BYTE m_byOwnerOfSector;
	
	/// Wem gehört/gehörte der Sektor zuerst? Also eigene Kolonie und Heimatsystem
	BYTE m_byColonyOwner;
	
	/// Variable speichert den Status über diesen Sektor, wobei 0 -> nichts, 1 -> gescannt,
	/// 2 -> Name bekannt, 3 -> alles inkl. Planeten bekannt, bedeutet
	BYTE m_byStatus[7];
	
	/// Gibts in diesem Sektor eine online Werft (bzw. auch Station)
	BOOLEAN m_bShipPort[7];
	
	/// Besitzt die jeweilige Rasse in dem Sektor einen Außenposten?
	BOOLEAN m_bOutpost[7];
	
	/// Besitzt die jeweilige Rasse in dem Sektor eine Sternbasis?
	BOOLEAN m_bStarbase[7];
	
	/// Hat eine Majorrace ein Schiff in diesem Sektor?
	BOOLEAN m_bWhoIsOwnerOfShip[7];

	/// Baut eine bestimmte Majorrasse gerade eine Station in dem Sektor?
	BOOLEAN m_bIsStationBuild[7];
	
	/// Scanstärke der jeweiligen Majorrace in dem Sektor
	short m_iScanPower[7];
	
	/// benötigte Scanstärke der Majorrace, um ihr Schiff in diesem Sektor erkennen zu können
	short m_iNeededScanPower[7];
	
	/// Die aktuell benötigten Stationsbaupunkte bis zur Fertigstellung
	short m_iNeededStationPoints[7];
	
	/// Die anfänglich benötigten Stationsbaupunkte (benötigt zur prozentualen Anzeige)
	short m_iStartStationPoints[7];

	/// Die Farbe der Sonne in diesem Sektor
	BYTE m_bySunColor;

	/// Punktem welche angeben, wer den größten Einfluss auf diesen Sektor hat. Wer die meisten Punkte hat und kein
	/// anderer in diesem Sektor einen Außenposten oder eine Kolonie besitzt, dem gehört der Sektor
	BYTE m_byOwnerPoints[7];

	/// Die Feld mit den einzelnen Planeten in dem Sektor
	CArray<CPlanet> m_Planets;

	/// Diese Variable wird hochgezählt, wenn ein Schiffspfad durch diesen Sektor geht. Muss nicht serialisiert werden.
	short m_iShipPathPoints;	
};