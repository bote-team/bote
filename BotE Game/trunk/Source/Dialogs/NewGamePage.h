/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CNewGamePage;

#include "MainDlg.h"

typedef enum MODE {MODE_CLIENT, MODE_SERVER, MODE_LOAD} MODE;

// CNewGamePage dialog

/**
 * PropertyPage, die das Starten des Servers bzw. Verbinden eines Clients erlaubt.
 *
 * @author CBot
 * @version 0.0.2
 *
 */
class CNewGamePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CNewGamePage)

private:
	CIPAddressCtrl m_hostIP;
	CEdit m_fileName;
	CButton *m_pClientBtn, *m_pServerBtn, *m_pLoadBtn, *m_pChooseBtn, *m_pPublish, *m_pSearch;
	CMainDlg *m_pParent;
	virtual BOOL OnSetActive();
	MODE m_nMode;

public:
	CNewGamePage();
	virtual ~CNewGamePage();

// Dialog Data
	enum { IDD = IDD_NEWGAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	virtual BOOL OnInitDialog();

	void SetMode(MODE mode);
	BOOL CheckValues();
	void EnableWizardButtons();

	afx_msg void OnBnClickedClient();
	afx_msg void OnBnClickedServer();
	afx_msg void OnChangedHostIP(NMHDR *pNMHDR, LRESULT *pResult);

	virtual LRESULT OnWizardNext();
	afx_msg void OnEnChangeUsername();
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedChoosefile();
	CEdit m_description;
	afx_msg void OnBnClickedPublish();
	afx_msg void OnEnChangeDescription();
	afx_msg void OnBnClickedSearchservers();
	afx_msg void OnEnChangeHostport();
	afx_msg void OnEnChangeServerport();
	CEdit m_hostPort;
	CEdit m_serverPort;
public:
	afx_msg void OnBnClickedShowoptionsdlg();
};
