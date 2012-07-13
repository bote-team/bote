// IntelBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "IntelBottomView.h"
#include "IntelMenuView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"

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
	Bitmap* graphic = NULL;

	Color color;
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrGalaxySectorText);

	fontBrush.SetColor(color);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + pMajor->GetPrefix() + "diplomacyV3.boj");

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
		short n = pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetActiveReport();
		if (n != -1)
		{
			CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			CIntelObject* report = pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(n);
			CString s;
			if (report->GetIsSpy())
				s = CResourceManager::GetString("SPY");
			else
				s = CResourceManager::GetString("SABOTAGE");
			g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 40, r.right-100, r.bottom-20), &fontFormat, &fontBrush);

			CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
			fontBrush.SetColor(Color(200,200,250));
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			if (report->GetOwner() == pMajor->GetRaceID())
				s = *report->GetOwnerDesc();
			else
				s = *report->GetEnemyDesc();
			g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40, 100, r.right-250, r.bottom-20), &fontFormat, &fontBrush);
		}
	}
	g.ReleaseHDC(pDC->GetSafeHdc());
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
