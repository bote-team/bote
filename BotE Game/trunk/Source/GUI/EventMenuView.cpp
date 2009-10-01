// EventMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "EventMenuView.h"
#include "Races\RaceController.h"

// CEventMenuView

IMPLEMENT_DYNCREATE(CEventMenuView, CMainBaseView)

CEventMenuView::CEventMenuView()
{

}

CEventMenuView::~CEventMenuView()
{
}

BEGIN_MESSAGE_MAP(CEventMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CEventMenuView drawing

void CEventMenuView::OnDraw(CDC* dc)
{
	SetFocus();
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// TODO: add draw code here
	CMemDC pDC(dc);
	CRect client;
	GetClientRect(&client);
		
	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());
	
	g.Clear(Color::Black);
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);
					
	if (pMajor->GetEmpire()->GetEventMessages()->GetSize())
	{
		CEventScreen* eventScreen = (CEventScreen*)pMajor->GetEmpire()->GetEventMessages()->GetAt(0);
		eventScreen->Create();
		eventScreen->Draw(&g, pDoc->GetGraphicPool());		
	}	
}

// CEventMenuView diagnostics

#ifdef _DEBUG
void CEventMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CEventMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEventMenuView message handlers

void CEventMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	m_TotalSize = CSize(1280, 1024);
}

void CEventMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CEventMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;	
}

void CEventMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	if (pMajor->GetEmpire()->GetEventMessages()->GetSize())
	{
		CEventScreen* eventScreen = (CEventScreen*)pMajor->GetEmpire()->GetEventMessages()->GetAt(0);
		CalcLogicalPoint(point);
		int counter = -1;
		ButtonReactOnLeftClick(point, eventScreen->GetButtons(), counter, true);

		if (counter == 0)
			CloseScreen(eventScreen);
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CEventMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	if (pMajor->GetEmpire()->GetEventMessages()->GetSize())
	{
		CEventScreen* eventScreen = (CEventScreen*)pMajor->GetEmpire()->GetEventMessages()->GetAt(0);
		CalcLogicalPoint(point);
		ButtonReactOnMouseOver(point, eventScreen->GetButtons());		
	}
	CMainBaseView::OnMouseMove(nFlags, point);
}

void CEventMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	if (nChar == VK_RETURN || nChar == VK_ESCAPE)
		if (pMajor->GetEmpire()->GetEventMessages()->GetSize())
		{
			CEventScreen* eventScreen = (CEventScreen*)pMajor->GetEmpire()->GetEventMessages()->GetAt(0);
			CloseScreen(eventScreen);
		}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/// Funktion schließt das derzeit angezeigte Event.
void CEventMenuView::CloseScreen(CEventScreen* eventScreen)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;
	
	delete eventScreen;
	pMajor->GetEmpire()->GetEventMessages()->RemoveAt(0);
	if (pMajor->GetEmpire()->GetEventMessages()->GetSize() == 0)
	{
		network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pMajor->GetRaceID());

		pDoc->GetMainFrame()->FullScreenMainView(false);
		if (pDoc->m_iSelectedView[client] == 0)
			pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
		pDoc->GetMainFrame()->SelectMainView(pDoc->m_iSelectedView[client], pMajor->GetRaceID());
		pDoc->m_iSelectedView[client] = 0;
	}
}