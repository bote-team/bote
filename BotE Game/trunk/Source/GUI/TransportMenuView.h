/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "Botf2Doc.h"
#include "MainBaseView.h"

// CTransportMenuView view

class CTransportMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CTransportMenuView)

protected:
	CTransportMenuView();           // protected constructor used by dynamic creation
	virtual ~CTransportMenuView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	// Funktionen
	/// Funkion legt alle Buttons für die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen des Flottenzusammenstellungsmenüs
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawTransportMenue(Graphics* g);

	// Attribute

	// Grafiken
	Bitmap* bg_transportmenu;			// Transportsmenü

	// Buttons

	// Hier die Variablen, wenn wir in der Transportansicht sind
	BYTE m_byTroopNumberInSystem;		// Nummer der Truppe im Feld der Truppen im System
	BYTE m_byTroopNumberInShip;			// Nummer der Truppe im Feld der Truppen im Schiff
	USHORT m_iTransportStorageQuantity;	// Wieviel Ressourcen werden pro Klick verschoben

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
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


