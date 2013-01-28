/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
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

public:
	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgeführt werden müssen.
	virtual void OnNewRound(void);

	/// Funktion lädt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

private:
	// Funktionen
	/// Funkion legt alle Buttons für die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen des Flottenzusammenstellungsmenüs
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawTransportMenue(Graphics* g);

	/// Funktion erstellt einen Vektor mit den transportierten Truppen
	void CreateTransportedTroopsVector();

	// Attribute

	// Grafiken
	Bitmap* bg_transportmenu;			// Transportsmenü

	// Buttons
	CArray<CMyButton*> m_TransportButtons;	///< die Buttons zum Ändern der Mengen im im Transportmenü

	// Hier die Variablen, wenn wir in der Transportansicht sind
	int m_nActiveTroopInSystem;			///< Nummer der Truppe im Feld der Truppen im System
	int m_nActiveTroopInShip;			///< Nummer der Truppe im Feld der Truppen im Schiff
	USHORT m_iTransportStorageQuantity;	///< Wieviel Ressourcen werden pro Klick verschoben
	CPoint m_ptLastSector;				///< Koordinate des letzten angezeigten Sektors
	vector<pair<CShips*, CTroop*> > m_vShipTroops; ///< Vektor mit allen Truppen auf den Schiffen

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
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


