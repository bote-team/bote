/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CChooseRacePage dialog
class CChooseRacePage;

#include "MainDlg.h"

/**
 * PropertyPage, die die Auswahl eines Volkes erlaubt.
 *
 * @author CBot
 * @version 0.0.1
 */
class CChooseRacePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CChooseRacePage)

private:
	CIPAddressCtrl m_hostIP;
	CEdit m_hostPort;
	CMainDlg *m_pParent;

	bool m_bIsServer;					///< <code>true</code>, wenn auf dem lokalen Rechner der Server läuft, sonst <code>false</code>
	int m_nPlayerCount;					///< Anzahl der Spieler, die ein Volk gewählt haben

public:
	CChooseRacePage();
	virtual ~CChooseRacePage();

	void SetServer(bool bIsServer = true) {m_bIsServer = bIsServer;}
	bool IsServer() const {return m_bIsServer;}

	void EnableRaceButtons();

	void AddChatMsg(CString name, CString msg);

// Dialog Data
	enum { IDD = IDD_CHOOSERACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void EnableWizardButtons();
	void EnableServerPlayerButtons();
	BOOL CheckValues();

	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	
	afx_msg void OnChooseRace(UINT nID);
	afx_msg void OnSetPlayByServer(UINT nID);
	virtual LRESULT OnWizardNext();
	afx_msg void OnEnChangeMsg();
	afx_msg BOOL OnWizardFinish();
};
