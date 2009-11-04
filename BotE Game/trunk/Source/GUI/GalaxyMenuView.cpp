// botf2View.cpp : Implementierung der Klasse CGalaxyMenuView
//

#include "stdafx.h"
#include "IOData.h"

#include "botf2Doc.h"
#include "GalaxyMenuView.h"
#include "SystemMenuView.h"
#include "MenuChooseView.h"
#include "PlanetBottomView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"
#include "Races\RaceController.h"
#include "Ships\Fleet.h"
#include "IniLoader.h"

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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGalaxyMenuView Konstruktion/Destruktion

CGalaxyMenuView::CGalaxyMenuView()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	m_pGalaxyBackground = NULL;
	m_pThumbnail = NULL;
	m_bUpdateOnly = false;
}

CGalaxyMenuView::~CGalaxyMenuView()
{
	// Hintergrundbild freigeben
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
}

BOOL CGalaxyMenuView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CScrollView::PreCreateWindow(cs);
}

void CGalaxyMenuView::OnNewRound()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	// Bei jeder neuen Runde die Galaxiekarte neu generieren
	GenerateGalaxyMap();
		
	m_bShipMove	= FALSE;
	m_bDrawTradeRoute = FALSE;
	m_bDrawResourceRoute = FALSE;
	m_bUpdateOnly = false;

	if (m_bScrollToHome)
	{
		// zu Beginn zum Startsektor scrollen, so dass dieser relativ zentral angezeigt wird.
		CPoint homePos = pDoc->GetRaceKO(m_pPlayersRace->GetRaceID());
		CPoint scrollPos;
		scrollPos.x = homePos.x * GetScrollLimit(SB_HORZ) / STARMAP_SECTORS_HCOUNT;
		scrollPos.y = homePos.y * GetScrollLimit(SB_VERT) / STARMAP_SECTORS_VCOUNT;
		SetScrollPos(SB_HORZ, scrollPos.x);
		SetScrollPos(SB_VERT, scrollPos.y);	
		m_bScrollToHome = false;
	}
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CGalaxyMenuView Zeichnen
void CGalaxyMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// ZU ERLEDIGEN: Hier Code zum Zeichnen der ursprünglichen Daten hinzufügen
	this->SetFocus();
	
	CRect r;
	r.SetRect(0, 0, m_TotalSize.cx, m_TotalSize.cy);
	
	// **************** DIE GALAXIEMAP ZEICHNEN ****************************************
	CRect client;
	GetClientRect(&client);
	
	CMemDC pDC(dc);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC.GetSafeHdc());
	
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
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y), &SolidBrush(Color::Aquamarine));
	s.Format("zoomlevel: %.1lf", m_fZoom);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 10), &SolidBrush(Color::Aquamarine));
	//s.Format("filename: \"test.jpg\"");
	//g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 20), &SolidBrush(Color::Aquamarine));
	s.Format("resolution: %d * %d", m_pGalaxyBackground->GetWidth(), m_pGalaxyBackground->GetHeight());
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 20), &SolidBrush(Color::Aquamarine));
	//s.Format("dpi: %.1lf * %.1lf", m_pGalaxyBackground->GetHorizontalResolution(), m_pGalaxyBackground->GetVerticalResolution());
	//g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(L"Arial", 8, FontStyleBold), PointF((REAL)pt.x, (REAL)pt.y + 40), &SolidBrush(Color::Orange));	
#endif
	// äußere schwarze Umrandung zeichnen
	//g.DrawRectangle(&Pen(Color::Black, 3.0), posX - 1, posY - 1, picWidth + 2, picHeight + 2);
	RectF thumbRect((REAL)((pt.x + client.right - m_pThumbnail->GetWidth() - 15)),
		(REAL)((pt.y + client.bottom - m_pThumbnail->GetHeight() - 15)),
		(REAL)m_pThumbnail->GetWidth(), (REAL)m_pThumbnail->GetHeight());

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
	//color.SetFromCOLORREF(clrColor);
	g.DrawRectangle(&Pen(Color(200,255,255,255), 1.5), thumbRect.X - 1, thumbRect.Y - 1, thumbRect.Width + 2, thumbRect.Height + 2);		
	
//////////////////////////////////////////////////////////////

	// Abbildungsmodus setzen, WindowExt erhält tatsächliche Größe, ViewportExt die gezoomte Größe
	pDC->SetMapMode(MM_ANISOTROPIC);	
	pDC->SetWindowExt(STARMAP_TOTALWIDTH, STARMAP_TOTALHEIGHT);
	CSize size(STARMAP_TOTALWIDTH, STARMAP_TOTALHEIGHT);
	Zoom(&size);	
	pDC->SetViewportExt(size);
	pDC->SetViewportOrg(-GetScrollPosition());
	// die Koordinaten der folgenden Zeichenoperationen werden automatisch gezoomt
	
	// --- Gitternetztlinien zeichnen ---

	// da nahezu alle Linien sofort richtig gezeichnet werden, tritt auch kein Flimmern auf, wenn nur das Gitter
	// neu gezeichnet wird (m_bUpdateOnly == TRUE); nur teilweises Zeichnen des Gitters bringt selbst auf meinem
	// Rechner nichts mehr, deswegen wird immer das ganze Gitter gezeichnet
	pDC->SetBkMode(TRANSPARENT);
	CPen	gridPen(PS_SOLID, 0, RGB(0,0,0));			// Gitternetz
	CPen	scanPen(PS_DOT, 0, RGB(75,75,75));			// gescanntes Gitternetz
	CPen	nearPen(PS_SOLID, 1, RGB(0, 200, 0)),		// kurze Entfernung
			middlePen(PS_SOLID, 1, RGB(200, 200, 0)),	// mittlere Entfernung
			farPen(PS_SOLID, 1, RGB(200, 0, 0));		// große Entfernung
	CPen	selPen(PS_SOLID, 0, RGB(255,125,0));		// Markierung

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
	{
		// Wenn das Schiff keine Flotte anführt
		if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0 || (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0 && pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->GetFleetSize() == 0))
			m_nRange = 3-pDoc->GetShip(pDoc->GetNumberOfTheShipInArray()).GetRange();
		else
			m_nRange = 3-pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->GetFleetRange(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()));	
	}
	else
		m_nRange = SM_RANGE_SPACE;
	// für jeden Sektor die Linie rechts und unten zeichnen; (j, i) ist der aktuell betrachtete Sektor,
	// (x, y) sind die Koordinaten dessen linker oberer Ecke
		
	for (int i = 0, y = 0; i < STARMAP_SECTORS_VCOUNT; i++, y += STARMAP_SECTOR_HEIGHT)
		for (int j = 0, x = 0; j < STARMAP_SECTORS_HCOUNT; j++, x += STARMAP_SECTOR_WIDTH)
		{
			// Flugweg der Schiffe zeichnen
			if (pDoc->m_Sector[j][i].GetShipPathPoints() > 0)
			{
				COLORREF color = RGB(255,255,255);
				CBrush brush(color);
				CPen pen(PS_SOLID, 0, color);
				pDC->SelectObject(&brush);
				pDC->SelectObject(&pen);
				pDC->Ellipse(x+STARMAP_SECTOR_WIDTH/2-4,y+STARMAP_SECTOR_HEIGHT/2-4,x+STARMAP_SECTOR_WIDTH/2+4,y+STARMAP_SECTOR_HEIGHT/2+4);
			}	

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
				// statt RANGE_MIDDLE befindet (kann bei entsprechender Rangemap um eine Starbase
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
			if (pDoc->GetSector(j,i).GetScanned(pMajor->GetRaceID()) == TRUE || (pDC->GetCurrentPen() != &gridPen && pDC->GetCurrentPen() != &scanPen))
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
			if (pDoc->GetSector(j, i).GetScanned(pMajor->GetRaceID()) == TRUE || (pDC->GetCurrentPen() != &gridPen && pDC->GetCurrentPen() != &scanPen))
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
			
	// ------
	// wenn gerade ein Ziel gewählt wird, den berechneten Weg zeichnen
	if (m_nRange && pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath()->GetSize())
	{
		// Vorlage mit jedem Sektor des Weges verknüpfen
		for (int i = 0; i < pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath()->GetSize(); i++)
		{
			COLORREF color = pMajor->GetDesign()->m_clrRouteColor;
			CBrush brush(color);
			CPen pen(PS_SOLID , 0, color);
			pDC->SelectObject(&brush);
			pDC->SelectObject(&pen);
			ASSERT_SECTOR_VALID(pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath()->GetAt(i));
			CPoint pt = pMajor->GetStarmap()->GetSectorCoords(pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath()->GetAt(i));
			pDC->Ellipse(pt.x+STARMAP_SECTOR_WIDTH/2-4,pt.y+STARMAP_SECTOR_HEIGHT/2-4,pt.x+STARMAP_SECTOR_WIDTH/2+4,pt.y+STARMAP_SECTOR_HEIGHT/2+4);
		}
		
		// Anzahl der benötigten Runden in letztes Feld des Weges zeichnen
		//CPoint last = pMajor->GetStarmap()->GetSectorCoords(path[path.GetUpperBound()]);
		int n = pDoc->GetNumberOfTheShipInArray();
		CPoint last = pMajor->GetStarmap()->GetSectorCoords(pDoc->m_ShipArray[n].GetPath()->GetAt(pDoc->m_ShipArray[n].GetPath()->GetUpperBound()));
		CString s;
		// Wenn das Schiff keine Flotte anführt
		if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0 || (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0 && pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->GetFleetSize() == 0))
			s.Format("%.0f",ceil((float)pDoc->m_ShipArray[n].GetPath()->GetSize() / (float)pDoc->m_ShipArray[n].GetSpeed()));
		else
			s.Format("%.0f",ceil((float)pDoc->m_ShipArray[n].GetPath()->GetSize() / (float)pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->GetFleetSpeed(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()))));	
		pDC->SetTextColor(RGB(255,255,255));
		pDC->SetBkMode(TRANSPARENT);
		pDC->TextOut(last.x+STARMAP_SECTOR_WIDTH/2+6, last.y+STARMAP_SECTOR_HEIGHT/2-8, s);

		/**/ // Debug: Länge des Weges ausgeben
/*		CString s;
		s.Format("%d ", path.GetSize());
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->TextOut(10, 10, s);
		/**/
	}
	pDC->SelectObject(pOldPen);
	bool bShowTraderoutes;
	CIniLoader::GetInstance()->ReadValue("Video", "SHOWTRADEROUTES", bShowTraderoutes);
	// Namen des Systems und Handelsrouten zeichnen, nur wenn komplett neu gezeichnet wurde
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			pDoc->GetSector(x,y).DrawSectorsName(pDC ,pDoc, m_pPlayersRace);
			// eigene Handelsrouten zeichnen
			if (bShowTraderoutes)
				if (pDoc->m_System[x][y].GetOwnerOfSystem() == pMajor->GetRaceID())
				{
					for (int i = 0; i < pDoc->m_System[x][y].GetTradeRoutes()->GetSize(); i++)
						pDoc->m_System[x][y].GetTradeRoutes()->GetAt(i).DrawTradeRoute(pDC, CPoint(x,y), pMajor);
					for (int i = 0; i < pDoc->m_System[x][y].GetResourceRoutes()->GetSize(); i++)
						pDoc->m_System[x][y].GetResourceRoutes()->GetAt(i).DrawResourceRoute(pDC, CPoint(x,y), pMajor);
				}
		}
	
	if (bShowTraderoutes)
	{
		if (m_bDrawTradeRoute && (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetOwnerOfSystem() == pMajor->GetRaceID()))
			m_TradeRoute.DrawTradeRoute(pDC, pDoc->GetKO(), pMajor);
		if (m_bDrawResourceRoute && (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetOwnerOfSystem() == pMajor->GetRaceID()))
			m_ResourceRoute.DrawResourceRoute(pDC, pDoc->GetKO(), pMajor);
	}
	
	pOldPen->DeleteObject();
	
	// aktuelle Scrollposition immer merken
	pDoc->m_ptScrollPoint = GetScrollPosition();
	g.ReleaseHDC(pDC.GetSafeHdc());	
}

void CGalaxyMenuView::OnInitialUpdate()
{
	// ZU ERLEDIGEN: Gesamte Größe dieser Ansicht berechnen
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CString sID = pDoc->GetPlayersRaceID();
	m_pPlayersRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sID));
	ASSERT(m_pPlayersRace);

	// Cursor aus Ressource laden
    HCURSOR m_hCur = AfxGetApp()->LoadCursor(IDC_CURSOR);
	// Geladenen Cursor nun setzen
    ::SetCursor(m_hCur);
    // Sehr wichtig, sonst wird Cursor sofort wieder zurueckgesetzt!
    ::SetClassLong(*this,GCL_HCURSOR,(LONG)m_hCur);

	// Neu mit Tonis Programmierung
	m_fZoom = STARMAP_ZOOM_INITIAL;
	m_nRange = SM_RANGE_SPACE;

	// Fontobjekt löschen
	if (CSector::m_Font)
	{
		delete CSector::m_Font;
		CSector::m_Font = NULL;
	}

	m_TotalSize = CSize(1075, 750);
	m_bShipMove	= FALSE;
	m_bScrollToHome = TRUE;

	pDoc->SetKO(pDoc->GetRaceKO(m_pPlayersRace->GetRaceID()));

	CScrollView::OnInitialUpdate();
}

BOOL CGalaxyMenuView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return FALSE;
	if (m_pPlayersRace == NULL)
		return FALSE;
	
	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}

BOOL CGalaxyMenuView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return FALSE;
	if (m_pPlayersRace == NULL)
		return FALSE;
	
	CRect client;
	GetClientRect(&client);
	InvalidateRect(&client, FALSE);	
	return CScrollView::OnScrollBy(sizeScroll, bDoScroll);
}

void CGalaxyMenuView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class
	// ScrollSizes auf Gesamtgröße inkl. Zoom setzen
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	if (!pDoc->m_bDataReceived)
		return;
	
	CSize size;
	size.cx = (LONG)(STARMAP_TOTALWIDTH * m_fZoom);
	size.cy = (LONG)(STARMAP_TOTALHEIGHT * m_fZoom);
	SetScrollSizes(MM_TEXT, size);	
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
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
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
	pt += GetScrollPosition();
	UnZoom(&pt);

	if (!m_nRange)
	{
		// angeklickten Sektor ermitteln, anschließend markieren
		struct::Sector sector = pMajor->GetStarmap()->GetClickedSector(pt);
		CSmallInfoView::SetShipInfo(false);
		CSmallInfoView::SetPlanetInfo(false);
		CSmallInfoView::SetPlanetStats(false);
		pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		
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
				map<CString, CRace*>* pmRaces = pDoc->GetRaceCtrl()->GetRaces();
				for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); it++)
					if ((pDoc->m_Sector[sector.x][sector.y].GetOwnerOfShip(it->first) == TRUE && it->first == pMajor->GetRaceID())
						|| (pDoc->m_Sector[sector.x][sector.y].GetOwnerOfShip(it->first) == TRUE && pDoc->m_Sector[sector.x][sector.y].GetNeededScanPower(it->first) < pDoc->m_Sector[sector.x][sector.y].GetScanPower(pMajor->GetRaceID())))
						{
							CShipBottomView::SetShowStation(false);
							pDoc->GetMainFrame()->SelectBottomView(SHIP_BOTTOM_VIEW);
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
							break;
						}
			}
			// Wenn in dem Sektor eine Station steht oder noch gebaut wird, dann Stationsansicht in View3 zeigen,
			// wenn wir unten rechts hingeklickt haben
			else if (modulo.x < STARMAP_SECTOR_WIDTH * 0.33 && modulo.y > STARMAP_SECTOR_HEIGHT * 0.66)
			{
				map<CString, CRace*>* pmRaces = pDoc->GetRaceCtrl()->GetRaces();
				for (map<CString, CRace*>::const_iterator it = pmRaces->begin(); it != pmRaces->end(); it++)
					if (pDoc->m_Sector[sector.x][sector.y].GetScanPower(pMajor->GetRaceID()) > 0 && (pDoc->m_Sector[sector.x][sector.y].GetIsStationBuilding(it->first) == TRUE
						|| pDoc->m_Sector[sector.x][sector.y].GetOutpost(it->first) == TRUE || pDoc->m_Sector[sector.x][sector.y].GetStarbase(it->first) == TRUE))
					{
						CShipBottomView::SetShowStation(true);
						pDoc->GetMainFrame()->SelectBottomView(SHIP_BOTTOM_VIEW);						
						pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
						break;
					}
			}			
			else
			{
				// Planetenansicht in View3 des angeklickten Sektors zeigen
				pDoc->GetMainFrame()->SelectBottomView(PLANET_BOTTOM_VIEW);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
			}
			
			// View muss neu gezeichnet werden
			m_bUpdateOnly = TRUE;
			Invalidate();
		}
		// Wenn wir eine Handelsroute festlegen wollen
		else if (sector != struct::Sector(-1,-1) && m_bDrawTradeRoute == TRUE)
		{
			CPoint p = pDoc->GetKO();
			BYTE numberOfRoutes = pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize();
			// konnten erfolgreich eine hinzufügen aufgrund der Bevölkerung
			if (pDoc->m_Sector[sector.x][sector.y].GetSunSystem() == TRUE && pDoc->m_System[p.x][p.y].AddTradeRoute(CPoint(sector.x,sector.y), pDoc->m_System, pMajor->GetEmpire()->GetResearch()->GetResearchInfo()))
			{
				// jetzt diplomatische Beziehung checken
				if (pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetAt(pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetUpperBound()).CheckTradeRoute(p, CPoint(sector.x, sector.y), pDoc))
				{
					// wenn wir noch weitere Handelsrouten hinzufügen können, dann in der Ansicht bleiben
					if (pDoc->m_System[p.x][p.y].CanAddTradeRoute(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()))
						m_bDrawTradeRoute = TRUE;
					else
						m_bDrawTradeRoute = FALSE;
					// Anzeige gleich aktualisieren
					pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),
						pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
					Invalidate();
				}
				// konnten wie die Handelsroute aufgrund der diploamtischen Beziehungen nicht hinzufügen, so
				// müssen wir sie gleich wieder löschen
				else
				{
					pDoc->m_System[p.x][p.y].GetTradeRoutes()->RemoveAt(pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetUpperBound());
				}
			}
			// Wenn wir eine Handelsroute gelöscht haben, dann aktualisieren
			else if (numberOfRoutes > pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize())
				pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),
				pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
		}
		// Wenn wir eine Ressourcenroute festlegen wollen
		else if (sector != struct::Sector(-1,-1) && m_bDrawResourceRoute == TRUE)
		{
			CPoint p = pDoc->GetKO();
			// konnten erfolgreich eine hinzufügen aufgrund der Bevölkerung
			if (pDoc->m_Sector[sector.x][sector.y].GetSunSystem() == TRUE && p != CPoint(sector.x,sector.y) &&
				pDoc->m_System[p.x][p.y].AddResourceRoute(CPoint(sector.x,sector.y), CSystemMenuView::GetResourceRouteRes(), pDoc->m_System, pMajor->GetEmpire()->GetResearch()->GetResearchInfo()))
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
		if (m_bShipMove && (pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath()->GetSize() 
			|| target == pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetKO()))
		{
			pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].SetTargetKO(target,0);
			CSmallInfoView::SetShipInfo(true);
			SetMoveShip(FALSE);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			m_nRange = 0;
			CSoundManager::GetInstance()->PlaySound(SNDMGR_SOUND_SHIPTARGET);
			// alte Flugdaten aus den Sektoren löschen
			for (int i = 0; i < m_oldPath.GetSize(); i++)
				pDoc->m_Sector[m_oldPath.GetAt(i).x][m_oldPath.GetAt(i).y].AddShipPathPoints(-1);
			m_oldPath.RemoveAll();
			// Die Flugkoordinaten des Schiffes den Sektoren mitteilen, damit sie die Flugroute des Schiffes
			// immer anzeigen können, auch wenn kein Schiff ausgewählt ist
			for (int i = 0; i < pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath()->GetSize(); i++)
				pDoc->m_Sector[pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath()->GetAt(i).x][pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath()->GetAt(i).y].AddShipPathPoints(1);
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

BOOL CGalaxyMenuView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	//return CScrollView::OnEraseBkgnd(pDC);
	return FALSE;
}

BOOL CGalaxyMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
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
	pt += GetScrollPosition();
	UnZoom(&pt);
	struct::Sector sector = pMajor->GetStarmap()->GetClickedSector(pt);

	// alten Zoom-Faktor merken
	double oldZoom = m_fZoom;

	// Zoom-Faktor ändern (funktioniert auch bei hochauflösenden Scroll-Rädern (gibt's die schon?))
	m_fZoom += (double)zDelta * STARMAP_ZOOM_STEP / WHEEL_DELTA;
	/*if (zDelta < 0)
		m_fZoom -= 0.05f * m_fZoom;
	else if (zDelta > 0)
		m_fZoom += 0.05f * m_fZoom;*/
	// wenn außerhalb der vorgegebenen Grenzen, dann auf die jeweilige Grenze setzen
	if (m_fZoom < STARMAP_ZOOM_MIN)
		m_fZoom = STARMAP_ZOOM_MIN;
	if (m_fZoom > STARMAP_ZOOM_MAX)
		m_fZoom = STARMAP_ZOOM_MAX;

	// wenn sich der Zoom-Faktor geändert hat
	if (m_fZoom != oldZoom)
	{
		Invalidate(TRUE);
		OnUpdate(this, 0, NULL); // Anpassen der ScrollSizes
		
		// ScrollPosition so anpassen, dass sich nach dem Zoomen wieder der Sektor unter dem Mauscursor
		// befindet, der vor dem Zoomen schon unter der Maus war
		if (sector.x > -1 && sector.y > -1)
		{
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
			ScrollToPosition(pt);
		}			
	}	

	return CScrollView::OnMouseWheel(nFlags, zDelta, point);
}

void CGalaxyMenuView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Wenn wir uns in der Galaxieansicht befinden
	CPoint pt(point);
	pt += GetScrollPosition();
	UnZoom(&pt);		
	if (!m_nRange)
	{
		// angeklickten Sektor ermitteln
		struct::Sector sector = pMajor->GetStarmap()->GetClickedSector(pt);
		if (pDoc->GetSystem(sector.x, sector.y).GetOwnerOfSystem() == pMajor->GetRaceID() &&
			pDoc->GetSector(sector.x, sector.y).GetSunSystem() == TRUE)
			{
				// falls ein Schiff markiert war wird dieses abgewählt
				SetMoveShip(FALSE);
				pDoc->SetKO(sector.x,sector.y);
				pDoc->GetMainFrame()->SelectMainView(SYSTEM_VIEW, pMajor->GetRaceID());				
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CGalaxyMenuView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	
	// Das hier alles nur machen, wenn wir in der Galaxiemap-Ansicht sind
	// Überprüfen, ob mit einem Doppelklick auf ein eigenes System geklickt wurde,
	// wenn ja, dann in die Systemansicht von diesem System gehen
	
	/*
	// Mit Rechtsklick gehört das System ner Testrasse
	pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].SetColonyOwner(FERENGI);
	pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].SetOwnerOfSector(FERENGI);
	pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].SetOwned(TRUE);
	for (int i = 0; i < pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetNumberOfPlanets(); i++)
	{
		pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetPlanet(i)->SetColonisized(TRUE);
		pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetPlanet(i)->SetMaxHabitant(10);
		pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetPlanet(i)->SetCurrentHabitant(2);			
	}
	pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].SetOwnerOfSystem(FERENGI);
	*/	

	//Wenn wir das Befehlgeben eines Schiffes abbrechen wollen
	if (m_bShipMove)
	{
		SetMoveShip(FALSE);
		CSmallInfoView::SetShipInfo(true);
		pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		m_nRange = 0;
		Invalidate();
	//	pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(View3));
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
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
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
		pt += GetScrollPosition();
		UnZoom(&pt);

		// Sektor, über dem sich die Maus befindet, ermitteln
		struct::Sector target = pMajor->GetStarmap()->GetClickedSector(pt);
		struct::Sector shipKO(pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetKO().x,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetKO().y);

		if (PT_IN_RECT(target, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
		{
			// Weg berechnen, neu zeichnen
			char speed = 0;
			// Wenn das Schiff keine Flotte anführt
			if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0 || (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0 && pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->GetFleetSize() == 0))
				speed = (char)(pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetSpeed());	
			else
				speed = (char)(pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->GetFleetSpeed(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray())));	
									
			struct::Sector result = pMajor->GetStarmap()->CalcPath(pMajor->GetStarmap()->GetSelection(), target, m_nRange, speed, *pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetPath());
			
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
		pt += GetScrollPosition();
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
		pt += GetScrollPosition();
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
	CScrollView::OnMouseMove(nFlags, point);	
}


void CGalaxyMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CPoint position = GetScrollPosition();
	CPoint oldPosition = position;
	if (nChar == VK_UP && position.y >= 0)
		position.y -= 15 * m_fZoom;
	else if (nChar == VK_DOWN && position.y <= GetScrollLimit(SB_VERT))
		position.y += 15 * m_fZoom;
	else if (nChar == VK_LEFT && position.x >= 0)
		position.x -= 15 * m_fZoom;
	else if (nChar == VK_RIGHT && position.x <= GetScrollLimit(SB_HORZ))
		position.x += 15 * m_fZoom;
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
			Invalidate();
			OnUpdate(this, 0, NULL); // Anpassen der ScrollSizes				
		}
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
			Invalidate();
			OnUpdate(this, 0, NULL); // Anpassen der ScrollSizes				
		}			
	}
	if (oldPosition != position)
	{
		Invalidate(TRUE);
		ScrollToPosition(position);
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
	}
	
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
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

void CGalaxyMenuView::SetNewShipPath()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	m_oldPath.RemoveAll();
	m_oldPath.Copy(*pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetPath());	
}

void CGalaxyMenuView::GenerateGalaxyMap()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	// Galaxiehintergrundbild laden
	if (m_pGalaxyBackground)
	{
		delete m_pGalaxyBackground;
		m_pGalaxyBackground = NULL;
	}
		
	CString sAppPath = CIOData::GetInstance()->GetAppPath();
	CString prefix = pMajor->GetPrefix();
	CString filePath = sAppPath + "Graphics\\Galaxies\\" + prefix + "galaxy.jpg";

	m_pGalaxyBackground = Bitmap::FromFile(filePath.AllocSysString());
	
	if (m_pGalaxyBackground->GetLastStatus() != Ok)
	{
		AfxMessageBox("Could not load galaxy background");
		#ifdef TRACE_GRAPHICLOAD
		MYTRACE(MT::LEVEL_WARNING, "CGalaxyMenuView::GenerateGalaxy(): Could not load galaxy background");
		#endif
	}

	// Mal die Sterne direkt in die Map setzen, neues Bild erzeugen
	Bitmap *stars[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	filePath = sAppPath + "Graphics\\MapStars\\star_blue.png";
	stars[0] = Bitmap::FromFile(filePath.AllocSysString());
	filePath = sAppPath + "Graphics\\MapStars\\star_green.png";
	stars[1] = Bitmap::FromFile(filePath.AllocSysString());
	filePath = sAppPath + "Graphics\\MapStars\\star_orange.png";
	stars[2] = Bitmap::FromFile(filePath.AllocSysString());
	filePath = sAppPath + "Graphics\\MapStars\\star_red.png";
	stars[3] = Bitmap::FromFile(filePath.AllocSysString());
	filePath = sAppPath + "Graphics\\MapStars\\star_violet.png";
	stars[4] = Bitmap::FromFile(filePath.AllocSysString());
	filePath = sAppPath + "Graphics\\MapStars\\star_white.png";
	stars[5] = Bitmap::FromFile(filePath.AllocSysString());
	filePath = sAppPath + "Graphics\\MapStars\\star_yellow.png";
	stars[6] = Bitmap::FromFile(filePath.AllocSysString());
	
	#ifdef TRACE_GRAPHICLOAD
	for (int i = 0; i < 7; i++)
		if (!stars[i] || stars[i]->GetLastStatus() != Ok)
			MYTRACE(MT::LEVEL_WARNING, "CGalaxyMenuView::GenerateGalaxy(): Could not load a star graphic");
	#endif

	map<CString, Bitmap*> ownerMark;
	
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		Color clr;
		clr.SetFromCOLORREF(it->second->GetDesign()->m_clrSector);		
		Color color(100, clr.GetR(), clr.GetG(), clr.GetB());
		
		// hier wurde der R Wert mit dem B Wert getauscht, da die Funktion SetPixelData sonst nicht stimmt.
		// color = RGB(GetBValue(color), GetGValue(color), GetRValue(color));

		// Bitmap generieren
		ownerMark[it->first] = new Gdiplus::Bitmap(STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT, PixelFormat32bppPARGB);
		if (ownerMark[it->first])
			for (int y = 0; y < STARMAP_SECTOR_HEIGHT; y++)
				for (int x = 0; x < STARMAP_SECTOR_WIDTH; x++)
					ownerMark[it->first]->SetPixel(x, y, color);		
	}
	
	// Farbe für Minors hinzufügen
	CString sMinorID = "__MINOR__";
	ownerMark[sMinorID] = new Bitmap(STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT, PixelFormat32bppPARGB);
	if (ownerMark[sMinorID])
		for (int y = 0; y < STARMAP_SECTOR_HEIGHT; y++)
			for (int x = 0; x < STARMAP_SECTOR_WIDTH; x++)
				ownerMark[sMinorID]->SetPixel(x, y, Color(100,200,200,200));
		
	// Farbe für Nebel des Krieges hinzuzufügen
	CString sFogOfWarID = "__FOG_OF_WAR__";
	ownerMark[sFogOfWarID] = new Bitmap(STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT, PixelFormat32bppPARGB);
	if (ownerMark[sFogOfWarID])
		for (int y = 0; y < STARMAP_SECTOR_HEIGHT; y++)
			for (int x = 0; x < STARMAP_SECTOR_WIDTH; x++)
				ownerMark[sFogOfWarID]->SetPixel(x, y, Color(160,0,0,0));
	
	Graphics* g = Graphics::FromImage(m_pGalaxyBackground);
	g->SetSmoothingMode(SmoothingModeHighQuality);
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g->SetPixelOffsetMode(PixelOffsetModeHighQuality);
	
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{	
			CPoint pt = pMajor->GetStarmap()->GetSectorCoords(struct::Sector(x, y));
			if (pDoc->GetSector(x,y).GetOwned() && pDoc->GetSector(x,y).GetScanned(pMajor->GetRaceID())
				&& pMajor->IsRaceContacted(pDoc->GetSector(x,y).GetOwnerOfSector()) || pDoc->GetSector(x,y).GetOwnerOfSector() == pMajor->GetRaceID())
			{
				g->DrawImage(ownerMark[pDoc->GetSector(x,y).GetOwnerOfSector()], pt.x, pt.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);
			}
			// Wurde der Sektor noch nicht gescannt, sprich ist noch Nebel des Krieges da?
			else if (!pDoc->GetSector(x,y).GetScanned(pMajor->GetRaceID()) && !pDoc->GetSector(x,y).GetKnown(pMajor->GetRaceID()))
			{
				g->DrawImage(ownerMark[sFogOfWarID], pt.x, pt.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);
			}
			// lebt eine Minorrace darauf und der Sektor ist uns bekannt, gehört aber noch niemanden
			else if (pDoc->GetSector(x,y).GetMinorRace() && !pDoc->GetSector(x,y).GetOwned() && pDoc->GetSector(x,y).GetKnown(pMajor->GetRaceID()))
			{
				g->DrawImage(ownerMark[sMinorID], pt.x, pt.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);
			}
			if (pDoc->GetSector(x,y).GetSunSystem() == TRUE && pDoc->GetSector(x,y).GetScanned(pMajor->GetRaceID()))
			{
				g->DrawImage(stars[pDoc->GetSector(x,y).GetSunColor()], pt.x, pt.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);
			}
			
			pDoc->GetSector(x,y).DrawShipSymbolInSector(g, pDoc, m_pPlayersRace);
		}	

	delete g;

	// Thumbnail generieren
	if (m_pThumbnail)
	{
		delete m_pThumbnail;
		m_pThumbnail = NULL;
	}
	Bitmap* tmp = m_pGalaxyBackground->Clone(0, 0, m_pGalaxyBackground->GetWidth(), m_pGalaxyBackground->GetHeight(), PixelFormat32bppPARGB);
	delete m_pGalaxyBackground;
	m_pGalaxyBackground = tmp;
		
	CSize thumbSize(150, STARMAP_TOTALHEIGHT * 150 / STARMAP_TOTALWIDTH);
	FCObjImage img;
	FCWin32::GDIPlus_LoadBitmap(*m_pGalaxyBackground, img);
	img.Stretch_Smooth(thumbSize.cx, thumbSize.cy);
	img.SetAlphaChannelValue(120);
	m_pThumbnail = FCWin32::GDIPlus_CreateBitmap(img);
	img.Destroy();
	
	// aufräumen
	for (map<CString, Bitmap*>::const_iterator it = ownerMark.begin(); it != ownerMark.end(); it++)
		delete it->second;
	ownerMark.clear();
	
	for (int i = 0; i < 7; i++)
		if (stars[i])
		{
			delete stars[i];
			stars[i] = NULL;
		}
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