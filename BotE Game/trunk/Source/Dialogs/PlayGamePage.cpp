// PlayGame.cpp : implementation file
//

#include "stdafx.h"
#include "Botf2.h"
#include "PlayGamePage.h"


// CPlayGamePage dialog

IMPLEMENT_DYNAMIC(CPlayGamePage, CPropertyPage)

CPlayGamePage::CPlayGamePage()
: CPropertyPage(CPlayGamePage::IDD), m_pParent(NULL), m_nState(0), m_nRound(0)
{

}

CPlayGamePage::~CPlayGamePage()
{
}

void CPlayGamePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayGamePage, CPropertyPage)
	ON_BN_CLICKED(IDC_ENDOFROUND, &CPlayGamePage::OnBnClickedEndOfRound)
	ON_BN_CLICKED(IDC_TOALL, &CPlayGamePage::OnBnClickedToall)
	ON_EN_CHANGE(IDC_MSG, &CPlayGamePage::OnEnChangeMsg)
END_MESSAGE_MAP()


// CPlayGamePage message handlers

void CPlayGamePage::OnBnClickedEndOfRound()
{
	m_nState = 0;
	UpdateControls();
	client.EndOfRound((network::CPeerData*)m_pParent->GetClientDocument());
}

void CPlayGamePage::EnableWizardButtons()
{
	m_pParent->SetDlgItemText(IDCANCEL, "Schließen");
	m_pParent->SetWizardButtons(PSWIZB_DISABLEDFINISH);
}

BOOL CPlayGamePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pParent = dynamic_cast<CMainDlg*>(GetParent());
	ASSERT(m_pParent);

	((CButton *)GetDlgItem(IDC_TOALL))->SetCheck(TRUE);

	// Controls im Rundenende-Zustand initialisieren
	m_nState = 0;
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPlayGamePage::OnSetActive()
{
	EnableWizardButtons();
	return CPropertyPage::OnSetActive();
}

void CPlayGamePage::UpdateControls()
{
	switch (m_nState)
	{
	case 0:
		// Rundenende-Zustand, auf Daten vom Server warten
		GetDlgItem(IDC_ENDOFROUND)->EnableWindow(FALSE);
		SetDlgItemText(IDC_TEXT, "Warte auf Daten vom Server...");
		break;

	case 1:
		{
		// Client führt seinen Spielzug durch
		GetDlgItem(IDC_ENDOFROUND)->EnableWindow(TRUE);
		CString msg;
		msg.Format("Runde #%u: Der Spielzug wird jetzt durchgeführt.\nNächste Runde mit \"Rundenende\".", m_nRound);
		SetDlgItemText(IDC_TEXT, msg);
		break;
		}

	default:
		ASSERT(FALSE);
	}

	EnableReceiverCheckBoxes(!((CButton *)GetDlgItem(IDC_TOALL))->GetCheck());
}

void CPlayGamePage::NextRound(UINT nRound)
{
	m_nState = 1;
	m_nRound = nRound;
	UpdateControls();
}

void CPlayGamePage::OnBnClickedToall()
{
	EnableReceiverCheckBoxes(!((CButton *)GetDlgItem(IDC_TOALL))->GetCheck());
}

void CPlayGamePage::EnableReceiverCheckBoxes(BOOL bEnabled)
{
	using namespace network;
	int ids[] = {IDC_TOPLAYER1, IDC_TOPLAYER2, IDC_TOPLAYER3, IDC_TOPLAYER4, IDC_TOPLAYER5, IDC_TOPLAYER6};
	RACE races[] = {RACE_1, RACE_2, RACE_3, RACE_4, RACE_5, RACE_6};
	for (int i = 0; i < 6; i++)
	{
		GetDlgItem(ids[i])->EnableWindow(bEnabled && client.IsPlayedByClient(races[i])
			&& races[i] != client.GetClientRace());
		if (((CButton *)GetDlgItem(ids[i]))->GetCheck() && !client.IsPlayedByClient(races[i]))
			((CButton *)GetDlgItem(ids[i]))->SetCheck(FALSE);
	}
}

void CPlayGamePage::OnEnChangeMsg()
{
	using namespace network;
	if (((CEdit *)GetDlgItem(IDC_MSG))->GetLineCount() > 1)
	{
		CString msg;
		GetDlgItemText(IDC_MSG, msg);
		msg.Replace("\r\n", "");
		msg.Trim();

		BOOL bSuccess = FALSE;
		if (msg.GetLength() > 0)
		{
			if (((CButton *)GetDlgItem(IDC_TOALL))->GetCheck())
			{
				client.Chat(msg);
				bSuccess = TRUE;
			}
			else
			{
				int ids[] = {IDC_TOPLAYER1, IDC_TOPLAYER2, IDC_TOPLAYER3,
					IDC_TOPLAYER4, IDC_TOPLAYER5, IDC_TOPLAYER6};
				BOOL bReceiver[RACE_COUNT];
				for (int i = 0; i < RACE_COUNT; i++)
				{
					bReceiver[i] = GetDlgItem(ids[i])->IsWindowEnabled() &&
						((CButton *)GetDlgItem(ids[i]))->GetCheck();
					if (bReceiver[i]) bSuccess = TRUE;
				}
				if (bSuccess) client.Chat(msg, bReceiver);
			}
		}

		if (bSuccess)
		{
			// Chat-Nachricht wurde versendet, zu eigenem Fenster hinzufügen, Eingabefeld leeren
			AddChatMsg(msg);
			SetDlgItemText(IDC_MSG, "");
		}
		else
		{
			// bereinigten Text wieder setzen; Cursor ans Ende setzen
			SetDlgItemText(IDC_MSG, msg);
			((CEdit *)GetDlgItem(IDC_MSG))->SetSel(msg.GetLength(), -1);
		}
	}
}

void CPlayGamePage::AddChatMsg(network::CChatMsg *pMsg)
{
	AddChatMsg(pMsg->GetSenderName(), pMsg->GetSenderRace(), pMsg->GetMessage());
}

void CPlayGamePage::AddChatMsg(CString msg)
{
	AddChatMsg(client.GetClientUserName(), client.GetClientRace(), msg);
}

void CPlayGamePage::AddChatMsg(CString name, network::RACE race, CString msg)
{
	using namespace network;
	CString total;
	GetDlgItemText(IDC_CHAT, total);
	if (total != "") total += "\r\n";

	CString raceName = "";
	switch (race)
	{
	case RACE_1:	raceName = "Föderation"; break;
	case RACE_2:	raceName = "Ferengi"; break;
	case RACE_3:	raceName = "Klingonen"; break;
	case RACE_4:	raceName = "Romulaner"; break;
	case RACE_5:	raceName = "Cardassianer"; break;
	case RACE_6:	raceName = "Dominion"; break;
	}

	CString newtext;
	newtext.Format("%s (%s): %s", name, raceName, msg);
	SetDlgItemText(IDC_CHAT, total + newtext);
	((CEdit *)GetDlgItem(IDC_CHAT))->SetSel(msg.GetLength(), -1);
}
