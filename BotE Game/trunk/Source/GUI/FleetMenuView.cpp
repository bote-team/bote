// FleetMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "GalaxyMenuView.h"
#include "FleetMenuView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"
#include "Ships\Fleet.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "Graphic\memdc.h"

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

	CShip* pShip = &(pDoc->FleetShip());
	bool bUnknown = (pMajor->GetRaceID() != pShip->GetOwnerOfShip() && pMajor->IsRaceContacted(pShip->GetOwnerOfShip()) == false);
	if (bUnknown)
	{
		// Wenn kein diplomatischer Kontakt möglich ist, wird das Schiff immer angezeigt
		CRace* pShipOwner = pDoc->GetRaceCtrl()->GetRace(pShip->GetOwnerOfShip());
		if (pShipOwner)
			bUnknown = !pShipOwner->HasSpecialAbility(SPECIAL_NO_DIPLOMACY);
	}

	if (!bUnknown)
	{
		// der gleichen Klasse hinzufügen
		s.Format("%s-%s", pShip->GetShipClass(),CResourceManager::GetString("CLASS"));
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,220,250,30), &fontFormat, &fontBrush);
		// des gleichen Types hinzufügen
		s.Format("%s %s",CResourceManager::GetString("TYPE"), pShip->GetShipTypeAsString());
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
		s.Format("%s %s-%s",CResourceManager::GetString("NOT"),	pShip->GetShipClass(),CResourceManager::GetString("CLASS"));
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,480,250,30), &fontFormat, &fontBrush);
		// fremden Types entfernen
		s.Format("%s %s %s",CResourceManager::GetString("NOT"),CResourceManager::GetString("TYPE"),	pShip->GetShipTypeAsString());
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
		bool bMarked = pShip == m_pMarkedShip;
		CPoint pt(250 * column, 65 * row + 60);
		pShip->DrawShip(g, pDoc->GetGraphicPool(), pt, bMarked, bUnknown, FALSE, normalColor, markColor, font);
		m_vShipRects.push_back(pair<CRect, CShip*>(CRect(pt.x, pt.y + 20, pt.x + 250, pt.y + 85), pShip));
	}
	counter++;
	row++;

	// Wenn das Schiff eine Flotte anführt, dann Schiffe in dieser Flotte anzeigen
	if (pShip->GetFleet() != NULL)
	{
		for (USHORT i = 0; i < pShip->GetFleet()->GetFleetSize(); i++)
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
				bool bMarked = pShip->GetShipFromFleet(i) == m_pMarkedShip;
				CPoint pt(250 * column, 65 * row + 60);
				pShip->GetShipFromFleet(i)->DrawShip(g, pDoc->GetGraphicPool(), pt, bMarked, bUnknown, FALSE, normalColor, markColor, font);
				m_vShipRects.push_back(pair<CRect, CShip*>(CRect(pt.x, pt.y + 20, pt.x + 250, pt.y + 85), pShip->GetShipFromFleet(i)));
			}
			row++;
			counter++;
			if (counter > m_iFleetPage*18)
				break;
		}
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
void CFleetMenuView::OnLButtonDown(UINT nFlags, CPoint point)
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

	// Wenn wir in der Flottenansicht sind
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);

	USHORT whichRect = 0;	// Auf welches Rechteck haben wir geklickt (gleiche Klasse oder gleichen Typ oder alle hinzufügen?)

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
	if (pDoc->FleetShip().GetOwnerOfShip() != pMajor->GetRaceID())
		return;

	// Überprüfen ob wir auf den Button geklickt haben, der alle Schiffe der gleichen Klasse wie das Anführerschiff
	// der Flotte hinzufügt bzw. entfernt
	if (CRect(0,220,250,250).PtInRect(point))
		whichRect = 1;
	// des gleichen Types hinzufügen
	else if (CRect(0,270,250,300).PtInRect(point))
		whichRect = 2;
	// alle Schiffe hinzufügen
	else if (CRect(0,320,250,350).PtInRect(point))
		whichRect = 3;
	// fremde Klasse entfernen
	else if (CRect(0,480,250,510).PtInRect(point))
		whichRect = 4;
	// fremden Typ entfernen
	else if (CRect(0,530,250,560).PtInRect(point))
		whichRect = 5;
	// fremde Klasse entfernen
	else if (CRect(0,580,250,610).PtInRect(point))
		whichRect = 6;
	// Wenn wir auf irgendeinen der Buttons geklickt haben um Schiffe hinzuzufügen
	if (whichRect > 0 && whichRect < 4)
	{
		CPoint ko = pDoc->FleetShip().GetKO();
		for (int i = 0; i < pDoc->m_ShipArray.GetSize(); i++)
			if (pDoc->m_ShipArray.GetAt(i).GetOwnerOfShip() == pDoc->FleetShip().GetOwnerOfShip())
				if (pDoc->m_ShipArray.GetAt(i).GetKO() == ko)
					if (!pDoc->m_ShipArray.GetAt(i).IsStation())
						if ((whichRect == 1 && pDoc->m_ShipArray.GetAt(i).GetShipClass() == pDoc->FleetShip().GetShipClass())
							|| (whichRect == 2 && pDoc->m_ShipArray.GetAt(i).GetShipType() == pDoc->FleetShip().GetShipType())
							|| whichRect == 3)
							{
							// Wenn das Schiff noch keine Flotte hat, dann müssen wir erstmal eine Flotte bilden
							if (pDoc->FleetShip().GetFleet() == 0 && pDoc->GetNumberOfFleetShip() != i)
									pDoc->FleetShip().CreateFleet();
							// Jetzt fügen wir der Flotte das angeklickte Schiff hinzu, wenn es nicht das Schiff selbst ist,
							// welches die Flotte anführt und wenn es selbst keine Flotte besitzt
							if (pDoc->GetNumberOfFleetShip() != i && pDoc->m_ShipArray.GetAt(i).GetFleet() == 0)
							{
								pDoc->FleetShip().AddShipToFleet(pDoc->m_ShipArray[i]);
								// Wenn wir hier removen und ein Schiff im Feld entfernen, welches vor unserem FleetShip
								// ist, dann müssen wir die Nummer des FleetShips um eins verringern
								if (i < pDoc->GetNumberOfFleetShip())
									pDoc->SetNumberOfFleetShip(pDoc->GetNumberOfFleetShip()-1);
								// Wenn wir das Schiff da hinzugefügt haben, dann müssen wir das aus der normalen Schiffsliste
								// rausnehmen, damit es nicht zweimal im Spiel vorkommt
								pDoc->m_ShipArray.RemoveAt(i);
								// Wenn wir das letzte Schiff entfernt haben, dann müssen wir die angeklickte Nummer im Spiel
								// um eins zurücknehmen
								if (i == pDoc->m_ShipArray.GetSize())
									pDoc->SetCurrentShipIndex(i-1);
								i--;
								// Wenn wir so Schiffe hinzufügen Ansicht auf Seite 1 stellen und markiertes Schiff ist
								// das Anführerschiff
								m_iFleetPage = 1;
								pDoc->SetNumberOfTheShipInFleet(0);
							}
						}
		Invalidate(FALSE);
		pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
		return;
	}
	// Wenn wir auf einen der Buttons geklickt haben um Schiffe zu entfernen
	else if (whichRect > 3)
	{
		if (pDoc->FleetShip().GetFleet() != 0)
			for (USHORT i = 0; i < pDoc->FleetShip().GetFleet()->GetFleetSize(); i++)
				if ((whichRect == 4 && pDoc->FleetShip().GetShipClass() != pDoc->FleetShip().GetShipFromFleet(i)->GetShipClass())
					|| (whichRect == 5 && pDoc->FleetShip().GetShipType() != pDoc->FleetShip().GetShipFromFleet(i)->GetShipType())
					|| whichRect == 6)
				{
					// Das Schiff welches wir aus der Flotte nehmen stecken wir wieder in das normale Schiffsarray
					pDoc->m_ShipArray.Add(pDoc->m_ShipArray.end(), *(pDoc->FleetShip().GetShipFromFleet(i)));
					pDoc->FleetShip().RemoveShipFromFleet(i);
					// Wenn wir das letzte Schiff in der Flotte entfernt haben, dann müssen wir
					// das markierte Schiff dekrementieren, da sonst ein ungültiger Feldaufruf kommt
					if (i >= (short)pDoc->FleetShip().GetFleet()->GetFleetSize())
						pDoc->SetNumberOfTheShipInFleet(i-1);
					// Wenn wir alle Schiffe aus der Flotte entfernt haben
					if (pDoc->FleetShip().GetFleet()->GetFleetSize() == 0)
					{
						pDoc->FleetShip().DeleteFleet();
						pDoc->SetNumberOfTheShipInFleet(-1);
						break;
					}
					i--;
					m_iFleetPage = 1;
					pDoc->SetNumberOfTheShipInFleet(0);
				}
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
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
	USHORT i = ((int)column * 9 + (int)row) + (m_iFleetPage-1)*18;
	// mit dem 'i' haben wir das angeklickte Schiff
	// Dieses Schiff wird mit einem Linksklick aus der Flotte entfernt
	// Aufpassen müssen wir wenn wir das Schiff aus der Flotte entfernen wollen, welches diese Flotte
	// besitzt. Dann übernimmt das nächste Schiff die Flotte
	if (pDoc->FleetShip().GetFleet() != 0 && i <= pDoc->FleetShip().GetFleet()->GetFleetSize())
	{
		// Wenn es nicht das Schiff ist welches die Flotte besitzt
		if ((i != 0 && m_iFleetPage == 1) || m_iFleetPage > 1)
		{
			// Das Schiff welches wir aus der Flotte nehmen stecken wir wieder in das normale Schiffsarray
			pDoc->m_ShipArray.Add(pDoc->m_ShipArray.end(), *(pDoc->FleetShip().GetShipFromFleet(i-1)));
			pDoc->FleetShip().RemoveShipFromFleet(i-1);
			// Wenn wir das letzte Schiff in der Flotte entfernt haben, dann müssen wir
			// das markierte Schiff dekrementieren, da sonst ein ungültiger Feldaufruf kommt
			if (i > (short)pDoc->FleetShip().GetFleet()->GetFleetSize())
				pDoc->SetNumberOfTheShipInFleet(i-1);
			// Wenn wir gerade das erste Schiff auf der nächsten Seite entfernt haben, dann eine Seite zurück
			if (m_iFleetPage > 1 && i == (m_iFleetPage-1)*18 && pDoc->FleetShip().GetFleet()->GetFleetSize() == i-1)
				m_iFleetPage--;
			// Wenn wir alle Schiffe aus der Flotte entfernt haben
			if (pDoc->FleetShip().GetFleet()->GetFleetSize() == 0)
			{
				pDoc->FleetShip().DeleteFleet();
				pDoc->SetNumberOfTheShipInFleet(-1);
			}
			Invalidate();
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
			CSmallInfoView::SetShipInfo(true);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		}
		// Wenn es das Schiff ist, welches die Flotte besitzt
		else if (i == 0 && m_iFleetPage == 1)
		{
			CShip& ship = pDoc->FleetShip();
			const CShip& new_fleetship = ship.GiveFleetToFleetsFirstShip();
			pDoc->m_ShipArray.Add(pDoc->m_ShipArray.end(), new_fleetship);

			pDoc->SetNumberOfFleetShip(pDoc->m_ShipArray.GetUpperBound());
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
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);

	// wurde die Maus über ein Schiff gehalten
	for (UINT i = 0; i < m_vShipRects.size(); i++)
	{
		if (m_vShipRects[i].first.PtInRect(point))
		{
			bool bNewMarkedShip = m_vShipRects[i].second != m_pMarkedShip;
			if (bNewMarkedShip)
			{
				if (pDoc->FleetShip().GetFleet() != 0 && pDoc->FleetShip().GetFleet()->GetFleetSize())
				{
					for (int j = 0; j < pDoc->FleetShip().GetFleet()->GetFleetSize(); j++)
						if (pDoc->FleetShip().GetShipFromFleet(j) == m_vShipRects[i].second)
						{
							pDoc->SetNumberOfTheShipInFleet(j + 1);
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
							break;
						}
				}

				InvalidateRect(m_rLastMarkedRect, FALSE);
				CRect r = m_vShipRects[i].first;
				CalcDeviceRect(r);
				m_rLastMarkedRect = r;
				m_pMarkedShip = m_vShipRects[i].second;
				InvalidateRect(r, FALSE);
			}
			return;
		}
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
	pDoc->SetCurrentShipIndex(pDoc->GetNumberOfFleetShip());
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

	CShip* pShip = NULL;
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
