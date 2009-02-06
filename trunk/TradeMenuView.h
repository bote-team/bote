/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "MainBaseView.h"


// CTradeMenuView view

class CTradeMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CTradeMenuView)

protected:
	CTradeMenuView();           // protected constructor used by dynamic creation
	virtual ~CTradeMenuView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	void SetSubMenu(BYTE menuID) {m_bySubMenu = menuID;}

	BYTE GetSubMenu() const {return m_bySubMenu;}

private:
	// Funktionen
	/// Funkion legt alle Buttons für die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zeichnet das Börsenmenu
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawGlobalTradeMenue(CDC* pDC, CRect theClientRect);

	/// Funktion zeichnet das Monopolmenü
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawMonopolMenue(CDC* pDC, CRect theClientRect);

	/// Funktion zeichnet das Transfermenü mit den Aktivitäten an der Handelsbörse
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawTradeTransferMenue(CDC* pDC, CRect theClientRect);
	// Attribute

	// Grafiken
	CBitmap bg_trademenu;			// Handelsmenü (Börse)
	CBitmap bg_monopolmenu;			// Monopolmenü
	CBitmap bg_tradetransfermenu;	// Handelstransfermenü
	CBitmap bg_empty1;				// genereller leerer Bildschirm 

	// Hier die Rechtecke und Variablen, die wir benötigen wenn wir in der Handelsansicht sind
	USHORT m_bySubMenu;				// in welchem Untermenü der Handelsansicht befinden wir uns
	double m_dMonopolCosts[5];		// die einzelnen Monopolkosten
	BOOLEAN   m_bCouldBuyMonopols;	// können wir schon Monopole kaufen

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
};


