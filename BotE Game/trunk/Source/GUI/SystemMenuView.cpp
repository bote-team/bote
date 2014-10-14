// SystemMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "MainFrm.h"
#include "GalaxyMenuView.h"
#include "MenuChooseView.h"
#include "SystemMenuView.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "RoundRect.h"
#include "Graphic\memdc.h"
#include "General/Loc.h"
#include "Ships/Ships.h"
#include "GraphicPool.h"
#include "ManagerSettingsDlg.h"

short CSystemMenuView::m_iClickedOn = 0;
BYTE CSystemMenuView::m_byResourceRouteRes = RESOURCES::TITAN;

// CSystemMenuView
IMPLEMENT_DYNCREATE(CSystemMenuView, CMainBaseView)

CSystemMenuView::CSystemMenuView() :
	bg_buildmenu(),
	bg_workmenu(),
	bg_energymenu(),
	bg_overviewmenu(),
	bg_systrademenu(),
	m_iWhichSubMenu(0),
	m_byStartList(0),
	m_byEndList(m_byStartList + NOEIBL),
	m_iBOPage(0),
	m_iELPage(0),
	m_iRightClicked(-1),
	m_iSTPage(0),
	m_bySubMenu(0),
	m_bClickedOnBuyButton(FALSE),
	m_bClickedOnDeleteButton(FALSE),
	m_bClickedOnBuildingInfoButton(TRUE),
	m_bClickedOnBuildingDescriptionButton(FALSE),
	m_iGlobalStoreageQuantity(1)
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

	for (int i = 0; i < m_WorkerButtons.GetSize(); i++)
	{
		delete m_WorkerButtons[i];
		m_WorkerButtons[i] = 0;
	}
	m_WorkerButtons.RemoveAll();

	for (int i = 0; i < m_SystemTradeButtons.GetSize(); i++)
	{
		delete m_SystemTradeButtons[i];
		m_SystemTradeButtons[i] = 0;
	}
	m_SystemTradeButtons.RemoveAll();
}

BEGIN_MESSAGE_MAP(CSystemMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_XBUTTONDOWN()
	ON_COMMAND(ID_MENU_ENERGY_BUILDINGS_AUTO_ONOFF, &CSystemMenuView::OnMenuEnergyBuildingsAutoOnoff)
END_MESSAGE_MAP()

void CSystemMenuView::OnNewRound()
{
	m_byStartList = 0;
	m_iBOPage = 0;
	m_iELPage = 0;
	m_iSTPage = 0;
	m_iClickedOn = 0;
	m_bClickedOnBuyButton = FALSE;
	m_bClickedOnDeleteButton = FALSE;
	m_byResourceRouteRes = RESOURCES::TITAN;
}
// CSystemMenuView drawing

void CSystemMenuView::OnDraw(CDC* dc)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	SetFocus();

	// ZU ERLEDIGEN: Code zum Zeichnen hier einfügen
	// Doublebuffering wird initialisiert
	CMyMemDC pDC(dc);
	CRect client;
	GetClientRect(&client);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());

	g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);
	g.Clear(static_cast<Gdiplus::ARGB>(Color::Black));

	if (m_bySubMenu == 0)
		DrawBuildMenue(&g);
	else if (m_bySubMenu == 1 || m_bySubMenu == 12)
		DrawWorkersMenue(&g);
	else if (m_bySubMenu == 2)
		DrawEnergyMenue(&g);
	else if (m_bySubMenu == 3)
		DrawBuildingsOverviewMenue(&g);
	else if (m_bySubMenu == 4)
		DrawSystemTradeMenue(&g);
	DrawButtonsUnderSystemView(&g);

	g.ReleaseHDC(pDC->GetSafeHdc());
	if ((pDoc->m_bTutorialLoaded) && (!(pDoc->m_bTutorialBoxAlreadyShown[2])))
	{	
		Invalidate(FALSE);
		pDoc->m_bTutorialBoxAlreadyShown[2] = true;
		pDoc->GenerateTutorialBox("TSYSTEMBUILDMENU");
	}
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

	// Baumenürechtecke
	m_iClickedOn = 0;
	BuildingDescription.SetRect(30,410,290,620);
	BuildingInfo.SetRect(340,560,700,650);
	AssemblyListRect.SetRect(740,400,1030,620);
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
	// Handelsrouten und Globales Lager Ansicht
	m_iGlobalStoreageQuantity = 1;

	// View bei den Tooltipps anmelden
	resources::pMainFrame->AddToTooltip(this);
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CSystemMenuView::LoadRaceGraphics()
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);

	// Alle Buttons in der View erstellen
	CreateButtons();

	CString sPrefix = pMajor->GetPrefix();

	bg_buildmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "buildmenu.boj");
	bg_workmenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "workmenu.boj");
	bg_overviewmenu = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "overviewmenu.boj");
	bg_energymenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "energymenu.boj");
	bg_systrademenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "systrademenu.boj");
}

void CSystemMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

void CSystemMenuView::OnXButtonDown(UINT nFlags, UINT nButton, CPoint point)
{
	// Dieses Feature erfordert mindestens Windows 2000.
	// Die Symbole _WIN32_WINNT und WINVER müssen >= 0x0500 sein.
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	// Wenn wir in irgendeinem System sind, können wir mit dem links rechts Pfeil zu einem anderen System wechseln
	CPoint p = pDoc->GetCo();

	// an welcher Stelle in der Liste befindet sich das aktuelle System?
	short pos = 0;
	for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
	{
		if (pMajor->GetEmpire()->GetSystemList()->GetAt(i).ko == p)
		{
			pos = i;
			break;
		}
	}

	if (nButton == XBUTTON2)
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
		m_iClickedOn = 0;
		m_byStartList = 0;
		pDoc->SetKO(pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.x, pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.y);
		Invalidate(FALSE);
	}

	CMainBaseView::OnXButtonDown(nFlags, nButton, point);
}

BOOL CSystemMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

// Funktion zum Zeichnen der Baumenüansicht
void CSystemMenuView::DrawBuildMenue(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	SolidBrush fontBrush(static_cast<Gdiplus::ARGB>(Color::White));

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color color;
	CFontLoader::GetGDIFontColor(pMajor, 3, color);
	fontBrush.SetColor(color);
	// alte Schriftfarbe merken
	Gdiplus::Color oldColor = color;

	CString s;
	CPoint p = pDoc->GetCo();
	if (bg_buildmenu)
		g->DrawImage(bg_buildmenu, 0, 0, 1075, 750);

	DrawSystemProduction(g);

	// Farbe für die Markierungen auswählen
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	Gdiplus::Pen pen(color);
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	fontBrush.SetColor(color);

	// Anzeige der Moral und der Runden über der Bauliste
	s.Format("%s: %i",CLoc::GetString("MORAL"), pDoc->GetSystem(p.x, p.y).GetMoral());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(370, 106, 335, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(CLoc::GetString("ROUNDS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(370, 106, 335, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(CLoc::GetString("JOB")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(370, 106, 335, 25), &fontFormat, &fontBrush);

	// Die Struktur BuildList erstmal löschen, alle Werte auf 0
	m_vBuildlist.RemoveAll();

	// Wenn man keine Schiffe zur Auswahl hat oder keine Truppen bauen kann, dann wird wieder auf das normale
	// Gebäudebaumenü umgeschaltet
	if (m_iWhichSubMenu == 1 && pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize() == 0)
		m_iWhichSubMenu = 0;
	else if (m_iWhichSubMenu == 2 && pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize() == 0)
		m_iWhichSubMenu = 0;

	// hier Anzeige der baubaren Gebäude und Upgrades
	if (m_iWhichSubMenu == 0)				// Sind wir im Gebäude/Update Untermenü
	{
		// Zuerst werden die Upgrades angezeigt
		for (int i = 0; i < pDoc->GetSystem(p.x, p.y).GetBuildableUpdates()->GetSize(); i++)
			m_vBuildlist.Add(pDoc->GetSystem(p.x, p.y).GetBuildableUpdates()->GetAt(i) * (-1));

		// Dann werden die baubaren Gebäude angezeigt
		for (int i = 0; i < pDoc->GetSystem(p.x, p.y).GetBuildableBuildings()->GetSize(); i++)
			m_vBuildlist.Add(pDoc->GetSystem(p.x, p.y).GetBuildableBuildings()->GetAt(i));
	}
	else if (m_iWhichSubMenu == 1)		// Sind wir im Werftuntermenü?
	{
		for (int i = 0; i < pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize(); i++)
			m_vBuildlist.Add(pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetAt(i));
	}
	else if (m_iWhichSubMenu == 2)		// Sind wir im Kasernenuntermenü?
	{
		for (int i = 0; i < pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize(); i++)
			m_vBuildlist.Add(pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetAt(i) + 20000);
	}

	if (m_iClickedOn > 0 && m_iClickedOn > m_vBuildlist.GetUpperBound())
		m_iClickedOn = m_vBuildlist.GetUpperBound();
	if (m_iClickedOn < m_byStartList || m_iClickedOn > m_byEndList)
		m_iClickedOn = m_byStartList;

	// Einträge der Bauliste anzeigen
	m_byEndList = m_byStartList + NOEIBL;	// auf maximal Zwölf Einträge in der Bauliste begrenzt
	int y = 150;
	for (int i = m_byStartList; i < m_vBuildlist.GetSize(); i++)
	{
		if (i > m_byEndList)
			break;

		y += 25;
		fontBrush.SetColor(oldColor);
		fontFormat.SetAlignment(StringAlignmentNear);
		fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);

		// Markierung zeichen
		if (i == m_iClickedOn)
		{
			// Markierung worauf wir geklickt haben
			g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(319,y-25,403,25));
			g->DrawLine(&pen, 319, y-25, 722,y-25);
			g->DrawLine(&pen, 319, y, 722,y);
			// Farbe der Schrift wählen, wenn wir den Eintrag markiert haben
			fontBrush.SetColor(color);
		}

		// noch verbleibende Runden bis das Projekt fertig wird
		const int nRounds = pDoc->GetSystem(p.x, p.y).
			NeededRoundsToBuild(m_vBuildlist[i], false, pDoc->GetSystem(p.x, p.y).Manager().Active());
		BOOLEAN bCanAddToAssemblyList = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->MakeEntry(m_vBuildlist[i], p, pDoc->m_Systems, true);
		if (!bCanAddToAssemblyList)
		{
			// Schrift dunkler darstellen, wenn das Projekt aufgrund nicht ausreichender Rohstoffe nicht gebaut werden kann
			Gdiplus::Color color2;
			fontBrush.GetColor(&color2);
			color2 = Color(100, color2.GetR(), color2.GetG(), color2.GetB());
			fontBrush.SetColor(color2);
		}

		// Gebäude oder Update können ein Symbol haben
		if (m_vBuildlist[i] < 10000) {
			CBuildingInfo* pInfo = &pDoc->GetBuildingInfo(abs(m_vBuildlist[i]));
			if (pInfo->GetNeverReady() == false)
			{
				Bitmap* graphic = NULL;
				if (pInfo->GetFoodProd() > 0 || pInfo->GetFoodBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\foodSmall.bop");
				else if (pInfo->GetIPProd() > 0 || pInfo->GetIndustryBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\industrySmall.bop");
				else if (pInfo->GetEnergyProd() > 0 || pInfo->GetEnergyBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\energySmall.bop");
				else if (pInfo->GetSPProd() > 0 || pInfo->GetSecurityBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\securitySmall.bop");
				else if (pInfo->GetFPProd() > 0 || pInfo->GetResearchBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\researchSmall.bop");
				else if (pInfo->GetTitanProd() > 0 || pInfo->GetTitanBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop");
				else if (pInfo->GetDeuteriumProd() > 0 || pInfo->GetDeuteriumBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop");
				else if (pInfo->GetDuraniumProd() > 0 || pInfo->GetDuraniumBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop");
				else if (pInfo->GetCrystalProd() > 0 || pInfo->GetCrystalBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop");
				else if (pInfo->GetIridiumProd() > 0 || pInfo->GetIridiumBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop");
				else if (pInfo->GetDeritiumProd() > 0 || pInfo->GetDeritiumBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\Deritium.bop");
				else if (pInfo->GetCredits() > 0 || pInfo->GetCreditsBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\creditsSmall.bop");
				if (graphic)
					g->DrawImage(graphic, 355, y-21, 20, 16);
			}
		}

		// handelt es sich um ein Update?
		if (m_vBuildlist[i] < 0)
		{
			s = CLoc::GetString("UPGRADING", FALSE, pDoc->GetBuildingName(abs(m_vBuildlist[i])));
		}
		// handelt es sich um ein Gebäude?
		else if (m_vBuildlist[i] < 10000)
		{
			s = pDoc->GetBuildingName(abs(m_vBuildlist[i]));
		}
		// handelt es sich um ein Schiff
		else if (m_vBuildlist[i] < 20000)
		{
			s.Format("%s-%s", pDoc->m_ShipInfoArray.GetAt(m_vBuildlist[i] - 10000).GetShipClass(), CLoc::GetString("CLASS"));
		}
		// handelt es sich um eine Truppe
		else
		{
			s.Format("%s", pDoc->m_TroopInfo.GetAt(m_vBuildlist[i] - 20000).GetName());
		}

		// Projekteintrag hinschreiben
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(380, y-25, 290, 25), &fontFormat, &fontBrush);
		// Anzahl der benötigten Runden hinschreiben
		if (nRounds)
		{
			s.Format("%i", nRounds);
			fontFormat.SetAlignment(StringAlignmentFar);
			fontFormat.SetTrimming(StringTrimmingNone);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(580, y-25, 105, 25), &fontFormat, &fontBrush);
		}
	}

	fontBrush.SetColor(oldColor);

	// Hier die Einträge in der Bauliste
	this->DrawBuildList(g);

	// Arbeiterzuweisung unter dem Bild der Rasse zeichnen
	// kleine Bilder von den Rohstoffen zeichnen
	Bitmap* graphic = NULL;
	CPoint px[WORKER::IRIDIUM_WORKER+1] = {CPoint(0,0)};

	px[WORKER::FOOD_WORKER].SetPoint(80,230); px[WORKER::INDUSTRY_WORKER].SetPoint(80,255); px[WORKER::ENERGY_WORKER].SetPoint(80,280);
	px[WORKER::SECURITY_WORKER].SetPoint(80,305); px[WORKER::RESEARCH_WORKER].SetPoint(80,330);
	px[WORKER::TITAN_WORKER].SetPoint(185,230); px[WORKER::DEUTERIUM_WORKER].SetPoint(185,255); px[WORKER::DURANIUM_WORKER].SetPoint(185,280);
	px[WORKER::CRYSTAL_WORKER].SetPoint(185,305); px[WORKER::IRIDIUM_WORKER].SetPoint(185,330);

	for (int i = WORKER::FOOD_WORKER; i <= WORKER::IRIDIUM_WORKER; i++)
	{
		WORKER::Typ nWorker = (WORKER::Typ)i;
		switch (nWorker)
		{
		case WORKER::FOOD_WORKER:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\foodSmall.bop"); break;
		case WORKER::INDUSTRY_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\industrySmall.bop"); break;
		case WORKER::ENERGY_WORKER:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\energySmall.bop"); break;
		case WORKER::SECURITY_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\securitySmall.bop"); break;
		case WORKER::RESEARCH_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\researchSmall.bop"); break;
		case WORKER::TITAN_WORKER:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop"); break;
		case WORKER::DEUTERIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop"); break;
		case WORKER::DURANIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop"); break;
		case WORKER::CRYSTAL_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop"); break;
		case WORKER::IRIDIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop"); break;
		}
		if (graphic)
		{
			g->DrawImage(graphic, px[i].x, px[i].y, 20, 16);
			graphic = NULL;
		}
		s.Format("%d/%d", pDoc->GetSystem(p.x, p.y).GetWorker(nWorker), pDoc->GetSystem(p.x, p.y).GetNumberOfWorkbuildings(nWorker, 0));
		fontFormat.SetAlignment(StringAlignmentNear);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(30 + px[i].x, px[i].y, 70, 25), &fontFormat, &fontBrush);

	}

	// Hier die Anzeige der nötigen Rohstoffe und Industrie zum Bau des Gebäudes und dessen Beschreibung
	// auch das Bild zum aktuellen Projekt wird angezeigt
	if (m_iClickedOn >= 0 && m_iClickedOn < m_vBuildlist.GetSize())
	{
		int RunningNumber = abs(m_vBuildlist[m_iClickedOn]);

		// Berechnung der nötigen Industrie und Rohstoffe
		if (m_bClickedOnBuyButton == FALSE && m_bClickedOnDeleteButton == FALSE)
		{
			// also ein Gebäude oder Gebäudeupdate
			if (m_vBuildlist[m_iClickedOn] < 10000)
				pDoc->CurrentSystem().GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->CurrentSystem().GetAllBuildings(), m_vBuildlist[m_iClickedOn], pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			// also ein Schiff
			else if (m_vBuildlist[m_iClickedOn] < 20000 && pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize() > 0)
				pDoc->CurrentSystem().GetAssemblyList()->CalculateNeededRessources(0,&pDoc->m_ShipInfoArray.GetAt(m_vBuildlist[m_iClickedOn] - 10000), 0, pDoc->CurrentSystem().GetAllBuildings(), m_vBuildlist[m_iClickedOn], pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			// also eine Truppe
			else if (pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize() > 0)
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->CalculateNeededRessources(0,0,&pDoc->m_TroopInfo.GetAt(m_vBuildlist[m_iClickedOn] - 20000), pDoc->CurrentSystem().GetAllBuildings(), m_vBuildlist[m_iClickedOn], pMajor->GetEmpire()->GetResearch()->GetResearchInfo());

			// Anzeige der ganzen Werte
			s = CLoc::GetString("BUILD_COSTS");
			fontBrush.SetColor(color);
			fontFormat.SetAlignment(StringAlignmentCenter);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360,562,340,30), &fontFormat, &fontBrush);

			fontBrush.SetColor(oldColor);
			s.Format("%s: %i",CLoc::GetString("INDUSTRY"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryForBuild());
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360,590,325,25), &fontFormat, &fontBrush);
			s.Format("%s: %i",CLoc::GetString("TITAN"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededTitanForBuild());
			fontFormat.SetAlignment(StringAlignmentCenter);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360,590,325,25), &fontFormat, &fontBrush);
			s.Format("%s: %i",CLoc::GetString("DEUTERIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDeuteriumForBuild());
			fontFormat.SetAlignment(StringAlignmentFar);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360,590,325,25), &fontFormat, &fontBrush);

			s.Format("%s: %i",CLoc::GetString("DURANIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDuraniumForBuild());
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360,615,325,25), &fontFormat, &fontBrush);
			s.Format("%s: %i",CLoc::GetString("CRYSTAL"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededCrystalForBuild());
			fontFormat.SetAlignment(StringAlignmentCenter);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360,615,325,25), &fontFormat, &fontBrush);
			s.Format("%s: %i",CLoc::GetString("IRIDIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIridiumForBuild());
			fontFormat.SetAlignment(StringAlignmentFar);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360,615,325,25), &fontFormat, &fontBrush);

			if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDeritiumForBuild() > NULL)
			{
				s.Format("%s: %i",CLoc::GetString("DERITIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDeritiumForBuild());
				fontFormat.SetAlignment(StringAlignmentCenter);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360,640,325,25), &fontFormat, &fontBrush);
			}
		}

		// Hier die Beschreibung des Gebäudes bzw. die Informationen
		if (m_bClickedOnBuildingDescriptionButton == TRUE)
		{
			if (m_iWhichSubMenu == 0)		// im Gebäudeuntermenü
				s = pDoc->GetBuildingDescription(RunningNumber);
			else if (m_iWhichSubMenu == 1 && pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize() > 0)	// im Schiffsuntermenü
				s = pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetDescription();
			else if (m_iWhichSubMenu == 2 && pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize() > 0)	// im Kasernenuntermenü
				s = pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetDescription();
			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(BuildingDescription.left, BuildingDescription.top, BuildingDescription.Width(), BuildingDescription.Height() - 10), &fontFormat, &fontBrush);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			fontFormat.SetTrimming(StringTrimmingNone);
		}
		// bzw. die Information zu dem, was das Gebäude produziert
		if (m_bClickedOnBuildingInfoButton == TRUE)
		{

			if (m_iWhichSubMenu == 0)
				DrawBuildingProduction(g);
			else if (m_iWhichSubMenu == 1 && pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize() > 0)
			{
				pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).DrawShipInformation(g, BuildingDescription, &(Gdiplus::Font(CComBSTR(fontName), fontSize)), oldColor, color, pMajor->GetEmpire()->GetResearch());
			}
			else if (m_iWhichSubMenu == 2 && pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize() > 0)
			{
				// Anzeige der Truppeneigenschaften
				fontBrush.SetColor(color);
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				fontFormat.SetAlignment(StringAlignmentCenter);
				fontFormat.SetLineAlignment(StringAlignmentNear);
				CRect r(BuildingDescription.left,BuildingDescription.top,BuildingDescription.right,BuildingDescription.bottom);
				g->DrawString(CComBSTR(pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetName()), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				fontBrush.SetColor(oldColor);
				r.top += 60;
				BYTE offPower = pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetOffense();
				if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
					offPower += (offPower * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TROOPS)->GetBonus(1) / 100);
				s.Format("%s: %d",CLoc::GetString("OPOWER"), offPower);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				r.top += 25;
				s.Format("%s: %d",CLoc::GetString("DPOWER"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetDefense());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				r.top += 25;
				s.Format("%s: %d",CLoc::GetString("MORALVALUE"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetMoralValue());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				r.top += 25;
				s.Format("%s: %d",CLoc::GetString("PLACE"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetSize());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				r.top += 25;
				s.Format("%s: %d",CLoc::GetString("MAINTENANCE_COSTS"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetMaintenanceCosts());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				fontFormat.SetLineAlignment(StringAlignmentCenter);
				fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			}
		}

		// Hier die Anzeige des Bildes zu dem jeweiligen Projekt
		CString file;
		if (m_iWhichSubMenu == 0)		// sind im Gebäudeuntermenü
			file.Format("Buildings\\%s",pDoc->GetBuildingInfo(RunningNumber).GetGraphikFileName());
		else if (m_iWhichSubMenu == 1 && pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize() > 0)	// sind im Schiffsuntermenü
			file.Format("Ships\\%s.bop",pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipClass());
		else if (m_iWhichSubMenu == 2 && pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize() > 0)	// sind im Kasernenuntermenü
			file.Format("Troops\\%s",pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetGraphicFileName());
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
		if (graphic == NULL)
		{
			if (m_iWhichSubMenu == 0)		// sind im Gebäudeuntermenü
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
			else if (m_iWhichSubMenu == 1 && pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize() > 0)	// sind im Schiffsuntermenü
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
			else if (m_iWhichSubMenu == 2 && pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize() > 0)	// sind im Kasernenuntermenü
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Troops\\ImageMissing.bop");
		}

		if (graphic)
		{
			g->DrawImage(graphic, 70, 60, 200, 150);
		}
	}

	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);

	int nFirstAssemblyListEntry = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetAssemblyListEntry(0);
	// Hier die Anzeige der Kaufkosten, wenn wir auf den "kaufen Button" geklickt haben
	if (m_bClickedOnBuyButton == TRUE)
	{
		// Wenn was in der Bauliste steht
		if (nFirstAssemblyListEntry != 0)
		{
			CRect infoRect;
			int RunningNumber = 0;
			if (nFirstAssemblyListEntry < 0)
			{
				RunningNumber = abs(nFirstAssemblyListEntry);
				s = CLoc::GetString("BUY_UPGRADE", FALSE, pDoc->GetBuildingName(pDoc->BuildingInfo.GetAt(RunningNumber-1).GetPredecessorID()),pDoc->GetBuildingName(RunningNumber));
			}
			else if (nFirstAssemblyListEntry < 10000)
			{
				RunningNumber = nFirstAssemblyListEntry;
				s = CLoc::GetString("BUY_BUILDING", FALSE, pDoc->GetBuildingName(RunningNumber));
			}
			else if (nFirstAssemblyListEntry < 20000)
			{
				RunningNumber = nFirstAssemblyListEntry;
				s = CLoc::GetString("BUY_SHIP", FALSE, pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipTypeAsString()
					,pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipClass());
			}
			else
			{
				RunningNumber = nFirstAssemblyListEntry;
				s = CLoc::GetString("BUY_BUILDING", FALSE, pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetName());
			}

			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(340,565,360,65), &fontFormat, &fontBrush);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

			CString costs;
			costs.Format("%d", pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetBuildCosts());
			s = CLoc::GetString("CREDITS_COSTS", FALSE, costs);

			fontFormat.SetLineAlignment(StringAlignmentFar);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(340,565,360,75), &fontFormat, &fontBrush);
			fontFormat.SetLineAlignment(StringAlignmentCenter);

			// Ja/Nein Buttons zeichnen
			// Kleine Buttons unter der Kaufkostenübersicht
			fontFormat.SetAlignment(StringAlignmentCenter);

			if (graphic)
				g->DrawImage(graphic, 355, 645, 120, 30);
			s = CLoc::GetString("BTN_OKAY");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(355,645,120,30), &fontFormat, &btnBrush);

			if (graphic)
				g->DrawImage(graphic, 565, 645, 120, 30);
			s = CLoc::GetString("BTN_CANCEL");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(565,645,120,30), &fontFormat, &btnBrush);

			OkayButton.SetRect(355,645,475,675);
			CancelButton.SetRect(565,645,685,675);
		}
	}
	// Anzeige der Bestätigungsfrage, ob ein Auftrag wirklich abgebrochen werden soll
	if (m_bClickedOnDeleteButton == TRUE)
	{
		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CLoc::GetString("CANCEL_PROJECT");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(350,570,340,70), &fontFormat, &fontBrush);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

		// Ja/Nein Buttons zeichnen
		fontFormat.SetAlignment(StringAlignmentCenter);

		if (graphic)
			g->DrawImage(graphic, 355, 645, 120, 30);
		s = CLoc::GetString("BTN_OKAY");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(355,645,120,30), &fontFormat, &btnBrush);

		if (graphic)
			g->DrawImage(graphic, 565, 645, 120, 30);
		s = CLoc::GetString("BTN_CANCEL");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(565,645,120,30), &fontFormat, &btnBrush);

		OkayButton.SetRect(355,645,475,675);
		CancelButton.SetRect(565,645,685,675);
	}

	// Anzeige der kleinen Buttons (Bauhof, Werft, Kaserne) unter der Bauliste
	fontFormat.SetAlignment(StringAlignmentCenter);

	if (graphic)
		g->DrawImage(graphic, 325, 510, 120, 30);
	s = CLoc::GetString("BTN_BAUHOF");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(325,510,120,30), &fontFormat, &btnBrush);

	if (graphic)
		g->DrawImage(graphic, 460, 510, 120, 30);
	s = CLoc::GetString("BTN_DOCKYARD");
	if (pDoc->CurrentSystem().GetBuildableShips()->GetSize() != 0)//Schrift ausgrauen wenn keine Schiffe baubar/keine Schiffswerft
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(460,510,120,30), &fontFormat, &btnBrush);
	else
	{
		Gdiplus::Color color2;
		btnBrush.GetColor(&color2);
		btnBrush.SetColor(Color(100, color2.GetR(), color2.GetG(), color2.GetB()));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(460,510,120,30), &fontFormat, &btnBrush);
		btnBrush.SetColor(color2);
	}

	if (graphic)
		g->DrawImage(graphic, 595, 510, 120, 30);
	s = CLoc::GetString("BTN_BARRACK");
	if (pDoc->CurrentSystem().GetBuildableTroops()->GetSize() != 0) //Schrift ausgrauen wenn keine Truppen baubar/keine Kaserne
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(595,510,120,30), &fontFormat, &btnBrush);
	else
	{
		Gdiplus::Color color2;
		btnBrush.GetColor(&color2);
		btnBrush.SetColor(Color(100, color2.GetR(), color2.GetG(), color2.GetB()));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(595,510,120,30), &fontFormat, &btnBrush);
		btnBrush.SetColor(color2);
	}

	// plus Anzeige der kleinen Button (Info & Beschreibung) unter der Gebäudeinfobox
	if (graphic)
		g->DrawImage(graphic, 30, 625, 120, 30);
	s = CLoc::GetString("BTN_INFORMATION");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(30,625,120,30), &fontFormat, &btnBrush);
	if (graphic)
		g->DrawImage(graphic, 165, 625, 120, 30);
	s = CLoc::GetString("BTN_DESCRIPTION");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(165,625,120,30), &fontFormat, &btnBrush);

	// plus Anzeige der kleinen Buttons unter der Assemblylist (kaufen und abbrechen)
	// wenn wir noch nicht in dieser Runde gekauft haben
	if (pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetWasBuildingBought() == FALSE && nFirstAssemblyListEntry != 0)
	{
		// Bei Gebäuden nur wenn es nicht ein Auftrag mit NeverReady (z.B. Kriegsrecht) ist)
		if ((nFirstAssemblyListEntry < 0)
			||
			(nFirstAssemblyListEntry > 0 && nFirstAssemblyListEntry < 10000 && pDoc->GetBuildingInfo(nFirstAssemblyListEntry).GetNeverReady() == FALSE)
			||
			// Bei Schiffen nur, wenn eine Werft noch aktiv ist
			(nFirstAssemblyListEntry >= 10000 && nFirstAssemblyListEntry < 20000 && pDoc->GetSystem(p.x, p.y).GetProduction()->GetShipYard())
			||
			// Bei Truppen nur mit aktiver Kaseren
			(nFirstAssemblyListEntry >= 20000 && pDoc->GetSystem(p.x, p.y).GetProduction()->GetBarrack()))
		{
			if (graphic)
				g->DrawImage(graphic, 750, 625, 120, 30);
			s = CLoc::GetString("BTN_BUY");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(750,625,120,30), &fontFormat, &btnBrush);
		}
	}
	if (nFirstAssemblyListEntry != 0)
	{
		if (graphic)
			g->DrawImage(graphic, 900, 625, 120, 30);
		s = CLoc::GetString("BTN_CANCEL");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(900,625,120,30), &fontFormat, &btnBrush);
	}

	// Systemnamen mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 4, color);
	fontBrush.SetColor(color);
	// Name des Systems oben in der Mitte zeichnen
	s.Format("%s", pDoc->GetSystem(p.x,p.y).GetName());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,m_TotalSize.cx - 15, 50), &fontFormat, &fontBrush);
}



/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Arbeiterzuweisungsmenües
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawWorkersMenue(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	CString s;
	CPoint p = pDoc->GetCo();

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	CSystem sys = pDoc->GetSystem(p.x,p.y);

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

	Color markPen;
	markPen.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	Gdiplus::Pen pen(markPen);

	if (bg_workmenu)
		g->DrawImage(bg_workmenu, 0, 0, 1075, 750);

	// Hier die Systemproduktion zeichnen
	DrawSystemProduction(g);

	// Hier die Einträge in der Bauliste
	DrawBuildList(g);

	// Die Buttons zum Erhöhen bzw. Verringern der Arbeiteranzahl
	DrawGDIButtons(g, &m_WorkerButtons, -1, Gdiplus::Font(NULL), fontBrush);

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
		for (int i = WORKER::FOOD_WORKER; i <= WORKER::RESEARCH_WORKER; i++)
		{
			WORKER::Typ nWorker = (WORKER::Typ)i;
			number[i] = sys.GetNumberOfWorkbuildings(nWorker,0);
			if (number[i] > greatestNumber)
				greatestNumber = number[i];
			online[i] = sys.GetWorker(nWorker);
			// Die Rechtecke der Arbeiterbalken erstmal löschen
			for (int j = 0; j < 200; j++)
				Timber[i][j].SetRect(0,0,0,0);
		}
		size = greatestNumber;
		if (size != 0)
			width = (unsigned short)200/size;
		if (width > 10)
			width = 10;
		else if (width < 3)
			width = 3;
		short space = width / 2;
		space = max(2, space);

		// Den Balken zeichnen
		for (int i = WORKER::FOOD_WORKER; i <= WORKER::RESEARCH_WORKER; i++)
		{
			for (int j = 0; j < number[i]; j++)
			{
				Color timberColor;
				// Fragen ob die Gebäude noch online sind
				if (j >= online[i])
				{
					// Dunkle Farbe wenn sie Offline sind
					timberColor = Color(42,46,30);
				}
				else
				{
					// Helle Farbe wenn sie Online sind
					short color = j * 4;
					if (color > 230)
						color = 200;
					timberColor = Color(230-color,230-color/2,20);
				}

				//g->FillRectangle(&darkBrush, 220 + j * width * 2, 115 + i * 95, 2 * width - 4, 35);
				CRoundRect::FillRoundRect(g, Rect(220 + j * width * 2, 115 + i * 95, 2 * width - 4, 35), timberColor, 3);

				// Hier werden die Rechtecke von der Größe noch ein klein wenig verändert, damit man besser drauf klicken kann
				Timber[i][j].SetRect(220+ j*width*2-space,115+i*95,220+width+(j+1)*width*2-space,150+i*95);
			}
		}
		// Das Gebäude über dem Balken zeichnen
		for (int i = WORKER::FOOD_WORKER; i <= WORKER::RESEARCH_WORKER; i++)
		{
			CString name = "";
			WORKER::Typ nWorker = (WORKER::Typ)i;
			USHORT tmp = sys.GetNumberOfWorkbuildings(nWorker, 1);
			if (tmp != 0)
			{
				// Bild des jeweiligen Gebäudes zeichnen
				CString file;
				file.Format("Buildings\\%s", pDoc->GetBuildingInfo(tmp).GetGraphikFileName());
				Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
				if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
				if (graphic)
				{
					g->DrawImage(graphic, 50, i * 95 + 100, 100, 75);
					graphic = NULL;
				}

				if( online[i] < number[i] ) {
					name.Format("%d",online[i]);
					CString wa = CLoc::GetString("WORKERS_ACTIVE",FALSE,name);
					name.Format("%d x %s %s",number[i],pDoc->GetBuildingName(tmp),wa);
				} else {
					name.Format("%d x %s",number[i],pDoc->GetBuildingName(tmp));
				}

				CString yield = CLoc::GetString("YIELD");
				fontFormat.SetAlignment(StringAlignmentNear);
				g->DrawString(CComBSTR(name), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(220, 90 + i * 95, 380, 25), &fontFormat, &fontBrush);

				const CSystemProd *prod = sys.GetProduction();

				switch(i) {
					default:
					case WORKER::FOOD_WORKER:		name.Format("%s: %d %s",yield, prod->GetMaxFoodProd(),	CLoc::GetString("FOOD"));		break;
					case WORKER::INDUSTRY_WORKER:	name.Format("%s: %d %s",yield, prod->GetIndustryProd(),	CLoc::GetString("INDUSTRY"));	break;
					case WORKER::ENERGY_WORKER:		name.Format("%s: %d %s",yield, prod->GetMaxEnergyProd(),CLoc::GetString("ENERGY"));		break;
					case WORKER::SECURITY_WORKER:	name.Format("%s: %d %s",yield, prod->GetSecurityProd(),	CLoc::GetString("SECURITY"));	break;
					case WORKER::RESEARCH_WORKER:	name.Format("%s: %d %s",yield, prod->GetResearchProd(),	CLoc::GetString("RESEARCH"));	break;
				}

				if( i == WORKER::FOOD_WORKER ) {
					CString scmax, scstore;
					scstore.Format("%d", sys.GetFoodStore());
					scmax.Format("%d", sys.GetFoodStoreMax());

					CString sysstorage = CLoc::GetString("SYSTEM_STORAGE_INFO",FALSE,scstore, scmax );
					scstore.Format("%d",(sys.GetFoodStore() * 100 / sys.GetFoodStoreMax()));
					sysstorage += CLoc::GetString("SYSTEM_STORAGE_INFO_PERCENT",FALSE,scstore);
					name += ", " + sysstorage;
				}

				g->DrawString(CComBSTR(name), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
			}
		}

		// Ressourcenbutton zeichnen
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);
		if (graphic)
			g->DrawImage(graphic, ChangeWorkersButton.left, ChangeWorkersButton.top, 120, 30);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = CLoc::GetString("BTN_RESOURCES");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(ChangeWorkersButton.left, ChangeWorkersButton.top, 120, 30), &fontFormat, &btnBrush);
	}
	// Wenn wir in die Ressourcenansicht wollen
	if (m_bySubMenu == 12)
	{
		// Ab hier Anzeige der Besetzungsbalken
		// Balken mit maximal 200 Einträgen, d.h. es dürfen nicht mehr als 200 Gebäude von einem Typ stehen!!!
		//CRect workertimber[200];
		unsigned short number[5] = {0};						// Anzahl der Gebäude eines Types
		unsigned short online[5] = {0};
		unsigned short greatestNumber = 0;					// Größte Nummer der Number[5]
		unsigned short width = 0;							// Breite eines einzelnen Balkens
		unsigned short size = 0;							// Größe der einzelnen Balken
		// Größte Nummer berechnen
		for (int i = WORKER::FOOD_WORKER; i <= WORKER::RESEARCH_WORKER; i++)
		{
			WORKER::Typ nWorker = (WORKER::Typ)(i+5);
			number[i] = sys.GetNumberOfWorkbuildings(nWorker,0);
			if (number[i] > greatestNumber)
				greatestNumber = number[i];
			online[i] = sys.GetWorker(nWorker);
			// Die Rechtecke der Arbeiterbalken erstmal löschen
			for (int j = 0; j < 200; j++)
				Timber[i][j].SetRect(0,0,0,0);
		}
		size = greatestNumber;
		if (size != 0)
			width = (unsigned short)200/size;
		if (width > 10)
			width = 10;
		else if (width < 3)
			width = 3;
		short space = width / 2;
		space = max(2, space);

		// Den Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < number[i]; j++)
			{
				Color timberColor;
				// Fragen ob die Gebäude noch online sind
				if (j >= online[i])
				{
					// Dunkle Farbe wenn sie Offline sind
					timberColor = Color(42,46,30);
				}
				else
				{
					// Helle Farbe wenn sie Online sind
					short color = j * 4;
					if (color > 230)
						color = 200;
					timberColor = Color(230-color,230-color/2,20);
				}

				//g->FillRectangle(&darkBrush, 220 + j * width * 2 - space, 115 + i * 95, 2 * width - 4, 35);
				CRoundRect::FillRoundRect(g, Rect(220 + j * width * 2, 115 + i * 95, 2 * width - 4, 35), timberColor, 3);

				// Hier werden die Rechtecke von der Größe noch ein klein wenig verändert, damit man besser drauf klicken kann
				Timber[i][j].SetRect(220+ j*width*2-space, 115+i*95, 220+width+(j+1)*width*2-space, 150+i*95);
			}
		}
		// Das Gebäude über dem Balken zeichnen
		for (int i = WORKER::FOOD_WORKER; i <= WORKER::RESEARCH_WORKER; i++)
		{
			WORKER::Typ nWorker = (WORKER::Typ)(i+5);
			CString name = "";
			USHORT tmp = sys.GetNumberOfWorkbuildings(nWorker,1);
			if (tmp != 0)
			{
				// Bild des jeweiligen Gebäudes zeichnen
				CString file;
				file.Format("Buildings\\%s", pDoc->GetBuildingInfo(tmp).GetGraphikFileName());
				Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
				if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
				if (graphic)
				{
					g->DrawImage(graphic, 50, i * 95 + 100, 100, 75);
					graphic = NULL;
				}
				// Name als Helper für die Aktive-Gebäude-Zahl verwenden
				if( online[i] < number[i] ) {
					name.Format("%d",online[i]);
					CString wa = CLoc::GetString("WORKERS_ACTIVE",FALSE,name);
					name.Format("%d x %s %s",number[i],pDoc->GetBuildingName(tmp),wa);
				} else {
					name.Format("%d x %s",number[i],pDoc->GetBuildingName(tmp));
				}

				fontFormat.SetAlignment(StringAlignmentNear);
				g->DrawString(CComBSTR(name), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(220, 90 + i * 95, 380, 25), &fontFormat, &fontBrush);

				CString yield = CLoc::GetString("YIELD");
				const CSystemProd *prod = sys.GetProduction();

				int cprod, cstore, cmax;
				CString resname;

				switch(i) {
					default:
					case 0: cprod = prod->GetTitanProd(); cmax = sys.GetTitanStoreMax(); cstore = sys.GetTitanStore(); resname = "TITAN"; break;
					case 1: cprod = prod->GetDeuteriumProd(); cmax = sys.GetDeuteriumStoreMax(); cstore = sys.GetDeuteriumStore(); resname = "DEUTERIUM"; break;
					case 2: cprod = prod->GetDuraniumProd(); cmax = sys.GetDuraniumStoreMax(); cstore = sys.GetDuraniumStore(); resname = "DURANIUM"; break;
					case 3: cprod = prod->GetCrystalProd(); cmax = sys.GetCrystalStoreMax(); cstore = sys.GetCrystalStore(); resname = "CRYSTAL"; break;
					case 4: cprod = prod->GetIridiumProd(); cmax = sys.GetIridiumStoreMax(); cstore = sys.GetIridiumStore(); resname = "IRIDIUM"; break;
				}

				CString scmax, scstore;
				scstore.Format("%d", cstore);
				scmax.Format("%d", cmax);

				CString sysstorage = CLoc::GetString("SYSTEM_STORAGE_INFO",FALSE,scstore, scmax );
				scstore.Format("%d",(cstore * 100 / cmax));
				sysstorage += CLoc::GetString("SYSTEM_STORAGE_INFO_PERCENT",FALSE,scstore);
				name.Format("%s: %d %s, %s",yield, cprod, CLoc::GetString(resname), sysstorage);

				g->DrawString(CComBSTR(name), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
			}
		}

		// Anzeige des Umschaltbuttons und der Erklärung
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);
		if (graphic)
			g->DrawImage(graphic, ChangeWorkersButton.left, ChangeWorkersButton.top, 120, 30);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = CLoc::GetString("BTN_PRODUCTION", TRUE);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(ChangeWorkersButton.left, ChangeWorkersButton.top, 120, 30), &fontFormat, &btnBrush);
	}

	// Hier noch die Gesamt- und freien Arbeiter unten in der Mitte zeichnen
	unsigned short width = 0;
	unsigned short worker = sys.GetNumberOfWorkbuildings(WORKER::ALL_WORKER,0);
	unsigned short size = worker;
	if (size != 0)
		width = (unsigned short)200/size;
	if (width > 10)
		width = 10;
	if (width < 3)
		width = 3;

	// Den Balken zeichnen
	for (int i = 0; i < worker; i++)
	{
		Color timberColor;
		if (i < sys.GetWorker(WORKER::FREE_WORKER))
		{
			// Helle Farbe wenn sie Online sind
			short color = i*4;
			if (color > 230)
				color = 200;
			timberColor = Color(230-color,230-color/2,20);
		}
		else
		{
			// Dunkle Farbe wenn sie Offline sind
			timberColor = Color(42,46,30);
		}

		// g->FillRectangle(&lightBrush, 220 + i * width, 600, width - 2, 25);
		CRoundRect::FillRoundRect(g, Rect(220 + i * width, 600, width - 2, 25), timberColor, 2);
	}

	// freie Arbeiter über dem Balken zeichnen
	fontBrush.SetColor(normalColor);
	s.Format("%s %d/%d",CLoc::GetString("FREE_WORKERS"), sys.GetWorker(WORKER::FREE_WORKER), sys.GetWorker(WORKER::ALL_WORKER));
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(220,575,380,25), &fontFormat, &fontBrush);

	// Großen Text ("Arbeiterzuweisung in xxx") oben links zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentCenter);
	// Arbeiterzuweisung auf xxx oben links zeichnen
	s = CLoc::GetString("WORKERS_MENUE")+" "+pDoc->GetSystem(p.x,p.y).GetName();
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,720,60), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Bauwerkeansicht (auch Abriss der Gebäude hier mgl.)
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawBuildingsOverviewMenue(Graphics* g)
{
	// Das Feld der ganzen Gebäude muß aufsteigend nach der RunningNumber sortiert sein.
	// Ansonsten funktioniert der Algorithmus hier nicht mehr.
	// Sortiert wird das Feld in der CalculateNumberOfWorkBuildings() Funktion der CSystem Klasse.
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	CPoint p = pDoc->GetCo();;
	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color textMark;
	textMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_overviewmenu)
		g->DrawImage(bg_overviewmenu, 0, 0, 1075, 750);

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
		USHORT curRunningNumber = pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(i).GetRunningNumber();
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
	// provisorische Buttons für vor und zurück
	Color redColor;
	redColor.SetFromCOLORREF(RGB(200,50,50));
	SolidBrush redBrush(redColor);

	if (m_BuildingOverview.GetSize() > m_iBOPage * NOBIOL + NOBIOL)
	{
		s = ">";
		g->FillRectangle(&redBrush, RectF(1011,190,63,52));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(1011,190,63,52), &fontFormat, &fontBrush);
	}
	if (m_iBOPage > 0)
	{
		s = "<";
		g->FillRectangle(&redBrush, RectF(1011,490,63,52));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(1011,490,63,52), &fontFormat, &fontBrush);
	}

	SolidBrush blackBrush(static_cast<Gdiplus::ARGB>(Color::Black));
	// prüfen, dass man nicht auf einer zu hohen Seite ist, wenn zu wenig Gebäude vorhanden sind
	if (m_iBOPage * NOBIOL >= m_BuildingOverview.GetSize())
		m_iBOPage = 0;
	for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
	{
		// Wenn wir auf der richtigen Seite sind
		if (i < m_iBOPage * NOBIOL + NOBIOL)
		{
			// Aller 4 Einträge Y Platzhalter zurücksetzen und X Platzhalter eins hoch
			if (i%4 == 0 && i != m_iBOPage * NOBIOL)
			{
				spaceX++;
				spaceY = 0;
			}

			// großes Rechteck, was gezeichnet wird
			g->FillRectangle(&blackBrush, RectF(60+spaceX*320,80+spaceY*150,290,120));
			CRect r(60+spaceX*320,80+spaceY*150,350+spaceX*320,200+spaceY*150);
			m_BuildingOverview[i].rect = r;
			// kleine Rechtecke, wo die verschiedenen Texte drin stehen
			CRect r1,r2;
			r1.SetRect(r.left+5,r.top,r.right-5,r.top+20);
			r2.SetRect(r.left+140,r.top,r.right-5,r.bottom);

			// Gebäudegrafik laden und anzeigen
			CString file;
			file.Format("Buildings\\%s", pDoc->GetBuildingInfo(m_BuildingOverview.GetAt(i).runningNumber).GetGraphikFileName());
			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
			if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
			if (graphic)
				g->DrawImage(graphic, r.left+5, r.top+25, 130, 97);

			//Gebäudenamen und Anzahl in den Rechtecken anzeigen
			s.Format("%i x %s",pDoc->GetSystem(p.x, p.y).GetNumberOfBuilding(m_BuildingOverview.GetAt(i).runningNumber),
				pDoc->GetBuildingName(m_BuildingOverview.GetAt(i).runningNumber));
			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r1.left,r1.top,r1.Width(),r1.Height()), &fontFormat, &fontBrush);

			// Gebäudeproduktion zeichnen
			s = pDoc->GetBuildingInfo(m_BuildingOverview.GetAt(i).runningNumber).GetProductionAsString
				(pDoc->GetSystem(p.x, p.y).GetNumberOfBuilding(m_BuildingOverview.GetAt(i).runningNumber));
			SolidBrush markBrush(textMark);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+140,r.top+25,r.Width()-140,r.Height()-5), &fontFormat, &markBrush);
			// Anzahl der abzureißenden Gebäude zeichnen
			unsigned short dn = pDoc->GetSystem(p.x, p.y).GetBuildingDestroy(m_BuildingOverview.GetAt(i).runningNumber);
			if (dn > 0)
			{
				s.Format("%s: %i",CLoc::GetString("TALON"), dn);
				COverlayBanner* banner = new COverlayBanner(r.TopLeft(), r.Size(), s, RGB(255,0,0));
				banner->SetBorderWidth(2);
				banner->Draw(g, &Gdiplus::Font(CComBSTR(fontName), fontSize));
				delete banner;
			}
			spaceY++;
		}
	}

	// Wenn eine 75%ige Blockade erreicht wurde, dann ist ein Abriss nicht mehr möglich
	if (pDoc->GetSystem(p.x, p.y).GetBlockade() > NULL)
	{
		COverlayBanner *banner = new COverlayBanner(CPoint(250,250), CSize(560,150), CLoc::GetString("ONLY_PARTIAL_BUILDINGSCRAP"), RGB(200,0,0));
		banner->Draw(g, &Gdiplus::Font(CComBSTR(fontName), fontSize));
		delete banner;
	}

	// Systemnamen mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	// Name des Systems oben in der Mitte zeichnen
	s = CLoc::GetString("BUILDING_OVERVIEW_MENUE")+" "+pDoc->GetSystem(p.x,p.y).GetName();
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Energiezuweisungsansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawEnergyMenue(Gdiplus::Graphics *g)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	CPoint p = pDoc->GetCo();;
	CString s;

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

	if (bg_energymenu)
		g->DrawImage(bg_energymenu, 0, 0, 1075, 750);

	m_EnergyList.RemoveAll();
	// die Inhalte der einzelnen Buttons berechnen, max. 3 vertikal und 3 horizontal
	USHORT NumberOfBuildings = pDoc->GetSystem(p.x,p.y).GetAllBuildings()->GetSize();
	// Alle Gebäude durchgehen, diese müssen nach RunningNumber aufsteigend sortiert sein und in die Variable schreiben
	short spaceX = 0;	// Platz in x-Richtung
	short spaceY = 0;	// Platz in y-Richtung
	for (int i = 0; i < NumberOfBuildings; i++)
	{
		const CBuildingInfo *buildingInfo = &pDoc->BuildingInfo.GetAt(pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);

		// wenn das Gebäude Energie benötigt
		if (buildingInfo->GetNeededEnergy() > 0)
		{
			ENERGYSTRUCT es;
			es.index = i;
			es.status = pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(i).GetIsBuildingOnline();
			m_EnergyList.Add(es);
		}
	}

	// provisorische Buttons für vor und zurück
	Color redColor;
	redColor.SetFromCOLORREF(RGB(200,50,50));
	SolidBrush redBrush(redColor);

	if (m_EnergyList.GetSize() > m_iELPage * NOBIEL + NOBIEL)
	{
		s = ">";
		g->FillRectangle(&redBrush, RectF(1011,190,63,52));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(1011,190,63,52), &fontFormat, &fontBrush);
	}
	if (m_iELPage > 0)
	{
		s = "<";
		g->FillRectangle(&redBrush, RectF(1011,490,63,52));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(1011,490,63,52), &fontFormat, &fontBrush);
	}

	// Gebäude, welche Energie benötigen anzeigen
	SolidBrush blackBrush(static_cast<Gdiplus::ARGB>(Color::Black));
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
			CRect r;
			r.SetRect(90+spaceX*320,100+spaceY*170,320+spaceX*320,247+spaceY*170);
			g->FillRectangle(&blackBrush, RectF(r.left, r.top, r.Width(), r.Height()));
			m_EnergyList[i].rect = r;
			// Rechteck machen damit der Text oben rechts steht
			CRect tmpr;
			tmpr.SetRect(r.left+10,r.top+5,r.right-5,r.bottom);
			int id = pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetRunningNumber();

			const CBuildingInfo *buildingInfo = &pDoc->BuildingInfo.GetAt(id - 1);

			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			g->DrawString(CComBSTR(pDoc->GetBuildingName(id)), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(tmpr.left, tmpr.top, tmpr.Width(), tmpr.Height()), &fontFormat, &fontBrush);

			// nötige Energie über den Status zeichnen
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			tmpr.SetRect(r.right-70,r.bottom-100,r.right-5,r.bottom);
			s.Format("%d EP", buildingInfo->GetNeededEnergy());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(tmpr.left, tmpr.top, tmpr.Width(), tmpr.Height()), &fontFormat, &fontBrush);

			// Rechteck machen damit der Status unten rechts steht
			tmpr.SetRect(r.right-70,r.bottom-50,r.right-5,r.bottom);
			// Wenn es offline ist
			if (!m_EnergyList.GetAt(i).status)
			{
				g->DrawString(L"offline", -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(tmpr.left, tmpr.top, tmpr.Width(), tmpr.Height()), &fontFormat, &SolidBrush(static_cast<Gdiplus::ARGB>(Color::Red)));

			}
			// Wenn es online ist
			else
			{
				g->DrawString(L"online", -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(tmpr.left, tmpr.top, tmpr.Width(), tmpr.Height()), &fontFormat, &SolidBrush(static_cast<Gdiplus::ARGB>(Color::Green)));
			}

			// Das Bild zu dem Gebäude zeichnen
			CString fileName;
			fileName.Format("Buildings\\%s", buildingInfo->GetGraphikFileName());
			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(fileName);
			if (graphic == NULL)
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
			if (graphic)
			{
				g->DrawImage(graphic, r.left + 5, r.top + 32, 150, 113);
			}
			spaceY++;
		}
	}

	// Oben in der Mitte den aktuellen/verfügbaren Energiebetrag zeichnen
	int energy = pDoc->GetSystem(p.x,p.y).GetProduction()->GetEnergyProd();
	s.Format("%s: %d",CLoc::GetString("USABLE_ENERGY"), energy);
	Color energyColor;
	if (energy < 0)
		energyColor.SetFromCOLORREF(RGB(200,0,0));
	else if (energy == 0)
		energyColor.SetFromCOLORREF(RGB(200,200,0));
	else
		energyColor.SetFromCOLORREF(RGB(0,200,0));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190, 65, 675, 30), &fontFormat, &SolidBrush(energyColor));

	// Systemnamen mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	// Name des Systems oben in der Mitte zeichnen
	s = CLoc::GetString("ENERGY_MENUE")+" "+pDoc->GetSystem(p.x,p.y).GetName();
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Systemhandelsansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawSystemTradeMenue(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	CPoint p = pDoc->GetCo();;
	CString s,s2;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color textMark;
	textMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_systrademenu)
		g->DrawImage(bg_systrademenu, 0, 0, 1075, 750);

	SolidBrush markBrush(textMark);
	g->DrawString(CComBSTR(CLoc::GetString("TRADE_AND_RESOURCEROUTES")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,80,538,25), &fontFormat, &markBrush);
	g->DrawString(CComBSTR(CLoc::GetString("STELLAR_STORAGE")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,80,538,25), &fontFormat, &markBrush);
	g->DrawString(CComBSTR(CLoc::GetString("SYSTEM_STORAGE")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,140,268,25), &fontFormat, &markBrush);
	g->DrawString(CComBSTR(CLoc::GetString("STELLAR_STORAGE")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(806,140,268,25), &fontFormat, &markBrush);
	g->DrawString(CComBSTR(CLoc::GetString("TITAN")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,220,538,60), &fontFormat, &markBrush);
	g->DrawString(CComBSTR(CLoc::GetString("DEUTERIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,280,538,60), &fontFormat, &markBrush);
	g->DrawString(CComBSTR(CLoc::GetString("DURANIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,340,538,60), &fontFormat, &markBrush);
	g->DrawString(CComBSTR(CLoc::GetString("CRYSTAL")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,400,538,60), &fontFormat, &markBrush);
	g->DrawString(CComBSTR(CLoc::GetString("IRIDIUM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,460,538,60), &fontFormat, &markBrush);

	USHORT maxTradeRoutes = (USHORT)(pDoc->GetSystem(p.x, p.y).GetHabitants() / TRADEROUTEHAB) + pDoc->GetSystem(p.x, p.y).GetProduction()->GetAddedTradeRoutes();
	short addResRoute = 1;
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		if (maxTradeRoutes == NULL)
		{
			maxTradeRoutes += pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(3);
			addResRoute += pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::TRADE)->GetBonus(3);
		}
	s.Format("%d", maxTradeRoutes);
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> eine Ressourcenroute mehr
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(3) == RESEARCH_STATUS::RESEARCHED)
		addResRoute += pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(3);
	USHORT maxResourceRoutes = (USHORT)(pDoc->GetSystem(p.x, p.y).GetHabitants() / TRADEROUTEHAB) + pDoc->GetSystem(p.x, p.y).GetProduction()->GetAddedTradeRoutes() + addResRoute;
	s2.Format("%d", maxResourceRoutes);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(CComBSTR(CLoc::GetString("SYSTEM_SUPPORTS_ROUTES",0,s,s2)), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(60,140,440,40), &fontFormat, &fontBrush);

	s.Format("%d",pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetSize());
	s2.Format("%d",pDoc->GetSystem(p.x, p.y).GetResourceRoutes()->GetSize());
	g->DrawString(CComBSTR(CLoc::GetString("SYSTEM_HAS_ROUTES",0,s,s2)), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(60,180,440,40), &fontFormat, &fontBrush);

	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(CComBSTR(CLoc::GetString("ROUTES_TO")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(60,220,440,30), &fontFormat, &fontBrush);

	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
	// Anzeige von max. NOTRIL Handelsrouten
	// prüfen, dass man nicht auf einer zu hohen Seite ist, wenn zu wenig Handelsrouten vorhanden sind
	if (m_iSTPage * NOTRIL >= pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetSize())
		m_iSTPage = 0;
	int numberOfTradeRoutes = 0;
	// zuerst die Handelsrouten anzeigen
	for (int i = m_iSTPage * NOTRIL; i < pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetSize(); i++)
	{
		CPoint dest = pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetAt(i).GetDestKO();
		s = pDoc->GetSystem(dest.x, dest.y).CoordsName(
			pDoc->GetSystem(dest.x, dest.y).GetKnown(pDoc->GetSystem(p.x, p.y).OwnerID())
		);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(70,260+i*30,125,25), &fontFormat, &fontBrush);

		// Gewinn inkl. der Boni auf Handelsrouten ohne Boni auf Credits und Boni durch Moral
		USHORT lat = pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetAt(i).GetCredits(pDoc->GetSystem(p.x, p.y).GetProduction()->GetIncomeOnTradeRoutes());
		s.Format("%s: %d %s",CLoc::GetString("PROFIT"), lat, CLoc::GetString("CREDITS"));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(200,260+i*30,170,25), &fontFormat, &fontBrush);

		// verbleibende Dauer der Handelsroute anzeigen
		short duration = pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetAt(i).GetDuration();
		if (duration < 0)
			duration = 6-abs(duration);
		if (duration > 1)
			s.Format("%s %d %s",CLoc::GetString("STILL"), duration, CLoc::GetString("ROUNDS"));
		else
			s.Format("%s %d %s",CLoc::GetString("STILL"), duration, CLoc::GetString("ROUND"));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(380,260+i*30,130,25), &fontFormat, &fontBrush);
		numberOfTradeRoutes = i + 1;
		if (i >= m_iSTPage * NOTRIL + NOTRIL - 1)
			break;
	}
	// jetzt die Ressourcenrouten anzeigen
	for (int i = 0; i < pDoc->GetSystem(p.x, p.y).GetResourceRoutes()->GetSize(); i++)
	{
		int j = i + numberOfTradeRoutes;
		CPoint dest = pDoc->GetSystem(p.x, p.y).GetResourceRoutes()->GetAt(i).GetCo();
		s.Format("%s", pDoc->GetSystem(dest.x, dest.y).GetName());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(70,260+j*30,125,25), &fontFormat, &markBrush);
		switch (pDoc->GetSystem(p.x, p.y).GetResourceRoutes()->GetAt(i).GetResource())
		{
		case RESOURCES::TITAN:		s = CLoc::GetString("TITAN"); break;
		case RESOURCES::DEUTERIUM: s = CLoc::GetString("DEUTERIUM"); break;
		case RESOURCES::DURANIUM:	s = CLoc::GetString("DURANIUM"); break;
		case RESOURCES::CRYSTAL:	s = CLoc::GetString("CRYSTAL"); break;
		case RESOURCES::IRIDIUM:	s = CLoc::GetString("IRIDIUM"); break;
		default:		s = "";
		}
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(200,260+j*30,150,25), &fontFormat, &markBrush);
		if (j >= m_iSTPage * NOTRIL + NOTRIL - 1)
			break;
	}

	// Inhalte des system- und globalen Lagers zeichnen
	Gdiplus::Color penMark;
	penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	fontFormat.SetTrimming(StringTrimmingNone);
	for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
	{
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format("%d",pDoc->GetSystem(p.x, p.y).GetResourceStore(i));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(668,220+i*60,407,60), &fontFormat, &fontBrush);
		// in Klammern darunter, wieviel Ressourcen ich nächste Runde aus diesem System ins Globale Lager verschiebe
		s.Format("(%d)", pMajor->GetEmpire()->GetGlobalStorage()->GetSubResource(i,p) - pMajor->GetEmpire()->GetGlobalStorage()->GetAddedResource(i,p));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(668,265+i*60,407,25), &fontFormat, &fontBrush);
		// globalen Lagerinhalt zeichnen
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%d", pMajor->GetEmpire()->GetGlobalStorage()->GetResourceStorage(i));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,220+i*60,407,60), &fontFormat, &fontBrush);
		// in Klammern steht, wieviel dieser Ressource nächste Runde aus dem Lager entfernt wird
		s.Format("(%d)", pMajor->GetEmpire()->GetGlobalStorage()->GetAllAddedResource(i) - pMajor->GetEmpire()->GetGlobalStorage()->GetAllSubResource(i));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,265+i*60,407,25), &fontFormat, &fontBrush);
	}
	// Die Buttons zum Verändern der Mengen im stellaren Lager zeichnen
	// Buttons zeichnen
	for (int i = 0; i < m_SystemTradeButtons.GetSize(); i++)
		m_SystemTradeButtons[i]->DrawButton(*g, pDoc->GetGraphicPool(), Gdiplus::Font(NULL), fontBrush);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	s.Format("%d",pMajor->GetEmpire()->GetGlobalStorage()->GetTakenRessources());
	s2.Format("%d",pMajor->GetEmpire()->GetGlobalStorage()->GetMaxTakenRessources());
	g->DrawString(CComBSTR(CLoc::GetString("TAKE_FROM_STORAGE",0,s,s2)), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,190,538,25), &fontFormat, &fontBrush);

	s.Format("%d",pMajor->GetEmpire()->GetGlobalStorage()->GetLosing());
	g->DrawString(CComBSTR(CLoc::GetString("LOST_PER_ROUND",0,s)), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,550,538,30), &fontFormat, &fontBrush);

	fontFormat.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(CLoc::GetString("MULTIPLIER")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(538,600,182,30), &fontFormat, &fontBrush);

	// kleine Buttons zeichnen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);
	fontFormat.SetAlignment(StringAlignmentCenter);

	// Button zum Anlegen einer Handelsroute zeichnen
	if (graphic)
		g->DrawImage(graphic, 60, 600, 120, 30);
	s = CLoc::GetString("BTN_TRADEROUTE");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(60, 600, 120, 30), &fontFormat, &btnBrush);

	// Button zum Anlegen einer Ressourcenroute zeichnen
	if (graphic)
		g->DrawImage(graphic, 360, 600, 120, 30);
	s = CLoc::GetString("BTN_RESOURCEROUTE");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360, 600, 120, 30), &fontFormat, &btnBrush);

	// Button zum Ändern der Ressource bei einer Ressourcenroute
	if (graphic)
		g->DrawImage(graphic, 360, 640, 120, 30);
	switch (m_byResourceRouteRes)
	{
	case RESOURCES::TITAN:		s = CLoc::GetString("TITAN"); break;
	case RESOURCES::DEUTERIUM: s = CLoc::GetString("DEUTERIUM"); break;
	case RESOURCES::DURANIUM:	s = CLoc::GetString("DURANIUM"); break;
	case RESOURCES::CRYSTAL:	s = CLoc::GetString("CRYSTAL"); break;
	case RESOURCES::IRIDIUM:	s = CLoc::GetString("IRIDIUM"); break;
	default:		s = "";
	}
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360, 640, 120, 30), &fontFormat, &btnBrush);

	// Buttons zum Kündigen/Aufheben einer Ressourcenroute zeichnen
	for (int i = 0; i < pDoc->GetSystem(p.x, p.y).GetResourceRoutes()->GetSize(); i++)
	{
		int j = i + numberOfTradeRoutes;
		if (graphic)
			g->DrawImage(graphic, 360, 260 + j * 30, 120, 30);
		g->DrawString(CComBSTR(CLoc::GetString("BTN_ANNUL")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(360, 260+j*30, 120, 30), &fontFormat, &btnBrush);
	}

	// Button zum Ändern der Menge, wieviel pro Klick vom oder ins Globale Lager verschoben werden zeichnen
	if (graphic)
		g->DrawImage(graphic, 747, 600, 120, 30);
	s.Format("%u",m_iGlobalStoreageQuantity);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(747, 600, 120, 30), &fontFormat, &btnBrush);

	// Systemnamen mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	// Name des Systems oben in der Mitte zeichnen
	s.Format("%s %s",CLoc::GetString("TRADEOVERVIEW_IN"),pDoc->GetSystem(p.x,p.y).GetName());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormat, &fontBrush);
}


/////////////////////////////////////////////////////////////////////////////////////////
// Ab hier die ganzen Buttons mit ihrer Beschreibung am unteren Bildschirmrand
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawButtonsUnderSystemView(Graphics* g)
{
	AssertBotE((CBotEDoc*)GetDocument());

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush fontBrush(btnColor);

	DrawGDIButtons(g, &m_BuildMenueMainButtons, m_bySubMenu, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Funktion zeichnet die Bauliste in der Baumenü- und Arbeitermenüansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawBuildList(Graphics* g)
{
	// Hier die Einträge in der Bauliste
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	SolidBrush fontBrush(static_cast<Gdiplus::ARGB>(Color::White));

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);

	Color firstColor;
	firstColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	SolidBrush usedBrush(normalColor);

	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	Pen rectPen(markColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	CPoint p = pDoc->GetCo();
	CString m_strAssemblyListEntry("");

	int y = 410;
	for (int i = 0; i < ALE; i++)
	{
		int nAssemblyListEntry = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetAssemblyListEntry(i);
		if (nAssemblyListEntry != 0)
		{
			// ersten Eintrag in der Bauliste
			if (i == 0)
			{
				// Farbe der Schrift für den ersten Eintrag in der Bauliste wählen
				usedBrush.SetColor(firstColor);
			}

			CString sFile;
			// ist es ein Update
			if (nAssemblyListEntry < 0)
			{
				m_strAssemblyListEntry = CLoc::GetString("UPGRADING", FALSE, pDoc->GetBuildingName(abs(nAssemblyListEntry)));
				sFile = "Buildings\\" + pDoc->GetBuildingInfo(abs(nAssemblyListEntry)).GetGraphikFileName();
			}
			// ist es ein Gebäude
			else if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i) < 10000)
			{
				m_strAssemblyListEntry = pDoc->GetBuildingName(nAssemblyListEntry);
				sFile = "Buildings\\" + pDoc->GetBuildingInfo(nAssemblyListEntry).GetGraphikFileName();
			}
			// ist es ein Schiff
			else if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i) < 20000)
			{
				m_strAssemblyListEntry.Format("%s-%s",pDoc->m_ShipInfoArray.GetAt(nAssemblyListEntry - 10000).GetShipClass(), CLoc::GetString("CLASS"));
				sFile = "Ships\\" + pDoc->m_ShipInfoArray.GetAt(nAssemblyListEntry - 10000).GetShipClass() + ".bop";
			}
			// ist es eine Truppe
			else
			{
				m_strAssemblyListEntry = pDoc->m_TroopInfo.GetAt(nAssemblyListEntry - 20000).GetName();
				sFile = "Troops\\" + pDoc->m_TroopInfo.GetAt(nAssemblyListEntry - 20000).GetGraphicFileName();
			}

			fontFormat.SetAlignment(StringAlignmentNear);
			CRect rect(760,y,1000,y+24);
			g->DrawString(CComBSTR(m_strAssemblyListEntry), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(rect.left + 25, rect.top, (rect.right - rect.left) - 25, rect.bottom - rect.top), &fontFormat, &usedBrush);

			// kleines Symbol in der Bauliste vom Auftrag zeichnen
			if (sFile != "")
			{
				Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(sFile);
				if (graphic)
					g->DrawImage(graphic, 750, y, 25, 20);
			}
			g->DrawRectangle(&rectPen, 749, y-1, 27, 22);

			m_strAssemblyListEntry.Format("%i", pDoc->GetSystem(p.x, p.y).NeededRoundsToBuild(i, true));
			fontFormat.SetAlignment(StringAlignmentFar);
			g->DrawString(CComBSTR(m_strAssemblyListEntry), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(765,y,260,25), &fontFormat, &usedBrush);
			y += 25;
			usedBrush.SetColor(normalColor);
		}
	}

	// Wenn nix in der Bauliste steht, automatisch Handelsgüter reinschreiben
	if (pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetAssemblyListEntry(0) == 0)
	{
		usedBrush.SetColor(firstColor);
		g->DrawString(CComBSTR(CLoc::GetString("COMMODITIES")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(760,410,285,25), &fontFormat, &usedBrush);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier werden die Informationen zur Produktion usw. oben rechts in der Systemansicht ausgegeben
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawSystemProduction(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);

	Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	SolidBrush fontBrush(penColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	CString s;
	CPoint p;
	p = pDoc->GetCo();

	// Die Rohstoffe und sonstige Informationen im System oben in der Mitte anzeigen
	// Lager und Rohstoffe rechts zeichnen
	g->DrawString(CComBSTR(CLoc::GetString("PRODUCTION")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(870,20), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("STORAGE")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(980,20), &fontFormat, &fontBrush);

	fontBrush.SetColor(normalColor);
	g->DrawString(CComBSTR(CLoc::GetString("FOOD")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,55), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("INDUSTRY")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,80), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("ENERGY")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,105), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("SECURITY")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,130), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("RESEARCH")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,155), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("TITAN")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,180), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("DEUTERIUM")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,205), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("DURANIUM")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,230), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("CRYSTAL")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,255), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("IRIDIUM")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,280), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("DERITIUM")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,305), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("MORAL")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,330), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CLoc::GetString("CREDITS")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(775,355), &fontFormat, &fontBrush);

	fontFormat.SetAlignment(StringAlignmentCenter);
	// Produktion anzeigen
	RectF rect(870,55,60,25);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetFoodProd());
	if (atoi(s) < 0)
	{
		fontBrush.SetColor(static_cast<Gdiplus::ARGB>(Color::Red));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
		fontBrush.SetColor(normalColor);
	}
	else
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);

	rect = RectF(870,80,60,25);
	if (m_iWhichSubMenu == 0)		// Wenn wir im normalen Baumenü sind
	{
		if (pDoc->GetSystem(p.x,p.y).GetProduction()->GetUpdateBuildSpeed() ==
			pDoc->GetSystem(p.x,p.y).GetProduction()->GetBuildingBuildSpeed())
		{
			s.Format("%i",(short)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() *
				(100 + pDoc->GetSystem(p.x, p.y).GetProduction()->GetBuildingBuildSpeed()) / 100));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
		}
		else
		{
			s.Format("%i",(pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd()));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
		}
	}
	else if (m_iWhichSubMenu == 1)	// Wenn wir im Werftuntermenü sind
	{
		s.Format("%i",(int)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() *
			pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency() / 100 *
			(100 + pDoc->GetSystem(p.x, p.y).GetProduction()->GetShipBuildSpeed()) / 100));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}
	else							// Wenn wir im Kasernenuntermenü sind
	{
		s.Format("%i",(short)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() *
			pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency() / 100 *
			(100 + pDoc->GetSystem(p.x, p.y).GetProduction()->GetTroopBuildSpeed()) / 100));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}

	rect = RectF(870,105,60,25);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetEnergyProd());
	if (atoi(s) < 0)
	{
		fontBrush.SetColor(static_cast<Gdiplus::ARGB>(Color::Red));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
		fontBrush.SetColor(normalColor);
	}
	else
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);

	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
	{
		rect.Y += 25;
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetResourceProd(res));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}
	// Moralproduktion
	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetMoralProd());
	if(pDoc->GetSystem(p.x,p.y).GetProduction()->GetMoralProd()<0) fontBrush.SetColor(Color(255,0,0));
	else if(pDoc->GetSystem(p.x,p.y).GetProduction()->GetMoralProd()>0) fontBrush.SetColor(Color(0,255,0));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	fontBrush.SetColor(normalColor);

	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetCreditsProd());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);

	// ab hier Lager anzeigen
	rect = RectF(950,55,100,25);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetFoodStore());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	// Wenn wir im normalen Baumenü sind
	if (m_iWhichSubMenu == 0 &&
			(pDoc->GetSystem(p.x,p.y).GetProduction()->GetBuildingBuildSpeed() > 0 ||
			pDoc->GetSystem(p.x, p.y).GetProduction()->GetUpdateBuildSpeed() > 0))
	{
		if (pDoc->GetSystem(p.x,p.y).GetProduction()->GetBuildingBuildSpeed() ==
				pDoc->GetSystem(p.x,p.y).GetProduction()->GetUpdateBuildSpeed())
		{
			s.Format("%i%%",(short)(100 + pDoc->GetSystem(p.x,p.y).GetProduction()->GetBuildingBuildSpeed()));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
		}
		else
		{
			s.Format("%i%%   %i%%",(short)(100 + pDoc->GetSystem(p.x,p.y).GetProduction()->GetBuildingBuildSpeed()),
				(short)(100 + pDoc->GetSystem(p.x,p.y).GetProduction()->GetUpdateBuildSpeed()));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
		}
	}
	else if (m_iWhichSubMenu == 1)		// Wenn wir im Werftuntermenü sind
	{
		s.Format("%i%%",(short)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency() *
			(100 + pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipBuildSpeed()) / 100));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}
	else if (m_iWhichSubMenu == 2)	// Wenn wir im Kasernenuntermenü sind
	{
		s.Format("%i%%",(short)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency() *
			(100 + pDoc->GetSystem(p.x,p.y).GetProduction()->GetTroopBuildSpeed()) / 100));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}

	rect = RectF(950,155,100,25);
	// Zusätzliche Ressourcen aus den Startsystemen von Ressourcenrouten ermitteln
	ULONG resFromRoutes[RESOURCES::DERITIUM + 1] = {0};
	ULONG nResInDistSys[RESOURCES::DERITIUM + 1]	= {0};
	for (int j = 0; j < pMajor->GetEmpire()->GetSystemList()->GetSize(); j++)
		if (pMajor->GetEmpire()->GetSystemList()->GetAt(j).ko != p)
		{
			CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(j).ko;
			// Wenn unser System blockiert wird so gelten die Ressourcenrouten nicht
			if (pDoc->GetSystem(ko.x, ko.y).GetBlockade() > NULL)
				continue;

			for (int i = 0; i < pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetSize(); i++)
			{
				CPoint ptTarget = pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(i).GetCo();
				// ist das Ziel das aktuelle System?
				if (ptTarget == p)
				{
					// Wenn das Startsystem blockiert wird, so kann die Ressourcenroute ebenfalls nicht benutzt werden
					if (pDoc->GetSystem(ptTarget.x, ptTarget.y).GetBlockade() > NULL)
						continue;

					BYTE res = pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(i).GetResource();
					resFromRoutes[res] += pDoc->GetSystem(ko.x, ko.y).GetResourceStore(res);
				}
			}
			// gilt nicht bei blockierten Systemen
			if (pDoc->GetSystem(p.x, p.y).GetBlockade() == NULL)
				for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
					if (pDoc->GetSystem(ko.x, ko.y).GetProduction()->GetResourceDistributor(res))
						nResInDistSys[res] = pDoc->GetSystem(ko.x, ko.y).GetResourceStore(res);
		}

	for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
	{
		rect.Y += 25;
		if (nResInDistSys[res] > resFromRoutes[res] + pDoc->GetSystem(p.x, p.y).GetResourceStore(res))
			s.Format("[%lu]", nResInDistSys[res]);
		else if (resFromRoutes[res] > 0)
			s.Format("(%i)",pDoc->GetSystem(p.x,p.y).GetResourceStore(res) + resFromRoutes[res]);
		else
			s.Format("%i",pDoc->GetSystem(p.x,p.y).GetResourceStore(res));

		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}

	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetMoral());
	if (pDoc->GetSystem(p.x,p.y).GetMoral() > 174) fontBrush.SetColor(Color(0,250,0)); //Fanatic
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 154) fontBrush.SetColor(Color(20,150,20)); //Loyal
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 130) fontBrush.SetColor(Color(20,150,100)); //Pleased
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 99)  fontBrush.SetColor(Color(150,150,200)); //Satisfied
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 75)  fontBrush.SetColor(Color(160,160,160)); //Apathetic
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 49)  fontBrush.SetColor(Color(200,100,50)); //Angry
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 29)  fontBrush.SetColor(Color(210,80,50)); //Furious
	else fontBrush.SetColor(Color(255,0,0));//Rebellious

	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);

	// kleine Bilder von den Rohstoffen zeichnen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\foodSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 55, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\industrySmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 80, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\energySmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 105, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\securitySmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 130, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\researchSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 155, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 180, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 205, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 230, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 255, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 280, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\Deritium.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 305, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\moralSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 330, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\creditsSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 355, 20, 16);
}

// Funktion berechnet und zeichnet im "rect" die Produktion eins Gebäudes, also alles was es macht
void CSystemMenuView::DrawBuildingProduction(Graphics* g)
{
	if (m_iClickedOn >= m_vBuildlist.GetSize())
		return;

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);

	RectF r(BuildingDescription.left, BuildingDescription.top + 5, BuildingDescription.right - BuildingDescription.left, 25);
	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);

	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	SolidBrush fontBrush(penColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	int RunningNumber = abs(m_vBuildlist[m_iClickedOn]);

	// Oben im Beschreibungsrechteck den Namen des Projektes hinschreiben
	// Ist es ein Gebäude oder ein Upgrade eines Gebäudes, welches aber keine Arbeiter benötigt
	if (m_vBuildlist[m_iClickedOn] > 0 || pDoc->GetBuildingInfo(RunningNumber).GetWorker() == FALSE)
	{
		CBuildingInfo* b = &pDoc->GetBuildingInfo(RunningNumber);

		s.Format("%s",pDoc->GetBuildingName(RunningNumber));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
		r.Y += 30;
		fontBrush.SetColor(markColor);
		g->DrawString(CComBSTR(CLoc::GetString("PRODUCTION")+":"), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
		r.Y += 22;
		fontBrush.SetColor(normalColor);

		if (b->GetFoodProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("FOOD"), b->GetFoodProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIPProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("INDUSTRY"), b->GetIPProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetEnergyProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("ENERGY"), b->GetEnergyProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetSPProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("SECURITY"), b->GetSPProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetFPProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("RESEARCH"), b->GetFPProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetTitanProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("TITAN"), b->GetTitanProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDeuteriumProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("DEUTERIUM"), b->GetDeuteriumProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDuraniumProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("DURANIUM"), b->GetDuraniumProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetCrystalProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("CRYSTAL"), b->GetCrystalProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIridiumProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("IRIDIUM"), b->GetIridiumProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDeritiumProd() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("DERITIUM"), b->GetDeritiumProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetCredits() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("CREDITS"), b->GetCredits());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetMoralProd() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("MORAL"), b->GetMoralProd());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetMoralProdEmpire() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("MORAL_EMPIREWIDE"), b->GetMoralProdEmpire());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}

		// Ab hier die Boni
		if (b->GetFoodBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("FOOD_BONUS"), b->GetFoodBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIndustryBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("INDUSTRY_BONUS"), b->GetIndustryBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetEnergyBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("ENERGY_BONUS"), b->GetEnergyBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetSecurityBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("SECURITY_BONUS"), b->GetSecurityBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResearchBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("RESEARCH_BONUS"), b->GetResearchBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetTitanBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("TITAN_BONUS"), b->GetTitanBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDeuteriumBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("DEUTERIUM_BONUS"), b->GetDeuteriumBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDuraniumBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("DURANIUM_BONUS"), b->GetDuraniumBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetCrystalBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("CRYSTAL_BONUS"), b->GetCrystalBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIridiumBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("IRIDIUM_BONUS"), b->GetIridiumBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDeritiumBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("DERITIUM_BONUS"), b->GetDeritiumBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetAllRessourcesBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("BONUS_TO_ALL_RES"), b->GetAllRessourcesBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetCreditsBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("CREDITS_BONUS"), b->GetCreditsBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		// Wenn alle Forschungsboni belegt sind, sprich Bonus auf alles
		if (b->GetBioTechBoni() != 0 && b->GetBioTechBoni() == b->GetEnergyTechBoni() && b->GetBioTechBoni() == b->GetCompTechBoni()
			&& b->GetBioTechBoni() == b->GetPropulsionTechBoni() && b->GetBioTechBoni() == b->GetConstructionTechBoni()
			&& b->GetBioTechBoni() == b->GetWeaponTechBoni())
		{
			s.Format("%s: %i%%",CLoc::GetString("TECHNIC_BONUS"), b->GetBioTechBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		else // bei einzelnen Gebieten
		{
			if (b->GetBioTechBoni() > 0)
			{
				s.Format("%s: %i%%",CLoc::GetString("BIOTECH_BONUS"), b->GetBioTechBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetEnergyTechBoni() > 0)
			{
				s.Format("%s: %i%%",CLoc::GetString("ENERGYTECH_BONUS"), b->GetEnergyTechBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetCompTechBoni() > 0)
			{
				s.Format("%s: %i%%",CLoc::GetString("COMPUTERTECH_BONUS"), b->GetCompTechBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetPropulsionTechBoni() > 0)
			{
				s.Format("%s: %i%%",CLoc::GetString("PROPULSIONTECH_BONUS"), b->GetPropulsionTechBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetConstructionTechBoni() > 0)
			{
				s.Format("%s: %i%%",CLoc::GetString("CONSTRUCTIONTECH_BONUS"), b->GetConstructionTechBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetWeaponTechBoni() > 0)
			{
				s.Format("%s: %i%%",CLoc::GetString("WEAPONTECH_BONUS"), b->GetWeaponTechBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
		}
		// Wenn alle Geheimdienstdinger belegt sind -> Geheimdienst auf alles
		if (b->GetInnerSecurityBoni() != 0 && b->GetInnerSecurityBoni() == b->GetEconomySpyBoni()
			&& b->GetInnerSecurityBoni() ==	b->GetEconomySabotageBoni() && b->GetInnerSecurityBoni() == b->GetResearchSpyBoni()
			&& b->GetInnerSecurityBoni() == b->GetResearchSabotageBoni() && b->GetInnerSecurityBoni() == b->GetMilitarySpyBoni()
			&& b->GetInnerSecurityBoni() == b->GetMilitarySabotageBoni())
		{
			s.Format("%s: %i%%",CLoc::GetString("COMPLETE_SECURITY_BONUS"), b->GetInnerSecurityBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		else
		{
			if (b->GetEconomySpyBoni() != 0 && b->GetEconomySpyBoni() == b->GetResearchSpyBoni()
				&& b->GetEconomySpyBoni() == b->GetMilitarySpyBoni())
			{
				s.Format("%s: %i%%",CLoc::GetString("SPY_BONUS"), b->GetEconomySpyBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			else
			{
				if (b->GetEconomySpyBoni() != 0)
				{
					s.Format("%s: %i%%",CLoc::GetString("ECONOMY_SPY_BONUS"), b->GetEconomySpyBoni());
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
				if (b->GetResearchSpyBoni() != 0)
				{
					s.Format("%s: %i%%",CLoc::GetString("RESEARCH_SPY_BONUS"), b->GetResearchSpyBoni());
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
				if (b->GetMilitarySpyBoni() != 0)
				{
					s.Format("%s: %i%%",CLoc::GetString("MILITARY_SPY_BONUS"), b->GetMilitarySpyBoni());
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
			}
			if (b->GetEconomySabotageBoni() != 0 && b->GetEconomySabotageBoni() == b->GetResearchSabotageBoni()
				&& b->GetEconomySabotageBoni() == b->GetMilitarySabotageBoni())
			{
				s.Format("%s: %i%%",CLoc::GetString("SABOTAGE_BONUS"), b->GetEconomySabotageBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			else
			{
				if (b->GetEconomySabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CLoc::GetString("ECONOMY_SABOTAGE_BONUS"), b->GetEconomySabotageBoni());
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
				if (b->GetResearchSabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CLoc::GetString("RESEARCH_SABOTAGE_BONUS"), b->GetResearchSabotageBoni());
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
				if (b->GetMilitarySabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CLoc::GetString("MILITARY_SABOTAGE_BONUS"), b->GetMilitarySabotageBoni());
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
			}
			if (b->GetInnerSecurityBoni() != 0)
			{
				s.Format("%s: %i%%",CLoc::GetString("INNER_SECURITY_BONUS"), b->GetInnerSecurityBoni());
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
		}
		// Ab hier zusätzliche Eigenschaften des Gebäudes
		if (b->GetShipYard() == TRUE)
		{
			s = CLoc::GetString("SHIPYARD");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
			switch (b->GetMaxBuildableShipSize())
			{
			case 0: s = CLoc::GetString("SMALL"); break;
			case 1: s = CLoc::GetString("MIDDLE"); break;
			case 2: s = CLoc::GetString("BIG"); break;
			case 3: s = CLoc::GetString("HUGE"); break;
			default: s = CLoc::GetString("HUGE");
			}
			CString ss;
			ss.Format("%s: %s",CLoc::GetString("MAX_SHIPSIZE"), s);
			g->DrawString(CComBSTR(ss), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipYardSpeed() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("SHIPYARD_EFFICIENCY"), b->GetShipYardSpeed());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetBarrack() == TRUE)
		{
			s = CLoc::GetString("BARRACK");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetBarrackSpeed() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("BARRACK_EFFICIENCY"), b->GetBarrackSpeed());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShieldPower() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("SHIELDPOWER"), b->GetShieldPower());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShieldPowerBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("SHIELDPOWER_BONUS"), b->GetShieldPowerBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipDefend() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("SHIPDEFEND"), b->GetShipDefend());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipDefendBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("SHIPDEFEND_BONUS"), b->GetShipDefendBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetGroundDefend() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("GROUNDDEFEND"), b->GetGroundDefend());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetGroundDefendBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("GROUNDDEFEND_BONUS"), b->GetGroundDefendBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetScanPower() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("SCANPOWER"), b->GetScanPower());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetScanPowerBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("SCANPOWER_BONUS"), b->GetScanPowerBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetScanRange() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("SCANRANGE"), b->GetScanRange());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetScanRangeBoni() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("SCANRANGE_BONUS"), b->GetScanRangeBoni());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipTraining() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("SHIPTRAINING"), b->GetShipTraining());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetTroopTraining() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("TROOPTRAINING"), b->GetTroopTraining());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResistance() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("RESISTANCE"), b->GetResistance());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetAddedTradeRoutes() != 0)
		{
			s.Format("%s: %i",CLoc::GetString("ADDED_TRADEROUTES"), b->GetAddedTradeRoutes());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIncomeOnTradeRoutes() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("INCOME_ON_TRADEROUTES"), b->GetIncomeOnTradeRoutes());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipRecycling() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("SHIPRECYCLING"), b->GetShipRecycling());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetBuildingBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("BUILDING_BUILDSPEED"), b->GetBuildingBuildSpeed());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetUpdateBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("UPGRADE_BUILDSPEED"), b->GetUpdateBuildSpeed());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("SHIP_BUILDSPEED"), b->GetShipBuildSpeed());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetTroopBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CLoc::GetString("TROOP_BUILDSPEED"), b->GetTroopBuildSpeed());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(RESOURCES::TITAN))
		{
			s.Format("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("TITAN"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(RESOURCES::DEUTERIUM))
		{
			s.Format("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("DEUTERIUM"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(RESOURCES::DURANIUM))
		{
			s.Format("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("DURANIUM"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(RESOURCES::CRYSTAL))
		{
			s.Format("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("CRYSTAL"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(RESOURCES::IRIDIUM))
		{
			s.Format("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("IRIDIUM"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(RESOURCES::DERITIUM))
		{
			s.Format("%s - %s\n", CLoc::GetString("RESOURCE_DISTRIBUTOR"), CLoc::GetString("DERITIUM"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}

		// Ab hier die Vorraussetzungen
		fontBrush.SetColor(markColor);
		s = CLoc::GetString("PREREQUISITES")+":";
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
		r.Y += 22;
		fontBrush.SetColor(normalColor);
		// benötigte Systeme
		if (b->GetNeededSystems() != 0)
		{
			s.Format("%s: %i\n", CLoc::GetString("NEEDED_SYSTEMS"), b->GetNeededSystems());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		// max X mal von ID pro Imperium
		if (b->GetMaxInEmpire() > 0)
		{
			if (b->GetMaxInEmpire() == 1)
			{
				s = CLoc::GetString("ONCE_PER_EMPIRE");
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			else
			{
				s.Format("%d",b->GetMaxInEmpire());
				g->DrawString(CComBSTR(CLoc::GetString("MAX_PER_EMPIRE",FALSE,s)), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
		}
		// max X mal von ID pro System
		if (b->GetMaxInSystem().Number > 0 && b->GetMaxInSystem().RunningNumber == b->GetRunningNumber())
		{
			if (b->GetMaxInSystem().Number == 1)
			{
				g->DrawString(CComBSTR(CLoc::GetString("ONCE_PER_SYSTEM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			else
			{
				s.Format("%d",b->GetMaxInSystem().Number);
				g->DrawString(CComBSTR(CLoc::GetString("MAX_PER_SYSTEM",FALSE,s)), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
		}
		else if (b->GetMaxInSystem().Number > 0 && b->GetMaxInSystem().RunningNumber != b->GetRunningNumber())
		{
			s.Format("%d",b->GetMaxInSystem().Number);
			g->DrawString(CComBSTR(CLoc::GetString("MAX_ID_PER_SYSTEM",FALSE,s,pDoc->GetBuildingName(b->GetMaxInSystem().RunningNumber))), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		// min X mal von ID pro System
		if (b->GetMinInSystem().Number > 0)
		{
			s.Format("%d",b->GetMinInSystem().Number);
			g->DrawString(CComBSTR(CLoc::GetString("MIN_PER_SYSTEM",FALSE,s,pDoc->GetBuildingName(b->GetMinInSystem().RunningNumber))), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetOnlyHomePlanet() == TRUE)
		{
			g->DrawString(CComBSTR(CLoc::GetString("ONLY_HOMEPLANET")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetOnlyOwnColony() == TRUE)
		{
			g->DrawString(CComBSTR(CLoc::GetString("ONLY_OWN_COLONY")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetOnlyMinorRace() == TRUE)
		{
			g->DrawString(CComBSTR(CLoc::GetString("ONLY_MINORRACE_SYSTEM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetOnlyTakenSystem() == TRUE)
		{
			g->DrawString(CComBSTR(CLoc::GetString("ONLY_TAKEN_SYSTEM")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetMinHabitants() > 0)
		{
			s.Format("%d",b->GetMinHabitants());
			g->DrawString(CComBSTR(CLoc::GetString("NEED_MIN_HABITANTS",FALSE,s)), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetNeededEnergy() > 0)
		{
			s.Format("%s: %i",CLoc::GetString("ENERGY"),b->GetNeededEnergy());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetWorker() == TRUE)
		{
			g->DrawString(CComBSTR(CLoc::GetString("NEED_WORKER")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
	}

	// Wenn es sich um ein Upgrade handelt:
	else if (m_vBuildlist[m_iClickedOn] < 0)
	{
		CPoint p = pDoc->GetCo();
		// aktuelles Gebäude, also Gebäude welches geupdated werden soll
		CBuildingInfo* b1 = &pDoc->GetBuildingInfo(pDoc->BuildingInfo.GetAt(RunningNumber - 1).GetPredecessorID());
		if (b1->GetWorker())
		{
			// Nachfolger des aktuellen Gebäudes
			CBuildingInfo* b2 = &pDoc->GetBuildingInfo(RunningNumber);
			fontBrush.SetColor(penColor);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			fontFormat.SetAlignment(StringAlignmentNear);
			r.Height = 50;
			s = CLoc::GetString("UPGRADE_FROM_TO",FALSE,pDoc->GetBuildingName(pDoc->BuildingInfo.GetAt(RunningNumber - 1).GetPredecessorID()),pDoc->GetBuildingName(RunningNumber));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 75;
			r.Height = 25;

			fontBrush.SetColor(markColor);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			fontFormat.SetAlignment(StringAlignmentCenter);

			g->DrawString(CComBSTR(CLoc::GetString("RELATIVE_PROFIT")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 25;

			fontBrush.SetColor(normalColor);
			if (b1->GetFoodProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::FOOD_WORKER,0);
				s.Format("%i %s",b2->GetFoodProd()*number-b1->GetFoodProd()*number, CLoc::GetString("FOOD"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetIPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::INDUSTRY_WORKER,0);
				s.Format("%i %s",b2->GetIPProd()*number-b1->GetIPProd()*number,CLoc::GetString("INDUSTRY"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetEnergyProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::ENERGY_WORKER,0);
				s.Format("%i %s",b2->GetEnergyProd()*number-b1->GetEnergyProd()*number,CLoc::GetString("ENERGY"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetSPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::SECURITY_WORKER,0);
				s.Format("%i %s",b2->GetSPProd()*number-b1->GetSPProd()*number,CLoc::GetString("SECURITY"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetFPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::RESEARCH_WORKER,0);
				s.Format("%i %s",b2->GetFPProd()*number-b1->GetFPProd()*number,CLoc::GetString("RESEARCH"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetTitanProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::TITAN_WORKER,0);
				s.Format("%i %s",b2->GetTitanProd()*number-b1->GetTitanProd()*number,CLoc::GetString("TITAN"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetDeuteriumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::DEUTERIUM_WORKER,0);
				s.Format("%i %s",b2->GetDeuteriumProd()*number-b1->GetDeuteriumProd()*number,CLoc::GetString("DEUTERIUM"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetDuraniumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::DURANIUM_WORKER,0);
				s.Format("%i %s",b2->GetDuraniumProd()*number-b1->GetDuraniumProd()*number,CLoc::GetString("DURANIUM"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetCrystalProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::CRYSTAL_WORKER,0);
				s.Format("%i %s",b2->GetCrystalProd()*number-b1->GetCrystalProd()*number,CLoc::GetString("CRYSTAL"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetIridiumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(WORKER::IRIDIUM_WORKER,0);
				s.Format("%i %s",b2->GetIridiumProd()*number-b1->GetIridiumProd()*number,CLoc::GetString("IRIDIUM"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);

				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CLoc::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), r, &fontFormat, &fontBrush);
			}
		}
	}
}

namespace
{
	struct ResearchAndSecurityRecalcHelper
	{
		CEmpire* m_pEmpire;
		CSystemProd* m_pProd;

		ResearchAndSecurityRecalcHelper(CEmpire* pEmpire, CSystemProd* pProd) :
			m_pEmpire(pEmpire), m_pProd(pProd)
		{
			// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
			m_pEmpire->AddFP(-m_pProd->GetResearchProd());
			m_pEmpire->AddSP(-m_pProd->GetSecurityProd());
		};

		~ResearchAndSecurityRecalcHelper()
		{
			// FP´s und SP´s wieder draufrechnen
			m_pEmpire->AddFP(m_pProd->GetResearchProd());
			m_pEmpire->AddSP(m_pProd->GetSecurityProd());
		}

	};

	void ReflectPossibleResearchOrSecurityWorkerChange(CMajor* pMajor, CBotEDoc* pDoc, const CPoint& p,
		bool manager, bool energy = true)
	{
		CSystem& system = pDoc->GetSystem(p.x,p.y);
		const ResearchAndSecurityRecalcHelper help(pMajor->GetEmpire(), system.GetProduction());

		// Variablen berechnen
		system.CalculateVariables();
		if(manager)
			system.ExecuteManager(*pMajor, false, energy);
	}

}

void CSystemMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

	// Wenn wir uns in der Systemansicht befinden
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_BuildMenueMainButtons, temp, FALSE, FALSE, 5))
	{
		if(temp == 5)
		{
			CSystem& system = pDoc->CurrentSystem();
			CManagerSettingsDlg dlg(&system.Manager(), system);
			if(dlg.DoModal() == IDOK)
				ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, pDoc->GetCo(), true);
		}
		else
			m_bySubMenu = temp;
		if (m_bySubMenu == 0)
		{
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
			m_iClickedOn = 0;
			m_byStartList = 0;
			m_iWhichSubMenu = 0;
			Invalidate(FALSE);
			return;
		}
		else if (ShipyardListButton.PtInRect(point))
		{
			// Wenn man keine Schiffe zur Auswahl hat dann wird wieder auf das normale Gebäudebaumenü umgeschaltet
			if (pDoc->CurrentSystem().GetBuildableShips()->GetSize() == 0)
				return;
			m_iClickedOn = 0;
			m_byStartList = 0;
			m_iWhichSubMenu = 1;
			Invalidate(FALSE);
			return;
		}
		else if (TroopListButton.PtInRect(point))
		{
			// Wenn man keine Schiffe zur Auswahl hat dann wird wieder auf das normale Gebäudebaumenü umgeschaltet
			if (pDoc->CurrentSystem().GetBuildableTroops()->GetSize() == 0)
				return;
			m_iClickedOn = 0;
			m_byStartList = 0;
			m_iWhichSubMenu = 2;
			Invalidate(FALSE);
			return;
		}

		for (int i = m_byStartList; i < m_vBuildlist.GetSize(); i++)
		{
			if (i > m_byEndList)
				break;

			CRect rect(380, 150 + (i - m_byStartList) * 25, 670, 150 + (i - m_byStartList) * 25 + 25);

			if (rect.PtInRect(point))
			{
				m_iClickedOn = i;
				m_bClickedOnBuyButton = FALSE;
				m_bClickedOnDeleteButton = FALSE;
				Invalidate(FALSE);
				return;
			}
		}

		const CPoint& p = pDoc->GetCo();
		// Überprüfen ob wir auf den KaufenButton gedrückt haben und wir noch nix gekauft haben
		// dies geht nicht bei NeverReady wie z.B. Kriegsrecht Aufträgen
		int nFirstAssemblyListEntry = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetAssemblyListEntry(0);
		if (BuyButton.PtInRect(point) && pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetWasBuildingBought() == FALSE
			&& nFirstAssemblyListEntry != 0 && m_bClickedOnDeleteButton == FALSE)
		{
			// Bei Gebäuden nur wenn es nicht ein Auftrag mit NeverReady (z.B. Kriegsrecht) ist)
			if ((nFirstAssemblyListEntry < 0)
				||
				(nFirstAssemblyListEntry > 0 && nFirstAssemblyListEntry < 10000 && pDoc->GetBuildingInfo(nFirstAssemblyListEntry).GetNeverReady() == FALSE)
				||
				// Bei Schiffen nur, wenn eine Werft noch aktiv ist
				(nFirstAssemblyListEntry >= 10000 && nFirstAssemblyListEntry < 20000 && pDoc->GetSystem(p.x, p.y).GetProduction()->GetShipYard())
				||
				// Bei Truppen nur mit aktiver Kaseren
				(nFirstAssemblyListEntry >= 20000 && pDoc->GetSystem(p.x, p.y).GetProduction()->GetBarrack()))
			{
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->CalculateBuildCosts(pMajor->GetTrade()->GetRessourcePriceAtRoundStart());
				m_bClickedOnBuyButton = TRUE;
				Invalidate();
				return;
			}
		}

		// Überprüfen ob wir auf den Okaybutton gedrückt haben um das aktuelle Projekt zu kaufen
		if (OkayButton.PtInRect(point) && m_bClickedOnBuyButton == TRUE)
		{
			int costs = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->BuyBuilding(pMajor->GetEmpire()->GetCredits());
			if (costs != 0)
			{
				OkayButton.SetRect(0,0,0,0);
				CancelButton.SetRect(0,0,0,0);
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->SetWasBuildingBought(TRUE);
				pMajor->GetEmpire()->SetCredits(-costs);
				// Die Preise an der Börse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
				// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
				pMajor->GetTrade()->BuyRessource(RESOURCES::TITAN,pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededTitanInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),true);
				pMajor->GetTrade()->BuyRessource(RESOURCES::DEUTERIUM,pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededDeuteriumInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),true);
				pMajor->GetTrade()->BuyRessource(RESOURCES::DURANIUM,pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededDuraniumInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),true);
				pMajor->GetTrade()->BuyRessource(RESOURCES::CRYSTAL,pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededCrystalInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),true);
				pMajor->GetTrade()->BuyRessource(RESOURCES::IRIDIUM,pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededIridiumInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),true);

				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				m_bClickedOnBuyButton = FALSE;
				Invalidate();
				return;
			}
		}
		// Überprüfen ob wir auf den Cancelbutton gedrückt haben um den Kauf abzubrechen
		if (CancelButton.PtInRect(point))
		{
			OkayButton.SetRect(0,0,0,0);
			CancelButton.SetRect(0,0,0,0);
			m_bClickedOnBuyButton = FALSE;
			m_bClickedOnDeleteButton = FALSE;
			Invalidate();
			return;
		}

		// Überprüfen ob wir auf den Bau_abbrechen Button gedrückt haben
		if (DeleteButton.PtInRect(point) && pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(0) != 0 && m_bClickedOnBuyButton == FALSE)
		{
			m_bClickedOnDeleteButton = TRUE;
			Invalidate();
			return;
		}
		// Überprüfen ob wir auf den Okaybutton gedrückt haben um das aktuelle Projekt abzubrechen
		if (OkayButton.PtInRect(point) && m_bClickedOnDeleteButton == TRUE)
		{
			// bekommen bei Abbruch die Ressourcen bzw. Teile der Ressourcen wieder
			for (int j = RESOURCES::TITAN; j <= RESOURCES::DERITIUM; j++)
			{
				// bestanden Ressourcenrouten, so kann es sein, dass deren Startsysteme einen Anteil oder auch
				// alles zurückbekommen
				long getBackRes = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j);
				for (int k = 0; k < pMajor->GetEmpire()->GetSystemList()->GetSize(); k++)
					if (pMajor->GetEmpire()->GetSystemList()->GetAt(k).ko != p)
					{
						CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(k).ko;
						for (int l = 0; l < pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetSize(); l++)
							if (pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(l).GetCo() == p)
								if (pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(l).GetResource() == j)
									if (pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(l).GetPercent() > 0)
									{
										// sind wir soweit, dann geht ein prozentualer Anteil zurück in das
										// Startsystem der Ressourcenroute
										int back = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j)
											* pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(l).GetPercent() / 100;
										AssertBotE(back >= 0);
										pDoc->GetSystem(ko.x, ko.y).SetResourceStore(j, back);
										getBackRes -= back;
									}
					}
				pDoc->GetSystem(p.x, p.y).SetResourceStore(j, getBackRes);
			}
			// Wenn wir was gekauft hatten, dann bekommen wir die Kaufkosten zurück und die Preise an der Börse
			// regulieren sich wieder auf den Kurs, bevor wir gekauft haben
			if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetWasBuildingBought() == TRUE)
			{
				pMajor->GetEmpire()->SetCredits(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetBuildCosts());
				// Die Preise an der Börse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
				// Achtung, hier bNotAtMarket == true setzen bei Aufruf der Funktion BuyRessource!!!!
				for (int j = RESOURCES::TITAN; j <= RESOURCES::IRIDIUM; j++)
					pMajor->GetTrade()->SellRessource(j, pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j) * 2, p, true);
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->SetWasBuildingBought(FALSE);
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
			pDoc->GetSystem(p.x, p.y).GetAssemblyList()->ClearAssemblyList(p, pDoc->m_Systems);
			// Nach ClearAssemblyList müssen wir die Funktion CalculateVariables() aufrufen
			ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, p, true);

			int RunningNumber = abs(nFirstAssemblyListEntry);
			// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben.
			// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt
			// hinzufügen wollen
			if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
			{
				// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
				// weniger in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist.
				pDoc->m_GlobalBuildings.DeleteGlobalBuilding(pMajor->GetRaceID(), RunningNumber);
				// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
				// durchführen
				for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
					for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
						if (pDoc->GetSystem(x,y).OwnerID() == pMajor->GetRaceID())
							pDoc->GetSystem(x,y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
			}
			// sonst den Baulistencheck nur in dem aktuellen System durchführen
			else if (nFirstAssemblyListEntry < 0 || (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
				pDoc->GetSystem(p.x, p.y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);

			m_bClickedOnDeleteButton = FALSE;
			OkayButton.SetRect(0,0,0,0);
			CancelButton.SetRect(0,0,0,0);
			Invalidate();
			return;
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
			return;
		}
		// Überprüfen ob wir auf den Gebäudebeschreibungs Button gedrückt haben um die Beschreibung zum Gebäude anzuzeigen
		if (BuildingDescriptionButton.PtInRect(point))
		{
			m_bClickedOnBuildingInfoButton = FALSE;
			m_bClickedOnBuildingDescriptionButton = TRUE;
			CRect r = BuildingDescription;
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
			return;
		}
	}

	//////////////////////////////////////////////////////////
	// Ins Arbeiterzuweisungsmenü (normale Gebäude)
	if (m_bySubMenu == 1)
	{
		CPoint p = pDoc->GetCo();
		if (ChangeWorkersButton.PtInRect(point))
		{
			m_bySubMenu = 12;
			Invalidate();
			return;
		}

		int i = -1;
		if (ButtonReactOnLeftClick(point, &m_WorkerButtons, i, FALSE, TRUE))
		{
			// Wenn wir die Arbeiterzahl erhöhen wollen
			if (i >= 5)
			{
				// bei den Plusbutton müssen wir 5 abziehen, um auf die korrelierten Arbeiter zu kommen
				i -= 5;
				WORKER::Typ nWorker = (WORKER::Typ)(i);
				// Wenn wir noch freie Arbeiter haben
				if (pDoc->GetSystem(p.x,p.y).GetWorker(WORKER::FREE_WORKER) > 0 && pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(nWorker,0) > pDoc->GetSystem(p.x,p.y).GetWorker(nWorker))
				{
					pDoc->GetSystem(p.x, p.y).SetWorker(nWorker,CSystem::SET_WORKER_MODE_INCREMENT);	// FoodWorker inkrementieren
					ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, p, false);
					Invalidate();
					return;
				}
			}
			// Wenn wir die Arbeiterzahl verringern möchten
			else
			{
				WORKER::Typ nWorker = (WORKER::Typ)(i);
				// Wenn wir noch Arbeiter in dem bestimmten Gebäude haben
				if (pDoc->GetSystem(p.x,p.y).GetWorker(nWorker) > 0)
				{
					pDoc->GetSystem(p.x, p.y).SetWorker(nWorker,CSystem::SET_WORKER_MODE_DECREMENT);	// FoodWorker dekrementieren
					ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, p, false);
					Invalidate();
					return;
				}
			}
		}
		// Wenn wir die Arbeiterzahl erhöhen/verringern wollen und auf den Balken gedrückt haben
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 200; j++)
				if (Timber[i][j].PtInRect(point))
				{
					WORKER::Typ nWorker = (WORKER::Typ)i;
					// Wenn unsere Erhöhung größer der freien Arbeiter ist
					if (j >= (pDoc->GetSystem(p.x,p.y).GetWorker(WORKER::FREE_WORKER)+pDoc->GetSystem(p.x,p.y).GetWorker(nWorker)))
					{
						// Dann setzen wir die Erhöhung auf den max. mgl. Wert
						j = (pDoc->GetSystem(p.x,p.y).GetWorker(WORKER::FREE_WORKER)+pDoc->GetSystem(p.x,p.y).GetWorker(nWorker));
						j--;	// Müssen wir machen
					}
					pDoc->GetSystem(p.x, p.y).SetWorker(nWorker,CSystem::SET_WORKER_MODE_SET,j+1);
					// Wenn wir ziemlich weit ganz links geklickt haben, dann Arbeiter auf null setzen, werden hier nur um eins dekrementiert
					if (j == 0 && point.x < Timber[i][j].left+3)
						pDoc->GetSystem(p.x, p.y).SetWorker(nWorker,CSystem::SET_WORKER_MODE_DECREMENT);
					ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, p, false);
					Invalidate();
					return;
				}

	}
	// Ins Arbeiterzuweisungsmenü (Ressourcen Gebäude)
	else
	{
		if (m_bySubMenu == 12)
		{
			CPoint p = pDoc->GetCo();
			if (ChangeWorkersButton.PtInRect(point))
			{
				m_bySubMenu = 1;
				Invalidate();
			}
			int i = -1;
			if (ButtonReactOnLeftClick(point, &m_WorkerButtons, i, FALSE, TRUE))
			{
				// Wenn wir die Arbeiterzahl erhöhen wollen
				if (i >= 5)
				{
					WORKER::Typ nWorker = (WORKER::Typ)i;
					// bei den Plusbutton müssen wir 5 abziehen, um auf die korrelierten Arbeiter zu kommen
					i -= 5;
					// Wenn wir noch freie Arbeiter haben
					if (pDoc->GetSystem(p.x,p.y).GetWorker(WORKER::FREE_WORKER) > 0 && pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(nWorker,0) > pDoc->GetSystem(p.x,p.y).GetWorker(nWorker))
					{
						pDoc->GetSystem(p.x, p.y).SetWorker(nWorker,CSystem::SET_WORKER_MODE_INCREMENT);	// FoodWorker inkrementieren
						pDoc->GetSystem(p.x, p.y).CalculateVariables();
						Invalidate();
						return;
					}
				}
				// Wenn wir die Arbeiterzahl verringern möchten
				else
				{
					WORKER::Typ nWorker = (WORKER::Typ)(i+5);
					// Wenn wir noch Arbeiter in dem bestimmten Gebäude haben
					if (pDoc->GetSystem(p.x,p.y).GetWorker(nWorker) > 0)
					{
						pDoc->GetSystem(p.x, p.y).SetWorker(nWorker,CSystem::SET_WORKER_MODE_DECREMENT);	// FoodWorker dekrementieren
						pDoc->GetSystem(p.x, p.y).CalculateVariables();
						Invalidate();
						return;
					}
				}
			}
			// Wenn wir die Arbeiterzahl erhöhen/verringern wollen und auf den Balken gedrückt haben
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 200; j++)
					if (Timber[i][j].PtInRect(point))
					{
						WORKER::Typ nWorker = (WORKER::Typ)(i+5);
						// Wenn unsere Erhöhung größer der freien Arbeiter ist
						if (j >= (pDoc->GetSystem(p.x,p.y).GetWorker(WORKER::FREE_WORKER)+pDoc->GetSystem(p.x,p.y).GetWorker(nWorker)))
						{
							// Dann setzen wir die Erhöhung auf den max. mgl. Wert
							j = (pDoc->GetSystem(p.x,p.y).GetWorker(WORKER::FREE_WORKER)+pDoc->GetSystem(p.x,p.y).GetWorker(nWorker));
							j--;	// Müssen wir machen
						}
						pDoc->GetSystem(p.x, p.y).SetWorker(nWorker,CSystem::SET_WORKER_MODE_SET,j+1);
						// Wenn wir ziemlich weit ganz links geklickt haben, dann Arbeiter auf null setzen, werden hier nur um eins dekrementiert
						if (j == 0 && point.x < Timber[i][j].left+3)
							pDoc->GetSystem(p.x, p.y).SetWorker(nWorker,CSystem::SET_WORKER_MODE_DECREMENT);
						pDoc->GetSystem(p.x, p.y).CalculateVariables();
						Invalidate();
						return;
					}
		}
	}

	// Ins Energiezuweisungsmenü
	if (m_bySubMenu == 2)
	{
		const CPoint& p = pDoc->GetCo();
		for (int i = m_iELPage * NOBIEL; i < m_EnergyList.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iELPage * NOBIEL + NOBIEL)
				if (m_EnergyList.GetAt(i).rect.PtInRect(point))
				{
					const CBuildingInfo *buildingInfo = &pDoc->BuildingInfo.GetAt(pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetRunningNumber() - 1);
					if (m_EnergyList.GetAt(i).status == 0)
					{
						if (pDoc->GetSystem(p.x,p.y).CanTakeOnline(*buildingInfo))
							pDoc->GetSystem(p.x, p.y).SetIsBuildingOnline(m_EnergyList.GetAt(i).index, TRUE);
					}
					else
					{
						pDoc->GetSystem(p.x, p.y).SetIsBuildingOnline(m_EnergyList.GetAt(i).index, FALSE);
					}

					ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, p, true, false);
					// Wenn es eine Werft war, die wir an bzw. aus geschaltet haben, dann nochmal schauen ob ich auch
					// noch alle Schiffe bauen kann. Denn wenn die aus ist, dann kann ich keine mehr bauen
					if (buildingInfo->GetShipYard())
						pDoc->GetSystem(p.x, p.y).CalculateBuildableShips();
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
		CPoint p = pDoc->GetCo();
		for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iBOPage * NOBIOL + NOBIOL)
				if (m_BuildingOverview.GetAt(i).rect.PtInRect(point))
				{
					USHORT ru = m_BuildingOverview.GetAt(i).runningNumber;
					pDoc->GetSystem(p.x, p.y).SetBuildingDestroy(ru,TRUE);
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
			CGalaxyMenuView::GetDrawnTradeRoute()->GenerateTradeRoute(pDoc->GetCo());
			resources::pMainFrame->SelectMainView(VIEWS::GALAXY_VIEW, pMajor->GetRaceID());
			resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
		}
		// Button zum Anlegen einer Ressourcenroute
		else if (CRect(360,600,480,630).PtInRect(point))
		{
			CGalaxyMenuView::IsDrawResourceRoute(TRUE);
			CGalaxyMenuView::GetDrawnResourceRoute()->GenerateResourceRoute(pDoc->GetCo(), m_byResourceRouteRes);
			resources::pMainFrame->SelectMainView(VIEWS::GALAXY_VIEW, pMajor->GetRaceID());
			resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
		}
		// Button zum Ändern der Ressource einer Ressourcenroute
		else if (CRect(360,640,480,670).PtInRect(point))
		{
			m_byResourceRouteRes++;
			if (m_byResourceRouteRes == RESOURCES::IRIDIUM + 1)
				m_byResourceRouteRes = RESOURCES::TITAN;
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
			CPoint p = pDoc->GetCo();
			// einzelne Buttons um womöglich eine Ressourcenroute zu kündigen
			for (int i = 0; i < pDoc->GetSystem(p.x, p.y).GetResourceRoutes()->GetSize(); i++)
			{
				int j = i + pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetSize();
				if (CRect(360,260+j*30,480,290+j*30).PtInRect(point))
				{
					// Eine Ressourcenroute kann nur gekündigt werden, wenn sie keine prozentualen Anteile am Bauauftrag
					// besitzt, sprich, wenn sie keine Ressourcen zum Bauauftrag beigetragen hat.
					CResourceRoute* pResRoute = &pDoc->GetSystem(p.x, p.y).GetResourceRoutes()->GetAt(i);
					if (pResRoute->GetPercent() == 0)
					{
						pDoc->GetSystem(p.x, p.y).GetResourceRoutes()->RemoveAt(i--);
						Invalidate(FALSE);
					}
					return;
				}
			}
			// einzelnen Buttons, um Waren zwischen System und globalen Lager hin und her zu schieben
			// Wenn das System blockiert wird kann man nicht mehr Waren ins oder aus dem stellaren Lager
			// nehmen
			if (pDoc->GetSystem(p.x, p.y).GetBlockade() > NULL)
				return;

			int i = -1;
			if (ButtonReactOnLeftClick(point, &m_SystemTradeButtons, i, FALSE, TRUE))
			{
				// kleine Pfeilbuttons um Waren aus dem globalen Lager ins System zu verschieben
				if (i >= 5)
				{
					i -= 5;

					if (pDoc->GetSystem(p.x, p.y).GetResourceStore(i) > 0 || pMajor->GetEmpire()->GetGlobalStorage()->GetSubResource(i,p) > 0)
					{
						UINT tempQuantity = m_iGlobalStoreageQuantity;
						if (pDoc->GetSystem(p.x, p.y).GetResourceStore(i) < m_iGlobalStoreageQuantity && pMajor->GetEmpire()->GetGlobalStorage()->GetSubResource(i,p) == 0)
							m_iGlobalStoreageQuantity = pDoc->GetSystem(p.x, p.y).GetResourceStore(i);
						UINT getBack = pMajor->GetEmpire()->GetGlobalStorage()->AddRessource(m_iGlobalStoreageQuantity,i,p);
						pDoc->GetSystem(p.x, p.y).SetResourceStore(i, (getBack - m_iGlobalStoreageQuantity));
						m_iGlobalStoreageQuantity = tempQuantity;
						Invalidate(FALSE);
					}
				}
				// kleine Pfeilbuttons um Waren aus dem System ins globale Lager zu verschieben
				else
				{
					pDoc->GetSystem(p.x, p.y).SetResourceStore(i, pMajor->GetEmpire()->GetGlobalStorage()->SubRessource(m_iGlobalStoreageQuantity,i,p));
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
			if (m_byStartList + NOEIBL / 2 <= m_iClickedOn && m_iClickedOn < m_vBuildlist.GetUpperBound())
				m_byStartList++;
			if (m_iClickedOn < m_vBuildlist.GetUpperBound())
				m_iClickedOn++;
			Invalidate(FALSE);
		}
		else if (zDelta > 0)
		{
			if (m_iClickedOn > 0)
				m_iClickedOn--;
			if (m_byStartList > 0)
				m_byStartList--;
			Invalidate(FALSE);
		}
	}

	return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSystemMenuView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// Doppelklick weiterleiten
	CSystemMenuView::OnLButtonDown(nFlags, point);

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	// Wenn wir uns in der Systemansicht befinden
	CPoint p = pDoc->GetCo();

	// Baulisteneintrag hinzufügen
	if (m_bySubMenu == 0)
	{
		for (int i = m_byStartList; i < m_vBuildlist.GetSize(); i++)
		{
			if (i > m_byEndList)
				break;

			CRect rect(380, 150 + (i - m_byStartList) * 25, 670, 150 + (i - m_byStartList) * 25 + 25);

			if (rect.PtInRect(point))
			{
				int nID = m_vBuildlist[i];
				int RunningNumber = abs(nID);
				// vor einem Doppelklick müssen die benötigten Ressourcen sicherheitshalber nochmal neu berechnet.
				// Denn bei ganz flinken Fingern kann es passieren, dass dies in der Draw Methode nicht gemacht werden.
				// also ein Gebäude oder Gebäudeupdate
				if (nID < 10000)
					pDoc->GetSystem(p.x, p.y).GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->GetSystem(p.x, p.y).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
				// also ein Schiff
				else if (nID < 20000 && pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize() > 0)
					pDoc->GetSystem(p.x, p.y).GetAssemblyList()->CalculateNeededRessources(0, &pDoc->m_ShipInfoArray.GetAt(nID - 10000),0, pDoc->GetSystem(p.x, p.y).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
				// also eine Truppe
				else if (pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize() > 0)
					pDoc->GetSystem(p.x, p.y).GetAssemblyList()->CalculateNeededRessources(0,0,&pDoc->m_TroopInfo.GetAt(nID - 20000), pDoc->GetSystem(p.x, p.y).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());

				if (pDoc->GetSystem(p.x, p.y).GetAssemblyList()->MakeEntry(nID, p, pDoc->m_Systems))
				{
					// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben.
					// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt
					// hinzufügen wollen
					if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
					{
						// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
						// mehr in der Bauliste befindet.
						pDoc->m_GlobalBuildings.AddGlobalBuilding(pMajor->GetRaceID(), RunningNumber);
						// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
						// durchführen
						for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
							for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
								if (pDoc->GetSystem(x,y).OwnerID() == pMajor->GetRaceID())
									pDoc->GetSystem(x,y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
					}
					// sonst den Baulistencheck nur in dem aktuellen System durchführen
					else if (nID < 0 || (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
						pDoc->GetSystem(p.x, p.y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);

					// Wenn wir den Baueintrag setzen konnten, also hier in der if-Bedingung sind, dann CalculateVariables() aufrufen
					ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, p, true);
					m_iClickedOn = i;

					// Die Struktur Buildlist löschen
					m_vBuildlist.RemoveAll();
					Invalidate(FALSE);
					return;
				}
				// Baulisteneintrag konnte aufgrund von Ressourcenmangel nicht gesetzt werden
				else
					CSoundManager::GetInstance()->PlaySound(SNDMGR_SOUND_ERROR);
				return;
			}
		}
	}

	// Baulisteneintrag wieder entfernen
	for (int i = 0; i < ALE; i++)
	{
		int nAssemblyListEntry = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetAssemblyListEntry(i);
		if (nAssemblyListEntry == 0)
			break;

		CRect rect(760, 410 + i * 25, 1000, 410 + i * 25 + 25);
		if (rect.PtInRect(point))
		{
			m_bClickedOnBuyButton = FALSE;
			m_bClickedOnDeleteButton = FALSE;
			int RunningNumber = abs(nAssemblyListEntry);

			// Nur beim 0-ten Eintrag in der Bauliste, also der, der oben steht
			if (i == 0)
			{
				// bekommen bei Abbruch die Ressourcen bzw. Teile der Ressourcen wieder
				for (int j = RESOURCES::TITAN; j <= RESOURCES::DERITIUM; j++)
				{
					// bestanden Ressourcenrouten, so kann es sein, dass deren Startsysteme einen Anteil oder auch
					// alles zurückbekommen
					long getBackRes = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j);
					for (int k = 0; k < pMajor->GetEmpire()->GetSystemList()->GetSize(); k++)
					{
						if (pMajor->GetEmpire()->GetSystemList()->GetAt(k).ko != p)
						{
							CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(k).ko;
							for (int l = 0; l < pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetSize(); l++)
							{
								if (pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(l).GetCo() == p)
								{
									if (pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(l).GetResource() == j)
									{
										if (pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(l).GetPercent() > 0)
										{
											// sind wir soweit, dann geht ein prozentualer Anteil zurück in das
											// Startsystem der Ressourcenroute
											int back = pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j)
												* pDoc->GetSystem(ko.x, ko.y).GetResourceRoutes()->GetAt(l).GetPercent() / 100;
											AssertBotE(back >= 0);
											pDoc->GetSystem(ko.x, ko.y).SetResourceStore(j, back);
											getBackRes -= back;
										}
									}
								}
							}
						}
					}

					pDoc->GetSystem(p.x, p.y).SetResourceStore(j, getBackRes);
				}
				// Wenn wir was gekauft hatten, dann bekommen wir die Kaufkosten zurück
				if (pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetWasBuildingBought() == TRUE)
				{
					pMajor->GetEmpire()->SetCredits(pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetBuildCosts());
					// Die Preise an der Börse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
					// Achtung, hier bNotAtMarket == true setzen bei Aufruf der Funktion BuyRessource!!!!
					for (int j = RESOURCES::TITAN; j <= RESOURCES::IRIDIUM; j++)
						pMajor->GetTrade()->SellRessource(j, pDoc->GetSystem(p.x, p.y).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j), p, true);
					pDoc->GetSystem(p.x, p.y).GetAssemblyList()->SetWasBuildingBought(FALSE);
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->ClearAssemblyList(p, pDoc->m_Systems);
				// Nach ClearAssemblyList müssen wir die Funktion CalculateVariables() aufrufen
				ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, p, true);
			}
			// Die restlichen Einträge seperat, weil wir die Bauliste anders löschen müssen und auch keine RES zurückbekommen müssen
			else
			{
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->AdjustAssemblyList(i);
			}

			// Wurde ein Gebäude oder ein Update aus der Bauliste genommen? (kein Schiff und keine Truppen)
			if (RunningNumber < 10000)
			{
				// Baulistencheck in jedem System machen, wenn das Gebäude oder Update die Eigenschaft "MaxInEmpire" besitzt
				if (pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
				{
					// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
					// weniger in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist.
					pDoc->m_GlobalBuildings.DeleteGlobalBuilding(pMajor->GetRaceID(), RunningNumber);
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchführen
					for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
						for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
							if (pDoc->GetSystem(x,y).OwnerID() == pMajor->GetRaceID())
								pDoc->GetSystem(x,y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
				// Baulistencheck im aktuellen System machen, wenn ein Update oder ein Gebäude mit MaxInSystem entfernt wurde
				else if (nAssemblyListEntry < 0 || pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0)
				{
					pDoc->GetSystem(p.x, p.y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
			}

			Invalidate(FALSE);
			return;
		}
	}

	CMainBaseView::OnLButtonDblClk(nFlags, point);
}

void CSystemMenuView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	CalcLogicalPoint(point);
	const CPoint& p = pDoc->GetCo();
	CSystem& system = pDoc->GetSystem(p.x, p.y);

	if (m_bySubMenu == 2)
	{
		for (int i = m_iELPage * NOBIEL; i < m_EnergyList.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iELPage * NOBIEL + NOBIEL)
			{
				const ENERGYSTRUCT& struct_building = m_EnergyList.GetAt(i);
				if (struct_building.rect.PtInRect(point))
				{
					const CBuilding& building = system.GetAllBuildings()->GetAt(struct_building.index);
					const bool ignored = system.Manager().IsBuildingIgnored(building.GetRunningNumber());
					CMenu menu;
					menu.LoadMenu(IDR_MENU_ENERGY_BUILDINGS);
					CMenu* onoff = menu.GetSubMenu(0);
					AssertBotE(onoff);
					onoff->CheckMenuItem(ID_MENU_ENERGY_BUILDINGS_AUTO_ONOFF,
						ignored ? MF_UNCHECKED : MF_CHECKED);
					m_iRightClicked = i;
					CRect r;
					GetWindowRect(r);
					onoff->TrackPopupMenu(TPM_RIGHTALIGN, r.left + point.x, r.top + point.y, this);
					break;
				}
			}
	}
	// Das hier alles nur machen, wenn wir in der System-Ansicht sind
	// Im Gebäudeübersichtsmenü
	else if (m_bySubMenu == 3)
	{
		// Wenn ich Gebäude nicht mehr abreißen will, mit rechts auf die Schaltfläche drücken

		for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iBOPage * NOBIOL + NOBIOL)
				if (m_BuildingOverview.GetAt(i).rect.PtInRect(point))
				{
					USHORT ru = m_BuildingOverview.GetAt(i).runningNumber;
					system.SetBuildingDestroy(ru,FALSE);
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
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);
	ButtonReactOnMouseOver(point, &m_BuildMenueMainButtons);
	if (m_bySubMenu == 1 || m_bySubMenu == 12)
		ButtonReactOnMouseOver(point, &m_WorkerButtons);
	else if (m_bySubMenu == 4)
		ButtonReactOnMouseOver(point, &m_SystemTradeButtons);

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CSystemMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;

	// Wenn wir in irgendeinem System sind, können wir mit dem links rechts Pfeil zu einem anderen System wechseln
	CPoint p = pDoc->GetCo();
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
			m_iClickedOn = 0;
			m_byStartList = 0;
			pDoc->SetKO(pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.x, pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.y);
			Invalidate(FALSE);
		}
	}
	// Wenn wir im Baumenü sind können wir mit den Pfeil auf und Pfeil ab Tasten durch die Bauliste scrollen
	if (m_bySubMenu == 0)
	{
		if (nChar == VK_DOWN)
		{
			if (m_byStartList + NOEIBL / 2 <= m_iClickedOn && m_iClickedOn < m_vBuildlist.GetUpperBound())
				m_byStartList++;
			if (m_iClickedOn < m_vBuildlist.GetUpperBound())
				m_iClickedOn++;
			Invalidate(FALSE);
		}
		else if (nChar == VK_UP)
		{
			if (m_iClickedOn > 0)
				m_iClickedOn--;
			if (m_byStartList > 0)
				m_byStartList--;
			Invalidate(FALSE);
		}
		// Mit der Entertaste können wir den Auftrag in die Bauliste übernehmen
		else if (nChar == VK_RETURN && m_iClickedOn < m_vBuildlist.GetSize())
		{
			int nID = m_vBuildlist[m_iClickedOn];
			int RunningNumber = abs(nID);

			// vor einem Doppelklick müssen die benötigten Ressourcen sicherheitshalber nochmal neu berechnet.
			// Denn bei ganz flinken Fingern kann es passieren, dass dies in der Draw Methode nicht gemacht werden.
			// also ein Gebäude oder Gebäudeupdate
			if (nID < 10000)
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->GetSystem(p.x, p.y).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			// also ein Schiff
			else if (nID < 20000 && pDoc->GetSystem(p.x, p.y).GetBuildableShips()->GetSize() > 0)
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->CalculateNeededRessources(0, &pDoc->m_ShipInfoArray.GetAt(nID - 10000),0, pDoc->GetSystem(p.x, p.y).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			// also eine Truppe
			else if (pDoc->GetSystem(p.x, p.y).GetBuildableTroops()->GetSize() > 0)
				pDoc->GetSystem(p.x, p.y).GetAssemblyList()->CalculateNeededRessources(0,0,&pDoc->m_TroopInfo.GetAt(nID - 20000), pDoc->GetSystem(p.x, p.y).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());

			if (pDoc->GetSystem(p.x, p.y).GetAssemblyList()->MakeEntry(nID, p, pDoc->m_Systems))
			{
				// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben.
				// Den Check nur machen, wenn wir ein Update oder ein Gebäude welches eine Maxanzahl voraussetzt
				// hinzufügen wollen
				if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
				{
					// Wir müssen die GlobalBuilding Variable ändern, weil sich mittlerweile ja solch ein Gebäude
					// mehr in der Bauliste befindet.
					pDoc->m_GlobalBuildings.AddGlobalBuilding(pMajor->GetRaceID(), RunningNumber);
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchführen
					for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
						for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
							if (pDoc->GetSystem(x,y).OwnerID() == pMajor->GetRaceID())
								pDoc->GetSystem(x,y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
				// sonst den Baulistencheck nur in dem aktuellen System durchführen
				else if (nID < 0 || (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
					pDoc->GetSystem(p.x, p.y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);

				// Wenn wir den Baueintrag setzen konnten, also hier in der if-Bedingung sind, dann CalculateVariables() aufrufen
				ReflectPossibleResearchOrSecurityWorkerChange(pMajor, pDoc, p, true);

				// Die Struktur BuildList löschen, alle Werte auf 0
				m_vBuildlist.RemoveAll();

				Invalidate(FALSE);
			}
		}
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSystemMenuView::CreateButtons()
{
	AssertBotE((CBotEDoc*)GetDocument());

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);

	CString sPrefix = pMajor->GetPrefix();
	// alle Buttons in der View anlegen und Grafiken laden
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CLoc::GetString("BTN_BUILDMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CLoc::GetString("BTN_WORKERSMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CLoc::GetString("BTN_ENERGYMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CLoc::GetString("BTN_BUILDING_OVERVIEWMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CLoc::GetString("BTN_TRADEMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(860,690), CSize(160,40), CLoc::GetString("BTN_SYSTEMMANAGER"), fileN, fileI, fileA));

	// Zuweisungsbuttons im Arbeitermenü
	fileN = "Other\\" + sPrefix + "buttonminus.bop";
	fileA = "Other\\" + sPrefix + "buttonminusa.bop";
	for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
		m_WorkerButtons.Add(new CMyButton(CPoint(170,115+i*95) , CSize(40,40), "", fileN, fileN, fileA));

	// Zuweisungsbuttons im Arbeitermenü
	fileN = "Other\\" + sPrefix + "buttonplus.bop";
	fileA = "Other\\" + sPrefix + "buttonplusa.bop";
	for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
		m_WorkerButtons.Add(new CMyButton(CPoint(630,115+i*95) , CSize(40,40), "", fileN, fileN, fileA));

	// Zuweisungsbuttons im Systemhandelsmenü
	fileN = "Other\\" + sPrefix + "buttonminus.bop";
	fileA = "Other\\" + sPrefix + "buttonminusa.bop";
	for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
		m_SystemTradeButtons.Add(new CMyButton(CPoint(608,235+i*60) , CSize(30,30), "", fileN, fileN, fileA));
	fileN = "Other\\" + sPrefix + "buttonplus.bop";
	fileA = "Other\\" + sPrefix + "buttonplusa.bop";
	for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
		m_SystemTradeButtons.Add(new CMyButton(CPoint(975,235+i*60) , CSize(30,30), "", fileN, fileN, fileA));
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CSystemMenuView::CreateTooltip(void)
{
	// Tooltips erstmal nur im Baumenü und in der Energie- und Gebäudeübersicht anzeigen
	if (m_bySubMenu != 0 && m_bySubMenu != 2 && m_bySubMenu != 3)
		return "";

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	int nID = -1;
	if (m_bySubMenu == 0)
	{
		// Prüfen ob die Maus über das angezeigte Bild gehalten wurde
		if (m_iClickedOn >= 0 && m_iClickedOn < m_vBuildlist.GetSize())
			if (CRect(70, 60, 70 + 200, 60 + 150).PtInRect(pt))
				nID = abs(m_vBuildlist[m_iClickedOn]);
	}
	else if (m_bySubMenu == 2)
	{
		// Prüfen über welches Gebäude die Maus gehalten wurde
		for (int i = m_iELPage * NOBIEL; i < m_EnergyList.GetSize(); i++)
		{
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iELPage * NOBIEL + NOBIEL)
				if (m_EnergyList.GetAt(i).rect.PtInRect(pt))
				{
					nID = pDoc->CurrentSystem().GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetRunningNumber();
					break;
				}
		}
	}
	else if (m_bySubMenu == 3)
	{
		// Prüfen über welches Gebäude die Maus gehalten wurde
		for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
		{
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iBOPage * NOBIOL + NOBIOL)
				if (m_BuildingOverview.GetAt(i).rect.PtInRect(pt))
				{
					nID = m_BuildingOverview.GetAt(i).runningNumber;
					break;
				}
		}
	}

	if (nID != -1)
	{
		// Gebäude oder Update
		if (nID < 10000)
		{
			CString sName = pDoc->GetBuildingInfo(nID).GetBuildingName();
			sName = CHTMLStringBuilder::GetHTMLColor(sName);
			sName = CHTMLStringBuilder::GetHTMLHeader(sName, _T("h3"));
			sName = CHTMLStringBuilder::GetHTMLCenter(sName);
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sProd = pDoc->GetBuildingInfo(nID).GetProductionAsString();
			sProd = CHTMLStringBuilder::GetHTMLColor(sProd);
			sProd = CHTMLStringBuilder::GetHTMLHeader(sProd, _T("h5"));
			sProd += CHTMLStringBuilder::GetHTMLStringNewLine();
			sProd += CHTMLStringBuilder::GetHTMLStringHorzLine();
			sProd += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sDesc = pDoc->GetBuildingInfo(nID).GetBuildingDescription();
			sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc, _T("silver"));
			sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));

			return sName + sProd + sDesc;
		}
		// Schiff
		else if (nID < 20000)
		{
			// Schiff erzeugen und Spezialforschungen einbeziehen
			CShips ship(pDoc->m_ShipInfoArray[nID - 10000]);
			ship.AddSpecialResearchBoni(pDoc->GetRaceCtrl()->GetRaceSafe(m_pPlayersRace->GetRaceID()));
			return ship.GetTooltip();
		}
		// Truppe
		else
		{
			return pDoc->m_TroopInfo.GetAt(nID - 20000).GetTooltip();
		}
	}

	return "";
}

void CSystemMenuView::OnMenuEnergyBuildingsAutoOnoff()
{
	AssertBotE(0 <= m_iRightClicked && m_iRightClicked < m_EnergyList.GetSize());
	const ENERGYSTRUCT& struct_building = m_EnergyList.GetAt(m_iRightClicked);
	CBotEDoc* pDoc = resources::pDoc;
	CSystem& system = pDoc->CurrentSystem();
	const CBuilding& building = system.GetAllBuildings()->GetAt(struct_building.index);
	system.Manager().ToggleBuildingIgnored(building.GetRunningNumber());
}
