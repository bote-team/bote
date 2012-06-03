// MySplitterWnd.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "botf2.h"
#include "MySplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySplitterWnd

CMySplitterWnd::CMySplitterWnd()
{
	// Hier wird die breite der Trennlinien festgesetzt. Diese  Variblen sind Attribute der Klasse
	// CSplitterWnd
/*	m_cxSplitter = 0;     // = 1
	m_cySplitter = 0;     // = 1
	m_cxBorderShare = 0;  // = 0
	m_cyBorderShare = 0;  // = 0
	m_cxSplitterGap = 1;  // = 1
	m_cySplitterGap = 1;  // = 1
*/
/*	m_cxSplitter = 5;     // = 1
	m_cySplitter = 5;     // = 1
	m_cxBorderShare = 2;  // = 0
	m_cyBorderShare = 2;  // = 0
	m_cxSplitterGap = 5;  // = 1
	m_cySplitterGap = 5;  // = 1*/
}

CMySplitterWnd::~CMySplitterWnd()
{
}


BEGIN_MESSAGE_MAP(CMySplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CMySplitterWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CMySplitterWnd

void CMySplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen

	// Das ist hier ganz wichtig, das wir die Methode nicht von MySplitterWnd aufrufen sondern von der
	// Klasse CWnd
	CWnd::OnMouseMove(nFlags, point);
}

void CMySplitterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen

	// Das ist hier ganz wichtig, das wir die Methode nicht von MySplitterWnd aufrufen sondern von der
	// Klasse CWnd
	CWnd::OnLButtonDown(nFlags, point);
}

void CMySplitterWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen

	// Das ist hier ganz wichtig, das wir die Methode nicht von MySplitterWnd aufrufen sondern von der
	// Klasse CWnd
	CWnd::OnLButtonDblClk(nFlags, point);
}
