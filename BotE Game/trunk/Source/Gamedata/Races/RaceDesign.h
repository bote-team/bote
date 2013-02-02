/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "afx.h"

/// Klasse beinhaltet Informationen zu Farben, Schriftfarben, Schriftart und Schriftgrößen
/// für bestimmte grafische Elemente einer Rasse.
class CRaceDesign :	public CObject
{
	/// CMajor-Klasse, welche ein Objekt dieser Klasse verwendet hat uneingeschränkten Zugriff
	friend class CMajor;
	friend class CFontLoader;

	DECLARE_SERIAL(CRaceDesign)

public:
	/// Standardkonstruktor
	CRaceDesign(void);

	/// Standarddestruktor
	~CRaceDesign(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen

	/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
	void Reset(void);

	/// Diese Funktion erstellt aus einem RGB String eine Farbe.
	/// @param sRGB RGB-Farbwerte als String
	/// @param sToken Trennzeichen zwischen den einzelnen Farbwerten im String
	/// @return Farbe
	static COLORREF StringToColor(const CString& sRGB, const CString& sToken = ",")
	{
		int iStart = 0;

		return RGB(
			atoi(sRGB.Tokenize(sToken, iStart)),
			atoi(sRGB.Tokenize(sToken, iStart)),
			atoi(sRGB.Tokenize(sToken, iStart)));
	}

	static COLORREF MakeDarker(const COLORREF& color, int nValue)
	{
		int r	= GetRValue(color) - nValue;
		int g	= GetGValue(color) - nValue;
		int b	= GetBValue(color) - nValue;

		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;

		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;

		return RGB(r,g,b);
	}

public:
	// Attribute
	COLORREF			m_clrSector;					///<!!! Sektormarkierung
	COLORREF			m_clrSmallBtn;					///<!!! Schriftfarbe für kleinen Button
	COLORREF			m_clrLargeBtn;					///<!!! Schriftfarbe für großen Button
	COLORREF			m_clrSmallText;					///<!!! Schriftfarbe für kleinen Text
	COLORREF			m_clrNormalText;				///<!!! Schriftfarbe für normalen Text
	COLORREF			m_clrSecondText;				///<!!! Zweitfarbe für normalen Text

	COLORREF			m_clrGalaxySectorText;			///<!!! Farbe der Systemnamen auf der Galaxiemap

	COLORREF			m_clrListMarkTextColor;			///<!!! Farbe für Text wenn dieser markiert ist
	COLORREF			m_clrListMarkPenColor;			///<!!! Farbe für Umrandung bei Markierung eines Eintrags in einer Liste

	COLORREF			m_clrRouteColor;				///<!!! Farbe der Handels- und Ressourcenrouten

	BYTE				m_byGDIFontSize[6];				///<!!! Fontgrößen für GDI Darstellung
	BYTE				m_byGDIPlusFontSize[6];			///<!!!	Fontgrößen für GDI+ Darstellung
	CString				m_sFontName;					///<!!!	Name der Schriftart
};
