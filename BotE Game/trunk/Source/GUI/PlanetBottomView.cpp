// PlanetBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "GalaxyMenuView.h"
#include "PlanetBottomView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"
#include "Races\RaceController.h"


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

	CMajor* pMajor = pDoc->GetPlayersRace();
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
	
	Bitmap* background = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "galaxyV3.png");
	if (background)
		g.DrawImage(background, 0, 0, 1075, 249);
	
	CPoint KO = pDoc->GetKO();
	short resizeItX = 0, resizeItY = 0;
	float maxHabitants = 0.0f;
	if (pDoc->m_Sector[KO.x][KO.y].GetFullKnown(pMajor->GetRaceID()) == TRUE)
	{
		short SizeOfLastPlanet = 0;
		m_arroundThePlanets = new CRect[pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets()];	
		for (int i = 0; i < pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets(); i++)
		{
			CPlanet* planet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
			SizeOfLastPlanet = planet->GetSize();
			maxHabitants += planet->GetMaxHabitant();
			if (SizeOfLastPlanet == 0)
			{
				resizeItX += 75;
				resizeItY = 68;
			}
			else if (SizeOfLastPlanet == 1)
			{
				resizeItX += 90;
				resizeItY = 60;
			}			
			else if (SizeOfLastPlanet == 2) 
			{
				resizeItX += 105;
				resizeItY = 50;				
			}
			else if (SizeOfLastPlanet==3)
			{
				resizeItX += 145;
				resizeItY = 28;				
			}
			m_arroundThePlanets[i].SetRect(900-resizeItX+resizeItX,20,900-resizeItX,0+resizeItY+resizeItY+200);
			
			// Planeten etwas mehr nach unten gerückt
			resizeItY += 15;
			CRect planetRect;
			planetRect.SetRect(900-resizeItX+resizeItX,0+resizeItY,900-resizeItX,0+resizeItY+resizeItY);
			planet->DrawPlanet(g, planetRect, pDoc->GetGraphicPool());
			m_arroundThePlanets[i].NormalizeRect();					
		}				
	}
	if (pDoc->GetSector(KO.x,KO.y).GetSunSystem() && pDoc->GetSector(KO.x,KO.y).GetScanned(pMajor->GetRaceID()))
	{
		graphic = NULL;
		switch(pDoc->GetSector(KO.x,KO.y).GetSunColor())
		{
		case 0:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_blue.png"); break;
		case 1:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_green.png"); break;
		case 2:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_orange.png"); break;
		case 3:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_red.png"); break;
		case 4:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_violet.png"); break;			
		case 5:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_white.png"); break;
		case 6:
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_yellow.png"); break;					
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
		s.Format("%s: %s",CResourceManager::GetString("SYSTEM"), pDoc->m_Sector[KO.x][KO.y].GetName());
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,47), &fontFormat, &fontBrush);
		if (pDoc->m_Sector[KO.x][KO.y].GetFullKnown(pMajor->GetRaceID()))
		{
			s.Format("%s: %.3lf Mrd.",CResourceManager::GetString("MAX_HABITANTS"), maxHabitants);
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,180), &fontFormat, &fontBrush);
			s.Format("%s: %.3lf Mrd.",CResourceManager::GetString("CURRENT_HABITANTS"), currentHabitants);
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,202), &fontFormat, &fontBrush);
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\popmaxSmall.png");
			if (graphic)
				g.DrawImage(graphic, 23, 180, 20, 16);
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\populationSmall.png");			
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
		if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) <= 0)
			fontBrush.SetColor(Color(245,0,0));
		if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) > 0)
			fontBrush.SetColor(Color(230,100,0));					
		if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 25)
			fontBrush.SetColor(Color(230,230,20));					
		if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 50)
			fontBrush.SetColor(Color(50,180,50));					
		if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 75)
			fontBrush.SetColor(Color(0,245,0));
		
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
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	CPoint KO = pDoc->GetKO();
	for (int i = 0; i < pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets(); i++)
		if (m_arroundThePlanets != 0 && m_arroundThePlanets[i].PtInRect(point))
		{
			CPlanet* planet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
			if (planet != CSmallInfoView::GetPlanet())
			{
				CSmallInfoView::SetPlanet(planet);
				CSmallInfoView::SetPlanetStats(true);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			}
			break;
		}

	CBottomBaseView::OnMouseMove(nFlags, point);
}
