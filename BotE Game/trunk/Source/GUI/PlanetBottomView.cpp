// PlanetBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "MainFrm.h"
#include "GalaxyMenuView.h"
#include "PlanetBottomView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "Galaxy\Anomaly.h"
#include "Graphic\memdc.h"
#include "Ships/Ships.h"
#include "General/Loc.h"
#include "GraphicPool.h"


// CPlanetBottomView

IMPLEMENT_DYNCREATE(CPlanetBottomView, CBottomBaseView)

CPlanetBottomView::CPlanetBottomView()
{
}

CPlanetBottomView::~CPlanetBottomView()
{
}

BEGIN_MESSAGE_MAP(CPlanetBottomView, CBottomBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CPlanetBottomView drawing

void CPlanetBottomView::OnDraw(CDC* dc)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
	if (!pMajor)
		return;
	// TODO: add draw code here

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

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(static_cast<Gdiplus::ARGB>(Color::White));
	Bitmap* graphic = NULL;

	// alte Planetenrechtecke löschen
	m_vPlanetRects.clear();

	// Galaxie im Hintergrund zeichnen
	CString sPrefix = pMajor->GetPrefix();

	Bitmap* background = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "galaxyV3.bop");
	if (background)
		g.DrawImage(background, 0, 0, 1075, 249);

	CPoint KO = pDoc->GetCo();
	float maxHabitants = 0.0f;
	int nPosX = m_TotalSize.cx - 175;
	int nVertCenter = m_TotalSize.cy / 2;

	if (pDoc->GetSystem(KO.x, KO.y).GetFullKnown(pMajor->GetRaceID()) == TRUE)
	{
		for (int i = 0; i < pDoc->GetSystem(KO.x, KO.y).GetNumberOfPlanets(); i++)
		{
			const CPlanet* planet = pDoc->GetSystem(KO.x, KO.y).GetPlanet(i);
			maxHabitants += planet->GetMaxHabitant();

			CRect rect;
			switch (planet->GetSize())
			{
			case PLANT_SIZE::SMALL:
				nPosX -= 75;
				rect.SetRect(nPosX, nVertCenter - 23, nPosX + 45, nVertCenter + 22);
				break;
			case PLANT_SIZE::NORMAL:
				nPosX -= 90;
				rect.SetRect(nPosX, nVertCenter - 30, nPosX + 60, nVertCenter + 30);
				break;
			case PLANT_SIZE::BIG:
				nPosX -= 105;
				rect.SetRect(nPosX, nVertCenter - 40, nPosX + 80, nVertCenter + 40);
				break;
			case PLANT_SIZE::GIANT:
				if (planet->GetPlanetName() == "Saturn")
				{
					nPosX -= 155;
					rect.SetRect(nPosX, nVertCenter - 35, nPosX + 145, nVertCenter + 49);
				}
				else
				{
					nPosX -= 145;
					rect.SetRect(nPosX, nVertCenter - 63, nPosX + 125, nVertCenter + 62);
				}
				break;
			}

			m_vPlanetRects.push_back(rect);
			planet->DrawPlanet(g, rect, pDoc->GetGraphicPool());
		}
	}
	if (pDoc->GetSystem(KO.x,KO.y).GetScanned(pMajor->GetRaceID()))
	{
		if (pDoc->GetSystem(KO.x,KO.y).GetSunSystem())
		{
			graphic = NULL;
			switch(pDoc->GetSystem(KO.x,KO.y).GetSunColor())
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
		else if (pDoc->GetSystem(KO.x, KO.y).GetAnomaly())
		{
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("MapStars\\" + pDoc->GetSystem(KO.x, KO.y).GetAnomaly()->GetImageFileName());

			if (graphic)
			{
				if (pDoc->GetSystem(KO.x, KO.y).GetAnomaly()->GetImageFlipHorz())
				{
					Bitmap* copy = graphic->Clone(0, 0, graphic->GetWidth(), graphic->GetHeight(), PixelFormat32bppPARGB);
					copy->RotateFlip(Gdiplus::RotateNoneFlipX);
					g.DrawImage(copy, 500, -11, 284, 284);
					delete copy;
					copy = NULL;
				}
				else
					g.DrawImage(graphic, 500, -11, 284, 284);
			}
		}
	}

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontBrush.SetColor(Color(170,170,170));

	// Informationen zu dem System angeben
	CString s = pDoc->GetSystem(KO.x, KO.y).CoordsName(false);
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(40,25), &fontFormat, &fontBrush);
	if (pDoc->GetSystem(KO.x, KO.y).GetAnomaly() && pDoc->GetSystem(KO.x, KO.y).GetScanned(pMajor->GetRaceID()))
	{
		AssertBotE(!pDoc->GetSystem(KO.x, KO.y).GetSunSystem());
		s.Format("%s: %s", CLoc::GetString("ANOMALY"), pDoc->GetSystem(KO.x, KO.y).GetAnomaly()->GetMapName(KO));
		g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(40,47), &fontFormat, &fontBrush);
	}
	else if(const boost::shared_ptr<const CShips>& station = pDoc->GetSystem(KO.x, KO.y).GetStation())
		if(pMajor->IsRaceContacted(station->OwnerID())
			&& pDoc->GetSystem(KO.x, KO.y).ShouldDrawOutpost(*pMajor, station->OwnerID())
			&& !pDoc->GetSystem(KO.x, KO.y).GetSunSystem())
		{
			s.Format("%s: %s", CLoc::GetString("STATION"), station->GetName());
			g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(40,47), &fontFormat, &fontBrush);
		}

	if (pDoc->GetSystem(KO.x, KO.y).GetScanned(pMajor->GetRaceID()) == FALSE)
	{
		s = CLoc::GetString("UNKNOWN");
		g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(40,47), &fontFormat, &fontBrush);
	}
	else if (pDoc->GetSystem(KO.x, KO.y).GetSunSystem() == TRUE && pDoc->GetSystem(KO.x, KO.y).GetKnown(pMajor->GetRaceID()) == TRUE)
	{
		// vorhandene Rohstoffe auf allen Planeten zeichnen
		s = CLoc::GetString("EXISTING_RES") + ":";
		g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(735,228), &fontFormat, &fontBrush);
		RectF boundingBox;
		g.MeasureString(CComBSTR(s), s.GetLength(), &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(735, 228), &fontFormat, &boundingBox);
		// Symbole der vorhanden Ressourcen im System ermitteln
		BOOLEAN res[RESOURCES::DERITIUM + 1] = {0},rescol[RESOURCES::DERITIUM + 1] = {0};
		pDoc->GetSystem(KO.x, KO.y).GetAvailableResources(res, false);	//alle Ressourcen
		pDoc->GetSystem(KO.x, KO.y).GetAvailableResources(rescol, true);//erschlossene Ressourcen
		int nExist = 0;
		for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
		{
			if (res[i])
			{
				graphic = NULL;
				switch(i)
				{
					case RESOURCES::TITAN:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop");		break;
					case RESOURCES::DEUTERIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop");	break;
					case RESOURCES::DURANIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop");	break;
					case RESOURCES::CRYSTAL:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop");		break;
					case RESOURCES::IRIDIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop");		break;
					case RESOURCES::DERITIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\Deritium.bop");		break;
				}
				if (graphic)
					g.DrawImage(graphic, (int)boundingBox.GetRight() + 5 + nExist * 20, 228, 20, 16);
				if(!rescol[i])
				{
					fontBrush.SetColor(Color(100,0,0,0));//graut nicht erschlossene Ressourcen aus
					g.FillRectangle(&fontBrush,(int)boundingBox.GetRight() + 5 + nExist * 20, 228,20,16);
					fontBrush.SetColor(Color(170,170,170));
				}
				nExist++;
			}
		}

		s.Format("%s: %s",CLoc::GetString("SYSTEM"), pDoc->GetSystem(KO.x, KO.y).GetName());
		g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(40,47), &fontFormat, &fontBrush);
		const float currentHabitants = pDoc->GetSystem(KO.x, KO.y).GetCurrentHabitants();
		if (pDoc->GetSystem(KO.x, KO.y).GetFullKnown(pMajor->GetRaceID()))
		{
			s.Format("%s: %.3lf %s",CLoc::GetString("MAX_HABITANTS"), maxHabitants, CLoc::GetString("MRD"));
			g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(40,180), &fontFormat, &fontBrush);
			s.Format("%s: %.3lf %s",CLoc::GetString("CURRENT_HABITANTS"), currentHabitants, CLoc::GetString("MRD"));
			g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(40,202), &fontFormat, &fontBrush);
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\popmaxSmall.bop");
			if (graphic)
				g.DrawImage(graphic, 23, 180, 20, 16);
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\populationSmall.bop");
			if (graphic)
				g.DrawImage(graphic, 23, 202, 20, 16);
		}
	}
	// Symbole zu Truppen zeichnen
	if (pDoc->GetSystem(KO.x, KO.y).Majorized() &&
		(pDoc->GetSystem(KO.x, KO.y).GetScanPower(pMajor->GetRaceID()) > 50 || pDoc->GetSystem(KO.x, KO.y).OwnerID() == pMajor->GetRaceID()))
	{
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\troopSmall.bop");
		int nTroopNumber = pDoc->GetSystem(KO.x, KO.y).GetTroops()->GetSize();
		if (graphic && nTroopNumber)
		{
			// soviel wie Truppen stationiert sind, so viele Symbole werden gezeichnet
			s.Format("%d x", nTroopNumber);
			int nPosX = 0;
			if (nTroopNumber >= 10)
				nPosX = 10;
			if (nTroopNumber >= 100)
				nPosX = 20;
			g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(40,69), &fontFormat, &fontBrush);
			// Bild zeichnen
			g.DrawImage(graphic, 62 + nPosX, 64, 24, 24);
		}
	}
	// Scannerstärke zeichnen
	if (pDoc->GetSystem(KO.x, KO.y).GetScanned(pMajor->GetRaceID()) == TRUE)
	{
		// Rassenspezifische Schrift auswählen
		CFontLoader::CreateGDIFont(pMajor, 1, fontName, fontSize);
		s.Format("%s: %i%%",CLoc::GetString("SCANPOWER"), pDoc->GetSystem(KO.x, KO.y).GetScanPower(pMajor->GetRaceID()));
		if (pDoc->GetSystem(KO.x, KO.y).GetScanPower(pMajor->GetRaceID()) >= 75)
			fontBrush.SetColor(Color(0,245,0));
		else if (pDoc->GetSystem(KO.x, KO.y).GetScanPower(pMajor->GetRaceID()) >= 50)
			fontBrush.SetColor(Color(50,180,50));
		else if (pDoc->GetSystem(KO.x, KO.y).GetScanPower(pMajor->GetRaceID()) >= 25)
			fontBrush.SetColor(Color(230,230,20));
		else if (pDoc->GetSystem(KO.x, KO.y).GetScanPower(pMajor->GetRaceID()) > 0)
			fontBrush.SetColor(Color(230,100,0));
		else
			fontBrush.SetColor(Color(245,0,0));

		g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(711,0), &fontFormat, &fontBrush);
	}
	// Namen des Besitzers des Sector unten rechts zeichnen
	if (pDoc->GetSystem(KO.x, KO.y).GetScanned(pMajor->GetRaceID()) && pMajor->IsRaceContacted(pDoc->GetSystem(KO.x, KO.y).OwnerID())
		|| pDoc->GetSystem(KO.x, KO.y).OwnerID() == pMajor->GetRaceID())
	{
		const RacePtr& pOwner = pDoc->GetSystem(KO.x, KO.y).Owner();
		if (pOwner)
		{
			s = pOwner->GetName();
			if (pOwner->IsMajor())
			{
				Color color;
				color.SetFromCOLORREF((dynamic_cast<CMajor*>(pOwner.get()))->GetDesign()->m_clrSector);
				fontBrush.SetColor(color);
			}
			else
				fontBrush.SetColor(Color(255,255,255));

			CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
			g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), PointF(735,190), &fontFormat, &fontBrush);
		}

		// Wir selbst und alle uns bekannten Rassen sehen, wenn das System blockiert wird.
		// Dafür wird ein OverlayBanner über die Ansicht gelegt.
		if (pDoc->GetSystem(KO.x, KO.y).GetBlockade() > 0)
		{
			CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
			CSize viewSize(m_TotalSize.cx - 160, m_TotalSize.cy - 120);
			s.Format("%d", pDoc->GetSystem(KO.x, KO.y).GetBlockade());
			COverlayBanner* banner = new COverlayBanner(CPoint(80,60), viewSize, CLoc::GetString("SYSTEM_IS_BLOCKED", FALSE, s), RGB(200,0,0));
			banner->SetBorderWidth(1);
			Gdiplus::Font font(CComBSTR(fontName), fontSize);
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

BOOL CPlanetBottomView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

void CPlanetBottomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	if (m_vPlanetRects.empty())
		return;

	CalcLogicalPoint(point);

	CPoint KO = pDoc->GetCo();
	for (UINT i = 0; i < m_vPlanetRects.size(); i++)
		if (m_vPlanetRects[i].PtInRect(point))
		{
			CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_PLANET_INFO);
			CSmallInfoView::SetPlanet(pDoc->GetSystem(KO.x, KO.y).GetPlanet(i));
			resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			break;
		}
	// Wenn wir die Planeten sehen und haben ein Schiff gewählt welches einen Planeten kolonisieren bzw. terraformen
	// soll müssen wir hier den Planeten anklicken können
	if (CGalaxyMenuView::IsMoveShip() == TRUE)
	{
		// Haben wir auf einen Planeten geklickt
		for (UINT i = 0; i < m_vPlanetRects.size(); i++)
			if (m_vPlanetRects[i].PtInRect(point))
			{
				// Lange Abfrage hie notwendig, weil bei Kolonisierung brauchen wir nen geterraformten Planeten und
				// beim Terraforming nen bewohnbaren noch nicht geterraformten Planeten
				if (pDoc->CurrentSystem().GetPlanet(i)->GetTerraformed() == FALSE
					&& pDoc->CurrentSystem().GetPlanet(i)->GetHabitable() == TRUE)
				{
					CGalaxyMenuView::SetMoveShip(FALSE);
					CShipBottomView::SetShowStation(false);
					CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
					AssertBotE(pDoc->CurrentShip()->second->GetCo() == pDoc->GetCo());
					pDoc->CurrentShip()->second->SetTerraform(i);

					Invalidate();
				}

				break;
			}
	}

	CBottomBaseView::OnLButtonDown(nFlags, point);
}

void CPlanetBottomView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_vPlanetRects.empty())
		return;

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CalcLogicalPoint(point);

	CPoint KO = pDoc->GetCo();
	for (UINT i = 0; i < m_vPlanetRects.size(); i++)
		if (m_vPlanetRects[i].PtInRect(point))
		{
			const CPlanet* pPlanet = pDoc->GetSystem(KO.x, KO.y).GetPlanet(i);
			if (pPlanet != CSmallInfoView::GetPlanet())
			{
				CSmallInfoView::SetPlanet(pPlanet);
				CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_PLANET_STATS);
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			}
			break;
		}

	CBottomBaseView::OnMouseMove(nFlags, point);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CPlanetBottomView::CreateTooltip(void)
{
	CBotEDoc* pDoc = resources::pDoc;
	CPoint KO = pDoc->GetCo();

	if (!pDoc->m_bDataReceived)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	// wurde die Maus über den Namen einer Rasse gehalten
	if (!pDoc->GetSystem(KO.x, KO.y).Free())
	{
		if (CRect(735,190,885,220).PtInRect(pt))
		{
			CMajor* pMajor = m_pPlayersRace;
			AssertBotE(pMajor);
			if (!pMajor)
				return "";

			if (pDoc->GetSystem(KO.x, KO.y).GetScanned(pMajor->GetRaceID()) && pMajor->IsRaceContacted(pDoc->GetSystem(KO.x, KO.y).OwnerID()) || pDoc->GetSystem(KO.x, KO.y).OwnerID() == pMajor->GetRaceID())
			{
				const RacePtr& pOwner = pDoc->GetSystem(KO.x, KO.y).Owner();
				if (!pOwner)
					return "";

				return pOwner->GetTooltip();
			}
		}
	}

	if (pDoc->GetSystem(KO.x, KO.y).GetAnomaly())
	{
		if (CRect(500, 0, 784, m_TotalSize.cy).PtInRect(pt))
		{
			CString sName = pDoc->GetSystem(KO.x, KO.y).GetAnomaly()->GetMapName(KO);
			sName = CHTMLStringBuilder::GetHTMLColor(sName);
			sName = CHTMLStringBuilder::GetHTMLHeader(sName);
			sName = CHTMLStringBuilder::GetHTMLCenter(sName);
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sDesc = pDoc->GetSystem(KO.x, KO.y).GetAnomaly()->GetPhysicalDescription();
			sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc);
			sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));
			sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			sDesc += CHTMLStringBuilder::GetHTMLStringHorzLine();
			sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sGame = pDoc->GetSystem(KO.x, KO.y).GetAnomaly()->GetGameplayDescription();
			sGame = CHTMLStringBuilder::GetHTMLColor(sGame, _T("silver"));
			sGame = CHTMLStringBuilder::GetHTMLHeader(sGame, _T("h5"));

			return sName + sDesc + sGame;
		}
	}

	// wurde die Maus über die Sonne gehalten
	else if (pDoc->GetSystem(KO.x, KO.y).GetSunSystem())
	{
		if (CRect(m_TotalSize.cx - 125, 0, m_TotalSize.cx, m_TotalSize.cy).PtInRect(pt))
		{
			CString sSunColor;
			switch (pDoc->GetSystem(KO.x, KO.y).GetSunColor())
			{
			case 0: sSunColor = CLoc::GetString("BLUE_STAR");	break;
			case 1: sSunColor = CLoc::GetString("GREEN_STAR");	break;
			case 2: sSunColor = CLoc::GetString("ORANGE_STAR");	break;
			case 3: sSunColor = CLoc::GetString("RED_STAR");	break;
			case 4: sSunColor = CLoc::GetString("VIOLET_STAR");	break;
			case 5: sSunColor = CLoc::GetString("WHITE_STAR");	break;
			case 6: sSunColor = CLoc::GetString("YELLOW_STAR");	break;
			}
			sSunColor = CHTMLStringBuilder::GetHTMLColor(sSunColor);
			sSunColor = CHTMLStringBuilder::GetHTMLHeader(sSunColor);
			sSunColor = CHTMLStringBuilder::GetHTMLCenter(sSunColor);
			sSunColor += CHTMLStringBuilder::GetHTMLStringNewLine();
			sSunColor += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sSunDesc;
			switch (pDoc->GetSystem(KO.x, KO.y).GetSunColor())
			{
			case 0: sSunDesc = CLoc::GetString("BLUE_STAR_DESC");	break;
			case 1: sSunDesc = CLoc::GetString("GREEN_STAR_DESC");	break;
			case 2: sSunDesc = CLoc::GetString("ORANGE_STAR_DESC");	break;
			case 3: sSunDesc = CLoc::GetString("RED_STAR_DESC");	break;
			case 4: sSunDesc = CLoc::GetString("VIOLET_STAR_DESC");	break;
			case 5: sSunDesc = CLoc::GetString("WHITE_STAR_DESC");	break;
			case 6: sSunDesc = CLoc::GetString("YELLOW_STAR_DESC");	break;
			}
			sSunDesc = CHTMLStringBuilder::GetHTMLColor(sSunDesc);
			sSunDesc = CHTMLStringBuilder::GetHTMLHeader(sSunDesc, _T("h5"));
			sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();

			// wurden keine Planeten angezeigt, das System ist also nicht bekannt, dann hier aufhören
			if (m_vPlanetRects.empty())
				return sSunColor + sSunDesc;

			sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			sSunDesc += CHTMLStringBuilder::GetHTMLStringHorzLine();
			sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sSystemBoni = CHTMLStringBuilder::GetHTMLColor(CLoc::GetString("BONI_IN_SYSTEM"), _T("silver"));
			sSystemBoni = CHTMLStringBuilder::GetHTMLHeader(sSystemBoni, _T("h4"));
			sSystemBoni = CHTMLStringBuilder::GetHTMLCenter(sSystemBoni);
			sSystemBoni += CHTMLStringBuilder::GetHTMLStringNewLine();

			for (int j = 0; j < 8; j++)
			{
				short nBonus = 0;
				for (int i = 0; i < pDoc->GetSystem(KO.x, KO.y).GetNumberOfPlanets(); i++)
				{
					const CPlanet* pPlanet = pDoc->GetSystem(KO.x, KO.y).GetPlanet(i);
					AssertBotE(pPlanet);
					if (pPlanet->GetBoni()[j])
						if (j != RESOURCES::DERITIUM)
							nBonus += (pPlanet->GetSize() + 1) * 25;
				}
				if (nBonus)
				{
					sSystemBoni += CHTMLStringBuilder::GetHTMLStringNewLine();
					CString sBoni;
					switch(j)
					{
						case RESOURCES::TITAN:		sBoni = CLoc::GetString("TITAN_BONUS");		break;
						case RESOURCES::DEUTERIUM: sBoni = CLoc::GetString("DEUTERIUM_BONUS"); break;
						case RESOURCES::DURANIUM:	sBoni = CLoc::GetString("DURANIUM_BONUS");	break;
						case RESOURCES::CRYSTAL:	sBoni = CLoc::GetString("CRYSTAL_BONUS");	break;
						case RESOURCES::IRIDIUM:	sBoni = CLoc::GetString("IRIDIUM_BONUS");	break;
						case 6:			sBoni = CLoc::GetString("FOOD_BONUS");		break;
						case 7:			sBoni = CLoc::GetString("ENERGY_BONUS");	break;
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
		if (m_vPlanetRects.empty())
			return "";

		// wurde die Maus über einen der Planeten gehalten oder über die Planetenboni?
		for (UINT i = 0; i < m_vPlanetRects.size(); i++)
		{
			const CPlanet* pPlanet = pDoc->GetSystem(KO.x, KO.y).GetPlanet(i);
			AssertBotE(pPlanet);

			// wurde auf den Planeten gezeigt
			if (m_vPlanetRects[i].PtInRect(pt))
			{
				CString sTip;

				CString sName = CHTMLStringBuilder::GetHTMLColor(pPlanet->GetPlanetName());
				sName = CHTMLStringBuilder::GetHTMLHeader(sName);
				sName = CHTMLStringBuilder::GetHTMLCenter(sName);
				sName += CHTMLStringBuilder::GetHTMLStringNewLine();
				sTip += sName;

				CString s;
				s.Format("CLASS_%c_TYPE", pPlanet->GetClass());
				s = CHTMLStringBuilder::GetHTMLColor(_T("(") + CLoc::GetString(s) + _T(")"), _T("silver"));
				s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
				s = CHTMLStringBuilder::GetHTMLCenter(s);
				s += CHTMLStringBuilder::GetHTMLStringNewLine();
				s += CHTMLStringBuilder::GetHTMLStringNewLine();
				sTip += s;

				s.Format("CLASS_%c_INFO", pPlanet->GetClass());
				s = CHTMLStringBuilder::GetHTMLColor(CLoc::GetString(s));
				s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h5"));
				sTip += s;
				return sTip;
			}
			else
			{
				// wurde knapp über das Planetenrechteck gezeigt
				CRect boniRect = m_vPlanetRects[i];
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
									case RESOURCES::TITAN:		sBoni = CLoc::GetString("TITAN_BONUS"); break;
									case RESOURCES::DEUTERIUM: sBoni = CLoc::GetString("DEUTERIUM_BONUS"); break;
									case RESOURCES::DURANIUM:	sBoni = CLoc::GetString("DURANIUM_BONUS"); break;
									case RESOURCES::CRYSTAL:	sBoni = CLoc::GetString("CRYSTAL_BONUS"); break;
									case RESOURCES::IRIDIUM:	sBoni = CLoc::GetString("IRIDIUM_BONUS"); break;
									case RESOURCES::DERITIUM:
										sBoni = CHTMLStringBuilder::GetHTMLColor(CLoc::GetString("DERITIUM") + " " + CLoc::GetString("EXISTING"));
										sBoni = CHTMLStringBuilder::GetHTMLHeader(sBoni, _T("h5"));
										return CHTMLStringBuilder::GetHTMLCenter(sBoni);
									case 6:			sBoni = CLoc::GetString("FOOD_BONUS"); break;
									case 7:			sBoni = CLoc::GetString("ENERGY_BONUS"); break;
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
	}

	return "";
}
