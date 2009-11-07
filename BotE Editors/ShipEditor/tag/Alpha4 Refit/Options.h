#if !defined _OPTIONS_H__20050315_1300_
#define _OPTIONS_H__20050315_1300_


// --- Feste Größen der Starmap ------------------------------------------------
#define STARMAP_SECTORS_HCOUNT		30		///< Anzahl der Sektoren horizontal
#define STARMAP_SECTORS_VCOUNT		20		///< Anzahl der Sektoren vertikal
#define STARMAP_SECTOR_WIDTH		40		///< Breite eines Sektors in Pixel bei 100% Zoom
#define STARMAP_SECTOR_HEIGHT		40		///< Höhe eines Sektors
#define STARMAP_TOTALWIDTH			1200	///< Gesamtbreite der View in Pixel bei 100% (= 40 * 30)
#define STARMAP_TOTALHEIGHT			800		///< Gesamthöhe bei 100% (= 20 * 40)
#define STARMAP_ZOOM_MIN			1.		///< minimaler Zoom-Faktor
#define STARMAP_ZOOM_MAX			3.		///< maximaler Zoom-Faktor
#define STARMAP_ZOOM_STEP			.2		///< Änderung des Zoom-Faktors beim Drehen des Scrollrades
#define STARMAP_ZOOM_INITIAL		1.		///< Zoom-Faktor zu Beginn

// ------------- Spielerrassen -------------------------------------------------
#define NOBODY				0
#define HUMAN				1
#define FERENGI				2
#define KLINGON				3
#define ROMULAN				4
#define CARDASSIAN			5
#define DOMINION			6
#define UNKNOWN				7

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
#define NOT_REACTED			2
#define DECLINED			3

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
#define REQUEST              11

// ------------- Die verschiedenen Arten einer MinorRace -----------------------
#define NOTHING_SPECIAL		0
#define FINANCIAL			1
#define WARLIKE				2
#define FARMER				3
#define INDUSTRIAL			4
#define SECRET				5
#define RESEARCHER			6
#define PRODUCER			7
#define PACIFIST			8
#define SNEAKY              9 // Hinterhältig

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
#define M					0
#define O					1
#define L					2
#define P					3
#define H					4
#define Q					5
#define K					6
#define G					7
#define R					8
#define F					9
#define C					10
#define N					11
#define A					12
#define B					13
#define E					14
#define Y					15
#define I					16
#define J					17
#define S					18
#define T					19

// ------------- Forschung -----------------------------------------------------
#define NOTRESEARCHED		0
#define RESEARCHED			1
#define RESEARCHING 		2

#define NoUC				10	// Number of Unique Complexes
#define NoTL				10	// Number of Tech Levels, aktuell 10

// ------------- Schiffe -------------------------------------------------------
// Schiffstypen
#define TRANSPORTER			0
#define KOLOSHIP            1
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
// Schiffsreichweiten
#define REICHWEITE_KURZ     0
#define REICHWEITE_MITTEL   1
#define REICHWEITE_LANG     2
// Schiffsbefehle
#define AVOID               0
#define ATTACK              1
#define CLOAK               2
#define ATTACK_SYSTEM       3
#define RAID_SYSTEM         4
#define DESTROY_SHIP        5
#define COLONIZE            6
#define TERRAFORM           7
#define BUILD_OUTPOST       8
#define BUILD_STARBASE		9
#define ASSIGN_FLAGSHIP     10
#define CREATE_FLEET        11
#define TRANSPORT			12
#define FOLLOW_SHIP			13
#define TRAIN_SHIP			14
// Sonstiges
#define DIFFERENT_TORPEDOS	29
#define TORPEDOSPEED		15
#define MAX_TORPEDO_RANGE	200
#define DAMAGE_TO_HULL		0.1	// Prozentwert der bei Treffer immer auf Hülle geht
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
#define MAJOR               0  // Handelt sich um eine MajorRace                                                 
#define MINOR               1  // Handelt sich um eine MinorRace
#define PLAYER				1
#define COMPUTER			2

// ------------- HANDEL ----------------------------------------------------
#define TRADEROUTEHAB		20	// aller wieviel Bevölkerung gibt es eine Handelsroute
#define NOTRIL				5	// Number of Trade Routes in List

// ------------- Ressourcen ----------------------------------------------------
#define TITAN               0
#define DEUTERIUM           1
#define DURANIUM            2
#define CRYSTAL             3
#define IRIDIUM             4
#define DILITHIUM           5

// ------------- Netzwerk ------------------------------------------------------
#define HANDSHAKE_ID		"ST-BotE\n1.0\n"

// --- Sonstiges ---------------------------------------------------------------
#define PT_IN_RECT(pt, x1, y1, x2, y2) \
	((x1) <= (pt).x && (pt).x < (x2) && (y1) <= (pt).y && (pt).y < (y2))

#define ALPHA_NEWS			"Star Trek - Birth of the Empires V0.41c (Alpha_Version)\n\nEs handelt sich bei dieser Version um eine Testversion. Sie spiegelt den\naktuellen Entwicklungsstand wider. Einige Funktionen sind in dieser\nVersion deaktiviert. Bitte meldet alle gefundenen Fehler im Forum.\nBugs, die ich nicht kenne, kann ich auch nicht beheben! Habt ihr\nVerbesserungsvorschläge, würden wir diese auch gern erfahren wollen.\n\nhttp://www.stbote.de.vu"
// -----------------------------------------------------------------------------

#endif // _OPTIONS_H__20050315_1300_
