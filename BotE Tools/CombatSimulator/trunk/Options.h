#if !defined _OPTIONS_H__20050315_1300_
#define _OPTIONS_H__20050315_1300_

#pragma warning(disable: 4204)

#include "ResourceManager.h"

// ------------- Spielerrassen -------------------------------------------------
#define NOBODY				0
#define HUMAN				1
#define FERENGI				2
#define KLINGON				3
#define ROMULAN				4
#define CARDASSIAN			5
#define DOMINION			6
#define UNKNOWN				7

#define SMALL				0
#define NORMAL				1
#define BIG					2
#define GIANT				3

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
// Schiffsreichweiten
//#define RANGE_SHORT		    0
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
// Schiffseigenschaften
enum {NONE, ASSULTSHIP, BLOCKADESHIP, COMMANDSHIP, COMBATTRACTORBEAM, DOGFIGHTER, DOGKILLER, PATROLSHIP, RAIDER, SCIENCEVESSEL};

// ------------- Ressourcen ----------------------------------------------------
#define TITAN               0
#define DEUTERIUM           1
#define DURANIUM            2
#define CRYSTAL             3
#define IRIDIUM             4
#define DILITHIUM           5

inline void SetAttributes(BOOLEAN is, int attribute, int &variable)
{
	if (is)
		variable |= attribute;
	else
		variable &= ~attribute;
}
#endif // _OPTIONS_H__20050315_1300_
