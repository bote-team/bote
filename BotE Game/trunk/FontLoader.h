/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

#include "Options.h"

class CFontLoader
{
public:
	/**
	 * Diese Funktion generiert die passende Schriftart. Dafür wird die jeweilige Rasse im Parameter <code>playersRace
	 * </code>, die allgemeine Schriftgröße (0: sehr klein, 1: klein, 2: normal, 3: groß, 4: sehr groß, 5: kolossal)
	 * mittels <code>size</code> und ein Zeiger auf ein Fontobjekt <code>font</code>, in welches die generierte Schrift
	 * gespeichert wird, übergeben.
	 */
	static void CreateFont(BYTE playersRace, BYTE size, CFont* font);

	/**
	 * Diese Funktion generiert die passende Schriftart. Dafür wird die jeweilige Rasse im Parameter <code>playersRace
	 * </code>, die allgemeine Schriftgröße (0: sehr klein, 1: klein, 2: normal, 3: groß, 4: sehr groß, 5: kolossal)
	 * mittels <code>size</code>, die gewünschte Schriftfarbe mittels <code>color</code> und ein Zeiger auf ein
	 * Fontobjekt <code>font</code>, in welches die generierte Schrift gespeichert wird, übergeben. Der Rückgabewert
	 * der Funktion ist die passende Schriftfarbe.
	 */
	static COLORREF CreateFont(BYTE playersRace, BYTE size, BYTE color, CFont* font);

	/**
	 * Diese Funktion gibt eine bestimmte Farbe für die Schrift einer Rasse zurück. Dafür wird die jeweilige Rasse im
	 * Parameter <code>playersRace</code> und die gewünschte Schriftfarbe im Paramter <code>color</code> übergeben.
	 * Jede Rasse hat fünfs verschiedene Farben für Schriften.
	 */
	static COLORREF GetFontColor(BYTE playersRace, BYTE color) {return m_Colors[playersRace-1][color];}

	static void CreateGDIFont(BYTE playersRace, BYTE size, CString &family, Gdiplus::REAL &fontSize);

	static void CFontLoader::GetGDIFontColor(BYTE playersRace, BYTE colorType, Gdiplus::Color &color);

private:
	/// Konstruktor
	CFontLoader(void);

	/// Destruktor
	~CFontLoader(void);

	/// Diese statische Variable beinhaltet die verschiedenen Schriftgrößen für die einzelnen Rassen.
	static BYTE m_byFontSizes[DOMINION][6];

	/// Diese statische Variable beinhaltet die verschiedenen Schriftgrößen für die einzelnen Rassen bei GDIPlus.
	static BYTE m_byGDIPlusFontSizes[DOMINION][6];

	/// Diese statische Variable beinhaltet die verschiedenen Schriftfarben der einzelnen Rassen.
	static COLORREF m_Colors[DOMINION][5];
};
