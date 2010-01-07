/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#if !defined(AFX_DBSCROLLVIEW_H__5990A9BB_C42A_476B_98CA_7FD49B4C160A__INCLUDED_)
#define AFX_DBSCROLLVIEW_H__5990A9BB_C42A_476B_98CA_7FD49B4C160A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBScrollView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDBScrollView view

/**
 * ScrollView, die Double Buffering unterstützt. Ist m_bUpdateOnly == TRUE wird der ursprüngliche Fensterinhalt
 * nicht gelöscht; damit ist es möglich nur die Änderungen zu zeichnen (ohne Double Buffering).
 */
class CDBScrollView : public CScrollView
{
protected:
	CDBScrollView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDBScrollView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBScrollView)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// TRUE, wenn nur Änderungen gezeichnet werden sollen (kein Double Buffering), sonst FALSE;
	// standardmäßig FALSE; wird nach dem Aufruf von OnDraw(CDC*) wieder auf FALSE gesetzt
	BOOL m_bUpdateOnly;
	// Hintergrundfarbe, mit der die View bei Double Buffering  vor OnDraw(CDC*) gefüllt werden soll
	COLORREF m_colorBackground;

	virtual void OnDraw(CDC* pDC);
	virtual ~CDBScrollView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDBScrollView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBSCROLLVIEW_H__5990A9BB_C42A_476B_98CA_7FD49B4C160A__INCLUDED_)
