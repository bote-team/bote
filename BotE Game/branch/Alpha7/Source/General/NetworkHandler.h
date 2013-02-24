/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once

#include "BotEDoc.h"
#include "BotEServer.h"
#include "BotEClient.h"

class CNetworkHandler : public network::CServerListener, public network::CClientListener
{
	CBotEDoc *m_pDoc;

public:
	CNetworkHandler(CBotEDoc *pDoc);
	~CNetworkHandler();

	virtual void OnServerSocketError(int nError) {MessageBox(AfxGetMainWnd()->GetSafeHwnd(),network::GetWSAErrorMsg(nError),"Server_Error",MB_ICONEXCLAMATION | MB_OK);}
	virtual void OnPrepareGame();
	virtual void OnCalculateGame();

	virtual void OnClientDisconnected() {MessageBox(AfxGetMainWnd()->GetSafeHwnd(),"Connection to server lost.","Error",MB_ICONEXCLAMATION | MB_OK);}
	virtual void OnClientSocketError(int nError) {MessageBox(AfxGetMainWnd()->GetSafeHwnd(),network::GetWSAErrorMsg(nError),"Client_Error",MB_ICONEXCLAMATION | MB_OK);}
	virtual void OnClientChangedRaces() {}
	virtual void OnNextRound(network::CNextRound *pMsg);
	virtual void OnChatMsg(network::CChatMsg *pMsg);

	virtual void OnClientLost(const CString &);
	virtual void OnBeginGame(network::CBeginGame *) {}

};
