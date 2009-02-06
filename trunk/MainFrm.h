/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__A9311EEF_D13E_46E6_BEB6_62C2ADEB9F30__INCLUDED_)
#define AFX_MAINFRM_H__A9311EEF_D13E_46E6_BEB6_62C2ADEB9F30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Verhältnis der Splitterviews zur Gesamtview, so dass auf allen Auflösung das Spiel gleich aussieht.
// Eine feste Größe der Splitters kann dann nicht vorgegeben werden
#define HORZ_PROPORTION 0.15625
#define VERT_PROPORTION	0.732421875

#include "MultiSplitterView.h"

class CMainFrame : public CFrameWnd
{
	
protected: // Nur aus Serialisierung erzeugen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)	

// Operationen
public:
	/// Funktion liefert die ID der gerade angezeigten View in einem bestimmten SplitterWindows.
	/// @param paneRow Nummer der Splitter-Zeile
	/// @param paneRow Nummer der Splitter-Spalte
	/// @return ID der aktiven View (<code>-1</code> wenn keine View aktive oder sonstiger Fehler)
	short GetActiveView(int paneRow, int paneCol) const;

	/// Funktion gibt die Klasse einer View zurück.
	/// @param className Klassenname der neuzuzeichnenden View
	/// @return Zeiger auf Viewobjekt
	CView* GetView(const CRuntimeClass* className) const;

	/// Funktion veranlasst eine View neu zu zeichnen.
	/// @param className Klassenname der neuzuzeichnenden View
	void InvalidateView(const CRuntimeClass* className);

	/// Funktion veranlasst eine View neu zu zeichnen.
	/// @param viewID ID der neuzuzeichnenden View
	void InvalidateView(USHORT viewID);

	/// Funktion setzt das Untermenü einer bestimmten View
	/// @param viewClassName Klassenname der View
	/// @param menuID Untermenü-ID der View
	void SetSubMenu(const CRuntimeClass* viewClassName, BYTE menuID);

	/// Funktion gibt das gerade aktive Untermenü einer View zurück
	/// @param viewClassName Klassenname der View
	/// @return ID des gerade aktiven Untermenüs
	BYTE GetSubMenu(const CRuntimeClass* viewClassName) const;

	/// Funktion legt die View für jede Rasse fest, also in welchem Menü man sich gerade befindet.
	/// @param whichView Nummer (ID) der View
	/// @param race auf welche Hauptrasse bezieht sich die Änderung
	void SelectMainView(USHORT whichView, BYTE race);

	/// Funktion wählt eine View für den unteren View-Bereich aus.
	/// @param viewID View-ID der gewünschten View
	void SelectBottomView(USHORT viewID);

	/// Funktion setzt das Splitterwindow mit der MainView auf die volle Fenstergröße.
	/// @param fullScrenn <code>true</code> für Fullscreenmodus, <code>false</code> für normalen Modus.
	void FullScreenMainView(bool fullScreen);

	/// Funktion gibt das Splitterwindow zurück
	/// @return Zeiger auf Splitterwindow
	CMultiSplitterView* GetSplitterWindow() {return &m_wndSplitter;}

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CDialogBar  m_wndDlgBar;
	CMultiSplitterView m_wndSplitter;

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MAINFRM_H__A9311EEF_D13E_46E6_BEB6_62C2ADEB9F30__INCLUDED_)
