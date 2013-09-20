/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Planet.h: Schnittstelle für die Klasse CPlanet.
//
//////////////////////////////////////////////////////////////////////

// ******************** Planetenklassen *********************
// A ... Geo-Thermal (junge, flüßiges Gestein)  - kalte Zone			Abkühlung -> C
// B ... Geo-Morteus (sehr heiß, dünne Atmo.)	- heiße Zone
// C ... Geo-Inactive (kalt, teilweise gefroren)- kalte Zone
// D ... Asteroiden								- überall
// E ... Geo-Plastic (heiß, flüßiges Gestein)	- normale Zone			Abkühlung -> F
// F ... Geo-Matallic (warm, dünne Oberfläche)	- normale Zone			Abkühlung -> G
// G ... Geo-Crystaline (warm, junge Erde)		- normale Zone			Abkühlung -> K,L,M,N,O,P
// H ... Desert									- heiße Zone
// I ... Gas Überriese							- kalte Zone
// J ... Gas Riese								- kalte Zone
// K ... Adaptable (warm, Wasser vorhanden)		- normale Zone
// L ... Dschungel								- normale Zone
// M ... Terrestrial (erdähnlich)				- normale Zone
// N ... Reducing (heiß, Wasserdampf, CO2)		- heiße Zone
// O ... Pelagic (wasserreich)					- normale Zone
// P ... Eis									- kalte Zone
// Q ... keine Eigenrotation					- normale Zone
// R ... interstellarer Wanderer				- überall
// S ... Gas Ultrariesen						- kalte Zone
// T ... Gas Überriesen (größer als I)			- kalte Zone
// Y ... Dämon (heiß, toxisch, strahlt)			- heiße Zone

// M ist am besten, am schlechtesten zu terraformen ist N, von A bis Y gar nicht!
// unbewohnbar sind : A,B,E,I,J,S,T,Y			= 8
// bewohnbar sind:    C,F,G,H,K,L,M,N,O,P,Q,R	= 12

#pragma once
#include "Constants.h"

// forward declaration
class CGraphicPool;

/// Klasse zur Abstraktion von Planeten in BotE
class CPlanet : public CObject
{
public:
	// Klasse serialisierbar machen
	DECLARE_SERIAL (CPlanet)

	/// Standardkonstruktor
	CPlanet(void);

	/// Standardestruktor
	virtual ~CPlanet();

	/// Kopierkonstruktor
	CPlanet(const CPlanet & rhs);

	/// Zuweisungsoperator
	CPlanet & operator=(const CPlanet &);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// zum Lesen der Membervaribalen
	PLANT_SIZE::Typ GetSize() const {return m_iSize;}
	BYTE GetType() const {return m_iType;}
	float GetMaxHabitant() const {return m_dMaxHabitant;}
	float GetCurrentHabitant() const {return m_dCurrentHabitant;}
	char GetClass() const {return m_cClass;}
	const CString& GetPlanetName() const {return m_strName;}
	float GetPlanetGrowth() const {return m_dGrowing;}
	BOOLEAN GetTerraformed() const {return m_bTerraformed;}
	BOOLEAN GetIsTerraforming() const {return m_bIsTerraforming;}
	BOOLEAN GetHabitable() const {return m_bHabitable;}
	BOOLEAN GetColonized() const {return m_bColonisized;}
	BYTE GetNeededTerraformPoints() const {return m_iNeededTerraformPoints;}
	BYTE GetStartdTerraformPoints() const {return m_iStartTerraformPoints;}
	const BOOLEAN* GetBoni() const {return m_bBoni;}
	bool HasIndividualGraphic() const { return m_bHasIndividualGraphic; }

	/// Funktion gibt einen Dateinamen für die Planetengrafik zurück. Dieser wird automatisch aus der Nummer der
	/// Grafik und der Planetenklasse generiert.
	CString GetGraphicFile() const;

	/// Gibt den endgültigen Grafikpfad, ob individuell oder mittels GetGraphicFile, zurück
	CString GetPlanetGraphicFile() const;

	// zum Schreiben der Membervariablen
	void SetSize(PLANT_SIZE::Typ Size) {m_iSize = Size;}
	void SetMaxHabitant(float MaxHabitant) {m_dMaxHabitant = MaxHabitant;}
	void SetCurrentHabitant(float CurrentHabitant) {m_dCurrentHabitant = CurrentHabitant;}
	void SetType(BYTE Type) {m_iType = Type;}
	void SetHabitable(BOOLEAN is) {m_bHabitable = is;}
	void SetTerraformed(BOOLEAN is) {m_bTerraformed = is;}
	void SetIsTerraforming(BOOLEAN is) {m_bIsTerraforming = is;}
	void SetColonisized(BOOLEAN is) {m_bColonisized = is;}
	void SetName(const CString& Name) {m_strName = Name;}
	void SetClass(char Class) {m_cClass = Class;}
	void SetPlanetGrowth() {m_dGrowing = m_dMaxHabitant/4;} // Wenn hier was geändert, auch in GeneratePlanet was ändern
	BOOLEAN SetNeededTerraformPoints(const unsigned sub);				// Subtrahiert "sub" von den Terraformpoints, bei kleiner 0 wird der Plani auf m_bTerraformed = TRUE gesetzt
	void SetBoni(BOOLEAN titan, BOOLEAN deuterium, BOOLEAN duranium, BOOLEAN crystal, BOOLEAN iridium, BOOLEAN deritium, BOOLEAN food, BOOLEAN energy);
	void SetBoni(BYTE res, BOOLEAN is) {m_bBoni[res] = is;}
	void SetHasIndividualGraphic(const bool has) { m_bHasIndividualGraphic = has; }

	/// Funktion setzt die Nummer für das Graphikfile des Planeten.
	/// @param n Nummer der Grafik
	void SetGraphicType(BYTE n) {m_iGraphicType = n;}

	/// Funktion setzt die anfänglichen Terraformpunkte für einen Planeten fest. Normalerweise geschieht das bei der
	/// Generierung des Planeten automatisch, wenn wir aber manuell Planeten anlegen wollen, dann müssen wir auch
	/// diese Funktion aufrufen.
	void SetStartTerraformPoints(BYTE startPoints = 0) {m_iStartTerraformPoints = m_iNeededTerraformPoints = startPoints;}

	// Sonstige Funktionen
	/// Funktion erzeugt einen Planeten.
	/// @param sSectorName Sektorname
	/// @param nLastZone Zone des zuletzt erzeugten Planeten (neuer Planet kann nicht in einer vorherigen Zone sein)
	/// @param byPlanetNumer Anzahl schon erzeugeter Planeten in diesem Sektor
	/// @param bMinor Minorrace im Sektor
	/// @return Zone des erzeugten Planeten
	PLANET_ZONE::Typ Create(const CString& sSectorName, PLANET_ZONE::Typ nLastZone, BYTE byPlanetNumber, BOOLEAN bMinor);

	/// Planetenwachstum durchführen
	void PlanetGrowth(void);

	/// Funktion ermittelt die vorhandenen Ressourcen auf dem Planeten
	/// @param res Feld in das geschrieben wird, welche Ressource auf dem Planeten vorhanden ist
	void GetAvailableResources(BOOLEAN res[RESOURCES::DERITIUM + 1]) const;

	/// Funktion zeichnet den Planeten in die untere Planetenansicht.
	/// @param g Referenz auf das Grafikobjekt
	/// @param rect Rechteck um den Planeten
	/// @param graphicPool Zeiger auf die Sammlung aller Grafiken
	void DrawPlanet(Graphics &g, const CRect& rect, CGraphicPool* graphicPool);

	/// Die Resetfunktion für die CPlanet Klasse, welche alle Werte wieder auf Ausgangswerte setzt.
	void Reset(void);

private:
	// private Funktionen
	/// Diese Funktion generiert einen eventuellen Bonus anhand einer speziellen Wahrscheinlichkeitstabelle.
	void GenerateBoni(void);

	// Attribute
	PLANT_SIZE::Typ m_iSize;		///< Größe das Planeten, siehe ENUM dazu
	BYTE m_iGraphicType;			///< Grafiknummer des Planeten
	bool m_bHasIndividualGraphic;	///<Besitzt der Planet eine besondere Graphik die wir versuchen sollten zu laden; typischerweise (nur ?) Majorheimatsysteme.
	BYTE m_iType;					///< Typ des Planeten
	float m_dMaxHabitant;			///< maximale Anzahl der Bewohner
	float m_dCurrentHabitant;		///< aktuelle Anzahl der Bewohner
	float m_dGrowing;				///< Das prozentuale Wachstum der Bevölkerung des Planeten, z.B. 0.2%
	BOOLEAN m_bHabitable;			///< Ist der Planet kolonisierbar?
	BOOLEAN m_bColonisized;			///< Ist der Planet bewohnt?
	BOOLEAN m_bTerraformed;			///< Wurde der Planet terraformt?
	BOOLEAN m_bIsTerraforming;		///< Wird der Planet gerade geterraformt?
	CString m_strName;				///< Name des Planeten
	unsigned m_iNeededTerraformPoints;	///< nötige Terraformpunkte um den Planeten zu terraformen
	BYTE m_iStartTerraformPoints;	///< nötigen Terraformpunkte am Anfang, brauchen wir um den prozentualen Fortschritt berechnen zu können
	char m_cClass;					///< Die Klasse nochmal als Buchstabe
	BOOLEAN m_bBoni[8];				///< Gibt es einen bestimmten Bonus auf dem Planeten, RESOURCES::TITAN, ..., IRDIUM, RESOURCES::DERITIUM, FOOD, ENERGY
};
