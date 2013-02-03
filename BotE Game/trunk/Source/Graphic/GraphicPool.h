/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

using namespace Gdiplus;
/**
 * Klasse beinhaltet und verwaltet alle Grafiken, welche für Buttons benutzt werden. Ein Button fragt
 * bei einem CGraphicPool Objekt nach seiner zugehörigen Grafik an.
 *
 * @author Sir Pustekuchen
 * @version 0.0.1
 */
class CGraphicPool
{
public:
	/// Konstruktor
	/// @param path relativer Pfad in dem die Anwendung läuft.
	CGraphicPool(const CString& path);

	/// Destruktor
	~CGraphicPool(void);

	// Funktionen
	/// Funktion liefert einen Zeiger auf eine Grafik.
	/// @param name Name der Grafik.
	Bitmap* GetGDIGraphic(const CString &name);

	/*
	// <obsolete>
	/// Funktion liefert einen Zeiger auf eine Grafik.
	/// @param name Name der Grafik.
	CBitmap* GetGraphic(const CString &name);
	*/

private:
	// Attribute
	// <obsolete>
	/// Map welche zu einem Grafiknamen die passende Grafik bereithält.
	//CMap<CString, LPCSTR, CBitmap*, CBitmap*> m_Graphics;

	/// Map welche zu einem Grafiknamen die passende Grafik bereithält.
	CMap<CString, LPCSTR, Bitmap*, Bitmap*> m_GDIGraphics;

	/// der relative Pfad, in welchem die Anwendung läuft
	const CString m_strPath;
};
