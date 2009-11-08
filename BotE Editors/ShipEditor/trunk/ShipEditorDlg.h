// ShipEditorDlg.h : Header-Datei
//

#if !defined(AFX_SHIPEDITORDLG_H__3E434A2E_0897_4295_8511_B1E21AFA76BC__INCLUDED_)
#define AFX_SHIPEDITORDLG_H__3E434A2E_0897_4295_8511_B1E21AFA76BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NewClassNameDlg.h"
#include "ShowStatsDlg.h"
#include "FileReader.h"
#include "ColorListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CShipEditorDlg Dialogfeld

class CShipEditorDlg : public CDialog
{
// Konstruktion
public:
	CShipEditorDlg(CWnd* pParent = NULL);	// Standard-Konstruktor
	
// Dialogfelddaten
	//{{AFX_DATA(CShipEditorDlg)
	enum { IDD = IDD_SHIPEDITOR_DIALOG };
	CComboBox	m_Special2;
	CComboBox	m_Special1;
	CComboBox	m_CtrlObsolets;
	CListBox	m_TorpedoList;
	CListBox	m_BeamList;
	CColorListBox	m_ListBox;
	CComboBox	m_iComboShipType;
	CString	m_strShipDescription;
	CString	m_strShipType;
	UINT	m_iBioTech;
	UINT	m_iComputerTech;
	UINT	m_iConstructionTech;
	UINT	m_iEnergyTech;
	UINT	m_iPropulsionTech;
	UINT	m_iWeaponsTech;
	UINT	m_iIndustry;
	UINT	m_iIridium;
	UINT	m_iDeuterium;
	UINT	m_iCrystal;
	UINT	m_iDilithium;
	UINT	m_iDuranium;
	UINT	m_iTitan;
	int		m_iHullMaterial;
	UINT	m_iBaseHull;
	BOOL	m_bDoubleHull;
	UINT	m_iMaxShield;
	UINT	m_iShieldType;
	int		m_iRange;
	UINT	m_iSpeed;
	UINT	m_iScanpower;
	UINT	m_iScanRange;
	UINT	m_iStorageRoom;
	UINT	m_iStationbuildPoints;
	UINT	m_iColonizationPoints;
	UINT	m_iCamouflagePower;
	UINT	m_iMaintenanceCosts;
	int		m_iRace;
	CString	m_strShipClass;
	CString	m_strOnlyOnSystem;
	BYTE	m_byShipSize;
	BYTE	m_byManeuverability;
	BOOL	m_bAblative;
	BOOL	m_bRegenerative;
	CString	m_strObsolete;
	BOOL	m_bPolarisation;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CShipEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;
	CNewClassNameDlg m_NewClassNameDlg;
	CShowStatsDlg m_ShowStatsDlg;
	
	CFileReader* fileReader;
	CArray<CShipInfo*> m_ShipInfo;
	
	int m_iShipType;
	int m_iClick;

	// verschiedene Schriftfarben für die ganzen Rassen
	COLORREF color[8];
	
	void DataToDialog();
	void DialogToData();

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CShipEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnSelchangeShipClassList();
	afx_msg void OnSelchangeBeamlist();
	afx_msg void OnModifybeam();
	afx_msg void OnAddbeam();
	afx_msg void OnDeletebeam();
	afx_msg void OnModifytube();
	afx_msg void OnSelchangeTorpedolist();
	afx_msg void OnDeletetube();
	afx_msg void OnAddtube();
	afx_msg void OnNewclass();
	afx_msg void OnDeleteclass();
	afx_msg void OnShowstats();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SHIPEDITORDLG_H__3E434A2E_0897_4295_8511_B1E21AFA76BC__INCLUDED_)
