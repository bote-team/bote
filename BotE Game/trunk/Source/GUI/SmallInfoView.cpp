// CSmallInfoView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BotE.h"
#include "SmallInfoView.h"
#include "GalaxyMenuView.h"
#include "BotEDoc.h"
#include "MainFrm.h"
#include "Galaxy\Planet.h"
#include "Races\RaceController.h"
#include "Iniloader.h"
#include "Ships/Ships.h"
#include "General/Loc.h"
#include "GraphicPool.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmallInfoView
CPlanet* CSmallInfoView::m_pPlanet = NULL;
CSmallInfoView::DISPLAY_MODE CSmallInfoView::m_DisplayMode = CSmallInfoView::DISPLAY_MODE_ICON;

IMPLEMENT_DYNCREATE(CSmallInfoView, CView)

CSmallInfoView::CSmallInfoView() :
	m_bAnimatedIcon(false)
{
	m_nTimer = 0;
	CIniLoader::GetInstance()->ReadValue("Video", "ANIMATEDICON", m_bAnimatedIcon);
}

CSmallInfoView::~CSmallInfoView()
{
}


BEGIN_MESSAGE_MAP(CSmallInfoView, CView)
	//{{AFX_MSG_MAP(CSmallInfoView)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CSmallInfoView

void CSmallInfoView::OnDraw(CDC* pDC)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CString sID = pDoc->GetPlayersRaceID();
	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sID));
	if (!pMajor)
	{
		ASSERT(pMajor);
		this->KillTimer(1);
		m_nTimer = 0;
		return;
	}
	if (pDoc->m_bRoundEndPressed)
		return;

	// ZU ERLEDIGEN: Code zum Zeichnen hier einfügen
	CRect r;
	r.SetRect(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	// Doublebuffering wird initialisiert
	CRect client;
	GetClientRect(&client);
	Graphics doubleBuffer(pDC->GetSafeHdc());
	doubleBuffer.SetSmoothingMode(SmoothingModeHighQuality);
	doubleBuffer.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	doubleBuffer.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Bitmap bmp(client.Width(), client.Height());
	Graphics* g = Graphics::FromImage(&bmp);
	g->Clear(Color::Black);
	g->SetSmoothingMode(SmoothingModeHighQuality);
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g->SetPixelOffsetMode(PixelOffsetModeHighQuality);
	g->ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);

	////////////// Überprüfen, ob in der CSmallInfoView Planeten angezeigt werden //////////////////////
	if (resources::pMainFrame->GetActiveView(1, 1) == VIEWS::PLANET_BOTTOM_VIEW && m_pPlanet != NULL
		&& (m_DisplayMode == DISPLAY_MODE_PLANET_STATS || m_DisplayMode == DISPLAY_MODE_PLANET_INFO))
	{
		this->KillTimer(1);
		m_nTimer = 0;
		// Überprüfen ob Statistik zu Planet angezeigt werden kann, sprich Maus muß drüber gehalten werden
		if (m_DisplayMode == DISPLAY_MODE_PLANET_STATS)
		{
			// gibt es eine spezielle Grafik für den Planeten, so wird versucht diese zu laden
			Bitmap* planet = NULL;
			if(m_pPlanet->HasIndividualGraphic())
				planet = pDoc->GetGraphicPool()->GetGDIGraphic("Planets\\" + m_pPlanet->GetPlanetName()+".bop");
			// ansonsten wird die zufällige Planetengrafik geladen
			if (planet == NULL)
				planet = pDoc->GetGraphicPool()->GetGDIGraphic(m_pPlanet->GetGraphicFile());

			if (planet)
			{
				CPoint center = client.CenterPoint();
				CalcLogicalPoint(center);
				CSize size(planet->GetWidth() * 0.85, planet->GetHeight() * 0.85);
				RectF drawRect(center.x - size.cx, center.y - size.cy + 20, 2 * size.cx, 2 * size.cy);
				g->DrawImage(planet, drawRect);
				// Planet etwas abdunkeln
				Gdiplus::SolidBrush brush(Gdiplus::Color(130, 0, 0, 0));
				g->FillRectangle(&brush, drawRect);

			}

			// Ab hier werden die ganzen Statistiken gezeichnet
			CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentFar);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			Color color;
			CFontLoader::GetGDIFontColor(pMajor, 3, color);
			fontBrush.SetColor(color);

			CString s;
			s.Format("%s: %s",CLoc::GetString("NAME"), m_pPlanet->GetPlanetName());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,55), &fontFormat, &fontBrush);
			s.Format("%s: %c",CLoc::GetString("CLASS"), m_pPlanet->GetClass());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,75), &fontFormat, &fontBrush);
			s.Format("%s: %.3lf %s",CLoc::GetString("MAX_HABITANTS"), m_pPlanet->GetMaxHabitant(), CLoc::GetString("MRD"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,95), &fontFormat, &fontBrush);
			s.Format("%s: %.3lf %s",CLoc::GetString("CURRENT_HABITANTS"), m_pPlanet->GetCurrentHabitant(), CLoc::GetString("MRD"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,115), &fontFormat, &fontBrush);
			s.Format("%s: %.2lf %% ",CLoc::GetString("GROWTH"), m_pPlanet->GetPlanetGrowth());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,135), &fontFormat, &fontBrush);
			if (m_pPlanet->GetNeededTerraformPoints() > 0)
			{
				s.Format("%s: %d %s",CLoc::GetString("TERRAFORM_ORDER"),
					m_pPlanet->GetNeededTerraformPoints(), CLoc::GetString("POINTS_SHORT"));
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,155), &fontFormat, &fontBrush);
			}
			// Die auf dem Planeten vorhanden Rohstoffe anzeigen
			// Steht so auch in der ConsumeRessources() Funktion der Klasse CMinorRace und in der BuildBuildingsForMinorRace()
			// Funktion in der CSystem Klasse, also wenn hier was geändert auch dort ändern!!
			//char PlanetClass = m_pPlanet->GetClass();
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			if (m_pPlanet->GetHabitable())
			{
				s = CLoc::GetString("EXISTING_RES") + ":";
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,165,r.right,25), &fontFormat, &fontBrush);
			}

			BOOLEAN res[DERITIUM + 1] = {FALSE};
			if (m_pPlanet->GetHabitable())
				m_pPlanet->GetAvailableResources(res);

			int n = 0;
			for (int i = TITAN; i <= DERITIUM; i++)
				n += res[i];

			int xPos = r.Width() / 2;
			xPos -= n * 10;
			int nExist = 0;
			for (int i = TITAN; i <= DERITIUM; i++)
			{
				if (res[i])
				{
					Bitmap* graphic = NULL;
					switch(i)
					{
						case TITAN:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop");		break;
						case DEUTERIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop");	break;
						case DURANIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop");	break;
						case CRYSTAL:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop");		break;
						case IRIDIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop");		break;
						case DERITIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\Deritium.bop");		break;
					}
					if (graphic)
						g->DrawImage(graphic, xPos + nExist * 20, 195, 20, 16);
					nExist++;
				}
			}

			/*
			s.Format("");
			if (PlanetClass == 'C')
				s = CLoc::GetString("EXISTING_RES")+":\n"+
				CLoc::GetString("TITAN")+", "+CLoc::GetString("IRIDIUM");
			else if (PlanetClass == 'F')
				s = CLoc::GetString("EXISTING_RES")+":\n"+
				CLoc::GetString("TITAN")+", "+CLoc::GetString("DURANIUM");
			else if (PlanetClass == 'G')
				s = CLoc::GetString("EXISTING_RES")+":\n"+
				CLoc::GetString("TITAN")+", "+CLoc::GetString("DURANIUM")+", "+CLoc::GetString("CRYSTAL");
			else if (PlanetClass == 'H')
				s = CLoc::GetString("EXISTING_RES")+":\n"+CLoc::GetString("IRIDIUM");
			else if (PlanetClass == 'K')
				s = CLoc::GetString("EXISTING_RES")+":\n"+
				CLoc::GetString("TITAN")+", "+CLoc::GetString("DURANIUM");
			else if (PlanetClass == 'L')
				s = CLoc::GetString("EXISTING_RES")+":\n"+
				CLoc::GetString("TITAN")+", "+CLoc::GetString("DEUTERIUM");
			else if (PlanetClass == 'M')
				s = CLoc::GetString("EXISTING_RES")+":\n"+
				CLoc::GetString("TITAN")+", "+CLoc::GetString("DEUTERIUM")+", "+
				CLoc::GetString("DURANIUM")+", "+CLoc::GetString("CRYSTAL")+", "+CLoc::GetString("IRIDIUM");
			else if (PlanetClass == 'N')
				s = CLoc::GetString("EXISTING_RES")+":\n"+CLoc::GetString("DEUTERIUM");
			else if (PlanetClass == 'O')
			s = CLoc::GetString("EXISTING_RES")+":\n"+CLoc::GetString("DEUTERIUM");
			else if (PlanetClass == 'P')
				s = CLoc::GetString("EXISTING_RES")+":\n"+
				CLoc::GetString("TITAN")+", "+CLoc::GetString("CRYSTAL")+", "+CLoc::GetString("IRIDIUM");
			else if (PlanetClass == 'Q')
				s = CLoc::GetString("EXISTING_RES")+":\n"+CLoc::GetString("CRYSTAL");
			else if (PlanetClass == 'R')
				s = CLoc::GetString("EXISTING_RES")+":\n"+CLoc::GetString("DURANIUM");

			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,165,r.right,85), &fontFormat, &fontBrush);
			*/
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			fontBrush.SetColor(Color(220,220,220));
			s.Format("I N F O R M A T I O N");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize, FontStyleUnderline), RectF(0,0,r.right,25), &fontFormat, &fontBrush);
		}

		// Überprüfen, ob auf den Planeten geklickt wurde und dann Informationen über Klasse anzeigen
		else if (m_DisplayMode == DISPLAY_MODE_PLANET_INFO)
		{
			char PlanetClass = m_pPlanet->GetClass();

			// gibt es eine spezielle Grafik für den Planeten, so wird versucht diese zu laden
			Bitmap* planet = pDoc->GetGraphicPool()->GetGDIGraphic("Planets\\" + m_pPlanet->GetPlanetName()+".bop");
			// ansonsten wird die zufällige Planetengrafik geladen
			if (planet == NULL)
				planet = pDoc->GetGraphicPool()->GetGDIGraphic(m_pPlanet->GetGraphicFile());
			if (planet)
			{
				CPoint center = client.CenterPoint();
				CalcLogicalPoint(center);
				CSize size(planet->GetWidth() * 0.85, planet->GetHeight() * 0.85);
				RectF drawRect(center.x - size.cx, center.y - size.cy + 20, 2 * size.cx, 2 * size.cy);
				g->DrawImage(planet, drawRect);
				// Planet etwas abdunkeln
				Gdiplus::SolidBrush brush(Gdiplus::Color(130, 0, 0, 0));
				g->FillRectangle(&brush, drawRect);
			}

			// Ab hier werden die ganzen Statistiken gezeichnet
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			fontBrush.SetColor(Color(200,200,200));

			g->DrawString(CComBSTR(CheckPlanetClassForInfoHead(PlanetClass)), -1, &Gdiplus::Font(L"Arial", 10), RectF(0,0,r.right,20), &fontFormat, &fontBrush);
			Color color;
			CFontLoader::GetGDIFontColor(pMajor, 3, color);
			fontBrush.SetColor(color);
			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(CComBSTR(CheckPlanetClassForInfo(PlanetClass)), -1, &Gdiplus::Font(L"Arial", 8.5), RectF(10,25,r.right-10,r.bottom-25), &fontFormat, &fontBrush);
		}
	}
	////////////// Überprüfen, ob in der CSmallInfoView Planeten angezeigt werden, hier zu ENDE //////////////////////

	////////////// Überprüfen, ob in der CSmallInfoView Schiffe angezeigt werden /////////////////////////////////////
	else if (resources::pMainFrame->GetActiveView(1, 1) == VIEWS::SHIP_BOTTOM_VIEW
		&& m_DisplayMode == DISPLAY_MODE_SHIP_BOTTEM_VIEW || m_DisplayMode == DISPLAY_MODE_FLEET_MENU_VIEW)
	{
		this->KillTimer(1);
		m_nTimer = 0;

		CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
		Color color;
		CFontLoader::GetGDIFontColor(pMajor, 3, color);
		fontBrush.SetColor(Color(200,200,200));
		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentFar);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

		CString Range;
		CString s;

		const CShipMap::const_iterator& pShip = GetShip(*pDoc);
		bool bUnknown = (pMajor->GetRaceID() != pShip->second->GetOwnerOfShip()
			&& pMajor->IsRaceContacted(pShip->second->GetOwnerOfShip()) == false);
		if (bUnknown)
		{
			// Wenn kein diplomatischer Kontakt möglich ist, wird das Schiff immer angezeigt
			CRace* pShipOwner = pDoc->GetRaceCtrl()->GetRace(pShip->second->GetOwnerOfShip());
			if (pShipOwner)
				bUnknown = !pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY);
		}

		// ist der Besitzer des Schiffes bekannt
		if (bUnknown)
		{
			s = _T("Ships\\Unknown.bop");
			Bitmap* shipGraphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
			if (shipGraphic == NULL)
				shipGraphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
			if (shipGraphic)
			{
				g->DrawImage(shipGraphic, 0, 50, 200, 150);
				// Schiff etwas abdunkeln
				Gdiplus::SolidBrush brush(Gdiplus::Color(175, 0, 0, 0));
				g->FillRectangle(&brush, 0, 50, 200, 150);
			}
		}
		// Wenn wir ein einzelnes Schiff anzeigen und nicht eine Flotte
		else if (!pShip->second->HasFleet() || pShip->second->LeaderIsCurrent())
		{
			// Schiffsgrafik etwas größer anzeigen
			s.Format("Ships\\%s.bop", pShip->second->GetShipClass());
			Bitmap* shipGraphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
			if (shipGraphic == NULL)
				shipGraphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
			if (shipGraphic)
			{
				g->DrawImage(shipGraphic, 0, 50, 200, 150);
				// Schiff etwas abdunkeln
				Gdiplus::SolidBrush brush(Gdiplus::Color(175, 0, 0, 0));
				g->FillRectangle(&brush, 0, 50, 200, 150);
			}
			Range = pShip->second->GetRangeAsString();

			s.Format("%s (%s: %d)",pShip->second->GetShipName(), CLoc::GetString("EXPERIANCE"), pShip->second->GetCrewExperience());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,45), &fontFormat, &fontBrush);

			fontBrush.SetColor(color);
			s.Format("%s: %s",CLoc::GetString("TYPE"),	pShip->second->GetShipTypeAsString(FALSE));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,85), &fontFormat, &fontBrush);

			s.Format("%s: %i / %i",CLoc::GetString("HULL"), pShip->second->GetHull()->GetCurrentHull(),pShip->second->GetHull()->GetMaxHull());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,105), &fontFormat, &fontBrush);

			s.Format("%s: %i / %i",CLoc::GetString("SHIELDS"), pShip->second->GetShield()->GetCurrentShield(),pShip->second->GetShield()->GetMaxShield());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,125), &fontFormat, &fontBrush);

			s.Format("%s: %s",CLoc::GetString("RANGE"), Range);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,145), &fontFormat, &fontBrush);

			s.Format("%s: %i",CLoc::GetString("SPEED"), pShip->second->GetSpeed(false));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,165), &fontFormat, &fontBrush);
		}
		// Wenn wir Infomationen zur Flotte anzeigen
		else
		{
			assert(pShip == pDoc->CurrentShip());
			Range = pShip->second->GetRangeAsString();
			r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);

			s.Format("%d %s",pShip->second->GetFleetSize()+1,
				CLoc::GetString("SHIPS"));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,45), &fontFormat, &fontBrush);

			fontBrush.SetColor(color);

			if (pShip->second->GetFleetShipType() != -1)
				s.Format("%s: %s",CLoc::GetString("TYPE"),
				pShip->second->GetShipTypeAsString(TRUE));
			else
				s = CLoc::GetString("MIXED_FLEET");
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,85), &fontFormat, &fontBrush);

			s.Format("%s: %s",CLoc::GetString("RANGE"), Range);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,105), &fontFormat, &fontBrush);

			s.Format("%s: %d",CLoc::GetString("SPEED"),
				pShip->second->GetSpeed(true));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,125), &fontFormat, &fontBrush);
		}

		CPoint TargetKO = pShip->second->GetTargetKO();
		if (TargetKO.x == -1 && pShip->second->GetOwnerOfShip() != pMajor->GetRaceID())
			s = CLoc::GetString("UNKNOWN_TARGET");
		if (TargetKO.x != -1 && pShip->second->GetOwnerOfShip() == pMajor->GetRaceID())
		{
			if(m_DisplayMode == DISPLAY_MODE_SHIP_BOTTEM_VIEW)
				assert(pShip == pDoc->CurrentShip());
			else//(m_ShipDisplayMode == SHIP_DISPLAY_MODE_FLEET_VIEW)
			{
				const CShips::const_iterator& fleetship = pDoc->FleetShip();
				if(fleetship->second->LeaderIsCurrent())
					assert(pShip == pDoc->FleetShip());
				else
					assert(pShip == fleetship->second->CurrentShip());
			}
			short range = 3-pShip->second->GetRange(true);
			short speed = pShip->second->GetSpeed(true);
			CArray<Sector> path;
			Sector position(pShip->second->GetKO().x, pShip->second->GetKO().y);
			Sector target(pShip->second->GetTargetKO().x, pShip->second->GetTargetKO().y);
			pMajor->GetStarmap()->CalcPath(position, target, range, speed, path);
			short rounds = 0;
			if (speed > 0)
				rounds = ceil((float)path.GetSize() / (float)speed);
			if (rounds > 1)
				s.Format("%s: %c%i (%d %s)", CLoc::GetString("TARGET"), (char)(TargetKO.y+97),TargetKO.x+1, rounds, CLoc::GetString("ROUNDS"));
			else
				s.Format("%s: %c%i (%d %s)", CLoc::GetString("TARGET"), (char)(TargetKO.y+97),TargetKO.x+1, rounds, CLoc::GetString("ROUND"));
		}
		if (TargetKO.x == -1 && pShip->second->GetOwnerOfShip() == pMajor->GetRaceID())
			s = CLoc::GetString("NO_TARGET");
		if (TargetKO.x != -1 && pShip->second->GetOwnerOfShip() != pMajor->GetRaceID())
			s = CLoc::GetString("UNKNOWN_TARGET");
		if (TargetKO.x == pShip->second->GetKO().x &&
			TargetKO.y == pShip->second->GetKO().y &&
			pShip->second->GetOwnerOfShip() == pMajor->GetRaceID())
			s = CLoc::GetString("NO_TARGET");
		if (CGalaxyMenuView::IsMoveShip() == TRUE)
		{
			fontBrush.SetColor(Color(0,225,0));
			s.Format("--- %s ---", CLoc::GetString("SET_MOVEMENT_TARGET").MakeUpper());
		}
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,r.right,185), &fontFormat, &fontBrush);
		// bei eigenem Schiff aktuellen Befehl zeichnen
		if (pShip->second->GetOwnerOfShip() == pMajor->GetRaceID())
		{
			pDC->SetTextColor(CFontLoader::GetFontColor(pMajor, 4));
			CFontLoader::GetGDIFontColor(pMajor, 4, color);
			fontBrush.SetColor(color);
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			s.Format("%s: %s",CLoc::GetString("COMBAT_BEHAVIOR"), pShip->second->GetCombatTacticAsString());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,190,r.right,50), &fontFormat, &fontBrush);
			// Name des Planeten ermitteln, welche gerade geterraformt wird
			if (pShip->second->GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
			{
				assert(pShip->second->GetTerraform() != -1);
				if (static_cast<int>(pDoc->GetSector(pShip->second->GetKO().x, pShip->second->GetKO().y).GetPlanets().size()) > pShip->second->GetTerraform())
					s.Format("%s: %s\n%s",CLoc::GetString("ORDER"), pShip->second->GetCurrentOrderAsString(), pDoc->GetSector(pShip->second->GetKO().x, pShip->second->GetKO().y).GetPlanet(pShip->second->GetTerraform())->GetPlanetName());
			}
			else
				s.Format("%s: %s",CLoc::GetString("ORDER"), pShip->second->GetCurrentOrderAsString());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,210,r.right,50), &fontFormat, &fontBrush);
		}
	}
	else
	{
		if (m_nTimer > 440)
			m_nTimer = 0;
		CIniLoader::GetInstance()->ReadValue("Video", "ANIMATEDICON", m_bAnimatedIcon);
		if (m_nTimer == 0 && m_bAnimatedIcon)
			this->SetTimer(1,125,NULL);

		// Wenn keine Informationen zu einem Planeten angezeigt werden sollen, dann das Rassensymbol einblenden
		CString path;
		path.Format("Symbols\\%s.bop", pMajor->GetRaceID());
		Bitmap* logo = pDoc->GetGraphicPool()->GetGDIGraphic(path);
		if (logo)
		{
			int nAlpha = m_nTimer;
			if (m_nTimer > 220)
				nAlpha = 440 - m_nTimer;
			g->DrawImage(logo, 0, 25, 200, 200);
			SolidBrush brush(Color(nAlpha, 0, 0, 0));
			g->FillRectangle(&brush, 0, 0, m_TotalSize.cx, m_TotalSize.cy);
		}
	}

	doubleBuffer.DrawImage(&bmp, client.left, client.top, client.right, client.bottom);
	delete g;
}

void CSmallInfoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	m_TotalSize.cx = 200;
	m_TotalSize.cy = 249;
}

void CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE mode) {
	m_DisplayMode = mode;
}

const CShipMap::const_iterator& CSmallInfoView::GetShip(const CBotEDoc& doc)
{
	if(m_DisplayMode == CSmallInfoView::DISPLAY_MODE_FLEET_MENU_VIEW) {
		const CShipMap::const_iterator& fleetship = doc.FleetShip();
		if(fleetship->second->LeaderIsCurrent())
			return fleetship;
		return fleetship->second->CurrentShip();
	}
	assert(m_DisplayMode == DISPLAY_MODE_SHIP_BOTTEM_VIEW);
	return doc.CurrentShip();
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CSmallInfoView

#ifdef _DEBUG
void CSmallInfoView::AssertValid() const
{
	CView::AssertValid();
}

void CSmallInfoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSmallInfoView


const CString CSmallInfoView::CheckPlanetClassForInfo(char PlanetClass) // Funktion, die die Informationen für die Planetenklasse anließt und die richtige auswählt
{
	CString s;
	s.Format("CLASS_%c_INFO", PlanetClass);
	return CLoc::GetString(s);
}


const CString CSmallInfoView::CheckPlanetClassForInfoHead(char PlanetClass) // Funktion, die die Überschrift zurückgibt
{
	CString s;
	s.Format("CLASS_%c_TYPE", PlanetClass);
	CString s2;
	s2.Format("%s %c - %s",CLoc::GetString("CLASS"), PlanetClass, CLoc::GetString(s));
	return s2;
}

BOOL CSmallInfoView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	return FALSE;
}

void CSmallInfoView::CalcLogicalPoint(CPoint &point)
{
	CRect client;
	GetClientRect(&client);

	point.x *= (float)m_TotalSize.cx / (float)client.Width();
	point.y *= (float)m_TotalSize.cy / (float)client.Height();
}

void CSmallInfoView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_nTimer += 4;
	Invalidate(FALSE);

	CView::OnTimer(nIDEvent);
}
