/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afxwin.h"

#include "LANClient.h"
#include "afxcmn.h"

// CSearchServersDlg dialog

typedef struct DLG_LISTENTRY
{
	CString strDescription;
	DWORD dwIP;
	UINT nPort;
}
DLG_LISTENTRY;

class CSearchServersDlg : public CDialog, public network::CLANClientListener
{
	DECLARE_DYNAMIC(CSearchServersDlg)

private:
	CList<DLG_LISTENTRY> m_lEntries;
	CListCtrl m_listServers;

public:
	CString m_strDescription;
	DWORD m_dwIP;
	UINT m_nPort;

public:
	CSearchServersDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSearchServersDlg();

// Dialog Data
	enum { IDD = IDD_SEARCHSERVERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	virtual void OnServerFound(const CString &strDescription, DWORD dwIP, int nTCPPort);

protected:
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnNMDblclkServers(NMHDR *pNMHDR, LRESULT *pResult);

public:
public:
	afx_msg void OnNMClickServers(NMHDR *pNMHDR, LRESULT *pResult);
};
