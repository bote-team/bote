// SearchServersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "SearchServersDlg.h"
#include "Loc.h"
#include "AssertBotE.h"

// CSearchServersDlg dialog

IMPLEMENT_DYNAMIC(CSearchServersDlg, CDialog)

CSearchServersDlg::CSearchServersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchServersDlg::IDD, pParent)
{
	m_strDescription = "";
	m_dwIP = m_nPort = 0;
}

CSearchServersDlg::~CSearchServersDlg()
{
}

void CSearchServersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVERS, m_listServers);
}


BEGIN_MESSAGE_MAP(CSearchServersDlg, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_SERVERS, &CSearchServersDlg::OnNMDblclkServers)
	ON_NOTIFY(NM_CLICK, IDC_SERVERS, &CSearchServersDlg::OnNMClickServers)
END_MESSAGE_MAP()


// CSearchServersDlg message handlers

BOOL CSearchServersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(CLoc::GetString("SEARCHLANSERVER"));

	if (CWnd* pWnd = GetDlgItem(IDC_STATIC_FOUND_BOTE_SERVERS))
		pWnd->SetWindowText(CLoc::GetString("FOUND_BOTE_SERVERS"));

	// Erscheinung der Liste
	m_listServers.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CRect rect;
	m_listServers.GetClientRect(&rect);
	int cx = (rect.Width() * 7) / 12;

	m_listServers.InsertColumn(0, CLoc::GetString("LANSERVERNAME"), LVCFMT_LEFT, cx, -1);
	m_listServers.InsertColumn(1, CLoc::GetString("LANSERVERIP"), LVCFMT_LEFT, rect.Width() - cx, -1);

	// als Handler registrieren und Anfrage broadcasten
	clientPublisher.AddLANClientListener(this);
	clientPublisher.SearchServers();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSearchServersDlg::OnServerFound(const CString &strDescription, DWORD dwIP, int nTCPPort)
{
	// ein Server hat sich gemeldet, zur Liste hinzufügen

	// Eintrag erzeugen, Beschreibung setzen
	int idx = m_listServers.InsertItem(m_listServers.GetItemCount(), strDescription);
	if (idx < 0) return;

	// Adresse setzen
	CString strAddr;
	strAddr.Format("%s:%i", network::AddrToString(dwIP), nTCPPort);
	m_listServers.SetItemText(idx, 1, strAddr);

	// Werte merken
	DLG_LISTENTRY entry = {strDescription, dwIP, nTCPPort};
	m_lEntries.AddTail(entry);
}

void CSearchServersDlg::OnNMDblclkServers(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	int idx = m_listServers.GetNextItem(-1, LVNI_SELECTED);
	if (idx > -1) OnOK();
	*pResult = 0;
}

void CSearchServersDlg::OnNMClickServers(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	GetDlgItem(IDOK)->EnableWindow(m_listServers.GetNextItem(-1, LVNI_SELECTED) > -1);
	*pResult = 0;
}

void CSearchServersDlg::OnOK()
{
	clientPublisher.RemoveLANClientListener(this);

	int idx = m_listServers.GetNextItem(-1, LVNI_SELECTED);
	if (idx > -1)
	{
		POSITION pos = m_lEntries.FindIndex(idx);
		AssertBotE(pos);
		DLG_LISTENTRY entry = m_lEntries.GetAt(pos);
		m_strDescription = entry.strDescription;
		m_dwIP = entry.dwIP;
		m_nPort = entry.nPort;

		__super::OnOK();
	}
}

void CSearchServersDlg::OnCancel()
{
	clientPublisher.RemoveLANClientListener(this);
	__super::OnCancel();
}
