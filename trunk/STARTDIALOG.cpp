// STARTDIALOG.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "botf2.h"
#include "STARTDIALOG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSTARTDIALOG 


CSTARTDIALOG::CSTARTDIALOG(CWnd* pParent /*=NULL*/)
	: CDialog(CSTARTDIALOG::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSTARTDIALOG)
	m_iRace = -1;
	//}}AFX_DATA_INIT
}


void CSTARTDIALOG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSTARTDIALOG)
	DDX_Radio(pDX, IDC_FEDERATION, m_iRace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSTARTDIALOG, CDialog)
	//{{AFX_MSG_MAP(CSTARTDIALOG)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSTARTDIALOG 
void CSTARTDIALOG::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}
