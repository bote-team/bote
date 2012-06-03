/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once
#include "CRoundButton2/roundbuttonstyle.h"
#include "CRoundButton2/roundbutton2.h"

typedef enum MODE {MODE_CLIENT, MODE_SERVER, MODE_LOAD} MODE;

// CNewGameView-Formularansicht

class CNewGameView : public CFormView
{
	DECLARE_DYNCREATE(CNewGameView)

protected:
	CNewGameView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	virtual ~CNewGameView();

	// This object includes the project wide style of the buttons
	CRoundButtonStyle m_tButtonStyle;

	CRoundButton2	m_btNext;
	CRoundButton2	m_btBack;

	CIPAddressCtrl m_hostIP;
	CRoundButton2 m_btSearchServers;
	CButton m_btPublish;
	CEdit m_hostPort;
	CEdit m_serverPort;
	CEdit m_username;
	CEdit m_description;
	CString m_sFileName;				///< Datei welche geladen werden soll

	MODE m_nMode;

	Bitmap* m_pBkgndImg;				///< Hintergrundgrafik
	CSize	m_TotalSize;				///< Größe der View

	BOOL CheckValues();

public:
	enum { IDD = IDD_NEWGAMEVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	void SetMode(MODE mode);
	void StartServer(bool bPublish);
	void EnableNextButton();
	void ShowChooseRaceView(bool bIsServer) const;
	void LoadGame(const CString& sPath);

private:
	/// Den Style für die Button-Controls setzen
	/// @pBtn Zeiger auf Button
	void SetButtonStyle(CRoundButton2* pBtn);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedBack();
	afx_msg void OnEnChangeHostport();
	afx_msg void OnEnChangeServerport();
	afx_msg void OnBnClickedPublish();
	afx_msg void OnEnChangeUsername();
	afx_msg void OnEnChangeDescription();
	afx_msg void OnBnClickedSearchservers();

	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* /*pDC*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


