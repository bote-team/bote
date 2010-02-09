/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
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
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawGlobalTradeMenue(Graphics* g);

	/// Funktion zeichnet das Monopolmenü
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawMonopolMenue(Graphics* g);

	/// Funktion zeichnet das Transfermenü mit den Aktivitäten an der Handelsbörse
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawTradeTransferMenue(Graphics* g);

	/// Funktion zeichnet die Buttons unter den Handelsmenüs.
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param pMajor Spielerrasse
	void DrawTradeMainButtons(Graphics* g, CMajor* pMajor);
	// Attribute

	// Grafiken
	Bitmap* bg_trademenu;			// Handelsmenü (Börse)
	Bitmap* bg_monopolmenu;			// Monopolmenü
	Bitmap* bg_tradetransfermenu;	// Handelstransfermenü
	Bitmap* bg_empty1;				// genereller leerer Bildschirm

	// Buttons
	CArray<CMyButton*> m_TradeMainButtons;		///< die unteren Buttons in den Handelsansichten

	// Hier die Rechtecke und Variablen, die wir benötigen wenn wir in der Handelsansicht sind
	USHORT m_bySubMenu;				// in welchem Untermenü der Handelsansicht befinden wir uns
	double m_dMonopolCosts[5];		// die einzelnen Monopolkosten
	BOOLEAN   m_bCouldBuyMonopols;	// können wir schon Monopole kaufen

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


