// TradeBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "TradeBottomView.h"
#include "Races\Major.h"

// CTradeBottomView

IMPLEMENT_DYNCREATE(CTradeBottomView, CBottomBaseView)

CTradeBottomView::CTradeBottomView()
{
	m_pSmallButton = NULL;
}

CTradeBottomView::~CTradeBottomView()
{
	if (m_pSmallButton)
	{
		delete m_pSmallButton;
		m_pSmallButton = NULL;
	}
}

BEGIN_MESSAGE_MAP(CTradeBottomView, CBottomBaseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CTradeBottomView drawing

void CTradeBottomView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	// TODO: add draw code here
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Handelhistoryobjekt holen
	CTradeHistory* pHistory = pMajor->GetTrade()->GetTradeHistory();
	ASSERT(pHistory);	

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

	// Wenn wir die Handelsmenüansicht in der Hauptansicht haben
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	CString s;

	Bitmap* graphic = NULL;
	
	CString sPrefix = pMajor->GetPrefix();
	// rassenspezifische Schriftart und Style wählen
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "tradeV3.jpg");
	
	// Grafik zeichnen		
	if (graphic)
	{
		g.DrawImage(graphic, 0, 0, 1075, 249);
		graphic = NULL;
	}

	CString resName;
	Gdiplus::Color resColor;
	switch(m_iWhichRessource)
	{
	case TITAN: resName = CResourceManager::GetString("TITAN");			resColor.SetFromCOLORREF(RGB(186,186,186)); break;
	case DEUTERIUM: resName = CResourceManager::GetString("DEUTERIUM"); resColor.SetFromCOLORREF(RGB(255,81,90)); break;
	case DURANIUM: resName = CResourceManager::GetString("DURANIUM");	resColor.SetFromCOLORREF(RGB(132,198,127)); break;
	case CRYSTAL: resName = CResourceManager::GetString("CRYSTAL");		resColor.SetFromCOLORREF(RGB(153,227,255)); break;
	case IRIDIUM: resName = CResourceManager::GetString("IRIDIUM");		resColor.SetFromCOLORREF(RGB(255,189,76)); break;
	}		
	
	USHORT start;
	USHORT end;
	USHORT backupNumberOfHistoryRounds = m_iNumberOfHistoryRounds;
	// Wenn wir weniger als unsere Anzeige der Runden haben, dann dürfen wir nur bis zu Anzahl der aktuellen
	// Runde zeichnen
	if ((pDoc->GetCurrentRound() - m_iNumberOfHistoryRounds + 1) < 1)
	{
		start = 1;
		end	  = pDoc->GetCurrentRound();
		if (end > start)
			m_iNumberOfHistoryRounds = end-1;
	}
	// ansonsten zeigen wir halt die letzten Runden an, so wie wir diese ausgewählt haben
	else
	{
		start = pDoc->GetCurrentRound() - m_iNumberOfHistoryRounds + 1;
		end   = pDoc->GetCurrentRound();
	}
	UINT max = pHistory->GetMaxPrice(m_iWhichRessource,start-1,end-1);
	// Maximalpreis an die Y-Achse schreiben
	if (max/10 == 0)
		s.Format("1");
	else
		s.Format("%d", max/10);
	
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	fontBrush.SetColor(Color(200,200,200));
	fontFormat.SetAlignment(StringAlignmentFar);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0, r.bottom-215, 290, 25), &fontFormat, &fontBrush);
	
	USHORT count = 0;
	float temp1 = 0.0;
	// Hier wird das Diagramm gezeichnet
	for (int i = start; i <= end; i++)
	{
		temp1 = (float)(pHistory->GetHistoryPriceFromRes(m_iWhichRessource)->GetAt(i-1)) / max;
		// Länge des Diagramms ist 600 Pixel -> bei 20 Runden Abstand 30px, bei 50 Runden Abstand 12px usw.
		if (count > 0)
		{
			float temp2 = (float)(pHistory->GetHistoryPriceFromRes(m_iWhichRessource)->GetAt(i-2)) / max;
			g.DrawLine(&Gdiplus::Pen(resColor), (int)(302+(count-1)*(600/m_iNumberOfHistoryRounds)), (int)(r.bottom-150*temp2+2-60), (int)(302+count*(600/m_iNumberOfHistoryRounds)), (int)(r.bottom-150*temp1+2-60));			
		}
		if (m_bDrawLittleRects)
			g.FillRectangle(&Gdiplus::SolidBrush(resColor), RectF(300+count*(600/m_iNumberOfHistoryRounds), r.bottom-150*temp1-60, 5, 5));
		count++;		
	}
	m_iNumberOfHistoryRounds = backupNumberOfHistoryRounds;	
	
	Gdiplus::Color fontColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, fontColor);
	fontBrush.SetColor(fontColor);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
			
	// Maximal, Minimal und Durchschnittspreis links anzeigen
	g.DrawString(CResourceManager::GetString("MIN_PRICE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 60, 180, 25), &fontFormat, &fontBrush);
	if (pHistory->GetMinPrice(m_iWhichRessource) / 10 == 0)
		s.Format("1");
	else
		s.Format("%d", pHistory->GetMinPrice(m_iWhichRessource) / 10);
	fontFormat.SetAlignment(StringAlignmentFar);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 60, 200, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentNear);
	g.DrawString(CResourceManager::GetString("MAX_PRICE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 90, 180, 25), &fontFormat, &fontBrush);
	if (pHistory->GetMaxPrice(m_iWhichRessource) / 10 == 0)
		s.Format("1");
	else
		s.Format("%d", pHistory->GetMaxPrice(m_iWhichRessource) / 10);
	fontFormat.SetAlignment(StringAlignmentFar);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 90, 200, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentNear);
	g.DrawString(CResourceManager::GetString("AVERAGE_PRICE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 120, 180, 25), &fontFormat, &fontBrush);		
	if (pHistory->GetAveragePrice(m_iWhichRessource) / 10 == 0)
		s.Format("1");
	else
		s.Format("%d", pHistory->GetAveragePrice(m_iWhichRessource) / 10);
	fontFormat.SetAlignment(StringAlignmentFar);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 120, 200, 25), &fontFormat, &fontBrush);
	
	// Namen der Ressource zeichnen
	fontBrush.SetColor(Color(200,200,200));
	fontFormat.SetAlignment(StringAlignmentCenter);
	g.DrawString(resName.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(40, 20, 200, 45), &fontFormat, &fontBrush);
	
	// ganzen kleine Buttons darstellen
	fontBrush.SetColor(this->GetFontColorForSmallButton());
	for (int i = TITAN; i <= IRIDIUM; i++)
		g.DrawImage(m_pSmallButton, r.right-120, 50+i*32, 120, 30);
	g.DrawString(CResourceManager::GetString("TITAN").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120, 50, 120, 30), &fontFormat, &fontBrush);
	g.DrawString(CResourceManager::GetString("DEUTERIUM").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120, 82, 120, 30), &fontFormat, &fontBrush);
	g.DrawString(CResourceManager::GetString("DURANIUM").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120, 114, 120, 30), &fontFormat, &fontBrush);
	g.DrawString(CResourceManager::GetString("CRYSTAL").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120, 146, 120, 30), &fontFormat, &fontBrush);
	g.DrawString(CResourceManager::GetString("IRIDIUM").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120, 178, 120, 30), &fontFormat, &fontBrush);
	
	g.DrawImage(m_pSmallButton, 80, 160, 120, 30);
	s.Format("%d %s",m_iNumberOfHistoryRounds, CResourceManager::GetString("ROUNDS"));
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(80, 160, 120, 30), &fontFormat, &fontBrush);
			
	// Wenn das System blockiert wird, ein OverlayBanner über die Ansicht gelegt.
	if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBlockade() > NULL)
	{
		CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
		CSize viewSize(m_TotalSize.cx - 160, m_TotalSize.cy - 120);
		s.Format("%d", pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBlockade());
		COverlayBanner* banner = new COverlayBanner(CPoint(80,60), viewSize, CResourceManager::GetString("SYSTEM_IS_BLOCKED", FALSE, s), RGB(200,0,0));
		banner->SetBorderWidth(1);
		Gdiplus::Font font(fontName.AllocSysString(), fontSize);
		banner->Draw(&g, &font);
		delete banner;
	}
}

// CTradeBottomView diagnostics

#ifdef _DEBUG
void CTradeBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CTradeBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTradeBottomView message handlers

void CTradeBottomView::OnInitialUpdate()
{
	CBottomBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CString sPrefix = pMajor->GetPrefix();
	CString s = *((CBotf2App*)AfxGetApp())->GetPath() + "Graphics\\Other\\" + sPrefix + "button_small.png";		
	
	m_pSmallButton = Bitmap::FromFile(s.AllocSysString());

	m_iNumberOfHistoryRounds = 20;
	m_iWhichRessource = TITAN;
	m_bDrawLittleRects = FALSE;
}

BOOL CTradeBottomView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

void CTradeBottomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);

	// Wenn wir in der Hauptansicht im Handelsmenü sind, dann hier das Diagramm
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	// checken ob wir uns eine andere Ressource anschauen wollen
	for (int i = TITAN; i <= IRIDIUM; i++)
		if (CRect(r.right-120,50+i*32,r.right,80+i*32).PtInRect(point))
		{
			m_iWhichRessource = i;
			Invalidate(FALSE);
			break;
		}
	// checken ob wir die Anzahl der Runden ändern möchten
	if (CRect(80,160,200,190).PtInRect(point))
	{
		if (m_iNumberOfHistoryRounds == 20)
			m_iNumberOfHistoryRounds = 50;
		else if (m_iNumberOfHistoryRounds == 50)
			m_iNumberOfHistoryRounds = 100;
		else
			m_iNumberOfHistoryRounds = 20;
		Invalidate(FALSE);			
	}
	// Haben wir in das Diagramm geklickt um die kleinen Vierecke an- oder auszuschalten
	else if (CRect(300,r.bottom-225,900,r.bottom-55).PtInRect(point))
	{
		m_bDrawLittleRects = !m_bDrawLittleRects;
		Invalidate(FALSE);
	}

	CBottomBaseView::OnLButtonDown(nFlags, point);
}