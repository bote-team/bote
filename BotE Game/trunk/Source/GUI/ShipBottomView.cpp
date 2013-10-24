// ShipBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "IOData.h"
#include "GalaxyMenuView.h"
#include "FleetMenuView.h"
#include "TransportMenuView.h"
#include "ShipBottomView.h"
#include "PlanetBottomView.h"
#include "SmallInfoView.h"
#include "MenuChooseView.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "Graphic\memdc.h"
#include "MainFrm.h"
#include "Ships/Ships.h"
#include "General/Loc.h"
#include "Galaxy/Anomaly.h"
#include "GraphicPool.h"
#include "ClientWorker.h"
#include <cassert>

#ifdef _DEBUG
// #define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CShipBottomView
BOOLEAN CShipBottomView::m_bShowStation = FALSE;

IMPLEMENT_DYNCREATE(CShipBottomView, CBottomBaseView)

CShipBottomView::CShipBottomView() :
	m_iPage(1),
	m_bShowNextButton(FALSE),
	m_iTimeCounter(0),
	m_iWhichMainShipOrderButton(MAIN_BUTTON_NONE),
	m_LastKO(-1, -1),
	m_dc(),
	bw(120),
	bh(30),
	bd(5),
	bt(35),
	bb(bt + (MAIN_BUTTON_CANCEL -1) * (bh + bd) + bh)
{
	m_pShipOrderButton = NULL;
	m_vMainShipOrders.reserve(MAIN_BUTTON_CANCEL);
	m_vSecondaryShipOrders.resize(SHIP_ORDER::UPGRADE_STARBASE + 1);
}

CShipBottomView::~CShipBottomView()
{
	if (m_pShipOrderButton)
	{
		delete m_pShipOrderButton;
		m_pShipOrderButton = NULL;
	}
}

BEGIN_MESSAGE_MAP(CShipBottomView, CBottomBaseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CShipBottomView::OnNewRound()
{
	m_iPage = 1;
	m_vShipRects.clear();
}

// CShipBottomView drawing

static bool ShipCanHaveOrder(const CShips& ships, SHIP_ORDER::Typ order, const CSystem* system = NULL) {
	switch(order) {
		case SHIP_ORDER::TRAIN_SHIP:
			return system->GetSunSystem() && system->GetOwnerOfSystem() == ships.GetOwnerOfShip()
				&& system->GetProduction()->GetShipTraining() > 0 && ships.CanHaveOrder(order, true);
		case SHIP_ORDER::REPAIR:
			return ships.CanHaveOrder(SHIP_ORDER::REPAIR, true)
				&& system->GetShipPort(ships.GetOwnerOfShip());
		case SHIP_ORDER::TRANSPORT:
			return ships.CanHaveOrder(order, true, false);
		case SHIP_ORDER::TERRAFORM:
			return ships.CanHaveOrder(order, false);
		case SHIP_ORDER::IMPROVE_SHIELDS:
			return ships.CanHaveOrder(order, true) && system->GetAnomaly() && system->GetAnomaly()
				->GetType() == IONSTORM;
	}
	return ships.CanHaveOrder(order, true);
}

void CShipBottomView::SetupDrawing() {
	CRect client;
	GetClientRect(&client);

	m_dc.fontName = "";
	m_dc.fontSize = 0.0;

	/*g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);*/

	m_dc.g->SetSmoothingMode(SmoothingModeHighQuality);
	m_dc.g->SetInterpolationMode(InterpolationModeLowQuality);
	m_dc.g->SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	m_dc.g->SetCompositingQuality(CompositingQualityHighSpeed);
	m_dc.g->ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);
	m_dc.g->Clear(Color::Black);

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(m_pPlayersRace, 2, m_dc.fontName, m_dc.fontSize);
	CFontLoader::GetGDIFontColor(m_pPlayersRace, 3, m_dc.normalColor);

	m_dc.fontBrush = new SolidBrush(m_dc.normalColor);

	m_dc.r.SetRect(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	// Angezeigten Sektor, aktuelle Seite und dergleichen initialisieren
	if (m_LastKO != m_dc.pDoc->GetKO())
	{
		m_LastKO = m_dc.pDoc->GetKO();
		m_iPage = 1;
		m_vShipRects.clear();

		KillTimer(1);
		m_iTimeCounter = 0;
	}

	if (m_bShowStation)
	{
		// Ebenfalls bei der Anzeige einer Station immer auf die erste Seite springen
		m_iPage = 1;
	}

	SetupMainButtons();
}

// Achtung: r.right = Breite, r.bottom = Höhe!
bool CShipBottomView::DrawImage( const CString& resName, const CRect& r ) const
{
	if (Bitmap* bmp = m_dc.gp->GetGDIGraphic(resName))
	{
		if (m_dc.g->DrawImage(bmp, r.left, r.top, r.Width(), r.Height()) == Gdiplus::Ok)
			return true;
	}

	return false;
}

void CShipBottomView::DrawSmallButton( const CString& resString, const CPoint& coords, SHIP_ORDER::Typ shiporder ) {
	m_dc.fontBrush->SetColor(this->GetFontColorForSmallButton());
	m_dc.fontFormat.SetAlignment(StringAlignmentCenter);
	m_dc.fontFormat.SetLineAlignment(StringAlignmentCenter);
	m_dc.fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	m_dc.g->DrawImage(m_pShipOrderButton, coords.x, coords.y, bw, bh);
	m_dc.g->DrawString(CComBSTR(CLoc::GetString(resString)), -1, &Gdiplus::Font(CComBSTR(m_dc.fontName), m_dc.fontSize), RectF(coords.x,coords.y,bw,bh), &m_dc.fontFormat, m_dc.fontBrush);

	if( shiporder != SHIP_ORDER::NONE ) {
		m_vSecondaryShipOrders.at(shiporder).rect.SetRect(coords.x,coords.y,coords.x + bw,coords.y + bh);
	}
}

bool CShipBottomView::CheckDisplayShip(CShips *pShip, const CSystem *system ) {
	if (m_LastKO != pShip->GetKO())
		return false;

	const BOOL is_base = pShip->IsStation();
	// Wenn eine Station angezeigt werden soll, dann muss der Typ von einer Station sein
	// Wenn keine Station angezeigt werden soll, dann darf der Typ nicht von einer Station sein
	if (m_bShowStation != is_base)
		return false;

	// Schiffe mit zu guter Stealthpower werden hier nicht angezeigt.
	const USHORT stealthPower = pShip->GetStealthPower();

	CString rid = m_pPlayersRace->GetRaceID();
	if (   pShip->GetOwnerOfShip() != rid				// Schiff gehört anderer Rasse als der aktuellen
		&& system->GetScanPower(rid) < stealthPower		// Scanpower im Sektor ist kleiner stealthpower des Schiffs
		&& m_pPlayersRace->GetAgreement(pShip->GetOwnerOfShip()) < DIPLOMATIC_AGREEMENT::AFFILIATION) // Diplomatie ist kleiner "Affiliation"
			return false;
	return true;
}

void CShipBottomView::DrawShipContent()
{
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);
	USHORT column = 0;
	USHORT row = 0;
	CMajor* pMajor = m_pPlayersRace;
	CShipMap::iterator oneShip = m_dc.pDoc->m_ShipMap.begin();
	Gdiplus::Color markColor;
	Gdiplus::Font font(CComBSTR(m_dc.fontName), m_dc.fontSize);
	USHORT counter = 0;
	CShips* pShip;

	m_vShipRects.clear();

	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	const CSystem& system = m_dc.pDoc->CurrentSystem();

	for (CShipMap::iterator i = m_dc.pDoc->m_ShipMap.begin(); i != m_dc.pDoc->m_ShipMap.end(); ++i)
	{
		pShip = i->second;
		if (!CheckDisplayShip( pShip, &system ) )
			continue;

		// mehrere Spalten anlegen, falls mehr als 3 Schiffe in dem System sind
		if (counter != 0 && counter%3 == 0)
		{
			column++;
			row = 0;
		}
		// Wenn wir eine Seite vollhaben
		if (counter%9 == 0)
			column = 0;

		if (counter < m_iPage*9 && counter >= (m_iPage-1)*9)
		{
			CPoint pt(250 * column, 65 * row);
			m_vShipRects.push_back(pair<CRect, CShips*>(CRect(pt.x, pt.y + 20, pt.x + 250, pt.y + 85), pShip));
		}

		// großes Bild der Station zeichnen
		if (m_bShowStation)
		{
			if (m_dc.pDoc->CurrentSystem().GetIsStationInSector())
			{
				// gehört uns die Station oder kennen wir die andere Rasse
				if (pMajor->GetRaceID() == pShip->GetOwnerOfShip() || pMajor->IsRaceContacted(pShip->GetOwnerOfShip()))
				{
					// Major holen, welcher die Station besitzt
					if (CMajor* pStationOwner = dynamic_cast<CMajor*>(m_dc.pDoc->GetRaceCtrl()->GetRace(pShip->GetOwnerOfShip())))
					{
						// schönerer Grafik der Station des Majors groß anzeigen
						DrawImage("Other\\" + pStationOwner->GetPrefix() + "Starbase.bop", CRect(CPoint(550,20),CSize(235,200)));
					}
					else
					{
						// z.B. handelt es sich um den Ehlenen-Beschützer, dann die Stationsgrafik etwas größer anzeigen
						DrawImage("Ships\\" + pShip->GetShipClass() + ".bop", CRect(CPoint(550,30),CSize(235,175)));
					}
				}
			}

			break;
		}

		row++;
		counter++;
		oneShip = i;

		if (counter > m_iPage*9)
			break;
	}

	// Befindet sich die Ansicht noch auf einer zu großen Seite, dann diese automatisch soweit
	// zurücksetzen, bis wieder Schiffe sichtbar sind
	bool bInvalidateAgain = false;
	while (m_iPage > 1 && counter <= (m_iPage - 1) * 9)
	{
		bInvalidateAgain = true;
		m_iPage--;
	}
	if (bInvalidateAgain)
	{
		Invalidate(FALSE);
		return;
	}

	// Wenn nur ein Schiff in dem System ist, so wird es automatisch ausgewählt
	// Nicht automatisch machen, wenn zuvor genau das gleiche CurrentShip angewählt war. Dann könnte man
	// in der Galaxieansicht nicht mehr mit Rechts abbrechen, da das Schiff gleich wieder angewählt wird.
	if (counter == 1 && !m_bShowStation	&& oneShip->second->GetOwnerOfShip() == pMajor->GetRaceID() && oneShip->second != m_dc.pDoc->CurrentShip()->second)
	{
		// Wenn wenn wir auf der Galaxiekarte sind
		if (resources::pMainFrame->GetActiveView(0, 1) == VIEWS::GALAXY_VIEW)
		{
			SHIP_ORDER::Typ nOrder = oneShip->second->GetCurrentOrder();
			bool bDirectMove = true;
			if (nOrder != SHIP_ORDER::NONE && nOrder != SHIP_ORDER::AVOID && nOrder != SHIP_ORDER::ATTACK && nOrder != SHIP_ORDER::ENCLOAK && nOrder != SHIP_ORDER::DECLOAK && nOrder != SHIP_ORDER::ASSIGN_FLAGSHIP && nOrder != SHIP_ORDER::CREATE_FLEET && nOrder != SHIP_ORDER::TRANSPORT)
				bDirectMove = false;

			if (bDirectMove)
				ActivateShip(oneShip);
		}
	}

	// Schiffe jetzt auch zeichnen
	for(std::vector<std::pair<CRect, CShips*>>::const_iterator itdraw = m_vShipRects.begin(); itdraw != m_vShipRects.end(); ++itdraw)
	{
		pShip = itdraw->second;

		// Kennen wir den Besizter des Schiffes?
		bool bUnknown = (pMajor->GetRaceID() != pShip->GetOwnerOfShip() && pMajor->IsRaceContacted(pShip->GetOwnerOfShip()) == false);
		if (bUnknown)
		{
			// Wenn kein diplomatischer Kontakt möglich ist, wird das Schiff immer angezeigt
			CRace* pShipOwner = m_dc.pDoc->GetRaceCtrl()->GetRace(pShip->GetOwnerOfShip());
			if (pShipOwner)
				bUnknown = !pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY);
		}
		CRect loc = itdraw->first;
		// ist das Schiff gerade markiert?
		bool bMarked = (pShip == m_dc.pDoc->CurrentShip()->second);
		pShip->DrawShip(m_dc.g, m_dc.gp, CPoint(loc.left,loc.top-20), bMarked, bUnknown, TRUE, m_dc.normalColor, markColor, font);
	}

	// Die Buttons für vor und zurück darstellen, wenn wir mehr als 9 Schiffe in dem Sektor sehen
	m_bShowNextButton = FALSE;
	if (counter > 9 && counter > m_iPage*9)
	{
		m_bShowNextButton = TRUE;
		DrawSmallButton("BTN_NEXT", CPoint(r.right-bw, r.bottom-bh));
	}
	// back-Button
	if (m_iPage > 1)
	{
		DrawSmallButton("BTN_BACK", CPoint(r.right-bw, r.top));
	}
}

CPoint CShipBottomView::CalcSecondaryButtonTopLeft(short counter, bool top_down) const {
	const unsigned y = top_down ? bt + counter * (bh + bd)
		: bb - counter * (bh + bd) - bh;
	return CPoint(m_dc.r.right - (bw *2 + bd), y);
}

void CShipBottomView::SetupMainButtons() {
	m_vMainShipOrders.clear();
	const CRect& r = m_dc.r;
	unsigned top = r.top + bt;
	unsigned bottom = bt + bh;
	const unsigned left = r.right-bw;
	const unsigned shift = bh + bd;
	for(int i = 1; i <= MAIN_BUTTON_CANCEL; ++i) {
		m_vMainShipOrders.push_back(MainButtonInfo(static_cast<MAIN_BUTTON>(i), CRect(left, top, r.right, bottom)));
		top += shift;
		bottom += shift;
	}
	assert(m_vMainShipOrders.size() == MAIN_BUTTON_CANCEL);
}

void CShipBottomView::DrawMaincommandMenu() {
	for(std::vector<const MainButtonInfo>::const_iterator i = m_vMainShipOrders.begin();
			i != m_vMainShipOrders.end(); ++i)
	{
		if(m_iTimeCounter < i->which)
			continue;
		DrawSmallButton(i->String(), CPoint(i->rect.left, i->rect.top),
			i->which == MAIN_BUTTON_CANCEL ? SHIP_ORDER::CANCEL : SHIP_ORDER::NONE);
	}
	if (m_iTimeCounter >= MAIN_BUTTON_CANCEL && m_iWhichMainShipOrderButton == MAIN_BUTTON_NONE)
	{
		this->KillTimer(1);
		m_iTimeCounter = 0;
	}
}

bool CShipBottomView::TimeDoDraw(short counter) const {
	return m_iTimeCounter > (MAIN_BUTTON_CANCEL + counter);
}

short CShipBottomView::DrawCombatMenu() {
	const CShips& pShip = *m_dc.pDoc->CurrentShip()->second;
	short counter = 0;
	// angreifen
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::ATTACK))
	{
		DrawSmallButton("BTN_ATTACK", CalcSecondaryButtonTopLeft(counter), SHIP_ORDER::ATTACK);
		counter++;
	}
	// meiden
	else if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::AVOID))
	{
		DrawSmallButton("BTN_AVOID",CalcSecondaryButtonTopLeft(counter), SHIP_ORDER::AVOID);
		counter++;
	}
	return counter;
}

short CShipBottomView::DrawMultiTurnOrderMenu() {
	const CShips& pShip = *m_dc.pDoc->CurrentShip()->second;
	const CRect r(m_dc.r);
	short counter = 0;
	const CSystem& system = m_dc.pDoc->CurrentSystem();
	const CMajor* pMajor = m_pPlayersRace;

	const bool top_down = false;

	//maximum orders in this cathegory:
	//5; in an own system with training, a terraformable planet, a damaged ship

	// Wache
	if (TimeDoDraw(counter) && ShipCanHaveOrder(*m_dc.pDoc->CurrentShip()->second, SHIP_ORDER::SENTRY_SHIP_ORDER))
	{
		DrawSmallButton("BTN_SENTRY_SHIP_ORDER",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::SENTRY_SHIP_ORDER);
		counter++;
	}
	// trainieren
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::TRAIN_SHIP, &system))
	{
		DrawSmallButton("BTN_TRAIN_SHIP",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::TRAIN_SHIP);
		counter++;
	}
	// Systemangriff
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::ATTACK_SYSTEM))
	{
		// Wenn im Sektor ein Sonnensystem ist
		if (system.GetSunSystem() == TRUE &&
			// Wenn im System noch Bevölkerung vorhanden ist
			system.GetCurrentHabitants() > 0.0f &&
			// Wenn das System nicht der Rasse gehört, der auch das Schiff gehört
			m_dc.pDoc->CurrentSystem().GetOwnerOfSystem() != pShip.GetOwnerOfShip())
		{
			CRace* pOwnerOfSector = m_dc.pDoc->GetRaceCtrl()->GetRace(system.GetOwnerOfSector());

			// Wenn im System eine Rasse lebt und wir mit ihr im Krieg sind
			if (pOwnerOfSector != NULL && pMajor->GetAgreement(pOwnerOfSector->GetRaceID()) == DIPLOMATIC_AGREEMENT::WAR
			// Wenn das System niemanden mehr gehört, aber noch Bevölkerung drauf lebt (z.B. durch Rebellion)
				|| m_dc.pDoc->CurrentSystem().GetOwnerOfSystem() == "" && system.GetMinorRace() == FALSE)
			{
				// nur wenn die Schiffe ungetarnt sind können sie Bombardieren
				// Ab hier check wegen Flotten
				if (ShipCanHaveOrder(pShip, SHIP_ORDER::ATTACK_SYSTEM))
				{
					DrawSmallButton("BTN_ATTACK_SYSTEM",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::ATTACK_SYSTEM);
					counter++;
				}
			}
		}
	}
	// Systemblockade
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::BLOCKADE_SYSTEM))
	{
		// Überprüfen ob man eine Blockade im System überhaupt errichten kann
		// Wenn das System nicht der Rasse gehört, der auch das Schiff gehört
		CRace* pOwnerOfSystem = m_dc.pDoc->GetRaceCtrl()->GetRace(m_dc.pDoc->CurrentSystem().GetOwnerOfSystem());
		if (pOwnerOfSystem != NULL && pOwnerOfSystem->GetRaceID() != pShip.GetOwnerOfShip()
			&& pMajor->GetAgreement(pOwnerOfSystem->GetRaceID()) < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
		{
			DrawSmallButton("BTN_BLOCKADE_SYSTEM",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::BLOCKADE_SYSTEM);
			counter++;
		}
	}
	// Terraforming (hier beachten wenn es eine Flotte ist, dort schauen ob auch jedes Schiff in
	// der Flotte auch terraformen kann)
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::TERRAFORM))
	{
		for (int l = 0; l < system.GetNumberOfPlanets(); l++)
			if (system.GetPlanet(l)->GetHabitable() == TRUE &&
				system.GetPlanet(l)->GetTerraformed() == FALSE)
			{
				DrawSmallButton("BTN_TERRAFORM",CalcSecondaryButtonTopLeft(counter, top_down), SHIP_ORDER::TERRAFORM);
				counter++;
				break;
			}
	}
	// Außenposten/Sternbasis bauen (hier beachten wenn es eine Flotte ist, dort schauen ob auch jedes
	// Schiff in der Flotte Stationen bauen kann)
	// (müssen nur einen der Befehle (egal ob Outpost oder
	// Starbase gebaut werden soll) übergeben, weil wenn das eine geht, geht auch das andere
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::BUILD_OUTPOST))
	{
		CPoint ShipKO = m_dc.pDoc->GetKO();
		// hier schauen, ob ich in der Schiffsinfoliste schon einen Außenposten habe den ich bauen kann, wenn in dem
		// Sector noch kein Außenposten steht und ob ich diesen in dem Sector überhaupt bauen kann. Das geht nur
		// wenn der Sektor mir oder niemanden gehört
		if(system.IsStationBuildable(SHIP_ORDER::BUILD_OUTPOST, pShip.GetOwnerOfShip()))
		{
			// Hier überprüfen, ob ich einen Außenposten technologisch überhaupt bauen kann
			for (int l = 0; l < m_dc.pDoc->m_ShipInfoArray.GetSize(); l++)
				if(pMajor->CanBuildShip(SHIP_TYPE::OUTPOST, m_dc.researchLevels, m_dc.pDoc->m_ShipInfoArray.GetAt(l)))
				{
					// Wenn ja dann Schaltfläche zum Außenpostenbau einblenden
					DrawSmallButton("BTN_BUILD_OUTPOST",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::BUILD_OUTPOST);
					counter++;
					break;
				}
		}
		// Wenn hier schon ein Außenposten steht, können wir vielleicht auch eine Sternbasis bauen
		if (system.IsStationBuildable(SHIP_ORDER::BUILD_STARBASE, pShip.GetOwnerOfShip()))
		{
			// Hier überprüfen, ob ich eine Sternbasis technologisch überhaupt bauen kann
			for (int l = 0; l < m_dc.pDoc->m_ShipInfoArray.GetSize(); l++)
				if(pMajor->CanBuildShip(SHIP_TYPE::STARBASE, m_dc.researchLevels, m_dc.pDoc->m_ShipInfoArray.GetAt(l)))
				{
					// Wenn ja dann Schaltfläche zum Sternenbasisbau einblenden
					DrawSmallButton("BTN_BUILD_STARBASE",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::BUILD_STARBASE);
					counter++;
					break;
				}
		}
		// If a better outpost is available and buildable in this sector draw the upgrade button
		if (system.IsStationBuildable(SHIP_ORDER::UPGRADE_OUTPOST, pShip.GetOwnerOfShip())) {
			DrawSmallButton("BTN_UPGRADE_OUTPOST",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::UPGRADE_OUTPOST);
			counter++;
		}
		// If a better starbase is available and buildable in this sector draw the upgrade button
		if (system.IsStationBuildable(SHIP_ORDER::UPGRADE_STARBASE, pShip.GetOwnerOfShip())) {
			DrawSmallButton("BTN_UPGRADE_STARBASE",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::UPGRADE_STARBASE);
			counter++;
		}
	}
	// shield improvement
	// Only possible if we are at an ionstorm and we haven't yet reached the max max shields
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::IMPROVE_SHIELDS,
			&m_dc.pDoc->GetSystem(pShip.GetKO().x, pShip.GetKO().y)))
	{
		DrawSmallButton("IMPROVE_SHIELDS_SHIP_ORDER",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::IMPROVE_SHIELDS);
		counter++;
	}
	// Repairing
	// Only possible if
	// 1) the ship or any of the ships in its fleet are actually damaged.
	// 2) we (or an allied race) have a ship port in this sector.
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::REPAIR,
			&m_dc.pDoc->GetSystem(
				pShip.GetKO().x,
				pShip.GetKO().y
			)
		)
	)
	{
		DrawSmallButton("BTN_REPAIR_SHIP",CalcSecondaryButtonTopLeft(counter, top_down),SHIP_ORDER::REPAIR);
		counter++;
	}
	return counter;
}

short CShipBottomView::DrawSingleTurnOrderMenu() {
	const CShips& pShip = *m_dc.pDoc->CurrentShip()->second;
	const CRect r(m_dc.r);
	const CSystem& system = m_dc.pDoc->CurrentSystem();
	short counter = 0;

	//maximum orders in this category: 5; in an own system with a colonizable planet, a coloship, a cloakable ship
	// Warten
	if (TimeDoDraw(counter) && ShipCanHaveOrder(*m_dc.pDoc->CurrentShip()->second, SHIP_ORDER::WAIT_SHIP_ORDER))
	{
		DrawSmallButton("BTN_WAIT_SHIP_ORDER",CalcSecondaryButtonTopLeft(counter),SHIP_ORDER::WAIT_SHIP_ORDER);
		counter++;
	}
	// Schiff abwracken/zerstören
	if (TimeDoDraw(counter) && ShipCanHaveOrder(*m_dc.pDoc->CurrentShip()->second, SHIP_ORDER::DESTROY_SHIP))
	{
		DrawSmallButton("BTN_DESTROY_SHIP",CalcSecondaryButtonTopLeft(counter),SHIP_ORDER::DESTROY_SHIP);
		counter++;
	}
	// tarnen (hier beachten wenn es eine Flotte ist, dort schauen ob sich jedes Schiff in der Flotte auch
	// tarnen kann)
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::ENCLOAK))
	{
		DrawSmallButton("BTN_CLOAK",CalcSecondaryButtonTopLeft(counter),SHIP_ORDER::ENCLOAK);
		counter++;
	}
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::DECLOAK))
	{
		DrawSmallButton("BTN_DECLOAK",CalcSecondaryButtonTopLeft(counter),SHIP_ORDER::DECLOAK);
		counter++;
	}
	// Kolonisierung (hier beachten wenn es eine Flotte ist, dort schauen ob auch jedes Schiff in
	// der Flotte auch kolonisieren kann)
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::COLONIZE))
	{
		// Wenn das System uns bzw. niemanden gehört können wir nur kolonisieren
		if (system.GetOwnerOfSector() == "" || system.GetOwnerOfSector() == pShip.GetOwnerOfShip())
			for (int l = 0; l < system.GetNumberOfPlanets(); l++)
				if (system.GetPlanet(l)->GetTerraformed() == TRUE
					&& system.GetPlanet(l)->GetCurrentHabitant() == 0.0f)
				{
					DrawSmallButton("BTN_COLONIZE", CalcSecondaryButtonTopLeft(counter), SHIP_ORDER::COLONIZE);
					counter++;
					break;
				}
	}
	return counter;
}

short CShipBottomView::DrawImmediateOrderMenu() {

	short counter = 0;
	const CShips& pShip = *m_dc.pDoc->CurrentShip()->second;
	const CRect r(m_dc.r);

	// gruppieren
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::CREATE_FLEET))
	{
		DrawSmallButton("BTN_CREATE_FLEET",CalcSecondaryButtonTopLeft(counter),SHIP_ORDER::CREATE_FLEET);
		counter++;
	}
	// Transport
	if (TimeDoDraw(counter) && ShipCanHaveOrder(pShip, SHIP_ORDER::TRANSPORT))
	{
		DrawSmallButton("BTN_TRANSPORT", CalcSecondaryButtonTopLeft(counter),SHIP_ORDER::TRANSPORT);
		counter++;
	}
	// Flagschiffernennung, geht nur wenn es keine Flotte ist
	if (TimeDoDraw(counter) && ShipCanHaveOrder(*m_dc.pDoc->CurrentShip()->second, SHIP_ORDER::ASSIGN_FLAGSHIP))
	{
		DrawSmallButton("BTN_ASSIGN_FLAGSHIP",CalcSecondaryButtonTopLeft(counter),SHIP_ORDER::ASSIGN_FLAGSHIP);
		counter++;
	}
	return counter;
}

void CShipBottomView::DrawStationData() {
	CMajor* pMajor = m_pPlayersRace;
	const CSystem& system = m_dc.pDoc->CurrentSystem();
	BYTE count = 0;

	// Wenn wir in dem Sektor gerade einen Außenposten bauen, dann prozentualen Fortschritt anzeigen.
	// Es kann auch passieren, das mehrere Rassen gleichzeitig dort einen Außenposten bauen, dann müssen wir
	// von jeder der Rasse den Fortschritt beim Stationsbau angeben
	map<CString, CMajor*>* pmMajors = m_dc.pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (system.GetIsStationBuilding(it->first) == TRUE) {
			m_dc.fontBrush->SetColor(Color(170,170,170));
			CString percent;
			percent.Format("%d",((system.GetStartStationPoints(it->first)
				- system.GetNeededStationPoints(it->first)) * 100
				/ system.GetStartStationPoints(it->first)));

			CString sRaceName;
			if (pMajor == it->second || pMajor->IsRaceContacted(it->first))
				sRaceName = it->second->GetRaceName();
			else
				sRaceName = CLoc::GetString("UNKNOWN");

			CString station;
			// If an outpost is under construction
			const SHIP_ORDER::Typ type = system.StationWork(it->first);
			if (type == SHIP_ORDER::BUILD_OUTPOST)
				station = CLoc::GetString("OUTPOST") + CLoc::GetString("STATION_BUILDING", FALSE, sRaceName, percent);
			// If a starbase is under construction
			else if (type == SHIP_ORDER::BUILD_STARBASE)
				station = CLoc::GetString("STARBASE") + CLoc::GetString("STATION_BUILDING", FALSE, sRaceName, percent);
			// If an outpost gets upgraded
			else if (type == SHIP_ORDER::UPGRADE_OUTPOST)
				station = CLoc::GetString("OUTPOST") + CLoc::GetString("STATION_UPGRADING", FALSE, sRaceName, percent);
			// If a starbase gets upgraded
			else if (type == SHIP_ORDER::UPGRADE_STARBASE)
				station = CLoc::GetString("STARBASE") + CLoc::GetString("STATION_UPGRADING", FALSE, sRaceName, percent);
			m_dc.fontFormat.SetAlignment(StringAlignmentCenter);
			m_dc.fontFormat.SetLineAlignment(StringAlignmentCenter);
			m_dc.g->DrawString(CComBSTR(station), -1, &Gdiplus::Font(CComBSTR(m_dc.fontName), m_dc.fontSize), PointF(250, 30+count*25), &m_dc.fontFormat, m_dc.fontBrush);
			count++;
		}
}

void CShipBottomView::DrawMenu() {
	CMajor* pMajor = m_pPlayersRace;

	CResearch *res = pMajor->GetEmpire()->GetResearch();
	m_dc.researchLevels[0] = res->GetBioTech();
	m_dc.researchLevels[1] = res->GetEnergyTech();
	m_dc.researchLevels[2] = res->GetCompTech();
	m_dc.researchLevels[3] = res->GetPropulsionTech();
	m_dc.researchLevels[4] = res->GetConstructionTech();
	m_dc.researchLevels[5] = res->GetWeaponTech();

	CRect rect;

	// Alle Rechtecke für die Buttons der Schiffsbefehle erstmal auf NULL setzen, damit wir nicht draufklicken
	// können. Wir dürfen ja nur auf Buttons klicken können, die wir auch sehen
	for (unsigned j = 0; j < m_vSecondaryShipOrders.size(); j++)
		m_vSecondaryShipOrders.at(j).rect.SetRect(0,0,0,0);

	DrawMaincommandMenu();

	short counter = 0;
	if( m_iWhichMainShipOrderButton == MAIN_BUTTON_COMBAT_BEHAVIOR )
		counter = DrawCombatMenu();
	else if( m_iWhichMainShipOrderButton == MAIN_BUTTON_IMMEDIATE_ORDER )
		counter = DrawImmediateOrderMenu();
	else if( m_iWhichMainShipOrderButton == MAIN_BUTTON_SINGLE_TURN_ORDER)
		counter = DrawSingleTurnOrderMenu();
	if( m_iWhichMainShipOrderButton == MAIN_BUTTON_MULTI_TURN_ORDER )
		counter = DrawMultiTurnOrderMenu();

	if (TimeDoDraw(counter))
	{
		this->KillTimer(1);
		m_iTimeCounter = 0;
	}
}

void CShipBottomView::OnDraw(CDC* dc)
{
	m_dc.pDoc = resources::pDoc;
	ASSERT(m_dc.pDoc);

	if (!m_dc.pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	if (!CGalaxyMenuView::IsMoveShip())
	{
		KillTimer(1);
		m_iTimeCounter = 0;
	}

	// Update Graphicpool Link
	m_dc.gp = m_dc.pDoc->GetGraphicPool();


	// TODO: add draw code here

	// Doublebuffering wird initialisiert
	CMyMemDC pDC(dc);
	// Graphicsobjekt, in welches gezeichnet wird anlegen
	m_dc.g = new Graphics(pDC.GetSafeHdc());

	SetupDrawing();

	// Galaxie im Hintergrund zeichnen
	DrawImage("Backgrounds\\" + pMajor->GetPrefix() + "galaxyV3.bop", CRect(CPoint(0,0),CSize(1075,249)));

	// Bis jetzt nur eine Anzeige bis max. 9 Schiffe
	if (m_iTimeCounter == 0)
	{
		DrawShipContent();
	}

	// Die ganzen Befehlsbuttons für die Schiffe anzeigen
	if (CGalaxyMenuView::IsMoveShip() == TRUE && m_dc.pDoc->CurrentShip()->second->GetOwnerOfShip() == pMajor->GetRaceID())
	{
		DrawMenu();
	}

	// Stationsansicht
	if (m_bShowStation)
	{
		DrawStationData();
	}

	//TODO: instead of the magic number 10, what should be here ?
	//probably something like MAIN_BUTTON_CANCEL + SHIP_ORDER::CANCEL ? (maximum values from those enums)
	// Wenn wir dem Schiff einen neuen Befehl geben, ohne das die Buttons über den Timer vollständig gezeichnet wurden
	// verschwindet das Schiff in der Anzeige, da der TimeCounter nicht wieder auf NULL gesetzt wird. Dies machen
	// wir hier manuell, falls dieser Fall eintritt
	if (m_iTimeCounter > 10)
	{
		KillTimer(1);
		m_iTimeCounter = 0;
	}

	m_dc.g->ReleaseHDC(pDC->GetSafeHdc());

	delete m_dc.fontBrush;
	delete m_dc.g;
	m_dc.pDoc = NULL;
}

// CShipBottomView diagnostics

#ifdef _DEBUG
void CShipBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CShipBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CShipBottomView message handlers

void CShipBottomView::OnInitialUpdate()
{
	CBottomBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	m_LastKO = pDoc->GetKO();
	m_iPage = 1;
	m_iTimeCounter = 0;
	m_bShowNextButton = FALSE;
	m_iWhichMainShipOrderButton = MAIN_BUTTON_NONE;
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CShipBottomView::LoadRaceGraphics()
{
	ASSERT((CBotEDoc*)GetDocument());

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	CString sPrefix = pPlayer->GetPrefix();
	CString s = CIOData::GetInstance()->GetAppPath() + "Graphics\\Other\\" + sPrefix + "button_shiporder.bop";
	m_pShipOrderButton = Bitmap::FromFile(CComBSTR(s));
}

BOOL CShipBottomView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

void CShipBottomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	assert(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	const CMajor* const pMajor = m_pPlayersRace;
	assert(pMajor);

	CalcLogicalPoint(point);

	//////////////////////////////////////////////////////////////////////////
	// Klick auf Schiff prüfen

	// wurde die Maus über ein Schiff gehalten
	for (std::vector<std::pair<CRect, CShips*>>::const_iterator i = m_vShipRects.begin(); i != m_vShipRects.end(); ++i)
	{
		if (!i->first.PtInRect(point))
			continue;

		// Wurde das aktuelle Schiff angeklickt?
		if (pDoc->CurrentShip()->second == i->second)
		{
			// Nicht machen, wenn es nicht unser eigenes Schiff ist
			if (pDoc->CurrentShip()->second->GetOwnerOfShip() != pMajor->GetRaceID())
				return;

			// Wenn wir in der MainView nicht im Flottenmenü sind
			if (resources::pMainFrame->GetActiveView(0, 1) != VIEWS::FLEET_VIEW)
			{
				ActivateShip(pDoc->CurrentShip());
				return;
			}

			// Wenn wir in der MainView im Flottenmenü sind

			// Dann stecken das angeklickte Schiff in die gerade angezeigte Flotte stecken
			// Fremde Flotten können nicht bearbeitet werden
			const CShipMap::iterator& fleetship = pDoc->FleetShip();
			//double-clicking a foreign fleet gets us into the fleet view for that fleet
			//clicking an own ship to add it then must be aborted here
			if(fleetship->second->GetOwnerOfShip() != pMajor->GetRaceID())
				return;
			const CShipMap::iterator& current_ship = pDoc->CurrentShip();
			// Jetzt fügen wir der Flotte das angeklickte Schiff hinzu, wenn es nicht das Schiff selbst ist,
			// welches die Flotte anführt
			if (fleetship == current_ship)
				return;

			assert(fleetship->second->GetKO() == current_ship->second->GetKO());
			// sicherheitshalber wird hier nochmal überprüft, dass keine Station hinzugefügt werden kann und
			// das sich das Schiff auch im gleichen Sektor befindet
			if (!current_ship->second->IsStation())
			{
				// Wenn das Schiff welches wir hinzufügen wollen selbst eine Flotte besizt, so müssen
				// wir diese Flotte natürlich auch noch hinzufügen
				//this is done in CShips::AddShipToFleet
				fleetship->second->AddShipToFleet(current_ship->second);
				// Wenn wir das Schiff da hinzugefügt haben, dann müssen wir das aus der normalen Schiffsliste
				// rausnehmen, damit es nicht zweimal im Spiel vorkommt
				CShipMap::iterator to_erase = current_ship;
				//set the current ship to the next ship following the removed one of the same race in the
				//same sector which must not be a station, or to the fleetship in case there's none
				CShipMap::iterator next_current_ship = fleetship;
				CShipMap::iterator it = current_ship;
				++it;
				while(it != pDoc->m_ShipMap.end())
				{
					if(fleetship->second->GetKO() != it->second->GetKO()
							|| fleetship->second->GetOwnerOfShip() != it->second->GetOwnerOfShip()
							|| it->second->IsStation())
					{
						++it;
						continue;
					}

					next_current_ship = it;
					break;
				}

				const bool was_terraform = to_erase->second->GetCurrentOrder() == SHIP_ORDER::TERRAFORM;
				assert(next_current_ship != to_erase);
				pDoc->m_ShipMap.EraseAt(to_erase, false);
				pDoc->SetCurrentShip(next_current_ship);
				const CPoint& co = next_current_ship->second->GetKO();
				if (was_terraform)
					pDoc->GetSector(co.x, co.y).RecalcPlanetsTerraformingStatus();

				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CFleetMenuView));
				Invalidate(FALSE);
			}

			// fertig
			return;
		}
		// Schiff als aktuelles Schiff setzen
		else
		{
			// Iterator des aktuellen Schiffes in Schiffsmap suchen
			for (CShipMap::iterator j = pDoc->m_ShipMap.begin(); j != pDoc->m_ShipMap.end(); ++j)
			{
				if (j->second != i->second)
					continue;

				// Schiff aktivieren
				ActivateShip(j);
			}

			return;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Klick auf Buttons prüfen

	// Überprüfen ob wir auf den "Next"-Button geklickt haben, falls wir mehr als 9 Schiffe in dem Sektor haben
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	if (m_bShowNextButton == TRUE)
	{
		CRect next;
		next.SetRect(r.right-bw,r.bottom-bh,r.right,r.bottom);
		if (next.PtInRect(point))
		{
			CGalaxyMenuView::SetMoveShip(FALSE);
			resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			m_iPage++;
			Invalidate(FALSE);
		}
	}
	if (m_iPage > 1)	// haben wir auf den "back-button" geklickt
	{
		CRect back;
		back.SetRect(r.right-bw,r.top,r.right,r.top+bh);
		if (back.PtInRect(point))
		{
			CGalaxyMenuView::SetMoveShip(FALSE);
			resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			m_iPage--;
			Invalidate(FALSE);
		}
	}
	// Überprüfen, auf welchen Schiffsoberbefehle-Button ich geklickt habe
	if (CGalaxyMenuView::IsMoveShip() == TRUE)
	{
		for(std::vector<const MainButtonInfo>::const_iterator i = m_vMainShipOrders.begin(); i != m_vMainShipOrders.end(); ++i)
		{
			if (!i->rect.PtInRect(point))
				continue;

			if (i->which != MAIN_BUTTON_CANCEL)
			{
				m_iWhichMainShipOrderButton = i->which;
				m_iTimeCounter = MAIN_BUTTON_CANCEL;
				SetTimer(1,100,NULL);
				return;
			}
		}

		// Ab jetzt die kleinen Buttons für die einzelnen genauen Schiffsbefehle
		for(std::vector<SecondaryButtonInfo>::const_iterator i = m_vSecondaryShipOrders.begin(); i != m_vSecondaryShipOrders.end(); ++i)
			if (i->rect.PtInRect(point))
			{
				SHIP_ORDER::Typ nOrder = static_cast<SHIP_ORDER::Typ>(i - m_vSecondaryShipOrders.begin());
				// Bei manchen Befehlen müssen wir einen möglichen Zielkurs wieder zurücknehmen.
				if (nOrder != SHIP_ORDER::NONE && nOrder != SHIP_ORDER::AVOID && nOrder != SHIP_ORDER::ATTACK && nOrder != SHIP_ORDER::ENCLOAK && nOrder != SHIP_ORDER::DECLOAK && nOrder != SHIP_ORDER::ASSIGN_FLAGSHIP && nOrder != SHIP_ORDER::CREATE_FLEET && nOrder != SHIP_ORDER::TRANSPORT)
				{
					pDoc->CurrentShip()->second->SetTargetKO(CPoint(-1, -1));
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CGalaxyMenuView));
				}

				// Wenn wir eine Flotte bilden wollen (Schiffe gruppieren), dann in der MainView die Flottenansicht zeigen
				if (nOrder == SHIP_ORDER::CREATE_FLEET)
				{
					pDoc->SetFleetShip(pDoc->CurrentShip()); // Dieses Schiff soll die Flotte beinhalten
					resources::pMainFrame->SelectMainView(VIEWS::FLEET_VIEW, pMajor->GetRaceID());		// Flottenansicht in der MainView anzeigen
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				else if (nOrder == SHIP_ORDER::ASSIGN_FLAGSHIP)
				{
					const CShipMap::const_iterator& current_ship = pDoc->CurrentShip();
					assert(!current_ship->second->HasFleet());
					// Das ganze Schiffsarray und auch die Flotten durchgehen, wenn wir ein altes Flagschiff						//finden, diesem den Titel wegnehmen
					for(CShipMap::iterator n = pDoc->m_ShipMap.begin(); n != pDoc->m_ShipMap.end(); ++n)
					{
						if (n->second->GetOwnerOfShip() != current_ship->second->GetOwnerOfShip())
							continue;
						n->second->UnassignFlagship();
					}
					// Jetzt das neue Schiff zum Flagschiff ernennen
					current_ship->second->SetIsShipFlagShip(TRUE);
				}
				// Bei einem Transportbefehl muss in der MainView auch die Transportansicht angeblendet werden
				else if (nOrder == SHIP_ORDER::TRANSPORT)
				{
					resources::pMainFrame->SelectMainView(VIEWS::TRANSPORT_VIEW, pMajor->GetRaceID());	// Transportansicht in der MainView anzeigen
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				else if (nOrder == SHIP_ORDER::TERRAFORM) {
					//command is given when clicked on planet
				}
				else if (nOrder == SHIP_ORDER::CANCEL) {
					pDoc->CurrentShip()->second->UnsetCurrentOrder();
					pDoc->CurrentShip()->second->SetIsShipFlagShip(false);
				}
				else if (nOrder == SHIP_ORDER::ATTACK)
					pDoc->CurrentShip()->second->SetCombatTactic(COMBAT_TACTIC::CT_ATTACK);
				else if (nOrder == SHIP_ORDER::AVOID)
					pDoc->CurrentShip()->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
				// ansonsten ganz normal den Befehl geben
				else
					pDoc->CurrentShip()->second->SetCurrentOrder(nOrder);
				// bei Terraforming wird die Planetenansicht eingeblendet
				if (nOrder == SHIP_ORDER::TERRAFORM)
				{
					resources::pMainFrame->SelectBottomView(VIEWS::PLANET_BOTTOM_VIEW);
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
					resources::pClientWorker->PlaySound(SNDMGR_MSG_TERRAFORM_SELECT, pMajor->GetRaceID());
				}
				else
				{
					if (nOrder == SHIP_ORDER::COLONIZE)
						resources::pClientWorker->PlaySound(SNDMGR_MSG_COLONIZING, pMajor->GetRaceID());
					else if (nOrder == SHIP_ORDER::BUILD_OUTPOST || nOrder == SHIP_ORDER::UPGRADE_OUTPOST)
						resources::pClientWorker->PlaySound(SNDMGR_MSG_OUTPOST_CONSTRUCT, pMajor->GetRaceID());
					else if (nOrder == SHIP_ORDER::BUILD_STARBASE || nOrder == SHIP_ORDER::UPGRADE_STARBASE)
						resources::pClientWorker->PlaySound(SNDMGR_MSG_STARBASE_CONSTRUCT, pMajor->GetRaceID());

					CGalaxyMenuView::SetMoveShip(FALSE);
					CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
				}

				Invalidate(FALSE);
				break;
			}
	}

	CBottomBaseView::OnLButtonDown(nFlags, point);
}

void CShipBottomView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	// keinen Doppelklick zulassen wenn in der Flottenansicht
	if (resources::pMainFrame->GetActiveView(0, 1) == VIEWS::FLEET_VIEW)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

	// wurde die Maus über ein Schiff gehalten
	for (std::vector<std::pair<CRect, CShips*>>::const_iterator i = m_vShipRects.begin(); i != m_vShipRects.end(); ++i)
	{
		if (!i->first.PtInRect(point))
			continue;

		// handelt es sich um eine Station, dann kein Flottenmenü anzeigen
		if (i->second->IsStation())
			return;

		// handelt es sich nur um ein nicht eigenes Schiff (keine Flotte), dann ebenfalls kein Flottenmenü anzeigen
		if (i->second->GetFleetSize() == 0 && i->second->GetOwnerOfShip() != pMajor->GetRaceID())
			return;

		CGalaxyMenuView::SetMoveShip(FALSE);
		this->KillTimer(1);
		m_iTimeCounter = 0;
		pDoc->SetFleetShip(pDoc->CurrentShip()); // Dieses Schiff soll die Flotte beinhalten
		resources::pMainFrame->SelectMainView(VIEWS::FLEET_VIEW, pMajor->GetRaceID());	// Flottenansicht in der MainView anzeigen
		resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
		Invalidate(FALSE);
		return;
	}

	CBottomBaseView::OnLButtonDblClk(nFlags, point);
}

void CShipBottomView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (CGalaxyMenuView::IsMoveShip())
		return;

	CBotEDoc* pDoc = resources::pDoc;
	assert(pDoc);
	CalcLogicalPoint(point);

	// wurde die Maus über ein Schiff gehalten
	for (std::vector<std::pair<CRect, CShips*>>::const_iterator i = m_vShipRects.begin(); i != m_vShipRects.end(); ++i)
	{
		if (!i->first.PtInRect(point))
			continue;

		// handelt es sich um das aktuelle Schiff, dann braucht nichts mehr gemacht werden
		if (i->second == pDoc->CurrentShip()->second)
			return;

		// Iterator des aktuellen Schiffes in Schiffsmap suchen
		for (CShipMap::iterator j = pDoc->m_ShipMap.begin(); j != pDoc->m_ShipMap.end(); ++j)
		{
			if (j->second != i->second)
				continue;

			ActivateShip(j, false);
			Invalidate(FALSE);
			return;
		}
	}

	CBottomBaseView::OnMouseMove(nFlags, point);
}

void CShipBottomView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	if (CGalaxyMenuView::IsMoveShip())
	{
		CGalaxyMenuView::SetMoveShip(FALSE);
		this->KillTimer(1);
		m_iTimeCounter = 0;
		if (resources::pMainFrame->GetActiveView(1, 1) == VIEWS::PLANET_BOTTOM_VIEW)	// Wenn wir kolon oder terraformen abbrechen wollen, zurück zum Schiffsmenü
		{
			pDoc->CurrentShip()->second->UnsetCurrentOrder();
			pDoc->CurrentShip()->second->SetCombatTactic(COMBAT_TACTIC::CT_AVOID);
			m_bShowStation = false;
			Invalidate(FALSE);
		}
		else // wenn wir schon im Schiffsmenü sind, dann brauchen wir nur die Befehlsbutton neu zeichnen
		{
			Invalidate(FALSE);
			resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CGalaxyMenuView));
		}

		CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
		resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
	}

	CBottomBaseView::OnRButtonDown(nFlags, point);
}


void CShipBottomView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_iTimeCounter++;
	CRect r(830,20,m_TotalSize.cx, m_TotalSize.cy);
	CalcDeviceRect(r);
	InvalidateRect(r, FALSE);

	CBottomBaseView::OnTimer(nIDEvent);
}

void CShipBottomView::ActivateShip(CShipMap::iterator i, bool bSetShipMove /* = true */)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	pDoc->SetCurrentShip(i);
	m_iWhichMainShipOrderButton = MAIN_BUTTON_NONE;

	if (bSetShipMove)
	{
		KillTimer(1);
		m_iTimeCounter = 0;
		CGalaxyMenuView::SetMoveShip(TRUE);
		this->SetTimer(1,100,NULL);
		resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CGalaxyMenuView));
	}

	CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
	resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CShipBottomView::CreateTooltip(void)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return "";

	CShips* pShip = NULL;
	for (UINT i = 0; i < m_vShipRects.size(); i++)
		if (m_vShipRects[i].first.PtInRect(pt))
		{
			pShip = m_vShipRects[i].second;
			break;
		}

	if (!pShip)
		return "";

	// ist der Besitzer des Schiffes unbekannt?
	if (pMajor->GetRaceID() != pShip->GetOwnerOfShip())
	{
		bool bNoDiplomacy = false;
		CRace* pShipOwner = pDoc->GetRaceCtrl()->GetRace(pShip->GetOwnerOfShip());
		if (pShipOwner)
			bNoDiplomacy = pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY);

		if (!bNoDiplomacy && pMajor->IsRaceContacted(pShip->GetOwnerOfShip()) == false)
		{
			CString s = CLoc::GetString("UNKNOWN");
			s = CHTMLStringBuilder::GetHTMLColor(s);
			s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
			s = CHTMLStringBuilder::GetHTMLCenter(s);
			return s;
		}
	}

	return pShip->GetTooltip();
}

/// Funktion ermittelt die Nummer des Schiffes im Array, über welches die Maus bewegt wurde.
/// @param pt Mauskoordinate
/// @return Nummer des Schiffes im Schiffsarray (<code>-1</code> wenn kein Schiff markiert wurde)
//int CShipBottomView::GetMouseOverShip(CPoint& pt)
//{
//	// TODO: Add your message handler code here and/or call default
//	const CBotEDoc* const pDoc = resources::pDoc;
//	ASSERT(pDoc);
//
//	if (!pDoc->m_bDataReceived)
//		return -1;
//
//	const CMajor* const pMajor = m_pPlayersRace;
//	ASSERT(pMajor);
//	if (!pMajor)
//		return -1;
//
//	m_RectForTheShip.SetRect(0,0,0,0);
//
//	if (CGalaxyMenuView::IsMoveShip() == FALSE) {
//		USHORT counter = 0;
//		USHORT row = 0;
//		USHORT column = 0;
//		for (int i = 0; i < pDoc->m_ShipMap.GetSize(); i++) {
//			const CShip* const pShip = &pDoc->m_ShipMap.GetAt(i);
//			const CPoint& active_sector = pDoc->GetKO();
//			if(active_sector != pShip->GetKO())
//				continue;
//			const BOOL is_base = pShip->IsStation();
//			if (m_bShowStation != is_base)
//				continue;
//			// Schiffe mit zu guter Stealthpower werden hier nicht angezeigt.
//			USHORT stealthPower = MAXBYTE;
//			const CFleet* pFleet = pShip->GetFleet();
//			if (!pFleet)
//			{
//				stealthPower = pShip->GetStealthPower() * 20;
//				if (pShip->GetStealthPower() > 3 && pShip->GetCloak() == FALSE)
//					stealthPower = 3 * 20;
//			}
//			else
//				stealthPower = pFleet->GetFleetStealthPower(pShip);
//
//			const CString& sRaceID = pMajor->GetRaceID();
//			if (pShip->GetOwnerOfShip() != sRaceID
//				&& pDoc->GetSector(active_sector.x, active_sector.y).GetScanPower(sRaceID) <= stealthPower)
//				continue;
//			if (counter < m_iPage*9 && counter >= (m_iPage-1)*9)
//			{
//				// mehrere Spalten anlegen, falls mehr als 3 Schiffe in dem System sind
//				if (counter != 0 && counter%3 == 0)
//				{
//					column++;
//					row = 0;
//				}
//				if (counter%9 == 0)
//					column = 0;
//				m_RectForTheShip.SetRect(column*250+37,row*65+30,column*250+187,row*65+80);
//				if (m_RectForTheShip.PtInRect(pt))
//					// Maus wurde über ein Schiff bewegt -> Nummer zurückgeben
//					return i;
//			}
//			row++;
//			counter++;
//			if (counter > m_iPage*9)
//				break;
//		}
//	}
//
//	// kein Schiff markiert
//	return -1;
//}
