// MyDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BotE.h"
#include "MyDialog.h"
#include "resource.h"
#include "BotEDoc.h"


// MyDialog-Dialogfeld

//IMPLEMENT_DYNAMIC(MyDialog, CDialog)

MyDialog::MyDialog(CWnd* pParent /*=NULL*/)
	: CDialog(MyDialog::IDD, pParent)
{
	//CEdit* pEdit;
	//pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	//pEdit->SetSel(strtest.GetLength(), -1);
}


BEGIN_MESSAGE_MAP(MyDialog, CDialog)
//	ON_BN_CLICKED(IDC_SEND, &CChatDlg::OnBnClickedSend)
//	ON_BN_CLICKED(IDC_CLOSE, &CChatDlg::OnBnClickedClose)

END_MESSAGE_MAP()

