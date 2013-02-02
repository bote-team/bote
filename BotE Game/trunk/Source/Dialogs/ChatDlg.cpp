// ChatDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BotE.h"
#include "ChatDlg.h"
#include "BotEDoc.h"


// CChatDlg-Dialogfeld

IMPLEMENT_DYNAMIC(CChatDlg, CDialog)

CChatDlg::CChatDlg(CBotEDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CChatDlg::IDD, pParent), m_pDoc(pDoc)
{
	ASSERT(pDoc);
}

CChatDlg::~CChatDlg()
{
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
	ON_BN_CLICKED(IDC_SEND, &CChatDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_CLOSE, &CChatDlg::OnBnClickedClose)

END_MESSAGE_MAP()

/// Nimmt einen String entgegen, welcher danach im Chat angezeigt werden kann.
void CChatDlg::AddChatMsg(CString userName, CString msg)
{
	CString s;
	GetDlgItemText(IDC_CHAT, s);
	if (!s.IsEmpty())
		s += "\r\n";
	s += userName+": "+msg;
	SetDlgItemText(IDC_CHAT, s);
	((CEdit *)GetDlgItem(IDC_CHAT))->SetSel(s.GetLength(), -1);
}

// CChatDlg-Meldungshandler

void CChatDlg::OnBnClickedSend()
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	CString s;
	GetDlgItemText(IDC_MESSAGE, s);
	s.Trim();
	if (!s.IsEmpty())
	{
		client.Chat(s);
		AddChatMsg(client.GetClientUserName(), s);
	}
	SetDlgItemText(IDC_MESSAGE, "");
}

void CChatDlg::OnBnClickedClose()
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	ShowWindow(SW_HIDE);
}
