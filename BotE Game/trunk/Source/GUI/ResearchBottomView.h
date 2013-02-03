/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BottomBaseView.h"

// CResearchBottomView view

class CResearchBottomView : public CBottomBaseView
{
	DECLARE_DYNCREATE(CResearchBottomView)

protected:
	CResearchBottomView();           // protected constructor used by dynamic creation
	virtual ~CResearchBottomView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	/// Funktion gibt die Nummer des angezeigten Techgebietes zurück.
	/// @return Nummer des angezeigten Techgebietes
	int GetCurrentTech() const { return m_nCurrentTech; }

	/// Funktion legt fest, zu welchem Techgebiet die Informationen angezeigt werden
	/// @param nTech Nummer des Techgebiets
	void SetCurrentTech(int nTech) { m_nCurrentTech = nTech; }

private:
	int m_nCurrentTech;	///< Welche Tech soll angezeigt werden?

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


