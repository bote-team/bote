/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "Botf2Doc.h"
#include "MainBaseView.h"

// CEmpireMenuView view

class CEmpireMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CEmpireMenuView)

protected:
	CEmpireMenuView();           // protected constructor used by dynamic creation
	virtual ~CEmpireMenuView();

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

	void SetSubMenu(BYTE menuID) {m_iSubMenu = menuID;}

	BYTE GetSubMenu() const {return m_iSubMenu;}

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnInitialUpdate();

private:
	// Funktionen
	void DrawEmpireNewsMenue(Graphics* g);			// Fkt. zum Zeichnen der Nachrichtenansicht an das Imperium
	void DrawEmpireSystemMenue(Graphics* g);		// Fkt. zum Zeichnen der Ansicht aller Systeme des Imperiums
	void DrawEmpireShipMenue(Graphics* g);			// Fkt. zum Zeichnen der Ansicht aller Schiffe des Imperiums
	void CreateButtons();

	//Attribute
	// Grafiken
	Bitmap* bg_newsovmenu;			// Nachrichtenübersichtsmenü
	Bitmap* bg_systemovmenu;		// Systemübersichtsmenü
	Bitmap* bg_shipovmenu;			// Schiffsübersichtsmenü

	// Buttons
	CArray<CMyButton*> m_EmpireNewsFilterButtons;	///< Buttons zum filtern der Nachrichten in der Imperiumsübersicht
	CArray<CMyButton*> m_EmpireNewsButtons;			///< Buttons am unteren Rand der Imperiumsübersicht
	CArray<CMyButton*> m_EmpireShipsFilterButtons;	///< Buttons um aktuelle oder verlorene Schiffe anzuzeigen
	CArray<CMyButton*> m_EmpireSystemFilterButtons;	///< Buttons um in der Imperiumssystemansicht zwischen Ressourcen- und normaler Ansicht zu wechseln

	// Hier die Rechtecke und Variablen, die wir benötigen wenn wir in der Nachrichten und Informationsansicht sind
	USHORT m_iSubMenu;						// in welchem Untermenü der Informationsansicht sind wir
	USHORT m_iWhichNewsButtonIsPressed;		// wollen wir nur eine bestimme Nachtichtenart anzeigen lassen, z.b. Forschung
	short  m_iClickedNews;					// auf welche Nachticht oder Information in der Liste wurde geklickt
	short  m_iOldClickedNews;				// auf welche Nachricht wurde vorher geklickt, braucht man als Modifikator
	short  m_iClickedSystem;				// auf welches System wurde in der Liste geklickt, welches wurde markiert
	short  m_iOldClickedSystem;				// auf welches System wurde vorher geklickt, braucht man als Modifikator
	short  m_iClickedShip;					// auf welches Schiff wurde in der Liste geklickt, welches wurde markiert
	short  m_iOldClickedShip;				// auf welches Schiffe wurde vorher geklickt, braucht man als Modifikator
	
	BOOLEAN m_bShowAliveShips;				// sollen nur die noch vorhandenen Schiffe angezeigt werden, oder die verlorenen
	BOOLEAN m_bShowResources;				// sollen in der Imperiumssystemübersicht die Ressourcen oder die normale Übersicht gezeichnet werden.		
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
};


