// ResearchBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "ResearchBottomView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"
#include "GraphicPool.h"

// CResearchBottomView

IMPLEMENT_DYNCREATE(CResearchBottomView, CBottomBaseView)

CResearchBottomView::CResearchBottomView()
{
	m_nCurrentTech = 0;
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
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;
	// TODO: add draw code here

	// Doublebuffering wird initialisiert
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

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(static_cast<Gdiplus::ARGB>(Color::White));
	CRect rect;
	rect.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);

	CString sPrefix = pMajor->GetPrefix();
	Color color;
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrGalaxySectorText);
	fontBrush.SetColor(color);
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "researchV3.boj");

	// gibt es keine Spezialforschung zur Auswahl, so wird auf Standardanzeige umgestellt
	if (m_nCurrentTech == 6 && pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == TRUE)
		m_nCurrentTech = 0;

	// Grafik zeichnen
	if (graphic)
	{
		g.DrawImage(graphic, 0, 0, 1075, 249);
		graphic = NULL;
	}
	switch(m_nCurrentTech)
	{
	case 0:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\biotech.bop"); break;
	case 1:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\energytech.bop"); break;
	case 2:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\computertech.bop"); break;
	case 3:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\propulsiontech.bop"); break;
	case 4:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\constructiontech.bop"); break;
	case 5:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\weapontech.bop"); break;
	case 6:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\specialtech.bop"); break;
	}
	if (graphic)
	{
		g.DrawImage(graphic, 790, 25, 240, 200);
		graphic = NULL;
	}
	// Name und Beschreibung der Forschung anzeigen
	CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	CString s;
	if (m_nCurrentTech != 6)
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(m_nCurrentTech);
	else
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 30, rect.right-325, rect.bottom), &fontFormat, &fontBrush);

	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	fontBrush.SetColor(Color(200,200,250));
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	if (m_nCurrentTech != 6)
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechDescription(m_nCurrentTech);
	else
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexDescription();
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 100, rect.right-325, rect.bottom), &fontFormat, &fontBrush);

	g.ReleaseHDC(pDC->GetSafeHdc());
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

BOOL CResearchBottomView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}
