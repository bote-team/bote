// ResearchBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "ResearchBottomView.h"


// CResearchBottomView

IMPLEMENT_DYNCREATE(CResearchBottomView, CBottomBaseView)

CResearchBottomView::CResearchBottomView()
{

}

CResearchBottomView::~CResearchBottomView()
{
}

BEGIN_MESSAGE_MAP(CResearchBottomView, CBottomBaseView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CResearchBottomView drawing

void CResearchBottomView::OnDraw(CDC* dc)
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
	CRect rect;
	rect.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);

	Bitmap* graphic = NULL;
					
	if (pDoc->GetPlayersRace() == HUMAN)
	{
		fontBrush.SetColor(Color(100,100,250));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE1_PREFIX") + "researchV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == FERENGI)
	{
		fontBrush.SetColor(Color(30,200,30));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE2_PREFIX") + "researchV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		fontBrush.SetColor(Color(250,80,30));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE3_PREFIX") + "researchV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == ROMULAN)
	{
		fontBrush.SetColor(Color(140,196,203));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE4_PREFIX") + "researchV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == CARDASSIAN)
	{
		fontBrush.SetColor(Color(74,146,138));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE5_PREFIX") + "researchV3.jpg");
	}
	else if (pDoc->GetPlayersRace() == DOMINION)
	{
		fontBrush.SetColor(Color(74,146,138));
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + CResourceManager::GetString("RACE6_PREFIX") + "researchV3.jpg");
	}
	// Grafik zeichnen		
	if (graphic)
	{
		g.DrawImage(graphic, 0, 0, 1075, 249);
		graphic = NULL;
	}
	switch(pDoc->m_iShowWhichTechInView3)
	{
	case 0:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\biotech.jpg"); break;
	case 1:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\energytech.jpg"); break;
	case 2:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\computertech.jpg"); break;
	case 3:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\propulsiontech.jpg"); break;
	case 4:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\constructiontech.jpg"); break;
	case 5:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\weapontech.jpg"); break;
	}
	if (graphic)
	{
		g.DrawImage(graphic, 790, 25, 240, 200);
		graphic = NULL;
	}
	// Name und Beschreibung der Forschung anzeigen
	CFontLoader::CreateGDIFont(pDoc->GetPlayersRace(), 4, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	CString s;
	s = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo()->GetTechName(pDoc->m_iShowWhichTechInView3);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 30, rect.right-325, rect.bottom), &fontFormat, &fontBrush);
	
	CFontLoader::CreateGDIFont(pDoc->GetPlayersRace(), 2, fontName, fontSize);
	fontBrush.SetColor(Color(200,200,250));
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	s = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo()->GetTechDescription(pDoc->m_iShowWhichTechInView3);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 100, rect.right-325, rect.bottom), &fontFormat, &fontBrush);
}


// CResearchBottomView diagnostics

#ifdef _DEBUG
void CResearchBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CResearchBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CResearchBottomView message handlers

BOOL CResearchBottomView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}
