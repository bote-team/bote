// DiplomacyMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "DiplomacyMenuView.h"
#include "DiplomacyBottomView.h"
#include "MenuChooseView.h"

// CDiplomacyMenuView

IMPLEMENT_DYNCREATE(CDiplomacyMenuView, CMainBaseView)

CDiplomacyMenuView::CDiplomacyMenuView()
{

}

CDiplomacyMenuView::~CDiplomacyMenuView()
{
	for (int i = 0; i < m_DiplomacyMainButtons.GetSize(); i++)
	{
		delete m_DiplomacyMainButtons[i];
		m_DiplomacyMainButtons[i] = 0;
	}
	m_DiplomacyMainButtons.RemoveAll();
	for (int i = 0; i < m_DiplomacyMajorOfferButtons.GetSize(); i++)
	{
		delete m_DiplomacyMajorOfferButtons[i];
		m_DiplomacyMajorOfferButtons[i] = 0;
	}
	m_DiplomacyMajorOfferButtons.RemoveAll();
	for (int i = 0; i < m_DiplomacyMinorOfferButtons.GetSize(); i++)
	{
		delete m_DiplomacyMinorOfferButtons[i];
		m_DiplomacyMinorOfferButtons[i] = 0;
	}
	m_DiplomacyMinorOfferButtons.RemoveAll();
}

BEGIN_MESSAGE_MAP(CDiplomacyMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CDiplomacyMenuView::OnNewRound()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
    
	m_iLatinumPayment = 0;
	m_iWhichOfferButtonIsPressed = -1;
	m_iClickedOnRace = -1;
	m_iClickedOnMajorRace = -1;
	m_iWhichRaceCorruption = NOBODY;
	m_iDurationOfAgreement = 0;		

	BYTE race = pDoc->GetPlayersRace();
	if (pDoc->GetSystem(pDoc->GetRaceKO(race).x,pDoc->GetRaceKO(race).y).GetOwnerOfSystem() == race)
		m_RessourceFromSystem = pDoc->GetRaceKO(race);
	// Systeme nochmal durchgehen um ein System zu finden, aus dem wir Rohstoffe verschenken könnten
	else if (pDoc->GetEmpire(race)->GetSystemList()->GetSize() > 0)
		m_RessourceFromSystem = pDoc->GetEmpire(race)->GetSystemList()->GetAt(0).ko;    
}

// CDiplomacyMenuView drawing

void CDiplomacyMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here
	CMemDC pDC(dc);
	pDC->SetBkMode(TRANSPARENT);
	if (pDoc->m_pIniLoader->GetValue("SMOOTHSCALING"))
		pDC->SetStretchBltMode(HALFTONE);
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);
	
	LoadRaceFont(pDC);
	// ***************************** DIE DIPLOMATIESANSICHT ZEICHNEN **********************************
	DrawDiplomacyMenue(pDC,r);
	// ************** DIE DIPLOMATIEANSICHT ZEICHNEN ist hier zu Ende **************
}


// CDiplomacyMenuView diagnostics

#ifdef _DEBUG
void CDiplomacyMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CDiplomacyMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDiplomacyMenuView message handlers

void CDiplomacyMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	// Alle Buttons in der View erstellen
	CreateButtons();
	
	// alle Hintergrundgrafiken laden
	bg_diploinfomenu.DeleteObject();
	bg_diploinmenu.DeleteObject();
	bg_diplooutmenu.DeleteObject();
	
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
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"diploinfomenu.jpg");
	bg_diploinfomenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"diplooutmenu.jpg");
	bg_diplooutmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"diploinmenu.jpg");
	bg_diploinmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Destroy();	
	
	// Diplomatieansicht
	m_bySubMenu = 0;
	m_iClickedOnRace = -1;
	m_iClickedOnMajorRace = -1;
	m_bMajorOrMinor = MINOR;
	m_iWhichOfferButtonIsPressed = -1;
	m_iLatinumPayment = 0;
	m_strDiplomaticText = "";
	for (int i = 0; i < 5; i++)
		m_iRessourcePayment[i] = 0;
	m_iWhichRessourceIsChosen = 0;
	m_RessourceFromSystem = pDoc->GetRaceKO(pDoc->GetPlayersRace());
	help = 0;
	help2 = 0;
	m_iWhichRaceCorruption = NOBODY;
	m_iDurationOfAgreement = 0;
}

void CDiplomacyMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CDiplomacyMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Diplomatiemenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CDiplomacyMenuView::DrawDiplomacyMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	COLORREF oldColor = pDC->GetTextColor();
	
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	if (m_bySubMenu == 0)
	{
		mdc.SelectObject(bg_diploinfomenu);
		pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);
	}
	else if (m_bySubMenu == 1)
	{
		mdc.SelectObject(bg_diplooutmenu);
		pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
	}
	else
	{
		mdc.SelectObject(bg_diploinmenu);
		pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
	}

	CPen mark(PS_SOLID, 1, RGB(255,125,0));
	pDC->SelectObject(&mark);

	USHORT race = pDoc->GetPlayersRace();
	m_bShowSendButton = TRUE;
	m_bShowDeclineButton = TRUE;
	CRect rect;
	CString s;
	CDiplomacyBottomView::SetText("");
	this->DrawMajorDiplomacyMenue(pDC, theClientRect);
	this->DrawMinorDiplomacyMenue(pDC, theClientRect);

	// Buttons am unteren Rand zeichnen und die Buttons in der jeweiligeb Diplomatieansicht zeichnen
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_DiplomacyMainButtons, m_bySubMenu);
	// Wenn wir uns in der Angebotsanicht des Diplomatiemenues befinden
	if (m_bySubMenu == 1 && ((m_iClickedOnRace != -1 && m_bMajorOrMinor == MINOR) || (m_iClickedOnMajorRace != -1 && m_bMajorOrMinor == MAJOR)))
	{
		if (m_bMajorOrMinor == MAJOR)
		{
			for (int i = 0; i < m_DiplomacyMajorOfferButtons.GetSize(); i++)
			{
				CMyButton *button = m_DiplomacyMajorOfferButtons.GetAt(i);
				// Handelsvertrag Button
				if (button->GetText() == CResourceManager::GetString("BTN_TRADECONTRACT"))
				{
					if (pDoc->m_MajorRace[race].GetDiplomacyStatus(m_iClickedOnMajorRace) < TRADE_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_FRIENDSHIP"))
				{
					if (pDoc->m_MajorRace[race].GetDiplomacyStatus(m_iClickedOnMajorRace) < FRIENDSHIP_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_COOPERATION"))
				{
					if (pDoc->m_MajorRace[race].GetDiplomacyStatus(m_iClickedOnMajorRace) < COOPERATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_AFFILIATION"))
				{
					if (pDoc->m_MajorRace[race].GetDiplomacyStatus(m_iClickedOnMajorRace) < AFFILIATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_NAP"))
				{
					if (pDoc->m_MajorRace[race].GetDiplomacyStatus(m_iClickedOnMajorRace) < FRIENDSHIP_AGREEMENT && pDoc->GetMajorRace(race)->GetDiplomacyStatus(m_iClickedOnMajorRace) != NON_AGGRESSION_PACT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_DEFENCE"))
				{
					if (pDoc->m_MajorRace[race].GetDefencePact(m_iClickedOnMajorRace) != TRUE && pDoc->GetMajorRace(race)->GetDiplomacyStatus(m_iClickedOnMajorRace) != WAR && pDoc->GetMajorRace(race)->GetDiplomacyStatus(m_iClickedOnMajorRace) != AFFILIATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_REQUEST"))
				{
					if (button->GetState() != 1)
						button->SetState(0);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_WARPACT"))
				{
					button->SetState(2);
					// sobald wir noch eine andere Majorracekennen, mit der wir nicht im Krieg sind, können wir
					// einen Kriegspakt anbieten
					if (pDoc->GetMajorRace(race)->GetDiplomacyStatus(m_iClickedOnMajorRace) != WAR)
					{
						for (int t = HUMAN; t <= DOMINION; t++)
							if (t != race && t != m_iClickedOnMajorRace && pDoc->GetMajorRace(m_iClickedOnMajorRace)->GetDiplomacyStatus(t) != WAR && pDoc->GetMajorRace(race)->GetKnownMajorRace(t) && pDoc->GetMajorRace(m_iClickedOnMajorRace)->GetKnownMajorRace(t))
							{							
								if (button->GetState() != 1)
									button->SetState(0);
								break;
							}
					}
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_WAR"))
				{
					if (pDoc->m_MajorRace[race].GetDiplomacyStatus(m_iClickedOnMajorRace) != WAR)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_PRESENT"))
				{
					if (button->GetState() != 1)
						button->SetState(0);
				}
			}
			DrawButtons(pDC, &m_DiplomacyMajorOfferButtons, -1);
		}
		else if (m_bMajorOrMinor == MINOR)
		{
			// Checken ob wir ein Angebot überhaupt machen können, z.B. wenn eine andere Hauptrasse
			// z.B. Mitgliedschaft mit einer Minorrace hat, dann können wir ihr kein Angebot machen, außer
			// Geschenke geben und Bestechen
			short status = NO_AGREEMENT;
			for (int i = HUMAN; i <= DOMINION; i++)
				if (i != pDoc->GetPlayersRace())
				{
					short temp = pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(i);
					if (temp > status)
						status = temp;
				}
			for (int i = 0; i < m_DiplomacyMinorOfferButtons.GetSize(); i++)
			{
				CMyButton *button = m_DiplomacyMinorOfferButtons.GetAt(i);
				if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetSubjugated() == TRUE)
				{
					button->SetState(2);
					continue;
				}
				// Handelsvertrag Button
				if (button->GetText() == CResourceManager::GetString("BTN_TRADECONTRACT"))
				{
					if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) < TRADE_AGREEMENT && status <= AFFILIATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_FRIENDSHIP"))
				{
					if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) < FRIENDSHIP_AGREEMENT && status <= COOPERATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_COOPERATION"))
				{
					if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) < COOPERATION && status <= FRIENDSHIP_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_AFFILIATION"))
				{
					if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) < AFFILIATION && status <= FRIENDSHIP_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_MEMBERSHIP"))
				{
					if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) < MEMBERSHIP && status <= FRIENDSHIP_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_WAR"))
				{
					if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) != WAR && status <= AFFILIATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_CORRUPTION"))
				{
					if (status >= AFFILIATION || (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) >= FRIENDSHIP_AGREEMENT && status >= TRADE_AGREEMENT))
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}				
				else if (button->GetText() == CResourceManager::GetString("BTN_PRESENT"))
				{
					if (button->GetState() != 1)
						button->SetState(0);
				}
			}
			DrawButtons(pDC, &m_DiplomacyMinorOfferButtons, -1);
		}
		mdc.SelectObject(bm);

		// Wenn wir Geld geben wollen, den Abschickenbutton einblenden
		if (m_iWhichOfferButtonIsPressed == 6 && m_bShowSendButton == TRUE)
		{
			if (m_iLatinumPayment > 0 || m_iRessourcePayment[0] > 0 || m_iRessourcePayment[1] > 0 || m_iRessourcePayment[2] > 0 ||
				m_iRessourcePayment[3] > 0 || m_iRessourcePayment[4] > 0)
			{
				pDC->BitBlt(871,690,160,40,&mdc,0,0,SRCCOPY);
				rect.SetRect(871,690,1031,730);
				pDC->DrawText(CResourceManager::GetString("BTN_SEND"),rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			}
		/*	// Text ändern Button anzeigen
			pDC->BitBlt(715,630,160,40,&mdc,0,0,SRCCOPY);
			s.Format("Text aendern");
			rect.SetRect(715,630,875,670);
			pDC->DrawText(s,rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);*/
		}
		// Wenn wir Freundschaft, Handelsvertrag usw. anbieten wollen, den Abschickenbutton anzeigen
		// Wenn wir eine kleine Rasse bestechen wollen bzw. eine Forderung an die große Rasse stellen Abschickenbutton
		// einblenden. Bei Forderung muß ein Wert ausgewählt worden sein
		else if ((m_iWhichOfferButtonIsPressed == 0 || m_iWhichOfferButtonIsPressed == 1 || m_iWhichOfferButtonIsPressed == 2 || m_iWhichOfferButtonIsPressed == 3 || m_iWhichOfferButtonIsPressed == 4 || m_iWhichOfferButtonIsPressed == 5 || m_iWhichOfferButtonIsPressed == 8 || (m_iWhichOfferButtonIsPressed == 9 && m_iWhichRaceCorruption != NOBODY)) 
			|| ((m_iWhichOfferButtonIsPressed == 7 && m_bMajorOrMinor == MINOR) || (m_bMajorOrMinor == MAJOR
			&& (m_iLatinumPayment > 0 || m_iRessourcePayment[0] > 0 || m_iRessourcePayment[1] > 0 || m_iRessourcePayment[2] > 0 ||
			m_iRessourcePayment[3] > 0 || m_iRessourcePayment[4] > 0))))
		{
			pDC->BitBlt(871,690,160,40,&mdc,0,0,SRCCOPY);
			rect.SetRect(871,690,1031,730);
			pDC->DrawText(CResourceManager::GetString("BTN_SEND"),rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		/*	// Text ändern Button anzeigen
			pDC->BitBlt(715,630,160,40,&mdc,0,0,SRCCOPY);
			s.Format("Text aendern");
			rect.SetRect(715,630,875,670);
			pDC->DrawText(s,rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);*/
		}
		// Wenn wir ein gegebenes Angebot wieder abbrechen wollen
		if (m_bShowSendButton == FALSE)
		{
			pDC->BitBlt(871,690,160,40,&mdc,0,0,SRCCOPY);
			rect.SetRect(871,690,1031,730);
			pDC->DrawText(CResourceManager::GetString("BTN_CANCEL"),rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
	// Buttons in der Eingangsansicht zeichnen
	else if (m_bySubMenu == 2 && ((m_iClickedOnRace != -1 && m_bMajorOrMinor == MINOR) || (m_iClickedOnMajorRace != -1 && m_bMajorOrMinor == MAJOR)))
	{
		if (m_bShowSendButton == TRUE)
			mdc.SelectObject(bm);
		else
			mdc.SelectObject(bm_dark);
		pDC->BitBlt(852,480,160,40,&mdc,0,0,SRCCOPY);
		rect.SetRect(852,480,1012,520);
		pDC->DrawText(CResourceManager::GetString("BTN_ACCEPT"),rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		if (m_bShowDeclineButton == TRUE)
			mdc.SelectObject(bm);
		else
			mdc.SelectObject(bm_dark);
		pDC->BitBlt(852,599,160,40,&mdc,0,0,SRCCOPY);
		rect.SetRect(852,599,1012,639);
		pDC->DrawText(CResourceManager::GetString("BTN_DECLINE"),rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	// "Diplomatieuebersicht" in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	rect.SetRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60);
	if (m_bySubMenu == 0)
		s = CResourceManager::GetString("DIPLOMACY_MENUE_INFO");
	else if (m_bySubMenu == 1)
		s = CResourceManager::GetString("DIPLOMACY_MENUE_OFFERS");
	else if (m_bySubMenu == 2)
		s = CResourceManager::GetString("DIPLOMACY_MENUE_RECEIPTS");
	pDC->DrawText(s,rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CDiplomacyBottomView));
}

void CDiplomacyMenuView::DrawMajorDiplomacyMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CBrush nb(RGB(0,0,0));
	COLORREF oldColor = pDC->GetTextColor();
	help2 = 0;	// zählt mit, wieviele Nachrichten von Majorraces ich im Eingangsmenü habe, also j, wir brauchen es aber in der ganzen Klasse
	CRect rect;
	CString name,s;
	USHORT j = 0;
	USHORT race = pDoc->GetPlayersRace();
	// erstmal die bekannten Majorrassen anzeigen
	for (int k = HUMAN; k <= DOMINION; k++)
	{
		// Wenn wir die Rasse kennen und nicht im Eingangsbildschirm sind
		if (pDoc->m_MajorRace[race].GetKnownMajorRace(k) == TRUE && k != race && m_bySubMenu != 2)
		{
			name.Format("%s",CMajorRace::GetRaceName(k));
			// Haben wir auf die Hauptrasse geklickt, dann Farbe ändern und jeweiliges Untermenü aufrufen
			if (m_iClickedOnMajorRace == k && m_bMajorOrMinor == MAJOR && m_iClickedOnMajorRace != -1)
			{
				// Wenn wir im Informationsbildschirm sind
				if (m_bySubMenu == 0)
					this->DrawDiplomacyInfoMenue(pDC, theClientRect, k);
				// Wenn wir im Angebotsbildschirm sind
				else if (m_bySubMenu == 1)
					this->DrawDiplomacyOfferMenue(pDC, theClientRect, k);
				// Bild der Hauptrasse zeichnen
				CDC mdc;
				mdc.CreateCompatibleDC(pDC);
				BOOLEAN succeed = FALSE;
				CString name = pDoc->GetMajorRace(k)->GetGraphicFileName();
				if (!name.IsEmpty())
				{
					s.Format(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Races/%s.jpg",name);
					FCObjImage img;
					succeed = img.Load(s);
					if (!succeed)
						succeed = img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Races/ImageMissing.jpg");
					bm.DeleteObject();
					bm.Attach(FCWin32::CreateDDBHandle(img));
					img.Destroy();
				}
				if (succeed)
				{
					mdc.SelectObject(bm);
					pDC->BitBlt(theClientRect.right-340,100,300,300,&mdc,0,0,SRCCOPY);
					bm.DeleteObject();
				}
				// Balken zeichnen, der angibt, wie gut die Majorrace uns gegenübersteht, aber nur wenn der Computer sie spielt
				// das ist hier anders als bei den Minorraces. Ich sehe wie gut die Majorrace mich findet, nicht wie gut ich
				// sie finde. Bei den Minorraces ist das einfacher, weil dort der Wert gleich ist, egal von welcher Seite man
				// das sieht
				if (pDoc->m_Empire[k].GetPlayerOfEmpire() == COMPUTER)
				{
					pDC->SetTextColor(RGB(255,0,0));
					CRect timber[20];
					CPen mark2(PS_SOLID, 1, RGB(30,30,30));
					pDC->SelectObject(&mark2);
					for (int t = 0; t < 20; t++)
					{
						CBrush nb2(RGB(250-t*12,0+t*12,0));
						timber[t].SetRect(theClientRect.left+650,theClientRect.top+400-t*15,theClientRect.left+680,theClientRect.top+387-t*15);
						if ((pDoc->m_MajorRace[k].GetRelationshipToMajorRace(race))*2/10 > t)
						{
							pDC->SelectObject(&nb2);
							pDC->SetTextColor(RGB(250-t*12,0+t*12,0));
						}
						else
							pDC->SelectObject(&nb);
						pDC->Rectangle(timber[t]);
					}
					// den Text zeichnen, der Angibt wie gut uns die Rasse gegenübersteht
					USHORT relation = pDoc->m_MajorRace[k].GetRelationshipToMajorRace(race);
					if (relation < 5) s = CResourceManager::GetString("HATEFUL");
					else if (relation < 15) s = CResourceManager::GetString("FURIOUS");
					else if (relation < 25) s = CResourceManager::GetString("HOSTILE");
					else if (relation < 35) s = CResourceManager::GetString("ANGRY");
					else if (relation < 45) s = CResourceManager::GetString("NOT_COOPERATIVE");
					else if (relation < 55) s = CResourceManager::GetString("NEUTRAL");
					else if (relation < 65) s = CResourceManager::GetString("COOPERATIVE");
					else if (relation < 75) s = CResourceManager::GetString("FRIENDLY");
					else if (relation < 85) s = CResourceManager::GetString("OPTIMISTIC");
					else if (relation < 95) s = CResourceManager::GetString("ENTHUSED");
					else s = CResourceManager::GetString("DEVOTED");
					rect.SetRect(theClientRect.right-340,403,theClientRect.right-40,428);
					pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
				CPen mark;
				if (pDoc->GetPlayersRace() == HUMAN)
				{
					pDC->SetTextColor(RGB(220,220,220));
					mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
				}
				else if (pDoc->GetPlayersRace() == FERENGI)
				{
					pDC->SetTextColor(RGB(220,220,220));
					mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
				}
				else if (pDoc->GetPlayersRace() == KLINGON)
				{
					pDC->SetTextColor(RGB(220,220,220));
					mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
				}
				else if (race == CARDASSIAN)
				{
					pDC->SetTextColor(RGB(255,128,0));
					mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
				}
				else
				{
					pDC->SetTextColor(RGB(220,220,220));
					mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
				}
				pDC->SelectObject(&mark);
				pDC->MoveTo(8,100+j*25); pDC->LineTo(150,100+j*25);
				pDC->MoveTo(8,125+j*25); pDC->LineTo(150,125+j*25);
			}
			rect.SetRect(20,100+j*25,theClientRect.left+150,125+j*25);
			pDC->DrawText(name,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			j++;
			pDC->SetTextColor(oldColor);
		}
		// Wenn wir die Majorrasse kennen und im Eingangsbildschirm sind
		else if (m_bySubMenu == 2)
		{
			// Wenn die Rasse ein Angebot an uns gemacht hat
			for (int l = 0; l < pDoc->m_Diplomacy[race].GetMadeOffers()->GetSize(); l++)
				if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace != FALSE
					&& pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).majorRace == k
					&& pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).majorRace != race
					&& pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).round+1 == pDoc->GetCurrentRound())
				{
					// haben wir ne Hauptrasse im Eingangsbildschirm angeklickt
					if (k == m_iClickedOnMajorRace && help == j)
					{
						// Bild der Hauptrasse zeichnen
						CDC mdc;
						mdc.CreateCompatibleDC(pDC);
						BOOLEAN succeed = FALSE;
						CString name = pDoc->GetMajorRace(k)->GetGraphicFileName();
						if (!name.IsEmpty())
						{
							s.Format(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Races/%s.jpg",name);
							FCObjImage img;
							succeed = img.Load(s);
							if (!succeed)
								succeed = img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Races/ImageMissing.jpg");
							bm.DeleteObject();
							bm.Attach(FCWin32::CreateDDBHandle(img));
							img.Destroy();
						}
						if (succeed)
						{
							mdc.SelectObject(bm);
							pDC->BitBlt(theClientRect.right-340,100,300,300,&mdc,0,0,SRCCOPY);
							bm.DeleteObject();
						}
						// Balken zeichnen, der angibt, wie gut die Majorrace uns gegenübersteht, aber nur wenn der Computer sie spielt
						if (pDoc->m_Empire[k].GetPlayerOfEmpire() == COMPUTER)
						{
							pDC->SetTextColor(RGB(255,0,0));
							CRect timber[20];
							CPen mark2(PS_SOLID, 1, RGB(30,30,30));
							pDC->SelectObject(&mark2);
							for (int t = 0; t < 20; t++)
							{
								CBrush nb2(RGB(250-t*12,0+t*12,0));
								timber[t].SetRect(theClientRect.left+650,theClientRect.top+400-t*15,theClientRect.left+680,theClientRect.top+387-t*15);
								if ((pDoc->m_MajorRace[k].GetRelationshipToMajorRace(race))*2/10 > t)
								{
									pDC->SelectObject(&nb2);
									pDC->SetTextColor(RGB(250-t*12,0+t*12,0));
								}
								else
									pDC->SelectObject(&nb);
								pDC->Rectangle(timber[t]);
							}
							// den Text zeichnen, der Angibt wie gut uns die Rasse gegenübersteht
							USHORT relation = pDoc->m_MajorRace[k].GetRelationshipToMajorRace(race);
							if (relation < 5) s = CResourceManager::GetString("HATEFUL");
							else if (relation < 15) s = CResourceManager::GetString("FURIOUS");
							else if (relation < 25) s = CResourceManager::GetString("HOSTILE");
							else if (relation < 35) s = CResourceManager::GetString("ANGRY");
							else if (relation < 45) s = CResourceManager::GetString("NOT_COOPERATIVE");
							else if (relation < 55) s = CResourceManager::GetString("NEUTRAL");
							else if (relation < 65) s = CResourceManager::GetString("COOPERATIVE");
							else if (relation < 75) s = CResourceManager::GetString("FRIENDLY");
							else if (relation < 85) s = CResourceManager::GetString("OPTIMISTIC");
							else if (relation < 95) s = CResourceManager::GetString("ENTHUSED");
							else s = CResourceManager::GetString("DEVOTED");
							rect.SetRect(theClientRect.right-340,403,theClientRect.right-40,428);
							pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}
						// Text vom Angebot anzeigen
						CDiplomacyBottomView::SetText(pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).shownText);
						if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == TRADE_AGREEMENT)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("TRADE_AGREEMENT_OFFER"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == FRIENDSHIP_AGREEMENT)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("FRIENDSHIP_OFFER"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == COOPERATION)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("COOPERATION_OFFER"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == AFFILIATION)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("AFFILIATION_OFFER"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == NON_AGGRESSION_PACT)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("NON_AGGRESSION_OFFER"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == DEFENCE_PACT)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("DEFENCE_PACT_OFFER"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == WAR_PACT)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("WAR_PACT_OFFER"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == WAR)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("WAR_OFFER"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == PRESENT)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("PRESENT"));
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == DIP_REQUEST)
							CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("REQUEST"));
						// Haben wir auf einen Button geklickt, so muß dieser gedrückt dargestellt werden
						if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace == TRUE)
							m_bShowSendButton = FALSE;
						else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace == DECLINED)
							m_bShowDeclineButton = FALSE;
						// Bei Kriegserklärung oder Geschenk haben wir keine Wahlmöglichkeit
						if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == WAR || pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == PRESENT)
						{
							m_bShowSendButton = FALSE;
							m_bShowDeclineButton = FALSE;
						}
						// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
						CPen mark;
						if (pDoc->GetPlayersRace() == HUMAN)
						{
							pDC->SetTextColor(RGB(220,220,220));
							mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
						}
						else if (pDoc->GetPlayersRace() == FERENGI)
						{
							pDC->SetTextColor(RGB(220,220,220));
							mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
						}
						else if (pDoc->GetPlayersRace() == KLINGON)
						{
							pDC->SetTextColor(RGB(220,220,220));
							mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
						}
						else if (race == CARDASSIAN)
						{
							pDC->SetTextColor(RGB(255,128,0));
							mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
						}
						else
						{
							pDC->SetTextColor(RGB(220,220,220));
							mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
						}
						pDC->SelectObject(&mark);
						pDC->MoveTo(8,100+j*25); pDC->LineTo(150,100+j*25);
						pDC->MoveTo(8,125+j*25); pDC->LineTo(150,125+j*25);
					}
					// Name der Rasse zeichnen
					name.Format("%s",CMajorRace::GetRaceName(k));
					rect.SetRect(20,100+j*25,theClientRect.left+150,125+j*25);
					pDC->DrawText(name,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					// Typ zeichnen (Vorschlag, Forderung, Geschenk)
					rect.SetRect(200,100+j*25,theClientRect.left+350,125+j*25);
					if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == PRESENT)
						s = CResourceManager::GetString("PRESENT");
					else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == DIP_REQUEST && k == m_iClickedOnMajorRace && help == j)
					{
						// Wenn eine Forderung gestellt wurde, welche Ressourcen beinhaltet, dann Annehmenbutton
						// nur einblenden, wenn wir diese Forderung auch erfüllen können
						USHORT *ressource = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).ressource;
						if (ressource[0] > 0 || ressource[1] > 0 || ressource[2] > 0 || ressource[3] > 0 || ressource[4] > 0)
						{
							// Wenn Forderung, dann Systemauswahlbutton einblenden, wovon ich die geforderte Ressource
							// abzweigen will
							COLORREF color = pDC->GetTextColor();
							CRect r;
							r.SetRect(theClientRect.left+350,100+j*25,theClientRect.left+500,125+j*25);
							s.Format("%s: ",CResourceManager::GetString("RESOURCE_FROM"));
							pDC->DrawText(s,r,DT_LEFT | DT_VCENTER | DT_SINGLELINE);
							this->LoadFontForLittleButton(pDC);
							CDC mdc;
							mdc.CreateCompatibleDC(pDC);
							mdc.SelectObject(bm2);
							pDC->BitBlt(476,97+j*25,120,30,&mdc,0,0,SRCCOPY);
							r.SetRect(theClientRect.left+476,100+j*25,theClientRect.left+596,125+j*25);
							CPoint p = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).KO;
							s.Format("%s",pDoc->GetSector(p.x,p.y).GetName());
							// Wenn hier noch kein System eingestellt ist, dann müssen wir uns eins suchen
							if (s.IsEmpty() || pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() != race)
								for (int y = 0; y < 20; y++)
									for (int x = 0; x < 30; x++)
										if (pDoc->GetSystem(x,y).GetOwnerOfSystem() == pDoc->GetPlayersRace() && pDoc->GetSector(x,y).GetSunSystem() == TRUE)
										{
											pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->ElementAt(l).KO.x = x;
											pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->ElementAt(l).KO.y = y;
											s.Format("%s",pDoc->GetSector(x,y).GetName());
											break;
										}
										// Überprüfen ob wir auf dem gewählten System die Menge der geforderten
										// Ressource im Lager haben und ob wir auch das geforderte Latinum bezahlen können
										p = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).KO;
										if (ressource[0] > 0 && pDoc->GetSystem(p.x,p.y).GetTitanStore() < ressource[0] && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race)
											m_bShowSendButton = FALSE;
										else if (ressource[1] > 0 && pDoc->GetSystem(p.x,p.y).GetDeuteriumStore() < ressource[1] && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race)
											m_bShowSendButton = FALSE;
										else if (ressource[2] > 0 && pDoc->GetSystem(p.x,p.y).GetDuraniumStore() < ressource[2] && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race)
											m_bShowSendButton = FALSE;
										else if (ressource[3] > 0 && pDoc->GetSystem(p.x,p.y).GetCrystalStore() < ressource[3] && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race)
											m_bShowSendButton = FALSE;
										else if (ressource[4] > 0 && pDoc->GetSystem(p.x,p.y).GetIridiumStore() < ressource[4] && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race && pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race)
											m_bShowSendButton = FALSE;
										pDC->DrawText(s,r,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
										pDC->SetTextColor(color);
						}
						// Überprüfen ob wir auch das geforderte Latinum bezahlen können
						CPoint p = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).KO;
						if (pDoc->GetEmpire(race)->GetLatinum() < pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).latinum)
							m_bShowSendButton = FALSE;								
						s = CResourceManager::GetString("REQUEST");
					}
					else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == DIP_REQUEST)
						s = CResourceManager::GetString("REQUEST");
					else
						s = CResourceManager::GetString("SUGGESTION");
					pDC->DrawText(s,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					pDC->SetTextColor(oldColor);
					j++;
				}

				// Wenn wir eine Antwort von einer Majorrace auf eins unserer Angebote bekommen haben
				for (int l = 0; l < pDoc->m_Empire[race].GetIncomingMajorAnswers()->GetSize(); l++)
					if (pDoc->m_Empire[race].GetIncomingMajorAnswers()->GetAt(l).AnswerFromMajorRace == k)
					{
						// haben wir ne Hauptrasse im Eingangsbildschirm angeklickt
						if (k == m_iClickedOnMajorRace && help == j)
						{
							// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
							CPen mark;
							if (pDoc->GetPlayersRace() == HUMAN)
							{
								pDC->SetTextColor(RGB(220,220,220));
								mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
							}
							else if (pDoc->GetPlayersRace() == FERENGI)
							{
								pDC->SetTextColor(RGB(220,220,220));
								mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
							}
							else if (pDoc->GetPlayersRace() == KLINGON)
							{
								pDC->SetTextColor(RGB(220,220,220));
								mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
							}
							else if (race == CARDASSIAN)
							{
								pDC->SetTextColor(RGB(255,128,0));
								mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
							}
							else
							{
								pDC->SetTextColor(RGB(220,220,220));
								mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
							}
							pDC->SelectObject(&mark);
							pDC->MoveTo(8,100+j*25); pDC->LineTo(150,100+j*25);
							pDC->MoveTo(8,125+j*25); pDC->LineTo(150,125+j*25);
							// Bild der Hauptrasse zeichnen
							CDC mdc;
							mdc.CreateCompatibleDC(pDC);
							BOOLEAN succeed = FALSE;
							CString name = pDoc->GetMajorRace(k)->GetGraphicFileName();
							if (!name.IsEmpty())
							{
								s.Format(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Races/%s.jpg",name);
								FCObjImage img;
								succeed = img.Load(s);
								if (!succeed)
									succeed = img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Races/ImageMissing.jpg");
								bm.DeleteObject();
								bm.Attach(FCWin32::CreateDDBHandle(img));
								img.Destroy();
							}
							if (succeed)
							{
								mdc.SelectObject(bm);
								pDC->BitBlt(theClientRect.right-340,100,300,300,&mdc,0,0,SRCCOPY);
								bm.DeleteObject();
							}
							// Balken zeichnen, der angibt, wie gut die Majorrace uns gegenübersteht, aber nur wenn der Computer sie spielt
							if (pDoc->m_Empire[k].GetPlayerOfEmpire() == COMPUTER)
							{
								pDC->SetTextColor(RGB(255,0,0));
								CRect timber[20];
								CPen mark2(PS_SOLID, 1, RGB(30,30,30));
								pDC->SelectObject(&mark2);
								for (int t = 0; t < 20; t++)
								{
									CBrush nb2(RGB(250-t*12,0+t*12,0));
									timber[t].SetRect(theClientRect.left+650,theClientRect.top+400-t*15,theClientRect.left+680,theClientRect.top+387-t*15);
									if ((pDoc->m_MajorRace[k].GetRelationshipToMajorRace(race))*2/10 > t)
									{
										pDC->SelectObject(&nb2);
										pDC->SetTextColor(RGB(250-t*12,0+t*12,0));
									}
									else
										pDC->SelectObject(&nb);
									pDC->Rectangle(timber[t]);
								}
								// den Text zeichnen, der Angibt wie gut uns die Rasse gegenübersteht
								USHORT relation = pDoc->m_MajorRace[k].GetRelationshipToMajorRace(race);
								if (relation < 5) s = CResourceManager::GetString("HATEFUL");
								else if (relation < 15) s = CResourceManager::GetString("FURIOUS");
								else if (relation < 25) s = CResourceManager::GetString("HOSTILE");
								else if (relation < 35) s = CResourceManager::GetString("ANGRY");
								else if (relation < 45) s = CResourceManager::GetString("NOT_COOPERATIVE");
								else if (relation < 55) s = CResourceManager::GetString("NEUTRAL");
								else if (relation < 65) s = CResourceManager::GetString("COOPERATIVE");
								else if (relation < 75) s = CResourceManager::GetString("FRIENDLY");
								else if (relation < 85) s = CResourceManager::GetString("OPTIMISTIC");
								else if (relation < 95) s = CResourceManager::GetString("ENTHUSED");
								else s = CResourceManager::GetString("DEVOTED");
								rect.SetRect(theClientRect.right-340,403,theClientRect.right-40,428);
								pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);						
							}
							pDC->SetTextColor(RGB(220,220,220));
							// Text vom Angebot anzeigen
							CDiplomacyBottomView::SetHeadLine(pDoc->m_Empire[race].GetIncomingMajorAnswers()->GetAt(l).headline);
							CDiplomacyBottomView::SetText(pDoc->m_Empire[race].GetIncomingMajorAnswers()->GetAt(l).message);
							m_bShowSendButton = FALSE;
							m_bShowDeclineButton = FALSE;
						}
						rect.SetRect(20,100+j*25,theClientRect.left+150,125+j*25);
						s = CMajorRace::GetRaceName(k);
						pDC->DrawText(s,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
						rect.SetRect(200,100+j*25,theClientRect.left+350,125+j*25);
						s = CResourceManager::GetString("ANSWER");
						pDC->DrawText(s,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
						pDC->SetTextColor(oldColor);
						j++;
					}
					help2 = j;	// hier die Anzahl der Nachrichten der Majorraces zwischenspeichern
		}
	}
}

void CDiplomacyMenuView::DrawMinorDiplomacyMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CBrush nb(RGB(0,0,0));
	CPen mark(PS_SOLID, 1, RGB(255,125,0));
	COLORREF oldColor = pDC->GetTextColor();
	CRect rect;
	CString s;
	USHORT race = pDoc->GetPlayersRace();
	USHORT numberOfKnownMajorRaces = pDoc->m_MajorRace[race].GetNumberOfKnownMajorRaces(race);
	USHORT j = help2;
	// Ab jetzt große Schleife, am Ende der Schleife werden auch die MinorRaces angezeigt
	if (m_bySubMenu != 2)
		j = numberOfKnownMajorRaces;
	
	for (int i = 0; i < pDoc->GetMinorRaceArray()->GetSize(); i++)
	{
		pDC->SetTextColor(oldColor);
		// Dem Imperium bekannte kleine Rassen anzeigen
		if (pDoc->GetMinorRaceArray()->GetAt(i).GetKnownByMajorRace(race) == TRUE)
		{
			// Wenn wir das erstemal hier reingehen und noch gar nix markiert ist
			if (m_iClickedOnRace == -1 && m_bySubMenu != 2)
				m_iClickedOnRace = i;
			// Wenn wir auf eine Rasse geklicked haben, dann diese markieren, und nur wenn es ne MinorRace ist
			if (i == m_iClickedOnRace && m_iClickedOnRace != -1 && m_bMajorOrMinor == MINOR)
			{
				// Wenn wir in dem Informationsbildschirm der Rassen sind
				if (m_bySubMenu == 0)
					this->DrawDiplomacyInfoMenue(pDC, theClientRect, i);

				// Wenn wir in dem Angebotsbildschirm der Rassen sind
				if (m_bySubMenu == 1)
					this->DrawDiplomacyOfferMenue(pDC, theClientRect, i);

				// Balken zeichnen, der angibt, wie gut die Minorrace uns gegenübersteht
				CRect timber[20];
				CPen mark2(PS_SOLID, 1, RGB(30,30,30));
				pDC->SelectObject(&mark2);
				pDC->SetTextColor(RGB(255,0,0));
				for (int t = 0; t < 20; t++)
				{
					CBrush nb2(RGB(250-t*12,0+t*12,0));
					timber[t].SetRect(theClientRect.left+650,theClientRect.top+400-t*15,theClientRect.left+680,theClientRect.top+387-t*15);
					if ((pDoc->GetMinorRaceArray()->GetAt(i).GetRelationshipToMajorRace(race))*2/10 > t)
					{
						pDC->SelectObject(&nb2);
						pDC->SetTextColor(RGB(250-t*12,0+t*12,0));
					}
					else
						pDC->SelectObject(&nb);
					pDC->Rectangle(timber[t]);
				}
				// den Text zeichnen, der Angibt wie gut uns die Rasse gegenübersteht
				USHORT relation = pDoc->GetMinorRaceArray()->GetAt(i).GetRelationshipToMajorRace(race);
				if (relation < 5) s = CResourceManager::GetString("HATEFUL");
				else if (relation < 15) s = CResourceManager::GetString("FURIOUS");
				else if (relation < 25) s = CResourceManager::GetString("HOSTILE");
				else if (relation < 35) s = CResourceManager::GetString("ANGRY");
				else if (relation < 45) s = CResourceManager::GetString("NOT_COOPERATIVE");
				else if (relation < 55) s = CResourceManager::GetString("NEUTRAL");
				else if (relation < 65) s = CResourceManager::GetString("COOPERATIVE");
				else if (relation < 75) s = CResourceManager::GetString("FRIENDLY");
				else if (relation < 85) s = CResourceManager::GetString("OPTIMISTIC");
				else if (relation < 95) s = CResourceManager::GetString("ENTHUSED");
				else s = CResourceManager::GetString("DEVOTED");
				rect.SetRect(theClientRect.right-340,403,theClientRect.right-40,428);
				pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				pDC->SetTextColor(oldColor);
				pDC->SelectObject(&mark);
				// Bild der Rasse zeichnen
				CBitmap bm;
				CDC mdc;
				mdc.CreateCompatibleDC(pDC);
				int trying = 0;				
				BOOLEAN succeed = FALSE;
				CString name = pDoc->GetMinorRaceArray()->GetAt(i).GetGraphicName();
				s.Format(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Races/%s",name);
				FCObjImage img;
				succeed = img.Load(s);
				if (!succeed)
					succeed = img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Races/ImageMissing.jpg");
				bm.DeleteObject();
				bm.Attach(FCWin32::CreateDDBHandle(img));
				img.Destroy();
				if (succeed)
				{
					mdc.SelectObject(bm);
					pDC->BitBlt(theClientRect.right-340,100,300,300,&mdc,0,0,SRCCOPY);
				}
			}

			// Wenn wir in dem Eingangsbildschirm sind, dann nur die Namen zeichnen, von denen wir auch ein Angebot
			// bekommen haben. Ansonsten alle Namen zeichnen, die wir kennen
			if (m_bySubMenu == 2)
			{
				// Wenn die Rasse ein Angebot an uns gemacht hat
				for (int l = 0; l < pDoc->m_Diplomacy[race].GetMadeOffers()->GetSize(); l++)
					if ((pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace == NOT_REACTED ||
						pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace == TRUE ||
						pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace == DECLINED) &&
						pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).majorRace == race &&
						pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).minorRace == i)
					{
						// Markierung zeichnen, wenn draufgeklickt wurde
						if (i == m_iClickedOnRace && help == j)
						{
							// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
							CPen mark;
							if (pDoc->GetPlayersRace() == HUMAN)
							{
								pDC->SetTextColor(RGB(220,220,220));
								mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
							}
							else if (pDoc->GetPlayersRace() == FERENGI)
							{
								pDC->SetTextColor(RGB(220,220,220));
								mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
							}
							else if (pDoc->GetPlayersRace() == KLINGON)
							{
								pDC->SetTextColor(RGB(220,220,220));
								mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
							}
							else if (race == CARDASSIAN)
							{
								pDC->SetTextColor(RGB(255,128,0));
								mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
							}
							else
							{
								pDC->SetTextColor(RGB(220,220,220));
								mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
							}
							pDC->SelectObject(&mark);
							pDC->MoveTo(8,100+j*25); pDC->LineTo(150,100+j*25);
							pDC->MoveTo(8,125+j*25); pDC->LineTo(150,125+j*25);
							// weiterhin, wenn wir dann auf diese Rasse geklickt haben
							// Text vom Angebot anzeigen
							CDiplomacyBottomView::SetText(pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).shownText);
							if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == TRADE_AGREEMENT)
								CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("TRADE_AGREEMENT_OFFER"));
							else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == FRIENDSHIP_AGREEMENT)
								CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("FRIENDSHIP_OFFER"));
							else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == COOPERATION)
								CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("COOPERATION_OFFER"));
							else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == AFFILIATION)
								CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("AFFILIATION_OFFER"));
							else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == MEMBERSHIP)
								CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("MEMBERSHIP_OFFER"));
							else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == WAR)
								CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("WAR_OFFER"));
							// Bei Kriegserklärung haben wir keine Wahlmöglichkeit und wenn wir auf einen Button
							// geklickt haben, so muß dieser gedrückt dargestellt werden
							if ((pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace == TRUE &&
								pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).minorRace == m_iClickedOnRace) ||
								pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == WAR)
								m_bShowSendButton = FALSE;
							if ((pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace == DECLINED &&
								pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).minorRace == m_iClickedOnRace) ||
								pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == WAR)
								m_bShowDeclineButton = FALSE;
						}
						rect.SetRect(20,100+j*25,theClientRect.left+150,125+j*25);
						s = pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName();
						pDC->DrawText(s,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
						rect.SetRect(200,100+j*25,theClientRect.left+350,125+j*25);
						s = CResourceManager::GetString("SUGGESTION");
						pDC->DrawText(s,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
						pDC->SetTextColor(oldColor);
						j++;
					}
					// Wenn die Rasse uns eine Antwort (Reaktion) auf eins unserer Angebote gegeben hat
					for (int l = 0; l < pDoc->m_Diplomacy[race].GetAnswers()->GetSize(); l++)
						if (pDoc->m_Diplomacy[race].GetAnswers()->GetAt(l).majorRace == race && pDoc->m_Diplomacy[race].GetAnswers()->GetAt(l).minorRace == i)
						{
							// Markierung zeichnen, wenn draufgeklickt wurde
							if (i == m_iClickedOnRace && help == j)
							{
								// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
								CPen mark;
								if (pDoc->GetPlayersRace() == HUMAN)
								{
									pDC->SetTextColor(RGB(220,220,220));
									mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
								}
								else if (pDoc->GetPlayersRace() == FERENGI)
								{
									pDC->SetTextColor(RGB(220,220,220));
									mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
								}
								else if (pDoc->GetPlayersRace() == KLINGON)
								{
									pDC->SetTextColor(RGB(220,220,220));
									mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
								}
								else if (race == CARDASSIAN)
								{
									pDC->SetTextColor(RGB(255,128,0));
									mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
								}
								else
								{
									pDC->SetTextColor(RGB(220,220,220));
									mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
								}
								pDC->SelectObject(&mark);
								pDC->MoveTo(8,100+j*25); pDC->LineTo(150,100+j*25);
								pDC->MoveTo(8,125+j*25); pDC->LineTo(150,125+j*25);
								// Text vom Angebot anzeigen
								CDiplomacyBottomView::SetHeadLine(pDoc->m_Diplomacy[race].GetAnswers()->GetAt(l).headline);
								CDiplomacyBottomView::SetText(pDoc->m_Diplomacy[race].GetAnswers()->GetAt(l).message);
								m_bShowSendButton = FALSE;
								m_bShowDeclineButton = FALSE;
							}
							rect.SetRect(20,100+j*25,theClientRect.left+150,125+j*25);
							s = pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName();
							pDC->DrawText(s,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
							rect.SetRect(200,100+j*25,theClientRect.left+350,125+j*25);
							s = CResourceManager::GetString("ANSWER");
							pDC->DrawText(s,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
							j++;
						}
			}
			// Namen aller bekannten Rassen anzeigen, also immer außer im Eingangsbildschirm
			else
			{
				// Markierung zeichnen, wenn draufgeklickt wurde
				if (i == m_iClickedOnRace && m_bMajorOrMinor == MINOR)
				{
					// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
					CPen mark;
					if (pDoc->GetPlayersRace() == HUMAN)
					{
						pDC->SetTextColor(RGB(220,220,220));
						mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
					}
					else if (pDoc->GetPlayersRace() == FERENGI)
					{
						pDC->SetTextColor(RGB(220,220,220));
						mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
					}
					else if (pDoc->GetPlayersRace() == KLINGON)
					{
						pDC->SetTextColor(RGB(220,220,220));
						mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
					}
					else if (race == CARDASSIAN)
					{
						pDC->SetTextColor(RGB(255,128,0));
						mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
					}
					else
					{
						pDC->SetTextColor(RGB(220,220,220));
						mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
					}
					pDC->SelectObject(&mark);
					pDC->MoveTo(8,100+j*25); pDC->LineTo(150,100+j*25);
					pDC->MoveTo(8,125+j*25); pDC->LineTo(150,125+j*25);
				}
				s = pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName();
				rect.SetRect(20,100+j*25,theClientRect.left+150,125+j*25);
				pDC->DrawText(s,rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				j++;
			}
		}
	}
}

void CDiplomacyMenuView::DrawDiplomacyInfoMenue(CDC* pDC, CRect theClientRect, USHORT whichRace)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	COLORREF oldColor = pDC->GetTextColor();
	CRect rect;
	CString s;
	USHORT i = whichRace;
	USHORT race = pDoc->GetPlayersRace();	
	// Angebote durchgehen, suchen ob wir schon eine Kündigung bei der Rasse gemacht haben
	for (int l = 0; l < pDoc->m_Diplomacy[race].GetMadeOffers()->GetSize(); l++)
		if (m_bMajorOrMinor == MINOR &&
			pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).minorRace == i &&
			pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).round == pDoc->GetCurrentRound() &&
			pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).majorRace == race &&
			pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == NO_AGREEMENT)
		{
			m_bShowSendButton = FALSE;
			CDiplomacyBottomView::SetHeadLine(CResourceManager::GetString("CANCEL_AGREEMENT"));
			s = CResourceManager::GetString("CANCEL_AGREEMENT_TEXT",FALSE,this->PrintDiplomacyStatus(race,i,pDC),pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
			CDiplomacyBottomView::SetText(s);
			break;
		}
		// Beschreibung der Rasse
		pDC->SetTextColor(RGB(220,220,220));
		rect.SetRect(theClientRect.left+190,theClientRect.top+470,theClientRect.right-40,theClientRect.bottom-90);
		if (m_bMajorOrMinor == MAJOR)
			s.Format("%s",pDoc->GetMajorRace(i)->GetRaceDescription());
		else
			s = pDoc->GetMinorRaceArray()->GetAt(i).GetRaceDescription();
		pDC->DrawText(s,rect, DT_WORDBREAK | DT_LEFT);
		// Ingame-Daten zu der Rasse, welche in der Box angezeigt werden
		pDC->SetTextColor(RGB(200,200,200));
		rect.SetRect(theClientRect.left+215,theClientRect.top+110,theClientRect.left+475,theClientRect.top+135);
		pDC->DrawText(CResourceManager::GetString("NAME").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
		rect += CSize(0,25);
		pDC->DrawText(CResourceManager::GetString("HOMESYSTEM").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
		rect += CSize(0,25);
		pDC->DrawText(CResourceManager::GetString("RELATIONSHIP").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
		rect += CSize(0,50);
		pDC->DrawText(CResourceManager::GetString("KNOWN_EMPIRES").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
		pDC->SetTextColor(oldColor);
		rect.SetRect(theClientRect.left+215,theClientRect.top+110,theClientRect.left+475,theClientRect.top+135);
		// Nur wenn wir auf eine Majorrace geklickt haben
		if (m_bMajorOrMinor == MAJOR)
		{
			pDC->DrawText(CMajorRace::GetRaceName(i), rect, DT_RIGHT | DT_TOP);
			rect += CSize(0,25);
			pDC->DrawText(pDoc->GetSector(pDoc->GetRaceKO(i).x,pDoc->GetRaceKO(i).y).GetName(), rect, DT_RIGHT | DT_TOP);
			rect += CSize(0,25);
			pDC->DrawText(this->PrintDiplomacyStatus(race,i,pDC), rect, DT_RIGHT | DT_TOP);
			rect += CSize(0,25);
			// Wenn wir einen Verteidigungpakt mit der Rasse haben
			if (pDoc->GetMajorRace(i)->GetDefencePact(race) == TRUE)
			{
				pDC->SetTextColor(RGB(226,44,250));
				if (pDoc->GetMajorRace(i)->GetDurationOfDefencePact(race) != 0)
					s.Format("%s (%d)",CResourceManager::GetString("DEFENCE_PACT"), pDoc->GetMajorRace(i)->GetDurationOfDefencePact(race));
				else
					s = CResourceManager::GetString("DEFENCE_PACT");
				pDC->DrawText(s, rect, DT_CENTER | DT_TOP);
			}
			pDC->SetTextColor(oldColor);
			rect += CSize(0,50);
			if (pDoc->m_MajorRace[race].GetDiplomacyStatus(i) >= FRIENDSHIP_AGREEMENT)
			{
				for (int j = HUMAN; j <= DOMINION; j++)
					if (j != i)
						if (pDoc->GetMajorRace(i)->GetKnownMajorRace(j))
						{
							pDC->SetTextColor(oldColor);
							pDC->DrawText(CMajorRace::GetRaceName(j), rect, DT_LEFT | DT_TOP);
							pDC->DrawText(this->PrintDiplomacyStatus(j,i,pDC), rect, DT_RIGHT | DT_TOP);
							rect += CSize(0,25);
						}
			}
			else
				pDC->DrawText(CResourceManager::GetString("NO_SPECS"), rect, DT_LEFT | DT_TOP);
		}
		// Nur wenn wir auf eine Minorrace geklickt haben
		else
		{
			pDC->DrawText(pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName(), rect, DT_RIGHT | DT_TOP);
			rect += CSize(0,25);
			pDC->DrawText(pDoc->GetMinorRaceArray()->GetAt(i).GetHomeplanetName(), rect, DT_RIGHT | DT_TOP);
			rect += CSize(0,25);
			pDC->DrawText(this->PrintDiplomacyStatus(race,i,pDC), rect, DT_RIGHT | DT_TOP);
			pDC->SetTextColor(RGB(200,200,200));
			rect += CSize(0,25);
			pDC->DrawText(CResourceManager::GetString("ACCEPTANCE").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
			pDC->SetTextColor(oldColor);
			s.Format("%d%%",(int)(pDoc->GetMinorRaceArray()->GetAt(i).GetRelationPoints(race) / 50));
			pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
			rect += CSize(0,50);
			if (pDoc->GetMinorRaceArray()->GetAt(i).GetDiplomacyStatus(race) >= FRIENDSHIP_AGREEMENT)
			{
				for (int j = HUMAN; j <= DOMINION; j++)
					if (j != race)
						if (pDoc->GetMinorRaceArray()->GetAt(i).GetKnownByMajorRace(j))
						{
							pDC->SetTextColor(oldColor);
							pDC->DrawText(CMajorRace::GetRaceName(j), rect, DT_LEFT | DT_TOP);
							pDC->DrawText(this->PrintDiplomacyStatus(j,i,pDC), rect, DT_RIGHT | DT_TOP);
							rect += CSize(0,25);
						}
			}
			else
				pDC->DrawText(CResourceManager::GetString("NO_SPECS"), rect, DT_LEFT | DT_TOP);
		}
		// Hier die ganzen Buttons in der Informationsansicht zeichnen
		// Wenn wir einen Vertrag haben, können wir den aufheben
		if (m_bMajorOrMinor == MINOR && pDoc->GetMinorRaceArray()->GetAt(i).GetDiplomacyStatus(race) > NO_AGREEMENT)
		{
			if (m_bShowSendButton == TRUE)
				s = CResourceManager::GetString("BTN_ANNUL");
			else
				s = CResourceManager::GetString("BTN_CANCEL");
			//this->LoadFontForBigButton(pDC);
			this->LoadFontForLittleButton(pDC);
			CDC mdc;
			mdc.CreateCompatibleDC(pDC);
			mdc.SelectObject(bm);
			pDC->BitBlt(275,370,120,30,&mdc,0,0,SRCCOPY);
			pDC->DrawText(s, CRect(275,370,395,400),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			this->LoadRaceFont(pDC);
		}
}

void CDiplomacyMenuView::DrawDiplomacyOfferMenue(CDC* pDC, CRect theClientRect, USHORT whichRace)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CBrush nb(RGB(0,0,0));
	COLORREF oldColor = pDC->GetTextColor();
	CRect rect;
	CString s;
	USHORT i = whichRace;
	USHORT race = pDoc->GetPlayersRace();
	pDC->SetTextColor(oldColor);
	rect.SetRect(200,100,560,125);
	pDC->DrawText(CResourceManager::GetString("CHOOSE_OFFER")+":" ,rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// Abfragen, ob wir ein Angebot schon gemacht haben in dieser Runde
	// wenn ja, können wir keine weiteren Angebote machen!
	// Den jeweiligen Angebotsbildschirm dann auch einblenden
	for (int l = 0; l < pDoc->m_Diplomacy[race].GetMadeOffers()->GetSize(); l++)
	{
		if ((m_bMajorOrMinor == MINOR && pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).minorRace == i && pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).round == pDoc->GetCurrentRound() && pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).majorRace == race)
			|| (m_bMajorOrMinor == MAJOR && pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).majorRace == m_iClickedOnMajorRace && pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).round == pDoc->GetCurrentRound() && pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).offerFromRace == FALSE))
		{
			m_iLatinumPayment = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).latinum;
			m_iRessourcePayment[0] = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).ressource[0];
			m_iRessourcePayment[1] = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).ressource[1];
			m_iRessourcePayment[2] = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).ressource[2];
			m_iRessourcePayment[3] = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).ressource[3];
			m_iRessourcePayment[4] = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).ressource[4];
			// auch das Umschaltfeld auf die gegebene Ressource umstellen
			if (m_iRessourcePayment[0] > 0) m_iWhichRessourceIsChosen = 0;
			else if (m_iRessourcePayment[1] > 0) m_iWhichRessourceIsChosen = 1;
			else if (m_iRessourcePayment[2] > 0) m_iWhichRessourceIsChosen = 2;
			else if (m_iRessourcePayment[3] > 0) m_iWhichRessourceIsChosen = 3;
			else if (m_iRessourcePayment[4] > 0) m_iWhichRessourceIsChosen = 4;
			// auch das System, aus dem wir die RES nehmen umschalten
			m_RessourceFromSystem = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).KO;
			if (m_bMajorOrMinor == MAJOR)
				m_iDurationOfAgreement = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).corruptedMajor / 10;
			m_iWhichRaceCorruption = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).corruptedMajor;
			m_strDiplomaticText = pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).shownText;
			m_bShowSendButton = FALSE;
			if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == TRADE_AGREEMENT)
				m_iWhichOfferButtonIsPressed = 0;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == FRIENDSHIP_AGREEMENT)
				m_iWhichOfferButtonIsPressed = 1;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == COOPERATION)
				m_iWhichOfferButtonIsPressed = 2;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == AFFILIATION)
				m_iWhichOfferButtonIsPressed = 3;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == MEMBERSHIP)
				m_iWhichOfferButtonIsPressed = 4;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == NON_AGGRESSION_PACT)
				m_iWhichOfferButtonIsPressed = 4;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == WAR)
				m_iWhichOfferButtonIsPressed = 5;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == PRESENT)
				m_iWhichOfferButtonIsPressed = 6;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == CORRUPTION)
				m_iWhichOfferButtonIsPressed = 7;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == DEFENCE_PACT)
				m_iWhichOfferButtonIsPressed = 8;
			else if (pDoc->m_Diplomacy[race].GetMadeOffers()->GetAt(l).type == WAR_PACT)
				m_iWhichOfferButtonIsPressed = 9;
			break;
		}
	}
	// Wenn wir auf den Geschenkbutton, Freundschaftsbutton, Kooperationsbutton usw. gedrückt haben
	if (m_iWhichOfferButtonIsPressed == 6 || m_iWhichOfferButtonIsPressed == 0 || m_iWhichOfferButtonIsPressed == 1 || m_iWhichOfferButtonIsPressed == 2 || m_iWhichOfferButtonIsPressed == 3 || m_iWhichOfferButtonIsPressed == 4 || m_iWhichOfferButtonIsPressed == 5 || m_iWhichOfferButtonIsPressed == 7 || m_iWhichOfferButtonIsPressed == 8 || m_iWhichOfferButtonIsPressed == 9)
	{
		// Balken zeichnen, auf dem ich den Betrag auswählen kann, außer bei Kriegserklärung
		if (m_iWhichOfferButtonIsPressed != 5)
		{
			CPen mark(PS_SOLID, 1, RGB(40,40,40));
			pDC->SelectObject(&mark);
			CRect timber[20];
			// Balken für Latinumgeschenk zeichnen
			for (int t = 0; t < 20; t++)
			{
				CBrush nb2(RGB(200-t*10,200,0));
				timber[t].SetRect(theClientRect.left+195+t*12,theClientRect.top+440,theClientRect.left+205+t*12,theClientRect.top+465);
				if ((USHORT)(m_iLatinumPayment/250) > t)
					pDC->SelectObject(&nb2);
				else
					pDC->SelectObject(&nb);
				pDC->Rectangle(timber[t]);
			}
			s.Format("%s: %d %s",CResourceManager::GetString("PAYMENT"),m_iLatinumPayment,
				CResourceManager::GetString("LATINUM"));
			rect.SetRect(195,410,450,440);
			pDC->DrawText(s,rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE);

			// Ressourcengeschenke können nur gemacht werden, wenn wir mindst. einen Handelsvertrag haben oder diese Fordern
			if ((m_bMajorOrMinor == MAJOR && pDoc->GetMajorRace(race)->GetDiplomacyStatus(m_iClickedOnMajorRace) >= TRADE_AGREEMENT)
				|| (m_bMajorOrMinor == MINOR && pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) >= TRADE_AGREEMENT)
				|| (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed == 7))
			{

				// Balken für Ressourcengeschenk zeichnen
				CString res;
				if (m_iWhichRessourceIsChosen == 0)
					res = CResourceManager::GetString("TITAN");
				else if (m_iWhichRessourceIsChosen == 1)
					res = CResourceManager::GetString("DEUTERIUM");
				else if (m_iWhichRessourceIsChosen == 2)
					res = CResourceManager::GetString("DURANIUM");
				else if (m_iWhichRessourceIsChosen == 3)
					res = CResourceManager::GetString("CRYSTAL");
				else if (m_iWhichRessourceIsChosen == 4)
					res = CResourceManager::GetString("IRIDIUM");
				for (int t = 0; t < 20; t++)
				{
					CBrush nb2(RGB(200-t*10,200,0));
					timber[t].SetRect(theClientRect.left+195+t*12,theClientRect.top+520,theClientRect.left+205+t*12,theClientRect.top+545);
					if ((USHORT)(m_iRessourcePayment[m_iWhichRessourceIsChosen]/1000) > t)
						pDC->SelectObject(&nb2);
					else
						pDC->SelectObject(&nb);
					pDC->Rectangle(timber[t]);
				}
				s.Format("%s: %d %s %s",CResourceManager::GetString("TRANSFER"), m_iRessourcePayment[m_iWhichRessourceIsChosen],
					CResourceManager::GetString("UNITS"),res);
				rect.SetRect(195,490,550,520);
				pDC->DrawText(s,rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE);

				// Wenn wir mit der Rasse mindst. einen Handelsvertrag haben, so zeigt er die ungefähre Menge des Rohstoffes an
				if (m_bMajorOrMinor == MINOR && pDoc->GetMinorRaceArray()->GetAt(i).GetDiplomacyStatus(race) >= TRADE_AGREEMENT)
				{
					if (pDoc->GetMinorRaceArray()->GetAt(i).GetResourceStorage(m_iWhichRessourceIsChosen) <= 2000)
						s = CResourceManager::GetString("SCARCELY_EXISTING");
					else if (pDoc->GetMinorRaceArray()->GetAt(i).GetResourceStorage(m_iWhichRessourceIsChosen) <= 5000)
						s = CResourceManager::GetString("VERY_LESS_EXISTING");
					else if (pDoc->GetMinorRaceArray()->GetAt(i).GetResourceStorage(m_iWhichRessourceIsChosen) <= 10000)
						s = CResourceManager::GetString("LESS_EXISTING");
					else if (pDoc->GetMinorRaceArray()->GetAt(i).GetResourceStorage(m_iWhichRessourceIsChosen) <= 15000)
						s = CResourceManager::GetString("MODERATE_EXISTING");
					else if (pDoc->GetMinorRaceArray()->GetAt(i).GetResourceStorage(m_iWhichRessourceIsChosen) <= 25000)
						s = CResourceManager::GetString("MUCH_EXISTING");
					else if (pDoc->GetMinorRaceArray()->GetAt(i).GetResourceStorage(m_iWhichRessourceIsChosen) <= 40000)
						s = CResourceManager::GetString("VERY_MUCH_EXISTING");
					else if (pDoc->GetMinorRaceArray()->GetAt(i).GetResourceStorage(m_iWhichRessourceIsChosen) > 40000)
						s = CResourceManager::GetString("ABOUNDING_EXISTING");
					rect.SetRect(195,545,550,570);
					pDC->DrawText(s,rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				}
				bm.DeleteObject();
				CDC mdc;
				this->LoadFontForLittleButton(pDC);
				COLORREF buttoncolor = pDC->GetTextColor();
				mdc.CreateCompatibleDC(pDC);
				mdc.SelectObject(bm);
				// Den Button für die Ressourcenauswahl zeichnen
				pDC->BitBlt(theClientRect.left+510,theClientRect.top+518,120,30,&mdc,0,0,SRCCOPY);
				rect.SetRect(510,518,630,548);
				pDC->DrawText(res,rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				pDC->SetTextColor(oldColor);
				rect.SetRect(510,490,630,520);
				pDC->DrawText(CResourceManager::GetString("RESOURCE"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				// Den Button für die Systemauswahl der abzuknüpfenden Ressource zeichnen, nicht bei Forderung von Majorrace
				if (m_bMajorOrMinor == MINOR || (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed != 7))
				{
					pDC->SetTextColor(buttoncolor);
					pDC->BitBlt(510,605,120,30,&mdc,0,0,SRCCOPY);
					rect.SetRect(510,605,630,635);
					s.Format("%s",pDoc->GetSector(m_RessourceFromSystem).GetName());
					pDC->DrawText(s,rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					pDC->SetTextColor(oldColor);
					rect.SetRect(510,577,630,607);
					pDC->DrawText(CResourceManager::GetString("FROM_SYSTEM"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
			bm.DeleteObject();
			CDC mdc;
			this->LoadFontForLittleButton(pDC);
			COLORREF buttoncolor = pDC->GetTextColor();
			mdc.CreateCompatibleDC(pDC);
			mdc.SelectObject(bm);
			// Wenn wir bestechen wollen, dann möglicherweise den Auswahlbutton wen wir bestechen wollen einblenden
			if (m_bMajorOrMinor == MINOR && m_iWhichOfferButtonIsPressed == 7)
			{
				if (m_iWhichRaceCorruption == NOBODY)
				{	// haben nicht auf den Button gedrückt -> mgl. Gegner auswählen
					if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) < FRIENDSHIP_AGREEMENT)
					{
						for (int t = HUMAN; t <= DOMINION; t++)
							if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(t) >= AFFILIATION && t != race)
							{
								m_iWhichRaceCorruption = t;
								break;
							}
					}
					else 
					{
						for (int t = HUMAN; t <= DOMINION; t++)
							if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(t) >= TRADE_AGREEMENT && t != race)
							{
								m_iWhichRaceCorruption = t;
								break;
							}
					}
				}
				if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(race) >= FRIENDSHIP_AGREEMENT)
				{
					switch (m_iWhichRaceCorruption)
					{
					case HUMAN:		{s = CResourceManager::GetString("RACE1");break;}
					case FERENGI:	{s = CResourceManager::GetString("RACE2");break;}
					case KLINGON:	{s = CResourceManager::GetString("RACE3");break;}
					case ROMULAN:	{s = CResourceManager::GetString("RACE4");break;}
					case CARDASSIAN:{s = CResourceManager::GetString("RACE5");break;}
					case DOMINION:	{s = CResourceManager::GetString("RACE6");break;}
					}
					pDC->SetTextColor(buttoncolor);
					pDC->BitBlt(theClientRect.left+510,theClientRect.top+438,120,30,&mdc,0,0,SRCCOPY);
					rect.SetRect(510,438,630,468);
					pDC->DrawText(s,rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					pDC->SetTextColor(oldColor);
					rect.SetRect(500,410,640,440);
					pDC->DrawText(CResourceManager::GetString("ENEMY"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
			// Wenn wir einen Vertrag einer Majorrace anbieten, dann Button für die Dauer dieses Vertrages einblenden
			else if (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed != 6 && m_iWhichOfferButtonIsPressed != 7 && m_iWhichOfferButtonIsPressed != 9)
			{
				if (m_iDurationOfAgreement == 0)
					s =CResourceManager::GetString("UNLIMITED");
				else
					s.Format("%d %s",m_iDurationOfAgreement*10,CResourceManager::GetString("ROUNDS"));
				pDC->SetTextColor(buttoncolor);
				pDC->BitBlt(theClientRect.left+510,theClientRect.top+438,120,30,&mdc,0,0,SRCCOPY);
				rect.SetRect(510,438,630,468);
				pDC->DrawText(s,rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				pDC->SetTextColor(oldColor);
				rect.SetRect(500,410,640,440);
				pDC->DrawText(CResourceManager::GetString("CONTRACT_DURATION"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			// Wenn wir einen Kriegspakt anbieten wollen, dann Button zur Auswahl des Kriegsgegners mit anzeigen
			else if  (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed == 9)
			{
				if (m_iWhichRaceCorruption == NOBODY)	// Wenn wir noch keinen Gegner ausgewählt haben
					for (int t = HUMAN; t <= DOMINION; t++)
						if (t != race && t != m_iClickedOnMajorRace && pDoc->GetMajorRace(race)->GetDiplomacyStatus(m_iClickedOnMajorRace) != WAR && pDoc->GetMajorRace(race)->GetKnownMajorRace(t) && pDoc->GetMajorRace(m_iClickedOnMajorRace)->GetKnownMajorRace(t) && pDoc->GetMajorRace(m_iClickedOnMajorRace)->GetDiplomacyStatus(t) != WAR)
						{
							m_iWhichRaceCorruption = t;
							break;
						}
						s.Format("%s",CMajorRace::GetRaceName(m_iWhichRaceCorruption));
						pDC->SetTextColor(buttoncolor);
						pDC->BitBlt(theClientRect.left+510,theClientRect.top+438,120,30,&mdc,0,0,SRCCOPY);
						rect.SetRect(510,438,630,468);
						pDC->DrawText(s,rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						pDC->SetTextColor(oldColor);
						rect.SetRect(500,410,640,440);
						pDC->DrawText(CResourceManager::GetString("WARPACT_ENEMY"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			pDC->SetTextColor(oldColor);
			bm.DeleteObject();
		}
		short type = NO_AGREEMENT;
		if (m_iWhichOfferButtonIsPressed == 0)
		{
			if (m_bMajorOrMinor == MAJOR)
				s.Format("%s %s %s",CResourceManager::GetString("TRADE_AGREEMENT_OFFER"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			else
				s.Format("%s %s %s",CResourceManager::GetString("TRADE_AGREEMENT_OFFER"), CResourceManager::GetString("TO"), pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
			CDiplomacyBottomView::SetHeadLine(s);
			type = TRADE_AGREEMENT;
		}
		else if (m_iWhichOfferButtonIsPressed == 1)
		{
			if (m_bMajorOrMinor == MAJOR)
				s.Format("%s %s %s",CResourceManager::GetString("FRIENDSHIP_OFFER"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			else
				s.Format("%s %s %s",CResourceManager::GetString("FRIENDSHIP_OFFER"), CResourceManager::GetString("TO"), pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
			CDiplomacyBottomView::SetHeadLine(s);
			type = FRIENDSHIP_AGREEMENT;
		}
		else if (m_iWhichOfferButtonIsPressed == 2)
		{
			if (m_bMajorOrMinor == MAJOR)
				s.Format("%s %s %s",CResourceManager::GetString("COOPERATION_OFFER"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			else
				s.Format("%s %s %s",CResourceManager::GetString("COOPERATION_OFFER"), CResourceManager::GetString("TO"), pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
			CDiplomacyBottomView::SetHeadLine(s);
			type = COOPERATION;
		}
		else if (m_iWhichOfferButtonIsPressed == 3)
		{
			if (m_bMajorOrMinor == MAJOR)
				s.Format("%s %s %s",CResourceManager::GetString("AFFILIATION_OFFER"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			else
				s.Format("%s %s %s",CResourceManager::GetString("AFFILIATION_OFFER"), CResourceManager::GetString("TO"), pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
			CDiplomacyBottomView::SetHeadLine(s);
			type = AFFILIATION;
		}
		else if (m_iWhichOfferButtonIsPressed == 4)
		{
			if (m_bMajorOrMinor == MAJOR)
			{
				s.Format("%s %s %s",CResourceManager::GetString("NON_AGGRESSION_OFFER"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
				type = NON_AGGRESSION_PACT;
			}
			else
			{
				s.Format("%s %s %s",CResourceManager::GetString("MEMBERSHIP_OFFER"), CResourceManager::GetString("TO"), pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
				type = MEMBERSHIP;
			}
			CDiplomacyBottomView::SetHeadLine(s);
		}
		else if (m_iWhichOfferButtonIsPressed == 8)
		{
			s.Format("%s %s %s",CResourceManager::GetString("DEFENCE_PACT_OFFER"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			CDiplomacyBottomView::SetHeadLine(s);
			type = DEFENCE_PACT;
		}
		else if (m_iWhichOfferButtonIsPressed == 9)
		{
			s.Format("%s %s %s",CResourceManager::GetString("WAR_PACT_OFFER"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			CDiplomacyBottomView::SetHeadLine(s);
			type = WAR_PACT;
		}
		else if (m_iWhichOfferButtonIsPressed == 5)
		{
			if (m_bMajorOrMinor == MAJOR)
				s.Format("%s %s %s",CResourceManager::GetString("WAR_OFFER"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			else
				s.Format("%s %s %s",CResourceManager::GetString("WAR_OFFER"), CResourceManager::GetString("TO"), pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
			CDiplomacyBottomView::SetHeadLine(s);
			type = WAR;
		}
		else if (m_iWhichOfferButtonIsPressed == 6)
		{
			if (m_bMajorOrMinor == MAJOR)
				s.Format("%s %s %s",CResourceManager::GetString("PRESENT"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			else
				s.Format("%s %s %s",CResourceManager::GetString("PRESENT"), CResourceManager::GetString("TO"), pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
			CDiplomacyBottomView::SetHeadLine(s);
			type = PRESENT;
		}
		else if (m_iWhichOfferButtonIsPressed == 7) // Bestechung bzw. Forderung bei großer Rasse
		{
			if (m_bMajorOrMinor == MAJOR) // Wir haben eine Forderung an ein anderes Imperium
				s.Format("%s %s %s",CResourceManager::GetString("REQUEST"), CResourceManager::GetString("TO"), CMajorRace::GetRaceName(i));
			else // bei Minorrace können wir bestechen
				s.Format("%s %s %s",CResourceManager::GetString("CORRUPTION"), CResourceManager::GetString("TO"), pDoc->GetMinorRaceArray()->GetAt(i).GetRaceName());
			CDiplomacyBottomView::SetHeadLine(s);
			type = DIP_REQUEST;
		}
		if (m_bMajorOrMinor == MAJOR)
		{
			if (type == WAR_PACT)
				CDiplomacyBottomView::SetText(CGenDiploMessage::GenerateMajorOffer(race,type,m_iLatinumPayment,m_iRessourcePayment,m_iWhichRaceCorruption));
			else
				CDiplomacyBottomView::SetText(CGenDiploMessage::GenerateMajorOffer(race,type,m_iLatinumPayment,m_iRessourcePayment,m_iDurationOfAgreement*10));
		}
		else
			CDiplomacyBottomView::SetText(CGenDiploMessage::GenerateMajorOffer(race,type,m_iLatinumPayment,m_iRessourcePayment));
		m_strDiplomaticText = CDiplomacyBottomView::GetText();
	}
}

CString CDiplomacyMenuView::PrintDiplomacyStatus(USHORT race, USHORT i, CDC* pDC)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CString status;
	short stat;
	if (m_bMajorOrMinor == MAJOR)
		stat = pDoc->m_MajorRace[race].GetDiplomacyStatus(i);
	else 
		stat = pDoc->GetMinorRaceArray()->GetAt(i).GetDiplomacyStatus(race);
	switch(stat)
	{
	case NO_AGREEMENT:
		return status = CResourceManager::GetString("NONE");
	case NON_AGGRESSION_PACT:
		{
			if (m_bMajorOrMinor == MAJOR && pDoc->m_MajorRace[race].GetDurationOfAgreement(i) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("NON_AGGRESSION_SHORT"),pDoc->m_MajorRace[race].GetDurationOfAgreement(i));
			else
				status = CResourceManager::GetString("NON_AGGRESSION_SHORT");
			pDC->SetTextColor(RGB(139,175,172));
			return status;
		}
	case TRADE_AGREEMENT:
		{
			if (m_bMajorOrMinor == MAJOR && pDoc->m_MajorRace[race].GetDurationOfAgreement(i) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("TRADE_AGREEMENT_SHORT"),pDoc->m_MajorRace[race].GetDurationOfAgreement(i));
			else
				status = CResourceManager::GetString("TRADE_AGREEMENT_SHORT");
			pDC->SetTextColor(RGB(233,183,12));
			return status;
		}
	case FRIENDSHIP_AGREEMENT:
		{			
			if (m_bMajorOrMinor == MAJOR && pDoc->m_MajorRace[race].GetDurationOfAgreement(i) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("FRIENDSHIP_SHORT"),pDoc->m_MajorRace[race].GetDurationOfAgreement(i));
			else
				status = CResourceManager::GetString("FRIENDSHIP_SHORT");
			pDC->SetTextColor(RGB(6,187,34));
			return status;
		}
	case COOPERATION:
		{
			if (m_bMajorOrMinor == MAJOR && pDoc->m_MajorRace[race].GetDurationOfAgreement(i) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("COOPERATION"),pDoc->m_MajorRace[race].GetDurationOfAgreement(i));
			else
				status = CResourceManager::GetString("COOPERATION");
			pDC->SetTextColor(RGB(37,159,250));
			return status;
		}
	case AFFILIATION:
		{
			if (m_bMajorOrMinor == MAJOR && pDoc->m_MajorRace[race].GetDurationOfAgreement(i) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("AFFILIATION"),pDoc->m_MajorRace[race].GetDurationOfAgreement(i));
			else
				status = CResourceManager::GetString("AFFILIATION");
			pDC->SetTextColor(RGB(29,29,248));
			return status;
		}
	case MEMBERSHIP:
		{
			pDC->SetTextColor(RGB(115,12,228));
			return status = CResourceManager::GetString("MEMBERSHIP");
		}
	case WAR:
		{
			pDC->SetTextColor(RGB(220,15,15));
			return status = CResourceManager::GetString("WAR");
		}
	default:
		return status = "";
	}
}

void CDiplomacyMenuView::TakeOrGetbackResLat(BOOL take, USHORT i)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	if (take == TRUE)
	{
		// Latinum von Latinumbesitz abziehen
		pDoc->m_Empire[pDoc->GetPlayersRace()].SetLatinum(-m_iLatinumPayment);
		m_iLatinumPayment = 0;
		// Rohstoffe aus dem Lager nehmen
		if (m_iRessourcePayment[0] > 0)
		{
			pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].SetTitanStore(-m_iRessourcePayment[0]);
			m_iRessourcePayment[0] = 0;
		}
		else if (m_iRessourcePayment[1] > 0)
		{
			pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].SetDeuteriumStore(-m_iRessourcePayment[1]);
			m_iRessourcePayment[1] = 0;
		}
		else if (m_iRessourcePayment[2] > 0)
		{
			pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].SetDuraniumStore(-m_iRessourcePayment[2]);
			m_iRessourcePayment[2] = 0;
		}
		else if (m_iRessourcePayment[3] > 0)
		{
			pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].SetCrystalStore(-m_iRessourcePayment[3]);
			m_iRessourcePayment[3] = 0;
		}
		else if (m_iRessourcePayment[4] > 0)
		{
			pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].SetIridiumStore(-m_iRessourcePayment[4]);
			m_iRessourcePayment[4] = 0;
		}
	}
	else
	{
		// Latinum wieder zurück zum Imperium geben
		pDoc->m_Empire[pDoc->GetPlayersRace()].SetLatinum(pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).latinum);
		// Die Rohstoffe wieder zurück in das System geben
		if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[0] > 0)
		{
			pDoc->m_System[pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.x][pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.y].SetTitanStore(pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[0]);
			m_iRessourcePayment[0] = 0;	
		}
		else if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[1] > 0)
		{
			pDoc->m_System[pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.x][pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.y].SetDeuteriumStore(pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[1]);
			m_iRessourcePayment[1] = 0;	
		}
		else if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[2] > 0)
		{
			pDoc->m_System[pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.x][pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.y].SetDuraniumStore(pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[2]);
			m_iRessourcePayment[2] = 0;	
		}
		else if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[3] > 0)
		{
			pDoc->m_System[pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.x][pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.y].SetCrystalStore(pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[3]);
			m_iRessourcePayment[3] = 0;	
		}
		else if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[4] > 0)
		{
			pDoc->m_System[pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.x][pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).KO.y].SetIridiumStore(pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).ressource[4]);
			m_iRessourcePayment[4] = 0;	
		}
		m_iLatinumPayment = 0;
	}
}

void CDiplomacyMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Wenn wir uns in der Diplomatieansicht befinden		
	// Überprüfen ob auf einen der Mainbutton am unteren Rand der Diplomatieansicht geklickt wurde
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_DiplomacyMainButtons, temp, FALSE))
	{
		m_bySubMenu = temp;
		m_iWhichOfferButtonIsPressed = -1;
		m_iLatinumPayment = 0;
		// im Eingangsbildschirm paar zusätzliche Variablen setzen
		if (m_bySubMenu == 2)
		{
			m_iClickedOnRace = -1;
			m_iClickedOnMajorRace = -1;
		}
		Invalidate(FALSE);
	}
	// Dem Imperium bekannte kleine Rassen anzeigen
	// hier unterscheiden, ob wir auf eine kleine Rasse oder auf eine MajorRace geklickt haben
	CRect rect;
	USHORT j = 0;
	USHORT numberOfKnownMajorRaces = pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetNumberOfKnownMajorRaces(pDoc->GetPlayersRace());
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		// Die Majorrasse abfragen, auf die wir geklickt haben wenn wir nicht im Eingangsbildschirm sind
		if (pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE && i != pDoc->GetPlayersRace() && m_bySubMenu != 2)
		{
			rect.SetRect(20,100+j*25,120,125+j*25);
			if (rect.PtInRect(point))
			{
				m_bMajorOrMinor = MAJOR;
				m_iClickedOnRace = i;
				m_iClickedOnMajorRace = i;
				m_iWhichOfferButtonIsPressed = -1;
				Invalidate();
				break;
			}
			j++;
		}
		// Die Majorrasse abfragen, auf die wir geklickt haben wenn wir im Eingangsbildschirm sind
		else if (m_bySubMenu == 2)
		{
			// Die Majorrassen abfragen die uns ein Angebot gemacht haben
			for (int l = 0; l < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetSize(); l++)
				if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace != FALSE
					&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace == i
					&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace != pDoc->GetPlayersRace()
					&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).round+1 == pDoc->GetCurrentRound())
				{
					rect.SetRect(20,100+j*25,120,125+j*25);
					if (rect.PtInRect(point))
					{
						m_bMajorOrMinor = MAJOR;
						m_iClickedOnRace = i;
						m_iClickedOnMajorRace = i;
						m_iWhichOfferButtonIsPressed = -1;
						help = j;
						Invalidate();
					}
					j++;
				}
				// Die Rassen abfragen die uns eine Antwort auf ein von uns gemachtes Angebot gegeben haben
				for (int l = 0; l < pDoc->m_Empire[pDoc->GetPlayersRace()].GetIncomingMajorAnswers()->GetSize(); l++)
					if (pDoc->m_Empire[pDoc->GetPlayersRace()].GetIncomingMajorAnswers()->GetAt(l).AnswerFromMajorRace == i)
					{
						rect.SetRect(20,100+j*25,120,125+j*25);
						if (rect.PtInRect(point))
						{
							m_bMajorOrMinor = MAJOR;
							m_iClickedOnRace = i;
							m_iClickedOnMajorRace = i;
							m_iWhichOfferButtonIsPressed = -1;
							help = j;
							Invalidate();
						}
						j++;
					}
		}
	}
	// Ab hier werden die ganzen Minorraces abgefragt
	if (m_bySubMenu != 2)
		j = pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetNumberOfKnownMajorRaces(pDoc->GetPlayersRace());
	for (int i = 0; i < pDoc->GetMinorRaceArray()->GetSize(); i++)
	{
		if (pDoc->GetMinorRaceArray()->GetAt(i).GetKnownByMajorRace(pDoc->GetPlayersRace()) == TRUE && m_bySubMenu != 2)
		{
			rect.SetRect(20,100+j*25,120,125+j*25);
			if (rect.PtInRect(point))
			{
				m_bMajorOrMinor = MINOR;
				m_iClickedOnRace = i;
				m_iWhichOfferButtonIsPressed = -1;
				Invalidate();
				break;
			}
			j++;
		}
		// Wenn wir im Eingangsbildschirm sind
		else if (m_bySubMenu == 2)
		{
			// Die Rassen anzeigen, die dem Imperium ein Angebot gemacht haben
			for (int l = 0; l < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetSize(); l++)
				if ((pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace == NOT_REACTED ||
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace == TRUE ||
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace == DECLINED) &&
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace == pDoc->GetPlayersRace() &&
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).minorRace == i)
				{
					rect.SetRect(20,100+j*25,120,125+j*25);
					if (rect.PtInRect(point))
					{
						m_bMajorOrMinor = MINOR;
						m_iClickedOnRace = i;
						m_iWhichOfferButtonIsPressed = -1;
						help = j;
						Invalidate();
					}
					j++;
				}
				// Wenn die Rasse uns eine Antwort (Reaktion) auf eins unserer Angebote gegeben hat
				for (int l = 0; l < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetAnswers()->GetSize(); l++)
					if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetAnswers()->GetAt(l).majorRace == pDoc->GetPlayersRace() && pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetAnswers()->GetAt(l).minorRace == i)
					{
						rect.SetRect(20,100+j*25,120,125+j*25);
						if (rect.PtInRect(point))
						{
							m_bMajorOrMinor = MINOR;
							m_iClickedOnRace = i;
							m_iWhichOfferButtonIsPressed = -1;
							help = j;
							Invalidate();
						}
						j++;
					}
		}
	}
		
	// Wenn wir in der Informationsansicht sind
	if (m_bySubMenu == 0 && ((m_iClickedOnRace != -1 && m_bMajorOrMinor == MINOR) || (m_iClickedOnMajorRace != -1 && m_bMajorOrMinor == MAJOR)))
	{
		//rect.SetRect(265,410,425,450);
		if (CRect(275,370,395,400).PtInRect(point) && m_bMajorOrMinor == MINOR && pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(pDoc->GetPlayersRace()) > NO_AGREEMENT)
		{
			for (int i = 0; i < 5; i++)
				m_iRessourcePayment[i] = 0;
			if (m_bShowSendButton == TRUE)
				pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,0,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,NO_AGREEMENT,m_RessourceFromSystem,NOBODY);
			else
			{
				// Angebote durchgehen, suchen ob wir schon eine Kündigung bei der Rasse gemacht haben
				for (int l = 0; l < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetSize(); l++)
					if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).minorRace == m_iClickedOnRace &&
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).round == pDoc->GetCurrentRound() &&
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace == pDoc->GetPlayersRace() &&
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).type == NO_AGREEMENT)

						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->RemoveAt(l);
			}
			CRect r(275,370,395,400);
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
	}
	// Wenn wir in der Angebotsansicht sind, überprüfen, auf welchen Angebotsbutton wir gedrückt haben
	else if (m_bySubMenu == 1 && ((m_iClickedOnRace != -1 && m_bMajorOrMinor == MINOR) || (m_iClickedOnMajorRace != -1 && m_bMajorOrMinor == MAJOR)))
	{
		// Checken ob wir ein Angebot überhaupt machen können, z.B. wenn eine andere Hauptrasse
		// z.B. Mitgliedschaft mit einer Minorrace hat, dann können wir ihr kein Angebot machen, außer
		// Krieg erklären, Geschenke geben und Bestechen
		if (m_bShowSendButton == TRUE)
		{
			int temp = m_iWhichOfferButtonIsPressed;
			if (m_bMajorOrMinor == MAJOR)
			{
				if (ButtonReactOnLeftClick(point, &m_DiplomacyMajorOfferButtons, temp, TRUE, TRUE))
				{					
					m_iWhichOfferButtonIsPressed = temp;					
					m_iLatinumPayment = 0;
					m_iWhichRaceCorruption = NOBODY;
					for (int i = TITAN; i <= IRIDIUM; i++)
						m_iRessourcePayment[i] = 0;
					return;
				}
			}
			else if (m_bMajorOrMinor == MINOR)
			{
				short status = NO_AGREEMENT;
				for (int i = HUMAN; i <= DOMINION; i++)
					if (i != pDoc->GetPlayersRace())
					{
						short temp = pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(i);
						if (temp > status)
							status = temp;
					}
				if (ButtonReactOnLeftClick(point, &m_DiplomacyMinorOfferButtons, temp, TRUE, TRUE))
				{					
					m_iWhichOfferButtonIsPressed = temp;			
					m_iLatinumPayment = 0;
					m_iWhichRaceCorruption = NOBODY;
					for (int i = TITAN; i <= IRIDIUM; i++)
						m_iRessourcePayment[i] = 0;
					return;
				}
			}
		}
		// Sind wir in dem Diplomatiemenue für Geschenke geben, Freundschaft, Kooperation, Angliederung usw. anbieten
		//if (m_iWhichOfferButtonIsPressed == 6 || m_iWhichOfferButtonIsPressed == 0 || m_iWhichOfferButtonIsPressed == 1 || m_iWhichOfferButtonIsPressed == 2 || m_iWhichOfferButtonIsPressed == 3 || m_iWhichOfferButtonIsPressed == 4 || m_iWhichOfferButtonIsPressed == 5 || m_iWhichOfferButtonIsPressed == 7 || m_iWhichOfferButtonIsPressed == 8 || m_iWhichOfferButtonIsPressed == 9)
		if (m_iWhichOfferButtonIsPressed > -1)
		{
			if (m_bShowSendButton == TRUE && m_iWhichOfferButtonIsPressed != 5)
			{
				// Balken zur Auswahl welche Menge wir verschenken
				CRect timber[21];
				for (int t = 0; t < 21; t++)
				{
					timber[t].SetRect(183+t*12,440,193+t*12+2,465);
					// Haben wir auf den Balken gedrückt, um den Wert des Geschenkes zu ändern
					if (timber[t].PtInRect(point))
					{
						m_iLatinumPayment = t*250;
						if (m_bMajorOrMinor == MINOR || (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed != 7))
							if (m_iLatinumPayment > pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum())
							{
								m_iLatinumPayment = (USHORT)(pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum()/250);
								m_iLatinumPayment *= 250;
							}
							Invalidate();
							break;
					}
					// nur wenn wir mindst. einen Handelsvertrag besitzen können wir Ressourcen verschenken oder bei einer Forderung stellen
					if ((m_bMajorOrMinor == MAJOR && pDoc->GetMajorRace(pDoc->GetPlayersRace())->GetDiplomacyStatus(m_iClickedOnMajorRace) >= TRADE_AGREEMENT)
						|| (m_bMajorOrMinor == MINOR && pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(pDoc->GetPlayersRace()) >= TRADE_AGREEMENT)
						|| (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed == 7))
					{
						timber[t].SetRect(183+t*12,520,193+t*12+2,545);
						// Haben wir auf den Balken gedrückt, um die Menge der zu verschenkenden Ressourcen zu ändern
						if (timber[t].PtInRect(point))
						{
							ULONG storages[5] = {pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].GetTitanStore(),
								pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].GetDeuteriumStore(),
								pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].GetDuraniumStore(),
								pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].GetCrystalStore(),
								pDoc->m_System[m_RessourceFromSystem.x][m_RessourceFromSystem.y].GetIridiumStore()};
							m_iRessourcePayment[m_iWhichRessourceIsChosen] = t*1000;
							if (m_bMajorOrMinor == MINOR || (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed != 7))
								if (m_iRessourcePayment[m_iWhichRessourceIsChosen] > storages[m_iWhichRessourceIsChosen])
								{
									m_iRessourcePayment[m_iWhichRessourceIsChosen] = (USHORT)(storages[m_iWhichRessourceIsChosen]/1000);
									m_iRessourcePayment[m_iWhichRessourceIsChosen] *= 1000;
								}
								Invalidate();
								break;
						}
					}
				}
				// Wenn wir auf den kleinen Ressourcenbutton geklickt haben, nächste Ressource anzeigen
				rect.SetRect(510,518,630,548);
				if (rect.PtInRect(point))
				{
					if (m_iWhichRessourceIsChosen < 4)
						m_iWhichRessourceIsChosen++;
					else
						m_iWhichRessourceIsChosen = 0;
					// Auch alle "alten" Ressourcenmengen löschen
					for (int i = TITAN; i <= IRIDIUM; i++)
						m_iRessourcePayment[i] = 0;
					Invalidate();
				}
				// Wenn wir auf den kleinen Systembutton geklickt haben um ein anderes System zu wählen
				rect.SetRect(510,605,630,635);
				if (rect.PtInRect(point) && (m_bMajorOrMinor == MINOR || (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed != 7)))
				{
					int current = -1;
					for (int i = 0; i < pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetSize(); i++)
						if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetAt(i).ko == m_RessourceFromSystem)
							current = i;
					if (current != -1)
					{
						current++;
						// Auch wieder alle Mengen auf Null zurücksetzen
						for (int i = 0; i < 5; i++)
							m_iRessourcePayment[i] = 0;
					}
					if (current == pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetSize())
						current = 0;
					m_RessourceFromSystem = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetAt(current).ko;
					Invalidate();
				}
				// Wenn wir auf den kleinen Button gedrückt haben um bei einer Bestechung den Gegner zu wählen
				rect.SetRect(510,438,630,468);
				if (rect.PtInRect(point) && m_iWhichOfferButtonIsPressed == 7 && m_bMajorOrMinor == MINOR)
				{
					if (m_iWhichRaceCorruption == DOMINION)
						m_iWhichRaceCorruption = NOBODY;
					// nur wenn wir mehr oder eine Freundschaft mit der Minorrace haben, dann können wir wählen
					if (pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(pDoc->GetPlayersRace()) >= FRIENDSHIP_AGREEMENT)
					{
						for (int i = m_iWhichRaceCorruption+1; ; i++)
						{
							if (i != pDoc->GetPlayersRace() && pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(i) >= TRADE_AGREEMENT)
							{
								m_iWhichRaceCorruption = i;
								CalcDeviceRect(rect);
								InvalidateRect(rect);
								break;
							}
							else if (i == m_iWhichRaceCorruption)
								break;
							else if (i == DOMINION)
								i = NOBODY;
						}
					}
					// ansonsten können wir nur bestechen, wenn eine andere Rasse mindst. ein Bündnis mit der Minor hat
					else
						for (int i = HUMAN; i <= DOMINION; i++)
							if (i != pDoc->GetPlayersRace() && pDoc->GetMinorRaceArray()->GetAt(m_iClickedOnRace).GetDiplomacyStatus(i) >= AFFILIATION)
							{
								m_iWhichRaceCorruption = i;
								CalcDeviceRect(rect);
								InvalidateRect(rect);
								break;
							}
				}
				// Wenn wir auf den kleinen Button geklickt haben um die Vertragsdauer zu ändern, geht nur bei Angebot an Majorrace
				else if (rect.PtInRect(point) && m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed != 6 && m_iWhichOfferButtonIsPressed != 7 && m_iWhichOfferButtonIsPressed != 9)
				{
					m_iDurationOfAgreement++;
					if (m_iDurationOfAgreement == 11)
						m_iDurationOfAgreement = 0;
					CalcDeviceRect(rect);
					InvalidateRect(rect);
				}
				// Wenn wir auf den kleinen Button geklickt haben um bei einem Kriegspakt den Kriegsgegner wählen zu wollen
				else if (rect.PtInRect(point) && m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed == 9)
				{
					if (m_iWhichRaceCorruption == DOMINION)
						m_iWhichRaceCorruption = NOBODY;
					int u = 0;
					for (int i = m_iWhichRaceCorruption+1; ; i++)
					{
						if (u == 36) // Abbruchbedingung, falls wir gar keine wählen können
							break;
						u++;
						if (i != pDoc->GetPlayersRace() && i != m_iClickedOnMajorRace && pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) && pDoc->m_MajorRace[m_iClickedOnMajorRace].GetKnownMajorRace(i)
							&& pDoc->GetMajorRace(m_iClickedOnMajorRace)->GetDiplomacyStatus(i) != WAR)
						{
							m_iWhichRaceCorruption = i;
							CalcDeviceRect(rect);
							InvalidateRect(rect);
							break;
						}
						else if (i == m_iWhichRaceCorruption)
							break;
						else if (i == DOMINION)
							i = NOBODY;
					}
				}
			}
			// Wenn wir auf den Abschicken Button geklicked haben
			/* Also hier muß auch unterschieden werden, ob das Angebot an eine Majorrace geht oder nicht.
			Wenn es an eine Major geht, dann muß ich in meinem Angebotsfeld ein Angebot machen, 'majorRace*
			ist gleich der Majorrace die das Angebot von mir bekommt und 'fromRace' ist gleich FALSE.
			Zusätzlich muß ich in dem Angebotsfeld des Diplomatieobjekts der Majorrace, an die das Angebot
			geht, auch einen Eintrag machen, mit mir als 'majorRace* und 'fromRace' ist gleich NOT_REACTED'.
			Außerdem wird hier als letzter Parameter noch die Vertragsdauer übergeben
			*/	
			//rect.SetRect(880,630,1040,670);
			rect.SetRect(871,690,1031,730);
			if (rect.PtInRect(point) && m_bShowSendButton == TRUE)
			{
				// Geschenke geben
				if (m_iWhichOfferButtonIsPressed == 6 && (m_iLatinumPayment > 0 || m_iRessourcePayment[0] > 0 || m_iRessourcePayment[1] > 0 || m_iRessourcePayment[2] > 0 ||
					m_iRessourcePayment[3] > 0 || m_iRessourcePayment[4] > 0))
				{
					if (m_bMajorOrMinor == MAJOR)
					{
						//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,PRESENT,m_RessourceFromSystem,NOBODY);
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,PRESENT,m_RessourceFromSystem,NOBODY);
					}
					else
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,PRESENT,m_RessourceFromSystem,NOBODY);
				}
				// Handelsvertrag anbieten
				if (m_iWhichOfferButtonIsPressed == 0)
				{
					if (m_bMajorOrMinor == MAJOR)
					{
						//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,TRADE_AGREEMENT,m_RessourceFromSystem,m_iDurationOfAgreement*10);
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,TRADE_AGREEMENT,m_RessourceFromSystem,m_iDurationOfAgreement*10);
					}
					else
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,TRADE_AGREEMENT,m_RessourceFromSystem,NOBODY);
				}
				// Freundschaft anbieten
				else if (m_iWhichOfferButtonIsPressed == 1)
				{
					if (m_bMajorOrMinor == MAJOR)
					{
						//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,FRIENDSHIP_AGREEMENT,m_RessourceFromSystem,m_iDurationOfAgreement*10);
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,FRIENDSHIP_AGREEMENT,m_RessourceFromSystem,m_iDurationOfAgreement*10);
					}
					else
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,FRIENDSHIP_AGREEMENT,m_RessourceFromSystem,NOBODY);
				}
				// Kooperation anbieten
				else if (m_iWhichOfferButtonIsPressed == 2)
				{
					if (m_bMajorOrMinor == MAJOR)
					{
						//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,COOPERATION,m_RessourceFromSystem,m_iDurationOfAgreement*10);
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,COOPERATION,m_RessourceFromSystem,m_iDurationOfAgreement*10);
					}
					else
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,COOPERATION,m_RessourceFromSystem,NOBODY);
				}
				// Angliederung anbieten
				else if (m_iWhichOfferButtonIsPressed == 3)
				{
					if (m_bMajorOrMinor == MAJOR)
					{
						//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,AFFILIATION,m_RessourceFromSystem,m_iDurationOfAgreement*10);
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,AFFILIATION,m_RessourceFromSystem,m_iDurationOfAgreement*10);
					}
					else
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,AFFILIATION,m_RessourceFromSystem,NOBODY);
				}
				// Mitgleidschaft anbieten bzw. bei Majorrace einen NAP anbieten
				else if (m_iWhichOfferButtonIsPressed == 4)
				{
					if (m_bMajorOrMinor == MAJOR)
					{
						//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,NON_AGGRESSION_PACT,m_RessourceFromSystem,m_iDurationOfAgreement*10);
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,NON_AGGRESSION_PACT,m_RessourceFromSystem,m_iDurationOfAgreement*10);
					}
					else
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,MEMBERSHIP,m_RessourceFromSystem,NOBODY);
				}
				// Krieg erklären
				else if (m_iWhichOfferButtonIsPressed == 5)
				{
					if (m_bMajorOrMinor == MAJOR)
					{
						//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,WAR,m_RessourceFromSystem,NOBODY);
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,WAR,m_RessourceFromSystem,NOBODY);
					}
					else
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,WAR,m_RessourceFromSystem,NOBODY);
				}
				// Bestechungsversuch starten bzw. bei Majorrace eine Bitte senden/Forderung stellen
				else if ((m_iWhichOfferButtonIsPressed == 7 && m_bMajorOrMinor == MINOR) || (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed == 7
					&& (m_iLatinumPayment > 0 || m_iRessourcePayment[0] > 0 || m_iRessourcePayment[1] > 0 || m_iRessourcePayment[2] > 0 ||
					m_iRessourcePayment[3] > 0 || m_iRessourcePayment[4] > 0)))
				{
					if (m_bMajorOrMinor == MAJOR)
					{
						//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,DIP_REQUEST,pDoc->GetRaceKO(m_iClickedOnMajorRace),NOBODY);
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,DIP_REQUEST,pDoc->GetRaceKO(m_iClickedOnMajorRace),NOBODY);
					}
					else
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMinorRace(pDoc->GetPlayersRace(),m_iClickedOnRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,CORRUPTION,m_RessourceFromSystem,(BYTE)m_iWhichRaceCorruption);
				}
				// Verteidigungsbündnis einer Majorrace anbieten
				else if (m_iWhichOfferButtonIsPressed == 8)
				{
					//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,DEFENCE_PACT,m_RessourceFromSystem,(BYTE)m_iDurationOfAgreement*10);
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,DEFENCE_PACT,m_RessourceFromSystem,(BYTE)m_iDurationOfAgreement*10);
				}
				// Kriegspakt einer Majorrace anbieten
				else if (m_iWhichOfferButtonIsPressed == 9 && m_iWhichRaceCorruption != NOBODY)
				{
					//pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetOfferFromMajorRace(pDoc->GetPlayersRace(),m_iLatinumPayment,m_iRessourcePayment,pDoc->GetNumberOfRound(),m_strDiplomaticText,WAR_PACT,m_RessourceFromSystem,(BYTE)m_iWhichRaceCorruption);
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].MakeOfferToMajorRace((BYTE)m_iClickedOnMajorRace,m_iLatinumPayment,m_iRessourcePayment,pDoc->GetCurrentRound(),m_strDiplomaticText,WAR_PACT,m_RessourceFromSystem,(BYTE)m_iWhichRaceCorruption);
				}
				// angebotenes Latinum und Ressourcen aus den Lagern nehmen, außer wenn wir eine Forderung
				// an eine andere Majorrace stellen, dann wollen wir das Zeugs ja haben
				if (m_bMajorOrMinor == MINOR || (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed != 7))
				{
					this->TakeOrGetbackResLat(TRUE,0);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				Invalidate();
			}
			// Wenn wir auf den Abbrechenbutton geklicked haben
			else if (rect.PtInRect(point) && m_bShowSendButton == FALSE)
			{
				for (int i = 0; i < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetSize(); i++)
					if ((pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).minorRace == m_iClickedOnRace &&
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).round == pDoc->GetCurrentRound() && m_bMajorOrMinor == MINOR)
						|| (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).majorRace == m_iClickedOnMajorRace &&
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(i).round == pDoc->GetCurrentRound() && m_bMajorOrMinor == MAJOR))
					{
						// angebotenes Latinum und Ressourcen wieder zurück in Lager geben, außer wenn wir
						// eine Forderung an eine andere Majorrace gestellt haben
						if (m_bMajorOrMinor == MINOR || (m_bMajorOrMinor == MAJOR && m_iWhichOfferButtonIsPressed != 7))
						{
							this->TakeOrGetbackResLat(FALSE,i);
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
						}
						else
						{
							for (int t = 0; t < 5; t++)
								m_iRessourcePayment[t] = 0;
						}
						pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->RemoveAt(i);
						// Wenn wir ein Angebot an eine andere Majorrace gemacht haben, dann müssen wir dieses Angebot auch bei
						// denen aus der Liste löschen
						if (m_bMajorOrMinor == MAJOR)
							/*for (int t = 0; t < pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetMadeOffers()->GetSize(); t++)
								if (pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetMadeOffers()->GetAt(t).majorRace == pDoc->GetPlayersRace()
									&& pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetMadeOffers()->GetAt(t).round == pDoc->GetNumberOfRound())
									pDoc->m_Diplomacy[m_iClickedOnMajorRace].GetMadeOffers()->RemoveAt(t);*/
							for (int t = 0; t < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetSize(); t++)
								if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(t).majorRace == (BYTE)m_iClickedOnMajorRace
									&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(t).round == pDoc->GetCurrentRound())
									pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->RemoveAt(t);
						Invalidate();
						break;
					}
			}
		}

	}

	// Wenn wir in dem Eingangsbildschirm sind
	else if (m_bySubMenu == 2)
	{
		// Annehmenbutton
		rect.SetRect(852,480,1012,520);
		if (rect.PtInRect(point) && m_bShowSendButton == TRUE)
			for (int l = 0; l < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetSize(); l++)
				if (((pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace == NOT_REACTED ||
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace == DECLINED) &&
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace == pDoc->GetPlayersRace() &&
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).minorRace == m_iClickedOnRace &&
					m_bMajorOrMinor == MINOR)
					|| // oder wir haben ein Angebot von einer anderen großen Rasse
					(pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace != FALSE 
					&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace == m_iClickedOnMajorRace
					&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace != pDoc->GetPlayersRace()
					&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).round+1 == pDoc->GetCurrentRound()
					&& m_bMajorOrMinor == MAJOR))
				{
					pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->ElementAt(l).offerFromRace = TRUE;
					// Wenn wir eine Forderung annehmen, dann sofort das Latinum und die RES aus den Lagern nehmen
					if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).type == DIP_REQUEST)
					{
						m_iLatinumPayment = pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).latinum;
						m_RessourceFromSystem = pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).KO;
						*m_iRessourcePayment = *pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).ressource;
						this->TakeOrGetbackResLat(TRUE,l);
						pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
					}
					Invalidate();
					break;
				}
				// Ablehnenbutton
				rect.SetRect(852,599,1012,639);
				if (rect.PtInRect(point) && m_bShowDeclineButton == TRUE)
					for (int l = 0; l < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetSize(); l++)
						if (((pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace == NOT_REACTED ||
							pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace == TRUE) &&
							pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace == pDoc->GetPlayersRace() &&
							pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).minorRace == m_iClickedOnRace &&
							m_bMajorOrMinor == MINOR)
							||
							(pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace != FALSE 
							&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace == m_iClickedOnMajorRace
							&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace != pDoc->GetPlayersRace()
							&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).round+1 == pDoc->GetCurrentRound()
							&& m_bMajorOrMinor == MAJOR))
						{
							// Wenn wir eine Forderung zuerst angenommen hatten und diese in der selben Runde doch ablehnen
							// wollen, so die geforderten RES und Latinum wieder zurückgeben
							if (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).type == DIP_REQUEST
								&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace == TRUE)
							{
								m_iLatinumPayment = pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).latinum;
								m_RessourceFromSystem = pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).KO;
								*m_iRessourcePayment = *pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).ressource;
								this->TakeOrGetbackResLat(FALSE,l);
								pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
							}
							pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->ElementAt(l).offerFromRace = DECLINED;
							Invalidate();
							break;
						}
						// Systemauswahlbutton, mit dem wir das System wählen können, woraus wir die Ressourcen abzapfen, wenn wir
						// eine Forderung einer anderen Majorrasse erfüllen wollen, welche Ressourcen beinhaltet
						rect.SetRect(476,100+help*25,596,125+help*25);
						if (rect.PtInRect(point))
							for (int l = 0; l < pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetSize(); l++)
							{
								USHORT *ressource = pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).ressource;
								if ((ressource[0] > 0 || ressource[1] > 0 || ressource[2] > 0 || ressource[3] > 0 || ressource[4] > 0)
									&& (pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace != FALSE 
									&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).offerFromRace != TRUE
									&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace == m_iClickedOnMajorRace
									&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).majorRace != pDoc->GetPlayersRace()
									&& pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->GetAt(l).round+1 == pDoc->GetCurrentRound()))
								{
									// Nächstes System finden, wenn wir durchklicken
									BYTE race = pDoc->GetPlayersRace();
									CPoint curPoint = pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->ElementAt(l).KO;
									int current = -1;
									for (int i = 0; i < pDoc->GetEmpire(race)->GetSystemList()->GetSize(); i++)
										if (pDoc->GetEmpire(race)->GetSystemList()->GetAt(i).ko == curPoint)
											current = i;
									if (current != -1)
										current++;
									if (current == pDoc->GetEmpire(race)->GetSystemList()->GetSize())
										current = 0;
									pDoc->m_Diplomacy[pDoc->GetPlayersRace()].GetMadeOffers()->ElementAt(l).KO = pDoc->GetEmpire(race)->GetSystemList()->GetAt(current).ko;
									Invalidate();
								}
							}		
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CDiplomacyMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Wenn wir uns in der Diplomatieansicht befinden
	ButtonReactOnMouseOver(point, &m_DiplomacyMainButtons);
	if (m_bySubMenu == 1)
		if (m_bShowSendButton)
		{
			if (m_bMajorOrMinor == MAJOR)
				ButtonReactOnMouseOver(point, &m_DiplomacyMajorOfferButtons);
			else if (m_bMajorOrMinor == MINOR)
				ButtonReactOnMouseOver(point, &m_DiplomacyMinorOfferButtons);
		}

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CDiplomacyMenuView::CreateButtons()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// alle Buttons in der View anlegen und Grafiken laden
	switch(pDoc->GetPlayersRace())
	{
	case HUMAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2i.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2a.jpg";
			// Buttons in den Diplomatieansichten
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_INFORMATION"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(200,690) , CSize(160,40), CResourceManager::GetString("OFFER"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(390,690) , CSize(160,40), CResourceManager::GetString("RECEIPT"), fileN, fileI, fileA));
			// Angebotsbuttons in der Diplomatieangebotsansicht
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,140) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,140) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,190) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,190) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,240) , CSize(160,40), CResourceManager::GetString("BTN_NAP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,340) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,290) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,290) , CSize(160,40), CResourceManager::GetString("BTN_REQUEST"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,240) , CSize(160,40), CResourceManager::GetString("BTN_DEFENCE"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,340) , CSize(160,40), CResourceManager::GetString("BTN_WARPACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,165) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,165) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,215) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,215) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,265) , CSize(160,40), CResourceManager::GetString("BTN_MEMBERSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,265) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,315) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,315) , CSize(160,40), CResourceManager::GetString("BTN_CORRUPTION"), fileN, fileI, fileA));
			break;			
		}
	case FERENGI:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttona.jpg";
			// Buttons in den Diplomatieansichten
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_INFORMATION"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(200,690) , CSize(160,40), CResourceManager::GetString("OFFER"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(390,690) , CSize(160,40), CResourceManager::GetString("RECEIPT"), fileN, fileI, fileA));
			// Angebotsbuttons in der Diplomatieangebotsansicht
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,140) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,140) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,190) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,190) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,240) , CSize(160,40), CResourceManager::GetString("BTN_NAP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,340) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,290) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,290) , CSize(160,40), CResourceManager::GetString("BTN_REQUEST"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,240) , CSize(160,40), CResourceManager::GetString("BTN_DEFENCE"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,340) , CSize(160,40), CResourceManager::GetString("BTN_WARPACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,165) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,165) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,215) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,215) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,265) , CSize(160,40), CResourceManager::GetString("BTN_MEMBERSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,265) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,315) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,315) , CSize(160,40), CResourceManager::GetString("BTN_CORRUPTION"), fileN, fileI, fileA));
			break;
		}
	case KLINGON:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "buttona.jpg";
			// Buttons in den Diplomatieansichten
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_INFORMATION"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(200,690) , CSize(160,40), CResourceManager::GetString("OFFER"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(390,690) , CSize(160,40), CResourceManager::GetString("RECEIPT"), fileN, fileI, fileA));
			// Angebotsbuttons in der Diplomatieangebotsansicht
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,140) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,140) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,190) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,190) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,240) , CSize(160,40), CResourceManager::GetString("BTN_NAP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,340) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,290) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,290) , CSize(160,40), CResourceManager::GetString("BTN_REQUEST"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,240) , CSize(160,40), CResourceManager::GetString("BTN_DEFENCE"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,340) , CSize(160,40), CResourceManager::GetString("BTN_WARPACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,165) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,165) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,215) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,215) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,265) , CSize(160,40), CResourceManager::GetString("BTN_MEMBERSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,265) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,315) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,315) , CSize(160,40), CResourceManager::GetString("BTN_CORRUPTION"), fileN, fileI, fileA));
			break;
		}
	case ROMULAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttona.jpg";
			// Buttons in den Diplomatieansichten
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_INFORMATION"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(200,690) , CSize(160,40), CResourceManager::GetString("OFFER"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(390,690) , CSize(160,40), CResourceManager::GetString("RECEIPT"), fileN, fileI, fileA));
			// Angebotsbuttons in der Diplomatieangebotsansicht
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,140) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,140) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,190) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,190) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,240) , CSize(160,40), CResourceManager::GetString("BTN_NAP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,340) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,290) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,290) , CSize(160,40), CResourceManager::GetString("BTN_REQUEST"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,240) , CSize(160,40), CResourceManager::GetString("BTN_DEFENCE"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,340) , CSize(160,40), CResourceManager::GetString("BTN_WARPACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,165) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,165) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,215) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,215) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,265) , CSize(160,40), CResourceManager::GetString("BTN_MEMBERSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,265) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,315) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,315) , CSize(160,40), CResourceManager::GetString("BTN_CORRUPTION"), fileN, fileI, fileA));
			break;
		}
	case CARDASSIAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttona.jpg";
			// Buttons in den Diplomatieansichten
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_INFORMATION"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(200,690) , CSize(160,40), CResourceManager::GetString("OFFER"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(390,690) , CSize(160,40), CResourceManager::GetString("RECEIPT"), fileN, fileI, fileA));
			// Angebotsbuttons in der Diplomatieangebotsansicht
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,140) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,140) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,190) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,190) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,240) , CSize(160,40), CResourceManager::GetString("BTN_NAP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,340) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,290) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,290) , CSize(160,40), CResourceManager::GetString("BTN_REQUEST"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,240) , CSize(160,40), CResourceManager::GetString("BTN_DEFENCE"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,340) , CSize(160,40), CResourceManager::GetString("BTN_WARPACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,165) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,165) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,215) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,215) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,265) , CSize(160,40), CResourceManager::GetString("BTN_MEMBERSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,265) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,315) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,315) , CSize(160,40), CResourceManager::GetString("BTN_CORRUPTION"), fileN, fileI, fileA));
			break;
		}
	case DOMINION:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttona.jpg";
			// Buttons in den Diplomatieansichten
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_INFORMATION"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(200,690) , CSize(160,40), CResourceManager::GetString("OFFER"), fileN, fileI, fileA));
			m_DiplomacyMainButtons.Add(new CMyButton(CPoint(390,690) , CSize(160,40), CResourceManager::GetString("RECEIPT"), fileN, fileI, fileA));
			// Angebotsbuttons in der Diplomatieangebotsansicht
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,140) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,140) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,190) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,190) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,240) , CSize(160,40), CResourceManager::GetString("BTN_NAP"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,340) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,290) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,290) , CSize(160,40), CResourceManager::GetString("BTN_REQUEST"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,240) , CSize(160,40), CResourceManager::GetString("BTN_DEFENCE"), fileN, fileI, fileA));
			m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,340) , CSize(160,40), CResourceManager::GetString("BTN_WARPACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,165) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,165) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,215) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,215) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,265) , CSize(160,40), CResourceManager::GetString("BTN_MEMBERSHIP"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,265) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,315) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
			m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,315) , CSize(160,40), CResourceManager::GetString("BTN_CORRUPTION"), fileN, fileI, fileA));
			break;
		}
	}
}
