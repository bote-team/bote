// BuildingEditor.h : Haupt-Header-Datei für die Anwendung BUILDINGEDITOR
//

#if !defined(AFX_BUILDINGEDITOR_H__D3C1AE19_CA02_4978_8590_D4B8B648E098__INCLUDED_)
#define AFX_BUILDINGEDITOR_H__D3C1AE19_CA02_4978_8590_D4B8B648E098__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole
#include "BuildingEditorDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CBuildingEditorApp:
// Siehe BuildingEditor.cpp für die Implementierung dieser Klasse
//

class CBuildingEditorApp : public CWinApp
{
public:
	/// Standardkonstruktor
	CBuildingEditorApp();
	
	/// Standarddestruktor
	virtual ~CBuildingEditorApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CBuildingEditorApp)
	public:
	virtual BOOL InitInstance();
	CBuildingEditorDlg* GetMainDlg() const {return m_pDlg;}
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CBuildingEditorApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBuildingEditorDlg *m_pDlg;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_BUILDINGEDITOR_H__D3C1AE19_CA02_4978_8590_D4B8B648E098__INCLUDED_)
