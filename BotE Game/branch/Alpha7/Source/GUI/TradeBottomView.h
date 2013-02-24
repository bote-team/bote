/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BottomBaseView.h"

// CTradeBottomView view

class CTradeBottomView : public CBottomBaseView
{
	DECLARE_DYNCREATE(CTradeBottomView)

protected:
	CTradeBottomView();           // protected constructor used by dynamic creation
	virtual ~CTradeBottomView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion lädt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

private:
	// Funktionen

	// Attribute
	// Grafiken
	Bitmap* m_pSmallButton;

	// Variablen für das Handelsmenü
	USHORT m_iNumberOfHistoryRounds;	// Wieviele Runden sollen in dem Diagramm für die TradeHistory angezeigt werden
	USHORT m_iWhichRessource;			// Was für eine Ressource soll im Diagramm angezeigt werden
	BOOLEAN m_bDrawLittleRects;			// soll auf der Diagrammlinie für jede Runde ein kleines Viereck angezeigt werden

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


