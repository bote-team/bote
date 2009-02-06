/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "Botf2Doc.h"
#include "MainBaseView.h"

// CFleetMenuView view

class CFleetMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CFleetMenuView)

protected:
	CFleetMenuView();           // protected constructor used by dynamic creation
	virtual ~CFleetMenuView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgeführt werden müssen.
	void OnNewRound(void);

private:
	// Funktionen
	/// Funkion legt alle Buttons für die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen des Flottenzusammenstellungsmenüs
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawFleetMenue(CDC* pDC, CRect thClientRect);

	// Attribute

	// Grafiken
	CBitmap bg_fleetmenu;			// Flottenzusammenstellungsmenü

	// Buttons

	// Hier die Rechtecke und Variablen, die wir benötigen wenn wir in der Flottenansicht sind
	USHORT m_iFleetPage;					// die aktuelle Seite für die Flottenansicht
	short  m_iOldShipInFleet;				// Merkvariable um das vorherige Schiff welches markiert war sich zu merken
	BOOLEAN m_bDrawFullFleet;				// soll die ganze Flottenansicht neu gezeichnet werden?
	BOOLEAN m_bShowNextButton;				// soll der next-Button angezeigt werden, falls wir nicht alles auf eine Seite bekommen
	BOOLEAN m_bShowBackButton;				// wenn wir nicht mehr auf der ersten Seite sind kommt der back-Button

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


