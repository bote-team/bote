/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

class CMainDlg;
#include "Resource.h"
#include "NewGamePage.h"
#include "ChooseRacePage.h"
#include "PlayGamePage.h"
#include "Network.h"
#include "PeerData.h"
#include "BotEServer.h"
#include "BotEClient.h"
class CBotEDoc;
// CMainDlg

/**
 * Testdialog, der das Starten des Servers, Verbinden eines Clients und die Auswahl des
 * zu spielenden Volkes erlaubt.
 *
 * @author CBot
 * @version 0.0.2
 */
class CMainDlg : public CPropertySheet, public network::CClientListener, public network::CServerListener
{
	DECLARE_DYNAMIC(CMainDlg)

private:
	CNewGamePage *m_pNewGame;
	CChooseRacePage *m_pChooseRace;
//	CPlayGamePage *m_pPlayGame;
	CBotEDoc *m_pDoc;

public:
	CMainDlg(CBotEDoc* pDoc);
	virtual ~CMainDlg();

	CBotEDoc *GetServerDocument() {return m_pDoc;}
	CBotEDoc *GetClientDocument() {return m_pDoc;}

protected:
	DECLARE_MESSAGE_MAP()

private:
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnUserMsg(WPARAM wParam, LPARAM lParam);

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
