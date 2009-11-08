#if !defined(AFX_MODIFYBEAMDLG_H__26FC6B80_4378_488E_8AAF_022461B1D050__INCLUDED_)
#define AFX_MODIFYBEAMDLG_H__26FC6B80_4378_488E_8AAF_022461B1D050__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyBeamDlg.h : Header-Datei
//
#include "ShipInfo.h"
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CModifyBeamDlg 

class CModifyBeamDlg : public CDialog
{
// Konstruktion
public:
	CModifyBeamDlg(CArray<CShipInfo*>* pShipInfos, int nClickedShip, int nClickedBeam, CWnd* pParent = NULL);   // Standardkonstruktor
		
// Dialogfelddaten
	//{{AFX_DATA(CModifyBeamDlg)
	enum { IDD = IDD_MODIFYBEAM_DIALOG };
	CComboBox m_CtrlBeamList;
	UINT	m_iBeamDamage;
	CString	m_strBeamName;
	UINT	m_iBeamNumber;
	UINT	m_iBeamType;
	BOOL	m_bModulating;
	BYTE	m_byBeamtime;
	BYTE	m_byBonus;
	BYTE	m_byRechargeTime;
	BYTE	m_byShootNumber;
	BOOL	m_bPiercing;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CModifyBeamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CModifyBeamDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Attribute
private:
	// Attribute
	CArray<CShipInfo*>* m_pShipInfos;
	int m_nClickedShip;
	int m_nClickedBeam;
public:
	afx_msg void OnCbnSelchangeBeamname();
	// Anbringung der Waffe auf dem Schiff <code>FRONT, RIGHT, BACK, LEFT</code>
	short m_nMountPos;
	// Schusswinkel der Waffe (zwischen 0° und 360°) kegelförmig
	short m_nAngle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_MODIFYBEAMDLG_H__26FC6B80_4378_488E_8AAF_022461B1D050__INCLUDED_
