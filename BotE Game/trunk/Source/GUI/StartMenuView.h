/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once

#include "BotEServer.h"
#include "BotEClient.h"

#include "CRoundButton2/roundbuttonstyle.h"
#include "CRoundButton2/roundbutton2.h"

// forward declaration
class CChooseRaceView;
class CNewGameView;

// CStartMenuView-Formularansicht

class CStartMenuView : public CFormView, public network::CClientListener, public network::CServerListener
{
	DECLARE_DYNCREATE(CStartMenuView)

protected:
	CStartMenuView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	virtual ~CStartMenuView();

	// This object includes the project wide style of the buttons
	CRoundButtonStyle m_tButtonStyle;

	CRoundButton2 m_btNewGame;
	CRoundButton2 m_btMultiplayer;
	CRoundButton2 m_btLoadGame;
	CRoundButton2 m_btOptions;
	CRoundButton2 m_btExit;

	CRoundButton2 m_btMPClient;
	CRoundButton2 m_btMPServer;

	Bitmap* m_pBkgndImg;				///< Hintergrundgrafik
	CSize	m_TotalSize;				///< Größe der View

	int m_nTimeCounter;					///< Zähler für Timer
	int m_nLogoTimer;					///< Zähler für Logo

	CChooseRaceView* m_pChooseRaceView;	///< Zeiger auf Rassenauswahlview
	CNewGameView* m_pNewGameView;		///< Zeiger auf Spielstartansicht

public:
	enum { IDD = IDD_STARTMENUVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC* /*pDC*/);

	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedNewGame();
	afx_msg void OnBnClickedMultiplayer();
	afx_msg void OnBnClickedLoadGame();
	afx_msg void OnBnClickedOptions();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedCreateMP();
	afx_msg void OnBnClickedJoinMP();

	afx_msg LRESULT OnUserMsg(WPARAM wParam, LPARAM lParam);

private:

	/// Den Style für die Button-Controls setzen
	/// @pBtn Zeiger auf Button
	void SetButtonStyle(CRoundButton2* pBtn);

	/// Sollen die Multiplayerbuttons (Erstellen und Beitreten) angezeigt werden
	/// @bShow <code>true</code> wenn MP-Buttons angezeigt werden soll, <code>false</code> wenn nicht
	void ShowMPButtons(bool bShow);

	// benötigte Handler für Server-Ereignisse
	virtual void OnServerSocketError(int nError);

	// benötigte Handler für Client-Ereignisse
	virtual void OnClientSocketError(int nError);
	virtual void OnClientChangedRaces();
	virtual void OnClientDisconnected();

	virtual void OnBeginGame(network::CBeginGame *pMsg);
	virtual void OnPrepareGame();
	virtual void OnNextRound(network::CNextRound *pMsg);
	virtual void OnCalculateGame();
	virtual void OnChatMsg(network::CChatMsg *pMsg);

	virtual void OnClientLost(const CString &) {}
};


