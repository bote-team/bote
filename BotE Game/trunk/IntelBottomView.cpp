// IntelBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "IntelBottomView.h"
#include "IntelMenuView.h"


// CIntelBottomView

IMPLEMENT_DYNCREATE(CIntelBottomView, CBottomBaseView)

CIntelBottomView::CIntelBottomView()
{

}

CIntelBottomView::~CIntelBottomView()
{
}

BEGIN_MESSAGE_MAP(CIntelBottomView, CBottomBaseView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CIntelBottomView drawing

void CIntelBottomView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here

	// Doublebuffering wird initialisiert
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
				
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);
	Bitmap* graphic = NULL;
					
	if (pDoc->GetPlayersRace() == HUMAN)
	{
		fontBrush.SetColor(Color(100,100,250));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE1_PREFIX") + "diplomacyV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == FERENGI)
	{
		fontBrush.SetColor(Color(30,200,30));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE2_PREFIX") + "diplomacyV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		fontBrush.SetColor(Color(250,80,30));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE3_PREFIX") + "diplomacyV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == ROMULAN)
	{
		fontBrush.SetColor(Color(140,196,203));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE4_PREFIX") + "diplomacyV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == CARDASSIAN)
	{
		fontBrush.SetColor(Color(74,146,138));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE5_PREFIX") + "diplomacyV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == DOMINION)
	{
		fontBrush.SetColor(Color(74,146,138));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE6_PREFIX") + "diplomacyV3.jpg");
	}
	// Grafik zeichnen		
	if (graphic)
	{
		g.DrawImage(graphic, 0, 0, 1075, 249);
		graphic = NULL;
	}

	// Nur in bestimmten Submenüs werden in der View3 Berichte angezeigt
	BYTE curIntelSubMenu = pDoc->GetMainFrame()->GetSubMenu(RUNTIME_CLASS(CIntelMenuView));
	if (curIntelSubMenu == 4 || curIntelSubMenu == 5)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		short n = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelReports()->GetActiveReport();
		if (n != -1)
		{
			CFontLoader::CreateGDIFont(pDoc->GetPlayersRace(), 4, fontName, fontSize);
			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			CIntelObject* report = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelReports()->GetReport(n);
			CString s;
			if (report->GetIsSpy())
				s = CResourceManager::GetString("SPY");
			else
				s = CResourceManager::GetString("SABOTAGE");
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 30, r.right-100, r.bottom-20), &fontFormat, &fontBrush);

			CFontLoader::CreateGDIFont(pDoc->GetPlayersRace(), 2, fontName, fontSize);
			fontBrush.SetColor(Color(200,200,250));
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			if (report->GetOwner() == pDoc->GetPlayersRace())
				s = *report->GetOwnerDesc();
			else
				s = *report->GetEnemyDesc();
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 100, r.right-250, r.bottom-20), &fontFormat, &fontBrush);
		}			
	}
}

// CIntelBottomView diagnostics

#ifdef _DEBUG
void CIntelBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIntelBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIntelBottomView message handlers

BOOL CIntelBottomView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}
