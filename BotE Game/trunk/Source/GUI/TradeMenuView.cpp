// TradeMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "TradeMenuView.h"
#include "MenuChooseView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTradeMenuView
IMPLEMENT_DYNCREATE(CTradeMenuView, CMainBaseView)

BEGIN_MESSAGE_MAP(CTradeMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


CTradeMenuView::CTradeMenuView() :
	bg_trademenu(),
	bg_monopolmenu(),
	bg_tradetransfermenu(),
	bg_empty1(),
	m_bySubMenu(0),
	m_bCouldBuyMonopols(FALSE)
{
	memset(m_dMonopolCosts, 0.0f, sizeof(m_dMonopolCosts));
}

CTradeMenuView::~CTradeMenuView()
{
	for (int i = 0; i < m_TradeMainButtons.GetSize(); i++)
	{
		delete m_TradeMainButtons[i];
		m_TradeMainButtons[i] = 0;
	}
	m_TradeMainButtons.RemoveAll();
}

// CTradeMenuView drawing
void CTradeMenuView::OnDraw(CDC* dc)
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

	// ***************************** DIE HANDELSANSICHT ZEICHNEN **********************************
	// System einstellen, in welchem wir handeln möchten
	if (pDoc->m_Systems.at(pDoc->GetKO().x+(pDoc->GetKO().y)*STARMAP_SECTORS_HCOUNT).GetOwnerOfSystem() != pMajor->GetRaceID())
	{
		if (pMajor->GetEmpire()->GetSystemList()->GetSize() > 0)
			pDoc->SetKO(pMajor->GetEmpire()->GetSystemList()->GetAt(0).ko.x, pMajor->GetEmpire()->GetSystemList()->GetAt(0).ko.y);
	}

	// normales Handelsmenü
	if (m_bySubMenu == 0)
		DrawGlobalTradeMenue(&g);
	// Handelsmonopolemenü
	else if (m_bySubMenu == 1)
		DrawMonopolMenue(&g);
	// Transfermenü
	else if (m_bySubMenu == 2)
		DrawTradeTransferMenue(&g);

	DrawTradeMainButtons(&g, pMajor);
	// ************** DIE HANDELSANSICHT ZEICHNEN ist hier zu Ende **************
	g.ReleaseHDC(pDC->GetSafeHdc());
}


// CTradeMenuView diagnostics

#ifdef _DEBUG
void CTradeMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CTradeMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTradeMenuView message handlers

void CTradeMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	// Handelsansicht
	m_bySubMenu = 0;
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CTradeMenuView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// Alle Buttons in der View erstellen
	CreateButtons();

	// alle Hintergrundgrafiken laden
	CString sPrefix = pMajor->GetPrefix();

	bg_trademenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "trademenu.boj");
	bg_monopolmenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "monopolmenu.boj");
	bg_empty1			= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "emptyscreen.boj");
	bg_tradetransfermenu= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "tradetransfermenu.boj");
}

void CTradeMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CTradeMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Handelsmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CTradeMenuView::DrawGlobalTradeMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	if (bg_trademenu)
		g->DrawImage(bg_trademenu, 0, 0, 1075, 750);

	CString s;

	// Hier die Rechtecke in denen wir den Preis der Ressourcen sehen und wo sich auch die Aktionsbuttons befinden
	RectF resRect[5];
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		resRect[i] = RectF(40+i*200,150,190,340);
		// "aktueller Kurs" in die Mitte der Rechtecke schreiben
		fontBrush.SetColor(markColor);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CResourceManager::GetString("CURRENT_PRICE");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40+i*200,190,190,40), &fontFormat, &fontBrush);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

		// "Lagermenge" hinschreiben
		s = CResourceManager::GetString("STORAGE_QUANTUM");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40+i*200,350,190,25), &fontFormat, &fontBrush);

		// "Monopolist" hinschreiben
		s = CResourceManager::GetString("MONOPOLIST");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40+i*200,420,190,25), &fontFormat, &fontBrush);

		// den Kurs auch hinzuschreiben
		fontBrush.SetColor(normalColor);
		s.Format("%d %s",(int)ceil((pMajor->GetTrade()->GetRessourcePrice()[i]) * pMajor->GetTrade()->GetTax()) / 10,
			CResourceManager::GetString("CREDITS"));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40+i*200,230,190,25), &fontFormat, &fontBrush);

		// Lagermenge im aktuellen System von der Ressource hinschreiben
		s.Format("%d %s",pDoc->m_Systems.at(pDoc->GetKO().x+(pDoc->GetKO().y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(i),CResourceManager::GetString("UNITS"));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40+i*200,385,190,25), &fontFormat, &fontBrush);

		// den Monopolbesitzer auch hinschreiben
		if (CTrade::GetMonopolOwner(i).IsEmpty())
		{
			s = CResourceManager::GetString("NOBODY");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40+i*200,455,190,25), &fontFormat, &fontBrush);
		}
		else if (CTrade::GetMonopolOwner(i) == pMajor->GetRaceID() || pMajor->IsRaceContacted(CTrade::GetMonopolOwner(i)) == true)
		{
			CRace* pMonopolist = pDoc->GetRaceCtrl()->GetRace(CTrade::GetMonopolOwner(i));
			if (pMonopolist)
			{
				s = pMonopolist->GetRaceName();
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40+i*200,455,190,25), &fontFormat, &fontBrush);
			}
		}
		else
		{
			s = CResourceManager::GetString("UNKNOWN");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40+i*200,455,190,25), &fontFormat, &fontBrush);
		}
	}

	// Namen der Rohstoffe in die Rechtecke schreiben
	fontFormat.SetLineAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(CResourceManager::GetString("TITAN")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), resRect[TITAN], &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("DEUTERIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), resRect[DEUTERIUM], &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("DURANIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), resRect[DURANIUM], &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("CRYSTAL")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), resRect[CRYSTAL], &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("IRIDIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), resRect[IRIDIUM], &fontFormat, &fontBrush);

	// unsere Handelssteuer zeichnen
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	CString sTax;
	sTax.Format("%.0f",(pMajor->GetTrade()->GetTax() - 1) * 100);
	s = CResourceManager::GetString("ALL_PRICES_INCL_TAX", FALSE, sTax);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,500,m_TotalSize.cx,25), &fontFormat, &fontBrush);

	// Kaufmenge ändern Text
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	s = CResourceManager::GetString("REPEAT_ACTIVITY");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(400,535,275,40), &fontFormat, &fontBrush);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	// Buttons für "Kaufen" und "Verkaufen zeichnen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);

	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		if (graphic)
			g->DrawImage(graphic, 75+i*200, 275, 120, 30);
		s = CResourceManager::GetString("BTN_BUY");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(75+i*200,275,120,30), &fontFormat, &btnBrush);

		if (graphic)
			g->DrawImage(graphic, 75+i*200, 310, 120, 30);
		s = CResourceManager::GetString("BTN_SELL");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(75+i*200,310,120,30), &fontFormat, &btnBrush);
	}
	// Button für Kaufmenge ändern hinmalen
	if (graphic)
		g->DrawImage(graphic, 478, 580, 120, 30);
	s.Format("%dx",pMajor->GetTrade()->GetQuantity()/100);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(478,580,120,30), &fontFormat, &btnBrush);

	// "globale Handelsboerse" in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("GLOBAL_TRADE_MENUE");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);

	// System, in dem die Handelsaktivitäten stattfinden sollen hinschreiben
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontBrush.SetColor(markColor);
	s.Format("%s: %s",CResourceManager::GetString("TRADE_IN_SYSTEM"), pDoc->m_Sectors.at(pDoc->GetKO().x+(pDoc->GetKO().y)*STARMAP_SECTORS_HCOUNT).GetName());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,90,m_TotalSize.cx,45), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Monopolmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CTradeMenuView::DrawMonopolMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	CString s;

	// Wir können erst ein Monopol kaufen, wenn wir mindst. die Hälfte der Hauptrassen kennen
	m_bCouldBuyMonopols = FALSE;
	USHORT otherEmpiresInGame = 0;
	USHORT otherKnownEmpires  = 0;

	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (it->second->GetEmpire()->GetNumberOfSystems() > 0 && it->first != pMajor->GetRaceID())
		{
			otherEmpiresInGame++;
			if (pMajor->IsRaceContacted(it->first) == true)
				otherKnownEmpires++;
		}
	// mindts. die Hälfte anderer Imperien müssen wir kennen um ein Monopol kaufen zu können
	if (otherKnownEmpires > (float)(otherEmpiresInGame / 2))
	{
		if (bg_monopolmenu)
			g->DrawImage(bg_monopolmenu, 0, 0, 1075, 750);
		m_bCouldBuyMonopols = TRUE;
		for (int i = TITAN; i <= IRIDIUM; i++)
		{
			fontBrush.SetColor(normalColor);
			m_dMonopolCosts[i] = 0.0f;
			CString resName;
			switch(i)
			{
			case TITAN: resName = CResourceManager::GetString("TITAN"); break;
			case DEUTERIUM: resName = CResourceManager::GetString("DEUTERIUM"); break;
			case DURANIUM: resName = CResourceManager::GetString("DURANIUM"); break;
			case CRYSTAL: resName = CResourceManager::GetString("CRYSTAL"); break;
			case IRIDIUM: resName = CResourceManager::GetString("IRIDIUM"); break;
			}
			fontFormat.SetAlignment(StringAlignmentNear);
			s.Format("%s %s:",CResourceManager::GetString("MONOPOLY_OWNER"), resName);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40,120+i*110,360,30), &fontFormat, &fontBrush);

			if (CTrade::GetMonopolOwner(i).IsEmpty())
				s = CResourceManager::GetString("NOBODY");
			else if (CTrade::GetMonopolOwner(i) == pMajor->GetRaceID() || pMajor->IsRaceContacted(CTrade::GetMonopolOwner(i)) == true)
			{
				CRace* pMonopol = pDoc->GetRaceCtrl()->GetRace(CTrade::GetMonopolOwner(i));
				if (pMonopol)
					s.Format("%s",pMonopol->GetRaceName());
			}
			else
				s = CResourceManager::GetString("UNKNOWN");
			fontBrush.SetColor(markColor);
			fontFormat.SetAlignment(StringAlignmentFar);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40,120+i*110,360,30), &fontFormat, &fontBrush);

			fontBrush.SetColor(normalColor);
			fontFormat.SetAlignment(StringAlignmentNear);
			s.Format("%s %s:",CResourceManager::GetString("MONOPOLY_COSTS"), resName);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(500,120+i*110,350,30), &fontFormat, &fontBrush);

			// Die Monopolkosten sind die gesamten Einwohner aller Systeme unserer und uns bekannter Rassen mal die Nummer
			// des Rohstoffes mal 15. Gehört jemand schon das Monopol und wir wollen das Wegkaufen, dann müssen wir
			// den doppelten Preis bezahlen
			for (int y = 0; y < 20; y++)
				for (int x = 0; x < 30; x++)
					if (pDoc->m_Systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetOwnerOfSystem() != "" && (pDoc->m_Systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetOwnerOfSystem() == pMajor->GetRaceID() || pMajor->IsRaceContacted(pDoc->m_Systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetOwnerOfSystem()) == TRUE))
						m_dMonopolCosts[i] += pDoc->m_Systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetHabitants();
			// Wenn wir das Monopol schon besitzen oder es in der Runde schon gekauft haben
			if (CTrade::GetMonopolOwner(i) == pMajor->GetRaceID() || pMajor->GetTrade()->GetMonopolBuying()[i] != 0.0f)
				m_dMonopolCosts[i] = 0.0f;
			// Wenn eine andere Hauptrasse das Monopol besitzt
			else if (CTrade::GetMonopolOwner(i).IsEmpty() == false)
				m_dMonopolCosts[i] *= 2;
			m_dMonopolCosts[i] *= 15 * (i+1);	// Monopolkosten festsetzen

			///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
			// Hier die Boni durch die Uniqueforschung "Handel" -> keine Handelsgebühr
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(2) == RESEARCH_STATUS::RESEARCHED)
				m_dMonopolCosts[i] -= m_dMonopolCosts[i] * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(2) / 100;

			fontBrush.SetColor(markColor);
			fontFormat.SetAlignment(StringAlignmentFar);
			s.Format("%.0lf %s",m_dMonopolCosts[i], CResourceManager::GetString("CREDITS"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(500,120+i*110,350,30), &fontFormat, &fontBrush);
		}

		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = CResourceManager::GetString("BTN_BUY");
		for (int i = TITAN; i <= IRIDIUM; i++)
		{
			// kleine Kauf-Buttons einblenden, wenn wir ein Monopol kaufen können
			if (m_dMonopolCosts[i] != 0.0f)
			{
				if (graphic)
					g->DrawImage(graphic, 915, 120+i*110, 120, 30);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(915,120+i*110,120,30), &fontFormat, &btnBrush);
			}
		}
	}


	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	// Wir kennen nicht genügend andere Imperien, somit können wir kein Monopol kaufen
	if (m_bCouldBuyMonopols == FALSE)
	{
		// emptyScreen zeichnen
		if (bg_empty1)
			g->DrawImage(bg_empty1, 0, 0, 1075, 750);
		s = CResourceManager::GetString("NO_MONOPOLY_BUY");
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(200,250,675,250), &fontFormat, &fontBrush);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	}
	// "Handelsmonopole" in der Mitte zeichnen
	s = CResourceManager::GetString("MONOPOLY_MENUE");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Transfermenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CTradeMenuView::DrawTradeTransferMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	SolidBrush fontBrush(markColor);

	CString s;

	if (bg_tradetransfermenu)
		g->DrawImage(bg_tradetransfermenu, 0, 0, 1075, 750);

	// hier mal alle Transfers hinschreiben
	long boughtResPrice[5];
	long boughtResNumber[5];
	long selledResPrice[5];
	long selledResNumber[5];
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		boughtResPrice[i]	= 0;
		boughtResNumber[i]	= 0;
		selledResPrice[i]	= 0;
		selledResNumber[i]	= 0;
	}
	// Transfers berechnen
	for (int i = 0; i < pMajor->GetTrade()->GetTradeActions()->GetSize(); i++)
		if (pMajor->GetTrade()->GetTradeActions()->GetAt(i).system == pDoc->GetKO())
		{
			USHORT res = pMajor->GetTrade()->GetTradeActions()->GetAt(i).res;
			// Ressourcen die gekauft werden
			if (pMajor->GetTrade()->GetTradeActions()->GetAt(i).price > 0)
			{
				boughtResNumber[res] += pMajor->GetTrade()->GetTradeActions()->GetAt(i).number;
				boughtResPrice[res]  += pMajor->GetTrade()->GetTradeActions()->GetAt(i).price;
			}
			// Ressourcen die verkauft werden
			else
			{
				selledResNumber[res] += pMajor->GetTrade()->GetTradeActions()->GetAt(i).number;
				selledResPrice[res]  += pMajor->GetTrade()->GetTradeActions()->GetAt(i).price;
			}
		}
	// berechnete und zusammengefasste Transfers hinschreiben
	g->DrawString(CComBSTR(CResourceManager::GetString("TITAN")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,250,m_TotalSize.cx,25), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("DEUTERIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,320,m_TotalSize.cx,25), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("DURANIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,390,m_TotalSize.cx,25), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("CRYSTAL")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,460,m_TotalSize.cx,25), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("IRIDIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,530,m_TotalSize.cx,25), &fontFormat, &fontBrush);

	fontBrush.SetColor(normalColor);
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		// Alle gekauften
		if (boughtResPrice[i] > 0)
		{
			s.Format("%d %s %d %s",boughtResNumber[i], CResourceManager::GetString("UNITS_FOR"),
				(int)ceil((boughtResPrice[i]) * pMajor->GetTrade()->GetTax()), CResourceManager::GetString("CREDITS"));
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(210,250+i*70,865,25), &fontFormat, &fontBrush);
		}
		// Alle verkauften
		if (selledResPrice[i] < 0)
		{
			s.Format("%d %s %d %s",selledResNumber[i], CResourceManager::GetString("UNITS_FOR"),
				-selledResPrice[i], CResourceManager::GetString("CREDITS"));
			fontFormat.SetAlignment(StringAlignmentFar);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,250+i*70,865,25), &fontFormat, &fontBrush);
		}
	}
	fontFormat.SetAlignment(StringAlignmentCenter);

	// "anstehender Ressourcentransfer" in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("TRADE_TRANSFER_MENUE");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);

	// System, in dem die Handelsaktivitäten stattfinden werden
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontBrush.SetColor(markColor);
	s = CResourceManager::GetString("TRANSFERS_NEXT_ROUND",FALSE,pDoc->m_Sectors.at(pDoc->GetKO().x+(pDoc->GetKO().y)*STARMAP_SECTORS_HCOUNT).GetName());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,90,m_TotalSize.cx,45), &fontFormat, &fontBrush);
}

/// Funktion zeichnet die Buttons unter den Handelsmenüs.
/// @param g Zeiger auf GDI+ Grafikobjekt
/// @param pMajor Spielerrasse
void CTradeMenuView::DrawTradeMainButtons(Graphics* g, CMajor* pMajor)
{
	CString fontName;
	REAL fontSize;
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush fontBrush(btnColor);
	DrawGDIButtons(g, &m_TradeMainButtons, m_bySubMenu, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}

void CTradeMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

	// Checken ob ich auf irgendeinen Button geklickt habe um in ein anderes Untermenü des Handelsmenüs komme
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_TradeMainButtons, temp))
	{
		m_bySubMenu = temp;
		return;
	}


	// Wenn wir in der globalen Handelsansicht sind
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	if (m_bySubMenu == 0)
	{
		// Checken ob wir auf den Button geklickt haben um die Kaufmenge zu ändern
		if (CRect(r.left+478,r.top+580,r.left+598,r.top+610).PtInRect(point))
		{
			switch(pMajor->GetTrade()->GetQuantity())
			{
			case 100:	pMajor->GetTrade()->SetQuantity(1000);	 break;
			case 1000:	pMajor->GetTrade()->SetQuantity(10000);	 break;
			case 10000: pMajor->GetTrade()->SetQuantity(100);	 break;
			}
			Invalidate(FALSE);
			return;
		}
		// Wenn das System blockiert wird, so kann man an der globalen Handelsbörse keine Käufe und Verkäufe tätigen
		if (pDoc->m_Systems.at(pDoc->GetKO().x+(pDoc->GetKO().y)*STARMAP_SECTORS_HCOUNT).GetBlockade() > NULL)
			return;
		// Checken ob wir auf irgendeinen Kaufen- oder Verkaufenbutton geklickt haben
		for (int i = TITAN; i <= IRIDIUM; i++)
		{
			// Überprüfen, ob wir auf einen Kaufenbutton geklickt haben
			if (CRect(r.left+75+i*200,r.top+275,r.left+195+i*200,r.top+305).PtInRect(point))
			{
				int costs = pMajor->GetTrade()->BuyRessource(i, pMajor->GetTrade()->GetQuantity(),
					pDoc->GetKO(),pMajor->GetEmpire()->GetCredits());
				// Wenn wir soviel Credits haben um etwas zu kaufen -> also costs != NULL)
				if (costs != 0)
				{
					pMajor->GetEmpire()->SetCredits(-costs);
					CSoundManager::GetInstance()->PlaySound(SNDMGR_SOUND_SHIPTARGET);
					Invalidate(FALSE);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				return;
			}
			// Überprüfen ob wir auf einen Verkaufenbutton geklickt haben
			else if (CRect(r.left+75+i*200,r.top+310,r.left+195+i*200,r.top+340).PtInRect(point))
			{
				// Überprüfen, das wir bei einem Verkauf nicht mehr Ressourcen aus dem System nehmen als im Lager vorhanden sind
				if (pDoc->m_Systems.at(pDoc->GetKO().x+(pDoc->GetKO().y)*STARMAP_SECTORS_HCOUNT).GetResourceStore(i) >= pMajor->GetTrade()->GetQuantity())
				{
					pMajor->GetTrade()->SellRessource(i, pMajor->GetTrade()->GetQuantity(),pDoc->GetKO());
					// Ressource aus dem Lager nehmen
					pDoc->m_Systems.at(pDoc->GetKO().x+(pDoc->GetKO().y)*STARMAP_SECTORS_HCOUNT).SetResourceStore(i,-pMajor->GetTrade()->GetQuantity());
					CSoundManager::GetInstance()->PlaySound(SNDMGR_SOUND_SHIPTARGET);
					Invalidate(FALSE);
					return;
				}
			}
		}
	}
	// Monopoluntermenü
	else if (m_bySubMenu == 1)
	{
		for (int i = TITAN; i <= IRIDIUM; i++)
			if (m_bCouldBuyMonopols == TRUE && m_dMonopolCosts[i] != 0.0f && CRect(r.left+915,r.top+120+i*110,r.left+1035,r.top+150+i*110).PtInRect(point))
			{
				if (pMajor->GetEmpire()->GetCredits() >= m_dMonopolCosts[i])
				{
					pMajor->GetTrade()->SetMonopolBuying(i,m_dMonopolCosts[i]);
					pMajor->GetEmpire()->SetCredits((long)-m_dMonopolCosts[i]);
					Invalidate(FALSE);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				break;
			}
	}
	// Transferuntermenü
	else if (m_bySubMenu == 2)
	{

	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CTradeMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);

	ButtonReactOnMouseOver(point, &m_TradeMainButtons);

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CTradeMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	// nur wenn nicht im Monopolmenü
	if (m_bySubMenu == 1)
		return;

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Wenn wir in irgendeinem System sind, können wir mit dem links rechts Pfeil zu einem anderen System wechseln
	CPoint p = pDoc->GetKO();
	if (nChar == VK_RIGHT || nChar == VK_LEFT)
	{
		// an welcher Stelle in der Liste befindet sich das aktuelle System?
		short pos = 0;
		for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
			if (pMajor->GetEmpire()->GetSystemList()->GetAt(i).ko == p)
			{
				pos = i;
				break;
			}
		if (nChar == VK_RIGHT)
		{
			pos++;
			if (pos == pMajor->GetEmpire()->GetSystemList()->GetSize())
				pos = 0;
		}
		else
		{
			pos--;
			if (pos < 0)
				pos = pMajor->GetEmpire()->GetSystemList()->GetUpperBound();
		}
		if (pMajor->GetEmpire()->GetSystemList()->GetSize() > 1)
		{
			pDoc->SetKO(pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.x, pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.y);
			Invalidate(FALSE);
		}
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTradeMenuView::CreateButtons()
{
	ASSERT((CBotf2Doc*)GetDocument());

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CString sPrefix = pMajor->GetPrefix();

	// alle Buttons in der View anlegen und Grafiken laden
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	// Buttons in den Diplomatieansichten
	m_TradeMainButtons.Add(new CMyButton(CPoint(288,690) , CSize(160,40), CResourceManager::GetString("BTN_STOCK_EXCHANGE"), fileN, fileI, fileA));
	m_TradeMainButtons.Add(new CMyButton(CPoint(458,690) , CSize(160,40), CResourceManager::GetString("BTN_MONOPOLY"), fileN, fileI, fileA));
	m_TradeMainButtons.Add(new CMyButton(CPoint(628,690) , CSize(160,40), CResourceManager::GetString("BTN_TRANSFERS"), fileN, fileI, fileA));
}
