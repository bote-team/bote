// TransportMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "TransportMenuView.h"
#include "RaceController.h"
#include "IniLoader.h"
#include "Fleet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CTransportMenuView

IMPLEMENT_DYNCREATE(CTransportMenuView, CMainBaseView)

CTransportMenuView::CTransportMenuView()
{
}

CTransportMenuView::~CTransportMenuView()
{
}

BEGIN_MESSAGE_MAP(CTransportMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CTransportMenuView drawing

void CTransportMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	
	// TODO: add draw code here
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
	
	// ***************************** DIE SCHIFFSTRANSPORT ANSICHT ZEICHNEN **********************************
	DrawTransportMenue(&g);
	// ************** DIE SCHIFFSDESIGN ANSICHT ZEICHNEN ist hier zu Ende **************
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
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);

	CreateButtons();

	// alle Hintergrundgrafiken laden
	CString sPrefix = pMajor->GetPrefix();
	bg_transportmenu = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "transportmenu.jpg");
		
	// Transportansicht
	m_iTransportStorageQuantity = 1;
}

void CTransportMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CTransportMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Transportmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CTransportMenuView::DrawTransportMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);
	if (!pMajor)
		return;

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
	CPoint p = pDoc->GetKO();
	CString systemOwner = pDoc->m_System[p.x][p.y].GetOwnerOfSystem();
	CString shipOwner = pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip();

	Gdiplus::Font font(fontName.AllocSysString(), fontSize);
	if (systemOwner == shipOwner)
		g->DrawString(CResourceManager::GetString("SYSTEM_STORAGE").AllocSysString(), -1, &font, RectF(200,80,338,25), &fontFormat, &fontBrush);

	g->DrawString(CResourceManager::GetString("SHIP_STORAGE").AllocSysString(), -1, &font, RectF(538,80,338,25), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("TITAN").AllocSysString(), -1, &font, RectF(0,120,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("DEUTERIUM").AllocSysString(), -1, &font, RectF(0,180,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("DURANIUM").AllocSysString(), -1, &font, RectF(0,240,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("CRYSTAL").AllocSysString(), -1, &font, RectF(0,300,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("IRIDIUM").AllocSysString(), -1, &font, RectF(0,360,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("DILITHIUM").AllocSysString(), -1, &font, RectF(0,420,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("TROOPS").AllocSysString(), -1, &font, RectF(0,560,1075,60), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("SELECTED").AllocSysString(), -1, &font, RectF(0,620,1075,60), &fontFormat, &fontBrush);
	fontBrush.SetColor(normalColor);
	
	// Inhalte des system- und globalen Lagers zeichnen
	CShip* ship = &pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray());
	for (int i = TITAN; i <= DILITHIUM; i++)
	{
		int res = ship->GetLoadedResources(i);
		if (ship->GetFleet())
			for (int j = 0; j < ship->GetFleet()->GetFleetSize(); j++)
				res += ship->GetFleet()->GetPointerOfShipFromFleet(j)->GetLoadedResources(i);
		// Lagerinhalt im Schiff zeichnen
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%d", res);		
		g->DrawString(s.AllocSysString(), -1, &font, RectF(0,120+i*60,725,60), &fontFormat, &fontBrush);
		
		if (systemOwner == shipOwner)
		{
			// Lagerinhalt im System zeichnen
			fontFormat.SetAlignment(StringAlignmentNear);
			s.Format("%d",pDoc->m_System[p.x][p.y].GetRessourceStore(i));
			g->DrawString(s.AllocSysString(), -1, &font, RectF(350,120+i*60,725,60), &fontFormat, &fontBrush);
			// kleine Pfeilbuttons zeichnen (links)
			fontFormat.SetAlignment(StringAlignmentCenter);
			g->FillRectangle(&SolidBrush(Color(100,30,30,30)), RectF(290,135+i*60,30,30));
			g->DrawRectangle(&Pen(penColor), RectF(290,135+i*60,30,30));		
			g->DrawString(L"-", -1, &font, RectF(290,135+i*60,30,30), &fontFormat, &fontBrush);
			// kleine Pfeilbuttons zeichnen (rechts)
			g->FillRectangle(&SolidBrush(Color(100,30,30,30)), RectF(755,135+i*60,30,30));
			g->DrawRectangle(&Pen(penColor), RectF(755,135+i*60,30,30));			
			g->DrawString(L"+", -1, &font, RectF(755,135+i*60,30,30), &fontFormat, &fontBrush);
		}
	}
	// verfügbaren Lagerraum im Schiff zeichnen:
	int usedStorage = ship->GetUsedStorageRoom(&pDoc->m_TroopInfo);
	int storageRoom = ship->GetStorageRoom();
	int troopNumber = ship->GetTransportedTroops()->GetSize();
	if (ship->GetFleet())
		for (int j = 0; j < ship->GetFleet()->GetFleetSize(); j++)
		{
			usedStorage += ship->GetFleet()->GetPointerOfShipFromFleet(j)->GetUsedStorageRoom(&pDoc->m_TroopInfo);
			storageRoom += ship->GetFleet()->GetPointerOfShipFromFleet(j)->GetStorageRoom();
			troopNumber += ship->GetFleet()->GetPointerOfShipFromFleet(j)->GetTransportedTroops()->GetSize();
		}
/*	for (int i = 0; i < ship->GetTransportedTroops()->GetSize(); i++)
	{
		BYTE id = ship->GetTransportedTroops()->GetAt(i).GetID();
		usedStorage += pDoc->m_TroopInfo.GetAt(id).GetSize();
	}
*/
	
	fontFormat.SetAlignment(StringAlignmentCenter);
	s.Format("%s: %d", CResourceManager::GetString("AVAILABLE"), storageRoom - usedStorage);
	g->DrawString(s.AllocSysString(), -1, &font, RectF(600,490,225,30), &fontFormat, &fontBrush);
	
	// Truppen im System und im Schiff zeichnen
	fontFormat.SetAlignment(StringAlignmentFar);
	s.Format("%d", troopNumber);
	g->DrawString(s.AllocSysString(), -1, &font, RectF(0,560,725,60), &fontFormat, &fontBrush);
	
	if (systemOwner == shipOwner)
	{
		s.Format("%d",pDoc->m_System[p.x][p.y].GetTroops()->GetSize());
		fontFormat.SetAlignment(StringAlignmentNear);
		g->DrawString(s.AllocSysString(), -1, &font, RectF(350,560,725,60), &fontFormat, &fontBrush);
		// kleine Pfeilbuttons zeichnen (links)
		fontFormat.SetAlignment(StringAlignmentCenter);
		g->FillRectangle(&SolidBrush(Color(100,30,30,30)), RectF(290,575,30,30));
		g->DrawRectangle(&Pen(penColor), RectF(290,575,30,30));		
		g->DrawString(L"-", -1, &font, RectF(290,575,30,30), &fontFormat, &fontBrush);
		// kleine Pfeilbuttons zeichnen (rechts)
		g->FillRectangle(&SolidBrush(Color(100,30,30,30)), RectF(755,575,30,30));
		g->DrawRectangle(&Pen(penColor), RectF(755,575,30,30));			
		g->DrawString(L"+", -1, &font, RectF(755,575,30,30), &fontFormat, &fontBrush);		
	}
	// Name der aktuell ausgewählten Truppe im System und auf dem Schiff zeichnen
	if (m_byTroopNumberInSystem >= pDoc->m_System[p.x][p.y].GetTroops()->GetSize())
		m_byTroopNumberInSystem = 0;
	if (m_byTroopNumberInShip >= ship->GetTransportedTroops()->GetSize())
		m_byTroopNumberInShip = 0;
	
	// Truppenbeschreibung auf der linken Seite, also die im System anzeigen
	//pDC->Rectangle(25,270,225,700);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
	if (pDoc->m_System[p.x][p.y].GetTroops()->GetSize() > 0 && systemOwner == shipOwner)
	{
		BYTE id = pDoc->m_System[p.x][p.y].GetTroops()->GetAt(m_byTroopNumberInSystem).GetID();
		// ein paar Daten zur ausgewählten Einheit werden links angezeigt
		fontBrush.SetColor(markColor);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = pDoc->m_TroopInfo.GetAt(id).GetName();
		g->DrawString(s.AllocSysString(), -1, &font, RectF(25,280,200,30), &fontFormat, &fontBrush);
				
		fontBrush.SetColor(normalColor);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format("%s: %d",CResourceManager::GetString("POWER"),pDoc->m_System[p.x][p.y].GetTroops()->GetAt(m_byTroopNumberInSystem).GetPower());
		g->DrawString(s.AllocSysString(), -1, &font, RectF(40,310,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("EXPERIANCE"),pDoc->m_System[p.x][p.y].GetTroops()->GetAt(m_byTroopNumberInSystem).GetExperiance());
		g->DrawString(s.AllocSysString(), -1, &font, RectF(40,340,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("PLACE"),pDoc->m_TroopInfo.GetAt(id).GetSize());
		g->DrawString(s.AllocSysString(), -1, &font, RectF(40,370,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("MORALVALUE"),pDoc->m_TroopInfo.GetAt(id).GetMoralValue());
		g->DrawString(s.AllocSysString(), -1, &font, RectF(40,400,185,30), &fontFormat, &fontBrush);
		
		fontFormat.SetLineAlignment(StringAlignmentNear);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = pDoc->m_TroopInfo.GetAt(id).GetDescription();
		g->DrawString(s.AllocSysString(), -1, &font, RectF(30,435,190,265), &fontFormat, &fontBrush);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
		
		s.Format("#%d: %s", m_byTroopNumberInSystem+1, pDoc->m_TroopInfo.GetAt(id).GetName());
		// Das Bild für die Truppe zeichnen
		CString file;
		file.Format("Troops\\%s.png", pDoc->m_TroopInfo.GetAt(id).GetName());
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
		if (graphic == NULL)
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Troops\\ImageMissing.png");
		if (graphic)
			g->DrawImage(graphic, 25, 95, 200, 150);		
	}
	else
		s = CResourceManager::GetString("NONE")+" "+CResourceManager::GetString("SELECTED");
	
	if (systemOwner == shipOwner)
	{
		fontFormat.SetAlignment(StringAlignmentNear);
		g->DrawString(s.AllocSysString(), -1, &font, RectF(290,620,198,60), &fontFormat, &fontBrush);		
	}
	// Truppenbeschreibung auf der rechten Seite, also die im Schiff anzeigen
	//pDC->Rectangle(850,270,1050,700);
	if (ship->GetTransportedTroops()->GetSize() > 0 && ship->GetFleet() == FALSE)
	{
		BYTE id = ship->GetTransportedTroops()->GetAt(m_byTroopNumberInShip).GetID();
		// ein paar Daten zur ausgewählten Einheit werden rechts angezeigt
		fontBrush.SetColor(markColor);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = pDoc->m_TroopInfo.GetAt(id).GetName();
		g->DrawString(s.AllocSysString(), -1, &font, RectF(850,280,200,30), &fontFormat, &fontBrush);
				
		fontBrush.SetColor(normalColor);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format("%s: %d",CResourceManager::GetString("POWER"), ship->GetTransportedTroops()->GetAt(m_byTroopNumberInShip).GetPower());
		g->DrawString(s.AllocSysString(), -1, &font, RectF(865,310,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("EXPERIANCE"), ship->GetTransportedTroops()->GetAt(m_byTroopNumberInShip).GetExperiance());
		g->DrawString(s.AllocSysString(), -1, &font, RectF(865,340,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("PLACE"),pDoc->m_TroopInfo.GetAt(id).GetSize());
		g->DrawString(s.AllocSysString(), -1, &font, RectF(865,370,185,30), &fontFormat, &fontBrush);
		s.Format("%s: %d",CResourceManager::GetString("MORALVALUE"),pDoc->m_TroopInfo.GetAt(id).GetMoralValue());
		g->DrawString(s.AllocSysString(), -1, &font, RectF(865,400,185,30), &fontFormat, &fontBrush);
		
		fontFormat.SetLineAlignment(StringAlignmentNear);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = pDoc->m_TroopInfo.GetAt(id).GetDescription();
		g->DrawString(s.AllocSysString(), -1, &font, RectF(855,435,190,265), &fontFormat, &fontBrush);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
		
		s.Format("#%d: %s", m_byTroopNumberInSystem+1, pDoc->m_TroopInfo.GetAt(id).GetName());
		// Das Bild für die Truppe zeichnen
		CString file;
		file.Format("Troops\\%s.png", pDoc->m_TroopInfo.GetAt(id).GetName());
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
		if (graphic == NULL)
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Troops\\ImageMissing.png");
		if (graphic)
			g->DrawImage(graphic, 850, 95, 200, 150);
	}
	else
		s = CResourceManager::GetString("NONE")+" "+CResourceManager::GetString("SELECTED");
	
	fontFormat.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &font, RectF(588,620,198,60), &fontFormat, &fontBrush);
	
	fontFormat.SetTrimming(StringTrimmingNone);

	// Button zum Ändern der Menge, wieviel pro Klick vom oder ins Schiff verschoben werden zeichnen, sowie den
	// Button um die nächste Einheit auf dem System bzw. auf dem Schiff zu wählen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.png");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);
	
	if (systemOwner == shipOwner)
	{
		fontFormat.SetAlignment(StringAlignmentFar);
		s = CResourceManager::GetString("MULTIPLIER");
		g->DrawString(s.AllocSysString(), -1, &font, RectF(0,490,457,30), &fontFormat, &fontBrush);
		
		if (graphic)
			g->DrawImage(graphic, 477,490,120,30);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s.Format("%d",m_iTransportStorageQuantity);
		g->DrawString(s.AllocSysString(), -1, &font, RectF(477,490,120,30), &fontFormat, &btnBrush);
		
		// Vor-Button links
		if (pDoc->m_System[p.x][p.y].GetTroops()->GetSize() > 0)
		{
			if (graphic)
				g->DrawImage(graphic, 290,670,120,30);
			s = CResourceManager::GetString("BTN_NEXT");
			g->DrawString(s.AllocSysString(), -1, &font, RectF(290,670,120,30), &fontFormat, &btnBrush);			
		}
		// Vor-Button rechts
		if (ship->GetTransportedTroops()->GetSize() && !ship->GetFleet())
		{
			if (graphic)
				g->DrawImage(graphic, 665,670,120,30);
			s = CResourceManager::GetString("BTN_NEXT");
			g->DrawString(s.AllocSysString(), -1, &font, RectF(665,670,120,30), &fontFormat, &btnBrush);	
		}
	}
	else if (ship->GetTransportedTroops()->GetSize() && !ship->GetFleet())
	{
		// Vor-Button rechts
		if (graphic)
			g->DrawImage(graphic, 665,670,120,30);
		s = CResourceManager::GetString("BTN_NEXT");
		g->DrawString(s.AllocSysString(), -1, &font, RectF(665,670,120,30), &fontFormat, &btnBrush);		
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
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);
}

void CTransportMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Wenn wir in der Transportansicht sind (brauchen auch nur Klicks überprüfen, wenn das Schiff Lagerraum hat)
	CShip* ship = &pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray());
			
	CPoint p = pDoc->GetKO();
	CString systemOwner = pDoc->m_System[p.x][p.y].GetOwnerOfSystem();
	CString shipOwner   = ship->GetOwnerOfShip();
	int usedStorage = 0;

	BOOLEAN isFleet = FALSE;
	int number = 1;
	if (ship->GetFleet())
	{
		isFleet = TRUE;
		number += ship->GetFleet()->GetFleetSize();
	}
			
	// Buttons um Ressourcen zu verschieben
	for (int i = TITAN; i <= DILITHIUM; i++)
		if (systemOwner == shipOwner)
		{
			int multi = 1;
			if (i == DILITHIUM)
				multi = 250;
			// kleine Pfeilbuttons checken (links) -> vom Schiff ins System
			if (CRect(290,105+i*60,320,165+i*60).PtInRect(point))
			{
				int oldQuantity = m_iTransportStorageQuantity;
				// Schiff und möglicherweise Schiffe in der Flotte durchgehen		
				for (int j = 0; j < number; j++)
				{
					// wenn mehr Ressourcen auf dem Schiff vorhanden sind als verschoben werden
					if (ship->GetLoadedResources(i) >= m_iTransportStorageQuantity)
					{
						pDoc->m_System[p.x][p.y].SetRessourceStore(i, m_iTransportStorageQuantity);
						ship->SetLoadedResources(-m_iTransportStorageQuantity, i);
						m_iTransportStorageQuantity = 0;
					}
					// wenn man mehr verschieben wollen als auf dem Schiff vorhanden sind
					else
					{
						m_iTransportStorageQuantity -= ship->GetLoadedResources(i);
						pDoc->m_System[p.x][p.y].SetRessourceStore(i, ship->GetLoadedResources(i));
						ship->SetLoadedResources(-ship->GetLoadedResources(i), i);
					}
					if (m_iTransportStorageQuantity == NULL)
						break;
					if (isFleet && j < number-1)
						ship = (&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()))->GetFleet()->GetPointerOfShipFromFleet(j);
				}
				m_iTransportStorageQuantity = oldQuantity;
				Invalidate(FALSE);
				return;
			}
			
			// kleine Pfeilbuttons checken (rechts) -> auch checken ob genug Lagerraum vorhanden ist
			if (CRect(755,105+i*60,785,165+i*60).PtInRect(point))
			{
				int oldQuantity = m_iTransportStorageQuantity;
				int transportedRes = oldQuantity;
				// Schiff und möglicherweise Schiffe in der Flotte durchgehen		
				for (int j = 0; j < number; j++)
				{
					m_iTransportStorageQuantity = oldQuantity;
					usedStorage = ship->GetUsedStorageRoom(&pDoc->m_TroopInfo);
					if (m_iTransportStorageQuantity*multi > ship->GetStorageRoom() - usedStorage)
						m_iTransportStorageQuantity = (ship->GetStorageRoom() - usedStorage) / multi;
					if (m_iTransportStorageQuantity > transportedRes)
						m_iTransportStorageQuantity = transportedRes;
					// wenn im System mehr Ressourcen vorhanden sind, als man verschieben möchte
					if (pDoc->m_System[p.x][p.y].GetRessourceStore(i) >= m_iTransportStorageQuantity)
					{
						transportedRes -= m_iTransportStorageQuantity;
						pDoc->m_System[p.x][p.y].SetRessourceStore(i, -m_iTransportStorageQuantity);
						ship->SetLoadedResources(m_iTransportStorageQuantity, i);
					}
					// wenn im System weniger Ressourcen vorhanden sind, als man aufs Schiff verschieben möchte
					else
					{
						transportedRes -= pDoc->m_System[p.x][p.y].GetRessourceStore(i);
						ship->SetLoadedResources(pDoc->m_System[p.x][p.y].GetRessourceStore(i), i);
						pDoc->m_System[p.x][p.y].SetRessourceStore(i, -(int)pDoc->m_System[p.x][p.y].GetRessourceStore(i));
					}
					if (transportedRes == NULL)
						break;
					if (isFleet && j < number-1)
						ship = (&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()))->GetFleet()->GetPointerOfShipFromFleet(j);
				}
				m_iTransportStorageQuantity = oldQuantity;
				Invalidate(FALSE);
				return;
			}
		}
	// Buttons um Truppen zu verschieben
	if (systemOwner == shipOwner)
	{
		// kleine Pfeilbuttons zeichnen (links)
		if (CRect(290,575,320,605).PtInRect(point))
		{
			// Schiff und möglicherweise Schiffe in der Flotte durchgehen		
			for (int j = 0; j < number; j++)
			{
				if (ship->GetTransportedTroops()->GetSize() > 0)
				{
					pDoc->m_System[p.x][p.y].GetTroops()->Add(ship->GetTransportedTroops()->GetAt(m_byTroopNumberInShip));
					ship->GetTransportedTroops()->RemoveAt(m_byTroopNumberInShip);
					if (m_byTroopNumberInShip > 0)
						m_byTroopNumberInShip--;
					m_byTroopNumberInSystem = pDoc->m_System[p.x][p.y].GetTroops()->GetUpperBound();
					Invalidate(FALSE);
					return;
				}
				if (isFleet && j < number-1)
					ship = (&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()))->GetFleet()->GetPointerOfShipFromFleet(j);
			}
		}
		// kleine Pfeilbuttons zeichnen (rechts) -> checken das noch genügend Lagerraum im Schiff vorhanden ist
		else if (CRect(755,575,785,605).PtInRect(point) && pDoc->m_System[p.x][p.y].GetTroops()->GetSize() > 0)
		{
			// Schiff und möglicherweise Schiffe in der Flotte durchgehen		
			for (int j = 0; j < number; j++)
			{
				BYTE id = pDoc->m_System[p.x][p.y].GetTroops()->GetAt(m_byTroopNumberInSystem).GetID();
				// durch Truppen und Ressourcen auf Schiff
				usedStorage = ship->GetUsedStorageRoom(&pDoc->m_TroopInfo);
				// dazu der benötigte Platz durch die Truppe, welche hinzukommen soll
				usedStorage += pDoc->m_TroopInfo.GetAt(id).GetSize();
				if (usedStorage <= ship->GetStorageRoom())
				{
					ship->GetTransportedTroops()->Add(pDoc->m_System[p.x][p.y].GetTroops()->GetAt(m_byTroopNumberInSystem));
					pDoc->m_System[p.x][p.y].GetTroops()->RemoveAt(m_byTroopNumberInSystem);
					if (m_byTroopNumberInSystem > 0)
						m_byTroopNumberInSystem--;
					m_byTroopNumberInShip = ship->GetTransportedTroops()->GetUpperBound();
					Invalidate(FALSE);
					return;
				}
				if (isFleet && j < number-1)
					ship = (&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()))->GetFleet()->GetPointerOfShipFromFleet(j);
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
		else if (pDoc->m_System[p.x][p.y].GetTroops()->GetSize() > 0 && CRect(290,670,410,700).PtInRect(point))
		{
			m_byTroopNumberInSystem++;
			Invalidate(FALSE);
		}
		// Vor-Button rechts
		else if (CRect(665,670,785,700).PtInRect(point) &&
			ship->GetTransportedTroops()->GetSize() > 0 && ship->GetFleet() == FALSE)
		{
			m_byTroopNumberInShip++;
			Invalidate(FALSE);
		}
	}
	else if (CRect(665,670,785,700).PtInRect(point) &&
		ship->GetTransportedTroops()->GetSize() > 0 && ship->GetFleet() == FALSE)
	{
		// Vor-Button rechts
		m_byTroopNumberInShip++;
		Invalidate(FALSE);
	}	

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CTransportMenuView::CreateButtons()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);

	// alle Buttons in der View anlegen und Grafiken laden
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
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);

	pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pMajor->GetRaceID());

	CMainBaseView::OnRButtonDown(nFlags, point);
}
