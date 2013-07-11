// MinorRaceEditorDlg.h : Header-Datei
//

#if !defined(AFX_MINORRACEEDITORDLG_H__9E009911_E325_4082_9ACC_F012F5CFB08F__INCLUDED_)
#define AFX_MINORRACEEDITORDLG_H__9E009911_E325_4082_9ACC_F012F5CFB08F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FileReader.h"

/////////////////////////////////////////////////////////////////////////////
// CMinorRaceEditorDlg Dialogfeld

class CMinorRaceEditorDlg : public CDialog
{
// Konstruktion
public:
	CMinorRaceEditorDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CMinorRaceEditorDlg)
	enum { IDD = IDD_MINORRACEEDITOR_DIALOG };
	CComboBox	m_TechProgress;
	CComboBox	m_Corruptibiliy;
	CListBox	m_ListBox;
	CString	m_strDescription;
	CString	m_strRaceName;
	BOOL	m_bSpaceflightNation;
	BOOL   m_bProperty[NUMBEROFKINDS];
	CString	m_strHomeSysName;
	CString	m_strGraphicName;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMinorRaceEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;
	CFileReader* fileReader;
	CArray<CMinorRace,CMinorRace> m_MinorInfo;
	int m_iClick;

	void DataToDialog();
	void DialogToData();
	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CMinorRaceEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeList();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnDelete();
	afx_msg void OnNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MINORRACEEDITORDLG_H__9E009911_E325_4082_9ACC_F012F5CFB08F__INCLUDED_)
