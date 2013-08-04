#if !defined _OPTIONS_H__20050315_1300_
#define _OPTIONS_H__20050315_1300_


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
#define NOB					17	// Nummer aller Gebäude +1, weil 0 zählt nicht mit, Felder beginnen aber bei 0
#define NOBIOL				12	// Number of Buildings in Overview List ... aktuell 12
#define ALE					8	// AssemblyListEntries -> aktuell 8

// ------------- Message-Types -------------------------------------------------
#define NO_TYPE				0
#define ECONOMY				1
#define MILITARY			2
#define RESEARCH			3
#define SECURITY			4
#define DIPLOMACY			5
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
#define HEISS				0
#define MITTEL				1
#define KALT				2
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
#define DIFFERENT_TORPEDOS	17

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

// ------------- Ressourcen ----------------------------------------------------
#define TITAN               0
#define DEUTERIUM           1
#define DURANIUM            2
#define CRYSTAL             3
#define IRIDIUM             4
#define DILITHIUM           5
#define FOOD				6


#endif // _OPTIONS_H__20050315_1300_
