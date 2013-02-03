/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#if !defined(AFX_CMenuChooseView_H__A7C9E84F_C945_4A52_9413_10EAD7056967__INCLUDED_)
#define AFX_CMenuChooseView_H__A7C9E84F_C945_4A52_9413_10EAD7056967__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CMenuChooseView.h : Header-Datei
#include "MyButton.h"
#include "FontLoader.h"

/////////////////////////////////////////////////////////////////////////////
// Ansicht CMenuChooseView

class CMenuChooseView : public CView
{
protected:
	CMenuChooseView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CMenuChooseView)

// Attribute
	CPoint m_LastSystem;			///< letztes System, in welchem wir uns aufgehalten haben
	CArray<CMyButton*> m_Buttons;	///< Feld beinhaltet die ganzen Menübuttons in dieser Ansicht
	CMyButton *m_RoundEnd;			///< der Rundenende Button
	CSize m_TotalSize;				///< Größe der View in logischen Koordinaten
	static CMajor* m_pPlayersRace;	///< Zeiger auf Spielerrasse
	
// Operationen
public:
	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von der View ausgeführt werden müssen.
	void OnNewRound();

	/// Funktion lädt die rassenspezifischen Grafiken.
	void LoadRaceGraphics();

	/// Funktion zum Festlegen der Spielerrasse in der View
	/// @pPlayer Zeiger auf Spielerrasse
	static void SetPlayersRace(CMajor* pPlayer) {m_pPlayersRace = pPlayer;}

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CMenuChooseView)
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CMenuChooseView();
	void CalcLogicalPoint(CPoint &point);
	void CalcDeviceRect(CRect &rect);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	bool m_bKnowOtherMajors;	///< Sind andere Majors bekannt?
	bool m_bKnowOtherRaces;		///< Sind andere Rassen bekannt?
	float m_fMark;				///< Bewertung des Spielers

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CMenuChooseView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CMenuChooseView_H__A7C9E84F_C945_4A52_9413_10EAD7056967__INCLUDED_
