/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "BottomBaseView.h"

// CDiplomacyBottomView view

class CDiplomacyBottomView : public CBottomBaseView
{
	DECLARE_DYNCREATE(CDiplomacyBottomView)

protected:
	CDiplomacyBottomView();           // protected constructor used by dynamic creation
	virtual ~CDiplomacyBottomView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:

	/// Funktion legt die Überschrift der diplomatischen Nachricht fest
	/// @param text Text
	static void SetHeadLine(CString text) {m_strHeadLine = text;}

	/// Funktion legt den Nachrichtentext der diplomatischen Nachricht fest
	/// @param text Text
	static void SetText(CString text) {m_strText = text;}

	/// Funktion gibt die Nachrichtenüberschrift zurück.
	/// @return Überschrift
	static CString GetHeadLine(void) {return m_strHeadLine;}

	/// Funktion gibt den Nachrichtentext zurück.
	/// @return Nachrichtentext
	static CString GetText(void) {return m_strText;}

private:
	static CString m_strHeadLine;	///< Überschrift der diplomatischen Nachricht
	static CString m_strText;		///< Text der diplomatischen Nachricht



protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


