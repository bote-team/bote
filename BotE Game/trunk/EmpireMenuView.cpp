// EmpireMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "SystemMenuView.h"
#include "ResearchMenuView.h"
#include "IntelMenuView.h"
#include "DiplomacyMenuView.h"
#include "EmpireMenuView.h"
#include "GalaxyMenuView.h"
#include "PlanetBottomView.h"
#include "MenuChooseView.h"

// CEmpireMenuView

IMPLEMENT_DYNCREATE(CEmpireMenuView, CMainBaseView)

CEmpireMenuView::CEmpireMenuView()
{

}

CEmpireMenuView::~CEmpireMenuView()
{
	for (int i = 0; i < m_EmpireNewsButtons.GetSize(); i++)
	{
		delete m_EmpireNewsButtons[i];
		m_EmpireNewsButtons[i] = 0;
	}
	m_EmpireNewsButtons.RemoveAll();
	for (int i = 0; i < m_EmpireSystemFilterButtons.GetSize(); i++)
	{
		delete m_EmpireSystemFilterButtons[i];
		m_EmpireSystemFilterButtons[i] = 0;
	}
	m_EmpireSystemFilterButtons.RemoveAll();
	for (int i = 0; i < m_EmpireShipsFilterButtons.GetSize(); i++)
	{
		delete m_EmpireShipsFilterButtons[i];
		m_EmpireShipsFilterButtons[i] = 0;
	}
	m_EmpireShipsFilterButtons.RemoveAll();
	for (int i = 0; i < m_EmpireNewsFilterButtons.GetSize(); i++)
	{
		delete m_EmpireNewsFilterButtons[i];
		m_EmpireNewsFilterButtons[i] = 0;
	}
	m_EmpireNewsFilterButtons.RemoveAll();
}

BEGIN_MESSAGE_MAP(CEmpireMenuView, CMainBaseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CEmpireMenuView::OnNewRound()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	// Sortierung der Systemliste nach dem Namen der Systeme
	c_arraysort<CArray<SystemViewStruct>,SystemViewStruct>(*pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList() ,sort_asc);
	// Sortierung der Schiffshistory nach dem Namen der Schiffe
	c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_shipname);

	m_iSubMenu = 0;
	m_iWhichNewsButtonIsPressed = NO_TYPE;
	m_iClickedNews = -1;
	m_iClickedSystem = -1;
	m_iClickedShip = -1;
	m_bShowAliveShips = TRUE;
	m_bShowResources = FALSE;
}

// CEmpireMenuView drawing
void CEmpireMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here
		
	CMemDC pDC(dc);
	pDC->SetBkMode(TRANSPARENT);
	if (pDoc->m_pIniLoader->GetValue("SMOOTHSCALING"))
		pDC->SetStretchBltMode(HALFTONE);
		
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);
	LoadRaceFont(pDC);

	if (m_iSubMenu == 0)
		DrawEmpireNewsMenue(pDC,r);
	else if (m_iSubMenu == 1)
		DrawEmpireSystemMenue(pDC,r);
	else if (m_iSubMenu == 2)
		DrawEmpireShipMenue(pDC,r);
}

void CEmpireMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	CreateButtons();

	bg_newsovmenu.DeleteObject();
	bg_systemovmenu.DeleteObject();
	bg_shipovmenu.DeleteObject();

	CString race = "";
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
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"newsovmenu.jpg");
	bg_newsovmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"systemovmenu.jpg");
	bg_systemovmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"shipovmenu.jpg");
	bg_shipovmenu.Attach(FCWin32::CreateDDBHandle(img));

	m_iSubMenu = 0;
	m_iWhichNewsButtonIsPressed = NO_TYPE;
	m_iClickedNews = -1;
	m_iClickedSystem = -1;
	m_iClickedShip = -1;
	m_bShowAliveShips = TRUE;
	m_bShowResources = FALSE;
}

void CEmpireMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CEmpireMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CMainBaseView::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Nachrichtenmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CEmpireMenuView::DrawEmpireNewsMenue(CDC* pDC, CRect theClientRect)
{	
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r = theClientRect;
	CString s;
	BYTE race = pDoc->GetPlayersRace();

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_newsovmenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
	
	// grobe Linien zeichnen
	COLORREF oldColor = pDC->GetTextColor();
	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	CPen mark;
	COLORREF markColor;
	if (pDoc->GetPlayersRace() == HUMAN)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
	}
	else if (pDoc->GetPlayersRace() == FERENGI)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
	}
	else if (race == CARDASSIAN)
	{
		markColor = RGB(255,128,0);
		mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
	}
	else
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
	}
	pDC->SelectObject(&mark);

	// Es gehen nur 21 Einträge auf die Seite, deshalb muss abgebrochen werden
	// Wenn noch keine News angeklickt wurde, es aber News gibt, dann die erste in der Reihe markieren
	if (m_iClickedNews == -1 && pDoc->m_Empire[race].GetMessages()->GetSize() > 0)
	{
		m_iClickedNews = 0;
		m_iOldClickedNews = 0;
	}
	int j = 0;
	short counter = m_iClickedNews - 20 + m_iOldClickedNews;
	short oldClickedNews = m_iClickedNews;
	for (int i = 0; i < pDoc->m_Empire[race].GetMessages()->GetSize(); i++)
	// nur Nachrichten anzeigen, dessen Typ wir auch gewählt haben
		if (m_iWhichNewsButtonIsPressed == pDoc->m_Empire[race].GetMessages()->GetAt(i).GetMessageType() ||
			m_iWhichNewsButtonIsPressed == NO_TYPE)
		{
			if (counter > 0)
			{
				m_iClickedNews--;
				counter--;
				continue;
			}
			if (j < 21)
			{
				// Die News markieren
				if (j == m_iClickedNews)
				{
					pDC->SetTextColor(markColor);
					pDC->MoveTo(r.left+100,r.top+140+j*25); pDC->LineTo(r.right-100,r.top+140+j*25);
					pDC->MoveTo(r.left+100,r.top+140+j*25+25); pDC->LineTo(r.right-100,r.top+140+j*25+25);
				}
				else
				{
					switch (pDoc->m_Empire[race].GetMessages()->GetAt(i).GetMessageType())
					{
						case ECONOMY:  pDC->SetTextColor(RGB(0,150,0));		break;
						case RESEARCH: pDC->SetTextColor(RGB(50,75,255));	break;
						case SECURITY: pDC->SetTextColor(RGB(155,25,255));	break;
						case DIPLOMACY:pDC->SetTextColor(RGB(255,220,0));	break;
						case MILITARY: pDC->SetTextColor(RGB(255,0,0));		break;
						default: pDC->SetTextColor(oldColor);
					}
				}
				pDoc->m_Empire[race].GetMessages()->GetAt(i).ShowMessage(pDC,CRect(r.left+100,r.top+140+j*25,r.right-100,r.top+165+j*25));
				j++;
			}
		}
	m_iClickedNews = oldClickedNews;
	
	LoadFontForBigButton(pDC);
	// Buttons am oberen Bildrand zeichnen, womit wir die Nachrichtenart verändern können, die angezeigt wird
	DrawButtons(pDC, &m_EmpireNewsFilterButtons, m_iWhichNewsButtonIsPressed);
	// Buttons am unteren Bildrand zeichnen
	DrawButtons(pDC, &m_EmpireNewsButtons, m_iSubMenu);
	
	// "Nachrichten und Informationen" in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("NEWS_MENUE") ,CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Menüs zur Anzeige aller Systeme im Imperium
/////////////////////////////////////////////////////////////////////////////////////////
void CEmpireMenuView::DrawEmpireSystemMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r = theClientRect;
	CString s;
	BYTE race = pDoc->GetPlayersRace();

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_systemovmenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
	
	// grobe Linien zeichnen
	COLORREF oldColor = pDC->GetTextColor();
	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	CPen mark;
	COLORREF markColor;
	if (pDoc->GetPlayersRace() == HUMAN)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
	}
	else if (pDoc->GetPlayersRace() == FERENGI)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
	}
	else if (race == CARDASSIAN)
	{
		markColor = RGB(255,128,0);
		mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
	}
	else
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
	}
	pDC->SelectObject(&mark);
/*	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.top+110);
	pDC->LineTo(theClientRect.right,theClientRect.top+110);
	CBrush nb(RGB(20,20,20));
	pDC->SelectObject(&nb);
	pDC->Rectangle(theClientRect.left+50,theClientRect.top+140,theClientRect.right-50,theClientRect.bottom-80);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-50);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-50);*/
	// Vertikale Linien in der Ansicht
/*	pDC->MoveTo(r.left+130,r.top+140);
	pDC->LineTo(r.left+130,r.bottom-80);
	pDC->MoveTo(r.left+270,r.top+140);
	pDC->LineTo(r.left+270,r.bottom-80);
	pDC->MoveTo(r.left+400,r.top+140);
	pDC->LineTo(r.left+400,r.bottom-80);
	pDC->MoveTo(r.left+480,r.top+140);
	pDC->LineTo(r.left+480,r.bottom-80);
	pDC->MoveTo(r.left+580,r.top+140);
	pDC->LineTo(r.left+580,r.bottom-80);
	pDC->MoveTo(r.left+680,r.top+140);
	pDC->LineTo(r.left+680,r.bottom-80);
*/		
	// Wenn noch kein System angeklickt wurde, es aber Systeme gibt, dann das erste in der Liste markieren
	if (m_iClickedSystem == -1 && pDoc->GetEmpire(race)->GetSystemList()->GetSize() > 0)
	{
		m_iClickedSystem = 0;
		m_iOldClickedSystem = 0;
	}
	short j = 0;
	short counter = m_iClickedSystem - 20 + m_iOldClickedSystem;
	short oldClickedSystem = m_iClickedSystem;
	// Es gehen nur 21 Einträge auf die Seite, deshalb muss abgebrochen werden
	for (int i = 0; i < pDoc->GetEmpire(race)->GetSystemList()->GetSize(); i++)
	{
		if (counter > 0)
		{
			m_iClickedSystem--;
			counter--;
			continue;
		}
		if (j < 21)
		{
			// Wenn System markiert ist, dann andere Schriftfarbe wählen
			if (j == m_iClickedSystem)
			{
				pDC->SetTextColor(markColor);
				pDC->MoveTo(r.left+46,r.top+140+j*25); pDC->LineTo(r.right-45,r.top+140+j*25);
				pDC->MoveTo(r.left+46,r.top+140+j*25+25); pDC->LineTo(r.right-45,r.top+140+j*25+25);
			}
			else
				pDC->SetTextColor(oldColor);
			CPoint KO = pDoc->GetEmpire(race)->GetSystemList()->GetAt(i).ko;
			s.Format("%c%i",(char)(KO.y+97),KO.x+1);
			pDC->DrawText(s, CRect(r.left+55,r.top+140+j*25,r.left+130,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			pDC->DrawText(pDoc->GetEmpire(race)->GetSystemList()->GetAt(i).name, CRect(r.left+130,r.top+140+j*25,r.left+270,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			
			// soll die normale Systemüberischt angezeigt werden
			if (!m_bShowResources)
			{
				// Moral anzeigen
				/*	Fanatic		175 - 194
					Loyal		155 - 174
					Pleased		131 - 154
					Content		100 - 130
					Apathetic	76 - 99
					Disgruntled	50 - 75
					Defiant		30 - 49
					Rebellious	0 - 29
				*/
				if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 174)
				{
					s = CResourceManager::GetString("FANATIC");
					pDC->SetTextColor(RGB(0,250,0));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 154)
				{
					s = CResourceManager::GetString("LOYAL");
					pDC->SetTextColor(RGB(20,150,20));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 130)
				{
					s = CResourceManager::GetString("PLEASED");
					pDC->SetTextColor(RGB(20,150,100));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 99)
				{
					s = CResourceManager::GetString("SATISFIED");
					pDC->SetTextColor(RGB(150,150,200));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 75)
				{
					s = CResourceManager::GetString("APATHETIC");
					pDC->SetTextColor(RGB(160,160,160));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 49)
				{
					s = CResourceManager::GetString("ANGRY");
					pDC->SetTextColor(RGB(200,100,50));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 29)
				{
					s = CResourceManager::GetString("FURIOUS");
					pDC->SetTextColor(RGB(210,80,50));
				}
				else
				{
					s = CResourceManager::GetString("REBELLIOUS");
					pDC->SetTextColor(RGB(255,0,0));
				}
				pDC->DrawText(s, CRect(r.left+270,r.top+140+j*25,r.left+400,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				// Nahrungsproduktion anzeigen
				int food = pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetFoodProd();
				s.Format("%d",food);
				if (food < 0)
					pDC->SetTextColor(RGB(250,0,0));
				else
					pDC->SetTextColor(RGB(0,250,0));
				pDC->DrawText(s, CRect(r.left+400,r.top+140+j*25,r.left+480,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				// Wenn System markiert ist, dann andere Schriftfarbe wählen
				if (j == m_iClickedSystem)
					pDC->SetTextColor(markColor);
				else
					pDC->SetTextColor(oldColor);
				// Nahrungslager anzeigen
				s.Format("%d",pDoc->GetSystem(KO.x,KO.y).GetFoodStore());
				pDC->DrawText(s, CRect(r.left+480,r.top+140+j*25,r.left+580,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				// Industrieproduktion anzeigen
				int ip = pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetIndustryProd();
				s.Format("%d",ip);
				pDC->DrawText(s, CRect(r.left+580,r.top+140+j*25,r.left+680,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				
				// Bauauftrag anzeigen inkl. Anzeige der noch nötigen Runden bis Bauauftrag fertig ist
				short id	= pDoc->GetSystem(KO.x,KO.y).GetAssemblyList()->GetAssemblyListEntry(0);
				int RoundToBuild = 0;
				if (id == 0)
					s = "";
				else if (id > 0 && id < 10000)
				{
					if (ip > 0 && !pDoc->GetBuildingInfo(id).GetNeverReady())
					{
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(KO.x,KO.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(0))/((float)ip
							* (100+pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetBuildingBuildSpeed())/100));
						if (RoundToBuild > 1)
							s.Format("%s (%d %s)",pDoc->GetBuildingName(id),RoundToBuild,CResourceManager::GetString("ROUNDS"));
						else
							s.Format("%s (%d %s)",pDoc->GetBuildingName(id),RoundToBuild,CResourceManager::GetString("ROUND"));
					}
					else
						s.Format("%s (? %s)",pDoc->GetBuildingName(id),CResourceManager::GetString("ROUNDS"));
				}
				else if (id >= 10000 && id < 20000)
				{
					if (pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetShipYardEfficiency() > 0 && ip > 0)
					{
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(KO.x,KO.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
								/((float)ip * pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetShipYardEfficiency() / 100
									* (100+pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetShipBuildSpeed())/100));
						if (RoundToBuild > 1)
							s.Format("%s-%s (%d %s)",pDoc->m_ShipInfoArray.GetAt(id-10000).GetShipClass(),CResourceManager::GetString("CLASS"),
							RoundToBuild,CResourceManager::GetString("ROUNDS"));
						else
							s.Format("%s-%s (%d %s)",pDoc->m_ShipInfoArray.GetAt(id-10000).GetShipClass(),CResourceManager::GetString("CLASS"),
							RoundToBuild,CResourceManager::GetString("ROUND"));
					}
					else
						s.Format("%s-%s (? %s)",pDoc->m_ShipInfoArray.GetAt(id-10000).GetShipClass(),CResourceManager::GetString("CLASS"),
						CResourceManager::GetString("ROUNDS"));
				}
				else if (id >= 20000)
				{
					if (pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetBarrackEfficiency() > 0 && ip > 0)
					{
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(KO.x,KO.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
								/((float)ip * pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetBarrackEfficiency() / 100
									* (100+pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetTroopBuildSpeed())/100));
						if (RoundToBuild > 1)
							s.Format("%s (%d %s)",pDoc->m_TroopInfo.GetAt(id-20000).GetName(),RoundToBuild,CResourceManager::GetString("ROUNDS"));
						else
							s.Format("%s (%d %s)",pDoc->m_TroopInfo.GetAt(id-20000).GetName(),RoundToBuild,CResourceManager::GetString("ROUND"));
					}
					else
						s.Format("%s (? %s)",pDoc->m_TroopInfo.GetAt(id-20000).GetName(),CResourceManager::GetString("ROUNDS"));
				}
				else if (id < 0)
				{
					if (ip > 0 && !pDoc->GetBuildingInfo(-id).GetNeverReady())
					{
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(KO.x,KO.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(0))/((float)ip
							* (100+pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetUpdateBuildSpeed())/100));
						if (RoundToBuild > 1)
							s.Format("Upgrade %s (%d %s)",pDoc->GetBuildingName(-id),RoundToBuild,CResourceManager::GetString("ROUNDS"));
						else
							s.Format("Upgrade %s (%d %s)",pDoc->GetBuildingName(-id),RoundToBuild,CResourceManager::GetString("ROUND"));
					}
					else
						s.Format("Upgrade %s (? %s)",pDoc->GetBuildingName(-id),CResourceManager::GetString("ROUNDS"));
				}
				if (id > 0 && id < 10000 && pDoc->GetBuildingInfo(id).GetNeverReady())
					s.Format("%s (%s)",pDoc->GetBuildingName(id),CResourceManager::GetString("RUNS"));
				if (pDoc->GetSystem(KO.x, KO.y).GetAutoBuild())
				{
					s.Insert(0, CResourceManager::GetString("AUTOBUILD") + ": ");
					pDC->SetTextColor(RGB(255,0,0));
				}
				pDC->DrawText(s, CRect(r.left+680,r.top+140+j*25,r.right-50,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->SetTextColor(oldColor);
			}
			// andernfalls wird hier die Systemressourcenübersicht angezeigt
			else
			{
				for (int k = TITAN; k <= DILITHIUM; k++)
				{
					s.Format("%d", pDoc->GetSystem(KO.x, KO.y).GetRessourceStore(k));
					pDC->DrawText(s, CRect(r.left+270+k*120,r.top+140+j*25,r.left+390+k*120,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				}
			}
			j++;
		}
	}
	m_iClickedSystem = oldClickedSystem;

	// Buttons am unteren Bildrand zeichnen, womit wir die Menüs umschalten können
	LoadFontForBigButton(pDC);
	// Buttons am unteren Bildrand zeichnen
	DrawButtons(pDC, &m_EmpireNewsButtons, m_iSubMenu);
	// Buttons zum Umschalten zwischen normaler und Ressourcenansicht
	DrawButtons(pDC, &m_EmpireSystemFilterButtons, m_bShowResources);
	
	// Tabellenüberschriften zeichnen
	pDC->SetTextColor(RGB(200,200,200));
	pDC->DrawText(CResourceManager::GetString("SECTOR"),CRect(r.left+50,r.top+110,r.left+130,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	pDC->DrawText(CResourceManager::GetString("NAME"),CRect(r.left+130,r.top+110,r.left+270,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	if (!m_bShowResources)
	{
		pDC->DrawText(CResourceManager::GetString("MORAL"),CRect(r.left+270,r.top+110,r.left+400,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("FOOD"),CRect(r.left+400,r.top+110,r.left+480,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		pDC->DrawText(CResourceManager::GetString("STORAGE"),CRect(r.left+480,r.top+110,r.left+580,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("INDUSTRY"),CRect(r.left+580,r.top+110,r.left+680,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		pDC->DrawText(CResourceManager::GetString("JOB"),CRect(r.left+680,r.top+110,r.right-50,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		pDC->DrawText(CResourceManager::GetString("TITAN"),CRect(r.left+270,r.top+110,r.left+390,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("DEUTERIUM"),CRect(r.left+390,r.top+110,r.left+510,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		pDC->DrawText(CResourceManager::GetString("DURANIUM"),CRect(r.left+510,r.top+110,r.left+630,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("CRYSTAL"),CRect(r.left+630,r.top+110,r.left+750,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		pDC->DrawText(CResourceManager::GetString("IRIDIUM"),CRect(r.left+750,r.top+110,r.left+870,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("DILITHIUM"),CRect(r.left+870,r.top+110,r.right-50,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
		
	// "Übersicht aller Systeme" oben in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("SYSTEM_OVERVIEW_MENUE") ,CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Menüs zur Anzeige aller Schiffe des Imperium
/////////////////////////////////////////////////////////////////////////////////////////
void CEmpireMenuView::DrawEmpireShipMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r = theClientRect;
	CString s;
	BYTE race = pDoc->GetPlayersRace();

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_shipovmenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);	
	
	COLORREF oldColor = pDC->GetTextColor();
	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	CPen mark;
	COLORREF markColor;
	if (pDoc->GetPlayersRace() == HUMAN)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
	}
	else if (pDoc->GetPlayersRace() == FERENGI)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
	}
	else if (race == CARDASSIAN)
	{
		markColor = RGB(255,128,0);
		mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
	}
	else
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
	}
	pDC->SelectObject(&mark);

	// grobe Linien zeichnen
/*	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.top+110);
	pDC->LineTo(theClientRect.right,theClientRect.top+110);
	CBrush nb(RGB(20,20,20));
	pDC->SelectObject(&nb);
	pDC->Rectangle(theClientRect.left+50,theClientRect.top+140,theClientRect.right-50,theClientRect.bottom-80);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-50);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-50);*/
	// Vertikale Linien in der Ansicht
/*	pDC->MoveTo(r.left+225,r.top+140);
	pDC->LineTo(r.left+225,r.bottom-80);
	pDC->MoveTo(r.left+375,r.top+140);
	pDC->LineTo(r.left+375,r.bottom-80);
	pDC->MoveTo(r.left+495,r.top+140);
	pDC->LineTo(r.left+495,r.bottom-80);
	pDC->MoveTo(r.left+560,r.top+140);
	pDC->LineTo(r.left+560,r.bottom-80);
	pDC->MoveTo(r.left+675,r.top+140);
	pDC->LineTo(r.left+675,r.bottom-80);
	if (m_bShowAliveShips)
		pDC->MoveTo(r.left+790,r.top+140);
	pDC->LineTo(r.left+790,r.bottom-80);
	pDC->MoveTo(r.left+850,r.top+140);
	pDC->LineTo(r.left+850,r.bottom-80);*/
	
	// Wenn noch kein Schiff angeklickt wurde, es aber Schiffe gibt, dann das erste in der Liste markieren
	if (m_iClickedShip == -1 && pDoc->m_ShipHistory[race].GetSizeOfShipHistory() > 0)
	{
		m_iClickedShip = 0;
		m_iOldClickedShip = 0;
	}
	
	unsigned short j = 0;
	unsigned short k = 0;
	unsigned short numberLive = 0;
	unsigned short numberDead = 0;
	short counter = m_iClickedShip - 20 + m_iOldClickedShip;
	short oldClickedShip = m_iClickedShip;
	for (UINT i = 0; i < pDoc->m_ShipHistory[race].GetSizeOfShipHistory(); i++)
	{
		if (pDoc->m_ShipHistory[race].IsShipAlive(i))
		{
			numberLive++;
			if (counter > 0 && m_bShowAliveShips)
			{
				m_iClickedShip--;
				counter--;
				continue;
			}
			if (m_bShowAliveShips && j < 21) // Auf jede Seite gehen nur 21 Einträge, deshalb muss abgebrochen werden
			{
				// Wenn Schiff markiert ist, dann andere Schriftfarbe wählen
				if (j == m_iClickedShip)
				{
					pDC->SetTextColor(markColor);
					pDC->MoveTo(r.left+46,r.top+140+j*25); pDC->LineTo(r.right-45,r.top+140+j*25);
					pDC->MoveTo(r.left+46,r.top+140+j*25+25); pDC->LineTo(r.right-45,r.top+140+j*25+25);
				//	pDC->FillRect(CRect(r.left+46,r.top+141+j*25,r.right-45,r.top+140+j*25+25), new CBrush(RGB(140,140,250)));
				}
				else
					pDC->SetTextColor(oldColor);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strShipName, CRect(r.left+55,r.top+140+j*25,r.left+225,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strShipType, CRect(r.left+225,r.top+140+j*25,r.left+375,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strShipClass, CRect(r.left+375,r.top+140+j*25,r.left+495,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				s.Format("%d",pDoc->m_ShipHistory[race].GetShipHistory(i)->m_iBuildRound);
				pDC->DrawText(s, CRect(r.left+495,r.top+140+j*25,r.left+560,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strSectorName, CRect(r.left+560,r.top+140+j*25,r.left+675,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strCurrentSector, CRect(r.left+675,r.top+140+j*25,r.left+800,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				s.Format("%d",pDoc->m_ShipHistory[race].GetShipHistory(i)->m_iExperiance);
				pDC->DrawText(s, CRect(r.left+800,r.top+140+j*25,r.left+850,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strCurrentTask, CRect(r.left+850,r.top+140+j*25,r.right-50,r.top+165+j*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
			}
			j++;
		}
		else
		{
			numberDead++;
			if (counter > 0 && !m_bShowAliveShips)
			{
				m_iClickedShip--;
				counter--;
				continue;
			}
			if (!m_bShowAliveShips && k < 21) // Auf jede Seite gehen nur 22 Einträge, deshalb muss abgebrochen werden
			{				
				// Wenn Schiff markiert ist, dann andere Schriftfarbe wählen
				if (k == m_iClickedShip)
				{
					pDC->SetTextColor(markColor);
					pDC->MoveTo(r.left+46,r.top+140+k*25); pDC->LineTo(r.right-45,r.top+140+k*25);
					pDC->MoveTo(r.left+46,r.top+140+k*25+25); pDC->LineTo(r.right-45,r.top+140+k*25+25);
				}
				else
					pDC->SetTextColor(oldColor);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strShipName, CRect(r.left+55,r.top+140+k*25,r.left+225,r.top+165+k*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strShipType, CRect(r.left+225,r.top+140+k*25,r.left+375,r.top+165+k*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strShipClass, CRect(r.left+375,r.top+140+k*25,r.left+495,r.top+165+k*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				s.Format("%d",pDoc->m_ShipHistory[race].GetShipHistory(i)->m_iDestroyRound);
				pDC->DrawText(s, CRect(r.left+495,r.top+140+k*25,r.left+560,r.top+165+k*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strSectorName, CRect(r.left+560,r.top+140+k*25,r.left+675,r.top+165+k*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strKindOfDestroy, CRect(r.left+675,r.top+140+k*25,r.left+850,r.top+165+k*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
				pDC->DrawText(pDoc->m_ShipHistory[race].GetShipHistory(i)->m_strCurrentTask, CRect(r.left+850,r.top+140+k*25,r.right-50,r.top+165+k*25), DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS);
			}
			k++;
		}
	}
	m_iClickedShip = oldClickedShip;

	// Buttons am oberen Bildrand zeichnen, womit wir die Anzeige der jetzigen Schiffe und der verlorenen Schiffe verändern
	// können
	LoadFontForBigButton(pDC);
	// Buttons am unteren Bildrand zeichnen
	DrawButtons(pDC, &m_EmpireNewsButtons, m_iSubMenu);
	DrawButtons(pDC, &m_EmpireShipsFilterButtons, !m_bShowAliveShips);

	// Tabellenüberschriften zeichnen
	if (pDoc->GetPlayersRace() == ROMULAN)
		pDC->SetTextColor(RGB(4,75,35));
	else
		pDC->SetTextColor(RGB(200,200,200));
	pDC->DrawText(CResourceManager::GetString("NAME"),CRect(r.left+50,r.top+110,r.left+225,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("TYPE"),CRect(r.left+225,r.top+110,r.left+375,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("CLASS"),CRect(r.left+375,r.top+110,r.left+495,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SYSTEM"),CRect(r.left+560,r.top+110,r.left+675,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	if (m_bShowAliveShips)
	{
		pDC->DrawText(CResourceManager::GetString("CONSTRUCT"),CRect(r.left+495,r.top+110,r.left+560,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("CURRENT_PLACE"),CRect(r.left+675,r.top+110,r.left+790,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("EXPERIANCE_SHORT"),CRect(r.left+790,r.top+110,r.left+850,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("STATUS"),CRect(r.left+850,r.top+110,r.right-50,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		pDC->DrawText(CResourceManager::GetString("LOST"),CRect(r.left+495,r.top+110,r.left+560,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("EVENT"),CRect(r.left+675,r.top+110,r.left+790,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("STATUS"),CRect(r.left+850,r.top+110,r.right-50,r.top+140), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	pDC->SetTextColor(CFontLoader::GetFontColor(pDoc->GetPlayersRace(), 4));

	s.Format("%s: %d",CResourceManager::GetString("SHIPS_TOTAL"),numberLive);
	pDC->DrawText(s,CRect(280,70,580,110), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%s: %d",CResourceManager::GetString("SHIPS_LOST"),numberDead);
	pDC->DrawText(s,CRect(675,70,r.right-50,110), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	// "Übersicht aller Schiffe" oben in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("SHIP_OVERVIEW_MENUE"),CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}


// CEmpireMenuView diagnostics

#ifdef _DEBUG
void CEmpireMenuView::AssertValid() const
{
	CMainBaseView::AssertValid();
}

#ifndef _WIN32_WCE
void CEmpireMenuView::Dump(CDumpContext& dc) const
{
	CMainBaseView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEmpireMenuView message handlers

void CEmpireMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);
	// Wenn wir in der Nachrichten und Informationsansicht sind
	// Checken, ob ich auf einen Button geklickt habe um in ein anderes Untermenü zu gelangen
	int temp = m_iSubMenu;
	if (ButtonReactOnLeftClick(point, &m_EmpireNewsButtons, temp))
	{
		m_iSubMenu = temp;
		return;
	}
	
	// Wenn wir in der Nachtichtenasicht sind
	if (m_iSubMenu == 0)
	{			
		// Checken auf welchen Newsbutton ich geklickt habe, um nur Kategorien anzuzeigen
		int temp = m_iWhichNewsButtonIsPressed;
		if (ButtonReactOnLeftClick(point, &m_EmpireNewsFilterButtons, temp))
		{
			m_iWhichNewsButtonIsPressed = temp;
			return;
		}
		
		// Wenn wir auf eine News geklickt haben, diese Markieren
		BYTE race = pDoc->GetPlayersRace();
		CRect r;
		r.SetRect(0, 0, m_TotalSize.cx,m_TotalSize.cy);
		unsigned short j = 0;
		short counter = m_iClickedNews - 20 + m_iOldClickedNews;
		short add = 0;
		for (int i = 0; i < pDoc->m_Empire[race].GetMessages()->GetSize(); i++)
			if (m_iWhichNewsButtonIsPressed == pDoc->m_Empire[race].GetMessages()->GetAt(i).GetMessageType()
				|| m_iWhichNewsButtonIsPressed == NO_TYPE)
			{
				if (counter > 0)
				{
					add++;
					counter--;
					continue;
				}
				if (j < 21)
				{
					if (CRect(r.left+50,r.top+140+j*25,r.right-50,r.top+165+j*25).PtInRect(point))
					{
						m_iClickedNews = j + add;
						m_iOldClickedNews = 20-(j)%21;
						Invalidate(FALSE);
						break;
					}
				j++;
				}
			}
	}
	// Wenn wir in der Systemübersicht sind
	else if (m_iSubMenu == 1)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		// Haben wir auf einen Button geklickt, um zwischen Ressourcen- und normaler Ansicht zu wechseln
		int temp = m_bShowResources;
		if (ButtonReactOnLeftClick(point, &m_EmpireSystemFilterButtons, temp, FALSE))
		{
			m_bShowResources = temp;
			Invalidate(FALSE);
			return;
		}
		// Haben wir auf ein System in der Liste geklickt
		unsigned short j = 0;
		short counter = m_iClickedSystem - 20 + m_iOldClickedSystem;
		short add = 0;
		BYTE race = pDoc->GetPlayersRace();
		for (int i = 0; i < pDoc->GetEmpire(race)->GetSystemList()->GetSize(); i++)
		{
			if (counter > 0)
			{
				add++;
				counter--;
				continue;
			}
			if (j < 21)
			{
				if (CRect(r.left+50,r.top+140+j*25,r.right-50,r.top+165+j*25).PtInRect(point))
				{
					m_iClickedSystem = j + add;
					m_iOldClickedSystem = 20-(j)%21;
					Invalidate(FALSE);
					break;
				}
			j++;
			}
		}
	}
	// Wenn wir in der Schiffsübersicht sind
	else if (m_iSubMenu == 2)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		// Checken ob ich auf irgendeinen Ordering-Button geklickt habe
		if (CRect(r.left+50,r.top+115,r.left+225,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach dem Namen der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_shipname);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+225,r.top+115,r.left+375,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach dem Typ der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_shiptype);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+375,r.top+115,r.left+495,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach der Klasse der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_shipclass);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+495,r.top+115,r.left+560,r.top+140).PtInRect(point) && m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach der Baurunde der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_roundbuild);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+560,r.top+115,r.left+675,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach dem Sektornamen der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_sectorname);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+675,r.top+115,r.left+790,r.top+140).PtInRect(point) && m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach dem aktuellen Sektor der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_currentsector);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+790,r.top+115,r.left+850,r.top+140).PtInRect(point) && m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach der Erfahrung der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_exp);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+850,r.top+115,r.right-50,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach dem Auftrag der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_task);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+495,r.top+115,r.left+560,r.top+140).PtInRect(point) && !m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach der Zerstörungsrunde der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_rounddestroy);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+675,r.top+115,r.left+850,r.top+140).PtInRect(point) && !m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach der Zerstörungsart des Schiffes
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pDoc->m_ShipHistory[pDoc->GetPlayersRace()].GetShipHistoryArray()),CShipHistoryStruct::sort_by_destroytype);
			Invalidate(FALSE);
			return;
		}
		
		// Haben wir auf einen Button geklickt, um nur noch vorhandene oder mittlerweile verlorene Schiffe anzuzeigen
		int temp = m_bShowAliveShips;
		if (ButtonReactOnLeftClick(point, &m_EmpireShipsFilterButtons, temp, FALSE))
		{
			m_bShowAliveShips = temp;
			m_bShowAliveShips = !m_bShowAliveShips;
			m_iClickedShip = -1;
			Invalidate(FALSE);
			return;
		}
		// Haben wir auf ein Schiff in der Liste geklickt
		unsigned short j = 0;
		unsigned short k = 0;
		short counter = m_iClickedShip - 20 + m_iOldClickedShip;
		short add = 0;
		BYTE race = pDoc->GetPlayersRace();
		for (UINT i = 0; i < pDoc->m_ShipHistory[race].GetSizeOfShipHistory(); i++)
		{
			if (pDoc->m_ShipHistory[race].IsShipAlive(i))
			{
				if (counter > 0 && m_bShowAliveShips)
				{
					add++;
					counter--;
					continue;
				}
				if (m_bShowAliveShips && j < 21)
				{
					if (CRect(r.left+50,r.top+140+j*25,r.right-50,r.top+165+j*25).PtInRect(point))
					{
						m_iClickedShip = j + add;
						m_iOldClickedShip = 20-(j)%21;
						Invalidate(FALSE);
						break;
					}
				j++;
				}
			}
			else
			{
				if (counter > 0 && !m_bShowAliveShips)
				{
					add++;
					counter--;
					continue;
				}
				if (!m_bShowAliveShips && k < 21)
				{
					if (CRect(r.left+50,r.top+140+k*25,r.right-50,r.top+165+k*25).PtInRect(point))
					{
						m_iClickedShip = k + add;
						m_iOldClickedShip = 20-(k)%21;
						Invalidate(FALSE);
						break;						
					}
				k++;
				}
			}
		}
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

BOOL CEmpireMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// Wenn wir im Imperiummenü sind
	// Wenn wir in der Nachrichtenübersicht sind
	if (m_iSubMenu == 0)
	{
		BYTE race = pDoc->GetPlayersRace();
		int maxNews = 0;
		// nur Nachrichten anzeigen, dessen Typ wir auch gewählt haben
		for (int i = 0; i < pDoc->m_Empire[race].GetMessages()->GetSize(); i++)
			if (m_iWhichNewsButtonIsPressed == pDoc->m_Empire[race].GetMessages()->GetAt(i).GetMessageType() ||
				m_iWhichNewsButtonIsPressed == NO_TYPE)
				maxNews++;
		if (zDelta < 0)
		{
			if (maxNews > m_iClickedNews+1)
			{
				if (m_iOldClickedNews > 0)
					m_iOldClickedNews--;
				m_iClickedNews++;
				Invalidate();
			}
		}
		else if (zDelta > 0)
		{
			if (m_iClickedNews > 0)
			{
				if (m_iClickedNews > 20 && m_iOldClickedNews < 20)
					m_iOldClickedNews++;
				m_iClickedNews--;
				Invalidate();
			}
		}
	}
	// Wenn wir in der Imperiumssystemübersicht sind
	else if (m_iSubMenu == 1)
	{
		BYTE race = pDoc->GetPlayersRace();
		if (zDelta < 0)
		{
			if (pDoc->GetEmpire(race)->GetSystemList()->GetSize() > m_iClickedSystem+1)
			{
				if (m_iOldClickedSystem > 0)
					m_iOldClickedSystem--;
				m_iClickedSystem++;
				Invalidate();
			}
		}
		else if (zDelta > 0)
		{
			if (m_iClickedSystem > 0)
			{
				if (m_iClickedSystem > 20 && m_iOldClickedSystem < 20)
					m_iOldClickedSystem++;
				m_iClickedSystem--;
				Invalidate();
			}
		}
	}
	// Wenn wir in der Imperiumsschiffsübersicht sind
	else if (m_iSubMenu == 2)
	{
		BYTE race = pDoc->GetPlayersRace();
		if (zDelta < 0)
		{
			if ((int)pDoc->m_ShipHistory[race].GetNumberOfShips(m_bShowAliveShips) > m_iClickedShip+1)
			{
				if (m_iOldClickedShip > 0)
					m_iOldClickedShip--;
				m_iClickedShip++;
				Invalidate();
			}
		}
		else if (zDelta > 0)
		{
			if (m_iClickedShip > 0)
			{
				if (m_iClickedShip > 20 && m_iOldClickedShip < 20)
					m_iOldClickedShip++;
				m_iClickedShip--;
				Invalidate();
			}
		}
	}	

	return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

void CEmpireMenuView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);
	// Wenn wir uns in der Imperiumsübersichtansicht befinden	
	// Wenn wir in der Nachtichtenansicht sind
	if (m_iSubMenu == 0)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		BYTE race = pDoc->GetPlayersRace();
		int j = 0;
		short counter = m_iClickedNews - 20 + m_iOldClickedNews;
		short add = 0;
		for (int i = 0; i < pDoc->m_Empire[race].GetMessages()->GetSize(); i++)
			if (m_iWhichNewsButtonIsPressed == pDoc->m_Empire[race].GetMessages()->GetAt(i).GetMessageType() ||
					m_iWhichNewsButtonIsPressed == NO_TYPE)
			{
				if (counter > 0)
				{
					add++;
					counter--;
					continue;
				}
				if (j < 21)
				{
					if (CRect(r.left+100,r.top+140+j*25,r.right-100,r.top+165+j*25).PtInRect(point))
					{
						if (pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetMessageType() == ECONOMY || 
							pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetMessageType() == SOMETHING)
						{
							CPoint p = pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetKO();
							if (pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race)
							{
								pDoc->SetKO(p.x,p.y);
								pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CSystemMenuView), pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetFlag());
								pDoc->GetMainFrame()->SelectMainView(SYSTEM_VIEW, pDoc->GetPlayersRace());
								pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
								CGalaxyMenuView::SetMoveShip(FALSE);
								pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
							}
						}
						else if (pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetMessageType() == MILITARY)
						{
							if (pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetFlag() == FALSE)
							{
								CPoint p = pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetKO();
								if (pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == race)
								{
									pDoc->SetKO(p.x,p.y);
									pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CSystemMenuView), 0);
									pDoc->GetMainFrame()->SelectMainView(SYSTEM_VIEW, pDoc->GetPlayersRace());
									CGalaxyMenuView::SetMoveShip(FALSE);
									pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
									pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
								}
							}
						}
						else if (pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetMessageType() == RESEARCH)
						{
							pDoc->GetMainFrame()->SelectMainView(RESEARCH_VIEW, pDoc->GetPlayersRace());
							pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CResearchMenuView), pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetFlag());
							Invalidate();
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
						}
						else if (pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetMessageType() == SECURITY)
						{
							pDoc->GetMainFrame()->SelectMainView(INTEL_VIEW, pDoc->GetPlayersRace());
							pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CIntelMenuView), pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetFlag());
							Invalidate();
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));							
						}
						else if (pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetMessageType() == DIPLOMACY)
						{
							pDoc->GetMainFrame()->SelectMainView(DIPLOMACY_VIEW, pDoc->GetPlayersRace());
							// bei Angeboten an uns direkt in das Eingangmenü schalten
							pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CDiplomacyMenuView), pDoc->GetEmpire(race)->GetMessages()->GetAt(i).GetFlag());
							Invalidate();
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
						}
						break;
					}
				j++;
				}
			}
	}
	// Wenn wir in der Systemübersicht sind
	else if (m_iSubMenu == 1)
	{
		// Haben wir auf ein System in der Liste geklickt
		if (m_iClickedSystem != -1)
		{
			BYTE race = pDoc->GetPlayersRace();
			CPoint ko = pDoc->GetEmpire(race)->GetSystemList()->GetAt(m_iClickedSystem).ko;
			pDoc->SetKO(ko.x, ko.y);
			pDoc->GetMainFrame()->SelectMainView(2,pDoc->GetPlayersRace());
			CSystemMenuView::SetMarkedBuildListEntry(1);
			Invalidate(FALSE);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			return;
		}
	}	

	CMainBaseView::OnLButtonDblClk(nFlags, point);
}

void CEmpireMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);
	// Wenn wir in der Imperiumsansicht sind
	// Buttons am unteren Rand überprüfen
	ButtonReactOnMouseOver(point, &m_EmpireNewsButtons);
	// Sind wir in der Nachrichtenansicht
	if (m_iSubMenu == 0)
		ButtonReactOnMouseOver(point, &m_EmpireNewsFilterButtons);
	// sind wir in der Schiffsübersicht
	else if (m_iSubMenu == 2)
		ButtonReactOnMouseOver(point, &m_EmpireShipsFilterButtons);	

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CEmpireMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// Wenn wir in der Nachrichten und Informationsansicht sind
	// Wenn wir in der Nachtichtenasicht sind
	if (m_iSubMenu == 0)
	{
		// Wenn wir auf eine News ausgewählt haben, können wir diese durch Betätigen der Entf-Taste löschen
		if (m_iClickedNews != -1 && nChar == VK_DELETE && m_iWhichNewsButtonIsPressed == NO_TYPE)
		{
			pDoc->m_Empire[pDoc->GetPlayersRace()].GetMessages()->RemoveAt(m_iClickedNews);
			if (m_iClickedNews == pDoc->m_Empire[pDoc->GetPlayersRace()].GetMessages()->GetSize())
				m_iClickedNews--;
			Invalidate(FALSE);
			return;
		}
		
		BYTE race = pDoc->GetPlayersRace();
		int maxNews = 0;
		// nur Nachrichten anzeigen, dessen Typ wir auch gewählt haben
		for (int i = 0; i < pDoc->m_Empire[race].GetMessages()->GetSize(); i++)
			if (m_iWhichNewsButtonIsPressed == pDoc->m_Empire[race].GetMessages()->GetAt(i).GetMessageType() ||
				m_iWhichNewsButtonIsPressed == NO_TYPE)
				maxNews++;
		if (nChar == VK_DOWN)
		{
			if (maxNews > m_iClickedNews+1)
			{
				if (m_iOldClickedNews > 0)
					m_iOldClickedNews--;
				m_iClickedNews++;
				Invalidate();
			}
		}
		else if (nChar == VK_UP)
		{
			if (m_iClickedNews > 0)
			{
				if (m_iClickedNews > 20 && m_iOldClickedNews < 20)
					m_iOldClickedNews++;
				m_iClickedNews--;
				Invalidate();
			}
		}
	}
	
	// Wenn wir in der Imperiumssystemübersicht sind
	if (m_iSubMenu == 1)
	{
		BYTE race = pDoc->GetPlayersRace();
		if (nChar == VK_DOWN)
		{
			if (pDoc->GetEmpire(race)->GetSystemList()->GetSize() > m_iClickedSystem+1)
			{
				if (m_iOldClickedSystem > 0)
					m_iOldClickedSystem--;
				m_iClickedSystem++;
				Invalidate();
			}
		}
		else if (nChar == VK_UP)
		{
			if (m_iClickedSystem > 0)
			{
				if (m_iClickedSystem > 20 && m_iOldClickedSystem < 20)
					m_iOldClickedSystem++;
				m_iClickedSystem--;
				Invalidate();
			}
		}
		// wird die Autobaufunktion aktiviert oder deaktiviert
		else if (nChar == 'A')
		{
			if (m_iClickedSystem != -1)
			{
				CPoint ko = pDoc->GetEmpire(race)->GetSystemList()->GetAt(m_iClickedSystem).ko;
				BOOLEAN autoBuild = pDoc->m_System[ko.x][ko.y].GetAutoBuild();
				pDoc->m_System[ko.x][ko.y].SetAutoBuild(!autoBuild);
			//	m_iClickedOn = 1;
				Invalidate(FALSE);
				return;
			}
		}
	}
	// Wenn wir in der Imperiumsschiffsübersicht sind
	else if (m_iSubMenu == 2)
	{
		BYTE race = pDoc->GetPlayersRace();
		if (nChar == VK_DOWN)
		{
			if ((int)pDoc->m_ShipHistory[race].GetNumberOfShips(m_bShowAliveShips) > m_iClickedShip+1)
			{
				if (m_iOldClickedShip > 0)
					m_iOldClickedShip--;
				m_iClickedShip++;
				Invalidate();
			}
		}
		else if (nChar == VK_UP)
		{
			if (m_iClickedShip > 0)
			{
				if (m_iClickedShip > 20 && m_iOldClickedShip < 20)
					m_iOldClickedShip++;
				m_iClickedShip--;
				Invalidate();
			}
		}
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEmpireMenuView::CreateButtons()
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
			// Buttons in der Imperiumansicht
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(57,70), CSize(160,40), CResourceManager::GetString("BTN_ALL"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(217,70), CSize(160,40), CResourceManager::GetString("BTN_ECONOMY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(377,70), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(537,70), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(697,70), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(857,70), CSize(160,40), CResourceManager::GetString("BTN_MILITARY"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_EVENTS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SYSTEMS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SHIPS"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(60,70), CSize(160,40), CResourceManager::GetString("BTN_NORMAL"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(220,70), CSize(160,40), CResourceManager::GetString("BTN_RESOURCES"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(96,70), CSize(160,40), CResourceManager::GetString("BTN_CURRENTS"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(495,70), CSize(160,40), CResourceManager::GetString("BTN_LOST"),  fileN, fileI, fileA));
			break;
		}
	case FERENGI:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttona.jpg";
			// Buttons in der Imperiumansicht
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(57,70), CSize(160,40), CResourceManager::GetString("BTN_ALL"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(217,70), CSize(160,40), CResourceManager::GetString("BTN_ECONOMY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(377,70), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(537,70), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(697,70), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(857,70), CSize(160,40), CResourceManager::GetString("BTN_MILITARY"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_EVENTS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SYSTEMS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SHIPS"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(60,70), CSize(160,40), CResourceManager::GetString("BTN_NORMAL"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(855,70), CSize(160,40), CResourceManager::GetString("BTN_RESOURCES"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(96,70), CSize(160,40), CResourceManager::GetString("BTN_CURRENTS"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(495,70), CSize(160,40), CResourceManager::GetString("BTN_LOST"),  fileN, fileI, fileA));
			break;
		}
	case KLINGON:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2i.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2a.jpg";
			// Buttons in der Imperiumansicht
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(57,73), CSize(160,40), CResourceManager::GetString("BTN_ALL"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(217,73), CSize(160,40), CResourceManager::GetString("BTN_ECONOMY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(377,73), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(537,73), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(697,73), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(857,73), CSize(160,40), CResourceManager::GetString("BTN_MILITARY"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(377,70), CSize(160,40), CResourceManager::GetString("BTN_NORMAL"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(537,70), CSize(160,40), CResourceManager::GetString("BTN_RESOURCES"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(96,73), CSize(160,40), CResourceManager::GetString("BTN_CURRENTS"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(495,73), CSize(160,40), CResourceManager::GetString("BTN_LOST"),  fileN, fileI, fileA));
			fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button.jpg";
			fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "buttoni.jpg";
			fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "buttona.jpg";
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_EVENTS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SYSTEMS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SHIPS"),  fileN, fileI, fileA));
			break;
		}
	case ROMULAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttona.jpg";
			// Buttons in der Imperiumansicht
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(57,70), CSize(160,40), CResourceManager::GetString("BTN_ALL"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(217,70), CSize(160,40), CResourceManager::GetString("BTN_ECONOMY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(377,70), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(537,70), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(697,70), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(857,70), CSize(160,40), CResourceManager::GetString("BTN_MILITARY"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_EVENTS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SYSTEMS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SHIPS"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(90,70), CSize(160,40), CResourceManager::GetString("BTN_NORMAL"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(825,70), CSize(160,40), CResourceManager::GetString("BTN_RESOURCES"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(96,70), CSize(160,40), CResourceManager::GetString("BTN_CURRENTS"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(495,70), CSize(160,40), CResourceManager::GetString("BTN_LOST"),  fileN, fileI, fileA));
			break;
		}
	case CARDASSIAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttona.jpg";
			// Buttons in der Imperiumansicht
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(57,70), CSize(160,40), CResourceManager::GetString("BTN_ALL"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(217,70), CSize(160,40), CResourceManager::GetString("BTN_ECONOMY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(377,70), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(537,70), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(697,70), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(857,70), CSize(160,40), CResourceManager::GetString("BTN_MILITARY"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_EVENTS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SYSTEMS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SHIPS"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(90,70), CSize(160,40), CResourceManager::GetString("BTN_NORMAL"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(825,70), CSize(160,40), CResourceManager::GetString("BTN_RESOURCES"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(96,70), CSize(160,40), CResourceManager::GetString("BTN_CURRENTS"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(495,70), CSize(160,40), CResourceManager::GetString("BTN_LOST"),  fileN, fileI, fileA));
			break;
		}
	case DOMINION:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttona.jpg";
			// Buttons in der Imperiumansicht
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(57,70), CSize(160,40), CResourceManager::GetString("BTN_ALL"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(217,70), CSize(160,40), CResourceManager::GetString("BTN_ECONOMY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(377,70), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(537,70), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(697,70), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"),  fileN, fileI, fileA));
			m_EmpireNewsFilterButtons.Add(new CMyButton(CPoint(857,70), CSize(160,40), CResourceManager::GetString("BTN_MILITARY"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_EVENTS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SYSTEMS"),  fileN, fileI, fileA));
			m_EmpireNewsButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SHIPS"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(90,70), CSize(160,40), CResourceManager::GetString("BTN_NORMAL"),  fileN, fileI, fileA));
			m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(825,70), CSize(160,40), CResourceManager::GetString("BTN_RESOURCES"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(96,70), CSize(160,40), CResourceManager::GetString("BTN_CURRENTS"),  fileN, fileI, fileA));
			m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(495,70), CSize(160,40), CResourceManager::GetString("BTN_LOST"),  fileN, fileI, fileA));
			break;
		}
	}
}