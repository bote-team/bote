/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "GraphicPool.h"

/**
 * Buttonklasse welche verschiedene Zustände eines Buttons darstellen kann. Gleichzeitig bietet diese Klasse
 * eine gute Abstraktion, um einfach Klicks abzufangen und Zeichenoperationen durchführen zu können.
 *
 * @author Sir Pustekuchen
 * @version 0.0.2
 */
class CMyButton : public CObject
{
public:
	/// Konstruktor mit kompletter Parameterübergabe.
	/// @param point linke obere Koordinate des Buttons
	/// @param size Größe des Buttons in x- und y-Richtung
	/// @param text der Text auf dem Button
	/// @param normGraphicName Name des Grafikfiles für die normale Darstellung
	/// @param inactiveGraphicName Name des Grafikfiles für die inaktive Darstellung
	/// @param activeGraphicName Name des Grafikfiles für die aktive Darstellung
	CMyButton(CPoint point, CSize size, const CString& text,
		const CString& normGraphicName, const CString& inactiveGraphicName, const CString& activeGraphicName);

	/// Destruktor
	~CMyButton(void);

	// Zugriffsfunktionen
	/// Funktion gibt den aktuellen Zustand des Buttons zurück.
	BYTE GetState() const {return m_byStatus;}

	/// Funktion gibt das benötigte Rechteck des Buttons zurück.
	CRect GetRect() const {return CRect(m_KO.x, m_KO.y, m_KO.x+m_Size.cx, m_KO.y+m_Size.cy);}

	/// Funktion gibt den Text des Buttons zurück
	const CString& GetText() const {return m_strText;}

	/// Funktion setzt den Status/Zustand des Buttons.
	/// @param newState neuer Status für den Button (0 -> normal, 1 -> aktiviert, sonst deaktiviert)
	void SetState(BYTE newState) {m_byStatus = newState;}

	/// Funktion ändert den Text auf dem Button.
	/// @param text Text auf dem Button
	void SetText(const CString& text) {m_strText = text;}

	// Funktionen
	/// Funktion überprüft, ob der übergebene Punkt (z.B. Mausklick) in dem Feld des Buttons liegt.
	/// @param pt der zur überprüfende Punkt
	BOOLEAN ClickedOnButton(const CPoint& pt);

	/// Diese Funktion zeichnet den Button in den übergebenen Gerätekontext.
	/// @param g Referenz auf Graphics Objekt
	/// @param graphicPool Zeiger auf die Sammlung aller Grafiken
	/// @param font Referenz auf zu benutzende Schrift
	/// @param brush Referenz auf Farbepinsel für Font (Schriftfarbe)
	void DrawButton(Gdiplus::Graphics &g, CGraphicPool* graphicPool, Gdiplus::Font &font, Gdiplus::SolidBrush &brush);

	/// Funktion aktiviert den Button. Wenn dieser inaktiv ist, kann er nicht aktiviert werden.
	BOOLEAN Activate();

	/// Funktion deaktiviert den Button. Dieser kehrt dann zu seinem alten Zustand zurück.
	BOOLEAN Deactivate();

private:
	// Attribute
	BYTE m_byStatus;				///< Der Status des Buttons, also ob normal, inaktiv oder aktiv.
	CString m_strText;				///< Der Text auf dem Button.
	const CPoint m_KO;				///< Koordinate der linken oberen Ecke des Buttons.
	const CSize m_Size;				///< Die Größe in x und y Richtung des Buttons.
	const CString m_strNormal;		///< Die Grafik für den Button im normalen Zustand.
	const CString m_strInactive;	///< Die Grafik für den Button im deaktivierten Zustand.
	const CString m_strActive;		///< Die Grafik für den Button im aktivierten Zustand.
};
