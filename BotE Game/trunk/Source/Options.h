/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

//#pragma warning(disable: 4204)

#include "General\mytrace.h"

/// Versionsnummer des Dokuments; es können nur Spielstände geladen werden, die diese Versionsnummer
/// haben.
#define DOCUMENT_VERSION			9		///< Alpha 6 Version
#define VERSION						0.81	///< benötigte Version in Datafiles
#define VERSION_INFORMATION			"0.81"	///< anzuzeigende Versionsinformation

// --- Feste Größen der Starmap ------------------------------------------------
extern int STARMAP_SECTORS_HCOUNT;		///< Anzahl der Sektoren horizontal
extern int STARMAP_SECTORS_VCOUNT;		///< Anzahl der Sektoren vertikal


extern const int STARMAP_SECTOR_WIDTH;//		80		///< Breite eines Sektors in Pixel bei 100% Zoom
extern const int STARMAP_SECTOR_HEIGHT;//		80		///< Höhe eines Sektors
extern int STARMAP_TOTALWIDTH	;//		2400	///< Gesamtbreite der View in Pixel bei 100% (= 40 * 30)
extern int STARMAP_TOTALHEIGHT	;//		1600	///< Gesamthöhe bei 100% (= 20 * 40)


#define STARMAP_ZOOM_MIN			.3		///< minimaler Zoom-Faktor
extern double STARMAP_ZOOM_MAX;//			2	///< maximaler Zoom-Faktor
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
#define COMBAT_VIEW				12

#define IS_MAIN_VIEW(id) \
	((id) >= GALAXY_VIEW && (id) <= COMBAT_VIEW)

// Startmenü
#define START_VIEW				50
#define CHOOSERACE_VIEW			51
#define NEWGAME_VIEW			52

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

// Menüauswahlview
#define MENUCHOOSE_VIEW			200

// ------------- Spielerrassen -------------------------------------------------
#define NOBODY				0
#define HUMAN				1
#define FERENGI				2
#define KLINGON				3
#define ROMULAN				4
#define CARDASSIAN			5
#define DOMINION			6
#define UNKNOWNRACE			7
#define MINORNUMBER			MAXBYTE


// ------------- Gebäude------ -------------------------------------------------
#define NOEIBL				12	// Number of Entries in Buildlist you can see
#define NOBIOL				12	// Number of Buildings in Overview List
#define NOBIEL				9	// Number of Buildings in Energy List
#define ALE					8	// AssemblyListEntries

// ------------- Message-Types -------------------------------------------------
namespace MESSAGE_TYPE
{
	enum Typ
	{
		NO_TYPE		= 0,
		ECONOMY		= 1,
		RESEARCH	= 2,
		SECURITY	= 3,
		DIPLOMACY	= 4,
		MILITARY	= 5,
		SOMETHING	= 6
	};
}

// ------------- Reaktion auf Nachrichten --------------------------------------
namespace ANSWER_STATUS
{
	enum Typ
	{
		DECLINED	= 0,
		ACCEPTED	= 1,
		NOT_REACTED	= 2
	};
}

// ------------- Diplomatische Vereinbarungen-----------------------------------
// Die verschiedenen Arten, welchen Status eine Minorrace gegenüber ein Majorrace haben kann
namespace DIPLOMATIC_AGREEMENT
{
	enum Typ
	{
		WARPACT			= -5,	// bei Annahme des Partners gemeinsame Kriegserklärung
		DEFENCEPACT		= -2,	// zusätzlich anbietbar, außer bei Bündnis oder Krieg
		WAR				= -1,	// Kriegserklärung
		NONE			= 0,
		NAP				= 1,
		TRADE			= 2,	// Handel mgl. & Rohstoffe als Zugabe möglich
		FRIENDSHIP		= 3,	// sehen dipl. Verträge mit anderen Rassen
		COOPERATION		= 4,	// gemeinsame Nutzung der Werften und Basen
		AFFILIATION		= 5,	// Zusammenarbeit auf dipl. Ebene
		MEMBERSHIP		= 6,
		PRESENT			= 10,
		CORRUPTION		= 11,
		REQUEST			= 12
	};
}

#define DIPLOMACY_PRESENT_VALUE	200	// Ein konstanter Wert, der mindst. erreicht werden muss um Beziehung zu verbessern

// ------------- Die verschiedenen Arten einer Rasse -----------------------
namespace RACE_PROPERTY
{
	enum Typ
	{
		NOTHING_SPECIAL = 0,
		FINANCIAL		= 1,
		WARLIKE			= 2,
		AGRARIAN		= 3,
		INDUSTRIAL		= 4,
		SECRET			= 5,
		SCIENTIFIC		= 6,
		PRODUCER		= 7,
		PACIFIST		= 8,
		SNEAKY          = 9,	// Hinterhältig
		SOLOING			= 10,	// Alleingeher
		HOSTILE			= 11
	};
}

// ------------- Namen der Weltraummonster (Aliens) ----------------------------
#define IONISIERENDES_GASWESEN		"Ionisierendes Gaswesen"
#define GABALLIANER_SEUCHENSCHIFF	"Gaballianer"
#define BLIZZARD_PLASMAWESEN		"Blizzard-Plasmawesen"
#define MORLOCK_RAIDER				"Morlock-Raider"
#define EELEN_GUARD					"Ehlen"
#define BOSEANER					"Boseaner"
#define KAMPFSTATION				"Kampfstation"
#define KRYONITWESEN				"Kryonitwesen"
#define MIDWAY_ZEITREISENDE			"Midway-Zeitreisende"
#define ANAEROBE_MAKROBE			"Anaerobe Makrobe"

// ------------- Planeteneigenschaften -----------------------------------------
// Zonen
namespace PLANET_ZONE
{
	enum Typ
	{
		HOT			= 0,
		TEMPERATE	= 1,
		COOL		= 2
	};
}

// Planetengröße
namespace PLANT_SIZE
{
	enum Typ
	{
		SMALL	= 0,
		NORMAL	= 1,
		BIG		= 2,
		GIANT	= 3
	};
}

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
namespace RESEARCH_STATUS
{
	enum Typ
	{
		NOTRESEARCHED	= 0,
		RESEARCHED		= 1,
		RESEARCHING 	= 2
	};
}

namespace RESEARCH_COMPLEX
{
	enum Typ
	{
		NONE						= -1,
		WEAPONS_TECHNOLOGY			= 0,
		CONSTRUCTION_TECHNOLOGY		= 1,
		GENERAL_SHIP_TECHNOLOGY		= 2,
		PEACEFUL_SHIP_TECHNOLOGY	= 3,
		TROOPS						= 4,
		ECONOMY						= 5,
		PRODUCTION					= 6,
		DEVELOPMENT_AND_SECURITY	= 7,
		RESEARCH					= 8,
		SECURITY					= 9,
		STORAGE_AND_TRANSPORT		= 10,
		TRADE						= 11,
		FINANCES					= 12	// noch nicht implementiert
	};
}

#define NoUC					12		// Number of Unique Complexes	( nur noch nicht Finanzen implementiert!!!! -> dann auf 13 hochstellen)
#define SPECIAL_RESEARCH_DIV	1.67	// Wert, durch den die benötigten Punkte bei der Spezialforschung geteilt werden (umso höher, umso weniger FP sind nötig)
#define TECHPRODBONUS			2.0		// Techbonus auf die Produktion (z.B. 2% pro Stufe auf Energie, Nahrung und Industrie)

// ------------- Schiffe -------------------------------------------------------
// Schiffstypen
namespace SHIP_TYPE
{
	enum Typ
	{
		TRANSPORTER		= 0,
		COLONYSHIP		= 1,
		PROBE			= 2,
		SCOUT			= 3,
		FIGHTER			= 4,// Jäger
		FRIGATE			= 5,
		DESTROYER		= 6,
		CRUISER			= 7,
		HEAVY_DESTROYER	= 8,
		HEAVY_CRUISER	= 9,
		BATTLESHIP		= 10,
		DREADNOUGHT		= 11,
		OUTPOST			= 12,
		STARBASE		= 13,
		ALIEN			= 14
	};
}

// Schiffsgrößen
namespace SHIP_SIZE
{
	enum Typ
	{
		SMALL	= 0,
		NORMAL	= 1,
		BIG		= 2,
		HUGE	= 3
	};
}

// Schiffsreichweiten
namespace SHIP_RANGE
{
	enum Typ
	{
		SHORT	= 0,
		MIDDLE	= 1,
		LONG	= 2
	};
}
// Schiffsbefehle
namespace SHIP_ORDER
{
	enum Typ
	{
		NONE				= -1,
		AVOID               = 0,
		ATTACK              = 1,
		ENCLOAK             = 2,
		ATTACK_SYSTEM       = 3,
		RAID_SYSTEM         = 4,
		BLOCKADE_SYSTEM		= 5,
		DESTROY_SHIP        = 6,
		COLONIZE            = 7,
		TERRAFORM           = 8,
		BUILD_OUTPOST       = 9,
		BUILD_STARBASE		= 10,
		ASSIGN_FLAGSHIP     = 11,
		CREATE_FLEET        = 12,
		TRANSPORT			= 13,
		FOLLOW_SHIP			= 14,
		TRAIN_SHIP			= 15,
		WAIT_SHIP_ORDER		= 16,
		SENTRY_SHIP_ORDER	= 17,
		REPAIR				= 18,
		DECLOAK				= 19,
		CANCEL				= 20,
		IMPROVE_SHIELDS		= 21
	};
}

// Sonstiges
#define DIFFERENT_TORPEDOS	29
#define TORPEDOSPEED		15
#define MAX_TORPEDO_RANGE	200
#define DAMAGE_TO_HULL		0.1	// Prozentwert der bei Treffer immer auf Hülle geht
#define POPSUPPORT_MULTI	3.0	// Multiplikator für Schiffsunterstützungskosten aus Bevölkerung im System

// Schiffseigenschaften
namespace SHIP_SPECIAL
{
	enum Typ
	{
		NONE				= 0,
		ASSULTSHIP			= 1,
		BLOCKADESHIP		= 2,
		COMMANDSHIP			= 3,
		COMBATTRACTORBEAM	= 4,
		DOGFIGHTER			= 5,
		DOGKILLER			= 6,
		PATROLSHIP			= 7,
		RAIDER				= 8,
		SCIENCEVESSEL		= 9
	};
}

// ------------- Arbeiter ------------------------------------------------------
namespace WORKER
{
	enum Typ
	{
		NONE				= -1,
		FOOD_WORKER			= 0,
		INDUSTRY_WORKER		= 1,
		ENERGY_WORKER		= 2,
		SECURITY_WORKER		= 3,
		RESEARCH_WORKER		= 4,
		TITAN_WORKER		= 5,
		DEUTERIUM_WORKER	= 6,
		DURANIUM_WORKER		= 7,
		CRYSTAL_WORKER		= 8,
		IRIDIUM_WORKER		= 9,
		ALL_WORKER			= 10,
		FREE_WORKER			= 11
	};
}

// ------------- System   ------------------------------------------------------

// ------------- Handel ----------------------------------------------------
#define TRADEROUTEHAB		20	// aller wieviel Bevölkerung gibt es eine Handelsroute
#define NOTRIL				12	// Number of Trade Routes in List

// ------------- Ressourcen ----------------------------------------------------
#define TITAN               0
#define DEUTERIUM           1
#define DURANIUM            2
#define CRYSTAL             3
#define IRIDIUM             4
#define DERITIUM	        5

#define MAX_FOOD_STORE		25000
#define MAX_RES_STORE		125000
#define MAX_DERITIUM_STORE	100

// ------------- Schiffskampfbefehle ----------------------------------------------------
namespace COMBAT_ORDER
{
	enum Typ
	{
		NONE		= -1,
		USER		= 0,
		HAILING		= 1,
		RETREAT		= 2,
		AUTOCOMBAT	= 3
	};
}

// ------------- Schiffskampftaktiken ----------------------------------------------------
namespace COMBAT_TACTIC
{
	enum Typ
	{
		CT_ATTACK	= 0,	///< Angriff
		CT_AVOID	= 1,	///< Meiden
		CT_RETREAT	= 2		///< Rückzug
	};
}

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
//#define DEBUG_AI_BASE_DEMO

namespace MT
{
	//If you wanna change something here, please consider using the --log-domains and/or --passive-domains
	//command line parameters instead!
	const std::string DEFAULT_LOG_DOMAINS[8] =
	{
		"general",//Used in case no domain is passed.
		//Avoid "general", unless you want to make sure that also normal players
		//can get, say, an important error or warning in the BotE.log
		//without the need to change something about logging.
		"logging",//Should be used by the MYTRACE class itsself only.
		"ai",
		"graphicload",
		"shipai",
		"diplomacy",
		"intel",
		"intelai",
		//"ai_base_demo",
	};
}

// Projektspezifische Defines
//The SEE_ALL_OF_MAP define has been replaced by the --see-all command line parameter.
//#define SAVE_GALAXYIMAGE		// speichert Bild der Galaxie aus sicht der gespielten Major im Ordner Bote/Screenshots
