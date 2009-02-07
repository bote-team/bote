// ShipDesignMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "ShipDesignMenuView.h"
#include "ShipDesignBottomView.h"

// CShipDesignMenuView

IMPLEMENT_DYNCREATE(CShipDesignMenuView, CMainBaseView)

CShipDesignMenuView::CShipDesignMenuView()
{

}

CShipDesignMenuView::~CShipDesignMenuView()
{
}

BEGIN_MESSAGE_MAP(CShipDesignMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

void CShipDesignMenuView::OnNewRound()
{
	m_iClickedOnShip = -1;
	m_iOldClickedOnShip = -1;
	m_iBeamWeaponNumber = 0;
	m_iTorpedoWeaponNumber = 0;
	m_bFoundBetterBeam = FALSE;
	m_bFoundWorseBeam = FALSE;
}
// CShipDesignMenuView drawing

void CShipDesignMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here
	CMemDC pDC(dc);
	pDC->SetBkMode(TRANSPARENT);
	if (pDoc->m_pIniLoader->GetValue("SMOOTHSCALING"))
		pDC->SetStretchBltMode(HALFTONE);
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	LoadRaceFont(pDC);
	// ***************************** DIE SCHIFFSDESIGN ANSICHT ZEICHNEN **********************************
	DrawShipDesignMenue(pDC, r);	
}


// CShipDesignMenuView diagnostics

#ifdef _DEBUG
void CShipDesignMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CShipDesignMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CShipDesignMenuView message handlers

void CShipDesignMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	CreateButtons();

	bg_designmenu.DeleteObject();
	
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
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"designmenu.jpg");
	bg_designmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Destroy();	
	
	// Schiffsdesignansicht
	m_iClickedOnShip = -1;
	m_iOldClickedOnShip = -1;
	m_iBeamWeaponNumber = 0;
	m_iTorpedoWeaponNumber = 0;
	m_bFoundBetterBeam = FALSE;
	m_bFoundWorseBeam = FALSE;
}

void CShipDesignMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CShipDesignMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Schiffsdesignmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CShipDesignMenuView::DrawShipDesignMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect r = theClientRect;

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_designmenu);
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
	pDC->MoveTo(theClientRect.left+200,theClientRect.top+70);
	pDC->LineTo(theClientRect.left+200,theClientRect.bottom-50);
	pDC->MoveTo(theClientRect.right-300,theClientRect.top+70);
	pDC->LineTo(theClientRect.right-300,theClientRect.bottom-50);
*/
	// Links im Bild die veränderbaren Schiffklassen zeichnen (bis jetzt darf man keine Stationen verändern,
	// weil deren Baukosten allein von den Industriekosten berechnet werden. Diese aber nicht steigen wenn
	// man die Hülle oder Schilde verbessert. Somit könnte man bessere Stationen für den gleichen Preis bauen.
	pDC->SetTextColor(RGB(0,0,0));
	if (pDoc->GetPlayersRace() == HUMAN)
		pDC->DrawText(CResourceManager::GetString("CLASS"),CRect(20,80,200,105),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	else
		pDC->DrawText(CResourceManager::GetString("CLASS"),CRect(20,80,200,105),DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	// Schiffsinfoarray durchgehen und nach zum Imperium gehörende baubare Schiffe suchen
	short j = 0;
	short counter = m_iClickedOnShip - 23 + m_iOldClickedOnShip;
	short oldClickedShip = m_iClickedOnShip;
	
	short ShipNumber = -1;
	m_nSizeOfShipDesignList = 0;
	// Es gehen nur 21 Einträge auf die Seite, deshalb muss abgebrochen werden
	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
		if (pDoc->m_ShipInfoArray.GetAt(i).GetRace() == pDoc->GetPlayersRace())
			if (pDoc->m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()))
				if (pDoc->m_ShipInfoArray.GetAt(i).GetShipType() != OUTPOST && pDoc->m_ShipInfoArray.GetAt(i).GetShipType() != STARBASE)
				{
					
					// wurde dieses Schiff durch kein anderes jetzt baubares Schiff schon obsolet?
					BOOLEAN foundObsolet = FALSE;
					for (int m = 0; m < pDoc->m_ShipInfoArray.GetSize(); m++)
						if (pDoc->m_ShipInfoArray.GetAt(m).GetRace() == pDoc->GetPlayersRace())
							if (pDoc->m_ShipInfoArray.GetAt(m).IsThisShipBuildableNow(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()))
								if (pDoc->m_ShipInfoArray.GetAt(m).GetObsoleteShipClass() == pDoc->m_ShipInfoArray.GetAt(i).GetShipClass())
								{
									foundObsolet = TRUE;
									break;
								}
					
					if (foundObsolet)
						continue;

					m_nSizeOfShipDesignList++;
					if (counter > 0)
					{
						m_iClickedOnShip--;
						counter--;
						continue;
					}
					
					if (j < 24)
					{
						pDC->SetTextColor(oldColor);
						// Wenn wir ein Schiff gefunden haben, dann zeichnen wir dieses in die Liste (max. 21)
						// Wenn wir das Schiff markiert haben, dann die Markierung zeichnen, haben wir kein spezielles Schiff
						// angeklickt, so wird das 1. Schiff in der Liste markiert
						if (j == m_iClickedOnShip || m_iClickedOnShip == -1)
						{
							pDC->SetTextColor(RGB(255,255,255));
							// Wenn wir nix angeklickt haben und nur das erste Schiff markiert war, dann automatisch
							m_iClickedOnShip = j;
							if (oldClickedShip == -1)
								oldClickedShip = j;
							ShipNumber = i;
							// Infos in View 3 aktualisieren							
							if (pDoc->m_iShowWhichShipInfoInView3 != i)
							{
								pDoc->m_iShowWhichShipInfoInView3 = i;
								pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CShipDesignBottomView));
							}		
						}						
						pDC->DrawText(CString(pDoc->m_ShipInfoArray.GetAt(i).GetShipClass()),CRect(25,120+j*25,200,145+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
						j++;
					}
				}
	m_iClickedOnShip = oldClickedShip;

	// Hier jetzt Informationen zum angeklickten Schiff anzeigen
	if (ShipNumber != -1)
	{
		// Bild des Schiffes zeichnen
		CString s;
		s.Format("Ships\\%s.jpg",pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetShipClass());
		graphic = pDoc->GetGraphicPool()->GetGraphic(s);
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGraphic("Ships\\ImageMissing.jpg");
		if (graphic)
		{
			oldGraphic = mdc.SelectObject(*graphic);
			pDC->BitBlt(388,90,200,150,&mdc,0,0,SRCCOPY);
			mdc.SelectObject(oldGraphic);
			graphic = NULL;
		}
		// allgemeine Schiffsinformationen anzeigen
		pDC->SetTextColor(oldColor);
		pDoc->m_ShipInfoArray.GetAt(ShipNumber).DrawShipInformation(pDC,CRect(235,250,r.right-335,300),pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch());
		// Baukosten des Schiffes anzeigen
		pDC->SetTextColor(RGB(200,200,200));
		pDC->DrawText(CResourceManager::GetString("BUILDCOSTS"),CRect(200,440,r.right-300,465), DT_CENTER | DT_TOP);
		pDC->SetTextColor(oldColor);
		s.Format("%s: %d  %s: %d  %s: %d",CResourceManager::GetString("INDUSTRY"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededIndustry(),
			CResourceManager::GetString("TITAN"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededTitan(),
			CResourceManager::GetString("DEUTERIUM"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededDeuterium());
		pDC->DrawText(s,CRect(200,465,r.right-300,490), DT_CENTER | DT_TOP | DT_SINGLELINE);
		s.Format("%s: %d  %s: %d  %s: %d  %s: %d",CResourceManager::GetString("DURANIUM"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededDuranium(),
			CResourceManager::GetString("CRYSTAL"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededCrystal(),
			CResourceManager::GetString("IRIDIUM"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededIridium(),
			CResourceManager::GetString("DILITHIUM"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededDilithium());
		pDC->DrawText(s,CRect(200,490,r.right-300,515), DT_CENTER | DT_TOP | DT_SINGLELINE);

		// Die Buttons zur Eigenschaftsänderung in der Rechten Seite der Ansicht anzeigen
		// zuerst überprüfen wir die Beamwaffen, wir können den Typ der Beamwaffe verändern, wenn wir mindst. ein anderes
		// Schiff des Imperiums finden, welches DIESE Beamwaffe mit einem höheren Typ oder einem niedrigeren Typ besitzt
		
		// Nach Beamwaffen suchen
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetSize() > m_iBeamWeaponNumber)
		{	
			// gibt es schon von dieser Beamwaffe hier auf dem Schiff einen höheren Typ?
			USHORT maxTyp =	pDoc->m_WeaponObserver[pDoc->GetPlayersRace()].GetMaxBeamType(pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamName());
			if (maxTyp > pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType())
			{
				// Dann können wir den Typ unserer Beamwaffe(n) verbessern
				LoadFontForLittleButton(pDC);
				mdc.SelectObject(bm2);
				pDC->BitBlt(r.right-145,120,120,30,&mdc,0,0,SRCCOPY);
				pDC->DrawText(CResourceManager::GetString("BTN_STRONGER"),CRect(r.right-145,120,r.right-25,150),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				m_bFoundBetterBeam = TRUE;
			}
			// Wenn wir einen größeren Typ als Typ 1 haben, dann können wir diesen verringern
			if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType() > 1)
			{
				// Dann können wir den Typ unserer Beamwaffe(n) verkleinern
				LoadFontForLittleButton(pDC);
				mdc.SelectObject(bm2);
				pDC->BitBlt(r.right-275,120,120,30,&mdc,0,0,SRCCOPY);
				pDC->DrawText(CResourceManager::GetString("BTN_WEAKER"),CRect(r.right-275,120,r.right-155,150),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				m_bFoundWorseBeam = TRUE;
			}
			// Haben also eine Verbesserung oder Verschlechterung gefunden
		//	if (m_bFoundBetterBeam == TRUE || m_bFoundWorseBeam == TRUE)
			{
				// Typ und Name der Beamwaffe zeichnen
				if (pDoc->GetPlayersRace() == HUMAN || pDoc->GetPlayersRace() == ROMULAN)
					pDC->SetTextColor(RGB(0,0,0));
				else
					pDC->SetTextColor(CFontLoader::GetFontColor(pDoc->GetPlayersRace(), 3));
				s.Format("%s %d %s",CResourceManager::GetString("TYPE"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType(),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamName());
				pDC->DrawText(s,CRect(r.right-230,80,r.right-70,105),DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
			}
		}
		LoadRaceFont(pDC);
		// Nach anderer Torpedowaffe suchen
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetTorpedoWeapons()->GetSize() > m_iTorpedoWeaponNumber)
		{
			// den aktuellen Torpedotyp holen
			USHORT currentTorpType = pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTorpedoType();
			// Torpedoname zeichnen
			s.Format("%s",pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeName());
			pDC->DrawText(s,CRect(r.right-300,170,r.right,195),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			s.Format("%s",pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTorpedoName());
			pDC->DrawText(s,CRect(r.right-300,195,r.right,220),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			LoadFontForLittleButton(pDC);
			pDC->BitBlt(r.right-275,230,120,30,&mdc,0,0,SRCCOPY);
			pDC->DrawText(CResourceManager::GetString("BTN_LAUNCHER"),CRect(r.right-275,230,r.right-155,260),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			pDC->BitBlt(r.right-145,230,120,30,&mdc,0,0,SRCCOPY);
			pDC->DrawText(CResourceManager::GetString("BTN_TORPEDO"),CRect(r.right-145,230,r.right-25,260),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			LoadRaceFont(pDC);
		}
		// hier Möglichkeit anderes Hüllenmaterial anzubringen eingebaut
		CString material;
		switch (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetHull()->GetHullMaterial())
		{
			case TITAN:		material = CResourceManager::GetString("TITAN");; break;
			case DURANIUM:	material = CResourceManager::GetString("DURANIUM");; break;
			case IRIDIUM:	material = CResourceManager::GetString("IRIDIUM");; break;
			default: material = "";
		}	
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetHull()->GetDoubleHull() == TRUE)
			s.Format("%s%s",material, CResourceManager::GetString("DOUBLE_HULL_ARMOUR"));
		else
			s.Format("%s%s",material, CResourceManager::GetString("HULL_ARMOR"));
		pDC->DrawText(s,CRect(r.right-300,380,r.right,410),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
		// Hier kann man den Schildtyp ändern
		// zuerst Anzeige der jetzt aktuellen Schilde. Beim Romulaner eine schwarze Schriftart wählen. Wenn dies
		// später auch bei der Föd heller unterlegt ist kann auch dort eine schwarze Schriftfarbe gewählt werden.
		if (pDoc->GetPlayersRace() == ROMULAN)
			pDC->SetTextColor(RGB(0,0,0));
		s.Format("%s %d %s",CResourceManager::GetString("TYPE"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetShield()->GetShieldType(),CResourceManager::GetString("SHIELDS"));
		pDC->DrawText(s,CRect(r.right-300,490,r.right,520),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
		// Ab jetzt die Buttons zum Ändern der jeweiligen Komponenten
		LoadFontForLittleButton(pDC);
		mdc.SelectObject(bm2);
		pDC->BitBlt(r.right-275,420,120,30,&mdc,0,0,SRCPAINT);
		pDC->DrawText(CResourceManager::GetString("BTN_MATERIAL"),CRect(r.right-275,420,r.right-155,450),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->BitBlt(r.right-145,420,120,30,&mdc,0,0,SRCPAINT);
		pDC->DrawText(CResourceManager::GetString("BTN_HULLTYPE"),CRect(r.right-145,420,r.right-25,450),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
		// Schildtyp schwächer Button einblenden
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetShield()->GetShieldType() > 0)
		{
			pDC->BitBlt(r.right-275,540,120,30,&mdc,0,0,SRCCOPY);
			pDC->DrawText(CResourceManager::GetString("BTN_WEAKER"),CRect(r.right-275,540,r.right-155,565),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		// Schildtyp stärker Button einblenden
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetShield()->GetShieldType() < pDoc->m_WeaponObserver[pDoc->GetPlayersRace()].GetMaxShieldType())
		{
			pDC->BitBlt(r.right-145,540,120,30,&mdc,0,0,SRCCOPY);
			pDC->DrawText(CResourceManager::GetString("BTN_STRONGER"),CRect(r.right-145,540,r.right-25,565),DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	}
	// Wenn das Schiff in irgendeinem unserer Systeme gebaut wird, dann großen Text ausgeben, in welchem System das Schiff
	// gerade gebaut wird
	CString systemName = CheckIfShipIsBuilding(ShipNumber);
	if (!systemName.IsEmpty())
	{
		COverlayBanner *banner = new COverlayBanner(CPoint(theClientRect.left+200,theClientRect.top+300), CSize(580, 200),
			CResourceManager::GetString("NO_CHANGE_POSSIBLE", FALSE, systemName), RGB(220,0,0));
		banner->Draw(pDC);
		delete banner;
	}

	// "Schiffsdesign" in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	if (pDoc->GetPlayersRace() == FERENGI)
		r.SetRect(188,10,830,40);
	else
		r.SetRect(188,10,788,60);
	pDC->DrawText(CResourceManager::GetString("SHIPDESIGN"), r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CShipDesignMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Wenn wir in der Schiffsdesignansicht sind
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	// Schiffsinfoarray durchgehen und nach zum Imperium gehörende baubare Schiffe suchen
	
	short j = 0;
	short counter = m_iClickedOnShip - 23 + m_iOldClickedOnShip;
	short add = 0;
	short n = -1;
	
	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
		if (pDoc->m_ShipInfoArray.GetAt(i).GetRace() == pDoc->GetPlayersRace())
			if (pDoc->m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()))
				if (pDoc->m_ShipInfoArray.GetAt(i).GetShipType() != OUTPOST && pDoc->m_ShipInfoArray.GetAt(i).GetShipType() != STARBASE)
				{
					// wurde dieses Schiff durch kein anderes jetzt baubares Schiff schon obsolet?
					BOOLEAN foundObsolet = FALSE;
					for (int m = 0; m < pDoc->m_ShipInfoArray.GetSize(); m++)
						if (pDoc->m_ShipInfoArray.GetAt(m).GetRace() == pDoc->GetPlayersRace())
							if (pDoc->m_ShipInfoArray.GetAt(m).IsThisShipBuildableNow(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()))
								if (pDoc->m_ShipInfoArray.GetAt(m).GetObsoleteShipClass() == pDoc->m_ShipInfoArray.GetAt(i).GetShipClass())
								{
									foundObsolet = TRUE;
									break;
								}
					if (foundObsolet)
						continue;
		
					if (counter > 0)
					{
						add++;
						counter--;
						continue;
					}
					if (j < 24)
					{
						// Müssen später noch die Seitenzahl beachten, bis jetzt aber wie in der Diplomatieansicht
						// geht die Liste nur auf eine Seite
						if (CRect(20,120+j*25,200,145+j*25).PtInRect(point))
						{
							m_iClickedOnShip = j + add;
							m_iOldClickedOnShip = 23-(j)%24;
							m_iBeamWeaponNumber = 0;
							m_iTorpedoWeaponNumber = 0;
							m_bFoundBetterBeam = FALSE;
							m_bFoundWorseBeam  = FALSE;
							Invalidate();
							return;
						}
						if (j + add == m_iClickedOnShip)
							n = i;
						j++;
					}
				}
	// Bevor wir irgendetwas ändern können müssen wir überprüfen, dass das gerade angeklickte Schiff nicht gerade
	// gebaut wird. Wenn das der Fall sein sollte können wir nix ändern. Es kommt dann eine Meldung in welchem
	// System das Schiff gerade gebaut wird
	if (CRect(r.right-300,80,r.right,r.bottom-80).PtInRect(point))
		if (!CheckIfShipIsBuilding(n).IsEmpty())
		{
			Invalidate(FALSE);
			return;
		}		
	counter = 0;
	// Überprüfen ob irgendetwas an den Beamwaffen ändern möchte
	if (n != -1 && pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetSize() > 0)
	{
		// Hat das Schiff mehr als eine Beamwaffe können wir auf die nächste zugreifen indem wir hier klicken
		if (CRect(r.right-300,80,r.right,105).PtInRect(point))
		{
			if (pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetUpperBound() > m_iBeamWeaponNumber)
			{
				m_iBeamWeaponNumber++;
				m_bFoundBetterBeam = FALSE;
				m_bFoundWorseBeam  = FALSE;
				Invalidate();
			}
			else if (pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetUpperBound() == m_iBeamWeaponNumber)
			{
				m_iBeamWeaponNumber = 0;
				m_bFoundBetterBeam = FALSE;
				m_bFoundWorseBeam  = FALSE;
				Invalidate();
			}
		}
		// Haben wir auf den Button geklickt um den Typ der Beamwaffe zu verbessern
		else if (m_bFoundBetterBeam == TRUE && CRect(r.right-145,120,r.right-25,150).PtInRect(point))
		{
			// Dann wird der Typ bei der aktuellen Beamwaffe um eins erhöht
			BYTE oldType = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType();
			USHORT oldPower = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamPower();
			BYTE oldNumber = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamNumber();
			BYTE oldShootNumber = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetShootNumber();
			CString oldName = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamName();
			BYTE oldBonus = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBonus();
			BYTE oldLenght = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamLenght();
			BYTE oldRechargeTime = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetRechargeTime();
			BOOLEAN piercing = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetPiercing();
			BOOLEAN modulating = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetModulating();
			// hier aktualisieren
			pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).ModifyBeamWeapon(
				(oldType+1),oldPower,oldNumber,oldName,modulating,piercing,oldBonus,oldLenght,oldRechargeTime,oldShootNumber);
			pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
			m_bFoundBetterBeam = FALSE;
			m_bFoundWorseBeam  = FALSE;
			Invalidate();
		}
		// Haben wir auf den Button geklickt um den Typ der Beamwaffe zu verringern
		else if (m_bFoundWorseBeam == TRUE && CRect(r.right-275,120,r.right-155,150).PtInRect(point))
		{
			// Dann wird der Typ bei der aktuellen Beamwaffe um eins erhöht
			BYTE oldType = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType();
			USHORT oldPower = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamPower();
			BYTE oldNumber = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamNumber();
			BYTE oldShootNumber = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetShootNumber();
			CString oldName = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamName();
			BYTE oldBonus = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBonus();
			BYTE oldLenght = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamLenght();
			BYTE oldRechargeTime = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetRechargeTime();
			BOOLEAN piercing = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetPiercing();
			BOOLEAN modulating = pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetModulating();
			// hier aktualisieren
			pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).ModifyBeamWeapon(
				(oldType-1),oldPower,oldNumber,oldName,modulating,piercing,oldBonus,oldLenght,oldRechargeTime,oldShootNumber);
			pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
			m_bFoundBetterBeam = FALSE;
			m_bFoundWorseBeam  = FALSE;
			Invalidate();
		}
	}
	counter++;

	// Überprüfen ob wir irgendetwas an einer Torpedowaffe ändern möchten
	if (n != -1 && pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetSize() > 0)
	{
		// Hat das Schiff mehr als eine Torpedowaffe können wir auf die nächste zugreifen indem wir hier klicken
		if (CRect(r.right-300,80+counter*90,r.right,125+counter*90).PtInRect(point))
		{
			if (pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetUpperBound() > m_iTorpedoWeaponNumber)
			{
				m_iTorpedoWeaponNumber++;
				Invalidate();
			}
			else if (pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetUpperBound() == m_iTorpedoWeaponNumber)
			{
				m_iTorpedoWeaponNumber = 0;
				Invalidate();
			}
		}
		// Haben wir auf den Button geklickt um den Torpedowerfer zu ändern
		else if (CRect(r.right-275,140+counter*90,r.right-145,170+counter*90).PtInRect(point))
		{
			BYTE oldTorpType = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTorpedoType();
			BYTE oldNumber   = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetNumber();
			BYTE oldFirerate = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeFirerate();
			BYTE oldTupeNumber=pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetNumberOfTupes();
			CString oldTupeName= pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeName();
			BOOLEAN oldOnlyMicro  = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetOnlyMicroPhoton();
			BYTE oldAcc		= pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetAccuracy();
			//USHORT newTorpType = pDoc->m_WeaponObserver[pDoc->GetPlayersRace()].GetNextTorpedo(oldTorpType, oldOnlyMicro);
			TupeWeaponsObserverStruct twos = pDoc->m_WeaponObserver[pDoc->GetPlayersRace()].GetNextTupe(oldTupeName,oldTorpType);
			// hier aktualisieren
			pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).ModifyTorpedoWeapon(oldTorpType,twos.number,twos.fireRate,oldTupeNumber,twos.TupeName,twos.onlyMicro,oldAcc);
			pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
			Invalidate();
		}
		// Haben wir auf den Button geklicht um den Torpedotyp zu ändern
		else if (CRect(r.right-145,140+counter*90,r.right-25,170+counter*90).PtInRect(point))
		{
			BYTE oldTorpType = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTorpedoType();
			BYTE oldNumber   = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetNumber();
			BYTE oldFirerate = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeFirerate();
			BYTE oldTupeNumber=pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetNumberOfTupes();
			CString oldTupeName= pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeName();
			BOOLEAN oldOnlyMicro  = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetOnlyMicroPhoton();
			BYTE oldAcc		= pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetAccuracy();
			BYTE newTorpType = pDoc->m_WeaponObserver[pDoc->GetPlayersRace()].GetNextTorpedo(oldTorpType, oldOnlyMicro);
			// hier aktualisieren
			pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).ModifyTorpedoWeapon(newTorpType,oldNumber,oldFirerate,oldTupeNumber,oldTupeName,oldOnlyMicro,oldAcc);
			pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
			Invalidate();
		}
	}
	counter++;
	// Überprüfen ob wir das Hüllenmaterial ändern möchten, also ob wir auf den Button "Hüllenmaterial ändern" geklickt haben
	if (n != -1 && CRect(r.right-275,180+counter*120,r.right-155,210+counter*120).PtInRect(point))
	{
		BOOLEAN oldDoubleHull = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetDoubleHull();
		ULONG oldBaseHull = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetBaseHull();
		BOOLEAN ablative = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetAblative();
		BOOLEAN polarisation = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetPolarisation();
		// Dann bekommt das nächste Schiff ein neues Hüllenmaterial
		switch (pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetHullMaterial())
		{
		case TITAN: pDoc->m_ShipInfoArray.GetAt(n).GetHull()->ModifyHull(oldDoubleHull,oldBaseHull,DURANIUM,ablative,polarisation);
			break;
		case DURANIUM: pDoc->m_ShipInfoArray.GetAt(n).GetHull()->ModifyHull(oldDoubleHull,oldBaseHull,IRIDIUM,ablative,polarisation);
			break;
		case IRIDIUM: pDoc->m_ShipInfoArray.GetAt(n).GetHull()->ModifyHull(oldDoubleHull,oldBaseHull,TITAN,ablative,polarisation);
			break;
		}
		pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
		Invalidate();
	}
	// Überprüfen ob wir geklickt haben um die Hüllenart zu wechseln (also Einzel- oder Doppelhülle)
	else if (n != -1 && CRect(r.right-145,180+counter*120,r.right-25,210+counter*120).PtInRect(point))
	{
		BOOLEAN oldDoubleHull = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetDoubleHull();
		// Wenn die alte Hülle eine Einzelhülle war und man eine Doppelhülle anbaut, dann verringert sich die
		// Manövriebarkeit um -1. Wenn man eine Einzelhülle anbaut, dann kommt zur Manö +1 dazu. Schiffe mit
		// Manö 1 oder Manö 8 sind von dieser Reglung ausgeschlossen.
		if (pDoc->m_ShipInfoArray.GetAt(n).GetManeuverability() > 1 && pDoc->m_ShipInfoArray.GetAt(n).GetManeuverability() < 8)
		{
			// wollen Doppelhülle draus machen
			if (oldDoubleHull == FALSE)
				pDoc->m_ShipInfoArray.GetAt(n).SetManeuverability(pDoc->m_ShipInfoArray.GetAt(n).GetManeuverability()-1);
			// wollen eine Einzelhülle draus machen
			else
				pDoc->m_ShipInfoArray.GetAt(n).SetManeuverability(pDoc->m_ShipInfoArray.GetAt(n).GetManeuverability()+1);
		}
		BOOLEAN ablative = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetAblative();
		BOOLEAN polarisation = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetPolarisation();
		ULONG oldBaseHull = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetBaseHull();
		BYTE oldHullMaterial = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetHullMaterial();
		pDoc->m_ShipInfoArray.GetAt(n).GetHull()->ModifyHull(!oldDoubleHull,oldBaseHull,oldHullMaterial,ablative,polarisation);
		pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
		Invalidate();
	}
	// Überprüfen ob ich geklickt habe um den Schildtyp zu verringern
	else if (n != -1 && CRect(r.right-275,300+counter*120,r.right-155,325+counter*120).PtInRect(point))
	{
		if (pDoc->m_ShipInfoArray.GetAt(n).GetShield()->GetShieldType() > 0)
		{
			UINT oldMaxShield = pDoc->m_ShipInfoArray.GetAt(n).GetShield()->GetMaxShield();
			BYTE oldShieldType= pDoc->m_ShipInfoArray.GetAt(n).GetShield()->GetShieldType();
			BOOLEAN regenerative= pDoc->m_ShipInfoArray.GetAt(n).GetShield()->GetRegenerative();
			pDoc->m_ShipInfoArray.GetAt(n).GetShield()->ModifyShield(oldMaxShield, (oldShieldType - 1), regenerative);
			pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
			Invalidate();
		}
	}
	// Überprüfen ob ich geklickt habe um den Schildtyp zu erhöhen
	else if (n != -1 && CRect(r.right-145,300+counter*120,r.right-25,325+counter*120).PtInRect(point))
	{
		USHORT oldShieldType= pDoc->m_ShipInfoArray.GetAt(n).GetShield()->GetShieldType();
		if (pDoc->m_WeaponObserver[pDoc->GetPlayersRace()].GetMaxShieldType() > oldShieldType)
		{
			UINT oldMaxShield = pDoc->m_ShipInfoArray.GetAt(n).GetShield()->GetMaxShield();
			BOOLEAN regenerative =pDoc->m_ShipInfoArray.GetAt(n).GetShield()->GetRegenerative();
			pDoc->m_ShipInfoArray.GetAt(n).GetShield()->ModifyShield(oldMaxShield, (oldShieldType + 1), regenerative);
			pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
			Invalidate();
		}
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

BOOL CShipDesignMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (zDelta < 0)
	{
		if (m_nSizeOfShipDesignList > m_iClickedOnShip+1)
		{
			if (m_iOldClickedOnShip > 0)
				m_iOldClickedOnShip--;
			m_iClickedOnShip++;
			m_iBeamWeaponNumber = 0;
			m_iTorpedoWeaponNumber = 0;
			m_bFoundBetterBeam = FALSE;
			m_bFoundWorseBeam  = FALSE;
			Invalidate();
		}
	}
	else if (zDelta > 0)
	{
		if (m_iClickedOnShip > 0)
		{
			if (m_iClickedOnShip > 23 && m_iOldClickedOnShip < 23)
				m_iOldClickedOnShip++;
			m_iClickedOnShip--;
			m_iBeamWeaponNumber = 0;
			m_iTorpedoWeaponNumber = 0;
			m_bFoundBetterBeam = FALSE;
			m_bFoundWorseBeam  = FALSE;
			Invalidate();
		}
	}

	return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

/// Funktion überprüft ob das in der Designansicht angeklickte Schiff in einem unserer Systeme gerade gebaut wird
/// Man benötigt diesen Check da man keine Schiffe ändern kann, welche gerade gebaut werden.
/// @param n Index des zu prüfenden Schiffes aus der Schiffsliste
/// @return CString mit dem Namen des Systems, wird das Schiff nirgends gebaut ist der String leer
CString CShipDesignMenuView::CheckIfShipIsBuilding(int n) const
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (m_iClickedOnShip != -1 && n < pDoc->m_ShipInfoArray.GetSize() && n >= 0)
	{
		USHORT ID = pDoc->m_ShipInfoArray.GetAt(n).GetID();
		// alle eigenen Systeme durchgehen und schauen, ob an erster Stelle in der Bauliste so ein Schiff steht
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (pDoc->GetSystem(x,y).GetOwnerOfSystem() == pDoc->GetPlayersRace())
					for (int i = 0; i < ALE; i++)
						if (pDoc->GetSystem(x,y).GetAssemblyList()->GetAssemblyListEntry(i) == ID)
							return pDoc->GetSector(x,y).GetName();
	}
	return "";
}

void CShipDesignMenuView::CreateButtons()
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