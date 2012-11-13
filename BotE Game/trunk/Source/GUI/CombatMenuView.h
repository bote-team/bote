/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

#include "gui/MainBaseView.h"

// forward declaration
class CRace;

class CCombatMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CCombatMenuView)

protected:
	CCombatMenuView(void);				// protected constructor used by dynamic creation
	virtual ~CCombatMenuView(void);

	public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

	/// Funktion lädt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

public:
	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgeführt werden müssen. Sollte von jeder
	/// abgeleiteten Klasse abgeleitet werden.
	virtual void OnNewRound();
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
	// Funktionen
	void DrawCombatDecisionMenue(Graphics* g);		///< Funktion zum Zeichnen des Entscheidungsmenüs
	void DrawCombatInfoMenue(Graphics* g);			///< Funktion zum Zeichnen des Kampfinfomenüs (andere Spieler im Kampf)
	void DrawCombatOrderMenue(Graphics* g);			///< Funktion zum Zeichnen des Kampfinfomenüs (andere Spieler im Kampf)
	void CreateButtons();

	// Attribute
	// Grafiken
	Bitmap* bg_combatdecmenu;			// Kampfentscheidungsmenü
	Bitmap* bg_combatinfomenu;			// Kampfinfomenü
	Bitmap* bg_combatordermenu;			// Kampfbefehlsmenü

	bool m_bInOrderMenu;				// Ist das Kampfbefehlsmenü aktiviert

	// Buttons
	CArray<CMyButton*> m_CombatDecisionButtons;	///< Buttons zur Kampfentscheidung
	CArray<CMyButton*> m_CombatTacticButtons;	///< Buttons zur Taktikeinstellung in der Kampfbefehlsansicht
	CArray<CMyButton*> m_CombatOrderButtons;	///< Buttons in der Kampfbefehlsansicht für bereit und zurück
	CArray<CMyButton*> m_FriendShipsCursor;		///< Buttons um freundliche Schiffe durchzuschalten
	CArray<CMyButton*> m_EnemyShipsCursor;		///< Buttons um feindliche Schiffe durchzuschalten
	CArray<CMyButton*> m_ShipTypeButton;		///< Buttons zum Wechseln des Schiffstyps

	std::set<const CRace*> m_sFriends;
	std::set<const CRace*> m_sEnemies;
	CArray<CShips*> m_vInvolvedShips;
	double m_dWinningChance;

	int m_nPageFriends;							///< Seite für alle befeundeten Schiffe in der Befehlsansicht
	int m_nPageEnemies;							///< Seite für alle feindlichen Schiffe in der Befehlsansicht
	vector<pair<CRect, CShips*> > m_vShipRects;	///< alle angezeigten Schiffe mit ihren Rechtecken zu draufklicken
	CShips* m_pMarkedShip;						///< aktuell markiertes Schiff
	CRect m_rLastMarkedRect;					///< zuletzt markiertes Rechteck
	int m_nShipType;							///< nur Schiffstypen dieses Types werden angezeigt
};
