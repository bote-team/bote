#include <afxwin.h>
#include <resource.h>

#if !defined(AFX_MYDIALOG_H__20B59A5E_FBE6_4A1C_A6B7_FDC199FE74EC__INCLUDED_)
#define AFX_MYDIALOG_H__20B59A5E_FBE6_4A1C_A6B7_FDC199FE74EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MyDialog dialog

class MyDialog : public CDialog
{
// Construction
public:
	MyDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MyDialog)
	enum { IDD = IDD_DIALOG3 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	BOOL OnInitDialog();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyDialog)
	//protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MyDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDIALOG_H__20B59A5E_FBE6_4A1C_A6B7_FDC199FE74EC__INCLUDED_)