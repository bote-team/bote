#include "stdafx.h"
#include "FontLoader.h"

BYTE CFontLoader::m_byFontSizes[DOMINION][6] = {
	14,24,25,34,50,54,	// ohne GDI+
	8,13,13,19,27,27,	// Ferengi Bajoran Regular Font
//	7,13,15,19,23,23,	// Ferengi Chemical Reaction Font
	10,13,15,20,31,34,
	12,20,19,28,40,50,
	11,17,17,23,31,45,
	11,17,17,23,31,45};	// Dominion Hammerhead
//	10,17,17,22,32,42};	// Dominion Hammerhead
//	16,25,25,35,45,55};	// Dominion Opticon

BYTE CFontLoader::m_byGDIPlusFontSizes[DOMINION][6] = {
	5,14,15,22,32,38,
	6,9,10,14,19,19,
	5,10,10,14,21,25,
	6,11,11,15,22,28,
	4,11,11,15,22,28,
	4,11,11,15,22,28};
//	10,17,17,22,32,42};	// Dominion Hammerhead

/*	0 -> ganz kleine Schriften
	1 -> kleiner Button
	2 -> großer Button
	3 -> für normale Schirften
	4 -> Zweitfarbe für normale Schriften*/
COLORREF CFontLoader::m_Colors[DOMINION][5] = {
	RGB(200,150,0), RGB(0,0,0),		 RGB(0,0,0),   RGB(255,185,45),  RGB(175,82,154),	// Föderation
	RGB(200,150,0), RGB(0,0,0),		 RGB(0,50,0),  RGB(195,195,0),	 RGB(195,195,0),	// Ferengi
	RGB(200,150,0),	RGB(0,0,0),		 RGB(0,0,0),   RGB(255,60,60),	 RGB(180,180,180),	// Klingonen
	RGB(200,150,0),	RGB(80,180,230), RGB(4,35,15), RGB(140,196,203), RGB(50,185,0),		// Romulaner
	RGB(200,150,0), RGB(0,0,0),		 RGB(0,0,0),   RGB(180,180,180), RGB(74,146,138),	// Cardassianer
	RGB(200,150,0), RGB(0,0,0),		 RGB(0,0,0),   RGB(200,255,255), RGB(74,146,138)	// Dominion
};

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CFontLoader::CFontLoader(void)
{
}

CFontLoader::~CFontLoader(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion generiert die passende Schriftart. Dafür wird die jeweilige Rasse im Parameter <code>playersRace
/// </code>, die allgemeine Schriftgröße (0: sehr klein, 1: klein, 2: normal, 3: groß, 4: sehr groß, 5: kolossal)
/// mittels <code>size</code> und ein Zeiger auf ein Fontobjekt <code>font</code>, in welches die generierte Schrift
/// gespeichert wird, übergeben.
void CFontLoader::CreateFont(BYTE playersRace, BYTE size, CFont* font)
{
	ASSERT(font != NULL);
	
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT)); 
	switch (playersRace)
	{
	case HUMAN:
		strcpy_s(lf.lfFaceName, FEDERATION_RACEFONT);
		break;
	case FERENGI:
		strcpy_s(lf.lfFaceName, FERENGI_RACEFONT);
		break;
	case KLINGON:
		strcpy_s(lf.lfFaceName, KLINGON_RACEFONT);
		break;
	case ROMULAN:
		strcpy_s(lf.lfFaceName, ROMULAN_RACEFONT);
		break;
	case CARDASSIAN:
		strcpy_s(lf.lfFaceName, CARDASSIAN_RACEFONT);
		break;
	case DOMINION:
		strcpy_s(lf.lfFaceName, DOMINION_RACEFONT);
		break;
	}
	lf.lfHeight = m_byFontSizes[playersRace-1][size];
	lf.lfQuality = CLEARTYPE_QUALITY;
	lf.lfPitchAndFamily = FF_SWISS;
	font->CreateFontIndirect(&lf);
}


/// Diese Funktion generiert die passende Schriftart. Dafür wird die jeweilige Rasse im Parameter <code>playersRace
/// </code>, die allgemeine Schriftgröße (0: sehr klein, 1: klein, 2: normal, 3: groß, 4: sehr groß, 5: kolossal)
/// mittels <code>size</code>, die gewünschte Schriftfarbe mittels <code>color</code> und ein Zeiger auf ein
/// Fontobjekt <code>font</code>, in welches die generierte Schrift gespeichert wird, übergeben. Der Rückgabewert
/// der Funktion ist die passende Schriftfarbe.
COLORREF CFontLoader::CreateFont(BYTE playersRace, BYTE size, BYTE color, CFont *font)
{
	CFontLoader::CreateFont(playersRace, size, font);
	return m_Colors[playersRace-1][color];
}

void CFontLoader::CreateGDIFont(BYTE playersRace, BYTE size, CString &family, Gdiplus::REAL &fontSize)
{
	switch (playersRace)
	{
	case HUMAN:
		family = FEDERATION_RACEFONT;
		break;
	case FERENGI:
		family = FERENGI_RACEFONT;
		break;
	case KLINGON:
		family = KLINGON_RACEFONT;
		break;
	case ROMULAN:
		family = ROMULAN_RACEFONT;
		break;
	case CARDASSIAN:
		family = CARDASSIAN_RACEFONT;
		break;
	case DOMINION:
		family = DOMINION_RACEFONT;
		break;
	}
	fontSize = (Gdiplus::REAL)m_byGDIPlusFontSizes[playersRace-1][size];	
}

void CFontLoader::GetGDIFontColor(BYTE playersRace, BYTE colorType, Gdiplus::Color &color)
{
	color.SetFromCOLORREF(m_Colors[playersRace-1][colorType]);
}