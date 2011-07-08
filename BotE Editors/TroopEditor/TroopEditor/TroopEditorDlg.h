// TroopEditorDlg.h : Headerdatei
//
//TroopEditor by Revisor 2011
//
//based on classes by SirPustekuchen
//
//
#include "FileReader.h"
#pragma once


// CTroopEditorDlg-Dialogfeld
class CTroopEditorDlg : public CDialog
{
// Konstruktion
public:
	CTroopEditorDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_TROOPEDITOR_DIALOG };
	CString	m_strDescription;
	CString	m_strName;
	CString m_strOwner;
	CString m_strName2;
	CString m_strDescription2;
	BYTE m_byPower;
	BYTE m_byMaintenanceCosts;
	BYTE m_byNeededTechs[6];
	int m_iNeededResources[5];
	int m_iNeededIndustry;
	BYTE m_byID;
	int m_iSize;
	BYTE m_byMoralValue;
	CListBox m_ListBox;
	BOOL m_Stringtablelookup;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;
	CFileReader* fileReader;
	CArray<CTroopInfo,CTroopInfo> m_TroopInfo;
	int m_iClick;

	void DataToDialog();
	void DialogToData();
	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNew();
	afx_msg void OnSelchangeList();
	afx_msg void OnDelete();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnDestroy();
	afx_msg void OnClickedStrtabel();
};
