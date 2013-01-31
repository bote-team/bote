// TransportMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "TransportMenuView.h"
#include "Races\RaceController.h"
#include "ShipBottomView.h"
#include "Graphic\memdc.h"
#include "Ships/Ships.h"
#include "General/ResourceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTransportMenuView

IMPLEMENT_DYNCREATE(CTransportMenuView, CMainBaseView)

CTransportMenuView::CTransportMenuView() :
bg_transportmenu(),
m_nActiveTroopInShip(-1),
m_nActiveTroopInSystem(-1),
m_iTransportStorageQuantity(1),
m_ptLastSector(-1, 1)
{
}

CTransportMenuView::~CTransportMenuView()
{
	for (int i = 0; i < m_TransportButtons.GetSize(); i++)
	{
		delete m_TransportButtons[i];
		m_TransportButtons[i] = 0;
	}
	m_TransportButtons.RemoveAll();
}

BEGIN_MESSAGE_MAP(CTransportMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CTransportMenuView::OnNewRound()
{
	m_nActiveTroopInShip = -1;
	m_nActiveTroopInSystem = -1;
	m_ptLastSector = CPoint(-1, -1);
	m_vShipTroops.clear();
}

// CTransportMenuView drawing
void CTransportMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	// TODO: add draw code here
	CMyMemDC pDC(dc);
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

	// ***************************** DIE SCHIFFSTRANSPORT ANSICHT ZEICHNEN **********************************
	DrawTransportMenue(&g);
	// ************** DIE SCHIFFSDESIGN ANSICHT ZEICHNEN ist hier zu Ende **************

	g.ReleaseHDC(pDC->GetSafeHdc());
}


// CTransportMenuView diagnostics

#ifdef _DEBUG
void CTransportMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CTransportMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTransportMenuView message handlers

void CTransportMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	// Transportansicht
	m_iTransportStorageQuantity = 1;

	// View bei den Tooltipps anmelden
	resources::pMainFrame->AddToTooltip(this);
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CTransportMenuView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CreateButtons();

	// alle Hintergrundgrafiken laden
	CString sPrefix = pMajor->GetPrefix();
	bg_transportmenu = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "transportmenu.boj");
}

void CTransportMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CTransportMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Transportmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CTransportMenuView::DrawTransportMenue(Graphics* g)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CPoint p = pDoc->GetKO();
	if (p != m_ptLastSector)
	{
		m_ptLastSector = p;
		m_nActiveTroopInShip = -1;
		m_nActiveTroopInSystem = -1;
		m_vShipTroops.clear();
	}

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	SolidBrush fontBrush(markColor);

	if (bg_transportmenu)
		g->DrawImage(bg_transportmenu, 0, 0, 1075, 750);

	CString s;
	CString systemOwner = pDoc->GetSystem(p.x, p.y).GetOwnerOfSystem();
	CString shipOwner = pDoc->CurrentShip()->second->GetOwnerOfShip();

	Gdiplus::Font font(CComBSTR(fontName), fontSize);
	if (systemOwner == shipOwner)
		g->DrawString(CComBSTR(CResourceManager::GetString("SYSTEM_STORAGE")), -1, &font, RectF(200,80,338,25), &fontFormat, &fontBrush);

	g->DrawString(CComBSTR(CResourceManager::GetString("SHIP_STORAGE")), -1, &font, RectF(538,80,338,25), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("TITAN")), -1, &font, RectF(0,120,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("DEUTERIUM")), -1, &font, RectF(0,180,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("DURANIUM")), -1, &font, RectF(0,240,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("CRYSTAL")), -1, &font, RectF(0,300,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("IRIDIUM")), -1, &font, RectF(0,360,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("DERITIUM")), -1, &font, RectF(0,420,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("TROOPS")), -1, &font, RectF(0,560,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CComBSTR(CResourceManager::GetString("SELECTED")), -1, &font, RectF(0,620,1075,60), &fontFormat, &fontBrush);
	fontBrush.SetColor(normalColor);

	// Inhalte des system- und globalen Lagers zeichnen
	const CShipMap::const_iterator& ship = pDoc->CurrentShip();
	for (int i = TITAN; i <= DERITIUM; i++)
	{
		int res = ship->second->GetLoadedResources(i);
		for(CShips::const_iterator j = ship->second->begin(); j != ship->second->end(); ++j)
			res += j->second->GetLoadedResources(i);
		// Lagerinhalt im Schiff zeichnen
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%d", res);
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,120+i*60,725,60), &fontFormat, &fontBrush);

		if (systemOwner == shipOwner)
		{
			// Lagerinhalt im System zeichnen
			fontFormat.SetAlignment(StringAlignmentNear);
			s.Format("%d",pDoc->GetSystem(p.x, p.y).GetResourceStore(i));
			g->DrawString(CComBSTR(s), -1, &font, RectF(350,120+i*60,725,60), &fontFormat, &fontBrush);
		}
	}
	// verfügbaren Lagerraum im Schiff zeichnen:
	int usedStorage = ship->second->GetUsedStorageRoom(&pDoc->m_TroopInfo);
	int storageRoom = ship->second->GetStorageRoom();
	// Lagerraum der Schiffe in der Flotte beachten
	for (CShips::const_iterator it = ship->second->begin(); it != ship->second->end(); ++it)
	{
		usedStorage += it->second->GetUsedStorageRoom(&pDoc->m_TroopInfo);
		storageRoom += it->second->GetStorageRoom();
	}

	// Truppen aus Schiffen sammeln
	CreateTransportedTroopsVector();

	fontFormat.SetAlignment(StringAlignmentCenter);
	s.Format("%s: %d", CResourceManager::GetString("AVAILABLE"), storageRoom - usedStorage);
	g->DrawString(CComBSTR(s), -1, &font, RectF(600,490,225,30), &fontFormat, &fontBrush);

	// Truppen im System und im Schiff zeichnen
	fontFormat.SetAlignment(StringAlignmentFar);
	s.Format("%d", m_vShipTroops.size());
	g->DrawString(CComBSTR(s), -1, &font, RectF(0,560,725,60), &fontFormat, &fontBrush);

	if (systemOwner == shipOwner)
	{
		// Die Buttons zum Transportieren zeichen
		// Buttons zeichnen
		for (int i = 0; i < m_TransportButtons.GetSize(); i++)
			m_TransportButtons[i]->DrawButton(*g, pDoc->GetGraphicPool(), Gdiplus::Font(NULL), fontBrush);

		s.Format("%d",pDoc->GetSystem(p.x, p.y).GetTroops()->GetSize());
		fontFormat.SetAlignment(StringAlignmentNear);
		g->DrawString(CComBSTR(s), -1, &font, RectF(350,560,725,60), &fontFormat, &fontBrush);
	}

	//////////////////////////////////////////////////////////////////////////
	// Name der aktuell ausgewählten Truppe im System und auf dem Schiff zeichnen

	// Truppenbeschreibung auf der linken Seite, also die im System anzeigen
	//pDC->Rectangle(25,270,225,700);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
	if (pDoc->GetSystem(p.x, p.y).GetTroops()->GetSize() > 0 && systemOwner == shipOwner)
	{
		if (m_nActiveTroopInSystem == -1 || m_nActiveTroopInSystem >= pDoc->GetSystem(p.x, p.y).GetTroops()->GetSize())
			m_nActiveTroopInSystem = 0;

		BYTE id = pDoc->GetSystem(p.x, p.y).GetTroops()->GetAt(m_nActiveTroopInSystem).GetID();
		// ein paar Daten zur ausgewählten Einheit werden links angezeigt
		fontBrush.SetColor(markColor);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = pDoc->m_TroopInfo.GetAt(id).GetName();
		g->DrawString(CComBSTR(s), -1, &font, RectF(25,280,200,30), &fontFormat, &fontBrush);

		fontBrush.SetColor(normalColor);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format("%s: %d",CResourceManager::GetString("OPOWER"),pDoc->GetSystem(p.x, p.y).GetTroops()->GetAt(m_nActiveTroopInSystem).GetOffense());
		g->DrawString(CComBSTR(s), -1, &font, RectF(40,310,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("DPOWER"),pDoc->GetSystem(p.x, p.y).GetTroops()->GetAt(m_nActiveTroopInSystem).GetDefense());
		g->DrawString(CComBSTR(s), -1, &font, RectF(40,340,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("EXPERIANCE"),pDoc->GetSystem(p.x, p.y).GetTroops()->GetAt(m_nActiveTroopInSystem).GetExperiance());
		g->DrawString(CComBSTR(s), -1, &font, RectF(40,370,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("PLACE"),pDoc->m_TroopInfo.GetAt(id).GetSize());
		g->DrawString(CComBSTR(s), -1, &font, RectF(40,400,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("MORALVALUE"),pDoc->m_TroopInfo.GetAt(id).GetMoralValue());
		g->DrawString(CComBSTR(s), -1, &font, RectF(40,430,185,30), &fontFormat, &fontBrush);

		fontFormat.SetLineAlignment(StringAlignmentNear);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = pDoc->m_TroopInfo.GetAt(id).GetDescription();
		g->DrawString(CComBSTR(s), -1, &font, RectF(30,465,190,265), &fontFormat, &fontBrush);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

		s.Format("#%d: %s", m_nActiveTroopInSystem+1, pDoc->m_TroopInfo.GetAt(id).GetName());
		// Das Bild für die Truppe zeichnen
		CString file;
		file.Format("Troops\\%s", pDoc->m_TroopInfo.GetAt(id).GetGraphicFileName());
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
		if (graphic == NULL)
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Troops\\ImageMissing.bop");
		if (graphic)
			g->DrawImage(graphic, 25, 95, 200, 150);
	}
	else
	{
		s = CResourceManager::GetString("NONE")+" "+CResourceManager::GetString("SELECTED");
	}

	if (systemOwner == shipOwner)
	{
		fontFormat.SetAlignment(StringAlignmentNear);
		g->DrawString(CComBSTR(s), -1, &font, RectF(290,620,198,60), &fontFormat, &fontBrush);
	}

	// Truppenbeschreibung auf der rechten Seite, also die im Schiff anzeigen
	//pDC->Rectangle(850,270,1050,700);
	if (!m_vShipTroops.empty())
	{
		if (m_nActiveTroopInShip == -1 || m_nActiveTroopInShip >= (int)m_vShipTroops.size())
			m_nActiveTroopInShip = 0;

		CTroop* pTroop = m_vShipTroops[m_nActiveTroopInShip].second;
		BYTE id = pTroop->GetID();
		// ein paar Daten zur ausgewählten Einheit werden rechts angezeigt
		fontBrush.SetColor(markColor);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = pDoc->m_TroopInfo.GetAt(id).GetName();
		g->DrawString(CComBSTR(s), -1, &font, RectF(850,280,200,30), &fontFormat, &fontBrush);

		fontBrush.SetColor(normalColor);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format("%s: %d",CResourceManager::GetString("OPOWER"), pTroop->GetOffense());
		g->DrawString(CComBSTR(s), -1, &font, RectF(865,310,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("DPOWER"), pTroop->GetDefense());
		g->DrawString(CComBSTR(s), -1, &font, RectF(865,340,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("EXPERIANCE"), pTroop->GetExperiance());
		g->DrawString(CComBSTR(s), -1, &font, RectF(865,370,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("PLACE"),pDoc->m_TroopInfo.GetAt(id).GetSize());
		g->DrawString(CComBSTR(s), -1, &font, RectF(865,400,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("MORALVALUE"),pDoc->m_TroopInfo.GetAt(id).GetMoralValue());
		g->DrawString(CComBSTR(s), -1, &font, RectF(865,430,185,30), &fontFormat, &fontBrush);

		fontFormat.SetLineAlignment(StringAlignmentNear);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = pDoc->m_TroopInfo.GetAt(id).GetDescription();
		g->DrawString(CComBSTR(s), -1, &font, RectF(855,465,190,265), &fontFormat, &fontBrush);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

		s.Format("#%d: %s", m_nActiveTroopInShip+1, pDoc->m_TroopInfo.GetAt(id).GetName());
		// Das Bild für die Truppe zeichnen
		CString file;
		file.Format("Troops\\%s", pDoc->m_TroopInfo.GetAt(id).GetGraphicFileName());
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
		if (graphic == NULL)
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Troops\\ImageMissing.bop");
		if (graphic)
			g->DrawImage(graphic, 850, 95, 200, 150);
	}
	else
	{
		s = CResourceManager::GetString("NONE")+" "+CResourceManager::GetString("SELECTED");
	}

	fontFormat.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &font, RectF(588,620,198,60), &fontFormat, &fontBrush);

	fontFormat.SetTrimming(StringTrimmingNone);

	// Button zum Ändern der Menge, wieviel pro Klick vom oder ins Schiff verschoben werden zeichnen, sowie den
	// Button um die nächste Einheit auf dem System bzw. auf dem Schiff zu wählen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);

	if (systemOwner == shipOwner)
	{
		fontFormat.SetAlignment(StringAlignmentFar);
		s = CResourceManager::GetString("MULTIPLIER");
		g->DrawString(CComBSTR(s), -1, &font, RectF(0,490,457,30), &fontFormat, &fontBrush);

		if (graphic)
			g->DrawImage(graphic, 477,490,120,30);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s.Format("%d",m_iTransportStorageQuantity);
		g->DrawString(CComBSTR(s), -1, &font, RectF(477,490,120,30), &fontFormat, &btnBrush);

		// Vor-Button links
		if (pDoc->GetSystem(p.x, p.y).GetTroops()->GetSize() > 0)
		{
			if (graphic)
				g->DrawImage(graphic, 290,670,120,30);
			s = CResourceManager::GetString("BTN_NEXT");
			g->DrawString(CComBSTR(s), -1, &font, RectF(290,670,120,30), &fontFormat, &btnBrush);
		}
		// Vor-Button rechts
		if (!m_vShipTroops.empty())
		{
			if (graphic)
				g->DrawImage(graphic, 665,670,120,30);
			s = CResourceManager::GetString("BTN_NEXT");
			g->DrawString(CComBSTR(s), -1, &font, RectF(665,670,120,30), &fontFormat, &btnBrush);
		}
	}
	else if (!m_vShipTroops.empty())
	{
		// Vor-Button rechts
		if (graphic)
			g->DrawImage(graphic, 665,670,120,30);

		s = CResourceManager::GetString("BTN_NEXT");
		g->DrawString(CComBSTR(s), -1, &font, RectF(665,670,120,30), &fontFormat, &btnBrush);
	}

	// Transport auf "Systemname" mit größerer Schrift in der Mitte zeichnen
	fontFormat.SetAlignment(StringAlignmentCenter);
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	// Transport auf "Systemname" mit größerer Schrift in der Mitte zeichnen
	if (systemOwner == shipOwner)
		s = CResourceManager::GetString("TRANSPORT_MENUE")+" "+pDoc->GetSector(p.x,p.y).GetName();
	else
		s = CResourceManager::GetString("TRANSPORT_MENUE")+" "+CResourceManager::GetString("SHIP");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);
}

void CTransportMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	if (!pMajor)
		return;

	// Wenn wir in der Transportansicht sind (brauchen auch nur Klicks überprüfen, wenn das Schiff Lagerraum hat)
	CShipMap::iterator ship = pDoc->CurrentShip();
	if (ship->second->GetOwnerOfShip() != pMajor->GetRaceID())
		return;

	CalcLogicalPoint(point);

	CPoint p = pDoc->GetKO();
	CString systemOwner = pDoc->GetSystem(p.x, p.y).GetOwnerOfSystem();
	CString shipOwner   = ship->second->GetOwnerOfShip();

	BOOLEAN isFleet = FALSE;
	int number = 1;
	if (ship->second->HasFleet())
	{
		isFleet = TRUE;
		number += ship->second->GetFleetSize();
	}

	// Buttons um Ressourcen zu verschieben
	int i = -1;
	if (ButtonReactOnLeftClick(point, &m_TransportButtons, i, FALSE, TRUE))
	{
		if (systemOwner == shipOwner)
		{
			if (i < 7)
			{
				// handelt es sich um Truppen
				if (i == 6)
				{
					// keine Truppe ausgewählt -> Abbruch
					if (m_nActiveTroopInShip == -1)
						return;

					// keine Truppen auf dem Schiff
					if (m_vShipTroops.empty())
						return;

					int nQuantity = m_iTransportStorageQuantity;
					while (nQuantity > 0)
					{
						// Truppe und Schiff holen
						CTroop* pTroop = m_vShipTroops[m_nActiveTroopInShip].second;
						CShips* pShip = m_vShipTroops[m_nActiveTroopInShip].first;

						// Position der Truppe im Vektor auf dem Schiff ermitteln
						int nPos = -1;
						for (int nTroop = 0; nTroop < pShip->GetTransportedTroops()->GetSize(); nTroop++)
						{
							if (&(pShip->GetTransportedTroops()->GetAt(nTroop)) == pTroop)
							{
								nPos = nTroop;
								break;
							}
						}

						if (nPos == -1)
						{
							// darf nicht sein
							ASSERT(FALSE);
							break;
						}

						// Truppe ins System stecken
						pDoc->GetSystem(p.x, p.y).GetTroops()->Add(*pTroop);
						m_nActiveTroopInSystem = pDoc->GetSystem(p.x, p.y).GetTroops()->GetUpperBound();
						// Danach auf Schiff entfernen (Zeiger wird ungültig)
						pShip->GetTransportedTroops()->RemoveAt(nPos);
						// weil alle Zeiger ungültig geworden sind, den Vektor neu erstellen
						CreateTransportedTroopsVector();
						if (m_vShipTroops.empty())
						{
							m_nActiveTroopInShip = -1;
							break;
						}

						// Vorgänger aufschalten
						m_nActiveTroopInShip = max(m_nActiveTroopInShip - 1, 0);

						nQuantity--;
					}

					Invalidate(FALSE);
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CShipBottomView));
					return;
				}
				// handelt es sich um Ressourcen
				else
				{
					int oldQuantity = m_iTransportStorageQuantity;

					// Lagergrenzen im System beachten
					if (i != DERITIUM)
					{
						if (m_iTransportStorageQuantity + pDoc->GetSystem(p.x, p.y).GetResourceStore(i) > MAX_RES_STORE)
							m_iTransportStorageQuantity = MAX_RES_STORE - pDoc->GetSystem(p.x, p.y).GetResourceStore(i);
					}
					else
					{
						UINT nMaxDeritiumStore = MAX_DERITIUM_STORE;
						if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetFieldStatus(1) == RESEARCH_STATUS::RESEARCHED)
							nMaxDeritiumStore *= pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(RESEARCH_COMPLEX::STORAGE_AND_TRANSPORT)->GetBonus(1);
						if (m_iTransportStorageQuantity + pDoc->GetSystem(p.x, p.y).GetResourceStore(i) > nMaxDeritiumStore)
							m_iTransportStorageQuantity = nMaxDeritiumStore - pDoc->GetSystem(p.x, p.y).GetResourceStore(i);
					}
					// gibt es nichts zu verschieben, dann abbrechen
					if (m_iTransportStorageQuantity == 0)
					{
						m_iTransportStorageQuantity = oldQuantity;
						return;
					}

					// Schiff und möglicherweise Schiffe in der Flotte durchgehen
					for (int j = 0; j < number; j++)
					{
						// wenn mehr Ressourcen auf dem Schiff vorhanden sind als verschoben werden
						if (ship->second->GetLoadedResources(i) >= m_iTransportStorageQuantity)
						{
							pDoc->GetSystem(p.x, p.y).SetResourceStore(i, m_iTransportStorageQuantity);
							ship->second->SetLoadedResources(-m_iTransportStorageQuantity, i);
							m_iTransportStorageQuantity = 0;
						}
						// wenn man mehr verschieben wollen als auf dem Schiff vorhanden sind
						else
						{
							m_iTransportStorageQuantity -= ship->second->GetLoadedResources(i);
							pDoc->GetSystem(p.x, p.y).SetResourceStore(i, ship->second->GetLoadedResources(i));
							ship->second->SetLoadedResources(-ship->second->GetLoadedResources(i), i);
						}
						if (m_iTransportStorageQuantity == NULL)
							break;
						if (isFleet && j < number-1)
							ship = pDoc->CurrentShip()->second->iterator_at(j);
					}
					m_iTransportStorageQuantity = oldQuantity;
					Invalidate(FALSE);
					return;
				}
			}
			// kleine Pfeilbuttons checken (rechts) -> auch checken ob genug Lagerraum vorhanden ist
			else
			{
				int usedStorage = 0;
				i -= 7;

				// handelt es sich um Truppen
				if (i == 6)
				{
					// keine Truppen im System
					if (m_nActiveTroopInSystem == -1)
						return;

					int nQuantity = m_iTransportStorageQuantity;
					// checken das noch genügend Lagerraum im Schiff vorhanden ist
					if (pDoc->GetSystem(p.x, p.y).GetTroops()->GetSize() > 0)
					{
						// Schiff und möglicherweise Schiffe in der Flotte durchgehen
						for (int j = 0; j < number; j++)
						{
							while (nQuantity > 0)
							{
								BYTE id = pDoc->GetSystem(p.x, p.y).GetTroops()->GetAt(m_nActiveTroopInSystem).GetID();
								// durch Truppen und Ressourcen auf Schiff
								usedStorage = ship->second->GetUsedStorageRoom(&pDoc->m_TroopInfo);
								// dazu der benötigte Platz durch die Truppe, welche hinzukommen soll
								usedStorage += pDoc->m_TroopInfo.GetAt(id).GetSize();
								if (usedStorage <= ship->second->GetStorageRoom())
								{
									ship->second->GetTransportedTroops()->Add(pDoc->GetSystem(p.x, p.y).GetTroops()->GetAt(m_nActiveTroopInSystem));
									pDoc->GetSystem(p.x, p.y).GetTroops()->RemoveAt(m_nActiveTroopInSystem);
									m_nActiveTroopInSystem--;
									m_nActiveTroopInShip++;
									Invalidate(FALSE);
									resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CShipBottomView));
									nQuantity--;
									if (pDoc->GetSystem(p.x, p.y).GetTroops()->GetSize() == 0)
										return;
								}
								else
									break;
							}
							if (isFleet && j < number-1)
								ship = pDoc->CurrentShip()->second->iterator_at(j);
						}
					}

					return;
				}
				// handelt es sich um Ressourcen
				else
				{
					int multi = 1;
					if (i == DERITIUM)
						multi = 250;

					int oldQuantity = m_iTransportStorageQuantity;
					int transportedRes = oldQuantity;
					// Schiff und möglicherweise Schiffe in der Flotte durchgehen
					for (int j = 0; j < number; j++)
					{
						m_iTransportStorageQuantity = oldQuantity;
						usedStorage = ship->second->GetUsedStorageRoom(&pDoc->m_TroopInfo);
						if (m_iTransportStorageQuantity*multi > ship->second->GetStorageRoom() - usedStorage)
							m_iTransportStorageQuantity = (ship->second->GetStorageRoom() - usedStorage) / multi;
						if (m_iTransportStorageQuantity > transportedRes)
							m_iTransportStorageQuantity = transportedRes;
						// wenn im System mehr Ressourcen vorhanden sind, als man verschieben möchte
						if (pDoc->GetSystem(p.x, p.y).GetResourceStore(i) >= m_iTransportStorageQuantity)
						{
							transportedRes -= m_iTransportStorageQuantity;
							pDoc->GetSystem(p.x, p.y).SetResourceStore(i, -m_iTransportStorageQuantity);
							ship->second->SetLoadedResources(m_iTransportStorageQuantity, i);
						}
						// wenn im System weniger Ressourcen vorhanden sind, als man aufs Schiff verschieben möchte
						else
						{
							transportedRes -= pDoc->GetSystem(p.x, p.y).GetResourceStore(i);
							ship->second->SetLoadedResources(pDoc->GetSystem(p.x, p.y).GetResourceStore(i), i);
							pDoc->GetSystem(p.x, p.y).SetResourceStore(i, -(int)pDoc->GetSystem(p.x, p.y).GetResourceStore(i));
						}
						if (transportedRes == NULL)
							break;
						if (isFleet && j < number-1)
							ship = pDoc->CurrentShip()->second->iterator_at(j);
					}
					m_iTransportStorageQuantity = oldQuantity;
					Invalidate(FALSE);
					return;
				}
			}
		}
	}
	// Button zum Ändern der Menge, wieviel pro Klick vom oder ins Schiff verschoben werden überprüfen, sowie den
	// Button um die nächste Einheit auf dem System bzw. auf dem Schiff wählen zu können überprüfen
	if (systemOwner == shipOwner)
	{
		// Menge der Ressourcen ändern, die bei jedem Transfer verschoben werden
		if (CRect(477,490,597,520).PtInRect(point))
		{
			m_iTransportStorageQuantity *= 10;
			if (m_iTransportStorageQuantity > 10000)
				m_iTransportStorageQuantity = 1;
			CRect r(477,490,597,520);
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
		// Vor-Button links
		else if (pDoc->GetSystem(p.x, p.y).GetTroops()->GetSize() > 0 && CRect(290,670,410,700).PtInRect(point))
		{
			m_nActiveTroopInSystem++;
			Invalidate(FALSE);
		}
		// Vor-Button rechts
		else if (CRect(665,670,785,700).PtInRect(point) && !m_vShipTroops.empty())
		{
			m_nActiveTroopInShip++;
			if (m_nActiveTroopInShip >= (int)m_vShipTroops.size())
				m_nActiveTroopInShip = 0;
			Invalidate(FALSE);
		}
	}
	else if (CRect(665,670,785,700).PtInRect(point) && !m_vShipTroops.empty())
	{
		// Vor-Button rechts
		m_nActiveTroopInShip++;
		if (m_nActiveTroopInShip >= (int)m_vShipTroops.size())
			m_nActiveTroopInShip = 0;
		Invalidate(FALSE);
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CTransportMenuView::CreateButtons()
{
	ASSERT((CBotf2Doc*)GetDocument());

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// alle Buttons in der View anlegen und Grafiken laden
	CString sPrefix = pMajor->GetPrefix();

	// Zuweisungsbuttons für Ressourcen
	CString fileN = "Other\\" + sPrefix + "buttonminus.bop";
	CString fileA = "Other\\" + sPrefix + "buttonminusa.bop";
	for (int i = TITAN; i <= DERITIUM; i++)
		m_TransportButtons.Add(new CMyButton(CPoint(290,134+i*60) , CSize(30,30), "", fileN, fileN, fileA));
	// plus für Truppen
	m_TransportButtons.Add(new CMyButton(CPoint(290,573) , CSize(30,30), "", fileN, fileN, fileA));

	fileN = "Other\\" + sPrefix + "buttonplus.bop";
	fileA = "Other\\" + sPrefix + "buttonplusa.bop";
	for (int i = TITAN; i <= DERITIUM; i++)
		m_TransportButtons.Add(new CMyButton(CPoint(755,134+i*60) , CSize(30,30), "", fileN, fileN, fileA));
	// plus für Truppen
	m_TransportButtons.Add(new CMyButton(CPoint(755,573) , CSize(30,30), "", fileN, fileN, fileA));
}
void CTransportMenuView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// Doppelklick direkt auf Linksklick weiterleiten
	CTransportMenuView::OnLButtonDown(nFlags, point);

	CMainBaseView::OnLButtonDblClk(nFlags, point);
}

void CTransportMenuView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	resources::pMainFrame->SelectMainView(GALAXY_VIEW, pMajor->GetRaceID());

	CMainBaseView::OnRButtonDown(nFlags, point);
}

void CTransportMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// Wenn wir in der Transportansicht sind (brauchen auch nur Klicks überprüfen, wenn das Schiff Lagerraum hat)
	const CShips::const_iterator& ship = pDoc->CurrentShip();
	if (ship->second->GetOwnerOfShip() != pMajor->GetRaceID())
		return;

	CPoint p = pDoc->GetKO();
	CString systemOwner = pDoc->GetSystem(p.x, p.y).GetOwnerOfSystem();
	CString shipOwner   = ship->second->GetOwnerOfShip();

	if (systemOwner == shipOwner)
	{
		CalcLogicalPoint(point);
		ButtonReactOnMouseOver(point, &m_TransportButtons);
	}

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CTransportMenuView::CreateTransportedTroopsVector()
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	const CShipMap::const_iterator& ship = pDoc->CurrentShip();

	m_vShipTroops.clear();
	for (int i = 0; i < ship->second->GetTransportedTroops()->GetSize(); i++)
		m_vShipTroops.push_back(make_pair(ship->second, &(ship->second->GetTransportedTroops()->GetAt(i))));

	for (CShips::const_iterator it = ship->second->begin(); it != ship->second->end(); ++it)
		for (int i = 0; i < it->second->GetTransportedTroops()->GetSize(); i++)
			m_vShipTroops.push_back(make_pair(it->second, &(it->second->GetTransportedTroops()->GetAt(i))));
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CTransportMenuView::CreateTooltip(void)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	if (m_nActiveTroopInSystem != -1 && CRect(25, 95, 225, 245).PtInRect(pt))
	{
		CPoint p = pDoc->GetKO();
		BYTE id = pDoc->GetSystem(p.x, p.y).GetTroops()->GetAt(m_nActiveTroopInSystem).GetID();
		return pDoc->m_TroopInfo.GetAt(id).GetTooltip();
	}

	if (m_nActiveTroopInShip != -1 && CRect(850, 95, 1050, 245).PtInRect(pt))
	{
		CTroop* pTroop = m_vShipTroops[m_nActiveTroopInShip].second;
		BYTE id = pTroop->GetID();
		return pDoc->m_TroopInfo.GetAt(id).GetTooltip();
	}	

	return "";
}