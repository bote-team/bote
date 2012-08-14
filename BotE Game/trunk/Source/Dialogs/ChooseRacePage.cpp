// ChooseRacePage.cpp : implementation file
//

#include "stdafx.h"
#include "Botf2.h"
#include "ChooseRacePage.h"

#include "BotEClient.h"
#include "BotEServer.h"

#include "LANServer.h"
#include "Botf2Doc.h"

// CChooseRacePage dialog

IMPLEMENT_DYNAMIC(CChooseRacePage, CPropertyPage)

CChooseRacePage::CChooseRacePage()
	: CPropertyPage(CChooseRacePage::IDD), m_bIsServer(false), m_nPlayerCount(0), m_pParent(NULL)
{
}

CChooseRacePage::~CChooseRacePage()
{
}

void CChooseRacePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOSTIP, m_hostIP);
	DDX_Control(pDX, IDC_HOSTPORT, m_hostPort);
}


BEGIN_MESSAGE_MAP(CChooseRacePage, CPropertyPage)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RACE, IDC_RACE6, OnChooseRace)
	ON_EN_CHANGE(IDC_MSG, &CChooseRacePage::OnEnChangeMsg)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SERVER1, IDC_SERVER6, &CChooseRacePage::OnSetPlayByServer)
END_MESSAGE_MAP()


// CChooseRacePage message handlers

BOOL CChooseRacePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Zeiger auf Steuerelemente
	m_pParent = dynamic_cast<CMainDlg*>(GetParent());
	ASSERT(m_pParent);

	using namespace network;
	// Name der Buttons aus dem ResourceManager auslesen
	int ids[] = {IDC_RACE, IDC_RACE2, IDC_RACE3, IDC_RACE4, IDC_RACE5, IDC_RACE6};
	for (int nRace = 0; nRace < RACE_COUNT; nRace++)
	{
		ASSERT(nRace < 6);
		network::RACE race = (network::RACE)(RACE_FIRST + nRace);
		CString empireName;
		switch (race)
		{
		case network::RACE_1:	empireName = CResourceManager::GetString("RACE1_EMPIRE"); break;
		case network::RACE_2:	empireName = CResourceManager::GetString("RACE2_EMPIRE"); break;
		case network::RACE_3:	empireName = CResourceManager::GetString("RACE3_EMPIRE"); break;
		case network::RACE_4:	empireName = CResourceManager::GetString("RACE4_EMPIRE"); break;
		case network::RACE_5:	empireName = CResourceManager::GetString("RACE5_EMPIRE"); break;
		case network::RACE_6:	empireName = CResourceManager::GetString("RACE6_EMPIRE"); break;
		}
		SetDlgItemTextA(ids[nRace], empireName);
	}
	EnableServerPlayerButtons();

	// Steuerelemente initialisieren
	m_hostIP.EnableWindow(FALSE);
	m_hostPort.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

#pragma warning(push)
#pragma warning(disable : 4706)
BOOL CChooseRacePage::OnSetActive()
{
	DWORD dwIP = 0;
	UINT nPort = 0;
	client.GetRemoteAddr(dwIP, nPort);

	CString sPort;
	sPort.Format("%u", nPort);

	char hostName[80];
	hostent* pHost;
	if (m_bIsServer && gethostname(hostName, 80) == 0 && (pHost = gethostbyname(hostName)))
	{
#ifdef _DEBUG
		for (int i = 0; pHost->h_addr_list[i]; i++)
		{
			DWORD dwAddr;
			memcpy(&dwAddr, pHost->h_addr_list[i], sizeof(dwAddr));
			MYTRACE(MT::LEVEL_INFO, "server: IP %s\n", network::AddrToString(ntohl(dwAddr)));
		}
#endif
		if (pHost->h_addr)
		{
			DWORD dwAddr;
			memcpy(&dwAddr, pHost->h_addr, sizeof(dwAddr));
			dwIP = ntohl(dwAddr);
		}

/*		char *ip = hostName;
		char *port = NULL;
		struct addrinfo aiHints;
		struct addrinfo *aiList = NULL;

		memset(&aiHints, 0, sizeof(aiHints));
		aiHints.ai_family = AF_INET;
		aiHints.ai_socktype = SOCK_STREAM;
		aiHints.ai_protocol = IPPROTO_TCP;

		// TODO prüfen, welche IP-Adressen genau hier geliefert werden, wenn eine Netzwerkverbindung
		// besteht -> dwIP auf öffentliche IP setzen
		if (getaddrinfo(ip, port, &aiHints, &aiList) == 0)
		{
			struct addrinfo *p = aiList;
			while (p)
			{
				struct in_addr *addr = &((sockaddr_in *)p->ai_addr)->sin_addr;
				TRACE("server: IP %s\n", network::AddrToString(ntohl(addr->S_un.S_addr))
					//, ntohs(((sockaddr_in *)p->ai_addr)->sin_port)
					);

				if (!p->ai_next)
					dwIP = ntohl(((sockaddr_in *)p->ai_addr)->sin_addr.S_un.S_addr);

				p = p->ai_next;
			}
		}
		freeaddrinfo(aiList); */
	}

	m_hostIP.SetAddress(dwIP);
	m_hostPort.SetWindowText(sPort);

	SetDlgItemText(IDC_MSG, "");
	SetDlgItemText(IDC_CHAT, "");

	EnableRaceButtons();
	EnableWizardButtons();

	return CPropertyPage::OnSetActive();
}
#pragma warning(pop)

void CChooseRacePage::EnableWizardButtons()
{
//	m_pParent->SetWizardButtons(PSWIZB_BACK | (CheckValues() ? PSWIZB_FINISH : PSWIZB_DISABLEDFINISH));

	DWORD dwFlags = PSWIZB_BACK;
	if (CheckValues())
		dwFlags |= PSWIZB_FINISH; //PSWIZB_NEXT;
	else
		dwFlags |= PSWIZB_DISABLEDFINISH;
	m_pParent->SetWizardButtons(dwFlags);
}

BOOL CChooseRacePage::CheckValues()
{
	// im Server-Programm kann Fertigstellen gewählt werden, wenn mind. ein Spieler
	// ein Volk gewählt hat
//	return m_bIsServer && m_nPlayerCount > 0;

	// können das Spiel erstmal nur starten, wenn der Server-Spieler ein Volk gewählt hat
	return m_bIsServer && client.GetClientRace() != network::RACE_NONE;
}

LRESULT CChooseRacePage::OnWizardBack()
{
	client.Disconnect();
	if (m_bIsServer)
	{
		serverPublisher.StopPublishing();
		server.Stop();
	}

	return CPropertyPage::OnWizardBack();
}

void CChooseRacePage::EnableRaceButtons()
{
	using namespace network;

	int ids[] = {IDC_RACE, IDC_RACE2, IDC_RACE3, IDC_RACE4, IDC_RACE5, IDC_RACE6};
	int names[] = {IDC_NAME1, IDC_NAME2, IDC_NAME3, IDC_NAME4, IDC_NAME5, IDC_NAME6};
	m_nPlayerCount = 0;

	// Buttons aktivieren/deaktivieren, aus-/abwählen, Anzahl der Spieler zählen
	for (int nRace = 0; nRace < RACE_COUNT; nRace++)
	{
		ASSERT(nRace < 6);
		CButton *pButton = (CButton *)GetDlgItem(ids[nRace]);
		ASSERT(pButton);
		network::RACE race = (network::RACE)(RACE_FIRST + nRace);

		if (client.IsPlayer(race))
		{
			m_nPlayerCount++;
			pButton->EnableWindow(race == client.GetClientRace());
			pButton->SetCheck(race == client.GetClientRace());
		}
		else
		{
			pButton->EnableWindow(TRUE);
			pButton->SetCheck(FALSE);
		}

		////////////////////////////////////////////
		// Alpha5 Bedingung
#ifndef DEVELOPMENT_VERSION
		if (nRace != 0 && nRace != 1 && nRace != 2 && nRace != 3 && nRace != 4)
			pButton->EnableWindow(FALSE);
#endif
		////////////////////////////////////////////

		if (client.GetUserName(race) != "")
			SetDlgItemText(names[nRace], "(" + client.GetUserName(race) + ")");
		else
			SetDlgItemText(names[nRace], "");
	}

	EnableServerPlayerButtons();
	EnableWizardButtons();
}

afx_msg void CChooseRacePage::OnChooseRace(UINT nID)
{
	using namespace network;

	// IDs der Buttons
	unsigned ids[] = {IDC_RACE, IDC_RACE2, IDC_RACE3, IDC_RACE4, IDC_RACE5, IDC_RACE6};

	for (int i = 0; i < RACE_COUNT; i++)
	{
		ASSERT(i < 6);

		CButton *pButton = (CButton *)GetDlgItem(ids[i]);
		ASSERT(pButton);

		if (ids[i] == nID)
		{
			if (pButton->GetCheck())
			{
				pButton->SetCheck(FALSE);
				m_pParent->GetDlgItem(ID_WIZBACK)->SetFocus();
				client.RequestRace(network::RACE_NONE);

			}
			else
			{
				pButton->SetCheck(TRUE);
				client.RequestRace((network::RACE)(network::RACE_FIRST + i));
				// Soundwiedergabe für die Rasse
				switch (i + 1)
				{
				case network::RACE_1:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_FED_RACESELECT); break;
				case network::RACE_2:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_FER_RACESELECT); break;
				case network::RACE_3:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_KLI_RACESELECT); break;
				case network::RACE_4:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_ROM_RACESELECT); break;
				case network::RACE_5:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_CAR_RACESELECT); break;
				case network::RACE_6:	CSoundManager::GetInstance()->PlaySound(SNDMGR_VOICE_DOM_RACESELECT); break;
				}
			}
		}
		else
			pButton->SetCheck(FALSE);
	}
}

afx_msg void CChooseRacePage::OnSetPlayByServer(UINT /*nID*/)
{
/*	using namespace network;

	if (!m_bIsServer) return;

	int id[] = {IDC_SERVER1, IDC_SERVER2, IDC_SERVER3, IDC_SERVER4, IDC_SERVER5, IDC_SERVER6};
	RACE race[] = {RACE_1, RACE_2, RACE_3, RACE_4, RACE_5, RACE_6};

	int idx = -1;
	for (int i = 0; i < RACE_COUNT; i++)
	{
		if (id[i] == nID)
		{
			idx = i;
			break;
		}
	}

	if (idx == -1) return;

	BOOL bChecked = ((CButton *)GetDlgItem(nID))->GetCheck();
	if (!server.SetPlayByServer(race[idx], bChecked))
		((CButton *)GetDlgItem(nID))->SetCheck(!bChecked); */
}

LRESULT CChooseRacePage::OnWizardNext()
{
/*	using namespace network;

	// Wechsel zur nächsten Seite nur möglich, wenn der Server läuft
	if (!m_bIsServer) return -1;

	SetCursor(::LoadCursor(NULL, IDC_WAIT));

	// evtl. laufende Veröffentlichung des Spiels stoppen
	serverPublisher.StopPublishing();

	// die nicht ausgewählten Völker vom Server spielen lassen
	for (int i = 0; i < RACE_COUNT; i++)
	{
		RACE race = (RACE)(RACE_FIRST + i);
		if (!server.IsPlayedByClient(race)) server.SetPlayByServer(race, TRUE, FALSE);
	}

	// Spiel starten; veranlasst die Clients, auf die nächste Seite zu wechseln
	server.BeginGame((network::CPeerData*)m_pParent->GetServerDocument());

	SetCursor(::LoadCursor(NULL, IDC_ARROW));*/
	return CPropertyPage::OnWizardNext();
}

void CChooseRacePage::AddChatMsg(CString name, CString msg)
{
	CString total;
	GetDlgItemText(IDC_CHAT, total);
	if (total != "") total += "\r\n";
	CString newtext;
	newtext.Format("%s: %s", name, msg);
	SetDlgItemText(IDC_CHAT, total + newtext);
	((CEdit *)GetDlgItem(IDC_CHAT))->SetSel(msg.GetLength(), -1);
}

void CChooseRacePage::OnEnChangeMsg()
{
	if (((CEdit *)GetDlgItem(IDC_MSG))->GetLineCount() > 1)
	{
		CString msg;
		GetDlgItemText(IDC_MSG, msg);
		msg.Replace("\r\n", "");
		msg.Trim();
		if (msg.GetLength() > 0)
		{
			client.Chat(msg);
			AddChatMsg(client.GetClientUserName(), msg);
		}
		SetDlgItemText(IDC_MSG, "");
	}
}

void CChooseRacePage::EnableServerPlayerButtons()
{
	using namespace network;

/*	int ids[] = {IDC_SERVER1, IDC_SERVER2, IDC_SERVER3, IDC_SERVER4, IDC_SERVER5, IDC_SERVER6};
	RACE race[] = {RACE_1, RACE_2, RACE_3, RACE_4, RACE_5, RACE_6};

	for (int i = 0; i < RACE_COUNT; i++)
	{
//		GetDlgItem(ids[i])->EnableWindow(m_bIsServer && !client.IsPlayedByClient(race[i]));
//		((CButton *)GetDlgItem(ids[i]))->SetCheck(client.IsPlayedByServer(race[i]));

		GetDlgItem(ids[i])->ShowWindow(SW_HIDE);
	}
*/
}

BOOL CChooseRacePage::OnWizardFinish()
{
	using namespace network;

	// Wechsel zur nächsten Seite nur möglich, wenn der Server läuft
	if (!m_bIsServer) return FALSE;

	SetCursor(::LoadCursor(NULL, IDC_WAIT));

	// evtl. laufende Veröffentlichung des Spiels stoppen
	serverPublisher.StopPublishing();

	// die nicht ausgewählten Völker vom Server spielen lassen
	for (int i = 0; i < RACE_COUNT; i++)
	{
		RACE race = (RACE)(RACE_FIRST + i);
		if (!server.IsPlayedByClient(race)) server.SetPlayByServer(race,TRUE,FALSE);
	}

	// Spiel starten; veranlasst die Clients, auf die nächste Seite zu wechseln
	m_pParent->GetServerDocument()->PrepareData();
	server.BeginGame(m_pParent->GetServerDocument());

	SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return CPropertyPage::OnWizardFinish();

	/*
	using namespace network;

	// Wechsel zur nächsten Seite nur möglich, wenn der Server läuft
	if (!m_bIsServer) return -1;

	SetCursor(::LoadCursor(NULL, IDC_WAIT));

	// evtl. laufende Veröffentlichung des Spiels stoppen
	serverPublisher.StopPublishing();

	// die nicht ausgewählten Völker vom Server spielen lassen
	for (int i = 0; i < RACE_COUNT; i++)
	{
		RACE race = (RACE)(RACE_FIRST + i);
		if (!server.IsPlayedByClient(race)) server.SetPlayByServer(race, TRUE, FALSE);
	}

	// Spiel starten; veranlasst die Clients, auf die nächste Seite zu wechseln
	server.BeginGame(m_pParent->GetServerDocument());

	SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return CPropertyPage::OnWizardNext();
	*/
}
