/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#if !defined(AFX_CSmallInfoView_H__ACC3A8E0_2540_4B19_91C2_7274772391FB__INCLUDED_)
#define AFX_CSmallInfoView_H__ACC3A8E0_2540_4B19_91C2_7274772391FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#include "FontLoader.h"
#include "Galaxy\Planet.h"
#include "Ships/Ships.h"
#endif // _MSC_VER > 1000
// CSmallInfoView.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Ansicht CSmallInfoView

class CSmallInfoView : public CView
{
public:
	enum DISPLAY_MODE
	{
		DISPLAY_MODE_SHIP_BOTTEM_VIEW,//sollen Schiffsinformationen angezeigt werden (that ship is in SHIP_BOTTEM_VIEW)
		DISPLAY_MODE_FLEET_MENU_VIEW,//sollen Schiffsinformationen angezeigt werden (that ship is in FLEET_MENU_VIEW)
		DISPLAY_MODE_PLANET_INFO,//< sollen Planeteninformationen angezeigt werden
		DISPLAY_MODE_PLANET_STATS,//< sollen Planetenstatistiken angezeigt werden
		DISPLAY_MODE_ICON//otherwise, show the major race's icon
	};
	//
protected:

	CSmallInfoView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CSmallInfoView)

// Attribute
	CSize m_TotalSize;					///< Größe der View in logischen Koordinaten
	static CPlanet* m_pPlanet;			///< Planet über den Informationen bzw. Statistiken angezeigt werden sollen
	int	m_nTimer;						///< Variable welche per Timerbefehl hochgezählt wird
	bool m_bAnimatedIcon;				///< soll das Rassensymbol animiert werden
	static CSmallInfoView::DISPLAY_MODE m_DisplayMode;

public:

	//Funktion legt fest, which informations are to be displayed
	static void SetDisplayMode(CSmallInfoView::DISPLAY_MODE mode);

	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgeführt werden müssen.
	void OnNewRound(void) {};

	/// Funktion legt den anzuzeigenden Planeten fest.
	/// @param planet Zeiger auf den anzuzeigenden Planeten
	static void SetPlanet(CPlanet* planet) { m_pPlanet = planet; }

	/// Funktion liefert einen Zeiger auf den aktuell angezeigten Planeten.
	/// @return Zeiger auf Planeten
	static const CPlanet* GetPlanet(void) { return m_pPlanet; }

private:
	const CShipMap::const_iterator& GetShip(const CBotf2Doc& doc);
protected:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSmallInfoView)
	protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CSmallInfoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	/// Funktion rechnet einen übergebenen Punkt in logische Koordinaten um.
	void CalcLogicalPoint(CPoint &point);

	const CString CheckPlanetClassForInfo(char PlanetClass);		// Wählt den richtigen Text anhand der Klasse des Planeten
	const CString CheckPlanetClassForInfoHead(char PlanetClass);	// Wählt die richtige Überschrift anhand der KLasse des Planeten

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CSmallInfoView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CSmallInfoView_H__ACC3A8E0_2540_4B19_91C2_7274772391FB__INCLUDED_
