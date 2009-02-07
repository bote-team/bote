// DiplomacyBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "DiplomacyBottomView.h"


// CDiplomacyBottomView

CString CDiplomacyBottomView::m_strHeadLine = "";
CString CDiplomacyBottomView::m_strText = "";

IMPLEMENT_DYNCREATE(CDiplomacyBottomView, CBottomBaseView)

CDiplomacyBottomView::CDiplomacyBottomView()
{

}

CDiplomacyBottomView::~CDiplomacyBottomView()
{
}

BEGIN_MESSAGE_MAP(CDiplomacyBottomView, CBottomBaseView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDiplomacyBottomView drawing

void CDiplomacyBottomView::OnDraw(CDC* dc)
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

	// Soll was über die Diplomatie angezeigt werden
	CRect rect;
	rect.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	
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
	CFontLoader::CreateGDIFont(pDoc->GetPlayersRace(), 4, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	g.DrawString(m_strHeadLine.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 30, rect.right, rect.bottom), &fontFormat, &fontBrush);
	
	if (m_strText.IsEmpty())
		g.DrawString(CResourceManager::GetString("NO_DIPLOMATIC_NEWS").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 30, rect.right, rect.bottom), &fontFormat, &fontBrush);
	m_strHeadLine = "";		
	
	CFontLoader::CreateGDIFont(pDoc->GetPlayersRace(), 2, fontName, fontSize);
	fontBrush.SetColor(Color(200,200,250));
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g.DrawString(m_strText.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 100, rect.right-250, rect.bottom), &fontFormat, &fontBrush);
}

// CDiplomacyBottomView diagnostics

#ifdef _DEBUG
void CDiplomacyBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CDiplomacyBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDiplomacyBottomView message handlers

BOOL CDiplomacyBottomView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}
