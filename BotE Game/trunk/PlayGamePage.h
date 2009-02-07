/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

class CPlayGamePage;

#include "MainDlg.h"
#include "ChatMsg.h"

// CPlayGamePage dialog

/**
 * PropertyPage, die den Spielablauf darstellt.
 *
 * @author CBot
 * @version 0.0.1
 */
class CPlayGamePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPlayGamePage)

private:
	CMainDlg *m_pParent;
	UINT m_nState;
	UINT m_nRound;

public:
	CPlayGamePage();   // standard constructor
	virtual ~CPlayGamePage();

// Dialog Data
	enum { IDD = IDD_PLAYING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void EnableWizardButtons();
	void UpdateControls();

	void EnableReceiverCheckBoxes(BOOL bEnabled = TRUE);

public:
	afx_msg void OnBnClickedEndOfRound();

	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();

	void NextRound(UINT nRound);
	void AddChatMsg(network::CChatMsg *pMsg);
	void AddChatMsg(CString msg);

private:
	void AddChatMsg(CString name, network::RACE race, CString msg);

public:
	afx_msg void OnBnClickedToall();
public:
	afx_msg void OnEnChangeMsg();
};
