#include "stdafx.h"
#include "Options.h"

const int STARMAP_SECTOR_WIDTH	= 80;	///< Breite eines Sektors in Pixel bei 100% Zoom
const int STARMAP_SECTOR_HEIGHT	= 80;	///< Höhe eines Sektors in Pixel bei 100% Zoom

int STARMAP_SECTORS_HCOUNT	= 30;		///< Anzahl der Sektoren horizontal
int STARMAP_SECTORS_VCOUNT	= 20;		///< Anzahl der Sektoren vertikal

int STARMAP_TOTALWIDTH		= STARMAP_SECTORS_HCOUNT * STARMAP_SECTOR_WIDTH;	///< Gesamtbreite der View in Pixel bei 100%
int STARMAP_TOTALHEIGHT		= STARMAP_SECTORS_VCOUNT * STARMAP_SECTOR_HEIGHT;	///< Gesamthöhe bei 100%

double STARMAP_ZOOM_MAX		= 1.5;    ///< maximaler Zoom-Faktor




