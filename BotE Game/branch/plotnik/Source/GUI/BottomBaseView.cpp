// BottomBaseView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "BottomBaseView.h"
#include "Races\RaceController.h"

// CBottomBaseView
CMajor* CBottomBaseView::m_pPlayersRace = NULL;

IMPLEMENT_DYNCREATE(CBottomBaseView, CView)

CBottomBaseView::CBottomBaseView()
{
}

CBottomBaseView::~CBottomBaseView()
{
}

BEGIN_MESSAGE_MAP(CBottomBaseView, CView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CBottomBaseView drawing

void CBottomBaseView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CBottomBaseView diagnostics

#ifdef _DEBUG
void CBottomBaseView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CBottomBaseView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBottomBaseView message handlers
void CBottomBaseView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

/*	CString sID = pDoc->GetPlayersRaceID();
	m_pPlayersRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sID));
	ASSERT(m_pPlayersRace);
	*/

	// Größe der View in logischen Koordinaten
	m_TotalSize = CSize(1075, 249);

	pDoc->GetMainFrame()->AddToTooltip(this);	
}

BOOL CBottomBaseView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}


Gdiplus::Color CBottomBaseView::GetFontColorForSmallButton(void)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	Color color;
	color.SetFromCOLORREF(pPlayer->GetDesign()->m_clrSmallBtn);
	return color;
}

void CBottomBaseView::CalcLogicalPoint(CPoint &point)
{
	CRect client;
	GetClientRect(&client);
	
	point.x *= (float)m_TotalSize.cx / (float)client.Width();
	point.y *= (float)m_TotalSize.cy / (float)client.Height();	
}

void CBottomBaseView::CalcDeviceRect(CRect &rect)
{
	CRect client;
	GetClientRect(&client);

	CPoint p1 = rect.TopLeft();
	p1.x *= (float)client.Width() / (float)m_TotalSize.cx;
	p1.y *= (float)client.Height() / (float)m_TotalSize.cy;
	
	CPoint p2 = rect.BottomRight();
	p2.x *= (float)client.Width() / (float)m_TotalSize.cx;
	p2.y *= (float)client.Height() / (float)m_TotalSize.cy;
	rect.SetRect(p1, p2);
}