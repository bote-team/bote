/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BottomBaseView.h"

// CPlanetBottomView view

class CPlanetBottomView : public CBottomBaseView
{
	DECLARE_DYNCREATE(CPlanetBottomView)

protected:
	CPlanetBottomView();           // protected constructor used by dynamic creation
	virtual ~CPlanetBottomView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	
	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	// Funktionen

	// Attribute
	vector<CRect> m_vPlanetRects;	///< Rechteck um die Planeten um Mausaktionen prüfen zu können

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


