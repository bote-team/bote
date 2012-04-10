#include "stdafx.h"
#include "FontLoader.h"
#include "Races\Major.h"

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
void CFontLoader::CreateFont(const CMajor* pPlayersRace, BYTE size, CFont* font)
{
	ASSERT(font != NULL);
	
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT)); 
	
	strcpy_s(lf.lfFaceName, pPlayersRace->GetDesign()->m_sFontName);
	
	lf.lfHeight = pPlayersRace->GetDesign()->m_byGDIFontSize[size];
	lf.lfQuality = CLEARTYPE_QUALITY;
	lf.lfPitchAndFamily = FF_SWISS;
	
	font->CreateFontIndirect(&lf);
}


/// Diese Funktion generiert die passende Schriftart. Dafür wird die jeweilige Rasse im Parameter <code>playersRace
/// </code>, die allgemeine Schriftgröße (0: sehr klein, 1: klein, 2: normal, 3: groß, 4: sehr groß, 5: kolossal)
/// mittels <code>size</code>, die gewünschte Schriftfarbe mittels <code>color</code> und ein Zeiger auf ein
/// Fontobjekt <code>font</code>, in welches die generierte Schrift gespeichert wird, übergeben. Der Rückgabewert
/// der Funktion ist die passende Schriftfarbe.
COLORREF CFontLoader::CreateFont(const CMajor* pPlayersRace, BYTE size, BYTE color, CFont *font)
{
	CFontLoader::CreateFont(pPlayersRace, size, font);
	return GetFontColor(pPlayersRace, color);
}

COLORREF CFontLoader::GetFontColor(const CMajor* pPlayersRace, BYTE colorType)
{
	COLORREF clrColor;
	switch (colorType)
	{
	case 0:	clrColor = pPlayersRace->GetDesign()->m_clrSmallText; break;
	case 1:	clrColor = pPlayersRace->GetDesign()->m_clrSmallBtn; break;
	case 2:	clrColor = pPlayersRace->GetDesign()->m_clrLargeBtn; break;
	case 3:	clrColor = pPlayersRace->GetDesign()->m_clrNormalText; break;
	case 4:	clrColor = pPlayersRace->GetDesign()->m_clrSecondText; break;
	}

	return clrColor;
}

void CFontLoader::CreateGDIFont(const CMajor* pPlayersRace, BYTE size, CString &family, Gdiplus::REAL &fontSize)
{
	family = pPlayersRace->GetDesign()->m_sFontName;
	fontSize = (Gdiplus::REAL)pPlayersRace->GetDesign()->m_byGDIPlusFontSize[size];
}

void CFontLoader::GetGDIFontColor(const CMajor* pPlayersRace, BYTE colorType, Gdiplus::Color &color)
{
	COLORREF clrColor = GetFontColor(pPlayersRace, colorType);
	color.SetFromCOLORREF(clrColor);
}