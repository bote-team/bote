#if !defined(AFX_MODIFYTUBEDLG_H__E63F2B95_3254_4B7C_BE6C_263C527A4B2E__INCLUDED_)
#define AFX_MODIFYTUBEDLG_H__E63F2B95_3254_4B7C_BE6C_263C527A4B2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyTubeDlg.h : Header-Datei
//
#include "ShipInfo.h"
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CModifyTubeDlg 

class CModifyTubeDlg : public CDialog
{
// Konstruktion
public:
	CModifyTubeDlg(CArray<CShipInfo*>* pShipInfos, int nClickedShip, int nClickedTube, CWnd* pParent = NULL);   // Standardkonstruktor
		
// Dialogfelddaten
	UINT m_iTorpedoType;
	//{{AFX_DATA(CModifyTubeDlg)
	enum { IDD = IDD_MODIFYTUBE_DIALOG };
	CComboBox	m_TubeList;
	CListBox	m_TorpedoTypeList;
	UINT	m_iTorpedoNumber;
	UINT	m_iTubeNumber;
	UINT	m_iTubeFirerate;
	CString	m_strTubeName;
	BOOL	m_bOnlyMicroPhoton;
	BYTE	m_byAccuracy;
	BOOL	m_bPenetrating;
	BOOL	m_bCollapseShields;
	BOOL	m_bDoubleHullDmg;
	BOOL	m_bDoubleShieldDmg;
	BOOL	m_bIgnoreAllShields;
	BOOL	m_bReduceManeuver;
	BOOL	m_bMicro;

	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CModifyTubeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CModifyTubeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTorpedotype();
	afx_msg void OnSelchangeTubetype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attribute
	CArray<CShipInfo*>* m_pShipInfos;
	int m_nClickedShip;
	int m_nClickedTube;
public:
	// Anbringung der Waffe auf dem Schiff <code>FRONT, RIGHT, BACK, LEFT</code>
	short m_nMountPos;
	// Schusswinkel der Waffe (zwischen 0° und 360°) kegelförmig
	short m_nAngle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_MODIFYTUBEDLG_H__E63F2B95_3254_4B7C_BE6C_263C527A4B2E__INCLUDED_
