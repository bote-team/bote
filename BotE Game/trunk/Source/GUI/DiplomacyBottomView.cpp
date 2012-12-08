// DiplomacyBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "DiplomacyBottomView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"
#include "General/ResourceManager.h"

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
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;
	// TODO: add draw code here

	// Doublebuffering wird initialisiert
	CMyMemDC pDC(dc);
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

	CString sPrefix = pMajor->GetPrefix();
	Color color;
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrGalaxySectorText);
	fontBrush.SetColor(color);
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "diplomacyV3.boj");

	// Grafik zeichnen
	if (graphic)
	{
		g.DrawImage(graphic, 0, 0, 1075, 249);
		graphic = NULL;
	}

	CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	g.DrawString(CComBSTR(m_strHeadLine), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 30, rect.right, rect.bottom), &fontFormat, &fontBrush);

	if (m_strText.IsEmpty())
		g.DrawString(CComBSTR(CResourceManager::GetString("NO_DIPLOMATIC_NEWS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 30, rect.right, rect.bottom), &fontFormat, &fontBrush);
	m_strHeadLine = "";

	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	fontBrush.SetColor(Color(200,200,250));
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g.DrawString(CComBSTR(m_strText), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 100, rect.right-250, rect.bottom), &fontFormat, &fontBrush);

	g.ReleaseHDC(pDC->GetSafeHdc());
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

BOOL CDiplomacyBottomView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}
