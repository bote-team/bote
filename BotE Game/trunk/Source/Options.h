/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

//#pragma warning(disable: 4204)

#include "General\ResourceManager.h"
#include "General\mytrace.h"

/// Versionsnummer des Dokuments; es können nur Spielstände geladen werden, die diese Versionsnummer
/// haben.
#define DOCUMENT_VERSION			8		///< Alpha 5.1 Version
#define VERSION						0.72	///< benötigte Version in Datafiles
#define VERSION_INFORMATION			"0.72"	///< anzuzeigende Versionsinformation 

// --- Feste Größen der Starmap ------------------------------------------------
#define STARMAP_SECTORS_HCOUNT		30		///< Anzahl der Sektoren horizontal
#define STARMAP_SECTORS_VCOUNT		20		///< Anzahl der Sektoren vertikal

#define STARMAP_SECTOR_WIDTH		80		///< Breite eines Sektors in Pixel bei 100% Zoom
#define STARMAP_SECTOR_HEIGHT		80		///< Höhe eines Sektors
#define STARMAP_TOTALWIDTH			2400	///< Gesamtbreite der View in Pixel bei 100% (= 40 * 30)
#define STARMAP_TOTALHEIGHT			1600	///< Gesamthöhe bei 100% (= 20 * 40)

#define STARMAP_ZOOM_MIN			.6		///< minimaler Zoom-Faktor
#define STARMAP_ZOOM_MAX			1.5		///< maximaler Zoom-Faktor
#define STARMAP_ZOOM_STEP			.05		///< Änderung des Zoom-Faktors beim Drehen des Scrollrades
#define STARMAP_ZOOM_INITIAL		1.0		///< Zoom-Faktor zu Beginn


// ------------- View-IDs ------------------------------------------------------
// Mainview
#define GALAXY_VIEW				1
#define SYSTEM_VIEW				2
#define RESEARCH_VIEW			3
#define INTEL_VIEW				4
#define DIPLOMACY_VIEW			5
#define TRADE_VIEW				6
#define EMPIRE_VIEW				7
#define FLEET_VIEW				8
#define SHIPDESIGN_VIEW			9
#define TRANSPORT_VIEW			10
#define EVENT_VIEW				11

#define IS_MAIN_VIEW(id) \
	((id) >= GALAXY_VIEW && (id) <= EVENT_VIEW)

// Planetenview (unten)
#define PLANET_BOTTOM_VIEW		101
#define SHIP_BOTTOM_VIEW		102
#define RESEARCH_BOTTOM_VIEW	103
#define INTEL_BOTTOM_VIEW		104
#define DIPLOMACY_BOTTOM_VIEW	105
#define TRADE_BOTTOM_VIEW		106
#define SHIPDESIGN_BOTTOM_VIEW	109

#define IS_BOTTOM_VIEW(id) \
	((id) >= PLANET_BOTTOM_VIEW && (id) <= SHIPDESIGN_BOTTOM_VIEW)

// ------------- Spielerrassen -------------------------------------------------
#define NOBODY				0
#define HUMAN				1
#define FERENGI				2
#define KLINGON				3
#define ROMULAN				4
#define CARDASSIAN			5
#define DOMINION			6
#define UNKNOWN				7
#define MINORNUMBER			MAXBYTE


// ------------- Gebäude------ -------------------------------------------------
#define NOEIBL				12	// Number of Entries in Buildlist you can see
#define NOBIOL				12	// Number of Buildings in Overview List
#define NOBIEL				9	// Number of Buildings in Energy List
#define ALE					8	// AssemblyListEntries

// ------------- Message-Types -------------------------------------------------
#define NO_TYPE				0
#define ECONOMY				1
#define RESEARCH			2
#define SECURITY			3
#define DIPLOMACY			4
#define MILITARY			5
#define SOMETHING			6

// ------------- Reaktion auf Nachrichten --------------------------------------
#define DECLINED			0
#define ACCEPTED			1
#define NOT_REACTED			2

// ------------- Diplomatische Vereinbarungen-----------------------------------
// Die verschiedenen Arten, welchen Status eine Minorrace gegenüber ein Majorrace haben kann
#define WAR_PACT            -5 // bei Annahme des Partners gemeinsame Kriegserklärung
#define DEFENCE_PACT        -2 // zusätzlich anbietbar, außer bei Bündnis oder Krieg
#define WAR					-1 // Kriegserklärung
#define NO_AGREEMENT		 0
#define NON_AGGRESSION_PACT	 1 
#define TRADE_AGREEMENT		 2 // Handel mgl. & Rohstoffe als Zugabe möglich
#define FRIENDSHIP_AGREEMENT 3 // sehen dipl. Verträge mit anderen Rassen
#define COOPERATION			 4 // gemeinsame Nutzung der Werften und Basen
#define AFFILIATION			 5 // Zusammenarbeit auf dipl. Ebene
#define MEMBERSHIP			 6
#define PRESENT				 10
#define CORRUPTION			 11
#define DIP_REQUEST          12
#define DIPLOMACY_PRESENT_VALUE	200	// Ein konstanter Wert, der mindst. erreicht werden muss um Beziehung zu verbessern

// ------------- Die verschiedenen Arten einer MinorRace -----------------------
#define NOTHING_SPECIAL		0
#define FINANCIAL			1
#define WARLIKE				2
#define AGRARIAN			3	
#define INDUSTRIAL			4
#define SECRET				5
#define SCIENTIFIC			6	
#define PRODUCER			7
#define PACIFIST			8
#define SNEAKY              9	// Hinterhältig
#define SOLOING				10	// Alleingeher
#define	HOSTILE				11

// ------------- Planeteneigenschaften -----------------------------------------
// Zonen
#define HOT					0
#define TEMPERATE			1
#define COOL				2
// Planetengröße
#define SMALL				0
#define NORMAL				1
#define BIG					2
#define GIANT				3
// Planetentypen
#define PLANETCLASS_M		0
#define PLANETCLASS_O		1
#define PLANETCLASS_L		2
#define PLANETCLASS_P		3
#define PLANETCLASS_H		4
#define PLANETCLASS_Q		5
#define PLANETCLASS_K		6
#define PLANETCLASS_G		7
#define PLANETCLASS_R		8
#define PLANETCLASS_F		9
#define PLANETCLASS_C		10
#define PLANETCLASS_N		11
#define PLANETCLASS_A		12
#define PLANETCLASS_B		13
#define PLANETCLASS_E		14
#define PLANETCLASS_Y		15
#define PLANETCLASS_I		16
#define PLANETCLASS_J		17
#define PLANETCLASS_S		18
#define PLANETCLASS_T		19
#define GRAPHICNUMBER		31 // Anzahl der verschiedenen Grafiken der Planeten

// ------------- Forschung -----------------------------------------------------
#define NOTRESEARCHED		0
#define RESEARCHED			1
#define RESEARCHING 		2

#define NoUC					12		// Number of Unique Complexes	( nur noch nicht Finanzen implementiert!!!! -> 13)
#define SPECIAL_RESEARCH_DIV	1.67	// Wert, durch den die benötigten Punkte bei der Spezialforschung geteilt werden (umso höher, umso weniger FP sind nötig)
#define TECHPRODBONUS			2.0		// Techbonus auf die Produktion (z.B. 2% pro Stufe auf Energie, Nahrung und Industrie)

// ------------- Schiffe -------------------------------------------------------
// Schiffstypen
#define TRANSPORTER			0
#define COLONYSHIP          1
#define PROBE				2
#define SCOUT				3
#define FIGHTER				4	// Jäger
#define FRIGATE				5
#define DESTROYER			6
#define CRUISER				7
#define HEAVY_DESTROYER     8
#define HEAVY_CRUISER       9
#define BATTLESHIP			10
#define FLAGSHIP			11
#define OUTPOST				12
#define STARBASE            13
#define ALIEN				14

#define IS_NONCOMBATSHIP(type) \
	((type) == TRANSPORTER || (type) == COLONYSHIP || (type) == PROBE)

// Schiffsreichweiten
#define RANGE_SHORT		    0
#define RANGE_MIDDLE	    1
#define RANGE_LONG		    2
// Schiffsbefehle
#define AVOID               0
#define ATTACK              1
#define CLOAK               2
#define ATTACK_SYSTEM       3
#define RAID_SYSTEM         4
#define BLOCKADE_SYSTEM		5
#define DESTROY_SHIP        6
#define COLONIZE            7
#define TERRAFORM           8
#define BUILD_OUTPOST       9
#define BUILD_STARBASE		10
#define ASSIGN_FLAGSHIP     11
#define CREATE_FLEET        12
#define TRANSPORT			13
#define FOLLOW_SHIP			14
#define TRAIN_SHIP			15
// Sonstiges
#define DIFFERENT_TORPEDOS	29
#define TORPEDOSPEED		15
#define MAX_TORPEDO_RANGE	200
#define DAMAGE_TO_HULL		0.1	// Prozentwert der bei Treffer immer auf Hülle geht
#define POPSUPPORT_MULTI	3.0	// Multiplikator für Schiffsunterstützungskosten aus Bevölkerung im System
// Schiffseigenschaften
enum {NONE, ASSULTSHIP, BLOCKADESHIP, COMMANDSHIP, COMBATTRACTORBEAM, DOGFIGHTER, DOGKILLER, PATROLSHIP, RAIDER, SCIENCEVESSEL};

// ------------- Arbeiter ------------------------------------------------------
#define FOOD_WORKER			0
#define INDUSTRY_WORKER		1
#define ENERGY_WORKER		2
#define SECURITY_WORKER		3
#define RESEARCH_WORKER		4
#define TITAN_WORKER		5
#define DEUTERIUM_WORKER	6
#define DURANIUM_WORKER		7
#define CRYSTAL_WORKER		8
#define IRIDIUM_WORKER		9

// ------------- System   ------------------------------------------------------
#define	MAJOR				0	// Hauptrasse
#define	MINOR				1	// kleine Rasse (keine Ausbreitung)

// ------------- HANDEL ----------------------------------------------------
#define TRADEROUTEHAB		20	// aller wieviel Bevölkerung gibt es eine Handelsroute
#define NOTRIL				12	// Number of Trade Routes in List

// ------------- Ressourcen ----------------------------------------------------
#define TITAN               0
#define DEUTERIUM           1
#define DURANIUM            2
#define CRYSTAL             3
#define IRIDIUM             4
#define DILITHIUM           5

#define MAX_FOOD_STORE		25000
#define MAX_RES_STORE		125000
#define MAX_DERITIUM_STORE	100

// --- Sonstiges ---------------------------------------------------------------
#define PT_IN_RECT(pt, x1, y1, x2, y2) \
	((x1) <= (pt).x && (pt).x < (x2) && (y1) <= (pt).y && (pt).y < (y2))

inline void SetAttributes(BOOLEAN is, int attribute, int &variable)
{
	if (is)
		variable |= attribute;
	else
		variable &= ~attribute;
}


// --- Künstliche Intelligenz ---------------------------------------------------
#define MINBASEPOINTS 120	///< minimale Punkte für den Außenpostenbau, damit die KI überhaupt beginnt dort einen zu bauen

// Tracebedingungen -> können hier ein und ausgeschaltet werden um bestimmte Traceausgaben zu bekommen
#define TRACE_AI
#define TRACE_GRAPHICLOAD
#define TRACE_SHIPAI
#define TRACE_DIPLOMATY
#define TRACE_INTEL
#define TRACE_INTELAI
//#define DEBUG_AI_BASE_DEMO
