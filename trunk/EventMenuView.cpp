// EventMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "EventMenuView.h"


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

void CEventMenuView::OnDraw(CDC* pDC)
{
	SetFocus();
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);
	
	// Doublebuffering wird initialisiert
	CRect client;
	GetClientRect(&client);
	Graphics doubleBuffer(pDC->GetSafeHdc());
	doubleBuffer.SetSmoothingMode(SmoothingModeHighQuality);
	doubleBuffer.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	doubleBuffer.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Bitmap bmp(client.Width(), client.Height());
	Graphics* g = Graphics::FromImage(&bmp);
	g->Clear(Color::Black);
	g->SetSmoothingMode(SmoothingModeHighQuality);
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g->SetPixelOffsetMode(PixelOffsetModeHighQuality);
	g->ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);
					
	if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetSize())
	{
		CEventScreen* eventScreen = (CEventScreen*)pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetAt(0);
		eventScreen->Create();
		eventScreen->Draw(g, pDoc->GetGraphicPool());		
	}
	doubleBuffer.DrawImage(&bmp, r.left, r.top, r.right, r.bottom);
	ReleaseDC(pDC);
	delete g;
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

	//bg_newsovmenu.DeleteObject();
	
	CString race = "";
	switch (pDoc->GetPlayersRace())
	{
		case HUMAN:		race = CResourceManager::GetString("RACE1_PREFIX"); break;
		case FERENGI:	race = CResourceManager::GetString("RACE2_PREFIX"); break;
		case KLINGON:	race = CResourceManager::GetString("RACE3_PREFIX"); break;
		case ROMULAN:	race = CResourceManager::GetString("RACE4_PREFIX"); break;
		case CARDASSIAN:race = CResourceManager::GetString("RACE5_PREFIX"); break;
		case DOMINION:	race = CResourceManager::GetString("RACE6_PREFIX"); break;
	}

	//FCObjImage img;
	//img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"newsovmenu.jpg");
	//bg_newsovmenu.Attach(FCWin32::CreateDDBHandle(img));

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

	if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetSize())
	{
		CEventScreen* eventScreen = (CEventScreen*)pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetAt(0);
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
	if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetSize())
	{
		CEventScreen* eventScreen = (CEventScreen*)pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetAt(0);
		CalcLogicalPoint(point);
		ButtonReactOnMouseOver(point, eventScreen->GetButtons());		
	}
	CMainBaseView::OnMouseMove(nFlags, point);
}

void CEventMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	if (nChar == VK_RETURN || nChar == VK_ESCAPE)
		if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetSize())
		{
			CEventScreen* eventScreen = (CEventScreen*)pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetAt(0);
			CloseScreen(eventScreen);
		}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/// Funktion schließt das derzeit angezeigte Event.
void CEventMenuView::CloseScreen(CEventScreen* eventScreen)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	
	delete eventScreen;
	pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->RemoveAt(0);
	if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetEventMessages()->GetSize() == 0)
	{
		pDoc->GetMainFrame()->FullScreenMainView(false);
		if (pDoc->m_iSelectedView[pDoc->GetPlayersRace()] == 0)
			pDoc->m_iSelectedView[pDoc->GetPlayersRace()] = EMPIRE_VIEW;
		pDoc->GetMainFrame()->SelectMainView(pDoc->m_iSelectedView[pDoc->GetPlayersRace()], pDoc->GetPlayersRace());
		pDoc->m_iSelectedView[pDoc->GetPlayersRace()] = 0;
	}
}