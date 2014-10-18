// MyDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BotE.h"
#include "MyDialog.h"
#include "resource.h"
#include "BotEDoc.h"


// CChatDlg-Dialogfeld

//IMPLEMENT_DYNAMIC(CChatDlg, CDialog)

MyDialog::MyDialog(CWnd* pParent /*=NULL*/)
	: CDialog(MyDialog::IDD, pParent)
{
}


BEGIN_MESSAGE_MAP(MyDialog, CDialog)
//	ON_BN_CLICKED(IDC_SEND, &CChatDlg::OnBnClickedSend)
//	ON_BN_CLICKED(IDC_CLOSE, &CChatDlg::OnBnClickedClose)

END_MESSAGE_MAP()

