// RaceEditorDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "RaceEditor.h"
#include "RaceEditorDlg.h"
#include "MinorRaceEditorDlg.h"


// CRaceEditorDlg-Dialogfeld

IMPLEMENT_DYNAMIC(CRaceEditorDlg, CDialog)

CRaceEditorDlg::CRaceEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRaceEditorDlg::IDD, pParent)
{

}

CRaceEditorDlg::~CRaceEditorDlg()
{
}

void CRaceEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TAB1, m_dynTabCtrl);
}


BEGIN_MESSAGE_MAP(CRaceEditorDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CRaceEditorDlg-Meldungshandler

BOOL CRaceEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  hier zusätzliche Initialisierung hinzufügen.
	m_dynTabCtrl.InsertItem(new CMinorRaceEditorDlg(), "Majorraces", IDD_MINORRACEEDITOR_DIALOG);
	m_dynTabCtrl.InsertItem(new CMinorRaceEditorDlg(), "Minorraces", IDD_MINORRACEEDITOR_DIALOG);

	return TRUE;  // return TRUE unless you set the focus to a control
	// AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

void CRaceEditorDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein.
	m_dynTabCtrl.DeleteItem(1);
	m_dynTabCtrl.DeleteItem(0);
}
