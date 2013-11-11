#include "stdafx.h"
#include "Constants.h"

#include <map>


const int STARMAP_SECTOR_WIDTH	= 80;	///< Breite eines Sektors in Pixel bei 100% Zoom
const int STARMAP_SECTOR_HEIGHT	= 80;	///< Höhe eines Sektors in Pixel bei 100% Zoom

int STARMAP_SECTORS_HCOUNT	= 30;		///< Anzahl der Sektoren horizontal
int STARMAP_SECTORS_VCOUNT	= 20;		///< Anzahl der Sektoren vertikal

int STARMAP_TOTALWIDTH		= STARMAP_SECTORS_HCOUNT * STARMAP_SECTOR_WIDTH;	///< Gesamtbreite der View in Pixel bei 100%
int STARMAP_TOTALHEIGHT		= STARMAP_SECTORS_VCOUNT * STARMAP_SECTOR_HEIGHT;	///< Gesamthöhe bei 100%

double STARMAP_ZOOM_MAX		= 1.5;    ///< maximaler Zoom-Faktor

namespace
{
	std::map<WORKER::Typ, RESOURCES::TYPE> transformer;
}

void InitConstants()
{
	transformer.insert(std::pair<WORKER::Typ, RESOURCES::TYPE>(WORKER::TITAN_WORKER, RESOURCES::TITAN));
	transformer.insert(std::pair<WORKER::Typ, RESOURCES::TYPE>(WORKER::DEUTERIUM_WORKER, RESOURCES::DEUTERIUM));
	transformer.insert(std::pair<WORKER::Typ, RESOURCES::TYPE>(WORKER::DURANIUM_WORKER, RESOURCES::DURANIUM));
	transformer.insert(std::pair<WORKER::Typ, RESOURCES::TYPE>(WORKER::CRYSTAL_WORKER, RESOURCES::CRYSTAL));
	transformer.insert(std::pair<WORKER::Typ, RESOURCES::TYPE>(WORKER::IRIDIUM_WORKER, RESOURCES::IRIDIUM));
	transformer.insert(std::pair<WORKER::Typ, RESOURCES::TYPE>(WORKER::FOOD_WORKER, RESOURCES::FOOD));
}

RESOURCES::TYPE WorkerToResource(WORKER::Typ type)
{
	const std::map<WORKER::Typ, RESOURCES::TYPE>::const_iterator it = transformer.find(type);
	AssertBotE(it != transformer.end());
	return it->second;
}




