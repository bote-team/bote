/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#include "afxwin.h"
#if !defined(AFX_STARTDIALOG_H__42D2E54A_3B58_4929_B02E_5060D7170A94__INCLUDED_)
#define AFX_STARTDIALOG_H__42D2E54A_3B58_4929_B02E_5060D7170A94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// STARTDIALOG.h : Header-Datei
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSTARTDIALOG 

class CSTARTDIALOG : public CDialog
{
// Konstruktion
public:
	CSTARTDIALOG(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSTARTDIALOG)
	enum { IDD = IDD_GAMESTART };
	int		m_iRace;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSTARTDIALOG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSTARTDIALOG)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_STARTDIALOG_H__42D2E54A_3B58_4929_B02E_5060D7170A94__INCLUDED_
