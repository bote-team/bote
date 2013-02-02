/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#if !defined(AFX_MYSPLITTERWND_H__D3DF53B5_7F4E_4A70_A93D_338EBFB8EA4A__INCLUDED_)
#define AFX_MYSPLITTERWND_H__D3DF53B5_7F4E_4A70_A93D_338EBFB8EA4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MySplitterWnd.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CMySplitterWnd

class CMySplitterWnd : public CSplitterWnd
{
// Konstruktion
public:
	CMySplitterWnd();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CMySplitterWnd)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CMySplitterWnd();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CMySplitterWnd)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_MYSPLITTERWND_H__D3DF53B5_7F4E_4A70_A93D_338EBFB8EA4A__INCLUDED_
