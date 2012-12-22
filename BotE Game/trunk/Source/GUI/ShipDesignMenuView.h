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


// CShipDesignMenuView view

class CShipDesignMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CShipDesignMenuView)

protected:
	CShipDesignMenuView();           // protected constructor used by dynamic creation
	virtual ~CShipDesignMenuView();

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

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

private:
	// Funktionen
	/// Funkion legt alle Buttons für die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen des Schiffsdesignmenüs
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawShipDesignMenue(Graphics* g);

	/// Funktion überprüft ob das in der Designansicht angeklickte Schiff in einem unserer Systeme gerade gebaut wird
	/// Man benötigt diesen Check da man keine Schiffe ändern kann, welche gerade gebaut werden.
	/// @param pShipInfo Zeiger des zu prüfenden Schiffes aus der Schiffsliste
	/// @return CString mit dem Namen des Systems, wird das Schiff nirgends gebaut ist der String leer
	CString CheckIfShipIsBuilding(const CShipInfo* pShipInfo) const;

	// Attribute

	// Grafiken
	Bitmap* bg_designmenu;			///< Schiffsdesignmenu

	// Buttons

	// Hier die Variablen, die wir benötigen, wenn wir in der Schiffsdesignansicht sind
	short  m_iClickedOnShip;			// auf welches Schiff wurde in der Designansicht geklickt
	short  m_iOldClickedOnShip;			// auf welches Schiffe wurde vorher geklickt, braucht man als Modifikator
	short  m_nSizeOfShipDesignList;		///< Anzahl der Einträge in der Schiffsdesignliste
	USHORT m_iBeamWeaponNumber;			// Nummer der Beamwaffe
	USHORT m_iTorpedoWeaponNumber;		// Nummer der Torpedowaffe
	BOOLEAN m_bFoundBetterBeam;			// Gibt es einen besseren Beamtyp
	BOOLEAN m_bFoundWorseBeam;			// Gibt es einen schlechteren Beamtyp
	CShipInfo* m_pShownShip;			// derzeit angezeigtes Schiff

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


