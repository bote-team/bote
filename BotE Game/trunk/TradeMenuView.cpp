// TradeMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "TradeMenuView.h"
#include "MenuChooseView.h"


// CTradeMenuView

IMPLEMENT_DYNCREATE(CTradeMenuView, CMainBaseView)

CTradeMenuView::CTradeMenuView()
{

}

CTradeMenuView::~CTradeMenuView()
{
}

BEGIN_MESSAGE_MAP(CTradeMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CTradeMenuView drawing

void CTradeMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here
	CMemDC pDC(dc);
	pDC->SetBkMode(TRANSPARENT);
	if (pDoc->m_pIniLoader->GetValue("SMOOTHSCALING"))
		pDC->SetStretchBltMode(HALFTONE);
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	// ***************************** DIE HANDELSANSICHT ZEICHNEN **********************************
	// System einstellen, in welchem wir handeln möchten
	if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetOwnerOfSystem() != pDoc->GetPlayersRace())
	{
		if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetSize() > 0)
			pDoc->SetKO(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetAt(0).ko.x, pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetAt(0).ko.y);
	}

	LoadRaceFont(pDC);
	// normales Handelsmenü
	if (m_bySubMenu == 0)
		DrawGlobalTradeMenue(pDC,r);
	// Handelsmonopolemenü
	else if (m_bySubMenu == 1)
		DrawMonopolMenue(pDC,r);
	// Transfermenü
	else if (m_bySubMenu == 2)
		DrawTradeTransferMenue(pDC,r);
	// ************** DIE HANDELSANSICHT ZEICHNEN ist hier zu Ende **************
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
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	// Alle Buttons in der View erstellen
	CreateButtons();
	
	// alle Hintergrundgrafiken laden
	bg_trademenu.DeleteObject();
	bg_monopolmenu.DeleteObject();
	bg_tradetransfermenu.DeleteObject();
	bg_empty1.DeleteObject();
	
	CString race;
	switch (pDoc->GetPlayersRace())
	{
	case HUMAN:		race = CResourceManager::GetString("RACE1_PREFIX"); break;
	case FERENGI:	race = CResourceManager::GetString("RACE2_PREFIX"); break;
	case KLINGON:	race = CResourceManager::GetString("RACE3_PREFIX"); break;
	case ROMULAN:	race = CResourceManager::GetString("RACE4_PREFIX"); break;
	case CARDASSIAN:race = CResourceManager::GetString("RACE5_PREFIX"); break;
	case DOMINION:	race = CResourceManager::GetString("RACE6_PREFIX"); break;
	}
	FCObjImage img;
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"trademenu.jpg");
	bg_trademenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"monopolmenu.jpg");
	bg_monopolmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"tradetransfermenu.jpg");
	bg_tradetransfermenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"emptyscreen.jpg");
	bg_empty1.Attach(FCWin32::CreateDDBHandle(img));
	img.Destroy();

	// Handelsansicht
	m_bySubMenu = 0;
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
void CTradeMenuView::DrawGlobalTradeMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r = theClientRect;
	CString s;
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_trademenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
	
	// grobe Linien zeichnen
	COLORREF oldColor = pDC->GetTextColor();
/*	CPen mark(PS_SOLID, 1, RGB(255,125,0));
	pDC->SelectObject(&mark);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-50);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-50);
*/
	// Hier die Rechtecke in denen wir den Preis der Ressourcen sehen und wo sich auch die Aktionsbuttons befinden
	CRect resRect[5];
	CBrush nb(RGB(40,40,40));
	pDC->SelectObject(&nb);
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		resRect[i].SetRect(r.left+40+i*200,r.top+150,r.left+230+i*200,r.top+490);
	//	pDC->Rectangle(&resRect[i]);
		// "aktueller Kurs" in die Mitte der Rechtecke schreiben
		pDC->SetTextColor(RGB(200,200,200));
		pDC->DrawText(CResourceManager::GetString("CURRENT_PRICE"), CRect(r.left+40+i*200,r.top+190,r.left+230+i*200,r.top+230), DT_CENTER | DT_VCENTER | DT_WORDBREAK);
		// "Lagermenge" hinschreiben
		pDC->DrawText(CResourceManager::GetString("STORAGE_QUANTUM"), CRect(r.left+40+i*200,r.top+350,r.left+230+i*200,r.top+375), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		// "Monopolist" hinschreiben
		pDC->DrawText(CResourceManager::GetString("MONOPOLIST"), CRect(r.left+40+i*200,r.top+420,r.left+230+i*200,r.top+445), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		// den Kurs auch hinzuschreiben
		pDC->SetTextColor(oldColor);
		s.Format("%d %s",(int)ceil((pDoc->m_Trade[pDoc->GetPlayersRace()].GetRessourcePrice()[i]) * pDoc->m_Trade[pDoc->GetPlayersRace()].GetTax()) / 10,
			CResourceManager::GetString("LATINUM"));
		pDC->DrawText(s,CRect(r.left+40+i*200,r.top+230,r.left+230+i*200,r.top+255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		// Lagermenge im aktuellen System von der Ressource hinschreiben
		s.Format("%d %s",pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetRessourceStore(i),CResourceManager::GetString("UNITS"));
		pDC->DrawText(s,CRect(r.left+40+i*200,r.top+385,r.left+230+i*200,r.top+410), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		// den Monopolbesitzer auch hinschreiben
		if (CTrade::GetMonopolOwner(i) == NOBODY)
			pDC->DrawText(CResourceManager::GetString("NOBODY"), CRect(r.left+40+i*200,r.top+455,r.left+230+i*200,r.top+480), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		else if (pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(CTrade::GetMonopolOwner(i)) == TRUE || CTrade::GetMonopolOwner(i) == pDoc->GetPlayersRace())
			pDC->DrawText(CString(CMajorRace::GetRaceName(CTrade::GetMonopolOwner(i))),CRect(r.left+40+i*200,r.top+455,r.left+230+i*200,r.top+480), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		else
			pDC->DrawText(CResourceManager::GetString("UNKNOWN"), CRect(r.left+40+i*200,r.top+455,r.left+230+i*200,r.top+480), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	// Namen der Rohstoffe in die Rechtecke schreiben
	pDC->DrawText(CResourceManager::GetString("TITAN"),CRect(resRect[TITAN]),DT_CENTER | DT_TOP | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DEUTERIUM"),CRect(resRect[DEUTERIUM]),DT_CENTER | DT_TOP | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DURANIUM"),CRect(resRect[DURANIUM]),DT_CENTER | DT_TOP | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("CRYSTAL"),CRect(resRect[CRYSTAL]),DT_CENTER | DT_TOP | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("IRIDIUM"),CRect(resRect[IRIDIUM]),DT_CENTER | DT_TOP | DT_SINGLELINE);
	// unsere Handelssteuer zeichnen
	s.Format("%.0f",(pDoc->m_Trade[pDoc->GetPlayersRace()].GetTax() - 1)*100);
	pDC->DrawText(CResourceManager::GetString("ALL_PRICES_INCL_TAX",FALSE,s), CRect(r.left,r.top+500,r.right,r.top+525), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// Kaufmenge ändern Text
	pDC->DrawText(CResourceManager::GetString("REPEAT_ACTIVITY"), CRect(r.left+400,r.top+535,r.right-400,r.top+575), DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	// Buttons für "Kaufen" und "Verkaufen zeichnen
	LoadFontForLittleButton(pDC);
	mdc.SelectObject(bm);
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		pDC->BitBlt(r.left+75+i*200,r.top+275,120,30,&mdc,0,0,SRCCOPY);
		pDC->DrawText(CResourceManager::GetString("BTN_BUY") ,CRect(r.left+75+i*200,r.top+275,r.left+195+i*200,r.top+305),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		pDC->BitBlt(r.left+75+i*200,r.top+310,120,30,&mdc,0,0,SRCCOPY);
		pDC->DrawText(CResourceManager::GetString("BTN_SELL"), CRect(r.left+75+i*200,r.top+310,r.left+195+i*200,r.top+340),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	// Button für Kaufmenge ändern hinmalen
	pDC->BitBlt(r.left+478,r.top+580,120,30,&mdc,0,0,SRCCOPY);
	s.Format("%dx",pDoc->m_Trade[pDoc->GetPlayersRace()].GetQuantity()/100);
	pDC->DrawText(s,CRect(r.left+478,r.top+580,r.left+598,r.top+610), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// "globale Handelsboerse" in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("GLOBAL_TRADE_MENUE") ,CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	
	// System, in dem die Handelsaktivitäten stattfinden sollen hinschreiben
	LoadFontForBigButton(pDC);
	oldColor = pDC->GetTextColor();
	pDC->SetTextColor(RGB(200,200,200));
	s.Format("%s: %s",CResourceManager::GetString("TRADE_IN_SYSTEM"), pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetName());
	if (pDoc->GetPlayersRace() == ROMULAN)
		pDC->DrawText(s,CRect(r.left+250,r.top+80,r.right-250,r.top+135), DT_CENTER | DT_WORDBREAK | DT_VCENTER);
	else
		pDC->DrawText(s,CRect(r.left,r.top+90,r.right,r.top+135), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// Buttons am unteren Bildrand zeichnen
	pDC->SetTextColor(oldColor);
	mdc.SelectObject(bm);
	pDC->BitBlt(288,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_STOCK_EXCHANGE"), CRect(288,690,448,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDC->BitBlt(458,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_MONOPOLY"), CRect(458,690,618,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDC->BitBlt(628,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_TRANSFERS"), CRect(628,690,788,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Monopolmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CTradeMenuView::DrawMonopolMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r = theClientRect;
	CString s;

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
		
	// grobe Linien zeichnen
	COLORREF oldColor = pDC->GetTextColor();
/*	CPen mark(PS_SOLID, 1, RGB(255,125,0));
	pDC->SelectObject(&mark);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-50);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-50);
*/
	// Wir können erst ein Monopol kaufen, wenn wir mindst. die Hälfte der Hauptrassen kennen
	m_bCouldBuyMonopols = FALSE;
	USHORT otherEmpiresInGame = 0;
	USHORT otherKnownEmpires  = 0;
	for (int i = HUMAN; i <= DOMINION; i++)
		if (pDoc->m_Empire[i].GetNumberOfSystems() > 0 && i != pDoc->GetPlayersRace())
		{
			otherEmpiresInGame++;
			if (pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE)
				otherKnownEmpires++;
		}
	// mindts. die Hälfte anderer Imperien müssen wir kennen um ein Monopol kaufen zu können
	if (otherKnownEmpires > (float)(otherEmpiresInGame / 2))
	{
		mdc.SelectObject(bg_monopolmenu);
		pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
		m_bCouldBuyMonopols = TRUE;
		for (int i = TITAN; i <= IRIDIUM; i++)
		{
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
			s.Format("%s %s:",CResourceManager::GetString("MONOPOLY_OWNER"), resName);
			pDC->DrawText(s,CRect(r.left+40,r.top+120+i*110,r.left+400,r.top+150+i*110),DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			if (CTrade::GetMonopolOwner(i) == NOBODY)
				s = CResourceManager::GetString("NOBODY");
			else if (pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(CTrade::GetMonopolOwner(i)) == TRUE || CTrade::GetMonopolOwner(i) == pDoc->GetPlayersRace())
				s.Format("%s",CMajorRace::GetRaceName(CTrade::GetMonopolOwner(i)));
			else
				s = CResourceManager::GetString("UNKNOWN");
			pDC->DrawText(s,CRect(r.left+40,r.top+120+i*110,r.left+400,r.top+150+i*110),DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			s.Format("%s %s:",CResourceManager::GetString("MONOPOLY_COSTS"), resName);
			pDC->DrawText(s,CRect(r.left+500,r.top+120+i*110,r.left+850,r.top+150+i*110),DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			// Die Monopolkosten sind die gesamten Einwohner aller Systeme unserer und uns bekannter Rassen mal die Nummer
			// des Rohstoffes mal 15. Gehört jemand schon das Monopol und wir wollen das Wegkaufen, dann müssen wir
			// den doppelten Preis bezahlen
			for (int y = 0; y < 20; y++)
				for (int x = 0; x < 30; x++)
					if (pDoc->m_System[x][y].GetOwnerOfSystem() != NOBODY && (pDoc->m_System[x][y].GetOwnerOfSystem() == pDoc->GetPlayersRace() || pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(pDoc->m_System[x][y].GetOwnerOfSystem()) == TRUE))
						m_dMonopolCosts[i] += pDoc->m_System[x][y].GetHabitants();
			// Wenn wir das Monopol schon besitzen oder es in der Runde schon gekauft haben
			if (CTrade::GetMonopolOwner(i) == pDoc->GetPlayersRace() || pDoc->m_Trade[pDoc->GetPlayersRace()].GetMonopolBuying()[i] != 0.0f)
				m_dMonopolCosts[i] = 0.0f;
			// Wenn eine andere Hauptrasse das Monopol besitzt
			else if (CTrade::GetMonopolOwner(i) != NOBODY)
				m_dMonopolCosts[i] *= 2;
			m_dMonopolCosts[i] *= 15 * (i+1);	// Monopolkosten festsetzen

			///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
			// Hier die Boni durch die Uniqueforschung "Handel" -> keine Handelsgebühr
			if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetFieldStatus(2) == RESEARCHED)
				m_dMonopolCosts[i] -= m_dMonopolCosts[i] * pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetBonus(2) / 100;
			
			s.Format("%.0lf %s",m_dMonopolCosts[i], CResourceManager::GetString("LATINUM"));
			pDC->DrawText(s,CRect(r.left+500,r.top+120+i*110,r.left+850,r.top+150+i*110),DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		}
		for (int i = TITAN; i <= IRIDIUM; i++)
		{
			// kleine Kauf-Buttons einblenden, wenn wir ein Monopol kaufen können
			LoadFontForLittleButton(pDC);
			mdc.SelectObject(bm);
			if (m_dMonopolCosts[i] != 0.0f)
			{
				pDC->BitBlt(r.left+915,r.top+120+i*110,120,30,&mdc,0,0,SRCCOPY);
				pDC->DrawText(CResourceManager::GetString("BTN_BUY") ,CRect(r.left+915,r.top+120+i*110,r.left+1035,r.top+150+i*110), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}
	}
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	
	// Wir kennen nicht genügend andere Imperien, somit können wir kein Monopol kaufen
	if (m_bCouldBuyMonopols == FALSE)
	{
		// emptyScreen zeichnen
		mdc.SelectObject(bg_empty1);
		pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
		pDC->DrawText(CResourceManager::GetString("NO_MONOPOLY_BUY") ,CRect(r.left+200,r.top+250,r.right-200,r.bottom),DT_CENTER | DT_WORDBREAK);
	}
	// "Handelsmonopole" in der Mitte zeichnen
	pDC->DrawText(CResourceManager::GetString("MONOPOLY_MENUE"), CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	// Buttons am unteren Bildrand zeichnen
	LoadFontForBigButton(pDC);
	mdc.SelectObject(bm);
	//pDC->BitBlt(10,690,170,730,&mdc,0,0,SRCCOPY);
	pDC->BitBlt(288,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_STOCK_EXCHANGE"), CRect(288,690,448,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDC->BitBlt(458,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_MONOPOLY"), CRect(458,690,618,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDC->BitBlt(628,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_TRANSFERS"), CRect(628,690,788,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Transfermenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CTradeMenuView::DrawTradeTransferMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r = theClientRect;
	CString s;

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_tradetransfermenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
	
	// grobe Linien zeichnen
	COLORREF oldColor = pDC->GetTextColor();
/*	CPen mark(PS_SOLID, 1, RGB(255,125,0));
	pDC->SelectObject(&mark);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-50);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-50);
*/
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
	for (int i = 0; i < pDoc->m_Trade[pDoc->GetPlayersRace()].GetTradeActions()->GetSize(); i++)
		if (pDoc->m_Trade[pDoc->GetPlayersRace()].GetTradeActions()->GetAt(i).system == pDoc->GetKO())
		{
			USHORT res = pDoc->m_Trade[pDoc->GetPlayersRace()].GetTradeActions()->GetAt(i).res;
			// Ressourcen die gekauft werden
			if (pDoc->m_Trade[pDoc->GetPlayersRace()].GetTradeActions()->GetAt(i).price > 0)
			{
				boughtResNumber[res] += pDoc->m_Trade[pDoc->GetPlayersRace()].GetTradeActions()->GetAt(i).number;
				boughtResPrice[res]  += pDoc->m_Trade[pDoc->GetPlayersRace()].GetTradeActions()->GetAt(i).price;
			}
			// Ressourcen die verkauft werden
			else
			{
				selledResNumber[res] += pDoc->m_Trade[pDoc->GetPlayersRace()].GetTradeActions()->GetAt(i).number;
				selledResPrice[res]  += pDoc->m_Trade[pDoc->GetPlayersRace()].GetTradeActions()->GetAt(i).price;
			}
		}
	// berechnete und zusammengefasste Transfers hinschreiben
	pDC->SetTextColor(RGB(200,200,200));
	pDC->DrawText(CResourceManager::GetString("TITAN"),CRect(r.left,r.top+250,r.right,r.top+275),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DEUTERIUM"),CRect(r.left,r.top+320,r.right,r.top+345),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DURANIUM"),CRect(r.left,r.top+390,r.right,r.top+415),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("CRYSTAL"),CRect(r.left,r.top+460,r.right,r.top+485),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("IRIDIUM"),CRect(r.left,r.top+530,r.right,r.top+555),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	pDC->SetTextColor(oldColor);
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		// Alle gekauften
		if (boughtResPrice[i] > 0)
		{
			s.Format("%d %s %d %s",boughtResNumber[i], CResourceManager::GetString("UNITS_FOR"),
				(int)ceil((boughtResPrice[i]) * pDoc->m_Trade[pDoc->GetPlayersRace()].GetTax()), CResourceManager::GetString("LATINUM"));
			pDC->DrawText(s,CRect(r.left+210,r.top+250+i*70,r.right,r.top+275+i*70),DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
		// Alle verkauften
		if (selledResPrice[i] < 0)
		{
			s.Format("%d %s %d %s",selledResNumber[i], CResourceManager::GetString("UNITS_FOR"), 
				-selledResPrice[i], CResourceManager::GetString("LATINUM"));
			pDC->DrawText(s,CRect(r.left,r.top+250+i*70,r.right-210,r.top+275+i*70),DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		}
	}
	// "anstehender Ressourcentransfer" in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("TRADE_TRANSFER_MENUE"),CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	
	// System, in dem die Handelsaktivitäten stattfinden werden
	LoadFontForBigButton(pDC);
	oldColor = pDC->GetTextColor();
	pDC->SetTextColor(RGB(200,200,200));
	s = CResourceManager::GetString("TRANSFERS_NEXT_ROUND",FALSE,pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetName());
	pDC->DrawText(s,CRect(r.left,r.top+90,r.right,r.top+135), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// Buttons am unteren Bildrand zeichnen
	pDC->SetTextColor(oldColor);
	mdc.SelectObject(bm);
	//pDC->BitBlt(10,690,170,730,&mdc,0,0,SRCCOPY);
	pDC->BitBlt(288,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_STOCK_EXCHANGE"), CRect(288,690,448,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDC->BitBlt(458,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_MONOPOLY"), CRect(458,690,618,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDC->BitBlt(628,690,160,40,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_TRANSFERS"), CRect(628,690,788,730),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CTradeMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);
	// Wenn wir in der globalen Handelsansicht sind
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	// Checken ob ich auf irgendeinen Button geklickt habe um in ein anderes Untermenü des Handelsmenüs komme
	if (CRect(288,690,448,730).PtInRect(point))
	{
		m_bySubMenu = 0;
		Invalidate();
	}
	else if (CRect(458,690,618,730).PtInRect(point))
	{
		m_bySubMenu = 1;
		Invalidate();
	}
	else if (CRect(628,690,788,730).PtInRect(point))
	{
		m_bySubMenu = 2;
		Invalidate();
	}
	if (m_bySubMenu == 0)
	{
		// Checken ob wir auf den Button geklickt haben um die Kaufmenge zu ändern
		if (CRect(r.left+478,r.top+580,r.left+598,r.top+610).PtInRect(point))
		{
			switch(pDoc->m_Trade[pDoc->GetPlayersRace()].GetQuantity())
			{
			case 100: pDoc->m_Trade[pDoc->GetPlayersRace()].SetQuantity(1000); break;
			case 1000: pDoc->m_Trade[pDoc->GetPlayersRace()].SetQuantity(10000); break;
			case 10000: pDoc->m_Trade[pDoc->GetPlayersRace()].SetQuantity(100); break;
			}
			Invalidate(FALSE);
			return;
		}	
		// Wenn das System blockiert wird, so kann man an der globalen Handelsbörse keine Käufe und Verkäufe tätigen
		if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBlockade() > NULL)
			return;
		// Checken ob wir auf irgendeinen Kaufen- oder Verkaufenbutton geklickt haben
		for (int i = TITAN; i <= IRIDIUM; i++)
		{
			// Überprüfen, ob wir auf einen Kaufenbutton geklickt haben
			if (CRect(r.left+75+i*200,r.top+275,r.left+195+i*200,r.top+305).PtInRect(point))
			{
				int costs = pDoc->m_Trade[pDoc->GetPlayersRace()].BuyRessource(i,pDoc->m_Trade[pDoc->GetPlayersRace()].GetQuantity(),
					pDoc->GetKO(),pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum());
				// Wenn wir soviel Latinum haben um etwas zu kaufen -> also costs != NULL)
				if (costs != 0)
				{
					pDoc->m_Empire[pDoc->GetPlayersRace()].SetLatinum(-costs);
					pDoc->m_pSoundManager->PlaySound(SNDMGR_SOUND_SHIPTARGET);
					Invalidate(FALSE);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				return;
			}
			// Überprüfen ob wir auf einen Verkaufenbutton geklickt haben
			else if (CRect(r.left+75+i*200,r.top+310,r.left+195+i*200,r.top+340).PtInRect(point))
			{
				// Überprüfen, das wir bei einem Verkauf nicht mehr Ressourcen aus dem System nehmen als im Lager vorhanden sind
				if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetRessourceStore(i) >= pDoc->m_Trade[pDoc->GetPlayersRace()].GetQuantity())
				{
					pDoc->m_Trade[pDoc->GetPlayersRace()].SellRessource(i,pDoc->m_Trade[pDoc->GetPlayersRace()].GetQuantity(),pDoc->GetKO());
					// Ressource aus dem Lager nehmen
					pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].SetRessourceStore(i,-pDoc->m_Trade[pDoc->GetPlayersRace()].GetQuantity());
					pDoc->m_pSoundManager->PlaySound(SNDMGR_SOUND_SHIPTARGET);
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
				if (pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum() >= m_dMonopolCosts[i])
				{
					pDoc->m_Trade[pDoc->GetPlayersRace()].SetMonopolBuying(i,m_dMonopolCosts[i]);
					pDoc->m_Empire[pDoc->GetPlayersRace()].SetLatinum((long)-m_dMonopolCosts[i]);
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

void CTradeMenuView::CreateButtons()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// alle Buttons in der View anlegen und Grafiken laden
	switch(pDoc->GetPlayersRace())
	{
	case HUMAN:
		{
			break;
		}
	case FERENGI:
		{
			break;
		}
	case KLINGON:
		{
			break;
		}
	case ROMULAN:
		{
			break;
		}
	case CARDASSIAN:
		{
			break;
		}
	case DOMINION:
		{
			break;
		}
	}
}