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

// CIntelMenuView view

class CIntelMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CIntelMenuView)

protected:
	CIntelMenuView();           // protected constructor used by dynamic creation
	virtual ~CIntelMenuView();

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

	/// Funktion zeichnet die kleinen Rassensymbole in den Geheimdienstansichten
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param highlightPlayersRace soll die eigene Rasse aktiviert dargestellt werden
	void DrawRaceLogosInIntelView(CDC* pDC, BOOLEAN highlightPlayersRace = FALSE);

	/// Funktion zeichnet die Geheimdienstinformationen an den rechten Rand der Geheimdienstansichten
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	void DrawIntelInformation(CDC* pDC);

	/// Funktion zeichnet das Geheimdienstmenü, in welchem man die globale prozentuale Zuteilung machen kann.
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawIntelAssignmentMenu(CDC* pDC, CRect theClientRect);

	/// Funktion zeichnet das Spionagegeheimdienstmenü.
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawIntelSpyMenu(CDC* pDC, CRect theClientRect);

	/// Funktion zeichnet das Sabotagegeheimdienstmenü.
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawIntelSabotageMenu(CDC* pDC, CRect theClientRect);

	/// Funktion zeichnet das Menü mit den Geheimdienstberichten.
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawIntelReportsMenu(CDC* pDC, CRect theClientRect);

	/// Funktion zeichnet das Menü für die Geheimdienstanschläge.
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawIntelAttackMenu(CDC* pDC, CRect theClientRect);

	/// Funktion zeichnet das Menü für die Geheimdienstinformationen
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawIntelInfoMenu(CDC* pDC, CRect theClientRect);

	// Attribute
	// Grafiken
	CBitmap bg_intelassignmenu;		///< globales Geheimdienstzuweisungsmenü
	CBitmap bg_intelspymenu;		///< Spionagegeheimdienstmenü
	CBitmap bg_intelsabmenu;		///< Sabotagegeheimdienstmenü
	CBitmap bg_intelreportmenu;		///< Geheimdienstnachrichtenmenü
	CBitmap bg_intelinfomenu;		///< Geheimdienstinformationsmenü
	CBitmap bg_intelattackmenu;		///< Geheimdienstanschlagsmenü

	// Buttons
	CArray<CMyButton*> m_IntelligenceMainButtons;	///< die Buttons unter den Geheimdienstmenüs

	// Hier die Variablen für die Geheimdienstansicht
	BYTE m_bySubMenu;						///< welches Untermenü im Geheimdienstmenü wurde aktiviert
	BYTE m_byActiveIntelRace;				///< auf welche gegnerische Rasse beziehen sich die Intelaktionen
	short m_iOldClickedIntelReport;			///< auf welchen Bericht wurde vorher geklickt, braucht man als Modifikator
	CBitmap m_RaceLogos[DOMINION];			///< die Rassensymbole der Hauptrassen
	CBitmap m_RaceLogosDark[DOMINION];		///< die Rassensymbole der Hauptrassen im dunklen Zustand

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	virtual void OnInitialUpdate();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


