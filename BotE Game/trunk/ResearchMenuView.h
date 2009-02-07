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


// CResearchMenuView view

class CResearchMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CResearchMenuView)

protected:
	CResearchMenuView();           // protected constructor used by dynamic creation
	virtual ~CResearchMenuView();

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

	/// Funktio zum Zeichnen des Forschungsmenüs
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawResearchMenue(CDC* pDC, CRect theClientRect);

	/// Funktion zum Zeichnen des Spezialforschungsmenüs
	/// @param pDC Zeiger auf den aktuellen Zeichenkontext
	/// @param theClientRect die komplette Zeichenfläche
	void DrawUniqueResearchMenue(CDC* pDC, CRect theClientRect);

	// Attribute

	// Buttons
	CArray<CMyButton*> m_ResearchMainButtons;		///< Buttons an der rechten Seite der Forschungsansichten

	// Grafiken
	CBitmap bg_researchmenu;				///< normales Forschungsmenü
	CBitmap bg_urmenu;						///< Spezialforschungsmenü
	CBitmap bg_emptyur;						///< wenn keine Spezialforschung zur Auswahl steht

	// Hier die Rechtecke zum Klicken in der Forschungsübersicht
	CRect LockStatusRect[7];				///< Rechteck zeigt den Lockstatus des jeweiligen Forschungsgebietes an
	CRect ResearchTimber[7][101];			///< 7 Balken für die ganzen Foschungsgebiete á 100 kleinen Rechtecke + 1
	BYTE m_bySubMenu;						///< Welcher Button im Forschungsmenue wurde gedrückt, 0 für normal, 1 für Unique

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
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


