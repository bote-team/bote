/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "FontLoader.h"
#include "OverlayBanner.h"
#include "Botf2Doc.h"

// CBottomBaseView view

class CBottomBaseView : public CView
{
	DECLARE_DYNCREATE(CBottomBaseView)

protected:
	CBottomBaseView();           // protected constructor used by dynamic creation
	virtual ~CBottomBaseView();

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

	/// Funktion gibt das akutuelle Untermenü zurück. Diese Funktion sollte von der abgeleiteten Klasse überschrieben werden.
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
	// Funktionen
	/// Funktion rechnet einen übergebenen Punkt in logische Koordinaten um.
	/// @param point Referenz auf umzurechnenden Punkt
	void CalcLogicalPoint(CPoint &point);

	/// Funktion rechnet ein übergebenes Rechteck in logische Koordinaten um.
	/// @param point Referenz auf umzurechnendes Rechteck
	void CalcDeviceRect(CRect &rect);

	/// Funktion liefert die rassenspezifische Schriftfarbe für die kleinen Buttons.
	/// @return Farbe
	Gdiplus::Color GetFontColorForSmallButton(void);

	// Attribute
	CSize m_TotalSize;					///< Größe der View in logischen Koordinaten

	static CMajor* m_pPlayersRace;		///< Zeiger auf Spielerrasse

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


