/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Starmap.h: interface for the CStarmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STARMAP_H__28F3FFEA_139A_45A5_AFF3_76F41E48496B__INCLUDED_)
#define AFX_STARMAP_H__28F3FFEA_139A_45A5_AFF3_76F41E48496B__INCLUDED_

// nur zu Demozwecken, sollte auskommentiert werden!
// #define DEBUG_AI_BASE_DEMO

#include "Constants.h"
#include "SortedList.h"
#include <list>
#include <set>
#include <map>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// die verschiedenen Reichweiten; je größer der Wert, desto näher ist ein Außenposten
#define SM_RANGE_SPACE		0		///< außerhalb aller Reichweiten
#define SM_RANGE_FAR		1		///< große Entfernung
#define SM_RANGE_MIDDLE		2		///< mittlere Entfernung
#define SM_RANGE_NEAR		3		///< kurze Entfernung


/// Struktur, die die Koordinaten eines Sektors aufnehmen kann
struct Sector
{
	Sector() : x(-1), y(-1) {}
	Sector(char _x, char _y) : x(_x), y(_y) {}
	Sector(const Sector &other) : x(other.x), y(other.y) {}
	Sector(const CPoint& p) : x((char)p.x), y((char)p.y) {}
	int operator==(const Sector &other) const {return (other.x == x && other.y == y);}
	Sector& operator=(const Sector &other) {x = other.x; y = other.y; return *this;}
	Sector operator+(const Sector &other) {return Sector(x + other.x, y + other.y);}
	int operator!=(const Sector &other) const {return (other.x != x || other.y != y);}

	/// Funktion, welche prüft, ob dies ein gültiger Sektor ist
	bool on_map() const {
		return 0 <= x && x < STARMAP_SECTORS_HCOUNT && 0 <= y && y < STARMAP_SECTORS_VCOUNT;
	}

	char x, y;
};

/// Struktur, die Informationen eines Sektors für die Berechnung eines kürzesten Weges aufnimmt
struct PathSector
{
	bool used;			///< <code>true</code>, wenn der Knoten bereits gewählt wurde, sonst <code>false</code>
	double distance;	///< Entfernung vom Start
	int hops;			///< Anzahl der Hops vom Start-Sektor
	Sector parent;		///< Verweis auf Elternknoten

	Sector position;	///< Koordinaten des Sektors
};

/// Komparator für PathSector-Objekte (vergleicht die distance-Werte)
class PathSectorComparator
{
public:
	/// liefert -1 wenn a &lt; b, 0 wenn a == b und 1 wenn a &gt; b
	int operator()(const PathSector *a, const PathSector *b) const
	{
		if (a->distance < b->distance)
			return -1;
		else if (a->distance == b->distance)
			return 0;
		else
			return 1;
	}
};

/// Liste, die Nachbar-Sektoren mit Abständen aufnehmen kann (für CalcPath)
typedef CSortedList<PathSector, PathSectorComparator> LeafList;

/// Doppelt verkettete List mit Sector-Einträgen
typedef std::list<Sector> SECTORLIST;

/// Nimmt Informationen über Sektoren und deren Bewertung auf.
struct BaseSector
{
	Sector position;		///< Koordinaten des Sektors
	short points;			///< Bewertung des Sektors

	BaseSector() : position(Sector()), points(-1) {}

	bool operator<(const BaseSector &other) const
	{
		// wollen absteigend sortieren
		return points > other.points;
	}

};

typedef struct RangeMap
{
	unsigned char *range;		///< Array, das die Reichweitenmatrix zeilenweise enthält
	char w;						///< Anzahl der Spalten der Matrix
	char h;						///< Anzahl Zeilen der Matrix

	/// nullbasierter Index einer Spalte; die Matrix wird so ausgerichtet, dass diese Spalte über dem Sektor eines Außenpostens steht
	char x0;
	/// nullbasierter Index einer Zeile; die Matrix wird so ausgerichtet, dass diese Zeile über dem Sektor eines Außenpostens steht
	char y0;
}
RangeMap;

/**
 * Klasse, die für einen Spieler sämtliche Infos der Starmap enthält. Enthält Methoden zur Berechnung von Reichweiten,
 * Flugrouten (kürzester Weg) und Sektoren zum Außenpostenbau durch die KI.
 *
 * @author CBot
 * @version 0.0.2
 */
class CSector;
class CStarmap
{
	friend class CGalaxyMenuView;

protected:
	/// Map, die die Entfernung vom nächsten Außenposten aufnimmt; der erste Index ist die x-,
	/// der zweite die y-Koordinate eines Sektors (modelliert Einfluss, den der Außenposten hat)
	std::vector<std::vector<unsigned char>> m_Range;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];
	/// Koordinaten des aktuell ausgewählten Sektors; (-1, -1) wenn keiner ausgewählt ist
	// (orange rectangle visible in galaxy view)
	Sector m_Selection;

	/// Liste der Sektoren, in denen sich ein Außenposten befindet
	SECTORLIST m_lBases;

	/// zu verwendende lokale RangeMap, für nachfolgend hinzugefügte Außenposten
	RangeMap m_RangeMap;

	/// bestimmte Sektoren sind gefährlich und sollten daher nicht primär angeflogen werden
	static std::vector<double> m_BadMapModifiers;

	/// Array, das Informationen zur Berechnung der kürzesten Wege aufnimmt
	std::vector<std::vector<PathSector>> pathMap;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];
	/// Liste der Nachbarn, die von den aktuellen Blättern aus erreichbar sind
	/// (werden als nächstes zu Blättern des Baumes)
	LeafList leaves;

	Sector pathStart;				///< Start-Sektor beim letzten Aufruf von CalcPath(), {-1, -1} wenn nicht gesetzt
	unsigned char pathRange;		///< Reichweite beim letzten Aufruf von CalcPath(), 0 wenn nicht gesetzt


	BOOL m_bAICalculation;			///< zusätzliche Berechnungen für den automatischen Außenpostenbau aktiviert?
	char m_nAIRange;				///< maximale Reichweite, innerhalb derer Außenposten gebaut werden sollen und die zusammenhängen soll
	SECTORLIST m_lAITargets;		///< Ziele, zu denen sich bevorzugt ausgebreitet werden soll
	SECTORLIST m_lAIKnownSystems;	///< Liste von der KI bekannten Systemen (durch Scannen)

	/// Gebietszuwachs für Sektor (x, y), wenn in diesem Sektor ein Außenposten gebaut würde
	std::vector<std::vector<short>> m_AIRangePoints;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

	/// für Sektoren außerhalb der gegebenen Reichweite die Anzahl der Nachbarn innerhalb der Reichweite
	std::vector<std::vector<unsigned char>> m_AINeighbourCount;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];
	/// Bewertung für den Zusammenhang eines Gebiets, um Lücken zu vermeiden und nicht zusammenhängende Gebiete zu verbinden
	std::vector<std::vector<short>> m_AIConnectionPoints;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

	/// Bewertung für bevorzugte Ausbreitungsrichtungen
	std::vector<std::vector<short>> m_AITargetPoints;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

	/// negative Bewertung für ein Gebiet durch feindliche Grenzen. Man sollte keinen Aussenposten auf einen freien
	/// Fleck inmitten des gegnerischen Gebiets bauen.
	std::vector<std::vector<short>> m_AIBadPoints;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];

public:
	/**
	 * @param bAICalculation aktiviert zusätzliche Berechnungen, die den automatischen Außenpostenbau ermöglichen
	 * @param nAIRange maximale Reichweite, innerhalb derer Außenposten gebaut werden sollen und die zusammenhängen soll
	 */
	CStarmap(BOOL bAICalculation = FALSE, char nAIRange = SM_RANGE_NEAR);
	virtual ~CStarmap();

	/**
	 * Funktion gibt den Wert aus der RangeMap <code>m_Range</code> an einer bestimmten Stelle <code>p</p> auf der
	 * Karte zurück.
	 */
	unsigned char GetRange(const CPoint &p) const {return (3 - m_Range[p.x][p.y]);}

	/**
	 * Ermittelt die Koordinaten eines angeklickten Sektors. <code>pt</code> gibt die ungezoomten Mauskoordinaten
	 * bezüglich der linken oberen Ecke der gesamten Starmap an.
	 */
	static Sector GetClickedSector(const CPoint& pt);
	/**
	 * Liefert die Koordinaten der linken oberen Ecke des angegebenen Sektors.
	 */
	static CPoint GetSectorCoords(const Sector& sector);

	/**
	 * Wählt den angegebenen Sektor aus. Bei Angabe von (-1, -1) hat diese Methode keine Wirkung. Zum Entfernen der
	 * Markierung muss Deselect() verwendet werden.
	 */
	void Select(const Sector &sector);
	/**
	 * Entfernt die Markierung.
	 */
	void Deselect();
	/**
	 * Liefert die Koordinaten des ausgewählten Sektors. Liefert {-1, -1}, wenn kein Sektor ausgewählt ist.
	 */
	Sector GetSelection() const {return m_Selection;}

	/**
	 * Setzt die für nachfolgend hinzugefügte Außenposten zu verwendende RangeMap.
	 */
	void SetRangeMap(unsigned char rangeMap[], char w, char h, char x0, char y0);

	/**
	 * Fügt im angegebenen Sektor einen Außenposten hinzu.
	 */
	void AddBase(const Sector& sector, BYTE propTech);

	/**
	 * @return <code>TRUE</code>, wenn im angegebenen Sektor ein Außenposten hinzugefügt wurde, sonst <code>FALSE</code>.
	 */
	BOOL IsBase(const Sector &sector);

	/**
	 * Schaltet die komplette Map frei, so dass überall mit kurzer Reichweite hingeflogen werden kann
	 */
	void SetFullRangeMap(int nRange = SM_RANGE_NEAR, const std::vector<Sector>& vExceptions = std::vector<Sector>());

	/**
	 * Gleicht die komplette Rangemap mit der Galaxiemap ab. Gebiete, welche einer Rasse gehören mit der wir einen
	 * Nichtangriffspakt haben, werden von der Rangemap entfernt. Übergeben werden dafür ein Zeiger auf alle
	 * Sektoren <code>sectors</code> und ein Wahrheitswert <code>races</code> für alle Rassen, ob wir einen
	 * Nichtangriffspakt mit dieser Rasse haben.
	 */
	void SynchronizeWithMap(const std::vector<CSector>& sectors, const std::set<CString>* races);

	/**
	 * Führt für gefährliche Anomalien mathematische Gewichte hinzu, so dass dieser Sektor bei der automatischen
	 * Wegsuche nicht überflogen wird.
	 */
	static void SynchronizeWithAnomalies(const std::vector<CSector>& sectors);

	/**
	 * Löscht alle Basen und setzt die Einträge in der Rangemap wieder auf Ausgangswert
	 */
	void ClearAll();

	/**
	 * Berechnet den kürzesten Weg von position nach target innerhalb der durch range angegebenen Reichweite. Liefert den
	 * kompletten Weg in path zurück. Rückgabewert ist der entsprechend speed als nächstes zu erreichende Punkt auf
	 * dem berechneten Weg. Tritt ein Fehler auf (z.B. wenn das Ziel nicht erreichbar ist), liefert die Funktion ein
	 * Sector-Objekt, dessen Koordinaten auf -1 gesetzt sind. Die in path bereits vorhandenen Einträge werden gelöscht,
	 * die Größe des Arrays wird auf die Länge des gelieferten Weges gesetzt.
	 */
	Sector CalcPath(const Sector &position, const Sector &target, unsigned char range,
		unsigned char speed, CArray<Sector> &path);

	/**
	 * Fügt der Liste der bevorzugten Ausbreitungsrichtungen ein weiteres Ziel hinzu. Dient der Ermittlung eines Sektors,
	 * in dem ein Außenposten gebaut werden sollte, um sich dem Ziel zu nähern; auch bei weiter entfernten Zielen.
	 */
	void AddTarget(const Sector &target);

	/**
	 * @return <code>TRUE</code>, wenn der angegebene Sektor als Ziel hinzugefügt wurde, sonst <code>FALSE</code>.
	 */
	BOOL IsTarget(const Sector &sector);

	/**
	 * Fügt der Liste der der KI bekannten Systeme ein weiteres System hinzu. Es wird bevorzugt versucht, diese Systeme
	 * in die gegebene Reichweite aufzunehmen, wenn sie dem bereits erschlossenen Gebiet nahe sind (etwas weniger aggresiv
	 * als die Ausbreitung zu einem Ziel).
	 */
	void AddKnownSystem(const Sector &sector);

	/// @return <code>TRUE</code>, wenn sich im angegebenen Sektor ein bekanntes System befindet, sonst <code>FALSE</code>.
	BOOL IsKnownSystem(const Sector &sector);

	/**
	 * Funktion erstellt anhand von Sektoren eine Matrix mit Werten, welche zur Berechnung der besten Position für
	 * einen Außenposten genutzt werden soll. Übergeben werden dafür ein Zeiger auf alle
	 * Sektoren <code>sectors</code> und die Rasse, zu welcher das Starmap Objekt gehört <code>race</code>.
	 */
	void SetBadAIBaseSectors(const std::vector<CSector>& sectors, const CString& race);

	/**
	 * Ermittelt einen Sektor, in dem günstig ein Außenposten gebaut werden könnte. Liefert <code>Sector(-1, -1)</code> im
	 * Feld <code>position</code> und <code>-1</code> für <code>points</code>, wenn nirgendwo ein Außenposten gebaut werden
	 * kann oder dieses Objekt mit <code>bAICalculation == FALSE</code> erzeugt wurde.
	 *
	 * @param variance maximal mögliche Abweichung gegenüber der besten Bewertung; aus [0, 1], 0: keine Abweichung,
	 * 1: 100% Abweichung möglich
	 */
	BaseSector CalcAIBaseSector(double variance = 0.1);

	/**
	 * @return Gesamtbewertung des angegebenen Sektors. Zuvor müssen die Funktionen zur Neuberechnung der Bewertung aufgerufen worden sein.
	 */
	short GetPoints(const Sector &sector) const;

private:
	/// Diese Funktion berechnet die Reichweitenkarte anhand der aktuellen Techstufe <code>propTech</code> und schreibt
	/// das Ergebnis in den Parameter <code>rangeMap</code>. Zusätzlich werden Referenzen auf paar Hilfsvariablen
	/// übergeben.
	void CalcRangeMap(BYTE propTech);

	/// @return Eintrag der gesetzten RangeMap an der Stelle (x, y), wobei (0, 0) die Außenpostenposition ist
	unsigned char GetRangeMapValue(char x, char y);

	/// Berechnet die Gebietszuwächse neu.
	void RecalcRangePoints();

	/// Berechnet die Bewertungen zum Verbinden benachbarter Gebiete und Verhindern von Lücken.
	void RecalcConnectionPoints();

	/// Berechnet Bewertungen für bevorzugte Ausbreitungsrichtungen.
	void RecalcTargetPoints();

	//(Re)Initializes some of the vector members
	void InitSomeMembers();
};

#endif // !defined(AFX_STARMAP_H__28F3FFEA_139A_45A5_AFF3_76F41E48496B__INCLUDED_)
