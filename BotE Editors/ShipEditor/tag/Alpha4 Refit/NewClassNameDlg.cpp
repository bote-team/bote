// NewClassNameDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShipEditor.h"
#include "NewClassNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CNewClassNameDlg 


CNewClassNameDlg::CNewClassNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewClassNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewClassNameDlg)
	m_strNewClassName = _T("");
	//}}AFX_DATA_INIT
}


void CNewClassNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewClassNameDlg)
	DDX_Text(pDX, IDC_NEWCLASSNAME, m_strNewClassName);
	DDV_MaxChars(pDX, m_strNewClassName, 25);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewClassNameDlg, CDialog)
	//{{AFX_MSG_MAP(CNewClassNameDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CNewClassNameDlg 
