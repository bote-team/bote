// EventMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "MainFrm.h"
#include "EventMenuView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"
#include "IniLoader.h"
#include "ClientWorker.h"
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
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	// TODO: add draw code here
	CMyMemDC pDC(dc);
	CRect client;
	GetClientRect(&client);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());

	g.Clear(static_cast<Gdiplus::ARGB>(Color::Black));
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);

	if (!pMajor->GetEmpire()->GetEvents()->empty())
	{
		const boost::shared_ptr<CEventScreen>& eventScreen = pMajor->GetEmpire()->FirstEvent(false);
		eventScreen->Create();
		eventScreen->Draw(&g, pDoc->GetGraphicPool());
		// Handelt es sich um ein Event zu einem Zufallsereignis?
		const CEventScreen::EVENT_SCREEN_TYPE type = eventScreen->GetType();
		if (type == CEventScreen::EVENT_SCREEN_TYPE_RANDOM
			|| type == CEventScreen::EVENT_SCREEN_TYPE_ALIEN_ENTITY)
		{
			// Dieses sofort schlieﬂen, wenn es nicht angezeigt werden soll
			if (CIniLoader* pIni = CIniLoader::GetInstance())
				if (!pIni->ReadValueDefault("Video", "SHOWEVENTPICTURES", true))
					CloseScreen(eventScreen);
		}
	}

	g.ReleaseHDC(pDC->GetSafeHdc());


	// TEST-OPTION (only for Debug)
	// Automatisch bis zu einer gewissen Runde durchzuklicken falls per Startparamter aktiviert
	// Eventscreen gleich wieder schlieﬂen
	if (const CCommandLineParameters* const clp = resources::pClp)
	{
		int nAutoTurns = clp->GetAutoTurns();
		if (!pDoc->m_bRoundEndPressed && pDoc->GetCurrentRound() < nAutoTurns)
		{
			if (!pMajor->GetEmpire()->GetEvents()->empty())
			{
				const boost::shared_ptr<CEventScreen>& eventScreen = pMajor->GetEmpire()->FirstEvent(false);
				CloseScreen(eventScreen);
			}
		}
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

	m_TotalSize = CSize(1280, 1024);

	// View bei den Tooltipps anmelden
	resources::pMainFrame->AddToTooltip(this);
}

void CEventMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CEventMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

void CEventMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	if (!pMajor->GetEmpire()->GetEvents()->empty())
	{
		const boost::shared_ptr<CEventScreen>& eventScreen = pMajor->GetEmpire()->FirstEvent(false);
		CalcLogicalPoint(point);
		int counter = -1;
		ButtonReactOnLeftClick(point, eventScreen->GetButtons(), counter);

		if (counter == 0)
			CloseScreen(eventScreen);
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CEventMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	if (!pMajor->GetEmpire()->GetEvents()->empty())
	{
		const boost::shared_ptr<CEventScreen>& eventScreen = pMajor->GetEmpire()->FirstEvent(false);
		CalcLogicalPoint(point);
		ButtonReactOnMouseOver(point, eventScreen->GetButtons());
	}
	CMainBaseView::OnMouseMove(nFlags, point);
}

void CEventMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	if (nChar == VK_RETURN || nChar == VK_ESCAPE)
		if (!pMajor->GetEmpire()->GetEvents()->empty())
		{
			const boost::shared_ptr<CEventScreen>& eventScreen = pMajor->GetEmpire()->FirstEvent(false);
			CloseScreen(eventScreen);
		}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/// Funktion schlieﬂt das derzeit angezeigte Event.
void CEventMenuView::CloseScreen(const boost::shared_ptr<CEventScreen>& eventScreen)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	eventScreen->Close();
	pMajor->GetEmpire()->FirstEvent(true);

	if (pMajor->GetEmpire()->GetEvents()->empty())
	{
		resources::pMainFrame->FullScreenMainView(false);
		resources::pMainFrame->SelectMainView(VIEWS::EMPIRE_VIEW, pMajor->GetRaceID());
		resources::pClientWorker->SetSelectedViewForTo(*pMajor, VIEWS::NULL_VIEW);
	}
	else
		Invalidate(false);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CEventMenuView::CreateTooltip(void)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return "";

	if (!pMajor->GetEmpire()->GetEvents()->empty())
	{
		// Wo sind wir
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		CalcLogicalPoint(pt);

		const boost::shared_ptr<CEventScreen>& eventScreen = pMajor->GetEmpire()->FirstEvent(false);
		return eventScreen->GetTooltip(pt);
	}


	return "";
}
