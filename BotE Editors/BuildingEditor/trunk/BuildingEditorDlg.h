// BuildingEditorDlg.h : Header-Datei
//

#if !defined(AFX_BUILDINGEDITORDLG_H__C633D525_E83E_4043_8F91_F5E3FE9A478D__INCLUDED_)
#define AFX_BUILDINGEDITORDLG_H__C633D525_E83E_4043_8F91_F5E3FE9A478D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileReader.h"
#include "ColorListBox.h"
#include "dyntabctrl.h"
#include "InfoDlg.h"
#include "PrerequisitesDlg.h"
#include "ProductionDlg.h"
#include "SpecialDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CBuildingEditorDlg Dialogfeld

class CBuildingEditorDlg : public CDialog
{
// Konstruktion
public:
	/// Standardkonstruktor
	CBuildingEditorDlg(CWnd* pParent = NULL);	

	/// Standarddestruktor
	virtual ~CBuildingEditorDlg();

	// Zugriffsfunktionen
	CBuildingInfo* GetClickedBuilding() {return &m_BuildingInfo[m_iClick];}
//	USHORT GetClick() const {return m_iClick;}
//	int GetListSel() const {return m_ListBox.GetCurSel();}
	CArray<CBuildingInfo, CBuildingInfo>* GetBuildings() {return &m_BuildingInfo;}
//	int GetLanguage() const {return m_Language.GetCurSel();}



// Dialogfelddaten
	//{{AFX_DATA(CBuildingEditorDlg)
	enum { IDD = IDD_BUILDINGEDITOR_DIALOG };
	CComboBox	m_Language;
	CColorListBox	m_ListBox;
	UINT	m_iRunningNumber;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CBuildingEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;
	CFileReader* fileReader;
	CArray<CBuildingInfo,CBuildingInfo> m_BuildingInfo;
	USHORT m_iClick;
	int color[8];
	bool m_bAutoSorting;


	// Memberfunktionen
	void DialogToData();
	void DataToDialog();
	void AutoSort();
		
	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CBuildingEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeList();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnDelete();
	afx_msg void OnNew();
	afx_msg void OnCopy();
	afx_msg void OnMoveup();
	afx_msg void OnMovedown();
	afx_msg void OnMoveupTen();
	afx_msg void OnMovedownTen();
	afx_msg void OnSelchangeLanguage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	DynTabCtrl m_dynTabCtrl;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_BUILDINGEDITORDLG_H__C633D525_E83E_4043_8F91_F5E3FE9A478D__INCLUDED_)
