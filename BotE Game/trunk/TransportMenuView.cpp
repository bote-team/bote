// TransportMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "TransportMenuView.h"


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
END_MESSAGE_MAP()


// CTransportMenuView drawing

void CTransportMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here

	CMemDC pDC(dc);
	pDC->SetBkMode(TRANSPARENT);
	if (pDoc->m_pIniLoader->GetValue("SMOOTHSCALING"))
		pDC->SetStretchBltMode(HALFTONE);
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	LoadRaceFont(pDC);
	// ***************************** DIE SCHIFFSTRANSPORT ANSICHT ZEICHNEN **********************************
	DrawTransportMenue(pDC, r);
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

	CreateButtons();

	// alle Hintergrundgrafiken laden
	bg_transportmenu.DeleteObject();

	CString race;
	switch (pDoc->GetPlayersRace())
	{
	case HUMAN:		race = CResourceManager::GetString("RACE1_PREFIX"); break;
	case FERENGI:	race = CResourceManager::GetString("RACE2_PREFIX"); break;
	case KLINGON:	race = CResourceManager::GetString("RACE3_PREFIX"); break;
	case ROMULAN:	race = CResourceManager::GetString("RACE4_PREFIX"); break;
	case CARDASSIAN:race = CResourceManager::GetString("RACE5_PREFIX"); break;
	case DOMINION:	race = CResourceManager::GetString("RACE6_PREFIX"); break;
	}
	FCObjImage img;
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"transportmenu.jpg");
	bg_transportmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Destroy();	
	
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
void CTransportMenuView::DrawTransportMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r = theClientRect;
	CString s;
	CPoint p = pDoc->GetKO();
	BYTE systemOwner = pDoc->m_System[p.x][p.y].GetOwnerOfSystem();
	BYTE shipOwner = pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip();

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_transportmenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);
	CBitmap* graphic = NULL;
	HGDIOBJ oldGraphic;

	// grobe Linien zeichnen
	COLORREF oldColor = pDC->GetTextColor();
/*	CPen mark(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&mark);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-50);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-50);
	pDC->MoveTo(theClientRect.left+25,theClientRect.top+70);
	pDC->LineTo(theClientRect.left+25,theClientRect.bottom-50);
	pDC->MoveTo(theClientRect.left+250,theClientRect.top+70);
	pDC->LineTo(theClientRect.left+250,theClientRect.bottom-50);
	pDC->MoveTo(theClientRect.left+225,theClientRect.top+70);
	pDC->LineTo(theClientRect.left+225,theClientRect.bottom-50);
	
	pDC->MoveTo(theClientRect.right-25,theClientRect.top+70);
	pDC->LineTo(theClientRect.right-25,theClientRect.bottom-50);
	pDC->MoveTo(theClientRect.right-250,theClientRect.top+70);
	pDC->LineTo(theClientRect.right-250,theClientRect.bottom-50);
	pDC->MoveTo(theClientRect.right-225,theClientRect.top+70);
	pDC->LineTo(theClientRect.right-225,theClientRect.bottom-50);
*/
	pDC->SetTextColor(RGB(220,220,220));
	if (systemOwner == shipOwner)
		pDC->DrawText(CResourceManager::GetString("SYSTEM_STORAGE"), CRect(200,80,538,105), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SHIP_STORAGE"), CRect(538,80,875,105), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("TITAN"), CRect(0,120,1075,180), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DEUTERIUM"), CRect(0,180,1075,240), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DURANIUM"), CRect(0,240,1075,300), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("CRYSTAL"), CRect(0,300,1075,360), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("IRIDIUM"), CRect(0,360,1075,420), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DILITHIUM"), CRect(0,420,1075,480), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("TROOPS"), CRect(0,560,1075,620), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SELECTED"), CRect(0,620,1075,680), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SetTextColor(oldColor);


	// Inhalte des system- und globalen Lagers zeichnen
	CBrush grey(RGB(40,40,40));
	pDC->SelectObject(&grey);
	CShip* ship = &pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray());
	for (int i = TITAN; i <= DILITHIUM; i++)
	{
		int res = ship->GetLoadedResources(i);
		if (ship->GetFleet())
			for (int j = 0; j < ship->GetFleet()->GetFleetSize(); j++)
				res += ship->GetFleet()->GetPointerOfShipFromFleet(j)->GetLoadedResources(i);
		// Lagerinhalt im Schiff zeichnen
		s.Format("%d", res);
		pDC->DrawText(s, CRect(0,120+i*60,725,180+i*60), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		if (systemOwner == shipOwner)
		{
			// Lagerinhalt im System zeichnen
			s.Format("%d",pDoc->m_System[p.x][p.y].GetRessourceStore(i));
			pDC->DrawText(s, CRect(350,120+i*60,1075,180+i*60), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			// kleine Pfeilbuttons zeichnen (links)
			pDC->Rectangle(290,135+i*60,320,165+i*60);
			pDC->DrawText("<",CRect(290,135+i*60,320,165+i*60), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			// kleine Pfeilbuttons zeichnen (rechts)
			pDC->Rectangle(755,135+i*60,785,165+i*60);
			pDC->DrawText(">",CRect(755,135+i*60,785,165+i*60), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
	
	s.Format("%s: %d", CResourceManager::GetString("AVAILABLE"), storageRoom - usedStorage);
	pDC->DrawText(s, CRect(600,490,825,520), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Truppen im System und im Schiff zeichnen
	s.Format("%d", troopNumber);
	pDC->DrawText(s, CRect(0,560,725,620), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	if (systemOwner == shipOwner)
	{
		s.Format("%d",pDoc->m_System[p.x][p.y].GetTroops()->GetSize());
		pDC->DrawText(s, CRect(350,560,1075,620), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		// kleine Pfeilbuttons zeichnen (links)
		pDC->Rectangle(290,575,320,605);
		pDC->DrawText("<",CRect(290,575,320,605), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		// kleine Pfeilbuttons zeichnen (rechts)
		pDC->Rectangle(755,575,785,605);
		pDC->DrawText(">",CRect(755,575,785,605), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	// Name der aktuell ausgewählten Truppe im System und auf dem Schiff zeichnen
	if (m_byTroopNumberInSystem >= pDoc->m_System[p.x][p.y].GetTroops()->GetSize())
		m_byTroopNumberInSystem = 0;
	if (m_byTroopNumberInShip >= ship->GetTransportedTroops()->GetSize())
		m_byTroopNumberInShip = 0;
	// Truppenbeschreibung auf der linken Seite, also die im System anzeigen
	//pDC->Rectangle(25,270,225,700);
	if (pDoc->m_System[p.x][p.y].GetTroops()->GetSize() > 0 && systemOwner == shipOwner)
	{
		BYTE id = pDoc->m_System[p.x][p.y].GetTroops()->GetAt(m_byTroopNumberInSystem).GetID();
		// ein paar Daten zur ausgewählten Einheit werden links angezeigt
		pDC->SetTextColor(RGB(220,220,220));
		pDC->DrawText(pDoc->m_TroopInfo.GetAt(id).GetName(), CRect(25,280,225,310), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		pDC->SetTextColor(oldColor);
		s.Format("%s: %d",CResourceManager::GetString("POWER"),pDoc->m_System[p.x][p.y].GetTroops()->GetAt(m_byTroopNumberInSystem).GetPower());
		pDC->DrawText(s, CRect(40,310,225,340), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		s.Format("%s: %d",CResourceManager::GetString("EXPERIANCE"),pDoc->m_System[p.x][p.y].GetTroops()->GetAt(m_byTroopNumberInSystem).GetExperiance());
		pDC->DrawText(s, CRect(40,340,225,370), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		s.Format("%s: %d",CResourceManager::GetString("PLACE"),pDoc->m_TroopInfo.GetAt(id).GetSize());
		pDC->DrawText(s, CRect(40,370,225,400), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		s.Format("%s: %d",CResourceManager::GetString("MORALVALUE"),pDoc->m_TroopInfo.GetAt(id).GetMoralValue());
		pDC->DrawText(s, CRect(40,400,225,430), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		pDC->DrawText(pDoc->m_TroopInfo.GetAt(id).GetDescription(), CRect(30,435,220,700), DT_LEFT | DT_TOP | DT_WORDBREAK);
		s.Format("#%d: %s", m_byTroopNumberInSystem+1, pDoc->m_TroopInfo.GetAt(id).GetName());
		// Das Bild für die Truppe zeichnen
		CString file;
		file.Format("Troops\\%s.jpg",pDoc->m_TroopInfo.GetAt(id).GetName());
		graphic = pDoc->GetGraphicPool()->GetGraphic(file);
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGraphic("Troops\\ImageMissing.jpg");
		if (graphic)
		{
			oldGraphic = mdc.SelectObject(*graphic);
			pDC->BitBlt(25,95,200,150,&mdc,0,0,SRCCOPY);
			mdc.SelectObject(oldGraphic);
			graphic = NULL;
		}
	}
	else
		s = CResourceManager::GetString("NONE")+" "+CResourceManager::GetString("SELECTED");
	if (systemOwner == shipOwner)
	{
		// Beim Romulaner ist das ein wenig nach oben verschoben
		if (shipOwner == ROMULAN)
			pDC->DrawText(s,CRect(290,600,488,660), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		else
			pDC->DrawText(s,CRect(290,620,488,680), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	}
	// Truppenbeschreibung auf der rechten Seite, also die im Schiff anzeigen
	//pDC->Rectangle(850,270,1050,700);
	if (ship->GetTransportedTroops()->GetSize() > 0 && ship->GetFleet() == FALSE)
	{
		BYTE id = ship->GetTransportedTroops()->GetAt(m_byTroopNumberInShip).GetID();
		// ein paar Daten zur ausgewählten Einheit werden rechts angezeigt
		pDC->SetTextColor(RGB(220,220,220));
		pDC->DrawText(pDoc->m_TroopInfo.GetAt(id).GetName(), CRect(850,280,1050,310), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		pDC->SetTextColor(oldColor);
		s.Format("%s: %d",CResourceManager::GetString("POWER"),
			ship->GetTransportedTroops()->GetAt(m_byTroopNumberInShip).GetPower());
		pDC->DrawText(s, CRect(865,310,1050,340), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		s.Format("%s: %d",CResourceManager::GetString("EXPERIANCE"),
			ship->GetTransportedTroops()->GetAt(m_byTroopNumberInShip).GetExperiance());
		pDC->DrawText(s, CRect(865,340,1050,370), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		s.Format("%s: %d",CResourceManager::GetString("PLACE"), pDoc->m_TroopInfo.GetAt(id).GetSize());
		pDC->DrawText(s, CRect(865,370,1050,400), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		s.Format("%s: %d",CResourceManager::GetString("MORALVALUE"), pDoc->m_TroopInfo.GetAt(id).GetMoralValue());
		pDC->DrawText(s, CRect(865,400,1050,430), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
		pDC->DrawText(pDoc->m_TroopInfo.GetAt(id).GetDescription(), CRect(855,435,1045,700), DT_LEFT | DT_TOP | DT_WORDBREAK);
		s.Format("#%d: %s", m_byTroopNumberInShip+1, pDoc->m_TroopInfo.GetAt(id).GetName());
		// Das Bild für die Truppe zeichnen
		CString file;
		file.Format("Troops\\%s.jpg",pDoc->m_TroopInfo.GetAt(id).GetName());
		graphic = pDoc->GetGraphicPool()->GetGraphic(file);
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGraphic("Troops\\ImageMissing.jpg");
		if (graphic)
		{
			oldGraphic = mdc.SelectObject(*graphic);
			pDC->BitBlt(850,95,200,150,&mdc,0,0,SRCCOPY);
			mdc.SelectObject(oldGraphic);
			graphic = NULL;
		}		
	}
	else
		s = CResourceManager::GetString("NONE")+" "+CResourceManager::GetString("SELECTED");
	// Beim Romulaner ist das ein wenig nach oben verschoben
	if (shipOwner == ROMULAN)
		pDC->DrawText(s,CRect(588,600,785,660), DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	else
		pDC->DrawText(s,CRect(588,620,785,680), DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	
	// Button zum Ändern der Menge, wieviel pro Klick vom oder ins Schiff verschoben werden zeichnen, sowie den
	// Button um die nächste Einheit auf dem System bzw. auf dem Schiff zu wählen
	if (systemOwner == shipOwner)
	{
		pDC->DrawText(CResourceManager::GetString("MULTIPLIER"),CRect(0,490,457,520), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		LoadFontForLittleButton(pDC);
		mdc.SelectObject(bm);
		pDC->BitBlt(477,490,120,30,&mdc,0,0,SRCCOPY);
		s.Format("%d",m_iTransportStorageQuantity);
		pDC->DrawText(s,CRect(477,490,597,520), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		// Vor-Button links
		if (pDoc->m_System[p.x][p.y].GetTroops()->GetSize() > 0)
		{
			pDC->BitBlt(290,670,120,30,&mdc,0,0,SRCCOPY);
			pDC->DrawText(CResourceManager::GetString("BTN_NEXT"), CRect(290,670,410,700), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		// Vor-Button rechts
		if (ship->GetTransportedTroops()->GetSize() && !ship->GetFleet())
		{
			pDC->BitBlt(665,670,120,30,&mdc,0,0,SRCCOPY);
			pDC->DrawText(CResourceManager::GetString("BTN_NEXT"), CRect(665,670,785,700), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}
	else if (ship->GetTransportedTroops()->GetSize() && !ship->GetFleet())
	{
		LoadFontForLittleButton(pDC);
		mdc.SelectObject(bm);
		// Vor-Button rechts
		pDC->BitBlt(665,670,120,30,&mdc,0,0,SRCCOPY);
		pDC->DrawText(CResourceManager::GetString("BTN_NEXT"), CRect(665,670,785,700), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
		
	// Transport auf "Systemname" mit größerer Schrift in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	// Transport auf "Systemname" mit größerer Schrift in der Mitte zeichnen
	r.SetRect(theClientRect.left,10,theClientRect.right,60);
	if (systemOwner == shipOwner)
		s = CResourceManager::GetString("TRANSPORT_MENUE")+" "+pDoc->GetSector(p.x,p.y).GetName();
	else
		s = CResourceManager::GetString("TRANSPORT_MENUE")+" "+CResourceManager::GetString("SHIP");
	pDC->DrawText(s, r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CTransportMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Wenn wir in der Transportansicht sind (brauchen auch nur Klicks überprüfen, wenn das Schiff Lagerraum hat)
	CShip* ship = &pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray());
			
	CPoint p = pDoc->GetKO();
	BYTE systemOwner = pDoc->m_System[p.x][p.y].GetOwnerOfSystem();
	BYTE shipOwner   = ship->GetOwnerOfShip();
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
	// alle Buttons in der View anlegen und Grafiken laden
	switch(pDoc->GetPlayersRace())
	{
	case HUMAN:
		{
			break;
		}
	case FERENGI:
		{
			break;
		}
	case KLINGON:
		{
			break;
		}
	case ROMULAN:
		{
			break;
		}
	case CARDASSIAN:
		{
			break;
		}
	case DOMINION:
		{
			break;
		}
	}
}