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
#include "Races\RaceController.h"

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
	c_arraysort<CArray<SystemViewStruct>,SystemViewStruct>(*pMajor->GetEmpire()->GetSystemList() ,sort_asc);
	// Sortierung der Schiffshistory nach dem Namen der Schiffe
	c_arraysort<CArray<CShipHistoryStruct,CShipHistoryStruct>,CShipHistoryStruct>(*(pMajor->GetShipHistory()->GetShipHistoryArray()),CShipHistoryStruct::sort_by_shipname);

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
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	// TODO: add draw code here
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

	if (m_iSubMenu == 0)
		DrawEmpireNewsMenue(&g);
	else if (m_iSubMenu == 1)
		DrawEmpireSystemMenue(&g);
	else if (m_iSubMenu == 2)
		DrawEmpireShipMenue(&g);

	g.ReleaseHDC(pDC->GetSafeHdc());
}

void CEmpireMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CreateButtons();

	CString sPrefix = pMajor->GetPrefix();
	
	bg_newsovmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "newsovmenu.boj");
	bg_systemovmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "systemovmenu.boj");
	bg_shipovmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "shipovmenu.boj");

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
				s.Format("%d",pDoc->GetSystem(KO.x,KO.y).GetFoodStore());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(480,140+j*25,100,25), &fontFormat, &fontBrush);
				
				// Industrieproduktion anzeigen
				int ip = pDoc->GetSystem(KO.x,KO.y).GetProduction()->GetIndustryProd();
				s.Format("%d",ip);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(580,140+j*25,100,25), &fontFormat, &fontBrush);
								
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
					fontBrush.SetColor(Color::Red);					
				}

				fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(680,140+j*25,345,25), &fontFormat, &fontBrush);
				fontFormat.SetTrimming(StringTrimmingNone);
				fontBrush.SetColor(normalColor);
			}
			// andernfalls wird hier die Systemressourcenübersicht angezeigt
			else
			{
				for (int k = TITAN; k <= DILITHIUM; k++)
				{
					fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
					s.Format("%d", pDoc->GetSystem(KO.x, KO.y).GetRessourceStore(k));
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270+k*120,140+j*25,120,25), &fontFormat, &fontBrush);
					fontFormat.SetTrimming(StringTrimmingNone);
				}
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
	DrawGDIButtons(g, &m_EmpireSystemFilterButtons, m_bShowResources, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
	// Buttons am unteren Bildrand zeichnen
	DrawGDIButtons(g, &m_EmpireNewsButtons, m_iSubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), btnBrush);
		
	// Tabellenüberschriften zeichnen
	fontBrush.SetColor(markColor);
	Gdiplus::Font font(fontName.AllocSysString(), fontSize);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
	g->DrawString(CResourceManager::GetString("SECTOR").AllocSysString(), -1, &font, RectF(50,110,80,30), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("NAME").AllocSysString(), -1, &font, RectF(130,110,140,30), &fontFormat, &fontBrush);
	if (!m_bShowResources)
	{
		g->DrawString(CResourceManager::GetString("MORAL").AllocSysString(), -1, &font, RectF(270,110,130,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("FOOD").AllocSysString(), -1, &font, RectF(400,110,80,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("STORAGE").AllocSysString(), -1, &font, RectF(480,110,100,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("INDUSTRY").AllocSysString(), -1, &font, RectF(580,110,100,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("JOB").AllocSysString(), -1, &font, RectF(680,110,345,30), &fontFormat, &fontBrush);
	}
	else
	{
		g->DrawString(CResourceManager::GetString("TITAN").AllocSysString(), -1, &font, RectF(270,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("DEUTERIUM").AllocSysString(), -1, &font, RectF(390,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("DURANIUM").AllocSysString(), -1, &font, RectF(510,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("CRYSTAL").AllocSysString(), -1, &font, RectF(630,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("IRIDIUM").AllocSysString(), -1, &font, RectF(750,110,120,30), &fontFormat, &fontBrush);
		g->DrawString(CResourceManager::GetString("DILITHIUM").AllocSysString(), -1, &font, RectF(870,110,120,30), &fontFormat, &fontBrush);		
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
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(495,140+j*25,65,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strSectorName;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(560,140+j*25,115,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strCurrentSector;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(675,140+j*25,125,25), &fontFormat, &fontBrush);
				s.Format("%d",pMajor->GetShipHistory()->GetShipHistory(i)->m_iExperiance);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,140+j*25,50,25), &fontFormat, &fontBrush);
				s = pMajor->GetShipHistory()->GetShipHistory(i)->m_strCurrentTask;
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(850,140+j*25,175,25), &fontFormat, &fontBrush);
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
		g->DrawString(CResourceManager::GetString("STATUS").AllocSysString(), -1, &font, RectF(850,110,175,30), &fontFormat, &fontBrush);
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
	else if (m_iSubMenu == 2)
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
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);

	// Wenn wir im Imperiummenü sind
	// Wenn wir in der Nachrichtenübersicht sind
	if (m_iSubMenu == 0)
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
	else if (m_iSubMenu == 1)
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
	else if (m_iSubMenu == 2)
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
	if (m_iSubMenu == 0)
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
	else if (m_iSubMenu == 1)
	{
		// Haben wir auf ein System in der Liste geklickt
		if (m_iClickedSystem != -1)
		{
			CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(m_iClickedSystem).ko;
			pDoc->SetKO(ko.x, ko.y);
			pDoc->GetMainFrame()->SelectMainView(2, pMajor->GetRaceID());
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
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);
	// Wenn wir in der Imperiumsansicht sind
	// Buttons am unteren Rand überprüfen
	ButtonReactOnMouseOver(point, &m_EmpireNewsButtons);
	// Sind wir in der Nachrichtenansicht
	if (m_iSubMenu == 0)
		ButtonReactOnMouseOver(point, &m_EmpireNewsFilterButtons);
	// sind wir in der Systenübersicht
	else if (m_iSubMenu == 1)
		ButtonReactOnMouseOver(point, &m_EmpireSystemFilterButtons);	
	// sind wir in der Schiffsübersicht
	else if (m_iSubMenu == 2)
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
	if (m_iSubMenu == 0)
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
	if (m_iSubMenu == 1)
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
	else if (m_iSubMenu == 2)
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
	m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(60,70), CSize(160,40), CResourceManager::GetString("BTN_NORMAL"),  fileN, fileI, fileA));
	m_EmpireSystemFilterButtons.Add(new CMyButton(CPoint(220,70), CSize(160,40), CResourceManager::GetString("BTN_RESOURCES"),  fileN, fileI, fileA));
	m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(96,70), CSize(160,40), CResourceManager::GetString("BTN_CURRENTS"),  fileN, fileI, fileA));
	m_EmpireShipsFilterButtons.Add(new CMyButton(CPoint(495,70), CSize(160,40), CResourceManager::GetString("BTN_LOST"),  fileN, fileI, fileA));
}