// ShipDesignBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "ShipDesignBottomView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"
#include "General/Loc.h"
#include "GraphicPool.h"

// CShipDesignBottomView

IMPLEMENT_DYNCREATE(CShipDesignBottomView, CBottomBaseView)

CShipDesignBottomView::CShipDesignBottomView()
{
	m_nCurrentShipInfo = 0;
}

CShipDesignBottomView::~CShipDesignBottomView()
{
}

BEGIN_MESSAGE_MAP(CShipDesignBottomView, CBottomBaseView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CShipDesignBottomView drawing

void CShipDesignBottomView::OnDraw(CDC* dc)
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

	// Soll was über die Diplomatie angezeigt werden
	CRect rect;
	rect.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);

	CString sPrefix = pMajor->GetPrefix();
	Color color;
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrGalaxySectorText);
	fontBrush.SetColor(color);
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "researchV3.boj");

	// Grafik zeichnen
	if (graphic)
	{
		g.DrawImage(graphic, 0, 0, 1075, 249);
		graphic = NULL;
	}

	// Klassenname und Beschreibung des Schiffes anzeigen
	CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	CString s;
	s.Format("%s %s %s-%s", pDoc->m_ShipInfoArray.GetAt(m_nCurrentShipInfo).GetShipTypeAsString(), CLoc::GetString("MASC_ARTICLE"),
		pDoc->m_ShipInfoArray.GetAt(m_nCurrentShipInfo).GetShipClass(), CLoc::GetString("CLASS"));
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 30, rect.right - 90, rect.bottom - 30), &fontFormat, &fontBrush);

	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	fontBrush.SetColor(Color(200,200,250));
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	s = pDoc->m_ShipInfoArray.GetAt(m_nCurrentShipInfo).GetDescription();
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 100, rect.right - 90, rect.bottom - 50), &fontFormat, &fontBrush);

	g.ReleaseHDC(pDC->GetSafeHdc());
}


// CShipDesignBottomView diagnostics

#ifdef _DEBUG
void CShipDesignBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CShipDesignBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CShipDesignBottomView message handlers

BOOL CShipDesignBottomView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}
