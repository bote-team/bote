#pragma once
#include "BuildingInfo.h"

// CInfoDlg dialog

class CInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInfoDlg)

public:
	CInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoDlg();

	// Memberfunktionen
	virtual void DialogToData(CBuildingInfo* building, int language);
	virtual void DataToDialog(CBuildingInfo* building, int language);

// Dialog Data
	enum { IDD = IDD_INFODLG };
	CComboBox	m_RaceComboBox;
	CString	m_strName;
	CString	m_strDescription;
	CString	m_strGraphicfile;
	BOOL	m_bAllwayOnline;
	UINT	m_iCarEqu;
	UINT	m_iDomEqu;
	UINT	m_iFedEqu;
	UINT	m_iFerEqu;
	UINT	m_iKliEqu;
	UINT	m_iRomEqu;
	CString m_strEquiName1;
	CString m_strEquiName2;
	CString m_strEquiName3;
	CString m_strEquiName4;
	CString m_strEquiName5;
	CString m_strEquiName6;
	BOOL	m_bNeedWorker;
	BOOL	m_bNeverReady;
	UINT	m_iPredecessorID;
	BOOL	m_bUpgradeable;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSelchangeRace();
	DECLARE_MESSAGE_MAP()	
};
