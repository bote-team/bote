/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BottomBaseView.h"

// CShipDesignBottomView view

class CShipDesignBottomView : public CBottomBaseView
{
	DECLARE_DYNCREATE(CShipDesignBottomView)

protected:
	CShipDesignBottomView();           // protected constructor used by dynamic creation
	virtual ~CShipDesignBottomView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	/// Funktion gibt die Nummer der angezeigten Schiffsinfo zurück.
	/// @return Nummer der angezeigten Schiffsinfo
	int GetCurrentShipInfo() const { return m_nCurrentShipInfo; }

	/// Funktion legt fest, zu welchem Schiff die Informationen angezeigt werden
	/// @param nShipInfo Nummer der Schiffsinfo
	void SetCurrentShipInfo(int nShipInfo) { m_nCurrentShipInfo = nShipInfo; }

private:
	int m_nCurrentShipInfo;	///< Welche Schiffsinfo soll angezeigt werden

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


