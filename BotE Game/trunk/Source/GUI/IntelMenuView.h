/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BotEDoc.h"
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
	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgeführt werden müssen.
	virtual void OnNewRound(void);

	/// Funktion lädt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

	void SetSubMenu(BYTE menuID) {m_bySubMenu = menuID;}

	BYTE GetSubMenu() const {return m_bySubMenu;}

private:
	// Funktionen
	/// Funkion legt alle Buttons für die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zeichnet die kleinen Rassensymbole in den Geheimdienstansichten
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param highlightPlayersRace soll die eigene Rasse aktiviert dargestellt werden
	void DrawRaceLogosInIntelView(Graphics* g, BOOLEAN highlightPlayersRace = FALSE);

	/// Funktion zeichnet die Geheimdienstinformationen an den rechten Rand der Geheimdienstansichten
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param font Zeiger auf zu benutzende Schriftart
	/// @param color Farbe für Schrift
	void DrawIntelInformation(Graphics* g, Gdiplus::Font* font, Gdiplus::Color color);

	/// Funktion zeichnet die Buttons unter den Intelmenüs.
	/// @param g Zeiger auf GDI+ Grafikobjekt
	/// @param pMajor Spielerrasse
	void DrawIntelMainButtons(Graphics* g, CMajor* pMajor);

	/// Funktion zeichnet das Geheimdienstmenü, in welchem man die globale prozentuale Zuteilung machen kann.
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawIntelAssignmentMenu(Graphics* g);

	/// Funktion zeichnet das Spionagegeheimdienstmenü.
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawIntelSpyMenu(Graphics* g);

	/// Funktion zeichnet das Sabotagegeheimdienstmenü.
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawIntelSabotageMenu(Graphics* g);

	/// Funktion zeichnet das Menü mit den Geheimdienstberichten.
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawIntelReportsMenu(Graphics* g);

	/// Funktion zeichnet das Menü für die Geheimdienstanschläge.
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawIntelAttackMenu(Graphics* g);

	/// Funktion zeichnet das Menü für die Geheimdienstinformationen
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawIntelInfoMenu(Graphics* g);

	// Attribute
	// Grafiken
	Bitmap* bg_intelassignmenu;		///< globales Geheimdienstzuweisungsmenü
	Bitmap* bg_intelspymenu;		///< Spionagegeheimdienstmenü
	Bitmap* bg_intelsabmenu;		///< Sabotagegeheimdienstmenü
	Bitmap* bg_intelreportmenu;		///< Geheimdienstnachrichtenmenü
	Bitmap* bg_intelinfomenu;		///< Geheimdienstinformationsmenü
	Bitmap* bg_intelattackmenu;		///< Geheimdienstanschlagsmenü

	// Buttons
	CArray<CMyButton*> m_IntelligenceMainButtons;	///< die Buttons unter den Geheimdienstmenüs
	CArray<CMyButton*> m_RaceLogoButtons;			///< Buttons um Rassenlogoleiste durchzuschalten

	// Hier die Variablen für die Geheimdienstansicht
	BYTE m_bySubMenu;						///< welches Untermenü im Geheimdienstmenü wurde aktiviert
	CString m_sActiveIntelRace;				///< auf welche gegnerische Rasse beziehen sich die Intelaktionen
	short m_iOldClickedIntelReport;			///< auf welchen Bericht wurde vorher geklickt, braucht man als Modifikator
	bool m_bSortDesc[4];					///< bei Klick zum Sortieren soll wie sortiert werden?
	int m_nScrollPos;						///< ab welcher Rasse sollen die Logos gezeichnet werden (es passen ja immer nur 6 hin)

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


