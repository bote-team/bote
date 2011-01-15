/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "MyButton.h"
#include "FontLoader.h"
#include "Botf2Doc.h"

// CMainBaseView view

class CMainBaseView : public CView
{
	DECLARE_DYNCREATE(CMainBaseView)

protected:
	CMainBaseView();           // protected constructor used by dynamic creation
	virtual ~CMainBaseView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion setzt ein Untermenü. Diese Funktion sollte von der abgeleiteten Klasse überschrieben werden.
	/// @param menuID Nummer des Untermenüs
	virtual void SetSubMenu(BYTE menuID) {};

	/// Funktion gibt das aktuelle Untermenü zurück. Diese Funktion sollte von der abgeleiteten Klasse überschrieben werden.
	/// @return <code>NULL</code>
	virtual BYTE GetSubMenu() const {return NULL;}

	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgeführt werden müssen. Sollte von jeder
	/// abgeleiteten Klasse abgeleitet werden.
	virtual void OnNewRound() {};

	/// Funktion zum Festlegen der Spielerrasse in der View
	/// @pPlayer Zeiger auf Spielerrasse
	static void SetPlayersRace(CMajor* pPlayer) {m_pPlayersRace = pPlayer;}

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void) { return ""; }

protected:
	/// Funktion zeichnet die Button und macht alle möglichen Überprüfungen.
	/// @param g Referenz auf Graphics Object
	/// @param buttonArray Zeiger auf das Feld mit den Buttons
	/// @param counter Zählvariable die meinst angibt in welchem aktuellen Menü man sich befindet
	/// @param font Referenz auf zu benutzende Schrift
	/// @param fontBrush Referenz auf Farbepinsel für Font (Schriftfarbe)
	void DrawGDIButtons(Graphics* g, CArray<CMyButton*>* buttonArray, int counter, Gdiplus::Font &font, Gdiplus::SolidBrush &fontBrush);

	/// Funktion läßt einen Button auf ein MouseOver reagieren
	/// @param point Referenz auf den Punkt, über dem sich die Maus aktuell befindet
	/// @param buttonArray Zeiger auf das Feld mit den Buttons
	void ButtonReactOnMouseOver(const CPoint &point, CArray<CMyButton*>* buttonArray);

	/// Funktion läßt einen Button auf einen linken Mausklick reagieren
	/// @param point Referenz auf den Punkt, über dem sich die Maus aktuell befindet
	/// @param buttonArray Zeiger auf das Feld mit den Buttons
	/// @param counter Zählvariable die meinst angibt in welchem aktuellen Menü man sich befindet
	/// @param invalidate gibt an ob die Funktion automatisch die View neuzeichnen soll
	/// @param onlyActivate gibt an ob der Button beim Klick nur aktiviert, nicht deaktiviert wird
	/// @return wurde ein Button angeklickt gibt die Funktion <code>TRUE</code> zurück, ansonsten <code>FALSE</code>
	BOOLEAN ButtonReactOnLeftClick(const CPoint &point, CArray<CMyButton*>* buttonArray, int &counter, BOOLEAN invalidate = TRUE, BOOLEAN onlyActivate = FALSE);

	/// Funktion rechnet einen übergebenen Punkt in logische Koordinaten um.
	/// @param point Referenz auf umzurechnenden Punkt
	void CalcLogicalPoint(CPoint &point);

	/// Funktion rechnet ein übergebenes Rechteck in logische Koordinaten um.
	/// @param point Referenz auf umzurechnendes Rechteck
	void CalcDeviceRect(CRect &rect);

	/// Funktion generiert alle Buttons für das Menü. Sollte von der abgeleiteten Klasse überschrieben werden.
	virtual void CreateButtons() {};	

	// Attribute
	CSize m_TotalSize;				///< Größe der View in logischen Koordinaten

	static CMajor* m_pPlayersRace;	///< Zeiger auf Spielerrasse
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
};


