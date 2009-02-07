// FleetMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "GalaxyMenuView.h"
#include "FleetMenuView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"


// CFleetMenuView

IMPLEMENT_DYNCREATE(CFleetMenuView, CMainBaseView)

CFleetMenuView::CFleetMenuView()
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
	m_iOldShipInFleet = -1;
	m_bDrawFullFleet = TRUE;
	m_bShowNextButton = FALSE;
	m_bShowBackButton = FALSE;
}
// CFleetMenuView drawing

void CFleetMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here
	dc->SetBkMode(TRANSPARENT);
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	LoadRaceFont(dc);
	// ***************************** DIE FLOTTENBILDUNGSANSICHT ZEICHNEN **********************************
	DrawFleetMenue(dc, r);
	// ************** DIE FLOTTENANSICHT ZEICHNEN ist hier zu Ende **************
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

	CreateButtons();

	// alle Hintergrundgrafiken laden
	bg_fleetmenu.DeleteObject();
	
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
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"fleetmenu.jpg");
	bg_fleetmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Destroy();	
	
	// Flottenansicht
	m_iFleetPage = 1;
	m_iOldShipInFleet = -1;
	m_bDrawFullFleet = TRUE;
	m_bShowNextButton = FALSE;
	m_bShowBackButton = FALSE;		
		
}

void CFleetMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CFleetMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Flottenmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CFleetMenuView::DrawFleetMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	
	if (m_bDrawFullFleet == TRUE)
	{
		CDC* dc = pDC;
		CMemDC pDC(dc);
		LoadRaceFont(pDC);
		
		FCObjImage shipbm;
		CBrush nb(RGB(0,0,0));
		pDC->SelectObject(nb);
		pDC->Rectangle(theClientRect);

		CDC mdc;
		mdc.CreateCompatibleDC(pDC);
		mdc.SelectObject(bg_fleetmenu);
		pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);

		// grobe Linien zeichnen
		COLORREF oldColor = pDC->GetTextColor();
/*		CPen mark(PS_SOLID, 1, RGB(125,175,255));
		pDC->SelectObject(&mark);
		pDC->MoveTo(theClientRect.left,theClientRect.top+70);
		pDC->LineTo(theClientRect.right,theClientRect.top+70);
		pDC->MoveTo(theClientRect.left+250,theClientRect.top+70);
		pDC->LineTo(theClientRect.left+250,theClientRect.bottom);
		pDC->MoveTo(theClientRect.right-250,theClientRect.top+70);
		pDC->LineTo(theClientRect.right-250,theClientRect.bottom);
*/
		pDC->SetBkMode(TRANSPARENT);
		CString s;
		// Hier die Buttons einzeigen, mit denen wir alle Schiffe im Sektor, Schiffe der gleichen Klasse oder
		// Schiffe des gleichen Types hinzufügen bzw. entfernen können
		// der gleichen Klasse hinzufügen
		if (pDoc->GetPlayersRace() == KLINGON)
		{
			pDC->SetTextColor(RGB(200,200,200));
			pDC->DrawText(CResourceManager::GetString("WHAT_SHIPS_TO_FLEET"),CRect(20,140,230,215),DT_CENTER | DT_WORDBREAK);
		}
		else if (pDoc->GetPlayersRace() == ROMULAN)
			pDC->DrawText(CResourceManager::GetString("WHAT_SHIPS_TO_FLEET"),CRect(20,175,230,250),DT_CENTER | DT_WORDBREAK);
		else
		{
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(CResourceManager::GetString("WHAT_SHIPS_TO_FLEET"),CRect(20,140,230,215),DT_CENTER | DT_WORDBREAK);
		}
		pDC->SetTextColor(oldColor);
		s.Format("%s-%s",pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipClass(),CResourceManager::GetString("CLASS"));
		pDC->DrawText(s,CRect(0,220,250,250),DT_CENTER | DT_SINGLELINE |DT_VCENTER);
		// des gleichen Types hinzufügen
		s.Format("%s %s",CResourceManager::GetString("TYPE"),pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipTypeAsString());
		pDC->DrawText(s,CRect(0,270,250,300),DT_CENTER | DT_SINGLELINE |DT_VCENTER);
		// alle Schiffe hinzufügen
		pDC->DrawText(CResourceManager::GetString("ALL_SHIPS"),CRect(0,320,250,350),DT_CENTER | DT_SINGLELINE |DT_VCENTER);
		if (pDoc->GetPlayersRace() == KLINGON)
		{
			pDC->SetTextColor(RGB(200,200,200));
			pDC->DrawText(CResourceManager::GetString("WHAT_SHIPS_FROM_FLEET"),CRect(20,400,230,475),DT_CENTER | DT_WORDBREAK);
		}
		else if (pDoc->GetPlayersRace() == ROMULAN)
			pDC->DrawText(CResourceManager::GetString("WHAT_SHIPS_FROM_FLEET"),CRect(20,415,230,490),DT_CENTER | DT_WORDBREAK);
		else
		{
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(CResourceManager::GetString("WHAT_SHIPS_FROM_FLEET"),CRect(20,400,230,475),DT_CENTER | DT_WORDBREAK);
		}
		pDC->SetTextColor(oldColor);
		// fremder Klassen entfernen
		s.Format("%s %s-%s",CResourceManager::GetString("NOT"),
			pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipClass(),CResourceManager::GetString("CLASS"));
		pDC->DrawText(s,CRect(0,480,250,510),DT_CENTER | DT_SINGLELINE |DT_VCENTER);
		// fremden Types entfernen
		s.Format("%s %s %s",CResourceManager::GetString("NOT"),CResourceManager::GetString("TYPE"),
			pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipTypeAsString());
		pDC->DrawText(s,CRect(0,530,250,560),DT_CENTER | DT_SINGLELINE |DT_VCENTER);
		// alle Schiffe entfernen
		pDC->DrawText(CResourceManager::GetString("ALL_SHIPS"),CRect(0,580,250,610),DT_CENTER | DT_SINGLELINE |DT_VCENTER);

		USHORT counter = 0;
		USHORT column = 1;
		USHORT row = 0;
		shipbm.Destroy();
		int oldStretchMode = pDC->GetStretchBltMode();
		pDC->SetStretchBltMode(HALFTONE);

		// Erstmal das Schiff anzeigen, welches die Flotte beinhaltet (nur auf erster Seite!)
		if (m_iFleetPage == 1)
		{
			CBitmap* graphic = NULL;
			HGDIOBJ oldGraphic;
			s.Format("Ships\\%s.jpg",pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipClass());
			graphic = pDoc->GetGraphicPool()->GetGraphic(s);
			if (graphic == NULL)
				graphic = pDoc->GetGraphicPool()->GetGraphic("Ships\\ImageMissing.jpg");
			if (graphic)
			{
				oldGraphic = mdc.SelectObject(*graphic);
				pDC->StretchBlt(250*column+37,row*65+90,65,49,&mdc,0,0,200,150,SRCCOPY);
				mdc.SelectObject(oldGraphic);
			}
			// Erfahrungsstufen des Schiffes anzeigen
			int bmHeight = 0;
			switch (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetExpLevel())
			{
			case 1: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_beginner.png");	bmHeight = 8;	break;
			case 2: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_normal.png");	bmHeight = 16;	break;
			case 3: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_profi.png");	bmHeight = 24;	break;
			case 4: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_veteran.png");	bmHeight = 32;	break;
			case 5: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_elite.png");	bmHeight = 40;	break;
			case 6: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_legend.png");	bmHeight = 48;	break;
			default: graphic = NULL;
			}
			if (graphic)
			{
				oldGraphic = mdc.SelectObject(*graphic);
				pDC->BitBlt(250 * column + 29, row * 65 + 90 + 49 - bmHeight, 8, bmHeight, &mdc, 0, 0, SRCPAINT);
				mdc.SelectObject(oldGraphic);
			}

			// Hier die Striche für Schilde und Hülle neben dem Schiffsbild anzeigen (jeweils max. 20)
			USHORT hullProz = (UINT)(pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetHull()->GetCurrentHull() * 20 / pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetHull()->GetMaxHull());
			CPen penColor(PS_SOLID,0,RGB(240-hullProz*12,0+hullProz*12,0));
			pDC->SelectObject(&penColor);
			for (USHORT n = 0; n <= hullProz; n++)
				pDC->Rectangle(250*column+102,row*65+135-n*2,250*column+107,row*65+134-n*2);
			USHORT shieldProz = 0;
			if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShield()->GetMaxShield() > 0)
				shieldProz = (UINT)(pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShield()->GetCurrentShield() * 20 / pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShield()->GetMaxShield());
			CPen penColor2(PS_SOLID,0,RGB(240-shieldProz*12,80,0+shieldProz*12));
			pDC->SelectObject(&penColor2);
			if (shieldProz > 0)
				for (USHORT n = 0; n <= shieldProz; n++)
					pDC->Rectangle(250*column+109,row*65+135-n*2,250*column+114,row*65+134-n*2);

			// Wenn es das Flagschiff unseres Imperiums ist, dann kleines Zeichen zeichnen
			if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetIsShipFlagShip() == TRUE)
			{
				pDC->SetTextColor(RGB(255,255,255));
				pDC->TextOut(250*column+45,row*65+95,"Flag");
			}
			if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetCloak())
				pDC->SetTextColor(RGB(80,80,80));
			else
				pDC->SetTextColor(RGB(170,170,170));
			// Wenn wir ein Schiff markiert haben, dann Markierung zeichnen
			// also der Mauszeiger ist über dem 1. Schiff in der Liste
			if (pDoc->GetNumberOfTheShipInFleet() == 0)
			{
				if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetCloak())
					pDC->SetTextColor(RGB(150,0,0));
				else
					pDC->SetTextColor(RGB(255,0,0));
			}
			pDC->TextOut(250*column+120,row*65+90,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipName());
			pDC->TextOut(250*column+120,row*65+110,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipClass()+"-"+CResourceManager::GetString("CLASS"));
		}
		counter++;
		row++;
	
		// Wenn das Schiff eine Flotte anführt, dann Schiffe in dieser Flotte anzeigen
		if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet() != 0)
			for (USHORT i = 0; i < pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetFleetSize(); i++)
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
					CBitmap* graphic = NULL;
					HGDIOBJ oldGraphic;
					s.Format("Ships\\%s.jpg",pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetShipClass());
					graphic = pDoc->GetGraphicPool()->GetGraphic(s);
					if (graphic == NULL)
						graphic = pDoc->GetGraphicPool()->GetGraphic("Ships\\ImageMissing.jpg");
					if (graphic)
					{
						oldGraphic = mdc.SelectObject(*graphic);
						pDC->StretchBlt(250*column+37,row*65+90,65,49,&mdc,0,0,200,150,SRCCOPY);
						mdc.SelectObject(oldGraphic);
					}
					// Erfahrungsstufen des Schiffes anzeigen
					int bmHeight = 0;
					switch (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetExpLevel())
					{
					case 1: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_beginner.png");	bmHeight = 8;	break;
					case 2: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_normal.png");	bmHeight = 16;	break;
					case 3: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_profi.png");	bmHeight = 24;	break;
					case 4: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_veteran.png");	bmHeight = 32;	break;
					case 5: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_elite.png");	bmHeight = 40;	break;
					case 6: graphic = pDoc->GetGraphicPool()->GetGraphic("Other\\xp_legend.png");	bmHeight = 48;	break;
					default: graphic = NULL;
					}
					if (graphic)
					{
						oldGraphic = mdc.SelectObject(*graphic);
						pDC->BitBlt(250 * column + 29, row * 65 + 90 + 49 - bmHeight, 8, bmHeight, &mdc, 0, 0, SRCPAINT);
						mdc.SelectObject(oldGraphic);
					}
					
					// Hier die Striche für Schilde und Hülle neben dem Schiffsbild anzeigen (jeweils max. 20)
					USHORT hullProz = (UINT)(pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetHull()->GetCurrentHull() * 20 / pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetHull()->GetMaxHull());
					CPen penColor(PS_SOLID,0,RGB(240-hullProz*12,0+hullProz*12,0));
					pDC->SelectObject(&penColor);
					for (USHORT n = 0; n <= hullProz; n++)
						pDC->Rectangle(250*column+102,row*65+135-n*2,250*column+107,row*65+134-n*2);
					USHORT shieldProz = 0;
					if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetShield()->GetMaxShield() > 0)
						shieldProz = (UINT)(pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetShield()->GetCurrentShield() * 20 / pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetShield()->GetMaxShield());
					CPen penColor2(PS_SOLID,0,RGB(240-shieldProz*12,80,0+shieldProz*12));
					pDC->SelectObject(&penColor2);
					if (shieldProz > 0)
						for (USHORT n = 0; n <= shieldProz; n++)
							pDC->Rectangle(250*column+109,row*65+135-n*2,250*column+114,row*65+134-n*2);
									
					// Wenn es das Flagschiff unseres Imperiums ist, dann kleines Zeichen zeichnen
					if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetIsShipFlagShip() == TRUE)
					{
						pDC->SetTextColor(RGB(255,255,255));
						pDC->TextOut(250*column+45,row*65+95,"Flag");
					}
					if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetCloak())
						pDC->SetTextColor(RGB(80,80,80));
					else
						pDC->SetTextColor(RGB(170,170,170));
					// Wenn wir ein Schiff markiert haben, dann Markierung zeichnen
					if (i+1 == pDoc->GetNumberOfTheShipInFleet())
					{
						if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetCloak())
							pDC->SetTextColor(RGB(150,0,0));
						else
							pDC->SetTextColor(RGB(255,0,0));
					}						
					pDC->TextOut(250*column+120,row*65+90,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetShipName());
					pDC->TextOut(250*column+120,row*65+110,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetShipClass()+"-"+CResourceManager::GetString("CLASS"));
				}
				row++;
				counter++;
				if (counter > m_iFleetPage*18)
					break;
			}
			pDC->SetStretchBltMode(oldStretchMode);
			// Die Buttons für vor und zurück darstellen, wenn wir mehr als 9 Schiffe in dem Sektor sehen
			this->LoadFontForLittleButton(pDC);
			m_bShowNextButton = FALSE;
			if (counter > 18 && counter > m_iFleetPage*18)
			{
				m_bShowNextButton = TRUE;
				mdc.SelectObject(bm2);
				pDC->BitBlt(540,theClientRect.bottom-70,120,30,&mdc,0,0,SRCCOPY);
				pDC->DrawText(CResourceManager::GetString("BTN_NEXT"),CRect(540,theClientRect.bottom-70,660,theClientRect.bottom-40),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			}
			// back-Button
			m_bShowBackButton = FALSE;
			if (m_iFleetPage > 1)
			{
				m_bShowBackButton = TRUE;
				mdc.SelectObject(bm2);
				pDC->BitBlt(286,theClientRect.bottom-70,120,30,&mdc,0,0,SRCCOPY);
				pDC->DrawText(CResourceManager::GetString("BTN_BACK"),CRect(290,theClientRect.bottom-70,410,theClientRect.bottom-40),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			}
			
			// "Flottenzusammenstellung" in der Mitte zeichnen
			CFont font;
			pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
			pDC->SelectObject(&font);
			pDC->DrawText(CResourceManager::GetString("FLEET_MENUE"),CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60),DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
	// Wenn wir nur ein anderes Schiff markiert haben, dann brauchen wir nur noch die Markierung neu zeichnen
	else
	{
		USHORT row = 0;
		USHORT column = 1;
		if (m_iOldShipInFleet > 0)
		{
			// Spalte und Zeile aus der Position der alten Markierung berechnen
			row = m_iOldShipInFleet%9;
			// Nur löschen wenn das alte Schiff auch auf der jetzigen Seite befand
			if (m_iOldShipInFleet < m_iFleetPage*18 && m_iOldShipInFleet >= (m_iFleetPage-1)*18)
			{
				// Unterschied zwischen erster und weiteren Seiten, weil auf der ersten Seite das Schiff
				// mit dabei ist, welches die Flotte besitzt (wird seperat behandelt) und auf den folgenden Seiten
				// dieses Schiff nicht dabei ist
				if (m_iFleetPage == 1)
					column = (USHORT)(m_iOldShipInFleet / 9) + 1;
				else
				{
					//column = (USHORT)(m_iOldShipInFleet / (9*(m_iFleetPage-1))) - 1;
					if (m_iOldShipInFleet < (18 * (m_iFleetPage-1) + 9))
						column = 1;
					else
						column = 2;
				}
				// Die alte Markierung wieder aufheben
				CBrush nb(RGB(0,0,0));
				pDC->SelectObject(nb);
				pDC->Rectangle(250*column+120,row*65+90,250*column+230,row*65+135);
				if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(m_iOldShipInFleet-1).GetCloak())
					pDC->SetTextColor(RGB(80,80,80));
				else
					pDC->SetTextColor(RGB(170,170,170));
				pDC->TextOut(250*column+120,row*65+90,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(m_iOldShipInFleet-1).GetShipName());
				pDC->TextOut(250*column+120,row*65+110,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(m_iOldShipInFleet-1).GetShipClass()+"-"+CResourceManager::GetString("CLASS"));
			}
		}
		// Das Schiff welches die Flotte besitzt ist nicht in der Flotte selbst, darum manuell holen
		else if (m_iOldShipInFleet == 0)
		{
			CBrush nb(RGB(0,0,0));
			pDC->SelectObject(nb);
			pDC->Rectangle(250*column+120,row*65+90,250*column+230,row*65+135);
			if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetCloak())
				pDC->SetTextColor(RGB(80,80,80));
			else
				pDC->SetTextColor(RGB(170,170,170));
			pDC->TextOut(250*column+120,row*65+90,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipName());
			pDC->TextOut(250*column+120,row*65+110,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipClass()+"-"+CResourceManager::GetString("CLASS"));
		}
		// Spalte und Zeile aus der Position der neuen Markierung berechnen
		if (pDoc->GetNumberOfTheShipInFleet() > 0)
		{
			row = pDoc->GetNumberOfTheShipInFleet()%9;
			// Unterschied zwischen erster und weiteren Seiten, weil auf der ersten Seite das Schiff
			// mit dabei ist, welches die Flotte besitzt (wird seperat behandelt) und auf den folgenden Seiten
			// dieses Schiff nicht dabei ist
			if (m_iFleetPage == 1)
				column = (USHORT)(pDoc->GetNumberOfTheShipInFleet() / 9) + 1;
			else
			{
				if (pDoc->GetNumberOfTheShipInFleet() < (18 * (m_iFleetPage-1) + 9))
					column = 1;
				else
					column = 2;
			}
			// Die neue Markierung zeichnen
			if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(pDoc->GetNumberOfTheShipInFleet()-1).GetCloak())
				pDC->SetTextColor(RGB(150,0,0));
			else
				pDC->SetTextColor(RGB(255,0,0));
			pDC->TextOut(250*column+120,row*65+90,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(pDoc->GetNumberOfTheShipInFleet()-1).GetShipName());
			pDC->TextOut(250*column+120,row*65+110,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(pDoc->GetNumberOfTheShipInFleet()-1).GetShipClass()+"-"+CResourceManager::GetString("CLASS"));
		}
		// Das Schiff welches die Flotte besitzt ist nicht in der Flotte selbst, darum manuell holen
		else if (pDoc->GetNumberOfTheShipInFleet() == 0)
		{
			row = 0;
			column = 1;
			if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetCloak())
				pDC->SetTextColor(RGB(150,0,0));
			else
				pDC->SetTextColor(RGB(255,0,0));
			pDC->TextOut(250*column+120,row*65+90,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipName());
			pDC->TextOut(250*column+120,row*65+110,pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipClass()+"-"+CResourceManager::GetString("CLASS"));
		}
		m_bDrawFullFleet = TRUE;
	}
}
void CFleetMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Wenn wir in der Flottenansicht sind
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	
	USHORT whichRect = 0;	// Auf welches Rechteck haben wir geklickt (gleiche Klasse oder gleichen Typ oder alle hinzufügen?)

	// Überprüfen ob wir auf den next-Button geklickt haben
	if (m_bShowNextButton == TRUE && CRect(540,r.bottom-70,660,r.bottom-40).PtInRect(point))
	{
		m_iFleetPage++;
		m_bDrawFullFleet = TRUE;
		Invalidate();
		return;
	}
	// Überprüfen ob wir auf den back-Button geklickt haben
	else if (m_bShowBackButton == TRUE && CRect(290,r.bottom-70,410,r.bottom-40).PtInRect(point))
	{
		m_iFleetPage--;
		m_bDrawFullFleet = TRUE;
		Invalidate();
		return;
	}

	// Fremde Flotten können nicht bearbeitet werden
	if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetOwnerOfShip() != pDoc->GetPlayersRace())
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
		CPoint ko = pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetKO();
		for (int i = 0; i < pDoc->m_ShipArray.GetSize(); i++)
			if (pDoc->m_ShipArray.GetAt(i).GetOwnerOfShip() == pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetOwnerOfShip())
				if (pDoc->m_ShipArray.GetAt(i).GetKO() == ko)
					if (pDoc->m_ShipArray.GetAt(i).GetShipType() != OUTPOST && pDoc->m_ShipArray.GetAt(i).GetShipType() != STARBASE)
						if ((whichRect == 1 && pDoc->m_ShipArray.GetAt(i).GetShipClass() == pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipClass())
							|| (whichRect == 2 && pDoc->m_ShipArray.GetAt(i).GetShipType() == pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipType())
							|| whichRect == 3)
							{
							// Wenn das Schiff noch keine Flotte hat, dann müssen wir erstmal eine Flotte bilden
							if (pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet() == 0 && pDoc->GetNumberOfFleetShip() != i)
									pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].CreateFleet();
							// Jetzt fügen wir der Flotte das angeklickte Schiff hinzu, wenn es nicht das Schiff selbst ist,
							// welches die Flotte anführt und wenn es selbst keine Flotte besitzt
							if (pDoc->GetNumberOfFleetShip() != i && pDoc->m_ShipArray.GetAt(i).GetFleet() == 0)
							{
								pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->AddShipToFleet(pDoc->m_ShipArray[i]);
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
									pDoc->SetNumberOfTheShipInArray(i-1);
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
		if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet() != 0)
			for (USHORT i = 0; i < pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetFleetSize(); i++)
				if ((whichRect == 4 && pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipClass() != pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetShipClass())
					|| (whichRect == 5 && pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetShipType() != pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetShipFromFleet(i).GetShipType())
					|| whichRect == 6)
				{
					// Das Schiff welches wir aus der Flotte nehmen stecken wir wieder in das normale Schiffsarray
					pDoc->m_ShipArray.Add(pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->RemoveShipFromFleet(i));
					// Wenn wir das letzte Schiff in der Flotte entfernt haben, dann müssen wir
					// das markierte Schiff dekrementieren, da sonst ein ungültiger Feldaufruf kommt
					if (i >= (short)pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetFleetSize())
						pDoc->SetNumberOfTheShipInFleet(i-1);
					// Wenn wir alle Schiffe aus der Flotte entfernt haben
					if (pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->GetFleetSize() == 0)
					{
						pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->DeleteFleet();
						pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].DeleteFleet();
						pDoc->SetNumberOfTheShipInFleet(-1);
						break;
					}
					i--;
					m_iFleetPage = 1;
					pDoc->SetNumberOfTheShipInFleet(0);
				}
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipBottomView));
				//pDoc->SetShowShipInfoInCSmallInfoView(TRUE);
				//pDoc->DrawShipInfoInCSmallInfoView();
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
	if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet() != 0 && i <= pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetFleetSize())
	{
		// Wenn es nicht das Schiff ist welches die Flotte besitzt
		if ((i != 0 && m_iFleetPage == 1) || m_iFleetPage > 1)
		{
			// Das Schiff welches wir aus der Flotte nehmen stecken wir wieder in das normale Schiffsarray
			pDoc->m_ShipArray.Add(pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->RemoveShipFromFleet(i-1));
			// Wenn wir das letzte Schiff in der Flotte entfernt haben, dann müssen wir
			// das markierte Schiff dekrementieren, da sonst ein ungültiger Feldaufruf kommt
			if (i > (short)pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetFleetSize())
				pDoc->SetNumberOfTheShipInFleet(i-1);
			// Wenn wir gerade das erste Schiff auf der nächsten Seite entfernt haben, dann eine Seite zurück
			if (m_iFleetPage > 1 && i == (m_iFleetPage-1)*18 && pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetFleetSize() == i-1)
				m_iFleetPage--;	
			// Wenn wir alle Schiffe aus der Flotte entfernt haben
			if (pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->GetFleetSize() == 0)
			{
				pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->DeleteFleet();
				pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].DeleteFleet();
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
			CFleet f = *(pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet());
			// Flotte löschen
			pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->DeleteFleet();
			pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].DeleteFleet();
			// Nun die Flotte auf das nächste Schiff übergeben, dafür das erste Schiff in der Flotte rausnehmen
			pDoc->m_ShipArray.Add(f.RemoveShipFromFleet(0));
			// für dieses eine Flotte erstellen
			pDoc->m_ShipArray[pDoc->m_ShipArray.GetUpperBound()].CreateFleet();
			for (USHORT i = 0; i < f.GetFleetSize(); i++)
				pDoc->m_ShipArray[pDoc->m_ShipArray.GetUpperBound()].GetFleet()->AddShipToFleet(f.GetShipFromFleet(i));
			pDoc->m_ShipArray[pDoc->m_ShipArray.GetUpperBound()].CheckFleet();
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
	CalcLogicalPoint(point);

	// Wenn wir in der Flottenansicht sind und mit der Maus über ein Schiff in der Flotte gehen
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
	USHORT i = (int)column * 9 + (int)row + (m_iFleetPage-1)*18;
	if ((i != pDoc->GetNumberOfTheShipInFleet() && pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet() != 0
		&& i <= pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetFleet()->GetFleetSize()))
	{
		m_iOldShipInFleet = pDoc->GetNumberOfTheShipInFleet();
		pDoc->SetNumberOfTheShipInFleet(i);
		m_bDrawFullFleet = FALSE;
		pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
		Invalidate(FALSE);		
	}
	
	CMainBaseView::OnMouseMove(nFlags, point);
}

void CFleetMenuView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	// Das hier alles nur machen, wenn wir in der Flotten-Ansicht sind	
	CGalaxyMenuView::SetMoveShip(FALSE);
	pDoc->SetNumberOfTheShipInArray(pDoc->GetNumberOfFleetShip());
	pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pDoc->GetPlayersRace());
	CSmallInfoView::SetShipInfo(true);
	pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));

	CMainBaseView::OnRButtonDown(nFlags, point);
}


void CFleetMenuView::CreateButtons()
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