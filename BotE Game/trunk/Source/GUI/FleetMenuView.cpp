// FleetMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "GalaxyMenuView.h"
#include "FleetMenuView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "Graphic\memdc.h"
#include "Ships/Ships.h"

#include <cassert>

// CFleetMenuView
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CFleetMenuView, CMainBaseView)

CFleetMenuView::CFleetMenuView() :
	bg_fleetmenu(),
	m_iFleetPage(1),
	m_bShowNextButton(FALSE),
	m_bShowBackButton(FALSE),
	m_pMarkedShip()
{
}

CFleetMenuView::~CFleetMenuView()
{
}

BEGIN_MESSAGE_MAP(CFleetMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CFleetMenuView::OnNewRound()
{
	m_iFleetPage = 1;
	m_bShowNextButton = FALSE;
	m_bShowBackButton = FALSE;

	m_vShipRects.clear();
	m_pMarkedShip = NULL;
	m_rLastMarkedRect = CRect(0,0,0,0);
}
// CFleetMenuView drawing

void CFleetMenuView::OnDraw(CDC* dc)
{
	// TODO: add draw code here
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	// Doublebuffering wird initialisiert
	CMyMemDC pDC(dc);
	CRect client;
	GetClientRect(&client);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());

	g.Clear(Color::Black);
	g.SetSmoothingMode(SmoothingModeHighQuality);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);
	// ***************************** DIE FLOTTENBILDUNGSANSICHT ZEICHNEN **********************************
	DrawFleetMenue(&g);
	// ************** DIE FLOTTENANSICHT ZEICHNEN ist hier zu Ende **************
	g.ReleaseHDC(pDC->GetSafeHdc());
}


// CFleetMenuView diagnostics

#ifdef _DEBUG
void CFleetMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFleetMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFleetMenuView message handlers

void CFleetMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	// Flottenansicht
	m_iFleetPage = 1;
	m_bShowNextButton = FALSE;
	m_bShowBackButton = FALSE;

	// View bei den Tooltipps anmelden
	pDoc->GetMainFrame()->AddToTooltip(this);
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CFleetMenuView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CreateButtons();

	// alle Hintergrundgrafiken laden
	CString sPrefix = pMajor->GetPrefix();

	bg_fleetmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "fleetmenu.boj");
}

void CFleetMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CFleetMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Flottenmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CFleetMenuView::DrawFleetMenue(Graphics* g)
{
	m_vShipRects.clear();

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);

	Gdiplus::Font font(CComBSTR(fontName), fontSize);

	if (bg_fleetmenu)
		g->DrawImage(bg_fleetmenu, 0, 0, 1075, 750);

	// Hier die Buttons einzeigen, mit denen wir alle Schiffe im Sektor, Schiffe der gleichen Klasse oder
	// Schiffe des gleichen Types hinzufügen bzw. entfernen können
	fontBrush.SetColor(penColor);
	CString s = CResourceManager::GetString("WHAT_SHIPS_TO_FLEET");
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(CComBSTR(s), -1, &font, RectF(20,140,210,75), &fontFormat, &fontBrush);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontBrush.SetColor(normalColor);

	const CShipArray::iterator& pShip = pDoc->FleetShip();
	bool bUnknown = (pMajor->GetRaceID() != pShip->second.GetOwnerOfShip() && pMajor->IsRaceContacted(pShip->second.GetOwnerOfShip()) == false);
	if (bUnknown)
	{
		// Wenn kein diplomatischer Kontakt möglich ist, wird das Schiff immer angezeigt
		CRace* pShipOwner = pDoc->GetRaceCtrl()->GetRace(pShip->second.GetOwnerOfShip());
		if (pShipOwner)
			bUnknown = !pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY);
	}

	if (!bUnknown)
	{
		// der gleichen Klasse hinzufügen
		s.Format("%s-%s", pShip->second.GetShipClass(),CResourceManager::GetString("CLASS"));
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,220,250,30), &fontFormat, &fontBrush);
		// des gleichen Types hinzufügen
		s.Format("%s %s",CResourceManager::GetString("TYPE"), pShip->second.GetShipTypeAsString());
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,270,250,30), &fontFormat, &fontBrush);
		// alle Schiffe hinzufügen
		s = CResourceManager::GetString("ALL_SHIPS");
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,320,250,30), &fontFormat, &fontBrush);
	}

	fontBrush.SetColor(penColor);
	s = CResourceManager::GetString("WHAT_SHIPS_FROM_FLEET");
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(CComBSTR(s), -1, &font, RectF(20,400,210,75), &fontFormat, &fontBrush);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontBrush.SetColor(normalColor);

	if (!bUnknown)
	{
		// fremder Klassen entfernen
		s.Format("%s %s-%s",CResourceManager::GetString("NOT"),	pShip->second.GetShipClass(),CResourceManager::GetString("CLASS"));
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,480,250,30), &fontFormat, &fontBrush);
		// fremden Types entfernen
		s.Format("%s %s %s",CResourceManager::GetString("NOT"),CResourceManager::GetString("TYPE"),	pShip->second.GetShipTypeAsString());
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,530,250,30), &fontFormat, &fontBrush);
		// alle Schiffe entfernen
		s = CResourceManager::GetString("ALL_SHIPS");
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,580,250,30), &fontFormat, &fontBrush);
	}


	USHORT counter = 0;
	USHORT column = 1;
	USHORT row = 0;
	// Erstmal das Schiff anzeigen, welches die Flotte beinhaltet (nur auf erster Seite!)
	if (m_iFleetPage == 1)
	{
		//bool bMarked = (pDoc->GetNumberOfTheShipInFleet() == 0);
		bool bMarked = &pShip->second == m_pMarkedShip;
		CPoint pt(250 * column, 65 * row + 60);
		pShip->second.DrawShip(g, pDoc->GetGraphicPool(), pt, bMarked, bUnknown, FALSE, normalColor, markColor, font);
		m_vShipRects.push_back(pair<CRect, CShips*>(CRect(pt.x, pt.y + 20, pt.x + 250, pt.y + 85), &pShip->second));
	}
	counter++;
	row++;

	// Wenn das Schiff eine Flotte anführt, dann Schiffe in dieser Flotte anzeigen
	for (CShips::iterator i = pShip->second.begin(); i != pShip->second.end(); ++i)
	{
		// mehrere Spalten anlegen, falls mehr Schiffe in dem System sind
		if (counter != 0 && counter%9 == 0)
		{
			column++;
			row = 0;
		}
		// Wenn wir eine Seite vollhaben
		if (counter%18 == 0)
			column = 1;
		if (counter < m_iFleetPage*18 && counter >= (m_iFleetPage-1)*18)
		{
			//bool bMarked = (i + 1 == pDoc->GetNumberOfTheShipInFleet());
			bool bMarked = &i->second == m_pMarkedShip;
			CPoint pt(250 * column, 65 * row + 60);
			i->second.DrawShip(g, pDoc->GetGraphicPool(), pt, bMarked, bUnknown, FALSE, normalColor, markColor, font);
			m_vShipRects.push_back(pair<CRect, CShips*>(CRect(pt.x, pt.y + 20, pt.x + 250, pt.y + 85), &i->second));
		}
		row++;
		counter++;
		if (counter > m_iFleetPage*18)
			break;
	}

	// Die Buttons für vor und zurück darstellen, wenn wir mehr als 9 Schiffe in dem Sektor sehen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);
	fontFormat.SetAlignment(StringAlignmentCenter);

	m_bShowNextButton = FALSE;
	if (counter > 18 && counter > m_iFleetPage*18)
	{
		m_bShowNextButton = TRUE;
		if (graphic)
			g->DrawImage(graphic, 540, 680, 120 ,30);
		s = CResourceManager::GetString("BTN_NEXT");
		g->DrawString(CComBSTR(s), -1, &font, RectF(540, 680, 120, 30), &fontFormat, &btnBrush);
	}
	// back-Button
	m_bShowBackButton = FALSE;
	if (m_iFleetPage > 1)
	{
		m_bShowBackButton = TRUE;

		if (graphic)
			g->DrawImage(graphic, 286, 680, 120 ,30);
		s = CResourceManager::GetString("BTN_BACK");
		g->DrawString(CComBSTR(s), -1, &font, RectF(286, 680, 120, 30), &fontFormat, &btnBrush);
	}

	// "Flottenzusammenstellung" in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	s = CResourceManager::GetString("FLEET_MENUE");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormat, &fontBrush);
}

//BEGINN: HELEPRS FOR CFleetMenuView::OnLButtonDown
static unsigned CheckClickedButtonRect(const CPoint& point) {
	// Überprüfen ob wir auf den Button geklickt haben, der alle Schiffe der gleichen Klasse wie das Anführerschiff
	// der Flotte hinzufügt bzw. entfernt
	if (CRect(0,220,250,250).PtInRect(point))
		return 1;
	// des gleichen Types hinzufügen
	else if (CRect(0,270,250,300).PtInRect(point))
		return 2;
	// alle Schiffe hinzufügen
	else if (CRect(0,320,250,350).PtInRect(point))
		return 3;
	// fremde Klasse entfernen
	else if (CRect(0,480,250,510).PtInRect(point))
		return 4;
	// fremden Typ entfernen
	else if (CRect(0,530,250,560).PtInRect(point))
		return 5;
	// fremde Klasse entfernen
	else if (CRect(0,580,250,610).PtInRect(point))
		return 6;
	return 0;
}

//END: HELEPRS FOR CFleetMenuView::OnLButtonDown
void CFleetMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBotf2Doc* pDoc = dynamic_cast<CBotf2Doc*>(GetDocument());
	assert(pDoc);
	if (!pDoc->m_bDataReceived)
		return;
	CMajor* pMajor = m_pPlayersRace;
	assert(pMajor);
	CalcLogicalPoint(point);
	// Wenn wir in der Flottenansicht sind
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);

	// Überprüfen ob wir auf den next-Button geklickt haben
	if (m_bShowNextButton == TRUE && CRect(540,r.bottom-70,660,r.bottom-40).PtInRect(point))
	{
		m_iFleetPage++;
		Invalidate();
		return;
	}
	// Überprüfen ob wir auf den back-Button geklickt haben
	else if (m_bShowBackButton == TRUE && CRect(290,r.bottom-70,410,r.bottom-40).PtInRect(point))
	{
		m_iFleetPage--;
		Invalidate();
		return;
	}
	// Fremde Flotten können nicht bearbeitet werden
	assert(pDoc->FleetShip()->second.GetOwnerOfShip() == pMajor->GetRaceID());
	// Auf welches Rechteck haben wir geklickt (gleiche Klasse oder gleichen Typ oder alle hinzufügen?)
	const unsigned whichRect = CheckClickedButtonRect(point);
	// Wenn wir auf irgendeinen der Buttons geklickt haben um Schiffe hinzuzufügen
	if (whichRect > 0 && whichRect < 4)
	{
		const CPoint& ko = pDoc->FleetShip()->second.GetKO();
		for(CShipArray::iterator i = pDoc->m_ShipArray.begin(); i != pDoc->m_ShipArray.end();) {
			const CShipArray::iterator& fleetship = pDoc->FleetShip();
			if (i->second.GetOwnerOfShip() != fleetship->second.GetOwnerOfShip() || i->second.GetKO() != ko || i->second.IsStation()) {
				++i;
				continue;
			}
			if (((whichRect == 1 && i->second.GetShipClass() == fleetship->second.GetShipClass())
					|| (whichRect == 2 && i->second.GetShipType() == fleetship->second.GetShipType()) || whichRect == 3)
				&& (&i->second != &fleetship->second && !i->second.HasFleet()))
			{
				// Jetzt fügen wir der Flotte das angeklickte Schiff hinzu, wenn es nicht das Schiff selbst ist,
				// welches die Flotte anführt und wenn es selbst keine Flotte besitzt
				fleetship->second.AddShipToFleet(i->second);
				// Wenn wir das Schiff da hinzugefügt haben, dann müssen wir das aus der normalen Schiffsliste
				// rausnehmen, damit es nicht zweimal im Spiel vorkommt
				pDoc->m_ShipArray.RemoveAt(i);
				// Wenn wir so Schiffe hinzufügen Ansicht auf Seite 1 stellen und markiertes Schiff ist
				// das Anführerschiff
				pDoc->SetShipInFleet(fleetship->second.end());
				m_iFleetPage = 1;
				continue;//increment only when not removing, since when removing the iterator already
				//points to the next element
			}
			++i;
		}
		Invalidate(FALSE);
		pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
		return;
	}
	// Wenn wir auf einen der Buttons geklickt haben um Schiffe zu entfernen
	else if (whichRect > 3)
	{
		const CShipArray::iterator& fleetship = pDoc->FleetShip();
		for(CShips::iterator i = fleetship->second.begin(); i != fleetship->second.end();) {
			if ((whichRect == 4 && fleetship->second.GetShipClass() != i->second.GetShipClass())
				|| (whichRect == 5 && fleetship->second.GetShipType() != i->second.GetShipType())
				|| whichRect == 6)
			{
				// Das Schiff welches wir aus der Flotte nehmen stecken wir wieder in das normale Schiffsarray
				pDoc->m_ShipArray.Add(i->second);
				fleetship->second.RemoveShipFromFleet(i);
				pDoc->SetShipInFleet(fleetship->second.end());
				continue;
			}
			Invalidate(FALSE);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
			++i;
		}
	}
	float column = ((float)(point.x - 287) / 250);
	if (column < 0)
		return;
	if (((float)(point.x - 287) / 250) - (int)column > 0.64)
		return;
	float row = ((float)(point.y - 90) / 65);
	if (row < 0)
		return;
	if (((float)(point.y - 90) / 65) - (int)row > 0.77)
		return;
	if ((int)column > 1 || (int)row > 8)
		return;
	const USHORT i = ((int)column * 9 + (int)row) + (m_iFleetPage-1)*18;
	// mit dem 'i' haben wir das angeklickte Schiff
	// Dieses Schiff wird mit einem Linksklick aus der Flotte entfernt
	// Aufpassen müssen wir wenn wir das Schiff aus der Flotte entfernen wollen, welches diese Flotte
	// besitzt. Dann übernimmt das nächste Schiff die Flotte
	//i == 0 : the ship which possesses the fleet (at the top)
	const CShipArray::iterator& fleetship = pDoc->FleetShip();
	if (fleetship->second.HasFleet() && i <= fleetship->second.GetFleetSize())
	{
		// Wenn es nicht das Schiff ist welches die Flotte besitzt
		if (m_iFleetPage > 1 || (i != 0 && m_iFleetPage == 1))
		{
			CShips::iterator j = fleetship->second.iterator_at(i-1);
			// Das Schiff welches wir aus der Flotte nehmen stecken wir wieder in das normale Schiffsarray
			pDoc->m_ShipArray.Add(j->second);
			fleetship->second.RemoveShipFromFleet(j);
			//we set the marked ship to the ship which was following the removed one,
			//or to the leading ship in case there's none left (in which case j comes back
			//as the end iterator)
			pDoc->SetShipInFleet(j);
			// Wenn wir gerade das erste Schiff auf der nächsten Seite entfernt haben, dann eine Seite zurück
			if (m_iFleetPage > 1 && i == (m_iFleetPage-1)*18 && pDoc->FleetShip()->second.GetFleetSize() == i-1)
				m_iFleetPage--;
			// Wenn wir alle Schiffe aus der Flotte entfernt haben
			Invalidate();
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
			CSmallInfoView::SetShipInfo(true);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		}
		// Wenn es das Schiff ist, welches die Flotte besitzt
		else if (i == 0 && m_iFleetPage == 1)
		{
			const CShips& new_fleetship = fleetship->second.GiveFleetToFleetsFirstShip();
			const CShipArray::iterator it = pDoc->m_ShipArray.Add(new_fleetship);

			pDoc->SetCurrentShip(it);
			pDoc->SetFleetShip(it);
			//set the current ship to the new leader (which was the ship following the old leader)
			pDoc->SetShipInFleet(it->second.end());

			Invalidate();
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
			CSmallInfoView::SetShipInfo(true);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		}
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CFleetMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	CBotf2Doc* pDoc = dynamic_cast<CBotf2Doc*>(GetDocument());
	assert(pDoc);
	if (!pDoc->m_bDataReceived)
		return;
	CalcLogicalPoint(point);
	// wurde die Maus über ein Schiff gehalten
	for(std::vector<std::pair<CRect, CShips*>>::const_iterator i = m_vShipRects.begin(); i != m_vShipRects.end(); ++i) {
		if (!i->first.PtInRect(point))
			continue;
		if (i->second == m_pMarkedShip)
			return;
		if(i->second == &pDoc->FleetShip()->second) {
			pDoc->SetShipInFleet(pDoc->FleetShip()->second.end());
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		}
		else
			for (CShips::iterator j = pDoc->FleetShip()->second.begin(); j != pDoc->FleetShip()->second.end(); ++j) {
				if (&j->second != i->second)
					continue;
				pDoc->SetShipInFleet(j);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
				break;
			}

		InvalidateRect(m_rLastMarkedRect, FALSE);
		CRect r = i->first;
		CalcDeviceRect(r);
		m_rLastMarkedRect = r;
		m_pMarkedShip = i->second;
		InvalidateRect(r, FALSE);
		return;
	}
	CMainBaseView::OnMouseMove(nFlags, point);
}

void CFleetMenuView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Das hier alles nur machen, wenn wir in der Flotten-Ansicht sind
	CGalaxyMenuView::SetMoveShip(FALSE);
	pDoc->SetCurrentShip(pDoc->FleetShip());
	pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pMajor->GetRaceID());
	pDoc->GetMainFrame()->SelectBottomView(SHIP_BOTTOM_VIEW);
	CSmallInfoView::SetShipInfo(true);
	pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));

	CMainBaseView::OnRButtonDown(nFlags, point);
}


void CFleetMenuView::CreateButtons()
{
	// alle Buttons in der View anlegen und Grafiken laden
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CFleetMenuView::CreateTooltip(void)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return "";

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	CShips* pShip = NULL;
	for (UINT i = 0; i < m_vShipRects.size(); i++)
		if (m_vShipRects[i].first.PtInRect(pt))
		{
			pShip = m_vShipRects[i].second;
			break;
		}

	if (!pShip)
		return "";

	bool bUnknown = (pMajor->GetRaceID() != pShip->GetOwnerOfShip() && pMajor->IsRaceContacted(pShip->GetOwnerOfShip()) == false);
	if (bUnknown)
	{
		// Wenn kein diplomatischer Kontakt möglich ist, wird das Schiff immer angezeigt
		CRace* pShipOwner = pDoc->GetRaceCtrl()->GetRace(pShip->GetOwnerOfShip());
		if (pShipOwner)
			bUnknown = !pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY);
	}

	// ist der Besitzer des Schiffes unbekannt?
	if (bUnknown)
	{
		CString s = CResourceManager::GetString("UNKNOWN");
		s = CHTMLStringBuilder::GetHTMLColor(s);
		s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
		s = CHTMLStringBuilder::GetHTMLCenter(s);
		return s;
	}

	return pShip->GetTooltip(false);
}
