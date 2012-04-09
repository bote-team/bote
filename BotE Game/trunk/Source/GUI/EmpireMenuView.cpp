// EmpireMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "SystemMenuView.h"
#include "ResearchMenuView.h"
#include "IntelMenuView.h"
#include "DiplomacyMenuView.h"
#include "EmpireMenuView.h"
#include "GalaxyMenuView.h"
#include "PlanetBottomView.h"
#include "MenuChooseView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"

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
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// Sortierung der Systemliste nach dem Namen der Systeme
	c_arraysort<CArray<SystemViewStruct>,SystemViewStruct>(*pMajor->GetEmpire()->GetSystemList(), sort_asc);
	// Sortierung der Schiffshistory nach dem Namen der Schiffe
	c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_shipname);

	m_iSubMenu = EMPIREVIEW_NEWS;
	m_iWhichNewsButtonIsPressed = NO_TYPE;
	m_iClickedNews = -1;
	m_iClickedSystem = -1;
	m_iClickedShip = -1;
	m_iClickedShipIndex = -1;
	m_bShowAliveShips = TRUE;
	m_iSystemSubMenue = EMPIREVIEW_SYSTEMS_NORMAL;
}

// CEmpireMenuView drawing
void CEmpireMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
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

	if (m_iSubMenu == EMPIREVIEW_NEWS)
		DrawEmpireNewsMenue(&g);
	else if (m_iSubMenu == EMPIREVIEW_SYSTEMS)
		DrawEmpireSystemMenue(&g);
	else if (m_iSubMenu == EMPIREVIEW_SHIPS)
		DrawEmpireShipMenue(&g);
	else if (m_iSubMenu == EMPIREVIEW_DEMOGRAPHICS)
		DrawEmpireDemographicsMenue(&g);
	else if (m_iSubMenu == EMPIREVIEW_TOP5)
		DrawEmpireTop5Menue(&g);
	else if (m_iSubMenu == EMPIREVIEW_VICTORY)
		DrawEmpireVictoryMenue(&g);

	g.ReleaseHDC(pDC->GetSafeHdc());
}

void CEmpireMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	m_iSubMenu = EMPIREVIEW_NEWS;
	m_iWhichNewsButtonIsPressed = NO_TYPE;
	m_iClickedNews = -1;
	m_iClickedSystem = -1;
	m_iClickedShip = -1;
	m_iClickedShipIndex = -1;
	m_bShowAliveShips = TRUE;
	m_iSystemSubMenue = EMPIREVIEW_SYSTEMS_NORMAL;
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CEmpireMenuView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CreateButtons();

	CString sPrefix = pMajor->GetPrefix();
	
	bg_newsovmenu			= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "newsovmenu.boj");
	bg_systemovmenu			= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "systemovmenu.boj");
	bg_shipovmenu			= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "shipovmenu.boj");
	bg_demographicsmenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "demomenu.boj");
	bg_top5menu				= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "top5menu.boj");
	bg_victorymenu			= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "victorymenu.boj");
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
void CEmpireMenuView::DrawEmpireNewsMenue(Graphics* g)
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

	if (bg_newsovmenu)
		g->DrawImage(bg_newsovmenu, 0, 0, 1075, 750);
	
	CString s;
	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	
	// Es gehen nur 21 Einträge auf die Seite, deshalb muss abgebrochen werden
	// Wenn noch keine News angeklickt wurde, es aber News gibt, dann die erste in der Reihe markieren
	if (m_iClickedNews == -1 && pMajor->GetEmpire()->GetMessages()->GetSize() > 0)
	{
		m_iClickedNews = 0;
		m_iOldClickedNews = 0;
	}
	int j = 0;
	short counter = m_iClickedNews - 20 + m_iOldClickedNews;
	short oldClickedNews = m_iClickedNews;
	for (int i = 0; i < pMajor->GetEmpire()->GetMessages()->GetSize(); i++)
		// nur Nachrichten anzeigen, dessen Typ wir auch gewählt haben
		if (m_iWhichNewsButtonIsPressed == pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() ||
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
					fontBrush.SetColor(markColor);
					// Markierung worauf wir geklickt haben
					g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(100,140+j*25,875,25));
					g->DrawLine(&Gdiplus::Pen(penColor), 100, 140+j*25, 975, 140+j*25);
					g->DrawLine(&Gdiplus::Pen(penColor), 100, 165+j*25, 975, 165+j*25);					
				}
				else
				{
					switch (pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType())
					{
						case ECONOMY:  fontBrush.SetColor(Color(0,150,0));		break;
						case RESEARCH: fontBrush.SetColor(Color(50,75,255));	break;
						case SECURITY: fontBrush.SetColor(Color(155,25,255));	break;
						case DIPLOMACY:fontBrush.SetColor(Color(255,220,0));	break;
						case MILITARY: fontBrush.SetColor(Color(255,0,0));		break;
						default: fontBrush.SetColor(normalColor);
					}
				}
				s = pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageString();
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,140+j*25,875,25), &fontFormat, &fontBrush);
				j++;
			}
		}
	m_iClickedNews = oldClickedNews;
	
	// Schriftart für große Buttons laden
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush btnBrush(btnColor);
	// Buttons am oberen Bildrand zeichnen, womit wir die Nachrichtenart verändern können, die angezeigt wird
	DrawGDIButtons(g, &m_EmpireNewsFilterButtons, m_iWhichNewsButtonIsPressed, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
	// Buttons am unteren Bildrand zeichnen
	DrawGDIButtons(g, &m_EmpireNewsButtons, m_iSubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
		
	// "Nachrichten und Informationen" in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("NEWS_MENUE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Menüs zur Anzeige aller Systeme im Imperium
/////////////////////////////////////////////////////////////////////////////////////////
void CEmpireMenuView::DrawEmpireSystemMenue(Graphics* g)
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
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	if (bg_systemovmenu)
		g->DrawImage(bg_systemovmenu, 0, 0, 1075, 750);
	
	CString s;
	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);		
		
	// Wenn noch kein System angeklickt wurde, es aber Systeme gibt, dann das erste in der Liste markieren
	if (m_iClickedSystem == -1 && pMajor->GetEmpire()->GetSystemList()->GetSize() > 0)
	{
		m_iClickedSystem = 0;
		m_iOldClickedSystem = 0;
	}
	short j = 0;
	short counter = m_iClickedSystem - 20 + m_iOldClickedSystem;
	short oldClickedSystem = m_iClickedSystem;
	// Es gehen nur 21 Einträge auf die Seite, deshalb muss abgebrochen werden
	for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
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
				fontBrush.SetColor(markColor);
				// Markierung worauf wir geklickt haben
				g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(46,140+j*25,984,25));
				g->DrawLine(&Gdiplus::Pen(penColor), 46, 140+j*25, 1030, 140+j*25);
				g->DrawLine(&Gdiplus::Pen(penColor), 46, 165+j*25, 1030, 165+j*25);
			}
			else
				fontBrush.SetColor(normalColor);
			CPoint KO = pMajor->GetEmpire()->GetSystemList()->GetAt(i).ko;
			s.Format("%c%i",(char)(KO.y+97),KO.x+1);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(55,140+j*25,75,25), &fontFormat, &fontBrush);
			s = pMajor->GetEmpire()->GetSystemList()->GetAt(i).name;
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(130,140+j*25,140,25), &fontFormat, &fontBrush);			
			
			// soll die normale Systemüberischt angezeigt werden
			if (m_iSystemSubMenue == EMPIREVIEW_SYSTEMS_NORMAL)
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
					fontBrush.SetColor(Color(0,250,0));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 154)
				{
					s = CResourceManager::GetString("LOYAL");
					fontBrush.SetColor(Color(20,150,20));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 130)
				{
					s = CResourceManager::GetString("PLEASED");
					fontBrush.SetColor(Color(20,150,100));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 99)
				{
					s = CResourceManager::GetString("SATISFIED");
					fontBrush.SetColor(Color(150,150,200));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 75)
				{
					s = CResourceManager::GetString("APATHETIC");
					fontBrush.SetColor(Color(160,160,160));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 49)
				{
					s = CResourceManager::GetString("ANGRY");
					fontBrush.SetColor(Color(200,100,50));
				}
				else if (pDoc->GetSystem(KO.x,KO.y).GetMoral() > 29)
				{
					s = CResourceManager::GetString("FURIOUS");
					fontBrush.SetColor(Color(210,80,50));
				}
				else
				{
					s = CResourceManager::GetString("REBELLIOUS");
					fontBrush.SetColor(Color(255,0,0));
				}
				s.Format("%s (%d)",s,pDoc->GetSystem(KO.x,KO.y).GetMoral());//Wert hinzufügen
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,140+j*25,130,25), &fontFormat, &fontBrush);
				
				// Nahrungsproduktion anzeigen
				int food = pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetFoodProd();
				s.Format("%d",food);
				if (food < 0)
					fontBrush.SetColor(Color(250,0,0));
				else
					fontBrush.SetColor(Color(0,250,0));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,140+j*25,80,25), &fontFormat, &fontBrush);

				// Wenn System markiert ist, dann andere Schriftfarbe wählen
				if (j == m_iClickedSystem)
					fontBrush.SetColor(markColor);
				else
					fontBrush.SetColor(normalColor);
				
				// Nahrungslager anzeigen
				s.Format("%d",pDoc->GetSystem(KO).GetFoodStore());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(480,140+j*25,100,25), &fontFormat, &fontBrush);
				
				// Industrieproduktion anzeigen
				int ip = pDoc->GetSystem(KO).GetProduction()->GetIndustryProd();
				s.Format("%d",ip);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(580,140+j*25,100,25), &fontFormat, &fontBrush);

				//Creditproduktionanzeigen
				int cp = pDoc->GetSystem(KO).GetProduction()->GetCreditsProd();
				s.Format("%d",cp);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(680,140+j*25,100,25), &fontFormat, &fontBrush);
								
				// Bauauftrag anzeigen inkl. Anzeige der noch nötigen Runden bis Bauauftrag fertig ist
				short id = pDoc->GetSystem(KO).GetAssemblyList()->GetAssemblyListEntry(0);
				int RoundToBuild = 0;
				if (id == 0)
					s = "";
				else if (id > 0 && id < 10000)
				{
					if (ip > 0 && !pDoc->GetBuildingInfo(id).GetNeverReady())
					{
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(KO).GetAssemblyList()->GetNeededIndustryInAssemblyList(0))/((float)ip
							* (100+pDoc->GetSystem(KO).GetProduction()->GetBuildingBuildSpeed())/100));
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
					if (pDoc->GetSystem(KO).GetProduction()->GetShipYardEfficiency() > 0 && ip > 0)
					{
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(KO).GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
								/((float)ip * pDoc->GetSystem(KO).GetProduction()->GetShipYardEfficiency() / 100
									* (100+pDoc->GetSystem(KO).GetProduction()->GetShipBuildSpeed())/100));
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
					if (pDoc->GetSystem(KO).GetProduction()->GetBarrackEfficiency() > 0 && ip > 0)
					{
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(KO).GetAssemblyList()->GetNeededIndustryInAssemblyList(0))
								/((float)ip * pDoc->GetSystem(KO).GetProduction()->GetBarrackEfficiency() / 100
									* (100+pDoc->GetSystem(KO).GetProduction()->GetTroopBuildSpeed())/100));
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
						RoundToBuild = (int)ceil((float)(pDoc->GetSystem(KO).GetAssemblyList()->GetNeededIndustryInAssemblyList(0))/((float)ip
							* (100+pDoc->GetSystem(KO).GetProduction()->GetUpdateBuildSpeed())/100));
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
				if (pDoc->GetSystem(KO).GetAutoBuild())
				{
					s.Insert(0, CResourceManager::GetString("AUTOBUILD") + ": ");
					fontBrush.SetColor(Color::Red);					
				}

				fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(780,140+j*25,345,25), &fontFormat, &fontBrush);
				fontFormat.SetTrimming(StringTrimmingNone);
				fontBrush.SetColor(normalColor);
			}
			// andernfalls wird hier die Systemressourcenübersicht angezeigt
			else if (m_iSystemSubMenue == EMPIREVIEW_SYSTEMS_RESOURCE)
			{
				// prüfen ob irgendein Verteiler steht
				bool bDist[DERITIUM + 1] = {false};
				int nLastID = -1;
				for (int l = 0; l < pDoc->GetSystem(KO).GetAllBuildings()->GetSize(); l++)
				{
					int nID = pDoc->GetSystem(KO).GetAllBuildings()->GetAt(l).GetRunningNumber();
					if (nID != nLastID)
						nLastID = nID;
					else
						continue;

					CBuildingInfo* pBuildingInfo = &pDoc->GetBuildingInfo(nID);
					for (int k = TITAN; k <= DERITIUM; k++)
						if (pBuildingInfo->GetResourceDistributor(k))
							bDist[k] = true;
				}				

				for (int k = TITAN; k <= DERITIUM; k++)
				{
					fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);					
					if (bDist[k])
					{
						Bitmap* graphic = NULL;
						// kleiens Ressourcensymbol vor dem Wert zeichnen
						switch (k)
						{
						case TITAN:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop"); break;
						case DEUTERIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop"); break;
						case DURANIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop"); break;
						case CRYSTAL:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop"); break;
						case IRIDIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop"); break;
						case DERITIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\Deritium.bop"); break;
						}
						if (graphic)
							g->DrawImage(graphic, 245+k*120, 145+j*25, 20 , 16);
					}
					
					s.Format("%d", pDoc->GetSystem(KO).GetResourceStore(k));
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270+k*120,140+j*25,120,25), &fontFormat, &fontBrush);
					fontFormat.SetTrimming(StringTrimmingNone);
				}
			}
			else if (m_iSystemSubMenue == EMPIREVIEW_SYSTEMS_DEFENCE)
			{
				// Verteidigungsgebäude ermitteln und anzeigen
				int nShieldDefenceCount			= 0;
				int nShieldDefenceOnlineCount	= 0;
				int nShipDefenceCount			= 0;
				int nShipDefenceOnlineCount		= 0;
				int nGroundDefenceCount			= 0;
				int nGroundDefenceOnlineCount	= 0;
				for (int l = 0; l < pDoc->GetSystem(KO).GetAllBuildings()->GetSize(); l++)
				{
					int nID = pDoc->GetSystem(KO).GetAllBuildings()->GetAt(l).GetRunningNumber();
					CBuildingInfo* pBuildingInfo = &pDoc->GetBuildingInfo(nID);
					if (pBuildingInfo->GetShieldPower() > 0 || pBuildingInfo->GetShieldPowerBoni())
					{
						nShieldDefenceCount++;
						if (pDoc->GetSystem(KO).GetAllBuildings()->GetAt(l).GetIsBuildingOnline() || pBuildingInfo->GetNeededEnergy() == 0 || pBuildingInfo->GetAllwaysOnline())
							nShieldDefenceOnlineCount++;
					}
					if (pBuildingInfo->GetShipDefend() > 0 || pBuildingInfo->GetShipDefendBoni() > 0)
					{
						nShipDefenceCount++;
						if (pDoc->GetSystem(KO).GetAllBuildings()->GetAt(l).GetIsBuildingOnline() || pBuildingInfo->GetNeededEnergy() == 0 || pBuildingInfo->GetAllwaysOnline())
							nShipDefenceOnlineCount++;
					}
					if (pBuildingInfo->GetGroundDefend() > 0 || pBuildingInfo->GetGroundDefendBoni() > 0)
					{
						nGroundDefenceCount++;
						if (pDoc->GetSystem(KO).GetAllBuildings()->GetAt(l).GetIsBuildingOnline() || pBuildingInfo->GetNeededEnergy() == 0 || pBuildingInfo->GetAllwaysOnline())
							nGroundDefenceOnlineCount++;
					}
				}

				// Anzahl Truppen anzeigen
				s.Format("%d", pDoc->GetSystem(KO).GetTroops()->GetSize());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,140+j*25,100,25), &fontFormat, &fontBrush);
				// Schildstärke anzeigen
				s.Format("%d (%d/%d)", pDoc->GetSystem(KO).GetProduction()->GetShieldPower(), nShieldDefenceOnlineCount, nShieldDefenceCount);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(370,140+j*25,150,25), &fontFormat, &fontBrush);
				// Schiffsabwehr anzeigen
				s.Format("%d (%d/%d)", pDoc->GetSystem(KO).GetProduction()->GetShipDefend(), nShipDefenceOnlineCount, nShipDefenceCount);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(520,140+j*25,150,25), &fontFormat, &fontBrush);
				// Bodenabwehr anzeigen
				s.Format("%d (%d/%d)", pDoc->GetSystem(KO).GetProduction()->GetGroundDefend(), nGroundDefenceOnlineCount, nGroundDefenceCount);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(670,140+j*25,150,25), &fontFormat, &fontBrush);
				// Scanstärke anzeigen
				s.Format("%d", pDoc->GetSystem(KO).GetProduction()->GetScanPower());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(820,140+j*25,150,25), &fontFormat, &fontBrush);				
			}
			j++;
		}
	}
	m_iClickedSystem = oldClickedSystem;

	// Buttons am unteren Bildrand zeichnen, womit wir die Menüs umschalten können
	// Schriftart für große Buttons laden
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush btnBrush(btnColor);
	// Buttons am oberen Bildrand zeichnen
	DrawGDIButtons(g, &m_EmpireSystemFilterButtons, m_iSystemSubMenue, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
	// Buttons am unteren Bildrand zeichnen
	DrawGDIButtons(g, &m_EmpireNewsButtons, m_iSubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
		
	// Tabellenüberschriften zeichnen
	fontBrush.SetColor(markColor);
	Gdiplus::Font font(fontName.AllocSysString(), fontSize);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
	g->DrawString(CResourceManager::GetString("SECTOR").AllocSysString(), -1, &font, RectF(50,110,80,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("NAME").AllocSysString(), -1, &font, RectF(130,110,140,30), &fontFormat, &fontBrush);
	if (m_iSystemSubMenue == EMPIREVIEW_SYSTEMS_NORMAL)
	{
		g->DrawString(CResourceManager::GetString("MORAL").AllocSysString(), -1, &font, RectF(270,110,130,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("FOOD").AllocSysString(), -1, &font, RectF(400,110,80,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("STORAGE").AllocSysString(), -1, &font, RectF(480,110,100,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("INDUSTRY").AllocSysString(), -1, &font, RectF(580,110,100,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("CREDITS").AllocSysString(), -1, &font, RectF(680,110,100,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("JOB").AllocSysString(), -1, &font, RectF(780,110,345,30), &fontFormat, &fontBrush);
	}
	else if (m_iSystemSubMenue == EMPIREVIEW_SYSTEMS_RESOURCE)
	{
		g->DrawString(CResourceManager::GetString("TITAN").AllocSysString(), -1, &font, RectF(270,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("DEUTERIUM").AllocSysString(), -1, &font, RectF(390,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("DURANIUM").AllocSysString(), -1, &font, RectF(510,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("CRYSTAL").AllocSysString(), -1, &font, RectF(630,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("IRIDIUM").AllocSysString(), -1, &font, RectF(750,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("DERITIUM").AllocSysString(), -1, &font, RectF(870,110,120,30), &fontFormat, &fontBrush);		
	}
	else if (m_iSystemSubMenue == EMPIREVIEW_SYSTEMS_DEFENCE)
	{
		g->DrawString(CResourceManager::GetString("TROOPS").AllocSysString(), -1, &font, RectF(270,110,100,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("SHIELDPOWER").AllocSysString(), -1, &font, RectF(370,110,150,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("SHIPDEFEND").AllocSysString(), -1, &font, RectF(520,110,150,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("GROUNDDEFEND").AllocSysString(), -1, &font, RectF(670,110,150,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("SCANPOWER").AllocSysString(), -1, &font, RectF(820,110,150,30), &fontFormat, &fontBrush);
	}
	fontFormat.SetTrimming(StringTrimmingNone);
	fontFormat.SetAlignment(StringAlignmentCenter);	
	// "Übersicht aller Systeme" oben in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("SYSTEM_OVERVIEW_MENUE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);	
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Menüs zur Anzeige aller Schiffe des Imperium
/////////////////////////////////////////////////////////////////////////////////////////
void CEmpireMenuView::DrawEmpireShipMenue(Graphics* g)
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
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	if (bg_shipovmenu)
		g->DrawImage(bg_shipovmenu, 0, 0, 1075, 750);
	
	CString s;
	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	
	// Wenn noch kein Schiff angeklickt wurde, es aber Schiffe gibt, dann das erste in der Liste markieren
	if (m_iClickedShip == -1 && pMajor->GetShipHistory()->GetSizeOfShipHistory() > 0)
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
	for (UINT i = 0; i < pMajor->GetShipHistory()->GetSizeOfShipHistory(); i++)
	{
		if (pMajor->GetShipHistory()->IsShipAlive(i))
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
					fontBrush.SetColor(markColor);
					// Markierung worauf wir geklickt haben
					g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(46,140+j*25,984,25));
					g->DrawLine(&Gdiplus::Pen(penColor), 46, 140+j*25, 1030, 140+j*25);
					g->DrawLine(&Gdiplus::Pen(penColor), 46, 165+j*25, 1030, 165+j*25);
				}
				else
					fontBrush.SetColor(normalColor);
				
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strShipName;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(55,140+j*25,170,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strShipType;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(225,140+j*25,150,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strShipClass;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(375,140+j*25,120,25), &fontFormat, &fontBrush);
				s.Format("%d",pMajor->GetShipHistory()->GetShipHistory(i)->m_iBuildRound);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(500,140+j*25,65,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strSectorName;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(560,140+j*25,115,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strCurrentSector;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(675,140+j*25,125,25), &fontFormat, &fontBrush);
				s.Format("%d",pMajor->GetShipHistory()->GetShipHistory(i)->m_iExperiance);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(795,140+j*25,50,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strCurrentTask;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(835,140+j*25,175,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strTarget;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(920,140+j*25,175,25), &fontFormat, &fontBrush);
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
					fontBrush.SetColor(markColor);
					// Markierung worauf wir geklickt haben
					g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(46,140+k*25,984,25));
					g->DrawLine(&Gdiplus::Pen(penColor), 46, 140+k*25, 1030, 140+k*25);
					g->DrawLine(&Gdiplus::Pen(penColor), 46, 165+k*25, 1030, 165+k*25);
				}
				else
					fontBrush.SetColor(normalColor);

				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strShipName;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(55,140+k*25,170,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strShipType;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(225,140+k*25,150,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strShipClass;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(375,140+k*25,120,25), &fontFormat, &fontBrush);
				s.Format("%d",pMajor->GetShipHistory()->GetShipHistory(i)->m_iDestroyRound);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(495,140+k*25,65,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strSectorName;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(560,140+k*25,115,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strKindOfDestroy;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(675,140+k*25,175,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strCurrentTask;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(850,140+k*25,175,25), &fontFormat, &fontBrush);
				
			}
			k++;
		}
	}
	m_iClickedShip = oldClickedShip;

	fontFormat.SetTrimming(StringTrimmingNone);

	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush btnBrush(btnColor);
	// Buttons am oberen Bildrand zeichnen, womit wir die Anzeige der jetzigen Schiffe und der verlorenen Schiffe verändern
	// können
	DrawGDIButtons(g, &m_EmpireShipsFilterButtons, !m_bShowAliveShips, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
	// Buttons am unteren Bildrand zeichnen
	DrawGDIButtons(g, &m_EmpireNewsButtons, m_iSubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
	
	// Tabellenüberschriften zeichnen
	fontBrush.SetColor(markColor);
	Gdiplus::Font font(fontName.AllocSysString(), fontSize);
	g->DrawString(CResourceManager::GetString("NAME").AllocSysString(), -1, &font, RectF(50,110,175,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("TYPE").AllocSysString(), -1, &font, RectF(225,110,150,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("CLASS").AllocSysString(), -1, &font, RectF(375,110,120,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("SYSTEM").AllocSysString(), -1, &font, RectF(560,110,115,30), &fontFormat, &fontBrush);
	if (m_bShowAliveShips)
	{
		g->DrawString(CResourceManager::GetString("CONSTRUCT").AllocSysString(), -1, &font, RectF(495,110,65,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("CURRENT_PLACE").AllocSysString(), -1, &font, RectF(675,110,115,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("EXPERIANCE_SHORT").AllocSysString(), -1, &font, RectF(790,110,60,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("STATUS").AllocSysString(), -1, &font, RectF(835,110,175,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("TARGET").AllocSysString(), -1, &font, RectF(915,110,175,30), &fontFormat, &fontBrush);
	}
	else
	{
		g->DrawString(CResourceManager::GetString("LOST").AllocSysString(), -1, &font, RectF(495,110,65,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("EVENT").AllocSysString(), -1, &font, RectF(675,110,115,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("STATUS").AllocSysString(), -1, &font, RectF(850,110,175,30), &fontFormat, &fontBrush);
	}
	CFontLoader::GetGDIFontColor(pMajor, 4, normalColor);
	fontBrush.SetColor(normalColor);
	
	s.Format("%s: %d",CResourceManager::GetString("SHIPS_TOTAL"),numberLive);
	g->DrawString(s.AllocSysString(), -1, &font, RectF(280,70,300,40), &fontFormat, &fontBrush);
	
	s.Format("%s: %d",CResourceManager::GetString("SHIPS_LOST"),numberDead);
	g->DrawString(s.AllocSysString(), -1, &font, RectF(675,70,350,40), &fontFormat, &fontBrush);
	
	// "Übersicht aller Schiffe" oben in der Mitte zeichnen
	fontFormat.SetAlignment(StringAlignmentCenter);
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("SHIP_OVERVIEW_MENUE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);	
}

void CEmpireMenuView::DrawEmpireDemographicsMenue(Gdiplus::Graphics *g)
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
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	if (bg_shipovmenu)
		g->DrawImage(bg_demographicsmenu, 0, 0, 1075, 750);
	
	CString s;

	// Tabellenüberschriften zeichnen
	fontBrush.SetColor(markColor);
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CResourceManager::GetString("DEMO_BSP").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,250,165,25), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("DEMO_PRODUCTIVITY").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,320,165,25), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("DEMO_MILITARY").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,390,165,25), &fontFormat, &fontBrush);	
	g->DrawString(CResourceManager::GetString("DEMO_SCIENCE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,460,165,25), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("DEMO_HAPPINESS").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,530,165,25), &fontFormat, &fontBrush);
	
	// Pokalgrafiken laden
	Bitmap* trophy1 = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\trophy1.bop");
	Bitmap* trophy2 = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\trophy2.bop");
	Bitmap* trophy3 = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\trophy3.bop");

	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentCenter);
	float fMark = 0.0f;
	int nPlace = 1;
	float fValue, fAverage, fFirst, fLast = 0.0;
	pDoc->GetStatistics()->GetDemographicsBSP(pMajor->GetRaceID(), nPlace, fValue, fAverage, fFirst, fLast);	
	s.Format("%d", nPlace);
	fMark += nPlace;
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(330,250,70,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fValue * 5.0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,250,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fFirst * 5.0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(525,250,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fAverage * 5.0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(650,250,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fLast * 5.0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,250,125,25), &fontFormat, &fontBrush);
	
	// Pokal zeichnen
	if (nPlace == 1 && trophy1)
		g->DrawImage(trophy1, 320, 247, 30, 30);
	else if (nPlace == 2 && trophy2)
		g->DrawImage(trophy2, 320, 247, 30, 30);
	else if (nPlace == 3 && trophy3)
		g->DrawImage(trophy3, 320, 247, 30, 30);
	
	pDoc->GetStatistics()->GetDemographicsProductivity(pMajor->GetRaceID(), nPlace, fValue, fAverage, fFirst, fLast);
	s.Format("%d", nPlace);
	fMark += nPlace;
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(330,320,70,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fValue);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,320,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fFirst);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(525,320,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fAverage);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(650,320,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fLast);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,320,125,25), &fontFormat, &fontBrush);
	
	// Pokal zeichnen
	if (nPlace == 1 && trophy1)
		g->DrawImage(trophy1, 320, 317, 30, 30);
	else if (nPlace == 2 && trophy2)
		g->DrawImage(trophy2, 320, 317, 30, 30);
	else if (nPlace == 3 && trophy3)
		g->DrawImage(trophy3, 320, 317, 30, 30);
		
	pDoc->GetStatistics()->GetDemographicsMilitary(pMajor->GetRaceID(), nPlace, fValue, fAverage, fFirst, fLast);
	s.Format("%d", nPlace);
	fMark += nPlace;
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(330,390,70,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fValue / 10);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,390,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fFirst / 10);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(525,390,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fAverage / 10);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(650,390,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fLast / 10);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,390,125,25), &fontFormat, &fontBrush);
	
	// Pokal zeichnen
	if (nPlace == 1 && trophy1)
		g->DrawImage(trophy1, 320, 387, 30, 30);
	else if (nPlace == 2 && trophy2)
		g->DrawImage(trophy2, 320, 387, 30, 30);
	else if (nPlace == 3 && trophy3)
		g->DrawImage(trophy3, 320, 387, 30, 30);
			
	pDoc->GetStatistics()->GetDemographicsResearch(pMajor->GetRaceID(), nPlace, fValue, fAverage, fFirst, fLast);
	s.Format("%d", nPlace);
	fMark += nPlace;
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(330,460,70,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fValue * 2.0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,460,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fFirst * 2.0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(525,460,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fAverage * 2.0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(650,460,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fLast * 2.0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,460,125,25), &fontFormat, &fontBrush);
	
	// Pokal zeichnen
	if (nPlace == 1 && trophy1)
		g->DrawImage(trophy1, 320, 457, 30, 30);
	else if (nPlace == 2 && trophy2)
		g->DrawImage(trophy2, 320, 457, 30, 30);
	else if (nPlace == 3 && trophy3)
		g->DrawImage(trophy3, 320, 457, 30, 30);

	pDoc->GetStatistics()->GetDemographicsMoral(pMajor->GetRaceID(), nPlace, fValue, fAverage, fFirst, fLast);
	s.Format("%d", nPlace);
	fMark += nPlace;
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(330,530,70,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fValue);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,530,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fFirst);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(525,530,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fAverage);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(650,530,125,25), &fontFormat, &fontBrush);
	s.Format("%.0lf", fLast);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,530,125,25), &fontFormat, &fontBrush);
	
	// Pokal zeichnen
	if (nPlace == 1 && trophy1)
		g->DrawImage(trophy1, 320, 527, 30, 30);
	else if (nPlace == 2 && trophy2)
		g->DrawImage(trophy2, 320, 527, 30, 30);
	else if (nPlace == 3 && trophy3)
		g->DrawImage(trophy3, 320, 527, 30, 30);
	
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush btnBrush(btnColor);
	// Buttons am unteren Bildrand zeichnen
	DrawGDIButtons(g, &m_EmpireNewsButtons, m_iSubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);

	// Bereiche zeichnen
	fontBrush.SetColor(markColor);
	Gdiplus::Font font(fontName.AllocSysString(), fontSize);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);	
	fontFormat.SetAlignment(StringAlignmentCenter);	
	g->DrawString(CResourceManager::GetString("RANK").AllocSysString(), -1, &font, RectF(330,195,70,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("VALUE").AllocSysString(), -1, &font, RectF(400,195,125,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("FIRST_RANK").AllocSysString(), -1, &font, RectF(525,195,125,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("AVERAGE").AllocSysString(), -1, &font, RectF(650,195,125,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("LAST_RANK").AllocSysString(), -1, &font, RectF(775,195,125,30), &fontFormat, &fontBrush);

	// "Demografie" oben in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Punktestand/Note oben in der Mitte groß zeichnen
	fMark /= 5;
	s.Format("%s: %.1lf", CResourceManager::GetString("RATING"), fMark);
	g->DrawString(s.AllocSysString(), -1, &font, RectF(0,65,m_TotalSize.cx,50), &fontFormat, &fontBrush);
	
	// Pokal für Gesamtbewertung zeichnen
	if (fMark < 1.5f && trophy1)
	{
		g->DrawImage(trophy1, 395, 65, 50, 45);
		g->DrawImage(trophy1, 625, 65, 50, 45);
	}
	else if (fMark < 2.0f && trophy2)
	{
		g->DrawImage(trophy2, 395, 65, 50, 45);
		g->DrawImage(trophy2, 625, 65, 50, 45);
	}
	else if (fMark < 2.5f && trophy3)
	{
		g->DrawImage(trophy3, 395, 65, 50, 45);
		g->DrawImage(trophy3, 625, 65, 50, 45);
	}
	
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("DEMOGRAPHY_MENUE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);	
}

void CEmpireMenuView::DrawEmpireTop5Menue(Gdiplus::Graphics *g)
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
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	if (bg_shipovmenu)
		g->DrawImage(bg_top5menu, 0, 0, 1075, 750);

	CString s;
	list<CPoint> lSystems;
	pDoc->GetStatistics()->GetTopSystems(5, lSystems);

	int nCount = 0;
	for (list<CPoint>::const_iterator it = lSystems.begin(); it != lSystems.end(); ++it)
	{
		CSector* pSector = &pDoc->GetSector(*it);
		// Ist das System komplett bekannt oder ist der Besitzer bekannt und das System gescannt -> dann wird das System angezeigt
		if (pSector->GetFullKnown(pMajor->GetRaceID()) || pMajor->IsRaceContacted(pSector->GetOwnerOfSector()) && pSector->GetScanned(pMajor->GetRaceID()))
		{
			// Planeten zeichnen
			DrawSunSystem(g, *it, 110 + 110 * nCount);

			s.Format("%s %d: %s", CResourceManager::GetString("PLACE"), nCount + 1, pSector->GetName());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(150, 135 + 110 * nCount, m_TotalSize.cx - 250, 25), &fontFormat, &fontBrush);
			// Besitzerimperium zeichnen
			if (pMajor->IsRaceContacted(pSector->GetOwnerOfSector()) || pMajor->GetRaceID() == pSector->GetOwnerOfSector())
			{
				CMajor* pOwner = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pSector->GetOwnerOfSector()));
				if (pOwner)
				{
					s = pOwner->GetEmpiresName();
					Color color;
					color.SetFromCOLORREF(pOwner->GetDesign()->m_clrSector);
					SolidBrush ownerBrush(color);
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(150, 160 + 110 * nCount, m_TotalSize.cx - 250, 25), &fontFormat, &ownerBrush);
				}
			}
		}
		else
		{
			s.Format("%s %d: Unbekannt", CResourceManager::GetString("PLACE"), nCount + 1);			
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(150, 135 + 110 * nCount, m_TotalSize.cx - 250, 25), &fontFormat, &fontBrush);			
		}
		
		nCount++;
	}	
	
	// Buttons am unteren Bildrand zeichnen, womit wir die Menüs umschalten können
	// Schriftart für große Buttons laden
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush btnBrush(btnColor);
	// Buttons am unteren Bildrand zeichnen
	DrawGDIButtons(g, &m_EmpireNewsButtons, m_iSubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
		
	// "Top-5 Systeme" oben in der Mitte zeichnen
	fontFormat.SetAlignment(StringAlignmentCenter);
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("TOP5SYSTEMS_MENUE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);	
}

void CEmpireMenuView::DrawSunSystem(Gdiplus::Graphics *g, const CPoint& ptKO, int nPos)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	int nPosX = m_TotalSize.cx - 300;
	int nPosY = nPos + 55;

	// Sonne zeichnen
	Bitmap* graphic = NULL;
	switch (pDoc->GetSector(ptKO).GetSunColor())
	{
	case 0:
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_blue.bop");		break;
	case 1:
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_green.bop");		break;
	case 2:
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_orange.bop");	break;
	case 3:
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_red.bop");		break;
	case 4:
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_violet.bop");	break;			
	case 5:
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_white.bop");		break;
	case 6:
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_yellow.bop");	break;					
	}				
	if (graphic)
	{
		UINT nWidth		= graphic->GetWidth() / 2.5;
		UINT nHeight	= graphic->GetHeight() / 2.5;
		g->DrawImage(graphic, (REAL)m_TotalSize.cx - 200, (REAL)nPosY - nHeight / 2.0, (REAL)nWidth, (REAL)nHeight);
	}
	
	// Planeten zeichnen
	for (int i = 0; i < pDoc->GetSector(ptKO).GetNumberOfPlanets(); i++)
	{
		CPlanet* pPlanet = pDoc->GetSector(ptKO).GetPlanet(i);
		graphic = NULL;
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Planets\\" + pPlanet->GetPlanetName() + ".bop");
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic(pPlanet->GetGraphicFile());
		if (graphic)
		{
			// Planetengrößen leicht anpassen
			float fSizeMod	= max(1.0, (4 - pPlanet->GetSize()) / 2.0);
			UINT nWidth		= graphic->GetWidth() / 1.75 / fSizeMod;
			UINT nHeight	= graphic->GetHeight() / 1.75 / fSizeMod;
			
			nPosX -= (nWidth + 3);
			g->DrawImage(graphic, (REAL)nPosX, (REAL)nPosY - nHeight / 2.0, (REAL)nWidth, (REAL)nHeight);
		}		
	}
}

void CEmpireMenuView::DrawEmpireVictoryMenue(Gdiplus::Graphics *g)
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
	
	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	SolidBrush fontBrush(markColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	if (bg_shipovmenu)
		g->DrawImage(bg_victorymenu, 0, 0, 1075, 750);
	
	CString s;
	fontFormat.SetAlignment(StringAlignmentNear);
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);	
	// Kategorie "Auslöschung"
	g->DrawString(CResourceManager::GetString("VC_ELIMINATION").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,175,250,25), &fontFormat, &fontBrush);
	// Kategorie "Diplomatie"
	g->DrawString(CResourceManager::GetString("VC_DIPLOMACY").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,250,250,25), &fontFormat, &fontBrush);
	// Kategorie "Eroberung"
	g->DrawString(CResourceManager::GetString("VC_CONQUEST").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,325,250,25), &fontFormat, &fontBrush);
	// Kategorie "Forschung"
	g->DrawString(CResourceManager::GetString("VC_RESEARCH").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,400,250,25), &fontFormat, &fontBrush);
	// Kategorie "Siege"
	g->DrawString(CResourceManager::GetString("VC_COMBAT").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,475,250,25), &fontFormat, &fontBrush);
	// Kategorie "Geheimdienst"
	g->DrawString(CResourceManager::GetString("VC_SECURITY").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,550,250,25), &fontFormat, &fontBrush);

	fontBrush.SetColor(normalColor);
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Kategorie "Auslöschung"
	if (pDoc->m_VictoryObserver.IsVictoryCondition(VICTORYTYPE_ELIMINATION))
	{
		g->DrawString(CResourceManager::GetString("ELIMINATE_ALL_RIVALS").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,200,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("RIVALS_LEFT").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(465,200,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetRivalsLeft());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(625,200,250,25), &fontFormat, &fontBrush);
	}
	else
		g->DrawString(CResourceManager::GetString("DEACTIVATED").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,200,250,25), &fontFormat, &fontBrush);

	// Kategorie "Diplomatie"
	if (pDoc->m_VictoryObserver.IsVictoryCondition(VICTORYTYPE_DIPLOMACY))
	{
		s.Format("%d %s", pDoc->m_VictoryObserver.GetNeededVictoryValue(VICTORYTYPE_DIPLOMACY), CResourceManager::GetString("SIGNED_HIGH_AGREEMENTS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,275,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("WE:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(465,275,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetVictoryStatus(pMajor->GetRaceID(), VICTORYTYPE_DIPLOMACY));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(625,275,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("BEST:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(750,275,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetBestVictoryValue(VICTORYTYPE_DIPLOMACY));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(850,275,250,25), &fontFormat, &fontBrush);
	}
	else
		g->DrawString(CResourceManager::GetString("DEACTIVATED").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,275,250,25), &fontFormat, &fontBrush);
	// Kategorie "Eroberung"
	if (pDoc->m_VictoryObserver.IsVictoryCondition(VICTORYTYPE_CONQUEST))
	{
		s.Format("%d %s", pDoc->m_VictoryObserver.GetNeededVictoryValue(VICTORYTYPE_CONQUEST), CResourceManager::GetString("CONQUERED_SYSTEMS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,350,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("WE:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(465,350,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetVictoryStatus(pMajor->GetRaceID(), VICTORYTYPE_CONQUEST));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(625,350,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("BEST:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(750,350,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetBestVictoryValue(VICTORYTYPE_CONQUEST));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(850,350,250,25), &fontFormat, &fontBrush);
	}
	else
		g->DrawString(CResourceManager::GetString("DEACTIVATED").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,350,250,25), &fontFormat, &fontBrush);
	// Kategorie "Forschung"
	if (pDoc->m_VictoryObserver.IsVictoryCondition(VICTORYTYPE_RESEARCH))
	{
		s.Format("%d %s", pDoc->m_VictoryObserver.GetNeededVictoryValue(VICTORYTYPE_RESEARCH), CResourceManager::GetString("RESEARCHED_SPECIALS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,425,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("WE:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(465,425,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetVictoryStatus(pMajor->GetRaceID(), VICTORYTYPE_RESEARCH));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(625,425,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("BEST:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(750,425,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetBestVictoryValue(VICTORYTYPE_RESEARCH));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(850,425,250,25), &fontFormat, &fontBrush);
	}
	else
		g->DrawString(CResourceManager::GetString("DEACTIVATED").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,425,250,25), &fontFormat, &fontBrush);
	// Kategorie "Siege"
	if (pDoc->m_VictoryObserver.IsVictoryCondition(VICTORYTYPE_COMBATWINS))
	{
		s.Format("%d %s", pDoc->m_VictoryObserver.GetNeededVictoryValue(VICTORYTYPE_COMBATWINS), CResourceManager::GetString("REACHED_COMBAT_WINNINGS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,500,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("WE:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(465,500,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetVictoryStatus(pMajor->GetRaceID(), VICTORYTYPE_COMBATWINS));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(625,500,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("BEST:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(750,500,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetBestVictoryValue(VICTORYTYPE_COMBATWINS));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(850,500,250,25), &fontFormat, &fontBrush);
	}
	else
		g->DrawString(CResourceManager::GetString("DEACTIVATED").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,500,250,25), &fontFormat, &fontBrush);
	// Kategorie "Geheimdienst"
	if (pDoc->m_VictoryObserver.IsVictoryCondition(VICTORYTYPE_SABOTAGE))
	{
		s.Format("%d %s", pDoc->m_VictoryObserver.GetNeededVictoryValue(VICTORYTYPE_SABOTAGE), CResourceManager::GetString("SUCCESSFULL_SABOTAGE_ACTIONS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,575,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("WE:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(465,575,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetVictoryStatus(pMajor->GetRaceID(), VICTORYTYPE_SABOTAGE));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(625,575,250,25), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("BEST:").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(750,575,250,25), &fontFormat, &fontBrush);
		s.Format("%d", pDoc->m_VictoryObserver.GetBestVictoryValue(VICTORYTYPE_SABOTAGE));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(850,575,250,25), &fontFormat, &fontBrush);
	}
	else
		g->DrawString(CResourceManager::GetString("DEACTIVATED").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,575,250,25), &fontFormat, &fontBrush);

	// Buttons am unteren Bildrand zeichnen, womit wir die Menüs umschalten können
	// Schriftart für große Buttons laden
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush btnBrush(btnColor);
	// Buttons am unteren Bildrand zeichnen
	DrawGDIButtons(g, &m_EmpireNewsButtons, m_iSubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
		
	// "Siegbedingunen" oben in der Mitte zeichnen
	fontFormat.SetAlignment(StringAlignmentCenter);
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("VICTORY_CONDITIONS_MENUE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);	
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
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

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
	if (m_iSubMenu == EMPIREVIEW_NEWS)
	{			
		// Checken auf welchen Newsbutton ich geklickt habe, um nur Kategorien anzuzeigen
		int temp = m_iWhichNewsButtonIsPressed;
		if (ButtonReactOnLeftClick(point, &m_EmpireNewsFilterButtons, temp))
		{
			m_iWhichNewsButtonIsPressed = temp;
			return;
		}
		
		// Wenn wir auf eine News geklickt haben, diese Markieren
		CRect r;
		r.SetRect(0, 0, m_TotalSize.cx,m_TotalSize.cy);
		unsigned short j = 0;
		short counter = m_iClickedNews - 20 + m_iOldClickedNews;
		short add = 0;
		for (int i = 0; i < pMajor->GetEmpire()->GetMessages()->GetSize(); i++)
			if (m_iWhichNewsButtonIsPressed == pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType()
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
	else if (m_iSubMenu == EMPIREVIEW_SYSTEMS)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		// Haben wir auf einen Button geklickt, um zwischen Ressourcen- und normaler Ansicht zu wechseln
		int temp = m_iSystemSubMenue;
		if (ButtonReactOnLeftClick(point, &m_EmpireSystemFilterButtons, temp, FALSE))
		{
			m_iSystemSubMenue = temp;
			Invalidate(FALSE);
			return;
		}
		// Haben wir auf ein System in der Liste geklickt
		unsigned short j = 0;
		short counter = m_iClickedSystem - 20 + m_iOldClickedSystem;
		short add = 0;
		
		for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
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
	else if (m_iSubMenu == EMPIREVIEW_SHIPS)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		// Checken ob ich auf irgendeinen Ordering-Button geklickt habe
		if (CRect(r.left+50,r.top+115,r.left+225,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach dem Namen der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_shipname);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+225,r.top+115,r.left+375,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach dem Typ der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_shiptype);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+375,r.top+115,r.left+495,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach der Klasse der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_shipclass);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+495,r.top+115,r.left+560,r.top+140).PtInRect(point) && m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach der Baurunde der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_roundbuild);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+560,r.top+115,r.left+675,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach dem Sektornamen der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_sectorname);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+675,r.top+115,r.left+790,r.top+140).PtInRect(point) && m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach dem aktuellen Sektor der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_currentsector);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+790,r.top+115,r.left+850,r.top+140).PtInRect(point) && m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach der Erfahrung der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_exp);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+850,r.top+115,r.right-50,r.top+140).PtInRect(point))
		{
			// Sortierung der Schiffshistory nach dem Auftrag der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_task);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+495,r.top+115,r.left+560,r.top+140).PtInRect(point) && !m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach der Zerstörungsrunde der Schiffe
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_rounddestroy);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+675,r.top+115,r.left+850,r.top+140).PtInRect(point) && !m_bShowAliveShips)
		{
			// Sortierung der Schiffshistory nach der Zerstörungsart des Schiffes
			c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_destroytype);
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
			m_iClickedShipIndex = -1;
			Invalidate(FALSE);
			return;
		}
		// Haben wir auf ein Schiff in der Liste geklickt
		unsigned short j = 0;
		unsigned short k = 0;
		short counter = m_iClickedShip - 20 + m_iOldClickedShip;
		short add = 0;
		for (UINT i = 0; i < pMajor->GetShipHistory()->GetSizeOfShipHistory(); i++)
		{
			if (pMajor->GetShipHistory()->IsShipAlive(i))
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
						m_iClickedShipIndex = i;
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
						m_iClickedShipIndex = i;
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
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);

	// Wenn wir im Imperiummenü sind
	// Wenn wir in der Nachrichtenübersicht sind
	if (m_iSubMenu == EMPIREVIEW_NEWS)
	{
		int maxNews = 0;
		// nur Nachrichten anzeigen, dessen Typ wir auch gewählt haben
		for (int i = 0; i < pMajor->GetEmpire()->GetMessages()->GetSize(); i++)
			if (m_iWhichNewsButtonIsPressed == pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() ||
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
	else if (m_iSubMenu == EMPIREVIEW_SYSTEMS)
	{
		if (zDelta < 0)
		{
			if (pMajor->GetEmpire()->GetSystemList()->GetSize() > m_iClickedSystem+1)
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
	else if (m_iSubMenu == EMPIREVIEW_SHIPS)
	{
		if (zDelta < 0)
		{
			if ((int)pMajor->GetShipHistory()->GetNumberOfShips(m_bShowAliveShips) > m_iClickedShip+1)
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
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);
	// Wenn wir uns in der Imperiumsübersichtansicht befinden	
	// Wenn wir in der Nachtichtenansicht sind
	if (m_iSubMenu == EMPIREVIEW_NEWS)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		
		int j = 0;
		short counter = m_iClickedNews - 20 + m_iOldClickedNews;
		short add = 0;
		for (int i = 0; i < pMajor->GetEmpire()->GetMessages()->GetSize(); i++)
			if (m_iWhichNewsButtonIsPressed == pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() ||
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
						if (pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() == ECONOMY || 
							pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() == SOMETHING)
						{
							CPoint p = pMajor->GetEmpire()->GetMessages()->GetAt(i).GetKO();
							if (pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == pMajor->GetRaceID())
							{
								pDoc->SetKO(p.x,p.y);
								pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CSystemMenuView), pMajor->GetEmpire()->GetMessages()->GetAt(i).GetFlag());
								pDoc->GetMainFrame()->SelectMainView(SYSTEM_VIEW, pMajor->GetRaceID());
								pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
								CGalaxyMenuView::SetMoveShip(FALSE);
								pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
							}
						}
						else if (pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() == MILITARY)
						{
							if (pMajor->GetEmpire()->GetMessages()->GetAt(i).GetFlag() == FALSE)
							{
								CPoint p = pMajor->GetEmpire()->GetMessages()->GetAt(i).GetKO();
								if (pDoc->GetSystem(p.x,p.y).GetOwnerOfSystem() == pMajor->GetRaceID())
								{
									pDoc->SetKO(p.x,p.y);
									pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CSystemMenuView), 0);
									pDoc->GetMainFrame()->SelectMainView(SYSTEM_VIEW, pMajor->GetRaceID());
									CGalaxyMenuView::SetMoveShip(FALSE);
									pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
									pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
								}
							}
						}
						else if (pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() == RESEARCH)
						{
							pDoc->GetMainFrame()->SelectMainView(RESEARCH_VIEW, pMajor->GetRaceID());
							pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CResearchMenuView), pMajor->GetEmpire()->GetMessages()->GetAt(i).GetFlag());
							Invalidate();
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
						}
						else if (pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() == SECURITY)
						{
							pDoc->GetMainFrame()->SelectMainView(INTEL_VIEW, pMajor->GetRaceID());
							pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CIntelMenuView), pMajor->GetEmpire()->GetMessages()->GetAt(i).GetFlag());
							Invalidate();
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));							
						}
						else if (pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() == DIPLOMACY)
						{
							pDoc->GetMainFrame()->SelectMainView(DIPLOMACY_VIEW, pMajor->GetRaceID());
							// bei Angeboten an uns direkt in das Eingangmenü schalten
							pDoc->GetMainFrame()->SetSubMenu(RUNTIME_CLASS(CDiplomacyMenuView), pMajor->GetEmpire()->GetMessages()->GetAt(i).GetFlag());
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
	else if (m_iSubMenu == EMPIREVIEW_SYSTEMS)
	{
		// Haben wir auf ein System in der Liste geklickt
		if (m_iClickedSystem != -1)
		{
			CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(m_iClickedSystem).ko;
			pDoc->SetKO(ko.x, ko.y);
			pDoc->GetMainFrame()->SelectMainView(SYSTEM_VIEW, pMajor->GetRaceID());
			CSystemMenuView::SetMarkedBuildListEntry(0);
			Invalidate(FALSE);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			return;
		}
	}
	// Wenn wir in der Schiffsansicht sind
	else if (m_iSubMenu == EMPIREVIEW_SHIPS)
	{
		// Haben wir auf ein Schiff in der Liste geklickt
		if (m_iClickedShipIndex != -1)
		{
			CShipHistoryStruct* pShipHistory = pMajor->GetShipHistory()->GetShipHistory(m_iClickedShipIndex);
			if (pShipHistory)
			{
				// Sektorkoordinate finden
				CPoint pt = CPoint(-1,-1);
				for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
					for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
						if (pDoc->GetSector(x,y).GetName(true) == pShipHistory->m_strCurrentSector)
						{
							pt = CPoint(x,y);
							break;
						}

				CGalaxyMenuView* pView = dynamic_cast<CGalaxyMenuView*>(pDoc->GetMainFrame()->GetView(RUNTIME_CLASS(CGalaxyMenuView)));
				if (pView && pt != CPoint(-1,-1))
				{				
					pDoc->SetKO(pt);
					pView->ScrollToSector(pt);
					pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pMajor->GetRaceID());
					Invalidate(FALSE);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
					return;
				}
			}
		}
	}

	CMainBaseView::OnLButtonDblClk(nFlags, point);
}

void CEmpireMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);
	// Wenn wir in der Imperiumsansicht sind
	// Buttons am unteren Rand überprüfen
	ButtonReactOnMouseOver(point, &m_EmpireNewsButtons);
	// Sind wir in der Nachrichtenansicht
	if (m_iSubMenu == EMPIREVIEW_NEWS)
		ButtonReactOnMouseOver(point, &m_EmpireNewsFilterButtons);
	// sind wir in der Systenübersicht
	else if (m_iSubMenu == EMPIREVIEW_SYSTEMS)
		ButtonReactOnMouseOver(point, &m_EmpireSystemFilterButtons);	
	// sind wir in der Schiffsübersicht
	else if (m_iSubMenu == EMPIREVIEW_SHIPS)
		ButtonReactOnMouseOver(point, &m_EmpireShipsFilterButtons);	

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CEmpireMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
	// Wenn wir in der Nachrichten und Informationsansicht sind
	// Wenn wir in der Nachtichtenasicht sind
	if (m_iSubMenu == EMPIREVIEW_NEWS)
	{
		// Wenn wir auf eine News ausgewählt haben, können wir diese durch Betätigen der Entf-Taste löschen
		if (m_iClickedNews != -1 && nChar == VK_DELETE && m_iWhichNewsButtonIsPressed == NO_TYPE)
		{
			pMajor->GetEmpire()->GetMessages()->RemoveAt(m_iClickedNews);
			if (m_iClickedNews == pMajor->GetEmpire()->GetMessages()->GetSize())
				m_iClickedNews--;
			Invalidate(FALSE);
			return;
		}
		
		int maxNews = 0;
		// nur Nachrichten anzeigen, dessen Typ wir auch gewählt haben
		for (int i = 0; i < pMajor->GetEmpire()->GetMessages()->GetSize(); i++)
			if (m_iWhichNewsButtonIsPressed == pMajor->GetEmpire()->GetMessages()->GetAt(i).GetMessageType() ||
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
	if (m_iSubMenu == EMPIREVIEW_SYSTEMS)
	{
		if (nChar == VK_DOWN)
		{
			if (pMajor->GetEmpire()->GetSystemList()->GetSize() > m_iClickedSystem+1)
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
				CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(m_iClickedSystem).ko;
				BOOLEAN autoBuild = pDoc->m_System[ko.x][ko.y].GetAutoBuild();
				pDoc->m_System[ko.x][ko.y].SetAutoBuild(!autoBuild);
			//	m_iClickedOn = 1;
				Invalidate(FALSE);
				return;
			}
		}
	}
	// Wenn wir in der Imperiumsschiffsübersicht sind
	else if (m_iSubMenu == EMPIREVIEW_SHIPS)
	{
		if (nChar == VK_DOWN)
		{
			if ((int)pMajor->GetShipHistory()->GetNumberOfShips(m_bShowAliveShips) > m_iClickedShip+1)
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
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CString sPrefix = pMajor->GetPrefix();
	// alle Buttons in der View anlegen und Grafiken laden
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
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
	m_EmpireNewsButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("BTN_DEMOGRAPHY"),  fileN, fileI, fileA));
	m_EmpireNewsButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_TOP5SYSTEMS"),  fileN, fileI, fileA));
	m_EmpireNewsButtons.Add(new CMyButton(CPoint(860,690), CSize(160,40), CResourceManager::GetString("BTN_VICTORIES"),  fileN, fileI, fileA));
	m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(60,70), CSize(160,40), CResourceManager::GetString("BTN_PRODUCTION"),  fileN, fileI, fileA));
	m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(220,70), CSize(160,40), CResourceManager::GetString("BTN_RESOURCES"),  fileN, fileI, fileA));
	m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(380,70), CSize(160,40), CResourceManager::GetString("BTN_DEFENCE"),  fileN, fileI, fileA));
	m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(96,70), CSize(160,40), CResourceManager::GetString("BTN_CURRENTS"),  fileN, fileI, fileA));
	m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(495,70), CSize(160,40), CResourceManager::GetString("BTN_LOST"),  fileN, fileI, fileA));
}