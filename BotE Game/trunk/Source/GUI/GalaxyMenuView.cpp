// BotEView.cpp : Implementierung der Klasse CGalaxyMenuView
//

#include "stdafx.h"
#include "IOData.h"

#include "BotE.h"
#include "BotEDoc.h"
#include "MainFrm.h"
#include "GalaxyMenuView.h"
#include "SystemMenuView.h"
#include "MenuChooseView.h"
#include "PlanetBottomView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"
#include "Races\RaceController.h"
#include "Galaxy\Anomaly.h"
#include "IniLoader.h"
#include "HTMLStringBuilder.h"
#include "ImageStone/ImageStone.h"
#include "Graphic\memdc.h"
#include "Ships/Ships.h"
#include "General/Loc.h"

BOOLEAN CGalaxyMenuView::m_bDrawTradeRoute = FALSE;
CTradeRoute CGalaxyMenuView::m_TradeRoute;
BOOLEAN CGalaxyMenuView::m_bDrawResourceRoute = FALSE;
CResourceRoute CGalaxyMenuView::m_ResourceRoute;
BOOLEAN CGalaxyMenuView::m_bShipMove = FALSE;
CMajor* CGalaxyMenuView::m_pPlayersRace = NULL;

/////////////////////////////////////////////////////////////////////////////
// CGalaxyMenuView

IMPLEMENT_DYNCREATE(CGalaxyMenuView, CScrollView)

BEGIN_MESSAGE_MAP(CGalaxyMenuView, CScrollView)
	//{{AFX_MSG_MAP(CGalaxyMenuView)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGalaxyMenuView Konstruktion/Destruktion

CGalaxyMenuView::CGalaxyMenuView() :
	m_bScrollToHome(TRUE),
	m_fZoom(STARMAP_ZOOM_INITIAL),
	m_nRange(SM_RANGE_SPACE),
	m_ptViewOrigin(0, 0),
	m_ptOldMousePos(0, 0),
	m_ptScrollToSector(-1, -1)
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	m_pGalaxyGraphic = NULL;
	m_pGalaxyBackground = NULL;
	m_pThumbnail = NULL;
	memset(m_vStars, NULL, sizeof(m_vStars));
}

CGalaxyMenuView::~CGalaxyMenuView()
{
	// Hintergrundbilder freigeben
	if (m_pGalaxyGraphic)
	{
		delete m_pGalaxyGraphic;
		m_pGalaxyGraphic = NULL;
	}
	if (m_pGalaxyBackground)
	{
		delete m_pGalaxyBackground;
		m_pGalaxyBackground = NULL;
	}
	if (m_pThumbnail)
	{
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}

	// Sektormarkierungen freigeben
	for (map<CString, Bitmap*>::iterator it = m_mOwnerMark.begin(); it != m_mOwnerMark.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}
	m_mOwnerMark.clear();

	// Sterne freigeben
	for (int i = 0; i < 7; i++)
	{
		if (m_vStars[i])
		{
			delete m_vStars[i];
			m_vStars[i] = NULL;
		}
	}
}

BOOL CGalaxyMenuView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CScrollView::PreCreateWindow(cs);
}

void CGalaxyMenuView::OnNewRound()
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT_VALID(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	// Bei jeder neuen Runde die Galaxiekarte neu generieren
	GenerateGalaxyMap();

	// und die Starmap mit den Reichweitengrenzen erneuern
	pDoc->GenerateStarmap(pPlayer->GetRaceID());

	m_bShipMove	= FALSE;
	m_bDrawTradeRoute = FALSE;
	m_bDrawResourceRoute = FALSE;
	m_PreviouslyJumpedToShip = RememberedShip();

	if (m_bScrollToHome)
	{
		CPoint pRaceKO = pDoc->GetRaceKO(m_pPlayersRace->GetRaceID());
		pDoc->SetKO(pRaceKO.x, pRaceKO.y);	// damit in der BottomView auch das System angezeigt wird
		ScrollToSector(pRaceKO);
		m_bScrollToHome = false;
	}

	// Ohne diesen Befehl wird die Galaxieansicht zum Beginn einer neuen Runde
	// nicht immer neu gezeichnet -> daher einfach drinlassen
	Invalidate(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CGalaxyMenuView Zeichnen
void CGalaxyMenuView::OnDraw(CDC* dc)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT_VALID(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	if (!m_pGalaxyBackground)
		return;

	if (m_ptScrollToSector != CPoint(-1, -1))
	{
		CenterOnScrollSector();
		m_ptScrollToSector = CPoint(-1, -1);
		Invalidate();
		return;
	}

	// ZU ERLEDIGEN: Hier Code zum Zeichnen der ursprünglichen Daten hinzufügen
	this->SetFocus();

	CRect r;
	r.SetRect(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	// **************** DIE GALAXIEMAP ZEICHNEN ****************************************
	CRect client;
	GetClientRect(&client);

	CMyMemDC pDC(dc);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC.GetSafeHdc());

	g.SetCompositingMode( CompositingModeSourceCopy );
	g.SetCompositingQuality( CompositingQualityHighSpeed );
	g.SetInterpolationMode( InterpolationModeDefault );
	g.SetPixelOffsetMode( PixelOffsetModeNone );
	g.SetSmoothingMode( SmoothingModeNone );
	// Note: TextRenderingHintClearTypeGridFit looks real crap
	//       if we're drawing onto a transparent object
	//       but now we're not... anyhow, this is OK:
	g.SetTextRenderingHint( TextRenderingHintAntiAliasGridFit );

	CPoint pt = GetScrollPosition();

	REAL picWidth  = (REAL)m_pGalaxyBackground->GetWidth() * m_fZoom;
	REAL picHeight = (REAL)m_pGalaxyBackground->GetHeight() * m_fZoom;
	REAL posX = 0.0;
	REAL posY = 0.0;
	if (picWidth < client.right)
		posX = (client.right - picWidth) / 2;
	if (picHeight < client.bottom)
		posY = (client.bottom - picHeight) / 2;

	g.Clear(Color::Black);
	g.DrawImage(m_pGalaxyBackground, posX, posY, picWidth, picHeight);

	CString s;
#ifdef _DEBUG
	s.Format("scrollposition: %.0lf/%.0lf", pt.x / m_fZoom, pt.y / m_fZoom);
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y), &SolidBrush(Color::Aquamarine));
	int nMinRange, nMaxRange;
	GetScrollRange(SB_HORZ, &nMinRange, &nMaxRange);
	s.Format("scrollrange horz: %d/%d", nMinRange, nMaxRange);
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 10), &SolidBrush(Color::Aquamarine));
	GetScrollRange(SB_VERT, &nMinRange, &nMaxRange);
	s.Format("scrollrange vert: %d/%d", nMinRange, nMaxRange);
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 20), &SolidBrush(Color::Aquamarine));
	s.Format("zoomlevel: %.1lf", m_fZoom);
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 30), &SolidBrush(Color::Aquamarine));
	//s.Format("filename: \"test.boj\"");
	//g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 20), &SolidBrush(Color::Aquamarine));
	s.Format("resolution: %d * %d", m_pGalaxyBackground->GetWidth(), m_pGalaxyBackground->GetHeight());
	g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 40), &SolidBrush(Color::Aquamarine));
	//s.Format("dpi: %.1lf * %.1lf", m_pGalaxyBackground->GetHorizontalResolution(), m_pGalaxyBackground->GetVerticalResolution());
	//g.DrawString(CComBSTR(s), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 40), &SolidBrush(Color::Orange));
#endif
	// äußere schwarze Umrandung zeichnen
	//g.DrawRectangle(&Pen(Color::Black, 3.0), posX - 1, posY - 1, picWidth + 2, picHeight + 2);
	bool bShowMiniMap = true;
	CIniLoader::GetInstance()->ReadValue("VIDEO", "SHOWMINIMAP", bShowMiniMap);
	if (bShowMiniMap)
	{
		g.SetCompositingMode( CompositingModeSourceOver );
		REAL yThumbPos = (REAL)((pt.y + client.bottom - m_pThumbnail->GetHeight() - 15));
		// wenn das System in der unteren rechten Hälfte ist, dann wird das Thumbnail oben rechts gezeichnet
		CPoint pRaceKO = pDoc->GetRaceKO(pMajor->GetRaceID());
		if (pRaceKO.x > STARMAP_SECTORS_HCOUNT * 0.75 && pRaceKO.y > STARMAP_SECTORS_VCOUNT * 0.75)
			yThumbPos = (REAL)((pt.y + 15));
		// Position des Thumbnails festlegen
		RectF thumbRect((REAL)((pt.x + client.right - m_pThumbnail->GetWidth() - 15)),
			yThumbPos, (REAL)m_pThumbnail->GetWidth(), (REAL)m_pThumbnail->GetHeight());

		g.DrawImage(m_pThumbnail, thumbRect);
		COLORREF clrColor = pMajor->GetDesign()->m_clrSector;
		Color color(50, GetRValue(clrColor), GetGValue(clrColor), GetBValue(clrColor));

		RectF thumbSelection((REAL)((m_pThumbnail->GetWidth() * pt.x / STARMAP_TOTALWIDTH / m_fZoom + thumbRect.GetLeft())),
			(REAL)((m_pThumbnail->GetHeight() * pt.y / STARMAP_TOTALHEIGHT / m_fZoom + thumbRect.GetTop())),
			(REAL)m_pThumbnail->GetWidth() * client.Width() / STARMAP_TOTALWIDTH / m_fZoom,
			(REAL)m_pThumbnail->GetHeight() * client.Height() / STARMAP_TOTALHEIGHT / m_fZoom);

		if (picWidth < client.right)
			thumbSelection.Width = (REAL)m_pThumbnail->GetWidth();
		if (picHeight < client.bottom)
			thumbSelection.Height = (REAL)m_pThumbnail->GetHeight();
		g.FillRectangle(&SolidBrush(color), thumbSelection);
		color.SetValue(Color::MakeARGB(200, GetRValue(clrColor), GetGValue(clrColor), GetBValue(clrColor)));
		g.DrawRectangle(&Pen(color, 1.0), thumbSelection);
		// weiße Umrandung zeichnen
		g.DrawRectangle(&Pen(Color(200,255,255,255), 1.5), thumbRect.X - 1, thumbRect.Y - 1, thumbRect.Width + 2, thumbRect.Height + 2);
		g.SetCompositingMode( CompositingModeSourceCopy );
	}

//////////////////////////////////////////////////////////////

	// Abbildungsmodus setzen, WindowExt erhält tatsächliche Größe, ViewportExt die gezoomte Größe
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(STARMAP_TOTALWIDTH, STARMAP_TOTALHEIGHT);
	CSize size(STARMAP_TOTALWIDTH, STARMAP_TOTALHEIGHT);
	Zoom(&size);
	pDC->SetViewportExt(size);
	
	if (CIniLoader::GetInstance()->ReadValueDefault("Control", "SHOWSCROLLBARS", true))
		SetScrollSizes(MM_TEXT, size);

	m_ptViewOrigin = CPoint(posX, posY);
	CPoint ptOrg(posX - GetScrollPosition().x, posY - GetScrollPosition().y);
	pDC->SetViewportOrg(ptOrg);
	// die Koordinaten der folgenden Zeichenoperationen werden automatisch gezoomt

	// --- Gitternetztlinien zeichnen ---

	// da nahezu alle Linien sofort richtig gezeichnet werden, tritt auch kein Flimmern auf, wenn nur das Gitter
	// neu gezeichnet wird (m_bUpdateOnly == TRUE); nur teilweises Zeichnen des Gitters bringt selbst auf meinem
	// Rechner nichts mehr, deswegen wird immer das ganze Gitter gezeichnet
	pDC->SetBkMode(TRANSPARENT);
	CPen	gridPen(PS_SOLID, 0, RGB(0,0,0));			// Gitternetz
	CPen	scanPen(PS_DOT, 0, RGB(130,130,130));		// gescanntes Gitternetz
	CPen	nearPen(PS_SOLID, 1, RGB(0, 200, 0)),		// kurze Entfernung
			middlePen(PS_SOLID, 1, RGB(200, 200, 0)),	// mittlere Entfernung
			farPen(PS_SOLID, 1, RGB(200, 0, 0));		// große Entfernung
	CPen	selPen(PS_SOLID, 1, RGB(255,125,0));		// Markierung

	/**/ CPen parentPen(PS_SOLID, 0, RGB(180, 180, 180)); // aufspannender Baum /**/

	// Linie ganz oben und ganz links
	CPen *pOldPen = pDC->SelectObject(&gridPen);
	pDC->MoveTo(STARMAP_TOTALWIDTH, 0); pDC->LineTo(0, 0); pDC->LineTo(0, STARMAP_TOTALHEIGHT);

	// wenn sich die Markierung ganz oben oder ganz links befindet, dann zugehörigen Abschnitt
	// in Linie ganz oben bzw. links in der Farbe der Markierung zeichnen
	CStarmap* pStarmap = pMajor->GetStarmap();
	if (pStarmap->m_Selection.x > -1 && pStarmap->m_Selection.y > -1)
	{
		pDC->SelectObject(&selPen);
		CPoint pos = pStarmap->GetSectorCoords(pStarmap->m_Selection);
		if (pStarmap->m_Selection.y == 0)
		{
			pDC->MoveTo(pos.x, pos.y);
			pDC->LineTo(pos.x + STARMAP_SECTOR_WIDTH, pos.y);
		}
		if (pStarmap->m_Selection.x == 0)
		{
			pDC->MoveTo(pos.x, pos.y);
			pDC->LineTo(pos.x, pos.y + STARMAP_SECTOR_HEIGHT);
		}
	}
	// Wenn wir ein Schiff bewegen wollen
	if (m_bShipMove)
		m_nRange = 3-pDoc->CurrentShip()->second->GetRange(true);
	else
		m_nRange = SM_RANGE_SPACE;

	// für jeden Sektor die Linie rechts und unten zeichnen; (j, i) ist der aktuell betrachtete Sektor,
	// (x, y) sind die Koordinaten dessen linker oberer Ecke
	for (int i = 0, y = 0; i < STARMAP_SECTORS_VCOUNT; i++, y += STARMAP_SECTOR_HEIGHT)
		for (int j = 0, x = 0; j < STARMAP_SECTORS_HCOUNT; j++, x += STARMAP_SECTOR_WIDTH)
		{
			/* // nur für Debug-Zwecke
			if (m_nRange)
			{
				// aufspannenden Baum zeichnen
				pDC->SelectObject(&parentPen);
				struct::Sector *parent = &pMajor->GetStarmap()->pathMap[j][i].parent;
				if (parent->x > -1 && parent->y > -1)
				{
					ASSERT_SECTOR_VALID(*parent);
					CPoint pt = pMajor->GetStarmap()->GetSectorCoords(*parent);
					pt += CPoint(STARMAP_SECTOR_WIDTH / 2, STARMAP_SECTOR_WIDTH / 2);

					pDC->MoveTo(x + STARMAP_SECTOR_WIDTH / 2, y + STARMAP_SECTOR_WIDTH / 2); pDC->LineTo(pt);
				}
			}
			/**/

			// rechter Rand
			if (pStarmap->m_Selection.y == i && (pStarmap->m_Selection.x == j || pStarmap->m_Selection.x == j + 1))
				// Markierung
				pDC->SelectObject(&selPen);
			else if (j < STARMAP_SECTORS_HCOUNT - 1)
			{
				// normales Gitternetz laden
				pDC->SelectObject(&gridPen);
				// besitzt der aktuelle Sektor oder der Sektor rechts daneben einen Scanpower, so wird
				// der Scangrid geladen
				if (pDoc->GetSector(j, i).GetScanPower(pMajor->GetRaceID()) > 0 && pDoc->GetSector(j + 1, i).GetScanPower(pMajor->GetRaceID()) > 0)
					pDC->SelectObject(&scanPen);

				// Grenze der Reichweite;
				// muss modifiziert werden, wenn sich z.B. neben RANGE_NEAR ein Feld mit RANGE_FAR
				// statt MIDDLE befindet (kann bei entsprechender Rangemap um eine Starbase
				// auftreten, oder wenn sich das eigene Gebiet mit dem anderer Spieler überlappt)
				int border = GetRangeBorder(pStarmap->m_Range[j][i], pStarmap->m_Range[j + 1][i], m_nRange);
				// bei Auswahl eines Ziels nur die jeweilige Grenze zeichnen
				if (!m_nRange || border == m_nRange)
				{
					switch (border)
					{
					case SM_RANGE_FAR:		pDC->SelectObject(&farPen); break;
					case SM_RANGE_MIDDLE:	pDC->SelectObject(&middlePen); break;
					case SM_RANGE_NEAR:		pDC->SelectObject(&nearPen); break;
					}
				}
			}
			else
				// Linien ganz rechts
				pDC->SelectObject(&gridPen);

			// zeichnen
			// es ist auskommentiert, dass das Grid nicht auf unbekannten Sektoren gezeichnet wird
			if (pStarmap->GetRange(CPoint(j,i)) != 3 && pDoc->GetSector(j,i).GetScanned(pMajor->GetRaceID()) == TRUE || (pDC->GetCurrentPen() != &gridPen && pDC->GetCurrentPen() != &scanPen))
			{
				pDC->MoveTo(x + STARMAP_SECTOR_WIDTH, y);
				pDC->LineTo(x + STARMAP_SECTOR_WIDTH, y + STARMAP_SECTOR_HEIGHT);
			}
			else
				pDC->MoveTo(x + STARMAP_SECTOR_WIDTH, y + STARMAP_SECTOR_HEIGHT);

			// unterer Rand
			if (pStarmap->m_Selection.x == j && (pStarmap->m_Selection.y == i
			  || pStarmap->m_Selection.y == i + 1))
				// Markierung
				pDC->SelectObject(&selPen);
			else if (i < STARMAP_SECTORS_VCOUNT - 1)
			{
				// normales Gitternetz laden
				pDC->SelectObject(&gridPen);
				// besitzt der aktuelle Sektor oder der Sektor darunter eine Scanpower, so wird
				// der Scangrid geladen
				if (pDoc->GetSector(j, i).GetScanPower(pMajor->GetRaceID()) > 0 && pDoc->GetSector(j, i + 1).GetScanPower(pMajor->GetRaceID()) > 0)
					pDC->SelectObject(&scanPen);
				// Grenze der Reichweite
				int border = GetRangeBorder(pStarmap->m_Range[j][i], pStarmap->m_Range[j][i + 1], m_nRange);
				// bei Auswahl eines Ziels nur die jeweilige Grenze zeichnen
				if (!m_nRange || border == m_nRange)
				{
					switch (border)
					{
					case SM_RANGE_FAR:		pDC->SelectObject(&farPen); break;
					case SM_RANGE_MIDDLE:	pDC->SelectObject(&middlePen); break;
					case SM_RANGE_NEAR:		pDC->SelectObject(&nearPen); break;
					}
				}
			}
			else
				// Linien ganz unten
				pDC->SelectObject(&gridPen);

			// zeichnen
			// es ist auskommentiert, dass das Grid nicht auf unbekannten Sektoren gezeichnet wird
			if (pStarmap->GetRange(CPoint(j,i)) != 3 && pDoc->GetSector(j, i).GetScanned(pMajor->GetRaceID()) == TRUE || (pDC->GetCurrentPen() != &gridPen && pDC->GetCurrentPen() != &scanPen))
				pDC->LineTo(x, y + STARMAP_SECTOR_HEIGHT);
		}

#ifdef DEBUG_AI_BASE_DEMO
	pDoc->starmap[pDoc->GetPlayersRace()]->RecalcRangePoints();
	pDoc->starmap[pDoc->GetPlayersRace()]->RecalcConnectionPoints();
	pDoc->starmap[pDoc->GetPlayersRace()]->RecalcTargetPoints();
	pDoc->starmap[pDoc->GetPlayersRace()]->SetBadAIBaseSectors(pDoc->m_Sector, pDoc->GetPlayersRace());
	for (int j = 0; j < STARMAP_SECTORS_VCOUNT; j++)
		for (int i = 0; i < STARMAP_SECTORS_HCOUNT; i++)
		{
			UINT ta = pDC->GetTextAlign();
			pDC->SetTextAlign(TA_CENTER | TA_BASELINE);
			Sector sector(i, j);
			short points = pDoc->starmap[pDoc->GetPlayersRace()]->GetPoints(sector);
			if (points != 0)
			{
				CPoint pt = pDoc->m_pStarmap->GetSectorCoords(sector);
				CString s;
				s.Format("%d", points);
				pDC->TextOut(pt.x + STARMAP_SECTOR_WIDTH / 2, pt.y + STARMAP_SECTOR_HEIGHT / 2 + 6, s);
			}
			pDC->SetTextAlign(ta);
		}
#endif

	// Flugrouten der Schiffe ermitteln
	set<pair<int, int> > setPathKOs;
	for (CShipMap::const_iterator ship = pDoc->m_ShipMap.begin(); ship != pDoc->m_ShipMap.end(); ++ship)
	{
		if (ship->second->GetOwnerOfShip() != pMajor->GetRaceID())
			continue;

		if (m_nRange && ship == pDoc->CurrentShip())
			continue;

		if (!ship->second->HasTarget())
			continue;

		// (Performance) nur neu berechnen, wenn Ziel und letzter Pfadeintrag nicht identisch
		if (ship->second->GetPath()->IsEmpty() || ship->second->GetPath()->GetAt(ship->second->GetPath()->GetUpperBound()) != Sector(ship->second->GetTargetKO()))
		{
			char speed = (char)(ship->second->GetSpeed(ship->second->HasFleet()));
			char range = (char)(ship->second->GetRange(ship->second->HasFleet()));
			pMajor->GetStarmap()->CalcPath(ship->second->GetKO(), ship->second->GetTargetKO(), 3 - range, speed, *ship->second->GetPath());
		}

		const CArray<Sector>* pPath = ship->second->GetPath();
		for (int i = 0; i < pPath->GetSize(); i++)
			setPathKOs.insert(make_pair(pPath->GetAt(i).x, pPath->GetAt(i).y));
	}

	// Flugrouten zeichnen
	if (!setPathKOs.empty())
	{
		COLORREF color = RGB(255,255,255);
		CBrush brush(color);
		CPen pen(PS_SOLID, 0, color);
		pDC->SelectObject(&brush);
		pDC->SelectObject(&pen);
		for (set<pair<int, int> >::const_iterator it = setPathKOs.begin(); it != setPathKOs.end(); ++it)
		{
			CPoint pt = pMajor->GetStarmap()->GetSectorCoords(Sector(it->first, it->second));
			pDC->Ellipse(pt.x+STARMAP_SECTOR_WIDTH/2-4,pt.y+STARMAP_SECTOR_HEIGHT/2-4,pt.x+STARMAP_SECTOR_WIDTH/2+4,pt.y+STARMAP_SECTOR_HEIGHT/2+4);
		}
	}

	// wenn gerade ein Ziel gewählt wird, den berechneten Weg zeichnen
	if (m_nRange && pDoc->CurrentShip()->second->GetPath()->GetSize())
	{
		// Vorlage mit jedem Sektor des Weges verknüpfen
		for (int i = 0; i < pDoc->CurrentShip()->second->GetPath()->GetSize(); i++)
		{
			COLORREF color = pMajor->GetDesign()->m_clrRouteColor;
			CBrush brush(color);
			CPen pen(PS_SOLID , 0, color);
			pDC->SelectObject(&brush);
			pDC->SelectObject(&pen);
			assert(pDoc->CurrentShip()->second->GetPath()->GetAt(i).on_map());
			CPoint pt = pMajor->GetStarmap()->GetSectorCoords(pDoc->CurrentShip()->second->GetPath()->GetAt(i));
			pDC->Ellipse(pt.x+STARMAP_SECTOR_WIDTH/2-4,pt.y+STARMAP_SECTOR_HEIGHT/2-4,pt.x+STARMAP_SECTOR_WIDTH/2+4,pt.y+STARMAP_SECTOR_HEIGHT/2+4);
		}

		// Anzahl der benötigten Runden in letztes Feld des Weges zeichnen
		CPoint ptLast = pMajor->GetStarmap()->GetSectorCoords(pDoc->CurrentShip()->second->GetPath()->GetAt(pDoc->CurrentShip()->second->GetPath()->GetUpperBound()));
		CString s;
		s.Format("%.0f",ceil((float)pDoc->CurrentShip()->second->GetPath()->GetSize() / (float)pDoc->CurrentShip()->second->GetSpeed(pDoc->CurrentShip()->second->HasFleet())));
		pDC->SetTextColor(RGB(255,255,255));
		pDC->TextOut(ptLast.x+STARMAP_SECTOR_WIDTH/2+6, ptLast.y+STARMAP_SECTOR_HEIGHT/2-8, s);
	}

	pDC->SelectObject(pOldPen);

	// Font für Sektornamen holen
	CFont font;
	CFontLoader::CreateFont(m_pPlayersRace, 0, &font);
	CFont* oldfont = pDC->SelectObject(&font);
	bool bShowTraderoutes;
	CIniLoader::GetInstance()->ReadValue("Video", "SHOWTRADEROUTES", bShowTraderoutes);
	// Namen des Systems und Handelsrouten zeichnen, nur wenn komplett neu gezeichnet wurde
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			pDoc->GetSector(x,y).DrawSectorsName(pDC ,pDoc, m_pPlayersRace);
			// eigene Handelsrouten zeichnen
			if (bShowTraderoutes)
				if (pDoc->GetSystem(x, y).GetOwnerOfSystem() == pMajor->GetRaceID())
				{
					for (int i = 0; i < pDoc->GetSystem(x, y).GetTradeRoutes()->GetSize(); i++)
						pDoc->GetSystem(x, y).GetTradeRoutes()->GetAt(i).DrawTradeRoute(pDC, CPoint(x,y), pMajor);
					for (int i = 0; i < pDoc->GetSystem(x, y).GetResourceRoutes()->GetSize(); i++)
						pDoc->GetSystem(x, y).GetResourceRoutes()->GetAt(i).DrawResourceRoute(pDC, CPoint(x,y), pMajor);
				}
		}
	pDC->SelectObject(oldfont);
	font.DeleteObject();

	if (bShowTraderoutes)
	{
		if (m_bDrawTradeRoute && (pDoc->CurrentSystem().GetOwnerOfSystem() == pMajor->GetRaceID()))
			m_TradeRoute.DrawTradeRoute(pDC, pDoc->GetKO(), pMajor);
		if (m_bDrawResourceRoute && (pDoc->CurrentSystem().GetOwnerOfSystem() == pMajor->GetRaceID()))
			m_ResourceRoute.DrawResourceRoute(pDC, pDoc->GetKO(), pMajor);
	}

	pOldPen->DeleteObject();

	g.ReleaseHDC(pDC.GetSafeHdc());
}

void CGalaxyMenuView::OnInitialUpdate()
{
	// ZU ERLEDIGEN: Gesamte Größe dieser Ansicht berechnen

	// Cursor aus Ressource laden
    HCURSOR m_hCur = AfxGetApp()->LoadCursor(IDC_CURSOR);
	// Geladenen Cursor nun setzen
    ::SetCursor(m_hCur);
    // Sehr wichtig, sonst wird Cursor sofort wieder zurueckgesetzt!
    ::SetClassLong(*this,GCL_HCURSOR,(LONG)m_hCur);

	// Neu mit Tonis Programmierung
	m_fZoom = STARMAP_ZOOM_INITIAL;
	m_nRange = SM_RANGE_SPACE;

	m_TotalSize = CSize(1075, 750);
	SetScrollSizes(MM_TEXT, m_TotalSize);

	m_bShipMove	= FALSE;
	m_bScrollToHome = TRUE;

	resources::pMainFrame->AddToTooltip(this);

	CScrollView::OnInitialUpdate();
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CGalaxyMenuView::LoadRaceGraphics()
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CString sAppPath = CIOData::GetInstance()->GetAppPath();
	CString prefix = pMajor->GetPrefix();
	CString filePath = sAppPath + "Graphics\\Galaxies\\" + prefix + "galaxy.boj";

	// Galaxiegrafik laden
	m_pGalaxyGraphic = Bitmap::FromFile(CComBSTR(filePath));
	if (m_pGalaxyGraphic->GetLastStatus() != Ok)
	{
		delete m_pGalaxyGraphic;
		m_pGalaxyGraphic = NULL;
		MYTRACE("graphicload")(MT::LEVEL_WARNING, "CGalaxyMenuView::GenerateGalaxy(): Could not load galaxy background");
		AfxMessageBox("Could not load galaxy background\n\n" + filePath);
		return;
	}

	// Grafik zuschneiden
	FCObjImage img;
	FCWin32::GDIPlus_LoadBitmap(*m_pGalaxyGraphic, img);
	// Bild entsprechend vergrößern. Dabei Seitenverhältnisse beibehalten
	if (STARMAP_TOTALWIDTH-img.Width()*STARMAP_TOTALHEIGHT/img.Height()<0)
		img.Stretch_Smooth(img.Width()*STARMAP_TOTALHEIGHT/img.Height(), STARMAP_TOTALHEIGHT);
	else
		img.Stretch_Smooth(STARMAP_TOTALWIDTH, img.Height()*STARMAP_TOTALWIDTH/img.Width());

	assert(m_pGalaxyGraphic && m_pGalaxyGraphic->GetLastStatus() == Ok);
	delete m_pGalaxyGraphic;
	m_pGalaxyGraphic = FCWin32::GDIPlus_CreateBitmap(img);

	//////////////////////////////////////////////////////////////////////////
	// Grafiken für Sektormarkierungen dynamisch generieren
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		Color clr;
		clr.SetFromCOLORREF(it->second->GetDesign()->m_clrSector);
		Color color(100, clr.GetR(), clr.GetG(), clr.GetB());

		// hier wurde der R Wert mit dem B Wert getauscht, da die Funktion SetPixelData sonst nicht stimmt.
		// color = RGB(GetBValue(color), GetGValue(color), GetRValue(color));

		// Bitmap generieren
		m_mOwnerMark[it->first] = new Gdiplus::Bitmap(STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT, PixelFormat32bppPARGB);
		if (m_mOwnerMark[it->first])
			for (int y = 0; y < STARMAP_SECTOR_HEIGHT; y++)
				for (int x = 0; x < STARMAP_SECTOR_WIDTH; x++)
					m_mOwnerMark[it->first]->SetPixel(x, y, color);
	}

	// Farbe für Minors hinzufügen
	CString sMinorID = "__MINOR__";
	m_mOwnerMark[sMinorID] = new Bitmap(STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT, PixelFormat32bppPARGB);
	if (m_mOwnerMark[sMinorID])
		for (int y = 0; y < STARMAP_SECTOR_HEIGHT; y++)
			for (int x = 0; x < STARMAP_SECTOR_WIDTH; x++)
				m_mOwnerMark[sMinorID]->SetPixel(x, y, Color(100,200,200,200));

	// Farbe für Nebel des Krieges hinzuzufügen
	CString sFogOfWarID = "__FOG_OF_WAR__";
	m_mOwnerMark[sFogOfWarID] = new Bitmap(STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT, PixelFormat32bppPARGB);
	if (m_mOwnerMark[sFogOfWarID])
		for (int y = 0; y < STARMAP_SECTOR_HEIGHT; y++)
			for (int x = 0; x < STARMAP_SECTOR_WIDTH; x++)
				m_mOwnerMark[sFogOfWarID]->SetPixel(x, y, Color(160,0,0,0));

	//////////////////////////////////////////////////////////////////////////
	// Sonnengrafiken einmalig laden
	filePath = sAppPath + "Graphics\\MapStars\\star_blue.bop";
	m_vStars[0] = Bitmap::FromFile(CComBSTR(filePath));
	filePath = sAppPath + "Graphics\\MapStars\\star_green.bop";
	m_vStars[1] = Bitmap::FromFile(CComBSTR(filePath));
	filePath = sAppPath + "Graphics\\MapStars\\star_orange.bop";
	m_vStars[2] = Bitmap::FromFile(CComBSTR(filePath));
	filePath = sAppPath + "Graphics\\MapStars\\star_red.bop";
	m_vStars[3] = Bitmap::FromFile(CComBSTR(filePath));
	filePath = sAppPath + "Graphics\\MapStars\\star_violet.bop";
	m_vStars[4] = Bitmap::FromFile(CComBSTR(filePath));
	filePath = sAppPath + "Graphics\\MapStars\\star_white.bop";
	m_vStars[5] = Bitmap::FromFile(CComBSTR(filePath));
	filePath = sAppPath + "Graphics\\MapStars\\star_yellow.bop";
	m_vStars[6] = Bitmap::FromFile(CComBSTR(filePath));

	if (MT::CMyTrace::IsLoggingEnabledFor("graphicload"))
	{
		for (int i = 0; i < 7; i++)
			if (!m_vStars[i] || m_vStars[i]->GetLastStatus() != Ok)
				MYTRACE_CHECKED("graphicload")(MT::LEVEL_WARNING, "CGalaxyMenuView::GenerateGalaxy(): Could not load a star graphic");
	}
}

BOOL CGalaxyMenuView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// TODO: Add your specialized code here and/or call the base class
	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}

BOOL CGalaxyMenuView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	// TODO: Add your specialized code here and/or call the base class
	CRect client;
	GetClientRect(&client);
	InvalidateRect(&client, FALSE);
	return CScrollView::OnScrollBy(sizeScroll, bDoScroll);
}

void CGalaxyMenuView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class

	// ScrollSizes auf Gesamtgröße inkl. Zoom setzen
	CSize size;
	size.cx = (LONG)(STARMAP_TOTALWIDTH * m_fZoom);
	size.cy = (LONG)(STARMAP_TOTALHEIGHT * m_fZoom);
	bool bShowScrollBars = false;
	CIniLoader::GetInstance()->ReadValue("Control", "SHOWSCROLLBARS", bShowScrollBars);
	if (bShowScrollBars)
	{
		Invalidate(FALSE);	// komisches flackern verhindern
		SetScrollSizes(MM_TEXT, size);
	}
	else
	{
		SetScrollRange(SB_HORZ, 0, size.cx, false);
		SetScrollRange(SB_VERT, 0, size.cy, false);
		ShowScrollBar(SB_BOTH, SW_HIDE);
	}
}

void CGalaxyMenuView::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	//CScrollView::OnSize(nType, cx, cy);

	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein.
}
/////////////////////////////////////////////////////////////////////////////
// CGalaxyMenuView Drucken

BOOL CGalaxyMenuView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CGalaxyMenuView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CGalaxyMenuView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Hier Bereinigungsarbeiten nach dem Drucken einfügen
}

/////////////////////////////////////////////////////////////////////////////
// CGalaxyMenuView Diagnose

#ifdef _DEBUG
void CGalaxyMenuView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGalaxyMenuView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGalaxyMenuView Nachrichten-Handler
void CGalaxyMenuView::Zoom(CSize *pSize) const
{
	if (!pSize) return;

	pSize->cx = (long)(pSize->cx * m_fZoom);
	pSize->cy = (long)(pSize->cy * m_fZoom);
}

void CGalaxyMenuView::UnZoom(CSize *pSize) const
{
	if (!pSize) return;

	pSize->cx = (long)(pSize->cx / m_fZoom);
	pSize->cy = (long)(pSize->cy / m_fZoom);
}

void CGalaxyMenuView::Zoom(CPoint *pPoint) const
{
	if (!pPoint) return;

	pPoint->x = (long)(pPoint->x * m_fZoom);
	pPoint->y = (long)(pPoint->y * m_fZoom);
}

void CGalaxyMenuView::UnZoom(CPoint *pPoint) const
{
	if (!pPoint) return;

	pPoint->x = (long)(pPoint->x / m_fZoom);
	pPoint->y = (long)(pPoint->y / m_fZoom);
}

void CGalaxyMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Das hier alles nur machen, wenn wir in der Galaxiemap-Ansicht sind
	// Mauskoordinaten in ungezoomte Koordinaten der Starmap umrechnen
	CPoint pt(point);
	m_ptOldMousePos = pt;
	pt += GetScrollPosition() - m_ptViewOrigin;
	UnZoom(&pt);


	if (!m_nRange)
	{
		// angeklickten Sektor ermitteln, anschließend markieren
		struct::Sector sector = pMajor->GetStarmap()->GetClickedSector(pt);
		CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_ICON);
		resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));

		if (sector != struct::Sector(-1,-1) && !m_bShipMove && m_bDrawTradeRoute == FALSE && m_bDrawResourceRoute == FALSE)
		{
			pDoc->SetKO(sector.x,sector.y);
			m_oldSelection = pMajor->GetStarmap()->GetSelection();
			pMajor->GetStarmap()->Select(sector);

			// Wenn in dem Sektor ein Schiff ist und wir den Punkt in der oberen rechten Ecke anklicken,
			// dann sollen die Schiffe in der View3 angezeigt werden
			CPoint modulo(pt.x%STARMAP_SECTOR_WIDTH, pt.y%STARMAP_SECTOR_HEIGHT);
			if (modulo.x > STARMAP_SECTOR_WIDTH * 0.66 && modulo.y < STARMAP_SECTOR_HEIGHT * 0.33)
			{
				const std::map<CString, CRace*>* pmRaces = pDoc->GetRaceCtrl()->GetRaces();
				const CSector& s = pDoc->GetSector(sector.x, sector.y);
				for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
					if (s.ShouldDrawShip(*pMajor, it->first))
						{
							if(it->first == pMajor->GetRaceID()) {
								for(CShipMap::const_iterator i = pDoc->m_ShipMap.begin();
										i != pDoc->m_ShipMap.end(); ++i) {
									const CPoint& point = i->second->GetKO();
									if(sector == Sector(point.x, point.y)) {
										m_PreviouslyJumpedToShip = RememberedShip(
											i->second->GetShipName(), i->first);
										break;
									}
								}
							}
							CShipBottomView::SetShowStation(false);
							resources::pMainFrame->SelectBottomView(SHIP_BOTTOM_VIEW);
							resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CShipBottomView));
							break;
						}
			}
			// Wenn in dem Sektor eine Station steht oder noch gebaut wird, dann Stationsansicht in View3 zeigen,
			// wenn wir unten rechts hingeklickt haben
			else if (modulo.x < STARMAP_SECTOR_WIDTH * 0.33 && modulo.y > STARMAP_SECTOR_HEIGHT * 0.66)
			{
				bool bShowStation = false;
				const CSector& s = pDoc->GetSector(sector.x, sector.y);
				const std::map<CString, CRace*>* pmRaces = pDoc->GetRaceCtrl()->GetRaces();
				for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); ++it)
					if (s.ShouldDrawOutpost(*pMajor, it->first))
					{
						bShowStation = true;
						break;
					}

				// Station anzeigen?
				if (bShowStation)
				{
					CShipBottomView::SetShowStation(true);
					resources::pMainFrame->SelectBottomView(SHIP_BOTTOM_VIEW);
					resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CShipBottomView));
				}
			}
			else
			{
				// Planetenansicht in View3 des angeklickten Sektors zeigen
				resources::pMainFrame->SelectBottomView(PLANET_BOTTOM_VIEW);
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
			}

			// View muss neu gezeichnet werden
			Invalidate();
		}
		// Wenn wir eine Handelsroute festlegen wollen
		else if (sector != struct::Sector(-1,-1) && m_bDrawTradeRoute == TRUE)
		{
			CPoint p = pDoc->GetKO();
			BYTE numberOfRoutes = pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetSize();
			// konnten erfolgreich eine hinzufügen aufgrund der Bevölkerung
			if (pDoc->GetSector(sector.x, sector.y).GetSunSystem() == TRUE && pDoc->GetSystem(p.x, p.y).AddTradeRoute(CPoint(sector.x,sector.y), pDoc->m_Systems, pMajor->GetEmpire()->GetResearch()->GetResearchInfo()))
			{
				// wurde keine hinzugefügt, dann fertig
				if (numberOfRoutes == pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetSize())
				{
					m_bDrawTradeRoute = FALSE;
					Invalidate();
				}
				// jetzt diplomatische Beziehung checken
				else if (pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetAt(pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetUpperBound()).CheckTradeRoute(p, CPoint(sector.x, sector.y), pDoc))
				{
					// wenn wir noch weitere Handelsrouten hinzufügen können, dann in der Ansicht bleiben
					if (pDoc->GetSystem(p.x, p.y).CanAddTradeRoute(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()))
						m_bDrawTradeRoute = TRUE;
					else
						m_bDrawTradeRoute = FALSE;
					// Anzeige gleich aktualisieren
					pDoc->GetSystem(p.x, p.y).CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),
						pDoc->GetSector(p.x, p.y).GetPlanets(), pMajor, CTrade::GetMonopolOwner());
					Invalidate();
				}
				// konnten wie die Handelsroute aufgrund der diploamtischen Beziehungen nicht hinzufügen, so
				// müssen wir sie gleich wieder löschen
				else
				{
					pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->RemoveAt(pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetUpperBound());
				}
			}
			// Wenn wir eine Handelsroute gelöscht haben, dann aktualisieren
			else if (numberOfRoutes > pDoc->GetSystem(p.x, p.y).GetTradeRoutes()->GetSize())
				pDoc->GetSystem(p.x, p.y).CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),
				pDoc->GetSector(p.x, p.y).GetPlanets(), pMajor, CTrade::GetMonopolOwner());
		}
		// Wenn wir eine Ressourcenroute festlegen wollen
		else if (sector != struct::Sector(-1,-1) && m_bDrawResourceRoute == TRUE)
		{
			CPoint p = pDoc->GetKO();
			// konnten erfolgreich eine hinzufügen aufgrund der Bevölkerung
			if (pDoc->GetSector(sector.x, sector.y).GetSunSystem() == TRUE && p != CPoint(sector.x,sector.y) &&
				pDoc->GetSystem(p.x, p.y).AddResourceRoute(CPoint(sector.x,sector.y), CSystemMenuView::GetResourceRouteRes(), pDoc->m_Systems, pMajor->GetEmpire()->GetResearch()->GetResearchInfo()))
			{
				m_bDrawResourceRoute = FALSE;
				// Anzeige gleich aktualisieren
				Invalidate();
			}
		}
	}
	else
	{
		if (pMajor->GetStarmap()->GetClickedSector(pt) == struct::Sector(-1,-1))
			return;
		CPoint target(pMajor->GetStarmap()->GetClickedSector(pt).x,pMajor->GetStarmap()->GetClickedSector(pt).y);
		// Wenn wir ein Schiff bewegen wollen und der Kurs größer als eins ist, dann neues Ziel setzen
		// Oder wir brechen den Kurs ab, indem wir auf den aktuellen Sektor klicken
		if (m_bShipMove && (pDoc->CurrentShip()->second->GetPath()->GetSize()
			|| target == pDoc->CurrentShip()->second->GetKO()))
		{
			const CShipMap::iterator& ship = pDoc->CurrentShip();
			ship->second->SetTargetKO(target == ship->second->GetKO() ? CPoint(-1, -1) : target);
			CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
			SetMoveShip(FALSE);
			resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			m_nRange = 0;
			CSoundManager::GetInstance()->PlaySound(SNDMGR_SOUND_SHIPTARGET);
			Invalidate();
		}
		else
		{
			m_nRange = 0;
			Invalidate();
		}
	}

	CScrollView::OnLButtonDown(nFlags, point);
}

void CGalaxyMenuView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	ReleaseCapture();

	CScrollView::OnLButtonUp(nFlags, point);
}


BOOL CGalaxyMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	//return CScrollView::OnEraseBkgnd(pDC);
	return FALSE;
}

BOOL CGalaxyMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return CScrollView::OnMouseWheel(nFlags, zDelta, point);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return CScrollView::OnMouseWheel(nFlags, zDelta, point);

	// Das hier alles nur machen, wenn wir in der Galaxiemap-Ansicht sind
	// bekommen in point Bildschirmkoordinaten; erstmal in Client-Koordinaten umrechnen
	ScreenToClient(&point);

	// Sektor unter dem Mauscursor ermitteln
	CPoint pt(point);
	pt += GetScrollPosition() - m_ptViewOrigin;
	UnZoom(&pt);
	struct::Sector sector = pMajor->GetStarmap()->GetClickedSector(pt);
	// kein Zoom durchführen, wenn außerhalb des Galaxiegegrenzen gezoomt wurde
	if (sector.x == -1 || sector.y == -1)
		return CScrollView::OnMouseWheel(nFlags, zDelta, point);

	// alten Zoom-Faktor merken
	double oldZoom = m_fZoom;

	// Zoom-Faktor ändern (funktioniert auch bei hochauflösenden Scroll-Rädern (gibt's die schon?))
	m_fZoom += (double)zDelta * STARMAP_ZOOM_STEP / WHEEL_DELTA;
	// wenn außerhalb der vorgegebenen Grenzen, dann auf die jeweilige Grenze setzen
	if (m_fZoom < STARMAP_ZOOM_MIN)
		m_fZoom = STARMAP_ZOOM_MIN;
	if (m_fZoom > STARMAP_ZOOM_MAX)
		m_fZoom = STARMAP_ZOOM_MAX;

	// wenn sich der Zoom-Faktor geändert hat
	if (m_fZoom != oldZoom)
	{
		// ScrollPosition so anpassen, dass sich nach dem Zoomen wieder der Sektor unter dem Mauscursor
		// befindet, der vor dem Zoomen schon unter der Maus war

		// Anpassen der ScrollSizes
		OnUpdate(this, 0, NULL);

		// (neue) gezoomte Koordinaten des Sektors (Mittelpunkt) ermitteln
		CPoint pt = pMajor->GetStarmap()->GetSectorCoords(sector);
		pt += CPoint(STARMAP_SECTOR_WIDTH >> 1, STARMAP_SECTOR_HEIGHT >> 1);
		Zoom(&pt);

		// ScrollPosition ermitteln
		pt -= point;

		// Verschiebung so anpassen, dass die Starmap nicht mitten in der View endet
		// @TODO Sind die Größe des Fensters und der minimale Zoom-Faktor so abgestimmt, dass die gezoomte
		// Starmap nie kleiner als das Fenster wird, sind diese Schritte überflüssig; später evtl. entfernen
		CRect client;
		GetClientRect(&client);

		CPoint ptmax(STARMAP_TOTALWIDTH, STARMAP_TOTALHEIGHT);
		Zoom(&ptmax);
		ptmax -= client.BottomRight();

		if (pt.x > ptmax.x) pt.x = ptmax.x;
		if (pt.y > ptmax.y) pt.y = ptmax.y;
		if (pt.x < 0) pt.x = 0;
		if (pt.y < 0) pt.y = 0;
		// @TODO END //

		// ScrollPosition setzen
		SetScrollPos(SB_HORZ, pt.x, false);
		SetScrollPos(SB_VERT, pt.y, false);
		Invalidate(TRUE);
	}

	return CScrollView::OnMouseWheel(nFlags, zDelta, point);
}

void CGalaxyMenuView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Wenn wir uns in der Galaxieansicht befinden
	CPoint pt(point);
	pt += GetScrollPosition() - m_ptViewOrigin;
	UnZoom(&pt);

	if (!m_nRange)
	{
		// angeklickten Sektor ermitteln
		struct::Sector sector = pMajor->GetStarmap()->GetClickedSector(pt);
		const CCommandLineParameters* const clp = resources::pClp;
		if (PT_IN_RECT(sector, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT) &&
			(clp->SeeAllOfMap() || pDoc->GetSystem(sector.x, sector.y).GetOwnerOfSystem() == pMajor->GetRaceID()) &&
			pDoc->GetSector(sector.x, sector.y).GetSunSystem() == TRUE)
			{
				// falls ein Schiff markiert war wird dieses abgewählt
				SetMoveShip(FALSE);
				pDoc->SetKO(sector.x,sector.y);
				resources::pMainFrame->SelectMainView(SYSTEM_VIEW, pMajor->GetRaceID());
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CGalaxyMenuView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	//Wenn wir das Befehlgeben eines Schiffes abbrechen wollen
	if (m_bShipMove)
	{
		SetMoveShip(FALSE);
		CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
		resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		m_nRange = 0;
		Invalidate();
		return;
	}
	// Wenn wir gerade eine Handelsroute ziehen, können wir mit einem Rechtsklick dies abbrechen
	else if (m_bDrawTradeRoute)
	{
		m_bDrawTradeRoute = FALSE;
		Invalidate();
	}
	// Wenn wir gerade eine Ressourcenroute ziehen, können wir mit einem Rechtsklick dies abbrechen
	else if (m_bDrawResourceRoute)
	{
		m_bDrawResourceRoute = FALSE;
		Invalidate();
	}

	CScrollView::OnRButtonDown(nFlags, point);
}

void CGalaxyMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	// Wenn wir in der Galaxieansicht sind und Schiffe bewegen wollen
	if (m_bShipMove && m_nRange)
	{
		CMajor* pMajor = m_pPlayersRace;
		ASSERT(pMajor);
		if (!pMajor)
			return;

		// ist irgendwas ausgewählt
		if (pMajor->GetStarmap()->GetSelection() == Sector(-1,-1))
			return;

		// Mauskoordinaten in ungezoomte Koordinaten der Starmap umrechnen
		CPoint pt(point);
		pt += GetScrollPosition() - m_ptViewOrigin;
		UnZoom(&pt);

		// Sektor, über dem sich die Maus befindet, ermitteln
		struct::Sector target = pMajor->GetStarmap()->GetClickedSector(pt);
		struct::Sector shipKO(pDoc->CurrentShip()->second->GetKO().x,pDoc->CurrentShip()->second->GetKO().y);

		if (PT_IN_RECT(target, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
		{
			// Weg berechnen, neu zeichnen
			char speed = (char)(pDoc->CurrentShip()->second->GetSpeed(pDoc->CurrentShip()->second->HasFleet()));
			struct::Sector result = pMajor->GetStarmap()->CalcPath(pMajor->GetStarmap()->GetSelection(), target, m_nRange, speed, *pDoc->CurrentShip()->second->GetPath());

			if (target != shipKO && target != oldtarget)
				Invalidate();

			oldtarget = target;
		}
	}
	// Wenn wir in der Galaxieansicht sind und eine Handelsroute ziehen wollen
	else if (m_bDrawTradeRoute == TRUE)
	{
		CMajor* pMajor = m_pPlayersRace;
		ASSERT(pMajor);
		if (!pMajor)
			return;

		// Mauskoordinaten in ungezoomte Koordinaten der Starmap umrechnen
		CPoint pt(point);
		pt += GetScrollPosition() - m_ptViewOrigin;
		UnZoom(&pt);

		// Sektor, über dem sich die Maus befindet, ermitteln
		struct::Sector target = pMajor->GetStarmap()->GetClickedSector(pt);
		if (target != oldtarget && PT_IN_RECT(target, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
		{
			m_TradeRoute.GenerateTradeRoute(CPoint(target.x,target.y));
			Invalidate();
			oldtarget = target;
		}
	}
	// Wenn wir in der Galaxieansicht sind und eine Ressourcenroute ziehen wollen
	else if (m_bDrawResourceRoute == TRUE)
	{
		CMajor* pMajor = m_pPlayersRace;
		ASSERT(pMajor);
		if (!pMajor)
			return;

		// Mauskoordinaten in ungezoomte Koordinaten der Starmap umrechnen
		CPoint pt(point);
		pt += GetScrollPosition() - m_ptViewOrigin;
		UnZoom(&pt);

		// Sektor, über dem sich die Maus befindet, ermitteln
		struct::Sector target = pMajor->GetStarmap()->GetClickedSector(pt);
		if (target != oldtarget && PT_IN_RECT(target, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
		{
			m_ResourceRoute.GenerateResourceRoute(CPoint(target.x,target.y), CSystemMenuView::GetResourceRouteRes());
			Invalidate();
			oldtarget = target;
		}
	}
	// wenn die linke Maustaste gedrückt ist und mit der Maus gescrollt werden soll
	else if (nFlags &= MK_LBUTTON)
	{
		CMajor* pMajor = m_pPlayersRace;
		ASSERT(pMajor);
		if (!pMajor)
			return;

		// Mauskoordinaten in ungezoomte Koordinaten der Starmap umrechnen
		CPoint pt(point);
		if (abs(m_ptOldMousePos.x - pt.x) > 10 || abs(m_ptOldMousePos.y - pt.y) > 10)
		{
			CPoint ptScrollPos = GetScrollPosition();
			bool bInvertMouse = false;
			CIniLoader::GetInstance()->ReadValue("Control", "INVERTMOUSE", bInvertMouse);
			if (!bInvertMouse)
				ptScrollPos += CPoint(pt.x - m_ptOldMousePos.x, pt.y - m_ptOldMousePos.y);
			else
				ptScrollPos -= CPoint(pt.x - m_ptOldMousePos.x, pt.y - m_ptOldMousePos.y);

			CRect client;
			GetClientRect(&client);
			ptScrollPos.x = min(ptScrollPos.x, STARMAP_TOTALWIDTH * m_fZoom - client.Width());
			ptScrollPos.y = min(ptScrollPos.y, STARMAP_TOTALHEIGHT * m_fZoom - client.Height());

			bool bShowScrollBars = false;
			CIniLoader::GetInstance()->ReadValue("Control", "SHOWSCROLLBARS", bShowScrollBars);
			if (bShowScrollBars)
			{
				Invalidate(TRUE);
				ScrollToPosition(ptScrollPos);
			}
			else
			{
				SetScrollPos(SB_HORZ, ptScrollPos.x, false);
				SetScrollPos(SB_VERT, ptScrollPos.y, false);
				Invalidate(FALSE);
			}

			SetCapture();
			m_ptOldMousePos = pt;
		}
	}
	CScrollView::OnMouseMove(nFlags, point);
}


void CGalaxyMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CRect client;
	GetClientRect(&client);
	CPoint position = GetScrollPosition();
	CPoint oldPosition = position;

	if (nChar == VK_UP)
	{
		position.y -= 25 * m_fZoom;
		position.y = max(0, position.y);
	}
	else if (nChar == VK_DOWN)// && position.y <= GetScrollLimit(SB_VERT))
	{
		position.y += 25 * m_fZoom;
		position.y = min(position.y, STARMAP_TOTALHEIGHT * m_fZoom - client.Height());
	}
	else if (nChar == VK_LEFT)// && position.x >= 0)
	{
		position.x -= 25 * m_fZoom;
		position.x = max(0, position.x);
	}
	else if (nChar == VK_RIGHT)// && position.x <= GetScrollLimit(SB_HORZ))
	{
		position.x += 25 * m_fZoom;
		position.x = min(position.x, STARMAP_TOTALWIDTH * m_fZoom - client.Width());
	}
	else if (nChar == VK_NEXT)
	{
		// alten Zoom-Faktor merken
		double oldZoom = m_fZoom;
		// Zoom-Faktor ändern (funktioniert auch bei hochauflösenden Scroll-Rädern (gibt's die schon?))
		m_fZoom += (double)(-1) * STARMAP_ZOOM_STEP;

		// wenn außerhalb der vorgegebenen Grenzen, dann auf die jeweilige Grenze setzen
		if (m_fZoom < STARMAP_ZOOM_MIN)
			m_fZoom = STARMAP_ZOOM_MIN;
		if (m_fZoom > STARMAP_ZOOM_MAX)
			m_fZoom = STARMAP_ZOOM_MAX;
		// wenn sich der Zoom-Faktor geändert hat
		if (m_fZoom != oldZoom)
		{
			// Anpassen der ScrollSizes
			OnUpdate(this, 0, NULL);
		}

		return;
	}
	else if (nChar == VK_PRIOR)
	{
		// alten Zoom-Faktor merken
		double oldZoom = m_fZoom;
		// Zoom-Faktor ändern (funktioniert auch bei hochauflösenden Scroll-Rädern (gibt's die schon?))
		m_fZoom += (double)(1) * STARMAP_ZOOM_STEP;

		// wenn außerhalb der vorgegebenen Grenzen, dann auf die jeweilige Grenze setzen
		if (m_fZoom < STARMAP_ZOOM_MIN)
			m_fZoom = STARMAP_ZOOM_MIN;
		if (m_fZoom > STARMAP_ZOOM_MAX)
			m_fZoom = STARMAP_ZOOM_MAX;
		// wenn sich der Zoom-Faktor geändert hat
		if (m_fZoom != oldZoom)
		{
			// Anpassen der ScrollSizes
			OnUpdate(this, 0, NULL);
		}

		return;
	}

	if (oldPosition != position)
	{
		bool bShowScrollBars = false;
		CIniLoader::GetInstance()->ReadValue("Control", "SHOWSCROLLBARS", bShowScrollBars);
		if (bShowScrollBars)
		{
			Invalidate(TRUE);
			ScrollToPosition(position);
		}
		else
		{
			SetScrollPos(SB_HORZ, position.x, false);
			SetScrollPos(SB_VERT, position.y, false);
			Invalidate(TRUE);
		}

		return;
	}

/*	// Cheat zum Forschung erhöhen
	if (nChar == VK_F9)
	{
		CString s;
		s.Format("Forschungsstufen + 1");
		pMajor->GetEmpire()->GetResearch()->SetBioTech(pMajor->GetEmpire()->GetResearch()->GetBioTech()+1);
		pMajor->GetEmpire()->GetResearch()->SetEnergyTech(pMajor->GetEmpire()->GetResearch()->GetEnergyTech()+1);
		pMajor->GetEmpire()->GetResearch()->SetCompTech(pMajor->GetEmpire()->GetResearch()->GetCompTech()+1);
		pMajor->GetEmpire()->GetResearch()->SetPropulsionTech(pMajor->GetEmpire()->GetResearch()->GetPropulsionTech()+1);
		pMajor->GetEmpire()->GetResearch()->SetConstructionTech(pMajor->GetEmpire()->GetResearch()->GetConstructionTech()+1);
		pMajor->GetEmpire()->GetResearch()->SetWeaponTech(pMajor->GetEmpire()->GetResearch()->GetWeaponTech()+1);
		AfxMessageBox(s);
	}
*/
	// Chatdialog anzeigen
	if (nChar == 'C')
	{
		AfxGetApp()->PostThreadMessage(WM_SHOWCHATDLG, 0, 0);
		return;
	}

	HandleShipHotkeys(nChar, pDoc);

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGalaxyMenuView::HandleShipHotkeys(const UINT nChar, CBotEDoc* pDoc)
{
	if(m_bShipMove) {
		m_bShipMove = FALSE;
		m_nRange = 0;
	}

	if(nChar == 'N') {
		SearchNextIdleShipAndJumpToIt(pDoc, SHIP_ORDER::NONE);
	}
	else if(nChar == VK_SPACE) {
		SearchNextIdleShipAndJumpToIt(pDoc, SHIP_ORDER::WAIT_SHIP_ORDER);
	}
	else if(nChar == 'S') {
		SearchNextIdleShipAndJumpToIt(pDoc, SHIP_ORDER::SENTRY_SHIP_ORDER);
	}
}

void CGalaxyMenuView::SearchNextIdleShipAndJumpToIt(CBotEDoc* pDoc, SHIP_ORDER::Typ order)
{
	assert(m_pPlayersRace);
	if(pDoc->m_ShipMap.empty())
		return;

	CShipMap::iterator start_at = pDoc->m_ShipMap.begin();
	CShipMap::const_iterator stop_at = pDoc->m_ShipMap.end();
	CShipMap::iterator previous_ship = pDoc->m_ShipMap.find(m_PreviouslyJumpedToShip.key);
	if(previous_ship != pDoc->m_ShipMap.end())
	{
		if(previous_ship->second->GetOwnerOfShip() == m_pPlayersRace->GetRaceID()
				&& Sector(previous_ship->second->GetKO()) == m_pPlayersRace->GetStarmap()->GetSelection()
				&& previous_ship->second->GetShipName() == m_PreviouslyJumpedToShip.name)
		{
			//the previously jumped to ship is still valid
			start_at = previous_ship;
			stop_at = previous_ship;
		}
		else
			previous_ship = pDoc->m_ShipMap.end();
	}
	bool found = false;
	for(CShipMap::iterator i = start_at;;) {
		++i;
		if(i == pDoc->m_ShipMap.end()) {
			if(i == stop_at)
				break;
			i = pDoc->m_ShipMap.begin();
		}
		if(m_pPlayersRace->GetRaceID() == i->second->GetOwnerOfShip()) {
			const CPoint& coords = i->second->GetKO();
			const Sector& sector = Sector(coords.x, coords.y);

			if(i->second->HasNothingToDo()) {
				if(previous_ship != pDoc->m_ShipMap.end() && order != SHIP_ORDER::NONE) {
					previous_ship->second->SetCurrentOrder(order);
					assert(order == SHIP_ORDER::WAIT_SHIP_ORDER || order == SHIP_ORDER::SENTRY_SHIP_ORDER);
					//In case the previous ship was selected via mouse instead via hotkey, which
					//checks whether there's a target != -1,-1 set, it can be a ship which still
					//has a valid target, but still would get order sentry or wait
					previous_ship->second->SetTargetKO(CPoint(-1, -1), true);
				}
				m_PreviouslyJumpedToShip = RememberedShip(i->second->GetShipName(), i->second->Key());
				m_pPlayersRace->GetStarmap()->Select(sector);// sets orange rectangle in galaxy view
				pDoc->SetKO(sector.x,sector.y);//neccessary for that the ship is selected for SHIP_BOTTOM_VIEW
				pDoc->SetCurrentShip(i);
				ScrollToSector(coords);

				CShipBottomView::SetShowStation(false);
				resources::pMainFrame->SelectBottomView(SHIP_BOTTOM_VIEW);
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CShipBottomView));//What's this doing ? Neccessary/sensible ?

				CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_SHIP_BOTTEM_VIEW);
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));//And this ?

				Invalidate(FALSE);//And this ?
				if(i != stop_at)
					found = true;
				break;
			}
		}
		if(i == stop_at)
			break;
	}
	if(!found) {
		CSmallInfoView::SetDisplayMode(CSmallInfoView::DISPLAY_MODE_ICON);
		resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		Invalidate(FALSE);
	}
}


int CGalaxyMenuView::GetRangeBorder(const unsigned char range1, const unsigned char range2, int m_nRange) const
{
	if (range1 != range2)
	{
		unsigned char maximum = max(range1, range2);
		if (m_nRange > 0)
			return max(min(maximum,m_nRange),min(range1,range2) + 1);
		else
			return maximum;
	}
	else
		return SM_RANGE_SPACE;
}

void CGalaxyMenuView::GenerateGalaxyMap()
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	
	if (!m_pGalaxyGraphic)
	{
		ASSERT(m_pGalaxyGraphic);
		return;
	}

	// Galaxiehintergrundbild verwenden und mit Informationen versehen
	if (m_pGalaxyBackground)
	{
		delete m_pGalaxyBackground;
		m_pGalaxyBackground = NULL;
	}

	// Kopie der Galaxiegrafik machen und bearbeiten
	m_pGalaxyBackground = m_pGalaxyGraphic->Clone(0,0,STARMAP_TOTALWIDTH,STARMAP_TOTALHEIGHT,m_pGalaxyGraphic->GetPixelFormat());
	assert(m_pGalaxyBackground && m_pGalaxyBackground->GetLastStatus() == Ok);
	
	Graphics* g = Graphics::FromImage(m_pGalaxyBackground);
	//g->SetSmoothingMode(SmoothingModeHighQuality);
	//g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	//g->SetPixelOffsetMode(PixelOffsetModeHighQuality);

	g->SetCompositingMode( CompositingModeSourceOver );  // 'Over for tranparency
	g->SetCompositingQuality( CompositingQualityHighSpeed );
	g->SetPixelOffsetMode( PixelOffsetModeHighSpeed );
	g->SetSmoothingMode( SmoothingModeHighSpeed );
	g->SetInterpolationMode( InterpolationModeHighQuality );

	CString sMinorID = "__MINOR__";
	CString sFogOfWarID = "__FOG_OF_WAR__";

	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			CPoint pt = pMajor->GetStarmap()->GetSectorCoords(struct::Sector(x, y));
			if (pDoc->GetSector(x,y).GetOwned() && pDoc->GetSector(x,y).GetScanned(pMajor->GetRaceID())
				&& pMajor->IsRaceContacted(pDoc->GetSector(x,y).GetOwnerOfSector()) || pDoc->GetSector(x,y).GetOwnerOfSector() == pMajor->GetRaceID())
			{
				g->DrawImage(m_mOwnerMark[pDoc->GetSector(x,y).GetOwnerOfSector()], pt.x, pt.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);
			}
			// Wurde der Sektor noch nicht gescannt, sprich ist noch Nebel des Krieges da?
			else if (!pDoc->GetSector(x,y).GetScanned(pMajor->GetRaceID()) && !pDoc->GetSector(x,y).GetKnown(pMajor->GetRaceID()))
			{
				g->DrawImage(m_mOwnerMark[sFogOfWarID], pt.x, pt.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);
			}
			// lebt eine Minorrace darauf und der Sektor ist uns bekannt, gehört aber noch niemanden
			else if (pDoc->GetSector(x,y).GetMinorRace() && !pDoc->GetSector(x,y).GetOwned() && pDoc->GetSector(x,y).GetKnown(pMajor->GetRaceID()))
			{
				g->DrawImage(m_mOwnerMark[sMinorID], pt.x, pt.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);
			}
			// Sonne bzw. Anomalie zeichnen
			if (pDoc->GetSector(x,y).GetScanned(pMajor->GetRaceID()))
			{
				if (pDoc->GetSector(x,y).GetSunSystem())
				{
					g->DrawImage(m_vStars[pDoc->GetSector(x,y).GetSunColor()], pt.x, pt.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);
				}
				else if (pDoc->GetSector(x,y).GetAnomaly())
				{
					pDoc->GetSector(x,y).GetAnomaly()->Draw(g, pt);
				}
			}

			pDoc->GetSector(x,y).DrawShipSymbolInSector(g, pDoc, m_pPlayersRace);
		}

	delete g;
	g = NULL;
	
#ifdef SAVE_GALAXYIMAGE
	FCObjImage img_to_save;
	FCWin32::GDIPlus_LoadBitmap(*m_pGalaxyBackground, img_to_save);
	CString name;
	name.Format("Round%iMajor%s",pDoc->GetCurrentRound(),pDoc->GetPlayersRaceID());
	img_to_save.Save("C://"+name+".bmp");
#endif

	// Thumbnail generieren
	if (m_pThumbnail)
	{
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}

	CSize thumbSize(150, STARMAP_TOTALHEIGHT * 150 / STARMAP_TOTALWIDTH);
	//wenn es eine hohe Karte ist wird anders skaliert
	if (STARMAP_TOTALWIDTH < STARMAP_TOTALHEIGHT)
		thumbSize.SetSize(STARMAP_TOTALWIDTH*150/STARMAP_TOTALHEIGHT,150);

	FCObjImage img;
	FCWin32::GDIPlus_LoadBitmap(*m_pGalaxyBackground, img);
	img.Stretch_Smooth(thumbSize.cx, thumbSize.cy);
	img.ConvertTo32Bit();
	img.SetAlphaChannelValue(120);
	m_pThumbnail = FCWin32::GDIPlus_CreateBitmap(img);
}

/// Funktion scrollt zur angegebenen Position in der Galaxiemap.
void CGalaxyMenuView::CenterOnScrollSector()
{
	if (m_ptScrollToSector == CPoint(-1, -1))
		return;

	// Punkt der Koordinate auf dem Bildschirm berechnen
	double x = (double)m_ptScrollToSector.x * (double)STARMAP_SECTOR_WIDTH;
	double y = (double)m_ptScrollToSector.y * (double)STARMAP_SECTOR_HEIGHT;

	// Scrollpunkt oben links ermitteln (Sektor wäre immer oben links zu sehen)
	CPoint scrollPos(x, y);
	Zoom(&scrollPos);

	// Scrollpunkt zentrieren (Sektor soll in der Mitte des Bildschirms angezeigt werden)
	CRect client;
	GetClientRect(&client);
	scrollPos.x -= client.Width() / 2;
	scrollPos.y -= client.Height() / 2;

	// maximalen Scroll beachten
	scrollPos.x = min(scrollPos.x, STARMAP_TOTALWIDTH * m_fZoom - client.Width());
	scrollPos.y = min(scrollPos.y, STARMAP_TOTALHEIGHT * m_fZoom - client.Height());

	// Scrollen
	SetScrollPos(SB_HORZ, scrollPos.x, false);
	SetScrollPos(SB_VERT, scrollPos.y, false);

	// Sektor markieren
	m_pPlayersRace->GetStarmap()->m_Selection = Sector(m_ptScrollToSector.x, m_ptScrollToSector.y);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CGalaxyMenuView::CreateTooltip(void)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	pt += GetScrollPosition() - m_ptViewOrigin;
	UnZoom(&pt);

	// Sektor, über dem sich die Maus befindet, ermitteln
	struct::Sector ko = pMajor->GetStarmap()->GetClickedSector(pt);
	if (PT_IN_RECT(ko, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
	{
		CSector* pSector = &(pDoc->GetSector(ko.x, ko.y));
		CString sTip;
		if (pSector->GetScanned(pMajor->GetRaceID()) == FALSE)
			sTip = CLoc::GetString("UNKNOWN");
		else if (pSector->GetAnomaly())
			sTip.Format("%s", pSector->GetAnomaly()->GetMapName(CPoint(ko.x,ko.y)));
		else if (pSector->GetKnown(pMajor->GetRaceID()) == FALSE)
			sTip.Format("%s %c%i", CLoc::GetString("SECTOR"),(char)(ko.y+97), ko.x+1);
		else
			sTip = pDoc->GetSector(ko.x, ko.y).GetName(true);

		sTip = CHTMLStringBuilder::GetHTMLColor(sTip);
		sTip = CHTMLStringBuilder::GetHTMLHeader(sTip, _T("h5"));
		sTip = CHTMLStringBuilder::GetHTMLCenter(sTip);

		// wenn der Sektor irgendwem gehört, dann Verteidigungsgebäude anzeigen
		if (pSector->GetSunSystem() && pSector->GetOwnerOfSector() != "" &&
			(pSector->GetScanPower(pMajor->GetRaceID()) > 50 || pSector->GetOwnerOfSector() == pMajor->GetRaceID()))
		{
			map<CString, int> mOnlineDefenceBuildings;
			map<CString, int> mAllDefenceBuildings;

			for (int l = 0; l < pDoc->GetSystem(ko.x, ko.y).GetAllBuildings()->GetSize(); l++)
			{
				int nID = pDoc->GetSystem(ko.x, ko.y).GetAllBuildings()->GetAt(l).GetRunningNumber();
				CBuildingInfo* pBuildingInfo = &pDoc->GetBuildingInfo(nID);
				if (pBuildingInfo->GetShipDefend() > 0 || pBuildingInfo->GetShipDefendBoni() > 0 ||
					pBuildingInfo->GetGroundDefend() > 0 || pBuildingInfo->GetGroundDefendBoni() > 0 ||
					pBuildingInfo->GetShieldPower() > 0 || pBuildingInfo->GetShieldPowerBoni())
				{
					mAllDefenceBuildings[pBuildingInfo->GetBuildingName()] += 1;
					if (pDoc->GetSystem(ko.x, ko.y).GetAllBuildings()->GetAt(l).GetIsBuildingOnline() || pBuildingInfo->GetNeededEnergy() == 0 || pBuildingInfo->GetAllwaysOnline())
						mOnlineDefenceBuildings[pBuildingInfo->GetBuildingName()] += 1;
				}
			}

			if (!mAllDefenceBuildings.empty())
			{
				sTip += CHTMLStringBuilder::GetHTMLStringNewLine();
				sTip += CHTMLStringBuilder::GetHTMLStringHorzLine();
				sTip += CHTMLStringBuilder::GetHTMLStringNewLine();
				sTip += CHTMLStringBuilder::GetHTMLStringNewLine();

				for (map<CString, int>::const_iterator it = mAllDefenceBuildings.begin(); it != mAllDefenceBuildings.end(); ++it)
				{
					CString sDefence;
					sDefence.Format("%s: %d/%d\n", it->first, mOnlineDefenceBuildings[it->first], it->second);
					sDefence = CHTMLStringBuilder::GetHTMLColor(sDefence);
					sDefence = CHTMLStringBuilder::GetHTMLHeader(sDefence, _T("h5"));
					sTip += sDefence;
				}
			}
		}

		// Auswirkungen einer Anomalie hinschreiben
		else if (pSector->GetAnomaly() && pSector->GetScanned(pMajor->GetRaceID()))
		{
			CString sGame = CHTMLStringBuilder::GetHTMLStringNewLine();
			sGame += CHTMLStringBuilder::GetHTMLStringNewLine();

			sGame += pSector->GetAnomaly()->GetGameplayDescription();
			sGame = CHTMLStringBuilder::GetHTMLColor(sGame, _T("silver"));
			sGame = CHTMLStringBuilder::GetHTMLHeader(sGame, _T("h5"));

			sTip += sGame;
		}

		return sTip;
	}

	return "";
}

/*
VK_RETURN                     Enter (Schreibmachinentastatur und Ziffernblock)
VK_SHIFT                      Umschalttaste (links und rechts)
VK_MENU                       ALT-Taste (links und rechts)
VK_CONTROL                    Strg (links und rechts)
VK_BACK                       Rücktaste
VK_TAB                        Tabulator
VK_PAUSE                      Pause
VK_CAPITAL                    Feststelltaste
VK_ESCAPE                     Esc
VK_SPACE                      Leertaset
VK_INSERT                     Einfg
VK_DELETE                     Entf
VK_HOME                       Pos 1
VK_END                        Ende
VK_PRIOR                      Bild auf
VK_NEXT                       Bild ab
VK_LEFT                       Linkspfeil
VK_RIGHT                      Rechtspfeil
VK_UP                         Pfeil aufwärts
VK_DOWN                       Pfeil abwärts
VK_SNAPSHOT                   Druck
VK_NUMPAD0                    0 bis
VK_NUMPAD9                    0 - 9 (Ziffernblock, Num_Lock an)
VK_MULTIPLY                   * (Ziffernblock)
VK_DIVIDE                     / (Ziffernblock)
VK_ADD                        + (Ziffernblock)
VK_SUBTRACT                   - (Ziffernblock)
VK_DECIMAL                    , (Ziffernblock)
VK_F1 bis VK_F10              F1 bis F10
VK_F11 bis VK_F24             F11 bis  F24
VK_NUMLOCK                    Num_Lock
VK_SCROLL                     Rollen
*/
