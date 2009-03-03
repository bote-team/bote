#pragma once
#include "dyntabctrl.h"

// CRaceEditorDlg-Dialogfeld

class CRaceEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CRaceEditorDlg)

public:
	CRaceEditorDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CRaceEditorDlg();

// Dialogfelddaten
	enum { IDD = IDD_RACEEDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()

public:
	DynTabCtrl m_dynTabCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
