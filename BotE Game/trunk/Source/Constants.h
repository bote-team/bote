/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
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
static const int DOCUMENT_VERSION = 10;		///< Alpha 7 Version
static const double VERSION = 0.9;		///< benötigte Version in Datafiles
static const std::string VERSION_INFORMATION("0.9");	///< anzuzeigende Versionsinformation

// Konsistenzchecks ein/ausschalten (nicht für Release!)
#define CONSISTENCY_CHECKS					///< anschalten wenn Konsistenzchecks durchgeführt werden sollen

// --- Feste Größen der Starmap ------------------------------------------------
extern int STARMAP_SECTORS_HCOUNT;		///< Anzahl der Sektoren horizontal
extern int STARMAP_SECTORS_VCOUNT;		///< Anzahl der Sektoren vertikal


extern const int STARMAP_SECTOR_WIDTH;//		80		///< Breite eines Sektors in Pixel bei 100% Zoom
extern const int STARMAP_SECTOR_HEIGHT;//		80		///< Höhe eines Sektors
extern int STARMAP_TOTALWIDTH	;//		2400	///< Gesamtbreite der View in Pixel bei 100% (= 40 * 30)
extern int STARMAP_TOTALHEIGHT	;//		1600	///< Gesamthöhe bei 100% (= 20 * 40)


static const double STARMAP_ZOOM_MIN = 0.3;		///< minimaler Zoom-Faktor
extern double STARMAP_ZOOM_MAX;				///< maximaler Zoom-Faktor
static const double STARMAP_ZOOM_STEP = 0.05;		///< Änderung des Zoom-Faktors beim Drehen des Scrollrades
static const double STARMAP_ZOOM_INITIAL = 1.0;		///< Zoom-Faktor zu Beginn

static const double ERROR_TOLERANCE = 10E-12; //minimum tolerance for double comparison
inline bool Equals(double is, double should)
{
	return should - ERROR_TOLERANCE < is && is < should + ERROR_TOLERANCE;
}


// ------------- View-IDs ------------------------------------------------------

namespace VIEWS
{
	// Mainview
	enum MAIN_VIEWS
	{
		NULL_VIEW				= 0,
		GALAXY_VIEW				= 1,
		SYSTEM_VIEW				= 2,
		RESEARCH_VIEW			= 3,
		INTEL_VIEW				= 4,
		DIPLOMACY_VIEW			= 5,
		TRADE_VIEW				= 6,
		EMPIRE_VIEW				= 7,
		FLEET_VIEW				= 8,
		SHIPDESIGN_VIEW			= 9,
		TRANSPORT_VIEW			= 10,
		EVENT_VIEW				= 11,
		COMBAT_VIEW				= 12
	};

	inline bool is_main_view(int id)
	{
		return VIEWS::GALAXY_VIEW <= id && id <= VIEWS::COMBAT_VIEW;
	}
}

namespace VIEWS
{
	// Startmenü
	enum START_MENU_VIEWS
	{
		START_VIEW				= 50,
		CHOOSERACE_VIEW			= 51,
		NEWGAME_VIEW			= 52
	};

	// Planetenview (unten)
	enum BOTTOM_VIEWS
	{
		PLANET_BOTTOM_VIEW		= 101,
		SHIP_BOTTOM_VIEW		= 102,
		RESEARCH_BOTTOM_VIEW	= 103,
		INTEL_BOTTOM_VIEW		= 104,
		DIPLOMACY_BOTTOM_VIEW	= 105,
		TRADE_BOTTOM_VIEW		= 106,
		SHIPDESIGN_BOTTOM_VIEW	= 109
	};

	inline bool is_bottom_view(int id)
	{
		return VIEWS::PLANET_BOTTOM_VIEW <= id && id <=VIEWS::SHIPDESIGN_BOTTOM_VIEW;
	}
}

namespace VIEWS
{
	// Menüauswahlview
	enum MENU_CHOOSE_VIEWS
	{
		MENU_CHOOSE_VIEW = 200
	};
}

// ------------- Spielerrassen -------------------------------------------------
namespace PLAYER_RACES
{
	enum TYPE
	{
		NOBODY				= 0,
		MAJOR1				= 1,
		MAJOR2				= 2,
		MAJOR3				= 3,
		MAJOR4				= 4,
		MAJOR5				= 5,
		MAJOR6				= 6,
		UNKNOWNRACE			= 7,
		MINORNUMBER			= MAXBYTE
	};
}


// ------------- Gebäude------ -------------------------------------------------
static const int NOEIBL		= 12;	// Number of Entries in Buildlist you can see
static const int NOBIOL		= 12;	// Number of Buildings in Overview List
static const int NOBIEL		= 9;		// Number of Buildings in Energy List
static const int ALE		= 8;		// AssemblyListEntries

// ------------- Message-Types -------------------------------------------------
namespace EMPIRE_NEWS_TYPE
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

static const int DIPLOMACY_PRESENT_VALUE = 200;	// Ein konstanter Wert, der mindst. erreicht werden muss um Beziehung zu verbessern

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
static const std::string IONISIERENDES_GASWESEN		("Ionisierendes Gaswesen");
static const std::string GABALLIANER_SEUCHENSCHIFF	("Gaballianer");
static const std::string BLIZZARD_PLASMAWESEN		("Blizzard-Plasmawesen");
static const std::string MORLOCK_RAIDER				("Morlock-Raider");
static const std::string EELEN_GUARD				("Ehlen");
static const std::string BOSEANER					("Boseaner");
static const std::string KAMPFSTATION				("Kampfstation");
static const std::string KRYONITWESEN				("Kryonitwesen");
static const std::string MIDWAY_ZEITREISENDE		("Midway-Zeitreisende");
static const std::string ANAEROBE_MAKROBE			("Anaerobe Makrobe");
static const std::string ISOTOPOSPHAERISCHES_WESEN	("Isotoposphärisches Wesen");

#define StrToCStr(str) \
	CString(str.c_str())

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
namespace PLANET_CLASSES
{
	enum TYPE
	{
		 PLANETCLASS_M		= 0,
		 PLANETCLASS_O		= 1,
		 PLANETCLASS_L		= 2,
		 PLANETCLASS_P		= 3,
		 PLANETCLASS_H		= 4,
		 PLANETCLASS_Q		= 5,
		 PLANETCLASS_K		= 6,
		 PLANETCLASS_G		= 7,
		 PLANETCLASS_R		= 8,
		 PLANETCLASS_F		= 9,
		 PLANETCLASS_C		= 10,
		 PLANETCLASS_N		= 11,
		 PLANETCLASS_A		= 12,
		 PLANETCLASS_B		= 13,
		 PLANETCLASS_E		= 14,
		 PLANETCLASS_Y		= 15,
		 PLANETCLASS_I		= 16,
		 PLANETCLASS_J		= 17,
		 PLANETCLASS_S		= 18,
		 PLANETCLASS_T		= 19,
		 GRAPHICNUMBER		= 31 // Anzahl der verschiedenen Grafiken der Planeten
	};
}

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

static const int NoUC = 12;		// Number of Unique Complexes	( nur noch nicht Finanzen implementiert!!!! -> dann auf 13 hochstellen)
static const double SPECIAL_RESEARCH_DIV = 1.67;	// Wert, durch den die benötigten Punkte bei der Spezialforschung geteilt werden (umso höher, umso weniger FP sind nötig)
static const double TECHPRODBONUS = 2.0;		// Techbonus auf die Produktion (z.B. 2% pro Stufe auf Energie, Nahrung und Industrie)

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
		IMPROVE_SHIELDS		= 21,
		UPGRADE_OUTPOST		= 22,
		UPGRADE_STARBASE	= 23
	};
}

// Sonstiges
static const int DIFFERENT_TORPEDOS = 29;
static const int TORPEDOSPEED = 15;
static const int MAX_TORPEDO_RANGE = 200;
static const double DAMAGE_TO_HULL = 0.1;	// Prozentwert der bei Treffer immer auf Hülle geht
static const double POPSUPPORT_MULTI = 3.0;	// Multiplikator für Schiffsunterstützungskosten aus Bevölkerung im System

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
static const int TRADEROUTEHAB	= 20;	// aller wieviel Bevölkerung gibt es eine Handelsroute
static const int NOTRIL			= 12;	// Number of Trade Routes in List

// ------------- Ressourcen ----------------------------------------------------
namespace RESOURCES
{
	enum TYPE
	{
		TITAN               = 0,
		DEUTERIUM           = 1,
		DURANIUM            = 2,
		CRYSTAL             = 3,
		IRIDIUM             = 4,
		DERITIUM	         = 5
	};
}

static const int MAX_FOOD_STORE			=	25000;
static const int MAX_RES_STORE			=	125000;
static const int MAX_DERITIUM_STORE		=	100;

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

inline bool PT_IN_RECT(const CPoint& pt, int x1, int y1, int x2, int y2)
{
	return (x1 <= pt.x && pt.x < x2) && (y1 <= pt.y && pt.y < y2);
}

inline void SetAttributes(BOOLEAN is, int attribute, int &variable)
{
	if (is)
		variable |= attribute;
	else
		variable &= ~attribute;
}


// --- Künstliche Intelligenz ---------------------------------------------------
static const int MINBASEPOINTS	= 120;	///< minimale Punkte für den Außenpostenbau, damit die KI überhaupt beginnt dort einen zu bauen

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
