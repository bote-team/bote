// CSmallInfoView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "botf2.h"
#include "SmallInfoView.h"
#include "GalaxyMenuView.h"
#include "Botf2Doc.h"
#include "MainFrm.h"
#include "Galaxy\Planet.h"
#include "Races\RaceController.h"
#include "Ships\Fleet.h"
#include "Iniloader.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmallInfoView

BOOLEAN CSmallInfoView::m_bShowShipInfo = FALSE;
BOOLEAN CSmallInfoView::m_bShowPlanetInfo = FALSE;
BOOLEAN CSmallInfoView::m_bShowPlanetStats = FALSE;
CPlanet* CSmallInfoView::m_pPlanet = NULL;
CShip* CSmallInfoView::m_pShip = NULL;

IMPLEMENT_DYNCREATE(CSmallInfoView, CView)

CSmallInfoView::CSmallInfoView()
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
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
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
	if (pDoc->GetMainFrame()->GetActiveView(1, 1) == PLANET_BOTTOM_VIEW && m_pPlanet != NULL && (m_bShowPlanetStats || m_bShowPlanetInfo))
	{
		this->KillTimer(1);
		m_nTimer = 0;
		// Überprüfen ob Statistik zu Planet angezeigt werden kann, sprich Maus muß drüber gehalten werden
		if (m_bShowPlanetStats)
		{
			// gibt es eine spezielle Grafik für den Planeten, so wird versucht diese zu laden
			Bitmap* planet = NULL;
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
			s.Format("%s: %s",CResourceManager::GetString("NAME"), m_pPlanet->GetPlanetName());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,55), &fontFormat, &fontBrush);
			s.Format("%s: %c",CResourceManager::GetString("CLASS"), m_pPlanet->GetClass());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,75), &fontFormat, &fontBrush);
			s.Format("%s: %.3lf %s",CResourceManager::GetString("MAX_HABITANTS"), m_pPlanet->GetMaxHabitant(), CResourceManager::GetString("MRD"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,95), &fontFormat, &fontBrush);
			s.Format("%s: %.3lf %s",CResourceManager::GetString("CURRENT_HABITANTS"), m_pPlanet->GetCurrentHabitant(), CResourceManager::GetString("MRD"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,115), &fontFormat, &fontBrush);
			s.Format("%s: %.2lf %% ",CResourceManager::GetString("GROWTH"), m_pPlanet->GetPlanetGrowth());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,135), &fontFormat, &fontBrush);
			if (m_pPlanet->GetNeededTerraformPoints() > 0)
			{
				s.Format("%s: %d %s",CResourceManager::GetString("TERRAFORM_ORDER"),
					m_pPlanet->GetNeededTerraformPoints(), CResourceManager::GetString("POINTS_SHORT"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,155), &fontFormat, &fontBrush);
			}
			// Die auf dem Planeten vorhanden Rohstoffe anzeigen
			// Steht so auch in der ConsumeRessources() Funktion der Klasse CMinorRace und in der BuildBuildingsForMinorRace()
			// Funktion in der CSystem Klasse, also wenn hier was geändert auch dort ändern!!
			char PlanetClass = m_pPlanet->GetClass();
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			if (m_pPlanet->GetHabitable())
			{
				s = CResourceManager::GetString("EXISTING_RES") + ":";
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,165,r.right,25), &fontFormat, &fontBrush);
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
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+
				CResourceManager::GetString("TITAN")+", "+CResourceManager::GetString("IRIDIUM");
			else if (PlanetClass == 'F')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+
				CResourceManager::GetString("TITAN")+", "+CResourceManager::GetString("DURANIUM");
			else if (PlanetClass == 'G')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+
				CResourceManager::GetString("TITAN")+", "+CResourceManager::GetString("DURANIUM")+", "+CResourceManager::GetString("CRYSTAL");
			else if (PlanetClass == 'H')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+CResourceManager::GetString("IRIDIUM");
			else if (PlanetClass == 'K')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+
				CResourceManager::GetString("TITAN")+", "+CResourceManager::GetString("DURANIUM");
			else if (PlanetClass == 'L')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+
				CResourceManager::GetString("TITAN")+", "+CResourceManager::GetString("DEUTERIUM");
			else if (PlanetClass == 'M')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+
				CResourceManager::GetString("TITAN")+", "+CResourceManager::GetString("DEUTERIUM")+", "+
				CResourceManager::GetString("DURANIUM")+", "+CResourceManager::GetString("CRYSTAL")+", "+CResourceManager::GetString("IRIDIUM");
			else if (PlanetClass == 'N')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+CResourceManager::GetString("DEUTERIUM");
			else if (PlanetClass == 'O')
			s = CResourceManager::GetString("EXISTING_RES")+":\n"+CResourceManager::GetString("DEUTERIUM");
			else if (PlanetClass == 'P')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+
				CResourceManager::GetString("TITAN")+", "+CResourceManager::GetString("CRYSTAL")+", "+CResourceManager::GetString("IRIDIUM");
			else if (PlanetClass == 'Q')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+CResourceManager::GetString("CRYSTAL");
			else if (PlanetClass == 'R')
				s = CResourceManager::GetString("EXISTING_RES")+":\n"+CResourceManager::GetString("DURANIUM");
			
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,165,r.right,85), &fontFormat, &fontBrush);
			*/
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			fontBrush.SetColor(Color(220,220,220));
			s.Format("I N F O R M A T I O N");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize, FontStyleUnderline), RectF(0,0,r.right,25), &fontFormat, &fontBrush);
		}

		// Überprüfen, ob auf den Planeten geklickt wurde und dann Informationen über Klasse anzeigen
		else if (m_bShowPlanetInfo)
		{
			char PlanetClass = m_pPlanet->GetClass();
						
			// gibt es eine spezielle Grafik für den Planeten, so wird versucht diese zu laden
			Bitmap* planet = NULL;
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
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			fontBrush.SetColor(Color(200,200,200));

			g->DrawString(CheckPlanetClassForInfoHead(PlanetClass).AllocSysString(), -1, &Gdiplus::Font(L"Arial", 10), RectF(0,0,r.right,20), &fontFormat, &fontBrush);
			Color color;
			CFontLoader::GetGDIFontColor(pMajor, 3, color);
			fontBrush.SetColor(color);
			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(CheckPlanetClassForInfo(PlanetClass).AllocSysString(), -1, &Gdiplus::Font(L"Arial", 8.5), RectF(10,25,r.right-10,r.bottom-25), &fontFormat, &fontBrush);
		}		
	}
	////////////// Überprüfen, ob in der CSmallInfoView Planeten angezeigt werden, hier zu ENDE //////////////////////

	////////////// Überprüfen, ob in der CSmallInfoView Schiffe angezeigt werden /////////////////////////////////////
	else if (pDoc->GetMainFrame()->GetActiveView(1, 1) == SHIP_BOTTOM_VIEW && m_bShowShipInfo && m_pShip != NULL)
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
		bool bUnknown = (pMajor->GetRaceID() != m_pShip->GetOwnerOfShip() && pMajor->IsRaceContacted(m_pShip->GetOwnerOfShip()) == false);
		if (bUnknown)
		{
			// Wenn kein diplomatischer Kontakt möglich ist, wird das Schiff immer angezeigt
			CRace* pShipOwner = pDoc->GetRaceCtrl()->GetRace(m_pShip->GetOwnerOfShip());
			if (pShipOwner)
				bUnknown = !pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY);
		}
		
		// ist der Besitzer des Schiffes bekannt
		if (bUnknown)
		{
			Bitmap* shipGraphic = NULL;
			s = _T("Ships\\Unknown.bop");
			shipGraphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
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
		else if (m_pShip->GetFleet() == 0 || (m_pShip->GetFleet() != 0 && m_pShip->GetFleet()->GetFleetSize() == 0))
		{
			// Schiffsgrafik etwas größer anzeigen
			Bitmap* shipGraphic = NULL;
			s.Format("Ships\\%s.bop", m_pShip->GetShipClass());			
			shipGraphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
			if (shipGraphic == NULL)
				shipGraphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
			if (shipGraphic)
			{
				g->DrawImage(shipGraphic, 0, 50, 200, 150);
				// Schiff etwas abdunkeln
				Gdiplus::SolidBrush brush(Gdiplus::Color(175, 0, 0, 0));
				g->FillRectangle(&brush, 0, 50, 200, 150);
			}

			if (m_pShip->GetRange() == RANGE_SHORT)
				Range = CResourceManager::GetString("SHORT");
			else if (m_pShip->GetRange() == RANGE_MIDDLE)
				Range = CResourceManager::GetString("MIDDLE");
			else if (m_pShip->GetRange() == RANGE_LONG)
				Range = CResourceManager::GetString("LONG");
			
			s.Format("%s",m_pShip->GetShipName());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,45), &fontFormat, &fontBrush);
			
			fontBrush.SetColor(color);
			s.Format("%s: %s",CResourceManager::GetString("TYPE"),	m_pShip->GetShipTypeAsString(FALSE));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,85), &fontFormat, &fontBrush);
			
			s.Format("%s: %i / %i",CResourceManager::GetString("HULL"), m_pShip->GetHull()->GetCurrentHull(),m_pShip->GetHull()->GetMaxHull());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,105), &fontFormat, &fontBrush);

			s.Format("%s: %i / %i",CResourceManager::GetString("SHIELDS"), m_pShip->GetShield()->GetCurrentShield(),m_pShip->GetShield()->GetMaxShield());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,125), &fontFormat, &fontBrush);
			
			s.Format("%s: %s",CResourceManager::GetString("RANGE"), Range);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,145), &fontFormat, &fontBrush);
			
			s.Format("%s: %i",CResourceManager::GetString("SPEED"), m_pShip->GetSpeed());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,165), &fontFormat, &fontBrush);
		}
		// Wenn wir Infomationen zur Flotte anzeigen
		else
		{
			short range = m_pShip->GetFleet()->GetFleetRange(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()));
			if (range == 0)
				Range = CResourceManager::GetString("SHORT");
			else if (range == 1)
				Range = CResourceManager::GetString("MIDDLE");
			else if (range == 2)
				Range = CResourceManager::GetString("LONG");
			r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);

			s.Format("%d %s",m_pShip->GetFleet()->GetFleetSize()+1, 
				CResourceManager::GetString("SHIPS"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,45), &fontFormat, &fontBrush);

			fontBrush.SetColor(color);
			
			if (m_pShip->GetFleet()->GetFleetShipType(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex())) != -1)
				s.Format("%s: %s",CResourceManager::GetString("TYPE"),
				m_pShip->GetShipTypeAsString(TRUE));
			else
				s = CResourceManager::GetString("MIXED_FLEET");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,85), &fontFormat, &fontBrush);

			s.Format("%s: %s",CResourceManager::GetString("RANGE"), Range);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,105), &fontFormat, &fontBrush);
			
			s.Format("%s: %d",CResourceManager::GetString("SPEED"), 
				m_pShip->GetFleet()->GetFleetSpeed(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex())));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,125), &fontFormat, &fontBrush);
		}			
		
		CPoint TargetKO = m_pShip->GetTargetKO();
		if (TargetKO.x == -1 && m_pShip->GetOwnerOfShip() != pMajor->GetRaceID())
			s = CResourceManager::GetString("UNKNOWN_TARGET");
		if (TargetKO.x != -1 && m_pShip->GetOwnerOfShip() == pMajor->GetRaceID())
		{
			short n = pDoc->GetCurrentShipIndex();
			short range = 0;
			short speed = 0;
			// Wenn das Schiff keine Flotte anführt
			if (pDoc->m_ShipArray.GetAt(n).GetFleet() == 0 || (pDoc->m_ShipArray.GetAt(n).GetFleet() != 0 && pDoc->m_ShipArray.GetAt(n).GetFleet()->GetFleetSize() == 0))
			{
				range = 3 - pDoc->m_ShipArray.GetAt(n).GetRange();
				speed = pDoc->m_ShipArray.GetAt(n).GetSpeed();
			}
			else
			{
				range = 3-pDoc->m_ShipArray.GetAt(n).GetFleet()->GetFleetRange(&pDoc->m_ShipArray.GetAt(n));
				speed = pDoc->m_ShipArray.GetAt(n).GetFleet()->GetFleetSpeed(&pDoc->m_ShipArray.GetAt(n));
			}
			CArray<Sector> path;
			Sector position(pDoc->m_ShipArray[n].GetKO().x, pDoc->m_ShipArray[n].GetKO().y);
			Sector target(pDoc->m_ShipArray[n].GetTargetKO().x, pDoc->m_ShipArray[n].GetTargetKO().y);
			pMajor->GetStarmap()->CalcPath(position, target, range, speed, path);
			short rounds = 0;
			if (speed > 0)
				rounds = ceil((float)path.GetSize() / (float)speed);
			if (rounds > 1)
				s.Format("%s: %c%i (%d %s)", CResourceManager::GetString("TARGET"), (char)(TargetKO.y+97),TargetKO.x+1, rounds, CResourceManager::GetString("ROUNDS"));
			else
				s.Format("%s: %c%i (%d %s)", CResourceManager::GetString("TARGET"), (char)(TargetKO.y+97),TargetKO.x+1, rounds, CResourceManager::GetString("ROUND"));
		}
		if (TargetKO.x == -1 && m_pShip->GetOwnerOfShip() == pMajor->GetRaceID())
			s = CResourceManager::GetString("NO_TARGET");
		if (TargetKO.x != -1 && m_pShip->GetOwnerOfShip() != pMajor->GetRaceID())
			s = CResourceManager::GetString("UNKNOWN_TARGET");
		if (TargetKO.x == m_pShip->GetKO().x &&
			TargetKO.y == m_pShip->GetKO().y &&
			m_pShip->GetOwnerOfShip() == pMajor->GetRaceID())
			s = CResourceManager::GetString("NO_TARGET");
		if (CGalaxyMenuView::IsMoveShip() == TRUE)
		{
			fontBrush.SetColor(Color(0,225,0));
			s.Format("--- %s ---", CResourceManager::GetString("ACTION").MakeUpper());
		}
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,r.right,185), &fontFormat, &fontBrush);
		// bei eigenem Schiff aktuellen Befehl zeichnen
		if (m_pShip->GetOwnerOfShip() == pMajor->GetRaceID())
		{			
			pDC->SetTextColor(CFontLoader::GetFontColor(pMajor, 4));
			// Name des Planeten ermitteln, welche gerade geterraformt wird
			if (m_pShip->GetCurrentOrder() == TERRAFORM && m_pShip->GetTerraformingPlanet() != -1)
			{
				if (pDoc->GetSector(m_pShip->GetKO()).GetPlanets()->GetSize() > m_pShip->GetTerraformingPlanet())
					s.Format("%s: %s\n%s",CResourceManager::GetString("ORDER"), m_pShip->GetCurrentOrderAsString(), pDoc->GetSector(m_pShip->GetKO()).GetPlanet(m_pShip->GetTerraformingPlanet())->GetPlanetName());
			}
			else
				s.Format("%s: %s",CResourceManager::GetString("ORDER"), m_pShip->GetCurrentOrderAsString());
			CFontLoader::GetGDIFontColor(pMajor, 4, color);
			fontBrush.SetColor(color);
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,190,r.right,50), &fontFormat, &fontBrush);
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
		Bitmap* logo = NULL;
		logo = pDoc->GetGraphicPool()->GetGDIGraphic(path);
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
	return CResourceManager::GetString(s);
}


const CString CSmallInfoView::CheckPlanetClassForInfoHead(char PlanetClass) // Funktion, die die Überschrift zurückgibt
{
	CString s;
	s.Format("CLASS_%c_TYPE", PlanetClass);
	CString s2;
	s2.Format("%s %c - %s",CResourceManager::GetString("CLASS"), PlanetClass, CResourceManager::GetString(s));
	return s2;
}

BOOL CSmallInfoView::OnEraseBkgnd(CDC* pDC) 
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
