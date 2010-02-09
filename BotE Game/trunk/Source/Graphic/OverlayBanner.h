/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

class COverlayBanner
{
public:
	/// Standardkonstruktor
	COverlayBanner(const CPoint &ko, const CPoint &size, const CString &text, COLORREF textColor);

	/// Destruktor
	~COverlayBanner(void);

	// Zugriffsfunktionen
	/// Funktion legt die Hintergrundfarbe des Banners fest.
	void SetBackgroundColor(COLORREF color) {m_BackgroundColor = color;}

	/// Funktion legt die Farbe des Rahmens vom Banner fest.
	void SetBorderColor(COLORREF color) {m_BorderColor = color;}

	/// Funktion legt die Farbe des Textes auf dem Banner fest.
	void SetTextColor(COLORREF color) {m_TextColor = color;}

	/// Funktion legt den Tranparenzwert (Alphawert) des Hintergrundes vom Banner fest.
	void SetAlphaValue(int value) {m_iAlphaValue = value;}

	/// Funktion legt die Breite des Rahmens des Banners fest.
	void SetBorderWidth(int width) {m_iBorderWidth = width;}

	// sonstige Funktionen
	
	/// Funktion zeichnet das Banner auf den übergebenen Zeichenkontext mittels GDI+.
	/// @param g Zeiger auf das GDI+ Grafikobjekt
	/// @param font Font zum Zeichnen des Textes
	void Draw(Graphics *g, Gdiplus::Font* font);

private:
	// Attribute
	CPoint m_KO;				///< linke obere Ecke des Banners

	CSize m_Size;				///< Größe des Banners

	COLORREF m_TextColor;		///< Textfarbe

	COLORREF m_BackgroundColor;	///< Hintergrundfarbe

	COLORREF m_BorderColor;		///< Farbe des Rahmens

	int m_iAlphaValue;			///< Wert des Alphakanals, somit Transparenzwert

	int m_iBorderWidth;			///< Breite des Rahmens

	CString m_strText;			///< Text im Banner
};
