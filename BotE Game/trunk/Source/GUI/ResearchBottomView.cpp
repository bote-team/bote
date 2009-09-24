// ResearchBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "ResearchBottomView.h"
#include "RaceController.h"

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
	ASSERT(pDoc);
	
	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);
	if (!pMajor)
		return;
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
					
	CString sPrefix = pMajor->GetPrefix();
	Color color;
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrGalaxySectorText);
	fontBrush.SetColor(color);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "researchV3.jpg");

	// gibt es keine Spezialforschung zur Auswahl, so wird auf Standardanzeige umgestellt
	if (pDoc->m_iShowWhichTechInView3 == 6 && pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == TRUE)
		pDoc->m_iShowWhichTechInView3 = 0;
	
	// Grafik zeichnen		
	if (graphic)
	{
		g.DrawImage(graphic, 0, 0, 1075, 249);
		graphic = NULL;
	}
	switch(pDoc->m_iShowWhichTechInView3)
	{
	case 0:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\biotech.png"); break;
	case 1:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\energytech.png"); break;
	case 2:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\computertech.png"); break;
	case 3:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\propulsiontech.png"); break;
	case 4:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\constructiontech.png"); break;
	case 5:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\weapontech.png"); break;
	case 6:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\specialtech.png"); break;
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
	if (pDoc->m_iShowWhichTechInView3 != 6)
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(pDoc->m_iShowWhichTechInView3);
	else
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 30, rect.right-325, rect.bottom), &fontFormat, &fontBrush);
	
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	fontBrush.SetColor(Color(200,200,250));
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	if (pDoc->m_iShowWhichTechInView3 != 6)
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechDescription(pDoc->m_iShowWhichTechInView3);
	else
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexDescription();
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
