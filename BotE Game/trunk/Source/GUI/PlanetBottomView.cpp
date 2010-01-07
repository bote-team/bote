// PlanetBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "GalaxyMenuView.h"
#include "PlanetBottomView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"


// CPlanetBottomView

IMPLEMENT_DYNCREATE(CPlanetBottomView, CBottomBaseView)

CPlanetBottomView::CPlanetBottomView()
{
	m_arroundThePlanets = NULL;
}

CPlanetBottomView::~CPlanetBottomView()
{
	if (m_arroundThePlanets)
	{
		delete m_arroundThePlanets;
		m_arroundThePlanets = NULL;
	}
}

BEGIN_MESSAGE_MAP(CPlanetBottomView, CBottomBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CPlanetBottomView drawing

void CPlanetBottomView::OnDraw(CDC* dc)
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
	Bitmap* graphic = NULL;

	if (m_arroundThePlanets)
	{
		delete[] m_arroundThePlanets;
		m_arroundThePlanets = NULL;
	}
		
	// Galaxie im Hintergrund zeichnen
	CString sPrefix = pMajor->GetPrefix();
	
	Bitmap* background = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "galaxyV3.bop");
	if (background)
		g.DrawImage(background, 0, 0, 1075, 249);
	
	CPoint KO = pDoc->GetKO();
	short resizeItX = 0;
	float maxHabitants = 0.0f;
	int nPosX = m_TotalSize.cx - 175;
	int nVertCenter = m_TotalSize.cy / 2;

	if (pDoc->m_Sector[KO.x][KO.y].GetFullKnown(pMajor->GetRaceID()) == TRUE)
	{
		m_arroundThePlanets = new CRect[pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets()];	
		for (int i = 0; i < pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets(); i++)
		{
			CPlanet* planet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
			maxHabitants += planet->GetMaxHabitant();

			switch (planet->GetSize())
			{
			case SMALL:
				nPosX -= 75;
				m_arroundThePlanets[i].SetRect(nPosX, nVertCenter - 23, nPosX + 45, nVertCenter + 22);
				break;
			case NORMAL:
				nPosX -= 90;
				m_arroundThePlanets[i].SetRect(nPosX, nVertCenter - 30, nPosX + 60, nVertCenter + 30);
				break;
			case BIG:
				nPosX -= 105;
				m_arroundThePlanets[i].SetRect(nPosX, nVertCenter - 40, nPosX + 80, nVertCenter + 40);
				break;
			case GIANT:
				if (planet->GetPlanetName() == "Saturn")
				{	
					nPosX -= 155;
					m_arroundThePlanets[i].SetRect(nPosX, nVertCenter - 35, nPosX + 145, nVertCenter + 49);					
				}
				else
				{
					nPosX -= 145;
					m_arroundThePlanets[i].SetRect(nPosX, nVertCenter - 63, nPosX + 125, nVertCenter + 62);					
				}
				break;
			}
			
			planet->DrawPlanet(g, m_arroundThePlanets[i], pDoc->GetGraphicPool());			
		}				
	}
	if (pDoc->GetSector(KO.x,KO.y).GetSunSystem() && pDoc->GetSector(KO.x,KO.y).GetScanned(pMajor->GetRaceID()))
	{
		graphic = NULL;
		switch(pDoc->GetSector(KO.x,KO.y).GetSunColor())
		{
		case 0:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_blue.bop"); break;
		case 1:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_green.bop"); break;
		case 2:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_orange.bop"); break;
		case 3:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_red.bop"); break;
		case 4:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_violet.bop"); break;			
		case 5:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_white.bop"); break;
		case 6:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_yellow.bop"); break;					
		}				
		if (graphic)
			g.DrawImage(graphic, 950, -10, 250, 261);
	}
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontBrush.SetColor(Color(170,170,170));
				
	// Informationen zu dem System angeben
	CString s;
	float currentHabitants = pDoc->m_Sector[KO.x][KO.y].GetCurrentHabitants();
	s.Format("%s %c%i",CResourceManager::GetString("SECTOR"),(char)(KO.y+97),KO.x+1);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,25), &fontFormat, &fontBrush);
	if (pDoc->m_Sector[KO.x][KO.y].GetScanned(pMajor->GetRaceID()) == FALSE)
	{
		s = CResourceManager::GetString("UNKNOWN");
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,47), &fontFormat, &fontBrush);
	}
	else if (pDoc->m_Sector[KO.x][KO.y].GetSunSystem() == TRUE && pDoc->m_Sector[KO.x][KO.y].GetKnown(pMajor->GetRaceID()) == TRUE)
	{
		// vorhandene Rohstoffe auf allen Planeten zeichnen
		s = CResourceManager::GetString("EXISTING_RES") + ":";
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(735,228), &fontFormat, &fontBrush);
		RectF boundingBox;
		g.MeasureString(s.AllocSysString(), s.GetLength(), &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(735, 228), &fontFormat, &boundingBox); 
		// Symbole der vorhanden Ressourcen im System ermitteln
		BOOLEAN res[DILITHIUM + 1] = {0};
		pDoc->GetSector(KO).GetAvailableResources(res, false);
		int nExist = 0;
		for (int i = TITAN; i <= DILITHIUM; i++)
		{
			if (res[i])
			{
				graphic = NULL;
				switch(i)
				{
					case TITAN:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop");		break;
					case DEUTERIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop");	break;
					case DURANIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop");	break;
					case CRYSTAL:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop");		break;
					case IRIDIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop");		break;
					case DILITHIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\Deritium.bop");		break;
				}
				if (graphic)
					g.DrawImage(graphic, (int)boundingBox.GetRight() + 5 + nExist * 20, 228, 20, 16);
				nExist++;
			}
		}
		
		s.Format("%s: %s",CResourceManager::GetString("SYSTEM"), pDoc->m_Sector[KO.x][KO.y].GetName());
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,47), &fontFormat, &fontBrush);
		if (pDoc->m_Sector[KO.x][KO.y].GetFullKnown(pMajor->GetRaceID()))
		{
			s.Format("%s: %.3lf Mrd.",CResourceManager::GetString("MAX_HABITANTS"), maxHabitants);
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,180), &fontFormat, &fontBrush);
			s.Format("%s: %.3lf Mrd.",CResourceManager::GetString("CURRENT_HABITANTS"), currentHabitants);
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,202), &fontFormat, &fontBrush);
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\popmaxSmall.bop");
			if (graphic)
				g.DrawImage(graphic, 23, 180, 20, 16);
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\populationSmall.bop");			
			if (graphic)
				g.DrawImage(graphic, 23, 202, 20, 16);
		}
	}
	// Scannerstärke zeichnen
	if (pDoc->m_Sector[KO.x][KO.y].GetScanned(pMajor->GetRaceID()) == TRUE)
	{
		// Rassenspezifische Schrift auswählen
		CFontLoader::CreateGDIFont(pMajor, 1, fontName, fontSize);
		s.Format("%s: %i%%",CResourceManager::GetString("SCANPOWER"), pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()));
		if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 75)
			fontBrush.SetColor(Color(0,245,0));
		else if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 50)
			fontBrush.SetColor(Color(50,180,50));
		else if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 25)
			fontBrush.SetColor(Color(230,230,20));
		else if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) > 0)
			fontBrush.SetColor(Color(230,100,0));
		else
			fontBrush.SetColor(Color(245,0,0));		
		
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(711,0), &fontFormat, &fontBrush);
	}
	// Namen des Besitzers des Sector unten rechts zeichnen
	if (pDoc->GetSector(KO.x,KO.y).GetScanned(pMajor->GetRaceID()) && pMajor->IsRaceContacted(pDoc->m_Sector[KO.x][KO.y].GetOwnerOfSector())
		|| pDoc->m_Sector[KO.x][KO.y].GetOwnerOfSector() == pMajor->GetRaceID())
	{
		CRace* pOwner = pDoc->GetRaceCtrl()->GetRace(pDoc->m_Sector[KO.x][KO.y].GetOwnerOfSector());
		if (pOwner)
		{
			s = pOwner->GetRaceName();
			if (pOwner->GetType() == MAJOR)
			{
				Color color;
				color.SetFromCOLORREF(((CMajor*)pOwner)->GetDesign()->m_clrSector);
				fontBrush.SetColor(color);
			}
			else
				fontBrush.SetColor(Color(255,255,255));			

			CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(735,190), &fontFormat, &fontBrush);
		}		

		// Wir selbst und alle uns bekannten Rassen sehen, wenn das System blockiert wird.
		// Dafür wird ein OverlayBanner über die Ansicht gelegt.
		if (pDoc->m_System[KO.x][KO.y].GetBlockade() > NULL)
		{
			CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
			CSize viewSize(m_TotalSize.cx - 160, m_TotalSize.cy - 120);
			s.Format("%d", pDoc->m_System[KO.x][KO.y].GetBlockade());
			COverlayBanner* banner = new COverlayBanner(CPoint(80,60), viewSize, CResourceManager::GetString("SYSTEM_IS_BLOCKED", FALSE, s), RGB(200,0,0));
			banner->SetBorderWidth(1);
			Gdiplus::Font font(fontName.AllocSysString(), fontSize);
			banner->Draw(&g, &font);
			delete banner;
		}
	}

	g.ReleaseHDC(pDC->GetSafeHdc());
}


// CPlanetBottomView diagnostics

#ifdef _DEBUG
void CPlanetBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlanetBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlanetBottomView message handlers

BOOL CPlanetBottomView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

void CPlanetBottomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);
	
	CPoint KO = pDoc->GetKO();
	if (m_arroundThePlanets != NULL)
		for (int i = 0; i < pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets(); i++)
			if (m_arroundThePlanets && m_arroundThePlanets[i].PtInRect(point))
			{
				CSmallInfoView::SetPlanetInfo(true);
				CSmallInfoView::SetPlanet(pDoc->m_Sector[KO.x][KO.y].GetPlanet(i));
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
				break;
			}
	// Wenn wir die Planeten sehen und haben ein Schiff gewählt welches einen Planeten kolonisieren bzw. terraformen
	// soll müssen wir hier den Planeten anklicken können
	if (CGalaxyMenuView::IsMoveShip() == TRUE)
	{
		pDoc->m_ShipArray.ElementAt(pDoc->GetNumberOfTheShipInArray()).SetTerraformingPlanet(-1);
		for (int i = 0; i < pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets(); i++)
			if (m_arroundThePlanets && m_arroundThePlanets[i].PtInRect(point))	// Haben wir auf einen Planeten geklickt
			{
				// Lange Abfrage hie notwendig, weil bei Kolonisierung brauchen wir nen geterraformten Planeten und
				// beim Terraforming nen bewohnbaren noch nicht geterraformten Planeten
				if (/*(pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetPlanet(i)->GetTerraformed() == TRUE
					&& pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetPlanet(i)->GetCurrentHabitant() == NULL
					&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() == COLONIZE)
					||*/ (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetPlanet(i)->GetTerraformed() == FALSE
					&& pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetPlanet(i)->GetHabitable() == TRUE
					&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() == TERRAFORM))
				{
					CGalaxyMenuView::SetMoveShip(FALSE);
					CShipBottomView::SetShowStation(false);
					CSmallInfoView::SetShipInfo(true);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
					pDoc->m_ShipArray.ElementAt(pDoc->GetNumberOfTheShipInArray()).SetTerraformingPlanet(i);
					pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetPlanet(i)->SetIsTerraforming(TRUE);
					Invalidate();
				}
				else
					pDoc->m_ShipArray.ElementAt(pDoc->GetNumberOfTheShipInArray()).SetCurrentOrder(AVOID);

				break;
			}
	}

	CBottomBaseView::OnLButtonDown(nFlags, point);
}

void CPlanetBottomView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!m_arroundThePlanets)
		return;

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	CPoint KO = pDoc->GetKO();
	for (int i = 0; i < pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets(); i++)
		if (m_arroundThePlanets[i].PtInRect(point))
		{
			CPlanet* pPlanet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
			if (pPlanet != CSmallInfoView::GetPlanet())
			{
				CSmallInfoView::SetPlanet(pPlanet);
				CSmallInfoView::SetPlanetStats(true);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));				
			}
			break;
		}

	CBottomBaseView::OnMouseMove(nFlags, point);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CPlanetBottomView::CreateTooltip(void)
{
	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CPoint KO = pDoc->GetKO();

	// wurde die Maus über die Sonne gehalten
	if (CRect(m_TotalSize.cx - 125, 0, m_TotalSize.cx, m_TotalSize.cy).PtInRect(pt))
	{
		CString sSunColor;
		switch (pDoc->GetSector(KO).GetSunColor())
		{
		case 0: sSunColor = CResourceManager::GetString("BLUE_STAR");	break;
		case 1: sSunColor = CResourceManager::GetString("GREEN_STAR");	break;
		case 2: sSunColor = CResourceManager::GetString("ORANGE_STAR");	break;
		case 3: sSunColor = CResourceManager::GetString("RED_STAR");	break;
		case 4: sSunColor = CResourceManager::GetString("VIOLET_STAR");	break;
		case 5: sSunColor = CResourceManager::GetString("WHITE_STAR");	break;
		case 6: sSunColor = CResourceManager::GetString("YELLOW_STAR");	break;
		}
		sSunColor = CHTMLStringBuilder::GetHTMLColor(sSunColor);
		sSunColor = CHTMLStringBuilder::GetHTMLHeader(sSunColor);
		sSunColor = CHTMLStringBuilder::GetHTMLCenter(sSunColor);
		sSunColor += CHTMLStringBuilder::GetHTMLStringNewLine();
		sSunColor += CHTMLStringBuilder::GetHTMLStringNewLine();
		
		CString sSunDesc;
		switch (pDoc->GetSector(KO).GetSunColor())
		{
		case 0: sSunDesc = CResourceManager::GetString("BLUE_STAR_DESC");	break;
		case 1: sSunDesc = CResourceManager::GetString("GREEN_STAR_DESC");	break;
		case 2: sSunDesc = CResourceManager::GetString("ORANGE_STAR_DESC");	break;
		case 3: sSunDesc = CResourceManager::GetString("RED_STAR_DESC");	break;
		case 4: sSunDesc = CResourceManager::GetString("VIOLET_STAR_DESC");	break;
		case 5: sSunDesc = CResourceManager::GetString("WHITE_STAR_DESC");	break;
		case 6: sSunDesc = CResourceManager::GetString("YELLOW_STAR_DESC");	break;
		}
		sSunDesc = CHTMLStringBuilder::GetHTMLColor(sSunDesc);
		sSunDesc = CHTMLStringBuilder::GetHTMLHeader(sSunDesc, _T("h5"));		
		sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
		
		// wurden keine Planeten angezeigt, das System ist also nicht bekannt, dann hier aufhören
		if (!m_arroundThePlanets)
			return sSunColor + sSunDesc;
		
		sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
		sSunDesc += CHTMLStringBuilder::GetHTMLStringHorzLine();
		sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
		
		CString sSystemBoni = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("BONI_IN_SYSTEM"), _T("silver"));
		sSystemBoni = CHTMLStringBuilder::GetHTMLHeader(sSystemBoni, _T("h4"));
		sSystemBoni = CHTMLStringBuilder::GetHTMLCenter(sSystemBoni);
		sSystemBoni += CHTMLStringBuilder::GetHTMLStringNewLine();
		
		for (int j = 0; j < 8; j++)
		{
			short nBonus = 0;
			for (int i = 0; i < pDoc->GetSector(KO).GetNumberOfPlanets(); i++)
			{
				CPlanet* pPlanet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
				ASSERT(pPlanet);
				if (pPlanet->GetBoni()[j])
					if (j != DILITHIUM)
						nBonus += (pPlanet->GetSize() + 1) * 25;
			}
			if (nBonus)
			{
				sSystemBoni += CHTMLStringBuilder::GetHTMLStringNewLine();
				CString sBoni;
				switch(j)
				{
					case TITAN:		sBoni = CResourceManager::GetString("TITAN_BONUS");		break;
					case DEUTERIUM: sBoni = CResourceManager::GetString("DEUTERIUM_BONUS"); break;
					case DURANIUM:	sBoni = CResourceManager::GetString("DURANIUM_BONUS");	break;
					case CRYSTAL:	sBoni = CResourceManager::GetString("CRYSTAL_BONUS");	break;
					case IRIDIUM:	sBoni = CResourceManager::GetString("IRIDIUM_BONUS");	break;
					case 6:			sBoni = CResourceManager::GetString("FOOD_BONUS");		break;
					case 7:			sBoni = CResourceManager::GetString("ENERGY_BONUS");	break;
				}
				
				CString sBonus;
				sBonus.Format("%d%% %s",nBonus, sBoni);
				sBonus = CHTMLStringBuilder::GetHTMLColor(sBonus);
				sBonus = CHTMLStringBuilder::GetHTMLHeader(sBonus, _T("h5"));
				sSystemBoni += sBonus;				
			}
		}
		return sSunColor + sSunDesc + sSystemBoni;
	}

	// wurden keine Planeten angezeigt, das System ist also nicht bekannt, dann hier aufhören
	if (!m_arroundThePlanets)
		return "";

	// wurde die Maus über einen der Planeten gehalten oder über die Planetenboni?
	for (int i = 0; i < pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets(); i++)
	{
		CPlanet* pPlanet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
		ASSERT(pPlanet);

		// wurde auf den Planeten gezeigt
		if (m_arroundThePlanets[i].PtInRect(pt))
		{
			CString sTip;
			CPlanet* pPlanet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
			
			CString sName = CHTMLStringBuilder::GetHTMLColor(pPlanet->GetPlanetName());
			sName = CHTMLStringBuilder::GetHTMLHeader(sName);
			sName = CHTMLStringBuilder::GetHTMLCenter(sName);
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();
			sTip += sName;
			
			CString s;
			s.Format("CLASS_%c_TYPE", pPlanet->GetClass());
			s = CHTMLStringBuilder::GetHTMLColor(_T("(") + CResourceManager::GetString(s) + _T(")"), _T("silver"));
			s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
			s = CHTMLStringBuilder::GetHTMLCenter(s);
			s += CHTMLStringBuilder::GetHTMLStringNewLine();
			s += CHTMLStringBuilder::GetHTMLStringNewLine();
			sTip += s;
			
			s.Format("CLASS_%c_INFO", pPlanet->GetClass());
			s = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString(s));
			s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h5"));			
			sTip += s;				
			return sTip;			
		}
		else
		{
			// wurde knapp über das Planetenrechteck gezeigt
			CRect boniRect = m_arroundThePlanets[i];
			boniRect.OffsetRect(0, -23);
			if (boniRect.PtInRect(pt))
			{
				// prüfen ob beim Planeten auf eine der möglichen Boni gezeigt wurde
				const BOOLEAN* boni = pPlanet->GetBoni();
				// erstmal schauen, wieviele Boni überhaupt vorhanden sind
				BYTE n = 0;
				for (int j = 0; j < 8; j++)
					n += boni[j];
				if (n == 0)
					continue;
				
				int x = boniRect.CenterPoint().x - 5;
				x -= (n*9-4);
				int y = boniRect.top;

				for (int j = 0; j < 8; j++)
					if (boni[j])
					{
						CRect rect(x, y, x + 20, y + 16);
						if (rect.PtInRect(pt))
						{
							CString sBoni;
							switch(j)
							{
								case TITAN:		sBoni = CResourceManager::GetString("TITAN_BONUS"); break;
								case DEUTERIUM: sBoni = CResourceManager::GetString("DEUTERIUM_BONUS"); break;
								case DURANIUM:	sBoni = CResourceManager::GetString("DURANIUM_BONUS"); break;
								case CRYSTAL:	sBoni = CResourceManager::GetString("CRYSTAL_BONUS"); break;
								case IRIDIUM:	sBoni = CResourceManager::GetString("IRIDIUM_BONUS"); break;
								case DILITHIUM:
									sBoni = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("DILITHIUM") + " " + CResourceManager::GetString("EXISTING"));
									sBoni = CHTMLStringBuilder::GetHTMLHeader(sBoni, _T("h5"));
									return CHTMLStringBuilder::GetHTMLCenter(sBoni);									
								case 6:			sBoni = CResourceManager::GetString("FOOD_BONUS"); break;
								case 7:			sBoni = CResourceManager::GetString("ENERGY_BONUS"); break;
							}
							CString sTip;
							sTip.Format("%d%% %s",(pPlanet->GetSize() + 1) * 25, sBoni);
							sTip = CHTMLStringBuilder::GetHTMLColor(sTip);
							sTip = CHTMLStringBuilder::GetHTMLHeader(sTip, _T("h5"));
							return CHTMLStringBuilder::GetHTMLCenter(sTip);
						}					
						x += 18;					
					}
			}
		}	
	}
	
	return "";
}
