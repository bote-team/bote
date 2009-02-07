// SystemMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "GalaxyMenuView.h"
#include "MenuChooseView.h"
#include "SystemMenuView.h"


short CSystemMenuView::m_iClickedOn = 1;
BYTE CSystemMenuView::m_byResourceRouteRes = TITAN;

// CSystemMenuView
IMPLEMENT_DYNCREATE(CSystemMenuView, CMainBaseView)

CSystemMenuView::CSystemMenuView()
{

}

CSystemMenuView::~CSystemMenuView()
{
	for (int i = 0; i < m_BuildMenueMainButtons.GetSize(); i++)
	{
		delete m_BuildMenueMainButtons[i];
		m_BuildMenueMainButtons[i] = 0;
	}
	m_BuildMenueMainButtons.RemoveAll();
}

BEGIN_MESSAGE_MAP(CSystemMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CSystemMenuView::OnNewRound()
{	
	m_byStartList = 0;
	m_iBOPage = 0;
	m_iELPage = 0;
	m_iSTPage = 0;
	m_iClickedOn = 1;
	m_bClickedOnBuyButton = FALSE;
	m_bClickedOnDeleteButton = FALSE;
	m_bClickedOneBuilding = TRUE;
	m_byResourceRouteRes = TITAN;
}
// CSystemMenuView drawing

void CSystemMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	SetFocus();
	// TODO: add draw code here
	CMemDC pDC(dc);
	pDC->SetBkMode(TRANSPARENT);
	if (pDoc->m_pIniLoader->GetValue("SMOOTHSCALING"))
		pDC->SetStretchBltMode(HALFTONE);
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	LoadRaceFont(pDC);
	// ***************************** DIE SYSTEMANSICHT ZEICHNEN **********************************
	if (m_bySubMenu == 0)
		DrawBuildMenue(pDC,r);
	else if (m_bySubMenu == 1 || m_bySubMenu == 12)
		DrawWorkersMenue(pDC,r);
	else if (m_bySubMenu == 2)
		DrawEnergyMenue(pDC,r);
	else if (m_bySubMenu == 3)
		DrawBuildingsOverviewMenue(pDC,r);
	else if (m_bySubMenu == 4)
		DrawSystemTradeMenue(pDC,r);
	DrawButtonsUnderSystemView(pDC);
	// ************** DIE SYSTEMANSICHT ZEICHNEN ist hier zu Ende **************		
}


// CSystemMenuView diagnostics

#ifdef _DEBUG
void CSystemMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CSystemMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSystemMenuView message handlers

void CSystemMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	// Alle Buttons in der View erstellen
	CreateButtons();
	
	// alle Hintergrundgrafiken laden
	bg_buildmenu.DeleteObject();
	bg_workmenu.DeleteObject();
	bg_energymenu.DeleteObject();
	bg_overviewmenu.DeleteObject();
	bg_systrademenu.DeleteObject();
	
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
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"buildmenu.jpg");
	bg_buildmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"workmenu.jpg");
	bg_workmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"energymenu.jpg");
	bg_energymenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"overviewmenu.jpg");
	bg_overviewmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"systrademenu.jpg");
	bg_systrademenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Destroy();	
	
	// Baumenürechtecke
	m_iClickedOn = 1;
	BuildingDescription.SetRect(30,410,290,620);
	BuildingInfo.SetRect(340,560,700,650);
	AssemblyListRect.SetRect(740,400,1030,620);
	m_bClickedOneBuilding = TRUE;
	m_bySubMenu = 0;
	m_bClickedOnBuyButton = FALSE;
	m_bClickedOnDeleteButton = FALSE;
	m_bClickedOnBuildingInfoButton = TRUE;
	m_bClickedOnBuildingDescriptionButton = FALSE;
	m_iWhichSubMenu = 0;

	// Die Koodinaten der Rechtecke für die ganzen Buttons
	CRect r;
	GetClientRect(&r);
	// Kleine Buttons in der Baumenüansicht
	BuildingListButton.SetRect(r.left+325,r.top+510,r.left+445,r.top+540);
	ShipyardListButton.SetRect(r.left+460,r.top+510,r.left+580,r.top+540);
	TroopListButton.SetRect(r.left+595,r.top+510,r.left+715,r.top+540);
	// Kleine Buttons unter der Bauliste/Assemblylist
	BuyButton.SetRect(r.left+750,r.top+625,r.left+870,r.top+655);
	DeleteButton.SetRect(r.left+900,r.top+625,r.left+1020,r.top+655);
	// Kleine Buttons unter der Gebäudebeschreibungs/Informations-Ansicht
	BuildingInfoButton.SetRect(r.left+30,r.top+625,r.left+150,r.top+655);
	BuildingDescriptionButton.SetRect(r.left+165,r.top+625,r.left+285,r.top+655);
	// Kleiner Umschaltbutton in der Arbeitermenüansicht
	ChangeWorkersButton.SetRect(r.left+542,r.top+645,r.left+662,r.top+675);
	for (int i = 0; i < 5; i++)
	{
		PlusButton[i].SetRect(r.left+630,r.top+115+i*95,r.left+660,r.top+150+i*95);
		MinusButton[i].SetRect(r.left+170,r.top+115+i*95,r.left+200,r.top+150+i*95);
	}
	// Handelsrouten und Globales Lager Ansicht
	m_iGlobalStoreageQuantity = 1;	
}

void CSystemMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CSystemMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

// Funktion zum Zeichnen der Baumenüansicht
void CSystemMenuView::DrawBuildMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CString s;
	CRect rect;
	CPoint p = pDoc->GetKO();
	COLORREF oldColor = pDC->GetTextColor();
	CString strBuildingInfo = "";
	CString strDescription = "";
	
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_buildmenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);

	DrawInfoRightInSystemView(pDC, theClientRect);
	CPen mark;
	// Auftrag links oben im System zeichnen
	if (pDoc->GetPlayersRace() == HUMAN)
	{
		pDC->SetTextColor(RGB(0,0,0));
		mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
		// Anzeige der Moral und der Runden über der Bauliste
		s.Format("%s: %i",CResourceManager::GetString("MORAL"), pDoc->GetSystem(p.x,p.y).GetMoral());
		pDC->DrawText(s,CRect(360,105,695,130),DT_CENTER);
		pDC->DrawText(CResourceManager::GetString("ROUNDS"),CRect(360,105,695,130),DT_RIGHT);
		pDC->DrawText(CResourceManager::GetString("JOB"),CRect(370,105,440,130),DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	else if (pDoc->GetPlayersRace() == FERENGI)
	{
		pDC->SetTextColor(RGB(0,50,0));
		mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
		// Anzeige der Moral und der Runden über der Bauliste
		s.Format("%s: %i",CResourceManager::GetString("MORAL"), pDoc->GetSystem(p.x,p.y).GetMoral());
		pDC->DrawText(s,CRect(360,105,695,130),DT_CENTER);
		pDC->DrawText(CResourceManager::GetString("ROUNDS"),CRect(360,105,695,130),DT_RIGHT);
		pDC->DrawText(CResourceManager::GetString("JOB"),CRect(370,105,440,130),DT_CENTER | DT_TOP | DT_SINGLELINE);		
		// Erwerbsregel zeichnen
		// pDC->DrawText(m_strFerengiRules, CRect(35,235,285,340), DT_CENTER | DT_WORDBREAK);
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		pDC->SetTextColor(RGB(180,180,180));
		mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
		// Anzeige der Moral und der Runden über der Bauliste
		s.Format("%s: %i",CResourceManager::GetString("MORAL"), pDoc->GetSystem(p.x,p.y).GetMoral());
		pDC->DrawText(s,CRect(360,105,695,130),DT_CENTER);
		pDC->DrawText(CResourceManager::GetString("ROUNDS"),CRect(360,105,695,130),DT_RIGHT);
		pDC->DrawText(CResourceManager::GetString("JOB"),CRect(370,105,440,130),DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	else if (pDoc->GetPlayersRace() == ROMULAN)
	{
		mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
		// Anzeige der Moral und der Runden über der Bauliste
		s.Format("%s: %i",CResourceManager::GetString("MORAL"), pDoc->GetSystem(p.x,p.y).GetMoral());
		pDC->DrawText(s,CRect(360,75,695,125),DT_CENTER);
		pDC->DrawText(CResourceManager::GetString("ROUNDS"),CRect(360,75,690,100),DT_RIGHT);
		pDC->DrawText(CResourceManager::GetString("JOB"),CRect(370,75,440,100),DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	else if (pDoc->GetPlayersRace() == CARDASSIAN)
	{
		mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
		// Anzeige der Moral und der Runden über der Bauliste
		s.Format("%s: %i",CResourceManager::GetString("MORAL"), pDoc->GetSystem(p.x,p.y).GetMoral());
		pDC->DrawText(s,CRect(360,105,695,130),DT_CENTER);
		pDC->DrawText(CResourceManager::GetString("ROUNDS"),CRect(360,105,695,130),DT_RIGHT);
		pDC->DrawText(CResourceManager::GetString("JOB"),CRect(370,105,440,130),DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	else
	{
		mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
		// Anzeige der Moral und der Runden über der Bauliste
		s.Format("%s: %i",CResourceManager::GetString("MORAL"), pDoc->GetSystem(p.x,p.y).GetMoral());
		pDC->DrawText(s,CRect(360,105,695,130),DT_CENTER);
		pDC->DrawText(CResourceManager::GetString("ROUNDS"),CRect(360,105,695,130),DT_RIGHT);
		pDC->DrawText(CResourceManager::GetString("JOB"),CRect(370,105,440,130),DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	// Farbe für die Markierungen auswählen
	pDC->SelectObject(&mark);
	// Die Struktur BuildList erstmal löschen, alle werte auf 0
	for (int i = 0; i < 50; i++)
	{
		BuildList[i].rect.SetRect(0,0,0,0);
		BuildList[i].runningNumber = 0;
	}
	// Wenn man keine Schiffe zur Auswahl hat oder keine Truppen bauen kann, dann wird wieder auf das normale
	// Gebäudebaumenü umgeschaltet
	if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() == 0)
		m_iWhichSubMenu = 0;
	else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() == 0)
		m_iWhichSubMenu = 0;
		
	// hier Anzeige der baubaren Gebäude und Upgrades
	int y = 150;
	int j;
	int RoundToBuild = 0;
	pDC->SetTextColor(oldColor);
	CBrush nb2(RGB(20,20,20));
	pDC->SelectObject(nb2);
	// Variable die mir sagt worauf ich geklickt habe
	USHORT clickedOn = m_iClickedOn;
	USHORT counter = 1;	// Einträge in der Liste, akt. auf 25 begrenzt
	// Ab hier die Projekte, die ausgewählt werden können
	m_byEndList = m_byStartList + NOEIBL;	// auf maximal Zwölf Einträge in der Bauliste begrenzt
	if (m_iWhichSubMenu == 0)				// Sind wir im Gebäude/Update Untermenü
	{
		// Zuerst werden die Upgrades angezeigt
		for (int i = 0; i < pDoc->m_System[p.x][p.y].GetBuildableUpdates()->GetSize(); i++)
		{
			j = counter;
			// Wenn wir ein Update machen, so ist die RunningNumber des Gebäudes negativ!!!
			if (i < pDoc->m_System[p.x][p.y].GetBuildableUpdates()->GetSize())
			{
				if (counter > m_byStartList)
				{
					pDC->SetTextColor(oldColor);
					s = CResourceManager::GetString("UPGRADING", FALSE, pDoc->GetBuildingName(pDoc->m_System[p.x][p.y].GetBuildableUpdates()->GetAt(i)));
					y += 25;
					// Markierung zeichen
					if (counter == clickedOn)
					{
						// Markierung worauf wir geklickt haben
						pDC->MoveTo(319,y-27); pDC->LineTo(722,y-27);
						pDC->MoveTo(319,y+2); pDC->LineTo(722,y+2);
						clickedOn = 0;								// Markierung löschen
						// Farbe der Schrift wählen, wenn wir den Eintrag markiert haben
						if (pDoc->GetPlayersRace() == CARDASSIAN)
							pDC->SetTextColor(RGB(255,128,0));
						else
							pDC->SetTextColor(RGB(220,220,220));
					}
					// Eintrag zeichnen
					BuildList[counter].rect.SetRect(theClientRect.left+380,y-25,670,y);
					pDC->DrawText(s,BuildList[counter].rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
					BuildList[counter].runningNumber = pDoc->m_System[p.x][p.y].GetBuildableUpdates()->GetAt(i)*(-1);
					j++;
					// Hier Berechnung der noch verbleibenden Runden, bis das Projekt fertig wird
					int RunningNumber = (BuildList[counter].runningNumber)*(-1);
					pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), BuildList[counter].runningNumber,pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo());
					// divide by zero check
					if (pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() > 0)
					{
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryForBuild())/((float)pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd()
							* (100+pDoc->GetSystem(p.x,p.y).GetProduction()->GetUpdateBuildSpeed())/100));
						s.Format("%i",RoundToBuild);
						pDC->DrawText(s,CRect(theClientRect.left+580,y-25,685,y), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
					}
				}
				else
					j++;
				counter++;
			}
			if (counter > m_byEndList)
				break;
		}
		// Dann werden die baubaren Gebäude angezeigt
		for (int i = 0; i < pDoc->m_System[p.x][p.y].GetBuildableBuildings()->GetSize(); i++)
		{
			j = counter;
			if (i < pDoc->m_System[p.x][p.y].GetBuildableBuildings()->GetSize())
			{
				if (counter > m_byStartList)
				{
					pDC->SetTextColor(oldColor);
					s.Format(pDoc->GetBuildingName(pDoc->m_System[p.x][p.y].GetBuildableBuildings()->GetAt(i)));
					y += 25;
					// Markierung zeichen
					if (j == clickedOn)
					{
						// Markierung worauf wir geklickt haben
						pDC->MoveTo(319,y-27); pDC->LineTo(722,y-27);
						pDC->MoveTo(319,y+2); pDC->LineTo(722,y+2);
						clickedOn = 0;								// Markierung löschen
						// Farbe der Schrift wählen, wenn wir den Eintrag markiert haben
						if (pDoc->GetPlayersRace() == CARDASSIAN)
							pDC->SetTextColor(RGB(255,128,0));
						else
							pDC->SetTextColor(RGB(220,220,220));
					}
					// Eintrag zeichnen
					BuildList[j].rect.SetRect(theClientRect.left+380,y-25,670,y);
					pDC->DrawText(s,BuildList[j].rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE  | DT_WORD_ELLIPSIS);
					BuildList[j].runningNumber = pDoc->m_System[p.x][p.y].GetBuildableBuildings()->GetAt(i);
					// Hier Berechnung der noch verbleibenden Runden, bis das Projekt fertig wird (nicht bei NeverReady-Aufträgen)
					int RunningNumber = (BuildList[j].runningNumber);
					if (!pDoc->GetBuildingInfo(RunningNumber).GetNeverReady())
					{
						pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), BuildList[j].runningNumber,pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo());
						// divide by zero check
						if (pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() > 0)
						{
							RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryForBuild())/((float)pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd()
								* (100+pDoc->GetSystem(p.x,p.y).GetProduction()->GetBuildingBuildSpeed())/100));
							s.Format("%i",RoundToBuild);
							pDC->DrawText(s,CRect(theClientRect.left+580,y-25,685,y), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
						}
					}
				}
				counter++;
			}
			if (counter > m_byEndList)
				break;
		}
	}
	else if (m_iWhichSubMenu == 1)		// Sind wir im Werftuntermenü?
	{
		for (int i = 0; i < pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize(); i++)
		{
			j = counter;
			if (i < pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() && counter > m_byStartList)
			{
				pDC->SetTextColor(oldColor);
				//s.Format("%s der %s-Klasse",pDoc->m_ShipInfoArray.GetAt(pDoc->m_System[p.x][p.y].GetBuildableShips()->GetAt(i)-10000).GetShipTypeAsString()
				//	,pDoc->m_ShipInfoArray.GetAt(pDoc->m_System[p.x][p.y].GetBuildableShips()->GetAt(i)-10000).GetShipClass());
				s.Format("%s-%s",pDoc->m_ShipInfoArray.GetAt(pDoc->m_System[p.x][p.y].GetBuildableShips()->GetAt(i)-10000).GetShipClass(),
					CResourceManager::GetString("CLASS"));
				y += 25;
				// Markierung zeichen
				if (j == clickedOn)
				{
					// Markierung worauf wir geklickt haben
					pDC->MoveTo(319,y-27); pDC->LineTo(722,y-27);
					pDC->MoveTo(319,y+2); pDC->LineTo(722,y+2);
					clickedOn = 0;								// Markierung löschen
					// Farbe der Schrift wählen, wenn wir den Eintrag markiert haben
					if (pDoc->GetPlayersRace() == CARDASSIAN)
						pDC->SetTextColor(RGB(255,128,0));
					else
						pDC->SetTextColor(RGB(220,220,220));
				}
				// Eintrag zeichnen
				BuildList[j].rect.SetRect(theClientRect.left+380,y-25,670,y);
				pDC->DrawText(s,BuildList[j].rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
				BuildList[j].runningNumber = pDoc->m_System[p.x][p.y].GetBuildableShips()->GetAt(i);
				// Hier Berechnung der noch verbleibenden Runden, bis das Projekt fertig wird
				int RunningNumber = (BuildList[j].runningNumber);
				pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAssemblyList()->CalculateNeededRessources(0,&pDoc->m_ShipInfoArray.GetAt(pDoc->m_System[p.x][p.y].GetBuildableShips()->GetAt(i)-10000),0, pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), BuildList[j].runningNumber,pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo());
				// divide by zero check
				if ((pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency()) > 0)
					{RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryForBuild())
						/((float)pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency() / 100
							* (100+pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipBuildSpeed())/100));
					s.Format("%i",RoundToBuild);
					pDC->DrawText(s,CRect(theClientRect.left+580,y-25,685,y), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				}
			}
			counter++;
			if (counter > m_byEndList)
				break;
		}
	}
	else if (m_iWhichSubMenu == 2)		// Sind wir im Kasernenuntermenü?
	{
		for (int i = 0; i < pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize(); i++)
		{
			j = counter;
			if (i < pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() && counter > m_byStartList)
			{
				pDC->SetTextColor(oldColor);
				s.Format("%s",pDoc->m_TroopInfo.GetAt(pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetAt(i)).GetName());
				y += 25;
				// Markierung zeichen
				if (j == clickedOn)
				{
					// Markierung worauf wir geklickt haben
					pDC->MoveTo(319,y-27); pDC->LineTo(722,y-27);
					pDC->MoveTo(319,y+2); pDC->LineTo(722,y+2);
					clickedOn = 0;								// Markierung löschen
					// Farbe der Schrift wählen, wenn wir den Eintrag markiert haben
					if (pDoc->GetPlayersRace() == CARDASSIAN)
						pDC->SetTextColor(RGB(255,128,0));
					else
						pDC->SetTextColor(RGB(220,220,220));
				}
				// Eintrag zeichnen
				BuildList[j].rect.SetRect(theClientRect.left+380,y-25,670,y);
				pDC->DrawText(s,BuildList[j].rect,DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
				BuildList[j].runningNumber = pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetAt(i) + 20000;
				// Hier Berechnung der noch verbleibenden Runden, bis das Projekt fertig wird
				int RunningNumber = (BuildList[j].runningNumber);
				pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAssemblyList()->CalculateNeededRessources(0,0,&pDoc->m_TroopInfo.GetAt(pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetAt(i)), pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), BuildList[j].runningNumber,pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo());
				// divide by zero check
				if ((pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency()) > 0)
					{RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryForBuild())
						/((float)pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency() / 100
							* (100+pDoc->GetSystem(p.x,p.y).GetProduction()->GetTroopBuildSpeed())/100));
					s.Format("%i",RoundToBuild);
					pDC->DrawText(s,CRect(theClientRect.left+580,y-25,685,y), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
				}
			}
			counter++;
			if (counter > m_byEndList)
				break;
		}
	}
	pDC->SetTextColor(oldColor);
	// Hier die Einträge in der Bauliste
	this->DrawBuildList(pDC, theClientRect);

	// Arbeiterzuweisung unter dem Bild der Rasse zeichnen
	// kleine Bilder von den Rohstoffen zeichnen
	CBitmap* graphic = NULL;
	HGDIOBJ oldGraphic;
	CPoint px[IRIDIUM_WORKER+1] = {CPoint(0,0)};
	if (pDoc->GetPlayersRace() == KLINGON)
	{
		px[FOOD_WORKER].SetPoint(50,225); px[INDUSTRY_WORKER].SetPoint(125,225); px[ENERGY_WORKER].SetPoint(200,225);
		px[SECURITY_WORKER].SetPoint(50,250); px[RESEARCH_WORKER].SetPoint(125,250); px[TITAN_WORKER].SetPoint(200,250);
		px[DEUTERIUM_WORKER].SetPoint(50,275); px[DURANIUM_WORKER].SetPoint(125,275); px[CRYSTAL_WORKER].SetPoint(200,275);
		px[IRIDIUM_WORKER].SetPoint(125,300); 
	}
	else if (pDoc->GetPlayersRace() == ROMULAN)
	{
		px[FOOD_WORKER].SetPoint(145,15); px[INDUSTRY_WORKER].SetPoint(145,40); px[ENERGY_WORKER].SetPoint(145,65);
		px[SECURITY_WORKER].SetPoint(145,90); px[RESEARCH_WORKER].SetPoint(145,115);
		px[TITAN_WORKER].SetPoint(220,15); px[DEUTERIUM_WORKER].SetPoint(220,40); px[DURANIUM_WORKER].SetPoint(220,65);
		px[CRYSTAL_WORKER].SetPoint(220,90); px[IRIDIUM_WORKER].SetPoint(220,115); 
	}
	else
	{
		px[FOOD_WORKER].SetPoint(80,230); px[INDUSTRY_WORKER].SetPoint(80,255); px[ENERGY_WORKER].SetPoint(80,280);
		px[SECURITY_WORKER].SetPoint(80,305); px[RESEARCH_WORKER].SetPoint(80,330);
		px[TITAN_WORKER].SetPoint(185,230); px[DEUTERIUM_WORKER].SetPoint(185,255); px[DURANIUM_WORKER].SetPoint(185,280);
		px[CRYSTAL_WORKER].SetPoint(185,305); px[IRIDIUM_WORKER].SetPoint(185,330); 
	}
	for (int i = FOOD_WORKER; i <= IRIDIUM_WORKER; i++)
	{
		switch (i)
		{
		case FOOD_WORKER:		graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\foodSmall.png"); break;
		case INDUSTRY_WORKER:	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\industrySmall.png"); break;
		case ENERGY_WORKER:		graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\energySmall.png"); break;
		case SECURITY_WORKER:	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\securitySmall.png"); break;
		case RESEARCH_WORKER:	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\researchSmall.png"); break;
		case TITAN_WORKER:		graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\titanSmall.png"); break;
		case DEUTERIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\deuteriumSmall.png"); break;
		case DURANIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\duraniumSmall.png"); break;
		case CRYSTAL_WORKER:	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\crystalSmall.png"); break;
		case IRIDIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\iridiumSmall.png"); break;
		}
		if (graphic)
		{
			oldGraphic = mdc.SelectObject(*graphic);
			pDC->BitBlt(px[i].x,px[i].y,20,16,&mdc,0,0,SRCCOPY);
			mdc.SelectObject(oldGraphic);
			graphic = NULL;
		}
		s.Format("%d/%d", pDoc->m_System[p.x][p.y].GetWorker(i), pDoc->m_System[p.x][p.y].GetNumberOfWorkbuildings(i, 0));
		pDC->DrawText(s, CRect(30 + px[i].x,px[i].y,100 + px[i].x,16 + px[i].y), DT_LEFT | DT_VCENTER | DT_SINGLELINE);		
	}
	
	// Hier die Anzeige der nötigen Rohstoffe und Industrie zum Bau des Gebäudes und dessen Beschreibung
	// auch das Bild zum aktuellen Projekt wird angezeigt	
	if (m_bClickedOneBuilding == TRUE)
	{
		// Berechnung der nötigen Industrie und Rohstoffe
		int RunningNumber = 0;
		int i = m_iClickedOn;
		if (BuildList[i].runningNumber < 0)
			RunningNumber = (BuildList[i].runningNumber)*(-1);
		else 
			RunningNumber = BuildList[i].runningNumber;

		if (m_bClickedOnBuyButton == FALSE && m_bClickedOnDeleteButton == FALSE && RunningNumber != 0)
		{
			// also ein Gebäude oder Gebäudeupdate
			if (BuildList[i].runningNumber < 10000)
				pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), BuildList[i].runningNumber,pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo());
			// also ein Schiff
			else if (BuildList[i].runningNumber < 20000 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)
				pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAssemblyList()->CalculateNeededRessources(0,&pDoc->m_ShipInfoArray.GetAt(BuildList[i].runningNumber-10000),0, pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), BuildList[j].runningNumber,pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo());
			// also eine Truppe
			else if (pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)
				pDoc->m_System[p.x][p.y].GetAssemblyList()->CalculateNeededRessources(0,0,&pDoc->m_TroopInfo.GetAt(BuildList[i].runningNumber-20000), pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), BuildList[j].runningNumber,pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo());

			// Anzeige der ganzen Werte				
			CRect infoRect;
			//pDC->Rectangle(BuildingInfo);
			infoRect.SetRect(theClientRect.left+360,562,700,595);
			s = CResourceManager::GetString("BUILD_COSTS");
			pDC->SetTextColor(RGB(200,200,200));
			pDC->DrawText(s,infoRect,DT_CENTER);
			pDC->SetTextColor(oldColor);
			infoRect.SetRect(theClientRect.left+360,590,685,615);
			strBuildingInfo.Format("%s: %i",CResourceManager::GetString("INDUSTRY"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryForBuild());
			pDC->DrawText(strBuildingInfo,infoRect,DT_LEFT);
			strBuildingInfo.Format("%s: %i",CResourceManager::GetString("TITAN"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededTitanForBuild());
			pDC->DrawText(strBuildingInfo,infoRect,DT_CENTER);
			strBuildingInfo.Format("%s: %i",CResourceManager::GetString("DEUTERIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDeuteriumForBuild());
			pDC->DrawText(strBuildingInfo,infoRect,DT_RIGHT);
			infoRect.SetRect(theClientRect.left+360,615,685,640);
			strBuildingInfo.Format("%s: %i",CResourceManager::GetString("DURANIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDuraniumForBuild());
			pDC->DrawText(strBuildingInfo,infoRect,DT_LEFT);
			strBuildingInfo.Format("%s: %i",CResourceManager::GetString("CRYSTAL"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededCrystalForBuild());
			pDC->DrawText(strBuildingInfo,infoRect,DT_CENTER);
			strBuildingInfo.Format("%s: %i",CResourceManager::GetString("IRIDIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIridiumForBuild());
			pDC->DrawText(strBuildingInfo,infoRect,DT_RIGHT);
			if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDilithiumForBuild() > NULL)
			{
				infoRect.SetRect(theClientRect.left+360,640,685,665);
				strBuildingInfo.Format("%s: %i",CResourceManager::GetString("DILITHIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDilithiumForBuild());
				pDC->DrawText(strBuildingInfo,infoRect,DT_CENTER);
			}
		}
		// Hier die Beschreibung des Gebäudes bzw. die Informationen
		//pDC->Rectangle(BuildingDescription);
		if (m_bClickedOnBuildingDescriptionButton == TRUE  && RunningNumber != 0)
		{
			if (m_iWhichSubMenu == 0)		// im Gebäudeuntermenü
				strDescription = pDoc->GetBuildingDescription(RunningNumber);
			else if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)	// im Schiffsuntermenü
				strDescription = pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipDescription();
			else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)	// im Kasernenuntermenü
				strDescription = pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetDescription();
			CRect descRect(BuildingDescription.left, BuildingDescription.top, BuildingDescription.right, BuildingDescription.bottom - 10);
			pDC->DrawText(strDescription, descRect, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_WORD_ELLIPSIS);
		}
		// bzw. die Information zu dem, was das Gebäude produziert
		if (m_bClickedOnBuildingInfoButton == TRUE  && RunningNumber != 0)
		{
			if (m_iWhichSubMenu == 0)
				DrawBuildingProduction(pDC, BuildingDescription);
			else if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)
			{
				pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).DrawShipInformation(pDC, BuildingDescription, pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch());
				// DrawShipInformation() verändert irgendwie die FontGröße auf den Buttons, deswegen neu laden
				this->LoadRaceFont(pDC);
			}
			else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)
			{
				// Anzeige der Truppeneigenschaften
				pDC->SetTextColor(RGB(200,200,200));
				CRect r(BuildingDescription.left,BuildingDescription.top,BuildingDescription.right,BuildingDescription.bottom);
				pDC->DrawText(pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetName(), r, DT_TOP | DT_CENTER | DT_WORDBREAK);
				pDC->SetTextColor(oldColor);
				r.top += 60;
				BYTE offPower = pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetPower();
				if (pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetFieldStatus(1) == RESEARCHED)
					offPower += (offPower * pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetBonus(1) / 100);
				s.Format("%s: %d",CResourceManager::GetString("POWER"), offPower);
				pDC->DrawText(s, r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.top += 25;
				s.Format("%s: %d",CResourceManager::GetString("MORALVALUE"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetMoralValue());
				pDC->DrawText(s, r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.top += 25;
				s.Format("%s: %d",CResourceManager::GetString("PLACE"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetSize());
				pDC->DrawText(s, r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.top += 25;
				s.Format("%s: %d",CResourceManager::GetString("MAINTENANCE_COSTS"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetMaintenanceCosts());
				pDC->DrawText(s, r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			}
		}
		// Hier die Anzeige des Bildes zu dem jeweiligen Projekt
		if (m_iClickedOn != 0)
		{
			int RunningNumber = 0;
			i = m_iClickedOn;
			if (BuildList[i].runningNumber < 0)
				RunningNumber = (BuildList[i].runningNumber)*(-1);
			else
				RunningNumber = BuildList[i].runningNumber;
			if (RunningNumber != 0)
			{
				CString file;
				if (m_iWhichSubMenu == 0)		// sind im Gebäudeuntermenü
					file.Format("Buildings\\%s",pDoc->GetBuildingInfo(RunningNumber).GetGraphikFileName());
				else if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)	// sind im Schiffsuntermenü 
					file.Format("Ships\\%s.jpg",pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipClass());
				else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)	// sind im Kasernenuntermenü
					file.Format("Troops\\%s.jpg",pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetName());
				CBitmap* graphic = NULL;
				HGDIOBJ oldGraphic;
				graphic = pDoc->GetGraphicPool()->GetGraphic(file);
				if (graphic == NULL)
				{
					if (m_iWhichSubMenu == 0)		// sind im Gebäudeuntermenü
						graphic = pDoc->GetGraphicPool()->GetGraphic("Buildings\\ImageMissing.jpg");
					else if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)	// sind im Schiffsuntermenü 
						graphic = pDoc->GetGraphicPool()->GetGraphic("Ships\\ImageMissing.jpg");
					else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)	// sind im Kasernenuntermenü
						graphic = pDoc->GetGraphicPool()->GetGraphic("Troops\\ImageMissing.jpg");
				}
					
				if (graphic)
				{
					oldGraphic = mdc.SelectObject(*graphic);
					if (pDoc->GetPlayersRace() == FERENGI)
						pDC->BitBlt(60,50,200,150,&mdc,0,0,SRCCOPY);
					else if (pDoc->GetPlayersRace() == KLINGON)
						pDC->BitBlt(60,58,200,150,&mdc,0,0,SRCCOPY);
					else if (pDoc->GetPlayersRace() == ROMULAN)
						pDC->BitBlt(65,165,200,150,&mdc,0,0,SRCCOPY);
					else if (pDoc->GetPlayersRace() == CARDASSIAN)
						pDC->BitBlt(68,125,200,150,&mdc,0,0,SRCCOPY);
					else
						pDC->BitBlt(70,60,200,150,&mdc,0,0,SRCCOPY);
					mdc.SelectObject(oldGraphic);
				}
				/*
				FCObjImage* img = new FCObjImage();
				if (!img->Load("F:\\C++\\Projekte\\BotE Projekt\\BotE Install\\BotE complete\\Graphics\\" + file))
					AfxMessageBox("Kann nicht laden!");
				img->ConvertTo32Bit();
				FCWin32::AlphaImageOnDC(*img, pDC->GetSafeHdc(), CRect(70,60,270,210), 100);
				*/
			}
		}
	}
	// Hier die Anzeige der Kaufkosten, wenn wir auf den "kaufen Button" geklickt haben
	if (m_bClickedOnBuyButton == TRUE)
	{
		// Wenn was in der Bauliste steht
		if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(0) != 0)
		{
			CRect infoRect;
			int RunningNumber = 0;
			if (AssemblyList[0].runningNumber < 0)
			{
				RunningNumber = (AssemblyList[0].runningNumber)*(-1);
				s = CResourceManager::GetString("BUY_UPGRADE", FALSE, pDoc->GetBuildingName(pDoc->BuildingInfo.GetAt(RunningNumber-1).GetPredecessorID()),pDoc->GetBuildingName(RunningNumber));
			}
			else if (AssemblyList[0].runningNumber < 10000)
			{
				RunningNumber = AssemblyList[0].runningNumber;
				s = CResourceManager::GetString("BUY_BUILDING", FALSE, pDoc->GetBuildingName(RunningNumber));
			}
			else if (AssemblyList[0].runningNumber < 20000)
			{
				RunningNumber = AssemblyList[0].runningNumber;
				s = CResourceManager::GetString("BUY_SHIP", FALSE, pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipTypeAsString()
					,pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipClass());
			}
			else
			{
				RunningNumber = AssemblyList[0].runningNumber;
				s = CResourceManager::GetString("BUY_BUILDING", FALSE, pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetName());
			}
			infoRect.SetRect(theClientRect.left+340,565,703,630);
			//pDC->Rectangle(infoRect);
			pDC->DrawText(s,infoRect,DT_CENTER | DT_WORDBREAK);
			CString costs; costs.Format("%d", pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetBuildCosts());
			s = CResourceManager::GetString("LATINUM_COSTS", FALSE, costs);
			infoRect.SetRect(theClientRect.left+340,565,703,640);
			pDC->DrawText(s,infoRect, DT_BOTTOM | DT_SINGLELINE | DT_CENTER);
			// Ja/Nein Buttons zeichnen
			// Kleine Buttons unter der Kaufkostenübersicht
			OkayButton.SetRect(355,645,475,675);
			CancelButton.SetRect(theClientRect.left+565,theClientRect.top+645,theClientRect.left+685,theClientRect.top+675);
			this->LoadFontForLittleButton(pDC);
			if (pDoc->GetPlayersRace() == HUMAN || pDoc->GetPlayersRace() == FERENGI)
				mdc.SelectObject(bm);
			else
				mdc.SelectObject(bm2);
			pDC->BitBlt(355,645,120,30,&mdc,0,0,SRCCOPY);
			s = CResourceManager::GetString("BTN_OKAY");
			pDC->DrawText(s,OkayButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			pDC->BitBlt(565,645,120,30,&mdc,0,0,SRCCOPY);
			s = CResourceManager::GetString("BTN_CANCEL");
			pDC->DrawText(s,CancelButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
	// Anzeige der Bestätigungsfrage, ob ein Auftrag wirklich abgebrochen werden soll
	if (m_bClickedOnDeleteButton == TRUE)
	{
		CRect infoRect;
		//infoRect.SetRect(theClientRect.left+340,565,703,640);
		//pDC->Rectangle(infoRect);
		infoRect.SetRect(theClientRect.left+350,570,693,640);
		s = CResourceManager::GetString("CANCEL_PROJECT");
		pDC->DrawText(s,infoRect,DT_CENTER | DT_VCENTER | DT_WORDBREAK);
		// Ja/Nein Buttons zeichnen
		this->LoadFontForLittleButton(pDC);
		if (pDoc->GetPlayersRace() == HUMAN || pDoc->GetPlayersRace() == FERENGI)
			mdc.SelectObject(bm);
		else
			mdc.SelectObject(bm2);
		OkayButton.SetRect(355,645,475,675);
		CancelButton.SetRect(theClientRect.left+565,theClientRect.top+645,theClientRect.left+685,theClientRect.top+675);
		pDC->BitBlt(355,645,120,30,&mdc,0,0,SRCCOPY);
		s = CResourceManager::GetString("BTN_OKAY");
		pDC->DrawText(s,OkayButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		pDC->BitBlt(565,645,120,30,&mdc,0,0,SRCCOPY);
		s = CResourceManager::GetString("BTN_CANCEL");
		pDC->DrawText(s,CancelButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	// Anzeige der kleinen Buttons (Bauhof, Werft, Kaserne) unter der Bauliste	
	this->LoadFontForLittleButton(pDC);
	mdc.SelectObject(bm2);
	pDC->BitBlt(325,510,120,30,&mdc,0,0,SRCCOPY);
	s = CResourceManager::GetString("BTN_BAUHOF");
	pDC->DrawText(s,BuildingListButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDC->BitBlt(460,510,120,30,&mdc,0,0,SRCCOPY);
	s = CResourceManager::GetString("BTN_DOCKYARD");
	pDC->DrawText(s,ShipyardListButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	pDC->BitBlt(595,510,120,30,&mdc,0,0,SRCCOPY);
	s = CResourceManager::GetString("BTN_BARRACK");
	pDC->DrawText(s,TroopListButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	// plus Anzeige der kleinen Button (Info & Beschreibung) unter der Gebäudeinfobox
	if (pDoc->GetPlayersRace() == FERENGI)
		mdc.SelectObject(bm3);
	pDC->BitBlt(30,625,120,30,&mdc,0,0,SRCCOPY);
	s = CResourceManager::GetString("BTN_INFORMATION");
	pDC->DrawText(s,BuildingInfoButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	if (pDoc->GetPlayersRace() == FERENGI)
		mdc.SelectObject(bm4);
	pDC->BitBlt(165,625,120,30,&mdc,0,0,SRCCOPY);
	s = CResourceManager::GetString("BTN_DESCRIPTION");
	pDC->DrawText(s,BuildingDescriptionButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	// plus Anzeige der kleinen Buttons unter der Assemblylist (kaufen und abbrechen)
	// wenn wir noch nicht in dieser Runde gekauft haben
	if (pDoc->GetPlayersRace() != FERENGI)
		mdc.SelectObject(bm);
	if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetWasBuildingBought() == FALSE && pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(0) != 0)
	{
		// Bei Gebäuden nur wenn es nicht ein Auftrag mit NeverReady (z.B. Kriegsrecht) ist)
		if ((AssemblyList[0].runningNumber < 0)
			||			
			(AssemblyList[0].runningNumber > 0 && AssemblyList[0].runningNumber < 10000 && pDoc->GetBuildingInfo(AssemblyList[0].runningNumber).GetNeverReady() == FALSE)
			||
			// Bei Schiffen nur, wenn eine Werft noch aktiv ist
			(AssemblyList[0].runningNumber >= 10000 && AssemblyList[0].runningNumber < 20000 && pDoc->GetSystem(p).GetProduction()->GetShipYard())
			||
			// Bei Truppen nur mit aktiver Kaseren
			(AssemblyList[0].runningNumber >= 20000 && pDoc->GetSystem(p).GetProduction()->GetBarrack()))
		{			
			if (pDoc->GetPlayersRace() == FERENGI)
			{
				mdc.SelectObject(bm3);
				pDC->BitBlt(750,625,120,30,&mdc,0,0,SRCCOPY);
			}
			else
				pDC->BitBlt(750,625,120,30,&mdc,0,0,SRCCOPY);
			s = CResourceManager::GetString("BTN_BUY");
			pDC->DrawText(s,BuyButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}
	if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(0) != 0)
	{
		if (pDoc->GetPlayersRace() == FERENGI)
		{
			mdc.SelectObject(bm4);
			pDC->BitBlt(900,625,120,30,&mdc,0,0,SRCCOPY);
		}
		else
			pDC->BitBlt(900,625,120,30,&mdc,0,0,SRCCOPY);
		s = CResourceManager::GetString("BTN_CANCEL");
		pDC->DrawText(s,DeleteButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}

	// Systemnamen mit größerer Schrift in der Mitte zeichnen
	if (pDoc->GetPlayersRace() == HUMAN)
		rect.SetRect(theClientRect.left,theClientRect.top+50,theClientRect.right-17,theClientRect.top+100);
	else if (pDoc->GetPlayersRace() == KLINGON)
		rect.SetRect(theClientRect.left+50,theClientRect.top+5,theClientRect.left+280,theClientRect.top+55);
	else
		rect.SetRect(theClientRect.left,theClientRect.top,theClientRect.right-17,theClientRect.top+50);
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 4, &font));
	pDC->SelectObject(&font);
	// Name des Systems oben in der Mitte zeichnen				
	s.Format("%s",pDoc->GetSector(p.x,p.y).GetName());
	pDC->DrawText(s,rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);	
}



/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Arbeiterzuweisungsmenües
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawWorkersMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CString s;
	CRect rect;
	CPoint p = pDoc->GetKO();
	COLORREF oldColor = pDC->GetTextColor();

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_workmenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);
	CBitmap* graphic = NULL;
	HGDIOBJ oldGraphic;

	DrawInfoRightInSystemView(pDC, theClientRect);
	// Hier die Einträge in der Bauliste
	DrawBuildList(pDC, theClientRect);
	CBrush nb2(RGB(50,50,50));
	pDC->SelectObject(nb2);

	// Die Buttons zum Erhöhen bzw. Verringern der Arbeiteranzahl
	for (int i = 0; i < 5; i++)
	{
		pDC->Rectangle(PlusButton[i]);
		s.Format(">");
		pDC->DrawText(s,PlusButton[i],DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		pDC->Rectangle(MinusButton[i]);
		s.Format("<");
		pDC->DrawText(s,MinusButton[i],DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	CRect r = theClientRect;
	// Ansicht der "normalen Gebäude"
	if (m_bySubMenu == 1)
	{
		// Ab hier Anzeige der Besetzungsbalken
		// Balken mit maximal 200 Einträgen, d.h. es dürfen nicht mehr als 200 Gebäude von einem Typ stehen!!!
		unsigned short number[5] = {0};						// Anzahl der Gebäude eines Types
		unsigned short online[5] = {0};						// Anzahl der Gebäude eines Types, die auch online sind
		unsigned short greatestNumber = 0;					// Größte Nummer der Number[5]
		unsigned short width = 0;							// Breite eines einzelnen Balkens
		unsigned short size = 0;							// Größe der einzelnen Balken
		// Größte Nummer berechnen
		for (int i = 0; i < 5; i++)
		{
			number[i] = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i,0);
			if (number[i] > greatestNumber)
				greatestNumber = number[i];
			online[i] = pDoc->GetSystem(p.x,p.y).GetWorker(i);
			// Die Rechtecke der Arbeiterbalken erstmal löschen
			for (int j = 0; j < 200; j++)
				Timber[i][j].SetRect(0,0,0,0);
		}
		size = greatestNumber;
		if (size != 0)
			width = (unsigned short)200/size;
		if (width > 10)
			width = 10;
		short space = 5;
		if (width < 5)
		{
			if (width < 2)
				width = 2;
			space = width;
		}

		// Den Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < number[i]; j++)
			{
				// Fragen ob die Gebäude noch online sind
				if (j >= online[i])
				{
					// Dunkle Farbe wenn sie Offline sind
					CBrush nb2(RGB(42,46,30));
					pDC->SelectObject(nb2);
					Timber[i][j].SetRect(r.left+220+ j*width*2-space,r.top+115+i*95,r.left+220+width+ j*width*2,r.top+150+i*95);
					pDC->Rectangle(Timber[i][j]);
				}
				else
				{
					// Helle Farbe wenn sie Online sind
					// CBrush nb(RGB(240,240,80));
					short color = j*4;
					if (color > 230) color = 200;
					CBrush nb(RGB(230-color,230-color/2,20));
					pDC->SelectObject(nb);
					//Timber[i][j].SetRect((r.left+220)+(j*width*2),r.top+115+i*95,(r.left+220+width)+(j*width*2),r.top+150+i*95);
					Timber[i][j].SetRect(r.left+220+ j*width*2-space,r.top+115+i*95,r.left+220+width+ j*width*2,r.top+150+i*95);
					pDC->Rectangle(Timber[i][j]);
				}
				// Hier werden die Rechtecke von der Größe noch ein klein wenig verändert, damit man besser drauf klicken kann
				Timber[i][j].SetRect(r.left+220+ j*width*2-space,r.top+115+i*95,r.left+220+width+(j+1)*width*2-space,r.top+150+i*95);
			}
		}
		// Das Gebäude über dem Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			CString name = "";
			USHORT tmp = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i,1);
			if (tmp != 0) 
			{
				BOOLEAN loadSuccess = FALSE;
				// Bild des jeweiligen Gebäudes zeichnen
				CString file;
				file.Format("Buildings\\%s",pDoc->GetBuildingInfo(tmp).GetGraphikFileName());
				graphic = pDoc->GetGraphicPool()->GetGraphic(file);
				if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGraphic("Buildings\\ImageMissing.jpg");
				if (graphic)
				{
					oldGraphic = mdc.SelectObject(*graphic);
					int oldStretch = pDC->GetStretchBltMode();
					pDC->SetStretchBltMode(HALFTONE);
					pDC->StretchBlt(50,i*95+100,100,75,&mdc,0,0,200,150,SRCCOPY);
					pDC->SetStretchBltMode(oldStretch);
					mdc.SelectObject(oldGraphic);
					graphic = NULL;
				}
				name = pDoc->GetBuildingName(tmp);
				name.Format("%d x %s",number[i],name);
				rect.SetRect(r.left+220,r.top+90+i*95,r.left+600,r.top+115+i*95);
				CString yield = CResourceManager::GetString("YIELD");
				pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				if (i == 0)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetMaxFoodProd(), CResourceManager::GetString("FOOD"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				/*	if (!loadSuccess)
					{
						rect.SetRect(r.left+5,r.top+120,r.left+120,r.top+145);
						s = CResourceManager::GetString("FOOD")+":";
						pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
					}*/
				}
				else if (i == 1)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd(), CResourceManager::GetString("INDUSTRY"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				/*	if (!loadSuccess)
					{
						rect.SetRect(r.left+5,r.top+215,r.left+120,r.top+240);
						s = CResourceManager::GetString("INDUSTRY")+":";
						pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
					}*/
				}
				else if (i == 2)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetMaxEnergyProd(), CResourceManager::GetString("ENERGY"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				/*	if (!loadSuccess)
					{
						rect.SetRect(r.left+5,r.top+310,r.left+120,r.top+335);
						s = CResourceManager::GetString("ENERGY")+":";
						pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
					}*/
				}
				else if (i == 3)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd(), CResourceManager::GetString("SECURITY"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				/*	if (!loadSuccess)
					{
						rect.SetRect(r.left+5,r.top+405,r.left+120,r.top+430);
						s = CResourceManager::GetString("SECURITY")+":";
						pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
					}*/
				}
				else if (i == 4)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd(), CResourceManager::GetString("RESEARCH"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				/*	if (!loadSuccess)
					{
						rect.SetRect(r.left+5,r.top+500,r.left+120,r.top+525);
						s = CResourceManager::GetString("RESEARCH")+":";
						pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
					}*/
				}
			}
		}
		this->LoadFontForLittleButton(pDC);
		mdc.SelectObject(bm2);
		pDC->BitBlt(ChangeWorkersButton.left,ChangeWorkersButton.top,120,30,&mdc,0,0,SRCCOPY);
		s = CResourceManager::GetString("BTN_RESOURCES");
		pDC->DrawText(s,ChangeWorkersButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	// Wenn wir in die Ressourcenansicht wollen
	if (m_bySubMenu == 12)
	{
		// Ab hier Anzeige der Besetzungsbalken
		// Balken mit maximal 200 Einträgen, d.h. es dürfen nicht mehr als 200 Gebäude von einem Typ stehen!!!
		CRect workertimber[200];
		unsigned short number[5] = {0};						// Anzahl der Gebäude eines Types
		unsigned short online[5] = {0};
		unsigned short greatestNumber = 0;					// Größte Nummer der Number[5]
		unsigned short width = 0;							// Breite eines einzelnen Balkens
		unsigned short size = 0;							// Größe der einzelnen Balken
		// Größte Nummer berechnen
		for (int i = 0; i < 5; i++)
		{
			number[i] = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i+5,0);
			if (number[i] > greatestNumber)
				greatestNumber = number[i];
			online[i] = pDoc->GetSystem(p.x,p.y).GetWorker(i+5);
			// Die Rechtecke der Arbeiterbalken erstmal löschen
			for (int j = 0; j < 200; j++)
				Timber[i][j].SetRect(0,0,0,0);
		}
		size = greatestNumber;
		if (size != 0)
			width = (unsigned short)200/size;
		if (width > 10)
			width = 10;
		short space = 5;
		if (width < 5)
		{
			if (width < 2)
				width = 2;
			space = width;
		}

		// Den Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < number[i]; j++)
			{
				// Fragen ob die Gebäude noch online sind
				if (j >= online[i])
				{
					// Dunkle Farbe wenn sie Offline sind
					CBrush nb2(RGB(42,46,30));
					pDC->SelectObject(nb2);
					Timber[i][j].SetRect(r.left+220+ j*width*2-space,r.top+115+i*95,r.left+220+width+ j*width*2,r.top+150+i*95);
					pDC->Rectangle(Timber[i][j]);
				}
				else
				{
					// Helle Farbe wenn sie Online sind
					CBrush nb(RGB(240,240,80));
					pDC->SelectObject(nb);
					//Timber[i][j].SetRect((r.left+220)+(j*width*2),r.top+115+i*95,(r.left+220+width)+(j*width*2),r.top+150+i*95);
					Timber[i][j].SetRect(r.left+220+ j*width*2-space,r.top+115+i*95,r.left+220+width+ j*width*2,r.top+150+i*95);
					pDC->Rectangle(Timber[i][j]);
				}
				// Hier werden die Rechtecke von der Größe noch ein klein wenig verändert, damit man besser drauf klicken kann
				Timber[i][j].SetRect(r.left+220+ j*width*2-space,r.top+115+i*95,r.left+220+width+(j+1)*width*2-space,r.top+150+i*95);
			}
		}
		// Das Gebäude über dem Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			CString name = "";
			USHORT tmp = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i+5,1);
			if (tmp != 0) 
			{
				// Bild des jeweiligen Gebäudes zeichnen
				CString file;
				file.Format("Buildings\\%s",pDoc->GetBuildingInfo(tmp).GetGraphikFileName());
				graphic = pDoc->GetGraphicPool()->GetGraphic(file);
				if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGraphic("Buildings\\ImageMissing.jpg");
				if (graphic)
				{
					oldGraphic = mdc.SelectObject(*graphic);
					int oldStretch = pDC->GetStretchBltMode();
					pDC->SetStretchBltMode(HALFTONE);
					pDC->StretchBlt(50,i*95+100,100,75,&mdc,0,0,200,150,SRCCOPY);
					pDC->SetStretchBltMode(oldStretch);
					mdc.SelectObject(oldGraphic);
					graphic = NULL;
				}
				name = pDoc->GetBuildingName(tmp);
				name.Format("%d x %s",number[i],name);
				rect.SetRect(r.left+220,r.top+90+i*95,r.left+600,r.top+115+i*95);
				pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				CString yield = CResourceManager::GetString("YIELD");
				if (i == 0)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetTitanProd(), CResourceManager::GetString("TITAN"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
				else if (i == 1)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetDeuteriumProd(), CResourceManager::GetString("DEUTERIUM"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
				else if (i == 2)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetDuraniumProd(), CResourceManager::GetString("DURANIUM"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
				else if (i == 3)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetCrystalProd(), CResourceManager::GetString("CRYSTAL"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
				else if (i == 4)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetIridiumProd(), CResourceManager::GetString("IRIDIUM"));
					rect.SetRect(r.left+220,r.top+150+i*95,r.left+700,r.top+175+i*95);
					pDC->DrawText(name,rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
			}
		}
/*		// Anzeige was die Gebäude grob machen
		if (pDoc->GetPlayersRace() != HUMAN)
		{
			rect.SetRect(r.left+5,r.top+120,r.left+120,r.top+145);
			s = CResourceManager::GetString("TITAN");
			pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			rect.SetRect(r.left+5,r.top+215,r.left+120,r.top+240);
			s = CResourceManager::GetString("DEUTERIUM");
			pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			rect.SetRect(r.left+5,r.top+310,r.left+120,r.top+335);
			s = CResourceManager::GetString("DURANIUM");
			pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			rect.SetRect(r.left+5,r.top+405,r.left+120,r.top+430);
			s = CResourceManager::GetString("CRYSTAL");
			pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			rect.SetRect(r.left+5,r.top+500,r.left+120,r.top+525);
			s = CResourceManager::GetString("IRIDIUM");
			pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		}
*/		// Anzeige des Umschaltbuttons und der Erklärung
		//rect.SetRect(r.left+10,r.top+650,r.left+575,r.top+670);
		//s.Format("klicken um zur normalen Ansicht zu gelangen");
		//pDC->DrawText(s,rect,DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		this->LoadFontForLittleButton(pDC);
		mdc.SelectObject(bm2);
		pDC->BitBlt(ChangeWorkersButton.left,ChangeWorkersButton.top,120,30,&mdc,0,0,SRCCOPY);
		s = CResourceManager::GetString("BTN_NORMAL", TRUE);
		pDC->DrawText(s,ChangeWorkersButton,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	// Hier noch die Gesamt- und freien Arbeiter unten in der Mitte zeichnen
	CRect workertimber[200];
	unsigned short width = 0;
	unsigned short size = 0;
	unsigned short worker = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(10,0);
	size = worker;
	if (size != 0)
		width = (unsigned short)200/size;
	if (width > 10)
		width = 10;
	short space = 5;
	if (width < 5)
	{
		if (width < 2)
			width = 2;
		space = width;
	}		
	// Den Balken zeichnen
	for (int i = 0; i < worker; i++)
	{
		if (i < pDoc->GetSystem(p.x,p.y).GetWorker(11))
		{
			// Helle Farbe wenn sie Online sind
			short color = i*4;
			if (color > 230) color = 200;
			CBrush nb(RGB(230-color,230-color/2,20));
			pDC->SelectObject(nb);
			workertimber[i].SetRect((r.left+220)+i*width-space,r.top+600,r.left+220+width+i*width-space,r.top+625);
			pDC->Rectangle(workertimber[i]);
		}
		else
		{
			// Dunkle Farbe wenn sie Offline sind
			CBrush nb2(RGB(42,46,30));
			pDC->SelectObject(nb2);
			workertimber[i].SetRect((r.left+220)+i*width-space,r.top+600,r.left+220+width+i*width-space,r.top+625);
			pDC->Rectangle(workertimber[i]);
		}
	}
	// freie Arbeiter über dem Balken zeichnen
	pDC->SetTextColor(oldColor);
	rect.SetRect(r.left+220,r.top+575,r.left+600,r.top+600);
	s.Format("%s %d/%d",CResourceManager::GetString("FREE_WORKERS"), pDoc->m_System[p.x][p.y].GetWorker(11), pDoc->m_System[p.x][p.y].GetWorker(10));
	pDC->DrawText(s, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	// Großen Text ("Arbeiterzuweisung in xxx") oben links zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 4, 3, &font));
	pDC->SelectObject(&font);	
	if (pDoc->GetPlayersRace() == FERENGI)
		rect.SetRect(theClientRect.left,theClientRect.top,theClientRect.left+720,theClientRect.top+35);
	else
		rect.SetRect(theClientRect.left,theClientRect.top+10,theClientRect.left+720,theClientRect.top+60);
	// Arbeiterzuweisung auf xxx oben links zeichnen			
	s = CResourceManager::GetString("WORKERS_MENUE")+" "+pDoc->GetSector(p.x,p.y).GetName();
	pDC->DrawText(s,rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	DeleteObject(font);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Bauwerkeansicht (auch Abriss der Gebäude hier mgl.)
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawBuildingsOverviewMenue(CDC* pDC, CRect theClientRect)
{
	// Das Feld der ganzen Gebäude muß aufsteigend nach der RunningNumber sortiert sein.
	// Ansonsten funktioniert der Algorithmus hier nicht mehr.
	// Sortiert wird das Feld in der CalculateNumberOfWorkBuildings() Funktion der CSystem Klasse.
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CPoint p = pDoc->GetKO();;
	CString s;
	
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_overviewmenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);
	COLORREF oldColor = pDC->GetTextColor();

	// alte Gebäudeliste erstmal löschen
	m_BuildingOverview.RemoveAll();
	USHORT NumberOfBuildings = pDoc->GetSystem(p.x,p.y).GetAllBuildings()->GetSize();
	// Alle Gebäude durchgehen, diese müssen nach RunningNumber aufsteigend sortiert sein und in die Variable schreiben
	USHORT minRunningNumber = 0;
	short i = 0;
	short spaceX = 0;	// Platz in x-Richtung
	short spaceY = 0;
	while (i < NumberOfBuildings)
	{
		USHORT curRunningNumber = pDoc->m_System[p.x][p.y].GetAllBuildings()->GetAt(i).GetRunningNumber();
		if (curRunningNumber > minRunningNumber)
		{
			BuildingOverviewStruct bos;
			bos.runningNumber = curRunningNumber;
			m_BuildingOverview.Add(bos);
			minRunningNumber = curRunningNumber;
		}
		i++;
	}
	// Gebäude anzeigen
	CRect r;
	// provisorische Buttons für vor und zurück
	CBrush but(RGB(200,50,50));
	pDC->SelectObject(but);
	if (m_BuildingOverview.GetSize() > m_iBOPage * NOBIOL + NOBIOL)
	{
		pDC->Rectangle(1011,190,1074,242);
		pDC->DrawText(">", CRect(1011,190,1074,242), DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	if (m_iBOPage > 0)
	{
		pDC->Rectangle(1011,490,1074,542);
		pDC->DrawText("<", CRect(1011,490,1074,542), DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	CBrush nb(RGB(0,0,0));
	pDC->SelectObject(nb);
	// prüfen, dass man nicht auf einer zu hohen Seite ist, wenn zu wenig Gebäude vorhanden sind
	if (m_iBOPage * NOBIOL >= m_BuildingOverview.GetSize())
		m_iBOPage = 0;
	for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
	{
		// Wenn wir auf der richtigen Seite sind
		if (i < m_iBOPage * NOBIOL + NOBIOL)
		{
			pDC->SetTextColor(oldColor);
			// Aller 4 Einträge Y Platzhalter zurücksetzen und X Platzhalter eins hoch
			if (i%4 == 0 && i != m_iBOPage * NOBIOL)
			{
				spaceX++;
				spaceY = 0;
			}
			// großes Rechteck, was gezeichnet wird
			r.SetRect(60+spaceX*320,80+spaceY*150,350+spaceX*320,200+spaceY*150);
			pDC->Rectangle(&r);
			m_BuildingOverview[i].rect = r;
			// kleine Rechtecke, wo die verschiedenen Texte drin stehen
			CRect r1,r2;
			r1.SetRect(r.left+5,r.top,r.right-5,r.top+20);
			r2.SetRect(r.left+140,r.top,r.right-5,r.bottom);
			// Gebäudegrafik laden und anzeigen
			CBitmap* graphic = NULL;
			HGDIOBJ oldGraphic;
			CString file;
			file.Format("Buildings\\%s", pDoc->GetBuildingInfo(m_BuildingOverview.GetAt(i).runningNumber).GetGraphikFileName());
			graphic = pDoc->GetGraphicPool()->GetGraphic(file);
			if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGraphic("Buildings\\ImageMissing.jpg");
			if (graphic)
			{
				int oldStretchMode = pDC->GetStretchBltMode();
				pDC->SetStretchBltMode(HALFTONE);
				oldGraphic = mdc.SelectObject(*graphic);
				pDC->StretchBlt(r.left+5,r.top+25,130,97,&mdc,0,0,200,150,SRCCOPY);
				pDC->SetStretchBltMode(oldStretchMode);
				mdc.SelectObject(oldGraphic);
			}
			
			//Gebäudenamen und Anzahl in den Rechtecken anzeigen
			s.Format("%i x %s",pDoc->m_System[p.x][p.y].GetNumberOfBuilding(m_BuildingOverview.GetAt(i).runningNumber),
				pDoc->GetBuildingName(m_BuildingOverview.GetAt(i).runningNumber));
			pDC->DrawText(s,r1, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_WORD_ELLIPSIS);
			// Gebäudeproduktion zeichnen
			pDC->SetTextColor(RGB(200,200,200));
			s = pDoc->GetBuildingInfo(m_BuildingOverview.GetAt(i).runningNumber).GetProductionAsString
				(pDoc->m_System[p.x][p.y].GetNumberOfBuilding(m_BuildingOverview.GetAt(i).runningNumber));
			pDC->DrawText(s,CRect(r.left+140,r.top+25,r.right-20,r.bottom-5), DT_TOP | DT_LEFT | DT_WORD_ELLIPSIS);
			// Anzahl der abzureißenden Gebäude zeichnen
			unsigned short dn = pDoc->m_System[p.x][p.y].GetBuildingDestroy(m_BuildingOverview.GetAt(i).runningNumber);
			if (dn > 0)
			{
				s.Format("%s: %i",CResourceManager::GetString("TALON"), dn);
				COverlayBanner* banner = new COverlayBanner(r.TopLeft(), r.Size(), s, RGB(255,0,0));
				banner->SetBorderWidth(2);
				banner->Draw(pDC);
				delete banner;
			}
			spaceY++;
		}
	}

	// Wenn eine 75%ige Blockade erreicht wurde, dann ist ein Abriss nicht mehr möglich
	if (pDoc->m_System[p.x][p.y].GetBlockade() > NULL)
	{
		COverlayBanner *banner = new COverlayBanner(CPoint(250,250), CSize(560,150), CResourceManager::GetString("ONLY_PARTIAL_BUILDINGSCRAP"), RGB(200,0,0));
		banner->Draw(pDC);
		delete banner;
	}

	// Systemnamen mit größerer Schrift in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	
	CRect rect;
	rect.SetRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60);
	// Name des Systems oben in der Mitte zeichnen				
	s = CResourceManager::GetString("BUILDING_OVERVIEW_MENUE")+" "+pDoc->GetSector(p.x,p.y).GetName();
	pDC->DrawText(s,rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Energiezuweisungsansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawEnergyMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CPoint p = pDoc->GetKO();;
	CString s;
	COLORREF oldColor = pDC->GetTextColor();
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_energymenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);

	m_EnergyList.RemoveAll();
	// die Inhalte der einzelnen Buttons berechnen, max. 3 vertikal und 3 horizontal
	USHORT NumberOfBuildings = pDoc->GetSystem(p.x,p.y).GetAllBuildings()->GetSize();
	// Alle Gebäude durchgehen, diese müssen nach RunningNumber aufsteigend sortiert sein und in die Variable schreiben
	short spaceX = 0;	// Platz in x-Richtung
	short spaceY = 0;	// Platz in y-Richtung
	for (int i = 0; i < NumberOfBuildings; i++)
	{
		// wenn das Gebäude Energie benötigt
		if (pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(i).GetNeededEnergy() > 0)
		{
			ENERGYSTRUCT es;
			es.index = i;
			es.status = pDoc->m_System[p.x][p.y].GetAllBuildings()->GetAt(i).GetIsBuildingOnline();
			m_EnergyList.Add(es);
		}
	}
	// Gebäude, welche Energie benötigen anzeigen
	CRect r;
	// provisorische Buttons für vor und zurück
	CBrush but(RGB(200,50,50));
	pDC->SelectObject(but);
	if (m_EnergyList.GetSize() > m_iELPage * NOBIEL + NOBIEL)
	{
		pDC->Rectangle(1011,190,1074,242);
		pDC->DrawText(">", CRect(1011,190,1074,242), DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	if (m_iELPage > 0)
	{
		pDC->Rectangle(1011,490,1074,542);
		pDC->DrawText("<", CRect(1011,490,1074,542), DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}

	CBrush nb(RGB(20,20,20));
	pDC->SelectObject(nb);
	// prüfen, dass man nicht auf einer zu hohen Seite ist, wenn zu wenig Gebäude vorhanden sind
	if (m_iELPage * NOBIEL >= m_EnergyList.GetSize())
		m_iELPage = 0;
	for (int i = m_iELPage * NOBIEL; i < m_EnergyList.GetSize(); i++)
	{
		// Wenn wir auf der richtigen Seite sind
		if (i < m_iELPage * NOBIEL + NOBIEL)
		{
			// Aller 3 Einträge Y Platzhalter zurücksetzen und X Platzhalter eins hoch
			if (i%3 == 0 && i != m_iELPage * NOBIEL)
			{
				spaceX++;
				spaceY = 0;
			}
			// großes Rechteck, was gezeichnet wird
			r.SetRect(90+spaceX*320,100+spaceY*170,320+spaceX*320,247+spaceY*170);
			pDC->Rectangle(&r);
			m_EnergyList[i].rect = r;
			// Rechteck machen damit der Text oben rechts steht
			CRect tmpr;
			tmpr.SetRect(r.left+10,r.top+5,r.right-5,r.bottom);
			int id = pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetRunningNumber();
			pDC->DrawText(pDoc->GetBuildingName(id), tmpr, DT_LEFT | DT_SINGLELINE);
			// nötige Energie über den Status zeichnen
			tmpr.SetRect(r.right-70,r.bottom-100,r.right-5,r.bottom);
			s.Format("%d EP", pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetNeededEnergy());
			pDC->DrawText(s, tmpr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			// Rechteck machen damit der Status unten rechts steht
			tmpr.SetRect(r.right-70,r.bottom-50,r.right-5,r.bottom);
			// Wenn es offline ist
			if (!m_EnergyList.GetAt(i).status)
			{
				pDC->SetTextColor(RGB(200,0,0));
				pDC->DrawText("offline", tmpr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				pDC->SetTextColor(oldColor);
			}
			// Wenn es online ist
			else
			{
				pDC->SetTextColor(RGB(0,200,0));
				pDC->DrawText("online", tmpr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				pDC->SetTextColor(oldColor);
			}
			// Das Bild zu dem Gebäude zeichnen
			CBitmap* graphic = NULL;
			HGDIOBJ oldGraphic;
			CString fileName;
			fileName.Format("Buildings\\%s",pDoc->GetBuildingInfo(id).GetGraphikFileName());
			graphic = pDoc->GetGraphicPool()->GetGraphic(fileName);
			if (graphic == NULL)
				graphic = pDoc->GetGraphicPool()->GetGraphic("Buildings\\ImageMissing.jpg");
			if (graphic)
			{
				int oldStretchMode = pDC->GetStretchBltMode();
				pDC->SetStretchBltMode(HALFTONE);
				oldGraphic = mdc.SelectObject(*graphic);
				pDC->StretchBlt(r.left+5,r.top+32,150,113,&mdc,0,0,200,150,SRCCOPY);
				pDC->SetStretchBltMode(oldStretchMode);
				mdc.SelectObject(oldGraphic);
			}
			spaceY++;
		}
	}

	// Oben in der Mitte den aktuellen/verfügbaren Energiebetrag zeichnen
	int energy = pDoc->GetSystem(p.x,p.y).GetProduction()->GetEnergyProd();
	s.Format("%s: %d",CResourceManager::GetString("USABLE_ENERGY"), energy);
	if (energy < 0)
		pDC->SetTextColor(RGB(200,0,0));
	else if (energy == 0)
		pDC->SetTextColor(RGB(200,200,0));
	else
		pDC->SetTextColor(RGB(0,200,0));
	pDC->DrawText(s, CRect(200,65,875,95) ,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Systemnamen mit größerer Schrift in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	// Name des Systems oben in der Mitte zeichnen				
	s = CResourceManager::GetString("ENERGY_MENUE")+" "+pDoc->GetSector(p.x,p.y).GetName();
	pDC->DrawText(s,CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60) ,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Systemhandelsansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawSystemTradeMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CPoint p = pDoc->GetKO();;
	CString s,s2;
	COLORREF oldColor = pDC->GetTextColor();
	CBrush nb(RGB(0,0,0));
	pDC->FillRect(theClientRect, &nb);
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_systrademenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);

	CPen pen(PS_SOLID, 1, RGB(200,200,200));
	pDC->SelectObject(&pen);

//	pDC->MoveTo((theClientRect.right)/2, 60); pDC->LineTo((theClientRect.right)/2, theClientRect.bottom);
	pDC->SetTextColor(RGB(220,220,220));
	pDC->DrawText(CResourceManager::GetString("TRADE_AND_RESOURCEROUTES"), CRect(0,80,538,105), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("STELLAR_STORAGE"), CRect(538,80,1075,105), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SYSTEM_STORAGE"), CRect(538,140,806,165), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("STELLAR_STORAGE"), CRect(806,140,1075,165), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("TITAN"), CRect(538,220,1075,280), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DEUTERIUM"), CRect(538,280,1075,340), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DURANIUM"), CRect(538,340,1075,400), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("CRYSTAL"), CRect(538,400,1075,460), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("IRIDIUM"), CRect(538,460,1075,520), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SetTextColor(oldColor);
	
	USHORT maxTradeRoutes = (USHORT)(pDoc->m_System[p.x][p.y].GetHabitants() / TRADEROUTEHAB) + pDoc->m_System[p.x][p.y].GetProduction()->GetAddedTradeRoutes();
	short addResRoute = 1;
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
	if (pDoc->GetEmpire(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetFieldStatus(3) == RESEARCHED)
		if (maxTradeRoutes == NULL)
		{
			maxTradeRoutes += pDoc->GetEmpire(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetBonus(3);
			addResRoute += pDoc->GetEmpire(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetBonus(3);
		}
	s.Format("%d", maxTradeRoutes);
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> eine Ressourcenroute mehr
	if (pDoc->GetEmpire(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetResearch()->GetResearchInfo()->GetResearchComplex(10)->GetFieldStatus(3) == RESEARCHED)
		addResRoute += pDoc->GetEmpire(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetResearch()->GetResearchInfo()->GetResearchComplex(10)->GetBonus(3);
	USHORT maxResourceRoutes = (USHORT)(pDoc->m_System[p.x][p.y].GetHabitants() / TRADEROUTEHAB) + pDoc->m_System[p.x][p.y].GetProduction()->GetAddedTradeRoutes() + addResRoute;
	s2.Format("%d", maxResourceRoutes);
	pDC->DrawText(CResourceManager::GetString("SYSTEM_SUPPORTS_ROUTES",0,s,s2), CRect(60,140,500,180), DT_LEFT | DT_TOP | DT_WORDBREAK);
	s.Format("%d",pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize());
	s2.Format("%d",pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetSize());
	pDC->DrawText(CResourceManager::GetString("SYSTEM_HAS_ROUTES",0,s,s2), CRect(60,180,500,220), DT_LEFT | DT_TOP | DT_WORDBREAK);
	pDC->DrawText(CResourceManager::GetString("ROUTES_TO"), CRect(60,220,500,250), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->SetTextColor(oldColor);
	// Anzeige von max. NOTRIL Handelsrouten
	// prüfen, dass man nicht auf einer zu hohen Seite ist, wenn zu wenig Handelsrouten vorhanden sind
	if (m_iSTPage * NOTRIL >= pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize())
		m_iSTPage = 0;
	int numberOfTradeRoutes = 0;
	// zuerst die Handelsrouten anzeigen
	for (int i = m_iSTPage * NOTRIL; i < pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize(); i++)
	{
		CPoint dest = pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetAt(i).GetDestKO();
		if (pDoc->GetSector(dest).GetKnown(pDoc->m_System[p.x][p.y].GetOwnerOfSystem()) == TRUE)
			s = pDoc->GetSector(dest).GetName();
		else
			s.Format("%s %c%i",CResourceManager::GetString("SECTOR"),(char)(dest.y+97),dest.x+1);
		pDC->DrawText(s,CRect(70,260+i*30,195,285+i*30), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		// Gewinn inkl. der Boni auf Handelsrouten ohne Boni auf Latinum und Boni durch Moral
		USHORT lat = pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetAt(i).GetLatinum(pDoc->m_System[p.x][p.y].GetProduction()->GetIncomeOnTradeRoutes());
		s.Format("%s: %d %s",CResourceManager::GetString("PROFIT"), lat, CResourceManager::GetString("LATINUM"));
		pDC->DrawText(s,CRect(200,260+i*30,370,285+i*30), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		// verbleibende Dauer der Handelsroute anzeigen
		short duration = pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetAt(i).GetDuration();
		if (duration < 0)
			duration = 6-abs(duration);
		if (duration > 1)
			s.Format("%s %d %s",CResourceManager::GetString("STILL"), duration, CResourceManager::GetString("ROUNDS"));
		else
			s.Format("%s %d %s",CResourceManager::GetString("STILL"), duration, CResourceManager::GetString("ROUND"));
		pDC->DrawText(s, CRect(380,260+i*30,510,285+i*30), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		numberOfTradeRoutes = i + 1;
		if (i >= m_iSTPage * NOTRIL + NOTRIL - 1)
			break;
	}
	// jetzt die Ressourcenrouten anzeigen
	pDC->SetTextColor(RGB(220,220,220));
	for (int i = 0; i < pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetSize(); i++)
	{
		int j = i + numberOfTradeRoutes;
		CPoint dest = pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetAt(i).GetKO();
		s.Format("%s", pDoc->GetSector(dest).GetName());
		pDC->DrawText(s, CRect(70,260+j*30,195,285+j*30), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		switch (pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetAt(i).GetResource())
		{
		case TITAN:		s = CResourceManager::GetString("TITAN"); break;
		case DEUTERIUM: s = CResourceManager::GetString("DEUTERIUM"); break;
		case DURANIUM:	s = CResourceManager::GetString("DURANIUM"); break;
		case CRYSTAL:	s = CResourceManager::GetString("CRYSTAL"); break;
		case IRIDIUM:	s = CResourceManager::GetString("IRIDIUM"); break;
		default:		s = "";
		}
		pDC->DrawText(s, CRect(200,260+j*30,350,285+j*30), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		if (j >= m_iSTPage * NOTRIL + NOTRIL - 1)
			break;
	}
	pDC->SetTextColor(oldColor);
	
	// Inhalte des system- und globalen Lagers zeichnen
	CPen(PS_SOLID, 0, RGB(255,100,100));
	CBrush grey(RGB(40,40,40));
	pDC->SelectObject(&pen);
	pDC->SelectObject(&grey);
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		s.Format("%d",pDoc->m_System[p.x][p.y].GetRessourceStore(i));
		pDC->DrawText(s, CRect(668,220+i*60,1075,280+i*60), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		// in Klammern darunter, wieviel Ressourcen ich nächste Runde aus diesem System ins Globale Lager verschiebe
		s.Format("(%d)",pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetSubResource(i,p)
			- pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetAddedResource(i,p));
		pDC->DrawText(s, CRect(668,265+i*60,1075,280+i*60), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		// globalen Lagerinhalt zeichnen
		s.Format("%d",pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetRessourceStorage(i));
		pDC->DrawText(s, CRect(538,220+i*60,945,280+i*60), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		// in Klammern steht, wieviel dieser Ressource nächste Runde aus dem Lager entfernt wird
		s.Format("(%d)",pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetAllAddedResource(i)
			- pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetAllSubResource(i));
		pDC->DrawText(s, CRect(538,265+i*60,945,280+i*60), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		// kleine Pfeilbuttons zeichnen (links)
		pDC->Rectangle(608,235+i*60,638,265+i*60);
		pDC->DrawText("<",CRect(608,235+i*60,638,265+i*60), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		// kleine Pfeilbuttons zeichnen (rechts)
		pDC->Rectangle(975,235+i*60,1005,265+i*60);
		pDC->DrawText(">",CRect(975,235+i*60,1005,265+i*60), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	s.Format("%d",pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetTakenRessources());
	s2.Format("%d",pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetMaxTakenRessources());
	pDC->DrawText(CResourceManager::GetString("TAKE_FROM_STORAGE",0,s,s2), CRect(538,190,1075,215), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d",pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetLosing());
	pDC->DrawText(CResourceManager::GetString("LOST_PER_ROUND",0,s), CRect(538,550,1075,580), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("MULTIPLIER"),CRect(538,600,720,630), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

	// Button zum Anlegen einer Handelsroute zeichnen
	LoadFontForLittleButton(pDC);
	mdc.SelectObject(bm);
	pDC->BitBlt(60,600,120,30,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_TRADEROUTE"), CRect(60,600,180,630), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// Button zum Anlegen einer Ressourcenroute zeichnen
	pDC->BitBlt(360,600,120,30,&mdc,0,0,SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_RESOURCEROUTE"), CRect(360,600,480,630), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// Button zum Ändern der Ressource bei einer Ressourcenroute
	pDC->BitBlt(360,640,120,30,&mdc,0,0,SRCCOPY);
	switch (m_byResourceRouteRes)
	{
	case TITAN:		s = CResourceManager::GetString("TITAN"); break;
	case DEUTERIUM: s = CResourceManager::GetString("DEUTERIUM"); break;
	case DURANIUM:	s = CResourceManager::GetString("DURANIUM"); break;
	case CRYSTAL:	s = CResourceManager::GetString("CRYSTAL"); break;
	case IRIDIUM:	s = CResourceManager::GetString("IRIDIUM"); break;
	default:		s = "";
	}
	pDC->DrawText(s, CRect(360,640,480,670), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	// Buttons zum Kündigen/Aufheben einer Ressourcenroute zeichnen
	for (int i = 0; i < pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetSize(); i++)
	{
		int j = i + numberOfTradeRoutes;
		pDC->BitBlt(360,260+j*30,120,30,&mdc,0,0,SRCCOPY);
		pDC->DrawText(CResourceManager::GetString("BTN_ANNUL"), CRect(360,260+j*30,480,290+j*30), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	// Button zum Ändern der Menge, wieviel pro Klick vom oder ins Globale Lager verschoben werden zeichnen
	pDC->BitBlt(747,600,120,30,&mdc,0,0,SRCCOPY);
	s.Format("%d",m_iGlobalStoreageQuantity);
	pDC->DrawText(s,CRect(747,600,867,630), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
	// Systemnamen mit größerer Schrift in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	// Name des Systems oben in der Mitte zeichnen				
	s.Format("%s %s",CResourceManager::GetString("TRADEOVERVIEW_IN"),pDoc->GetSector(p.x,p.y).GetName());
	pDC->DrawText(s,CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60) ,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}


/////////////////////////////////////////////////////////////////////////////////////////	
// Ab hier die ganzen Buttons mit ihrer Beschreibung am unteren Bildschirmrand
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawButtonsUnderSystemView(CDC* pDC)
{	
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_BuildMenueMainButtons, m_bySubMenu);
}

/////////////////////////////////////////////////////////////////////////////////////////	
// Funktion zeichnet die Bauliste in der Baumenü- und Arbeitermenüansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawBuildList(CDC* pDC, CRect theClientRect)
{
	// Hier die Einträge in der Bauliste
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	COLORREF oldColor = pDC->GetTextColor();
	int RoundToBuild;
	CRect rect = theClientRect;
	CPoint p = pDoc->GetKO();
	CString m_strAssemblyListEntry("");
	int y = 410;
	for (int i = 0; i < ALE; i++)
		if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i) != 0)	// noch Platz in der Bauliste
		{
			// ersten Eintrag in der Bauliste
			if (i == 0)
			{
				// Farbe der Schrift für den ersten Eintrag in der Bauliste wäheln
				if (pDoc->GetPlayersRace() == CARDASSIAN)
					pDC->SetTextColor(RGB(255,128,0));
				else
					pDC->SetTextColor(RGB(220,220,220));
			}
			// ist es ein Update
			if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i) < 0)
			{
				m_strAssemblyListEntry = CResourceManager::GetString("UPGRADING", FALSE, pDoc->GetBuildingName(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i)*(-1)));
				AssemblyList[i].runningNumber = pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i);
			}
			// ist es ein Gebäude
			else if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i) < 10000)
				m_strAssemblyListEntry = pDoc->GetBuildingName(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i));
			// ist es ein Schiff
			else if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i) < 20000)
				m_strAssemblyListEntry.Format("%s-%s",pDoc->m_ShipInfoArray.GetAt(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i)-10000).GetShipClass(),
				CResourceManager::GetString("CLASS"));
			// ist es eine Truppe
			else
				m_strAssemblyListEntry = pDoc->m_TroopInfo.GetAt(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i)-20000).GetName();
			AssemblyList[i].rect.SetRect(760,y,1000,y+24);
			AssemblyList[i].runningNumber = pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i);
			pDC->DrawText(m_strAssemblyListEntry,AssemblyList[i].rect, DT_TOP | DT_LEFT | DT_WORD_ELLIPSIS);
			
			// Testweise kleines Symbol in der Bauliste vom Auftrag (achtung, hier nur Gebäude)
			/*CEnBitmap bm;
			CDC mdc;
			mdc.CreateCompatibleDC(pDC);
			CString file;
			file.Format(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Buildings/%s",pDoc->GetBuildingInfo(AssemblyList[i].runningNumber).GetGraphikFileName());
			bm.LoadImage(file);
			mdc.SelectObject(bm);
			int oldStretch = pDC->GetStretchBltMode();
			pDC->SetStretchBltMode(HALFTONE);
			pDC->StretchBlt(750,y,33,25,&mdc,0,0,200,150,SRCPAINT);
			pDC->SetStretchBltMode(oldStretch);*/
						
			// Hier Berechnung der noch verbleibenden Runden, bis das Projekt fertig wird (nicht bei NeverReady-Aufträgen)
			// divide by zero check
			if (pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() > 0)
			{
				if (AssemblyList[i].runningNumber > 0 && AssemblyList[i].runningNumber < 10000 && pDoc->GetBuildingInfo(AssemblyList[i].runningNumber).GetNeverReady())
				{
					RoundToBuild = pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededIndustryInAssemblyList(i);
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				// Bei Upgrades
				else if (AssemblyList[i].runningNumber < 0)
				{
					RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(i))
						/((float)pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd()
							* (100+pDoc->GetSystem(p.x,p.y).GetProduction()->GetUpdateBuildSpeed())/100));
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				// Bei Gebäuden
				else if (AssemblyList[i].runningNumber < 10000)
				{
					RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(i))
						/((float)pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd()
							* (100+pDoc->GetSystem(p.x,p.y).GetProduction()->GetBuildingBuildSpeed())/100));
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				// Bei Schiffen Wertfeffiziens mitbeachten
				else if (AssemblyList[i].runningNumber < 20000 && pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency() > 0)
				{					
					RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(i))
						/((float)pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency() / 100
							* (100+pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipBuildSpeed())/100));
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				// Bei Truppen die Kaserneneffiziens beachten
				else if (pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency() > 0)
				{
					RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryInAssemblyList(i))
						/((float)pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency() / 100
							* (100+pDoc->GetSystem(p.x,p.y).GetProduction()->GetTroopBuildSpeed())/100));
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				else
					m_strAssemblyListEntry.Format("?");
				pDC->DrawText(m_strAssemblyListEntry,CRect(740,y,1025,y+24), DT_TOP | DT_RIGHT);
			}
			y += 25;
			pDC->SetTextColor(oldColor);
		}
	// Wenn nix in der Bauliste steht, automatisch Handelsgüter reinschreiben
	if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(0) == 0)
	{
		rect.SetRect(760,410,1030,434);
		pDC->SetTextColor(RGB(220,220,220));
		pDC->DrawText(CResourceManager::GetString("COMMODITIES") ,rect,DT_TOP | DT_LEFT);
		pDC->SetTextColor(oldColor);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////	
// Hier werden die Informationen zur Produktion usw. oben rechts in der Systemansicht ausgegeben
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawInfoRightInSystemView(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CString s;
	CRect rect;
	CPoint p;
	p = pDoc->GetKO();
	COLORREF oldColor = pDC->GetTextColor();

	// Die Rohstoffe und sonstige Informationen im System oben in der Mitte anzeigen
	// Lager und Rohstoffe rechts zeichnen
	if (pDoc->GetPlayersRace() == HUMAN)
		pDC->SetTextColor(RGB(0,0,0));
	if (pDoc->GetPlayersRace() == FERENGI)
	{
		pDC->SetTextColor(RGB(0,50,0));
		pDC->TextOut(860,10, CResourceManager::GetString("PRODUCTION"));
		pDC->TextOut(975,16, CResourceManager::GetString("STORAGE"));
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		pDC->SetTextColor(RGB(180,180,180));
		pDC->TextOut(870,20, CResourceManager::GetString("PRODUCTION"));
		pDC->TextOut(970,20, CResourceManager::GetString("STORAGE"));
	}
	else if (pDoc->GetPlayersRace() == CARDASSIAN)
	{
		pDC->TextOut(877,18, CResourceManager::GetString("PRODUCTION"));
		pDC->TextOut(977,19, CResourceManager::GetString("STORAGE"));
	}
	else
	{
		pDC->TextOut(870,20, CResourceManager::GetString("PRODUCTION"));
		pDC->TextOut(980,20, CResourceManager::GetString("STORAGE"));
	}
	pDC->SetTextColor(oldColor);
	pDC->TextOut(775,55,CResourceManager::GetString("FOOD")+":");
	pDC->TextOut(775,80,CResourceManager::GetString("INDUSTRY")+":");
	pDC->TextOut(775,105,CResourceManager::GetString("ENERGY")+":");
	pDC->TextOut(775,130,CResourceManager::GetString("SECURITY")+":");
	pDC->TextOut(775,155,CResourceManager::GetString("RESEARCH")+":");
	pDC->TextOut(775,180,CResourceManager::GetString("TITAN")+":");
	pDC->TextOut(775,205,CResourceManager::GetString("DEUTERIUM")+":");
	pDC->TextOut(775,230,CResourceManager::GetString("DURANIUM")+":");
	pDC->TextOut(775,255,CResourceManager::GetString("CRYSTAL")+":");
	pDC->TextOut(775,280,CResourceManager::GetString("IRIDIUM")+":");
	pDC->TextOut(775,305,CResourceManager::GetString("DILITHIUM")+":");
	pDC->TextOut(775,330,CResourceManager::GetString("MORAL")+":");
	pDC->TextOut(775,355,CResourceManager::GetString("LATINUM")+":");
	// Produktion anzeigen
	rect.SetRect(870,55,930,80);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetFoodProd());
	if (atoi(s) < 0)
	{
		pDC->SetTextColor(RGB(200,0,0));
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
		pDC->SetTextColor(oldColor);
	}
	else
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	if (m_iWhichSubMenu == 0)		// Wenn wir im normalen Baumenü sind
	{
		rect.SetRect(870,80,930,105);
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd());
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	else if (m_iWhichSubMenu == 1)	// Wenn wir im Werftuntermenü sind
	{
		rect.SetRect(870,80,930,105);
		s.Format("%i",(int)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * 
			pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency() / 100));
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	else							// Wenn wir im Kasernenuntermenü sind
	{
		rect.SetRect(870,80,930,105);
		s.Format("%i",(short)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * 
			pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency() / 100));
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	}	
	rect.SetRect(870,105,930,130);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetEnergyProd());
	if (atoi(s) < 0)
	{
		pDC->SetTextColor(RGB(200,0,0));
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
		pDC->SetTextColor(oldColor);
	}
	else
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,130,930,155);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,155,930,180);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,180,930,205);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetTitanProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,205,930,230);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetDeuteriumProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,230,930,255);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetDuraniumProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,255,930,280);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetCrystalProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,280,930,305);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetIridiumProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,305,930,330);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetDilithiumProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(870,355,930,380);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetLatinumProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	// Moralproduktion
	rect.SetRect(870,330,930,355);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetMoralProd());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	// ab hier Lager anzeigen
	rect.SetRect(950,55,1050,80);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetFoodStore());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	if (m_iWhichSubMenu == 1)		// Wenn wir im Werftuntermenü sind
	{
		rect.SetRect(950,80,1050,105);
		s.Format("%i%%",pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency());
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	if (m_iWhichSubMenu == 2)		// Wenn wir im Kasernenuntermenü sind
	{
		rect.SetRect(950,80,1050,105);
		s.Format("%i%%",pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency());
		pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	}
	rect.SetRect(950,180,1050,205);
	// Zusätzliche Ressourcen aus den Startsystemen von Ressourcenrouten ermitteln
	ULONG resFromRoutes[5] = {0};
	BYTE race = pDoc->GetPlayersRace();
	for (int j = 0; j < pDoc->GetEmpire(race)->GetSystemList()->GetSize(); j++)
		if (pDoc->GetEmpire(race)->GetSystemList()->GetAt(j).ko != p)
		{
			CPoint ko = pDoc->GetEmpire(race)->GetSystemList()->GetAt(j).ko;
			// Wenn unser System blockiert wird so gelten die Ressourcenrouten nicht
			if (pDoc->m_System[ko.x][ko.y].GetBlockade() > NULL)
				continue;
			for (int i = 0; i < pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetSize(); i++)
				if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(i).GetKO() == p)
				{
					// Wenn das Startsystem blockiert wird, so kann die Ressourcenroute ebenfalls nicht benutzt werden
					if (pDoc->m_System[pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(i).GetKO().x][pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(i).GetKO().y].GetBlockade() > NULL)
						continue;
					BYTE res = pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(i).GetResource();
					resFromRoutes[res] += pDoc->m_System[ko.x][ko.y].GetRessourceStore(res);
				}
		}				
	if (resFromRoutes[TITAN] > 0)
		s.Format("(%i)",pDoc->GetSystem(p.x,p.y).GetTitanStore() + resFromRoutes[TITAN]);
	else
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetTitanStore());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,205,1050,230);
	if (resFromRoutes[DEUTERIUM] > 0)
		s.Format("(%i)",pDoc->GetSystem(p.x,p.y).GetDeuteriumStore() + resFromRoutes[DEUTERIUM]);
	else
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetDeuteriumStore());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,230,1050,255);
	if (resFromRoutes[DURANIUM] > 0)
		s.Format("(%i)",pDoc->GetSystem(p.x,p.y).GetDuraniumStore() + resFromRoutes[DURANIUM]);
	else
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetDuraniumStore());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,255,1050,280);
	if (resFromRoutes[CRYSTAL] > 0)
		s.Format("(%i)",pDoc->GetSystem(p.x,p.y).GetCrystalStore() + resFromRoutes[CRYSTAL]);
	else
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetCrystalStore());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,280,1050,305);
	if (resFromRoutes[IRIDIUM] > 0)
		s.Format("(%i)",pDoc->GetSystem(p.x,p.y).GetIridiumStore() + resFromRoutes[IRIDIUM]);
	else
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetIridiumStore());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,305,1050,330);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetDilithiumStore());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,330,1050,355);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetMoral());
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	// Überall ne Null hinschreiben, wovon es eh kein Lager gibt
	/*s.Format("0");
	rect.SetRect(950,80,1050,105);
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,105,1050,130);
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,130,1050,155);
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,155,1050,180);
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	rect.SetRect(950,330,1050,355);
	pDC->DrawText(s,rect,DT_CENTER | DT_TOP | DT_SINGLELINE);*/
	// in der Mitte unter dem Systemnamen die Moral des Systems anteigen

	// kleine Bilder von den Rohstoffen zeichnen
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	CBitmap* graphic = NULL;
	HGDIOBJ oldGraphic;

	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\foodSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,58,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\industrySmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,83,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\energySmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,108,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\securitySmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,133,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\researchSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,158,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\titanSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,183,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\deuteriumSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,208,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\duraniumSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,233,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\crystalSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,258,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\iridiumSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,283,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\Dilithium.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,308,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\moralSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,333,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\latinumSmall.png");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->BitBlt(740,358,20,16,&mdc,0,0,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
}

// Funktion berechnet und zeichnet im "rect" die Produktion eins Gebäudes, also alles was es macht
void CSystemMenuView::DrawBuildingProduction(CDC* pDC, CRect rect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r;
	CString s;
	int RunningNumber = BuildList[m_iClickedOn].runningNumber;
	COLORREF oldColor = pDC->GetTextColor();
	// Oben im Beschreibungsrechteck den Namen des Projektes hinschreiben
	// Ist es ein Gebäude oder ein Upgrade eines Gebäudes, welches aber keine Arbeiter benötigt
	if (RunningNumber > 0 || pDoc->GetBuildingInfo(RunningNumber*(-1)).GetWorker() == FALSE)
	{
		if (RunningNumber < 0)
			RunningNumber *= -1;
		CBuildingInfo* b = &pDoc->GetBuildingInfo(RunningNumber);
		r.SetRect(rect.left,rect.top+5,rect.right,rect.top+25);
		s.Format("%s:",pDoc->GetBuildingName(RunningNumber));
		pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
		r.SetRect(rect.left,rect.top+35,rect.right,rect.top+55);
		pDC->SetTextColor(RGB(200,200,200));
		pDC->DrawText(CResourceManager::GetString("PRODUCTION")+":" ,r,DT_TOP | DT_SINGLELINE | DT_CENTER);
		r.SetRect(r.left,r.bottom,r.right,r.bottom+20);
		pDC->SetTextColor(oldColor);
	
		if (b->GetFoodProd() > 0)
		{

			s.Format("%s: %i",CResourceManager::GetString("FOOD"), b->GetFoodProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetIPProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("INDUSTRY"), b->GetIPProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetEnergyProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("ENERGY"), b->GetEnergyProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetSPProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SECURITY"), b->GetSPProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetFPProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("RESEARCH"), b->GetFPProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetTitanProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("TITAN"), b->GetTitanProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetDeuteriumProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("DEUTERIUM"), b->GetDeuteriumProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetDuraniumProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("DURANIUM"), b->GetDuraniumProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetCrystalProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("CRYSTAL"), b->GetCrystalProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetIridiumProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("IRIDIUM"), b->GetIridiumProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetDilithiumProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("DILITHIUM"), b->GetDilithiumProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetLatinum() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("LATINUM"), b->GetLatinum());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetMoralProd() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("MORAL"), b->GetMoralProd());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetMoralProdEmpire() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("MORAL_EMPIREWIDE"), b->GetMoralProdEmpire());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		
		// Ab hier die Boni
		if (b->GetFoodBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("FOOD_BONUS"), b->GetFoodBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetIndustryBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("INDUSTRY_BONUS"), b->GetIndustryBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetEnergyBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("ENERGY_BONUS"), b->GetEnergyBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetSecurityBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SECURITY_BONUS"), b->GetSecurityBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetResearchBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("RESEARCH_BONUS"), b->GetResearchBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetTitanBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("TITAN_BONUS"), b->GetTitanBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetDeuteriumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("DEUTERIUM_BONUS"), b->GetDeuteriumBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetDuraniumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("DURANIUM_BONUS"), b->GetDuraniumBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetCrystalBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("CRYSTAL_BONUS"), b->GetCrystalBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetIridiumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("IRIDIUM_BONUS"), b->GetIridiumBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetDilithiumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("DILITHIUM_BONUS"), b->GetDilithiumBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetAllRessourcesBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("BONUS_TO_ALL_RES"), b->GetAllRessourcesBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetLatinumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("LATINUM_BONUS"), b->GetLatinumBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		// Wenn alle Forschungsboni belegt sind, sprich Bonus auf alles
		if (b->GetBioTechBoni() != 0 && b->GetBioTechBoni() == b->GetEnergyTechBoni() && b->GetBioTechBoni() == b->GetCompTechBoni()
			&& b->GetBioTechBoni() == b->GetPropulsionTechBoni() && b->GetBioTechBoni() == b->GetConstructionTechBoni()
			&& b->GetBioTechBoni() == b->GetWeaponTechBoni())
		{
			s.Format("%s: %i%%",CResourceManager::GetString("TECHNIC_BONUS"), b->GetBioTechBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		else // bei einzelnen Gebieten
		{
			if (b->GetBioTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("BIOTECH_BONUS"), b->GetBioTechBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			if (b->GetEnergyTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("ENERGYTECH_BONUS"), b->GetEnergyTechBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			if (b->GetCompTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("COMPUTERTECH_BONUS"), b->GetCompTechBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			if (b->GetPropulsionTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("PROPULSIONTECH_BONUS"), b->GetPropulsionTechBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			if (b->GetConstructionTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("CONSTRUCTIONTECH_BONUS"), b->GetConstructionTechBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			if (b->GetWeaponTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("WEAPONTECH_BONUS"), b->GetWeaponTechBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
		}
		// Wenn alle Geheimdienstdinger belegt sind -> Geheimdienst auf alles
		if (b->GetInnerSecurityBoni() != 0 && b->GetInnerSecurityBoni() == b->GetEconomySpyBoni() 
			&& b->GetInnerSecurityBoni() ==	b->GetEconomySabotageBoni() && b->GetInnerSecurityBoni() == b->GetResearchSpyBoni()
			&& b->GetInnerSecurityBoni() == b->GetResearchSabotageBoni() && b->GetInnerSecurityBoni() == b->GetMilitarySpyBoni()
			&& b->GetInnerSecurityBoni() == b->GetMilitarySabotageBoni())
		{
			s.Format("%s: %i%%",CResourceManager::GetString("COMPLETE_SECURITY_BONUS"), b->GetInnerSecurityBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		else
		{
			if (b->GetEconomySpyBoni() != 0 && b->GetEconomySpyBoni() == b->GetResearchSpyBoni()
				&& b->GetEconomySpyBoni() == b->GetMilitarySpyBoni())
			{
				s.Format("%s: %i%%",CResourceManager::GetString("SPY_BONUS"), b->GetEconomySpyBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			else
			{
				if (b->GetEconomySpyBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("ECONOMY_SPY_BONUS"), b->GetEconomySpyBoni());
					pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
					r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
				}
				if (b->GetResearchSpyBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("RESEARCH_SPY_BONUS"), b->GetResearchSpyBoni());
					pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
					r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
				}
				if (b->GetMilitarySpyBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("MILITARY_SPY_BONUS"), b->GetMilitarySpyBoni());
					pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
					r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
				}
			}
			if (b->GetEconomySabotageBoni() != 0 && b->GetEconomySabotageBoni() == b->GetResearchSabotageBoni()
				&& b->GetEconomySabotageBoni() == b->GetMilitarySabotageBoni())
			{
				s.Format("%s: %i%%",CResourceManager::GetString("SABOTAGE_BONUS"), b->GetEconomySabotageBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			else
			{
				if (b->GetEconomySabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("ECONOMY_SABOTAGE_BONUS"), b->GetEconomySabotageBoni());
					pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
					r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
				}
				if (b->GetResearchSabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("RESEARCH_SABOTAGE_BONUS"), b->GetResearchSabotageBoni());
					pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
					r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
				}
				if (b->GetMilitarySabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("MILITARY_SABOTAGE_BONUS"), b->GetMilitarySabotageBoni());
					pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
					r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
				}
			}
			if (b->GetInnerSecurityBoni() != 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("INNER_SECURITY_BONUS"), b->GetInnerSecurityBoni());
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
		}
		// Ab hier zusätzliche Eigenschaften des Gebäudes
		if (b->GetShipYard() == TRUE)
		{
			pDC->DrawText(CResourceManager::GetString("SHIPYARD"),r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			switch (b->GetMaxBuildableShipSize())
			{
			/*case TRANSPORTER: s = CResourceManager::GetString("TRANSPORTER"); break;
			case COLONYSHIP: s = CResourceManager::GetString("COLONIZESHIP"); break;
			case PROBE: s = CResourceManager::GetString("PROBE"); break;
			case SCOUT: s = CResourceManager::GetString("SCOUT"); break;
			case FIGHTER: s = CResourceManager::GetString("FIGHTER"); break;
			case FRIGATE: s = CResourceManager::GetString("FRIGATE"); break;
			case DESTROYER: s = CResourceManager::GetString("DESTROYER"); break;
			case CRUISER: s = CResourceManager::GetString("CRUISER"); break;
			case HEAVY_DESTROYER: CResourceManager::GetString("HEAVY_DESTROYER"); break;
			case HEAVY_CRUISER: s = CResourceManager::GetString("HEAVY_CRUISER"); break;
			case BATTLESHIP: s = CResourceManager::GetString("BATTLESHIP"); break;
			case FLAGSHIP: s = CResourceManager::GetString("FLAGSHIP"); break;
			default: s = CResourceManager::GetString("NONE");*/
			case 0: s = CResourceManager::GetString("SMALL"); break;
			case 1: s = CResourceManager::GetString("MIDDLE"); break;
			case 2: s = CResourceManager::GetString("BIG"); break;
			case 3: s = CResourceManager::GetString("HUGE"); break;
			default: s = CResourceManager::GetString("HUGE");
			}
			CString ss;
			ss.Format("%s: %s",CResourceManager::GetString("MAX_SHIPSIZE"), s);
			pDC->DrawText(ss,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetShipYardSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIPYARD_EFFICIENCY"), b->GetShipYardSpeed());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetBarrack() == TRUE)
		{
			pDC->DrawText(CResourceManager::GetString("BARRACK") ,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetBarrackSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("BARRACK_EFFICIENCY"), b->GetBarrackSpeed());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetShieldPower() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SHIELDPOWER"), b->GetShieldPower());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetShieldPowerBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIELDPOWER_BONUS"), b->GetShieldPowerBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetShipDefend() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SHIPDEFEND"), b->GetShipDefend());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetShipDefendBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIPDEFEND_BONUS"), b->GetShipDefendBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetGroundDefend() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("GROUNDDEFEND"), b->GetGroundDefend());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetGroundDefendBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("GROUNDDEFEND_BONUS"), b->GetGroundDefendBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetScanPower() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SCANPOWER"), b->GetScanPower());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetScanPowerBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SCANPOWER_BONUS"), b->GetScanPowerBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetScanRange() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SCANRANGE"), b->GetScanRange());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetScanRangeBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SCANRANGE_BONUS"), b->GetScanRangeBoni());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetShipTraining() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SHIPTRAINING"), b->GetShipTraining());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetTroopTraining() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("TROOPTRAINING"), b->GetTroopTraining());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetResistance() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("RESISTANCE"), b->GetResistance());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetAddedTradeRoutes() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("ADDED_TRADEROUTES"), b->GetAddedTradeRoutes());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetIncomeOnTradeRoutes() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("INCOME_ON_TRADEROUTES"), b->GetIncomeOnTradeRoutes());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetShipRecycling() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIPRECYCLING"), b->GetShipRecycling());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetBuildingBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("BUILDING_BUILDSPEED"), b->GetBuildingBuildSpeed());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetUpdateBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("UPGRADE_BUILDSPEED"), b->GetUpdateBuildSpeed());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetShipBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIP_BUILDSPEED"), b->GetShipBuildSpeed());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetTroopBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("TROOP_BUILDSPEED"), b->GetTroopBuildSpeed());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		
		// Ab hier die Vorraussetzungen
		r.SetRect(r.left,r.bottom-10,r.right,r.bottom+15);
		pDC->SetTextColor(RGB(200,200,200));
		pDC->DrawText(CResourceManager::GetString("PREREQUISITES")+":" ,r,DT_TOP | DT_SINGLELINE | DT_CENTER);
		r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		pDC->SetTextColor(oldColor);
		// max X mal von ID pro Imperium
		if (b->GetMaxInEmpire().Number > 0 && b->GetMaxInEmpire().RunningNumber == b->GetRunningNumber())
		{
			if (b->GetMaxInEmpire().Number == 1)
			{
				pDC->DrawText(CResourceManager::GetString("ONCE_PER_EMPIRE"), r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			else
			{
				s.Format("%d",b->GetMaxInEmpire().Number);
				pDC->DrawText(CResourceManager::GetString("MAX_PER_EMPIRE",FALSE,s), r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
		}
		else if (b->GetMaxInEmpire().Number > 0 && b->GetMaxInEmpire().RunningNumber != b->GetRunningNumber())
		{
			s.Format("%d",b->GetMaxInEmpire().Number);
			pDC->DrawText(CResourceManager::GetString("MAX_ID_PER_EMPIRE",FALSE,s,pDoc->GetBuildingName(b->GetMaxInEmpire().RunningNumber)), 
				r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		// max X mal von ID pro System
		if (b->GetMaxInSystem().Number > 0 && b->GetMaxInSystem().RunningNumber == b->GetRunningNumber())
		{
			if (b->GetMaxInSystem().Number == 1)
			{
				pDC->DrawText(CResourceManager::GetString("ONCE_PER_SYSTEM"), r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
			else
			{
				s.Format("%d",b->GetMaxInSystem().Number);
				pDC->DrawText(CResourceManager::GetString("MAX_PER_SYSTEM",FALSE,s),r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
			}
		}
		else if (b->GetMaxInSystem().Number > 0 && b->GetMaxInSystem().RunningNumber != b->GetRunningNumber())
		{
			s.Format("%d",b->GetMaxInSystem().Number);
			pDC->DrawText(CResourceManager::GetString("MAX_ID_PER_SYSTEM",FALSE,s,pDoc->GetBuildingName(b->GetMaxInSystem().RunningNumber)),
				r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		// min X mal von ID pro Imperium
		if (b->GetMinInEmpire().Number > 0)
		{
			s.Format("%d",b->GetMinInEmpire().Number);
			pDC->DrawText(CResourceManager::GetString("MIN_PER_EMPIRE",FALSE,s,pDoc->GetBuildingName(b->GetMinInEmpire().RunningNumber))
				,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		// min X mal von ID pro System
		if (b->GetMinInSystem().Number > 0)
		{
			s.Format("%d",b->GetMinInSystem().Number);
			pDC->DrawText(CResourceManager::GetString("MIN_PER_SYSTEM",FALSE,s,pDoc->GetBuildingName(b->GetMinInSystem().RunningNumber))
				,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetOnlyHomePlanet() == TRUE)
		{
			pDC->DrawText(CResourceManager::GetString("ONLY_HOMEPLANET"),r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetOnlyOwnColony() == TRUE)
		{
			pDC->DrawText(CResourceManager::GetString("ONLY_OWN_COLONY"),r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetOnlyTakenSystem() == TRUE)
		{
			pDC->DrawText(CResourceManager::GetString("ONLY_TAKEN_SYSTEM"),r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetMinHabitants() > 0)
		{
			s.Format("%d",b->GetMinHabitants());
			pDC->DrawText(CResourceManager::GetString("NEED_MIN_HABITANTS",FALSE,s),r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetNeededEnergy() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("ENERGY"),b->GetNeededEnergy());
			pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
		if (b->GetWorker() == TRUE)
		{
			pDC->DrawText(CResourceManager::GetString("NEED_WORKER"),r, DT_TOP | DT_CENTER | DT_SINGLELINE);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+22);
		}
	}
	// Wenn es sich um ein Upgrade handelt:
	else if (RunningNumber < 0)	
	{
		CPoint p = pDoc->GetKO();
		// aktuelles Gebäude, also Gebäude welches geupdated werden soll
		CBuildingInfo* b1 = &pDoc->GetBuildingInfo(pDoc->BuildingInfo.GetAt(RunningNumber*(-1)-1).GetPredecessorID());
		if (b1->GetWorker())
		{
			// Nachfolger des aktuellen Gebäudes
			CBuildingInfo* b2 = &pDoc->GetBuildingInfo(RunningNumber*(-1));
			r.SetRect(rect.left+5,rect.top+5,rect.right,rect.top+60);
			s = CResourceManager::GetString("UPGRADE_FROM_TO",FALSE,pDoc->GetBuildingName(pDoc->BuildingInfo.GetAt(RunningNumber*(-1)-1).GetPredecessorID()),pDoc->GetBuildingName(RunningNumber*(-1)));
			pDC->DrawText(s,r, DT_TOP | DT_LEFT | DT_WORDBREAK);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+25);
			pDC->SetTextColor(RGB(200,200,200));
			pDC->DrawText(CResourceManager::GetString("RELATIVE_PROFIT"), r, DT_CENTER | DT_TOP);
			r.SetRect(r.left,r.bottom,r.right,r.bottom+25);
			pDC->SetTextColor(oldColor);
			if (b1->GetFoodProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(0,0);
				s.Format("%i %s",b2->GetFoodProd()*number-b1->GetFoodProd()*number, CResourceManager::GetString("FOOD"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetIPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(1,0);
				s.Format("%i %s",b2->GetIPProd()*number-b1->GetIPProd()*number,CResourceManager::GetString("INDUSTRY"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetEnergyProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(2,0);
				s.Format("%i %s",b2->GetEnergyProd()*number-b1->GetEnergyProd()*number,CResourceManager::GetString("ENERGY"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetSPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(3,0);
				s.Format("%i %s",b2->GetSPProd()*number-b1->GetSPProd()*number,CResourceManager::GetString("SECURITY"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetFPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(4,0);
				s.Format("%i %s",b2->GetFPProd()*number-b1->GetFPProd()*number,CResourceManager::GetString("RESEARCH"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetTitanProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(5,0);
				s.Format("%i %s",b2->GetTitanProd()*number-b1->GetTitanProd()*number,CResourceManager::GetString("TITAN"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetDeuteriumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(6,0);
				s.Format("%i %s",b2->GetDeuteriumProd()*number-b1->GetDeuteriumProd()*number,CResourceManager::GetString("DEUTERIUM"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetDuraniumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(7,0);
				s.Format("%i %s",b2->GetDuraniumProd()*number-b1->GetDuraniumProd()*number,CResourceManager::GetString("DURANIUM"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetCrystalProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(8,0);
				s.Format("%i %s",b2->GetCrystalProd()*number-b1->GetCrystalProd()*number,CResourceManager::GetString("CRYSTAL"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
			else if (b1->GetIridiumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(9,0);
				s.Format("%i %s",b2->GetIridiumProd()*number-b1->GetIridiumProd()*number,CResourceManager::GetString("IRIDIUM"));
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				r.SetRect(r.left,r.bottom+15,r.right,r.bottom+60);
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				pDC->DrawText(s,r, DT_TOP | DT_CENTER | DT_WORDBREAK);
			}
		}
	}
}

void CSystemMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Wenn wir uns in der Systemansicht befinden
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_BuildMenueMainButtons, temp, FALSE))
	{
		m_bySubMenu = temp;
		if (m_bySubMenu == 0)
		{
			m_bClickedOneBuilding = TRUE;
			m_bClickedOnBuyButton = FALSE;
			m_bClickedOnDeleteButton = FALSE;
		}
		Invalidate(FALSE);
	}
	// Ins Baumenü (Bauliste usw.)
	if (m_bySubMenu == 0)
	{
		// haben wir hier auf einen Button geklickt um in ein anderes Untermenü zu gelangen (Gebäude, Schiffe oder Truppen)
		if (BuildingListButton.PtInRect(point))
		{
			m_iClickedOn = 1;
			m_byStartList = 0;
			m_iWhichSubMenu = 0;
			Invalidate(FALSE);
		}
		else if (ShipyardListButton.PtInRect(point))
		{
			// Wenn man keine Schiffe zur Auswahl hat dann wird wieder auf das normale Gebäudebaumenü umgeschaltet
			if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBuildableShips()->GetSize() == 0)
				return;
			m_iClickedOn = 1;
			m_byStartList = 0;
			m_iWhichSubMenu = 1;
			Invalidate(FALSE);
		}
		else if (TroopListButton.PtInRect(point))
		{
			// Wenn man keine Schiffe zur Auswahl hat dann wird wieder auf das normale Gebäudebaumenü umgeschaltet
			if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBuildableTroops()->GetSize() == 0)
				return;
			m_iClickedOn = 1;
			m_byStartList = 0;
			m_iWhichSubMenu = 2;
			Invalidate(FALSE);
		}

		int RunningNumber;
		for (int i = 1; i < 50; i++)
		{
			if (BuildList[i].rect.PtInRect(point))
			{
				if (BuildList[i].runningNumber < 0)
					RunningNumber = (BuildList[i].runningNumber)*(-1)+1;
				else
					RunningNumber = BuildList[i].runningNumber;
				// Bei linksclick berechnet er die Ressourcen/Industrie
				//pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),BuildList[i].runningNumber);
				m_iClickedOn = i;
				m_bClickedOneBuilding = TRUE;
				m_bClickedOnBuyButton = FALSE;
				m_bClickedOnDeleteButton = FALSE;
				Invalidate();
			}
		}
		CPoint p = pDoc->GetKO();
		// Überprüfen ob wir auf den KaufenButton gedrückt haben und wir noch nix gekauft haben
		// dies geht nicht bei NeverReady wie z.B. Kriegsrecht Aufträgen
		if (BuyButton.PtInRect(point) && pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetWasBuildingBought() == FALSE
			&& pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(0) != 0 && m_bClickedOnDeleteButton == FALSE)
		{
			// Bei Gebäuden nur wenn es nicht ein Auftrag mit NeverReady (z.B. Kriegsrecht) ist)
			if ((AssemblyList[0].runningNumber < 0)
				||					
				(AssemblyList[0].runningNumber > 0 && AssemblyList[0].runningNumber < 10000 && pDoc->GetBuildingInfo(AssemblyList[0].runningNumber).GetNeverReady() == FALSE)
				||
				// Bei Schiffen nur, wenn eine Werft noch aktiv ist
				(AssemblyList[0].runningNumber >= 10000 && AssemblyList[0].runningNumber < 20000 && pDoc->GetSystem(p).GetProduction()->GetShipYard())
				||
				// Bei Truppen nur mit aktiver Kaseren
				(AssemblyList[0].runningNumber >= 20000 && pDoc->GetSystem(p).GetProduction()->GetBarrack()))
			{
				pDoc->m_System[p.x][p.y].GetAssemblyList()->CalculateBuildCosts(pDoc->m_Trade[pDoc->GetPlayersRace()].GetRessourcePriceAtRoundStart());
				m_bClickedOnBuyButton = TRUE;
				//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
				Invalidate();
			}
		}
		// Überprüfen ob wir auf den Okaybutton gedrückt haben um das aktuelle Projekt zu kaufen
		if (OkayButton.PtInRect(point) && m_bClickedOnBuyButton == TRUE)
		{
			int costs = pDoc->m_System[p.x][p.y].GetAssemblyList()->BuyBuilding(pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum());
			if (costs != 0)
			{
				//m_bClickedOneBuilding = TRUE;
				OkayButton.SetRect(0,0,0,0);
				CancelButton.SetRect(0,0,0,0);
				pDoc->m_System[p.x][p.y].GetAssemblyList()->SetWasBuildingBought(TRUE);
				pDoc->m_Empire[pDoc->GetPlayersRace()].SetLatinum(-costs);
				// Die Preise an der Börse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
				// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
				pDoc->m_Trade[pDoc->GetPlayersRace()].BuyRessource(TITAN,pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededTitanInAssemblyList(0),p,pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum(),1);
				pDoc->m_Trade[pDoc->GetPlayersRace()].BuyRessource(DEUTERIUM,pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededDeuteriumInAssemblyList(0),p,pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum(),1);
				pDoc->m_Trade[pDoc->GetPlayersRace()].BuyRessource(DURANIUM,pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededDuraniumInAssemblyList(0),p,pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum(),1);
				pDoc->m_Trade[pDoc->GetPlayersRace()].BuyRessource(CRYSTAL,pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededCrystalInAssemblyList(0),p,pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum(),1);
				pDoc->m_Trade[pDoc->GetPlayersRace()].BuyRessource(IRIDIUM,pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededIridiumInAssemblyList(0),p,pDoc->m_Empire[pDoc->GetPlayersRace()].GetLatinum(),1);
			
				//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				m_bClickedOnBuyButton = FALSE;
				Invalidate();
			}
		}
		// Überprüfen ob wir auf den Cancelbutton gedrückt haben um den Kauf abzubrechen
		if (CancelButton.PtInRect(point))
		{
			//m_bClickedOneBuilding = TRUE;
			OkayButton.SetRect(0,0,0,0);
			CancelButton.SetRect(0,0,0,0);
			//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_bClickedOnBuyButton = FALSE;
			m_bClickedOnDeleteButton = FALSE;
			Invalidate();
		}

		// Überprüfen ob wir auf den Bau_abbrechen Button gedrückt haben
		if (DeleteButton.PtInRect(point) && pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(0) != 0 && m_bClickedOnBuyButton == FALSE)
		{
			m_bClickedOnDeleteButton = TRUE;
			//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
			Invalidate();
		}
		// Überprüfen ob wir auf den Okaybutton gedrückt haben um das aktuelle Projekt abzubrechen
		if (OkayButton.PtInRect(point) && m_bClickedOnDeleteButton == TRUE)
		{
			BYTE race = pDoc->GetPlayersRace();
			// bekommen bei Abbruch die Ressourcen bzw. Teile der Ressourcen wieder
			for (int j = TITAN; j <= DILITHIUM; j++)
			{
				// bestanden Ressourcenrouten, so kann es sein, dass deren Startsysteme einen Anteil oder auch
				// alles zurückbekommen
				long getBackRes = pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j);
				for (int k = 0; k < pDoc->GetEmpire(race)->GetSystemList()->GetSize(); k++)
					if (pDoc->GetEmpire(race)->GetSystemList()->GetAt(k).ko != p)
					{
						CPoint ko = pDoc->GetEmpire(race)->GetSystemList()->GetAt(k).ko;
						for (int l = 0; l < pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetSize(); l++)
							if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetKO() == p)
								if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetKO() == p)
									if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetResource() == j)
										if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetPercent() > 0)
										{
											// sind wir soweit, dann geht ein prozentualer Anteil zurück in das
											// Startsystem der Ressourcenroute
											int back = pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j)
												* pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetPercent() / 100;
											ASSERT(back >= 0);
											pDoc->m_System[ko.x][ko.y].SetRessourceStore(j, back);
											getBackRes -= back;
										}
					}
				pDoc->m_System[p.x][p.y].SetRessourceStore(j, getBackRes);
			}
			// Wenn wir was gekauft hatten, dann bekommen wir die Kaufkosten zurück und die Preise an der Börse
			// regulieren sich wieder auf den Kurs, bevor wir gekauft haben
			if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetWasBuildingBought() == TRUE)
			{
				pDoc->m_Empire[pDoc->GetPlayersRace()].SetLatinum(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetBuildCosts());
				// Die Preise an der Börse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
				// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
				for (int j = TITAN; j <= IRIDIUM; j++)
					pDoc->m_Trade[pDoc->GetPlayersRace()].SellRessource(j, pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j), p, 1);
				pDoc->m_System[p.x][p.y].GetAssemblyList()->SetWasBuildingBought(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
			pDoc->m_System[p.x][p.y].GetAssemblyList()->ClearAssemblyList(p, pDoc->m_System);
			// Nach ClearAssemblyList müssen wir die Funktion CalculateVariables() aufrufen
			pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->m_Empire[pDoc->m_System[p.x][p.y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(),
				pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
			if (AssemblyList[0].runningNumber < 0)
				RunningNumber = AssemblyList[0].runningNumber * (-1);
			else
				RunningNumber = AssemblyList[0].runningNumber;
			// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
			// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt
			// hinzufügen wollen
			if (RunningNumber < 10000 && (AssemblyList[0].runningNumber < 0
				|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0
				|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
			{
				// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
				// weniger in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist
				if (AssemblyList[0].runningNumber > 0)
					pDoc->m_GlobalBuildings.DeleteGlobalBuilding(RunningNumber);
				pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
			}
			m_bClickedOnDeleteButton = FALSE;
			//m_bClickedOneBuilding = TRUE;
			OkayButton.SetRect(0,0,0,0);
			CancelButton.SetRect(0,0,0,0);
			//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
			Invalidate();
		}
		// Überprüfen ob wir auf den Cancelbutton gedrückt haben um das aktuelle Projekt abbrechen wollen
		// -> gleiche Cancelfunktion wie oben bei Kaufen

		// Überprüfen ob wir auf den Gebäudeinfo Button gedrückt haben um die Info zum Gebäude anzuzeigen
		if (BuildingInfoButton.PtInRect(point))
		{
			m_bClickedOnBuildingInfoButton = TRUE;
			m_bClickedOnBuildingDescriptionButton = FALSE;
			CRect r = BuildingDescription;
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
		// Überprüfen ob wir auf den Gebäudebeschreibungs Button gedrückt haben um die Beschreibung zum Gebäude anzuzeigen
		if (BuildingDescriptionButton.PtInRect(point))
		{
			m_bClickedOnBuildingInfoButton = FALSE;
			m_bClickedOnBuildingDescriptionButton = TRUE;
			CRect r = BuildingDescription;
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
	}

	//////////////////////////////////////////////////////////	
	// Ins Arbeiterzuweisungsmenü (normale Gebäude)
	if (m_bySubMenu == 1)
	{
		CPoint p = pDoc->GetKO();
		if (ChangeWorkersButton.PtInRect(point))
		{
			m_bySubMenu = 12;
			//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep2.wav", NULL, SND_FILENAME | SND_ASYNC);
			Invalidate();
		}
		// Wenn wir die Arbeiterzahl erhöhen wollen
		for (int i = 0; i < 5; i++)
		{
			if (PlusButton[i].PtInRect(point))
			{
				// Wenn wir noch freie Arbeiter haben
				if (pDoc->GetSystem(p.x,p.y).GetWorker(11) > 0 && pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i,0) > pDoc->GetSystem(p.x,p.y).GetWorker(i))
				{
					pDoc->m_System[p.x][p.y].SetWorker(i,0,0);	// FoodWorker inkrementieren
					// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddFP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddSP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Variablen berechnen
					pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
					// FP´s und SP´s wieder draufrechnen
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddFP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddSP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
					Invalidate();
					break;
				}
			}
			// Wenn wir die Arbeiterzahl verringern möchten
			if (MinusButton[i].PtInRect(point))
			{
				// Wenn wir noch Arbeiter in dem bestimmten Gebäude haben
				if (pDoc->GetSystem(p.x,p.y).GetWorker(i) > 0)
				{
					pDoc->m_System[p.x][p.y].SetWorker(i,0,1);	// FoodWorker dekrementieren
					// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddFP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddSP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Variablen berechnen
					pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
					// FP´s und SP´s wieder draufrechnen
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddFP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddSP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
					Invalidate();
					break;
				}
			}
		}
		// Wenn wir die Arbeiterzahl erhöhen/verringern wollen und auf den Balken gedrückt haben
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 200; j++)
				if (Timber[i][j].PtInRect(point))
				{
					// Wenn unsere Erhöhung größer der freien Arbeiter ist
					if (j >= (pDoc->GetSystem(p.x,p.y).GetWorker(11)+pDoc->GetSystem(p.x,p.y).GetWorker(i)))
					{
						// Dann setzen wir die Erhöhung auf den max. mgl. Wert
						j = (pDoc->GetSystem(p.x,p.y).GetWorker(11)+pDoc->GetSystem(p.x,p.y).GetWorker(i));
						j--;	// Müssen wir machen
					}
					pDoc->m_System[p.x][p.y].SetWorker(i,j+1,2);
					// Wenn wir ziemlich weit ganz links geklickt haben, dann Arbeiter auf null setzen, werden hier nur um eins dekrementiert
					if (j == 0 && point.x < Timber[i][j].left+3)
						pDoc->m_System[p.x][p.y].SetWorker(i,0,1);
					// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddFP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddSP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Variablen berechnen
					pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
					// FP´s und SP´s wieder draufrechnen
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddFP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddSP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
					Invalidate();
					break;
				}

	}
	// Ins Arbeiterzuweisungsmenü (Ressourcen Gebäude)
	else
	{
		if (m_bySubMenu == 12)
		{
			CPoint p = pDoc->GetKO();	
			if (ChangeWorkersButton.PtInRect(point))
			{
				m_bySubMenu = 1;
				//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep2.wav", NULL, SND_FILENAME | SND_ASYNC);
				Invalidate();
			}
			// Wenn wir die Arbeiterzahl erhöhen wollen
			for (int i = 0; i < 5; i++)
			{
				if (PlusButton[i].PtInRect(point))
				{
					// Wenn wir noch freie Arbeiter haben
					if (pDoc->GetSystem(p.x,p.y).GetWorker(11) > 0 && pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i+5,0) > pDoc->GetSystem(p.x,p.y).GetWorker(i+5))
					{
						pDoc->m_System[p.x][p.y].SetWorker(i+5,0,0);	// FoodWorker inkrementieren
						pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
						//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
						Invalidate();
						break;
					}
				}
				// Wenn wir die Arbeiterzahl verringern möchten
				if (MinusButton[i].PtInRect(point))
				{
					// Wenn wir noch Arbeiter in dem bestimmten Gebäude haben
					if (pDoc->GetSystem(p.x,p.y).GetWorker(i+5) > 0)
					{
						pDoc->m_System[p.x][p.y].SetWorker(i+5,0,1);	// FoodWorker dekrementieren
						pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
						//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
						Invalidate();
						break;
					}
				}
			}
			// Wenn wir die Arbeiterzahl erhöhen/verringern wollen und auf den Balken gedrückt haben
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 200; j++)
					if (Timber[i][j].PtInRect(point))
					{
						// Wenn unsere Erhöhung größer der freien Arbeiter ist
						if (j >= (pDoc->GetSystem(p.x,p.y).GetWorker(11)+pDoc->GetSystem(p.x,p.y).GetWorker(i+5)))
						{
							// Dann setzen wir die Erhöhung auf den max. mgl. Wert
							j = (pDoc->GetSystem(p.x,p.y).GetWorker(11)+pDoc->GetSystem(p.x,p.y).GetWorker(i+5));
							j--;	// Müssen wir machen
						}
						pDoc->m_System[p.x][p.y].SetWorker(i+5,j+1,2);
						// Wenn wir ziemlich weit ganz links geklickt haben, dann Arbeiter auf null setzen, werden hier nur um eins dekrementiert
						if (j == 0 && point.x < Timber[i][j].left+3)
							pDoc->m_System[p.x][p.y].SetWorker(i+5,0,1);
						pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
						//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
						Invalidate();
						break;
					}
		}
	}

	// Ins Energiezuweisungsmenü
	if (m_bySubMenu == 2)
	{
		CPoint p = pDoc->GetKO();
		for (int i = m_iELPage * NOBIEL; i < m_EnergyList.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iELPage * NOBIEL + NOBIEL)
				if (m_EnergyList.GetAt(i).rect.PtInRect(point))
				{										
					if (m_EnergyList.GetAt(i).status == 0)
					{
						if (pDoc->GetSystem(p.x,p.y).GetProduction()->GetEnergyProd() >= pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetNeededEnergy())
							pDoc->m_System[p.x][p.y].SetIsBuildingOnline(m_EnergyList.GetAt(i).index, 1);
					}
					else
						pDoc->m_System[p.x][p.y].SetIsBuildingOnline(m_EnergyList.GetAt(i).index, 0);
					// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddFP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddSP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Variablen berechnen
					pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
					// FP´s und SP´s wieder draufrechnen
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddFP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pDoc->m_Empire[pDoc->GetPlayersRace()].AddSP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Wenn es eine Werft war, die wir an bzw. aus geschaltet haben, dann nochmal schauen ob ich auch
					// noch alle Schiffe bauen kann. Denn wenn die aus ist, dann kann ich keine mehr bauen
					if (pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetShipYard())
						pDoc->m_System[p.x][p.y].CalculateBuildableShips(&pDoc->m_ShipInfoArray,
						pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch(),pDoc->m_Sector[p.x][p.y].GetName(),pDoc->m_Sector[p.x][p.y].GetMinorRace());
					//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
					Invalidate(FALSE);
					break;
				}
		// Wenn ich in dem Menü vor oder zurück blättern kann/will
		if (CRect(1011,190,1074,242).PtInRect(point) && m_iELPage * NOBIEL + NOBIEL < m_EnergyList.GetSize())
		{
			m_iELPage++;
			Invalidate(FALSE);
		}
		else if (CRect(1011,490,1074,542).PtInRect(point) && m_iELPage > 0)
		{
			m_iELPage--;
			Invalidate(FALSE);				
		}
	}

	// Ins Gebäudeübersichtsmenü
	else if (m_bySubMenu == 3)
	{
		// Wenn ich Gebäude abreißen will, mit links auf die Schaltfläche drücken
		CPoint p = pDoc->GetKO();
		for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iBOPage * NOBIOL + NOBIOL)
				if (m_BuildingOverview.GetAt(i).rect.PtInRect(point))
				{
					USHORT ru = m_BuildingOverview.GetAt(i).runningNumber;
					pDoc->m_System[p.x][p.y].SetBuildingDestroy(ru,TRUE);
					CRect r = m_BuildingOverview.GetAt(i).rect;
					CalcDeviceRect(r);
					InvalidateRect(r, FALSE);
					break;;
				}
		// Wenn ich in dem Menü vor oder zurück blättern kann/will
		if (CRect(1011,190,1074,242).PtInRect(point) && m_iBOPage * NOBIOL + NOBIOL < m_BuildingOverview.GetSize())
		{
			m_iBOPage++;
			Invalidate(FALSE);
		}
		else if (CRect(1011,490,1074,542).PtInRect(point) && m_iBOPage > 0)
		{
			m_iBOPage--;
			Invalidate(FALSE);				
		}
	}
	// Ins Systemhandelsmenü
	else if (m_bySubMenu == 4)
	{
		// Button zum Handelsrouten anlegen
		if (CRect(60,600,180,630).PtInRect(point))
		{
			CGalaxyMenuView::IsDrawTradeRoute(TRUE);
			CGalaxyMenuView::GetDrawnTradeRoute()->GenerateTradeRoute(pDoc->GetKO());
			pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pDoc->GetPlayersRace());
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
		}
		// Button zum Anlegen einer Ressourcenroute
		else if (CRect(360,600,480,630).PtInRect(point))
		{
			CGalaxyMenuView::IsDrawResourceRoute(TRUE);
			CGalaxyMenuView::GetDrawnResourceRoute()->GenerateResourceRoute(pDoc->GetKO(), m_byResourceRouteRes);
			pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pDoc->GetPlayersRace());
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
		}
		// Button zum Ändern der Ressource einer Ressourcenroute
		else if (CRect(360,640,480,670).PtInRect(point))
		{
			m_byResourceRouteRes++;
			if (m_byResourceRouteRes == IRIDIUM + 1)
				m_byResourceRouteRes = TITAN;
			CRect r(360,640,480,670);
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
		// Button um die Anzahl, die bei einem Klick verschoben wird zu verändern
		else if (CRect(747,600,867,630).PtInRect(point))
		{
			m_iGlobalStoreageQuantity *= 10;
			if (m_iGlobalStoreageQuantity > 10000)
				m_iGlobalStoreageQuantity = 1;
			CRect r(747,600,867,630);
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
		else
		{
			CPoint p = pDoc->GetKO();
			// einzelne Buttons um womöglich eine Ressourcenroute zu kündigen
			for (int i = 0; i < pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetSize(); i++)
			{
				int j = i + pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize();
				if (CRect(360,260+j*30,480,290+j*30).PtInRect(point))
				{
					// Eine Ressourcenroute kann nur gekündigt werden, wenn sie keine prozentualen Anteile am Bauauftrag
					// besitzt, sprich, wenn sie keine Ressourcen zum Bauauftrag beigetragen hat.
					if (pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetAt(i).GetPercent() == 0)
					{
						pDoc->m_System[p.x][p.y].GetResourceRoutes()->RemoveAt(i--);
						Invalidate(FALSE);
					}
					return;
				}
			}
			// einzelnen Buttons, um Waren zwischen System und globalen Lager hin und her zu schieben
			// Wenn das System blockiert wird kann man nicht mehr Waren ins oder aus dem stellaren Lager
			// nehmen
			if (pDoc->m_System[p.x][p.y].GetBlockade() > NULL)
				return;
			for (int i = TITAN; i <= IRIDIUM; i++)
			{
				// kleine Pfeilbuttons um Waren aus dem globalen Lager ins System zu verschieben
				if (CRect(608,235+i*60,638,265+i*60).PtInRect(point))
				{
					pDoc->m_System[p.x][p.y].SetRessourceStore(i,
						pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->SubRessource(m_iGlobalStoreageQuantity,i,p));
					Invalidate(FALSE);
				}
				// kleine Pfeilbuttons um Waren aus dem System ins globale Lager zu verschieben
				else if (CRect(975,235+i*60,1005,265+i*60).PtInRect(point))
					if (pDoc->m_System[p.x][p.y].GetRessourceStore(i) > 0 || 
						pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetSubResource(i,p) > 0)
					{
						USHORT tempQuantity = m_iGlobalStoreageQuantity;
						if (pDoc->m_System[p.x][p.y].GetRessourceStore(i) < m_iGlobalStoreageQuantity && 
							pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())->GetSubResource(i,p) == 0)
							m_iGlobalStoreageQuantity = (USHORT)pDoc->m_System[p.x][p.y].GetRessourceStore(i);
						USHORT getBack = pDoc->GetGlobalStorage(pDoc->m_System[p.x][p.y].GetOwnerOfSystem())
							->AddRessource(m_iGlobalStoreageQuantity,i,p);
						pDoc->m_System[p.x][p.y].SetRessourceStore(i, (getBack - m_iGlobalStoreageQuantity));
						m_iGlobalStoreageQuantity = tempQuantity;
						Invalidate(FALSE);
					}
			}
		}
	}	

	CMainBaseView::OnLButtonDown(nFlags, point);
}

BOOL CSystemMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	// Wenn wir im Baumenü sind können wir mit dem Mausrad die Bauliste scrollen
	if (m_bySubMenu == 0)
	{
		if (zDelta < 0)
		{
			if (BuildList[m_byStartList+NOEIBL].runningNumber != 0)
				m_byStartList++;
			if (BuildList[m_iClickedOn+1].runningNumber != 0)
				m_iClickedOn++;
			Invalidate();
		}
		else if (zDelta > 0)
		{
			if (m_iClickedOn > 1)
				m_iClickedOn--;
			if (m_byStartList > 0)
				m_byStartList--;
			Invalidate();
		}
	}

	return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSystemMenuView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Wenn wir uns in der Systemansicht befinden
	CPoint p = pDoc->GetKO();
	// Baulisteneintrag hinzufügen
	int RunningNumber;
	BYTE race = pDoc->GetPlayersRace();
	for (int i = 1; i < 50; i++)
	{
		if (BuildList[i].rect.PtInRect(point) && m_bySubMenu == 0)
		{
			if (BuildList[i].runningNumber < 0)
				RunningNumber = (BuildList[i].runningNumber)*(-1);
			else
				RunningNumber = BuildList[i].runningNumber;
			if (pDoc->m_System[p.x][p.y].GetAssemblyList()->MakeEntry(BuildList[i].runningNumber, p, pDoc->m_System))
			{
				// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
				// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt
				// hinzufügen wollen
				if (RunningNumber < 10000 && (BuildList[i].runningNumber < 0
					|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0
					||  pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
				{
					// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
					// mehr in der Bauliste befindet.
					pDoc->m_GlobalBuildings.AddGlobalBuilding(RunningNumber);
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchführen
					if (pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0)
					{
						for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
							for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
								if (pDoc->m_System[x][y].GetOwnerOfSystem() == pDoc->GetBuildingInfo(RunningNumber).GetOwnerOfBuilding())
									pDoc->m_System[x][y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
					}
					// sonst den Baulistencheck nur in dem aktuellen System durchführen
					else
						pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
				
				// Wenn wir den Baueintrag setzen konnten, also hier in der if-Bedingung sind, dann CalculateVariables() aufrufen
				pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->m_Empire[pDoc->m_System[p.x][p.y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
				m_iClickedOn = i;
				Invalidate();
			}
			// Baulisteneintrag konnte aufgrund von Ressourcenmangel nicht gesetzt werden
			else
				pDoc->m_pSoundManager->PlaySound(SNDMGR_SOUND_ERROR);
		}
	}
	// Baulisteneintrag wieder entfernen
	for (int i = 0; i < ALE; i++)
		if (AssemblyList[i].rect.PtInRect(point))
		{
			m_bClickedOnBuyButton = FALSE;
			int RunningNumber;
			if (AssemblyList[i].runningNumber < 0)
				RunningNumber = AssemblyList[i].runningNumber * (-1);
			else
				RunningNumber = AssemblyList[i].runningNumber;
			// Nur beim 0-ten Eintrag in der Bauliste, also der, der oben steht
			if (i == 0 && pDoc->m_System[p.x][p.y].GetAssemblyList()->GetAssemblyListEntry(0) != 0)
			{
				// bekommen bei Abbruch die Ressourcen bzw. Teile der Ressourcen wieder
				for (int j = TITAN; j <= DILITHIUM; j++)
				{
					// bestanden Ressourcenrouten, so kann es sein, dass deren Startsysteme einen Anteil oder auch
					// alles zurückbekommen
					long getBackRes = pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j);
					for (int k = 0; k < pDoc->GetEmpire(race)->GetSystemList()->GetSize(); k++)
						if (pDoc->GetEmpire(race)->GetSystemList()->GetAt(k).ko != p)
						{
							CPoint ko = pDoc->GetEmpire(race)->GetSystemList()->GetAt(k).ko;
							for (int l = 0; l < pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetSize(); l++)
								if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetKO() == p)
									if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetKO() == p)
										if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetResource() == j)
											if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetPercent() > 0)
											{
												// sind wir soweit, dann geht ein prozentualer Anteil zurück in das
												// Startsystem der Ressourcenroute
												int back = pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j)
													* pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetPercent() / 100;
												ASSERT(back >= 0);
												pDoc->m_System[ko.x][ko.y].SetRessourceStore(j, back);
												getBackRes -= back;
											}
						}
					pDoc->m_System[p.x][p.y].SetRessourceStore(j, getBackRes);
				}
				// Wenn wir was gekauft hatten, dann bekommen wir die Kaufkosten zurück
				if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetWasBuildingBought() == TRUE)
				{
					pDoc->m_Empire[pDoc->GetPlayersRace()].SetLatinum(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetBuildCosts());
					// Die Preise an der Börse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
					// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
					for (int j = TITAN; j <= IRIDIUM; j++)
						pDoc->m_Trade[pDoc->GetPlayersRace()].SellRessource(j, pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j), p, 1);
					pDoc->m_System[p.x][p.y].GetAssemblyList()->SetWasBuildingBought(FALSE);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				pDoc->m_System[p.x][p.y].GetAssemblyList()->ClearAssemblyList(p, pDoc->m_System);
				// Nach ClearAssemblyList müssen wir die Funktion CalculateVariables() aufrufen
				pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->m_Empire[pDoc->m_System[p.x][p.y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
				// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
				// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt
				// hinzufügen wollen
				if (RunningNumber < 10000 && (AssemblyList[i].runningNumber < 0
					|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0
					|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
				{
					// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
					// weniger in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist.
					if (AssemblyList[i].runningNumber > 0)
						pDoc->m_GlobalBuildings.DeleteGlobalBuilding(RunningNumber);
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchführen
					if (pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0)
					{
						for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
							for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
								if (pDoc->m_System[x][y].GetOwnerOfSystem() == pDoc->GetBuildingInfo(RunningNumber).GetOwnerOfBuilding())
									pDoc->m_System[x][y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
									

					}
					// sonst den Baulistencheck nur in dem aktuellen System durchführen
					else
						pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
			}
			// Die restlichen Einträge
			// seperat, weil wir die Bauliste anders löschen müssen und auch keine RES zurückbekommen müssen
			else if (pDoc->m_System[p.x][p.y].GetAssemblyList()->GetAssemblyListEntry(0) != 0)
			{
				pDoc->m_System[p.x][p.y].GetAssemblyList()->AdjustAssemblyList(i);
				// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
				// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt
				// hinzufügen wollen
				if (RunningNumber < 10000 && (AssemblyList[i].runningNumber < 0
					|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0
					|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
				{
					// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
					// weniger in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist.
					if (AssemblyList[i].runningNumber > 0)
						pDoc->m_GlobalBuildings.DeleteGlobalBuilding(RunningNumber);
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchführen
					if (pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0)
					{
						for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
							for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
								if (pDoc->m_System[x][y].GetOwnerOfSystem() == pDoc->GetBuildingInfo(RunningNumber).GetOwnerOfBuilding())
									pDoc->m_System[x][y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
									

					}
					// sonst den Baulistencheck nur in dem aktuellen System durchführen
					else
						pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
			}
			//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
			Invalidate();
		}

	CMainBaseView::OnLButtonDblClk(nFlags, point);
}

void CSystemMenuView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Das hier alles nur machen, wenn wir in der System-Ansicht sind
	// Im Gebäudeübersichtsmenü
	if (m_bySubMenu == 3)
	{
		// Wenn ich Gebäude nicht mehr abreißen will, mit rechts auf die Schaltfläche drücken
		CPoint p = pDoc->GetKO();
		for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iBOPage * NOBIOL + NOBIOL)
				if (m_BuildingOverview.GetAt(i).rect.PtInRect(point))
				{
					USHORT ru = m_BuildingOverview.GetAt(i).runningNumber;
					pDoc->m_System[p.x][p.y].SetBuildingDestroy(ru,FALSE);
					CRect r = m_BuildingOverview.GetAt(i).rect;
					CalcDeviceRect(r);
					InvalidateRect(r);
					break;
				}
	}

	CMainBaseView::OnRButtonDown(nFlags, point);
}

void CSystemMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CalcLogicalPoint(point);
	ButtonReactOnMouseOver(point, &m_BuildMenueMainButtons);
	
	CMainBaseView::OnMouseMove(nFlags, point);
}

void CSystemMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// Wenn wir in irgendeinem System sind, können wir mit dem links rechts Pfeil zu einem anderen System wechseln
	CPoint p = pDoc->GetKO();
	if (nChar == VK_RIGHT || nChar == VK_LEFT)
	{
		// an welcher Stelle in der Liste befindet sich das aktuelle System?
		short pos = 0;
		for (int i = 0; i < pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetSize(); i++)
			if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetAt(i).ko == p)
			{
				pos = i;
				break;
			}
		if (nChar == VK_RIGHT)
		{
			pos++;
			if (pos == pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetSize())
				pos = 0;
		}
		else
		{
			pos--;
			if (pos < 0)
				pos = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetUpperBound();
		}
		if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetSize() > 1)
		{
			m_iClickedOn = 1;
			pDoc->SetKO(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetAt(pos).ko.x, pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSystemList()->GetAt(pos).ko.y);
			Invalidate(FALSE);
		}
	}
	// Wenn wir im Baumenü sind können wir mit den Pfeil auf und Pfeil ab Tasten durch die Bauliste scrollen
	if (m_bySubMenu == 0)
	{
		if (nChar == VK_DOWN)
		{
			if (BuildList[m_byStartList+NOEIBL].runningNumber != 0)
				m_byStartList++;
			if (BuildList[m_iClickedOn+1].runningNumber != 0)
				m_iClickedOn++;
			Invalidate();
		}
		else if (nChar == VK_UP)
		{
			if (m_iClickedOn > 1)
				m_iClickedOn--;
			if (m_byStartList > 0)
				m_byStartList--;
			Invalidate();
		}
		// Mit der Entertaste können wir den Auftrag in die Bauliste übernehmen
		else if (nChar == VK_RETURN && m_iClickedOn != 0 && BuildList[m_iClickedOn].runningNumber != 0)
		{
			int i = m_iClickedOn;
			int RunningNumber;
			if (BuildList[i].runningNumber < 0)
				RunningNumber = (BuildList[i].runningNumber)*(-1);
			else
				RunningNumber = BuildList[i].runningNumber;
			if (pDoc->m_System[p.x][p.y].GetAssemblyList()->MakeEntry(BuildList[i].runningNumber, p, pDoc->m_System))
			{
				// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
				// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt
				// hinzufügen wollen
				if (RunningNumber < 10000 && (BuildList[i].runningNumber < 0
					|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0
					||  pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
				{
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchführen
					if (pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire().Number > 0)
					{
						for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
							for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
								if (pDoc->m_System[x][y].GetOwnerOfSystem() == pDoc->GetBuildingInfo(RunningNumber).GetOwnerOfBuilding())
									pDoc->m_System[x][y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
					}
					// sonst den Baulistencheck nur in dem aktuellen System durchführen
					else
						pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
				
				// Wenn wir den Baueintrag setzen konnten, also hier in der if-Bedingung sind, dann CalculateVariables() aufrufen
				pDoc->m_System[p.x][p.y].CalculateVariables(pDoc->m_Empire[pDoc->m_System[p.x][p.y].GetOwnerOfSystem()].GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), CTrade::GetMonopolOwner());
				m_iClickedOn = i;
				Invalidate();
			}
		}
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSystemMenuView::CreateButtons()
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
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_BUILDMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_WORKERSMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_ENERGYMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("BTN_BUILDING_OVERVIEWMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_TRADEMENUE"), fileN, fileI, fileA));
			break;			
		}
	case FERENGI:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttona.jpg";
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_BUILDMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_WORKERSMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_ENERGYMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("BTN_BUILDING_OVERVIEWMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_TRADEMENUE"), fileN, fileI, fileA));
			break;
		}
	case KLINGON:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button3.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button3i.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button3a.jpg";
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_BUILDMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_WORKERSMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_ENERGYMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("BTN_BUILDING_OVERVIEWMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_TRADEMENUE"), fileN, fileI, fileA));
			break;
		}
	case ROMULAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttona.jpg";
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_BUILDMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_WORKERSMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_ENERGYMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("BTN_BUILDING_OVERVIEWMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_TRADEMENUE"), fileN, fileI, fileA));
			break;
		}
	case CARDASSIAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttona.jpg";
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_BUILDMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_WORKERSMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_ENERGYMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("BTN_BUILDING_OVERVIEWMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_TRADEMENUE"), fileN, fileI, fileA));
			break;
		}
	case DOMINION:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttona.jpg";
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_BUILDMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_WORKERSMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_ENERGYMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("BTN_BUILDING_OVERVIEWMENUE"), fileN, fileI, fileA));
			m_BuildMenueMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_TRADEMENUE"), fileN, fileI, fileA));
			break;
		}
	}
}

