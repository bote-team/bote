// ShipDesignMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "ShipDesignMenuView.h"
#include "ShipDesignBottomView.h"
#include "IniLoader.h"
#include "RaceController.h"

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
	// Doublebuffering wird initialisiert
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
	
	DrawShipDesignMenue(&g);	
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
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);

	CreateButtons();

	CString sPrefix = pMajor->GetPrefix();
	bg_designmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "designmenu.jpg");
		
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
// Hier die Funktion zum Zeichnen des Schiffsdesignmen�s
/////////////////////////////////////////////////////////////////////////////////////////
void CShipDesignMenuView::DrawShipDesignMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pDoc);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	
	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
		
	if (bg_designmenu)
		g->DrawImage(bg_designmenu, 0, 0, 1075, 750);

	SolidBrush fontBrush(normalColor);

	
	// Links im Bild die ver�nderbaren Schiffklassen zeichnen (bis jetzt darf man keine Stationen ver�ndern,
	// weil deren Baukosten allein von den Industriekosten berechnet werden. Diese aber nicht steigen wenn
	// man die H�lle oder Schilde verbessert. Somit k�nnte man bessere Stationen f�r den gleichen Preis bauen.
		
	// Schiffsinfoarray durchgehen und nach zum Imperium geh�rende baubare Schiffe suchen
	short j = 0;
	short counter = m_iClickedOnShip - 23 + m_iOldClickedOnShip;
	short oldClickedShip = m_iClickedOnShip;
	
	short ShipNumber = -1;
	m_nSizeOfShipDesignList = 0;
	// Es gehen nur 21 Eintr�ge auf die Seite, deshalb muss abgebrochen werden
	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
		if (pDoc->m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			if (pDoc->m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
				if (pDoc->m_ShipInfoArray.GetAt(i).GetShipType() != OUTPOST && pDoc->m_ShipInfoArray.GetAt(i).GetShipType() != STARBASE)
				{
					
					// wurde dieses Schiff durch kein anderes jetzt baubares Schiff schon obsolet?
					BOOLEAN foundObsolet = FALSE;
					for (int m = 0; m < pDoc->m_ShipInfoArray.GetSize(); m++)
						if (pDoc->m_ShipInfoArray.GetAt(m).GetRace() == pMajor->GetRaceShipNumber())
							if (pDoc->m_ShipInfoArray.GetAt(m).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
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
						fontBrush.SetColor(normalColor);
						// Wenn wir ein Schiff gefunden haben, dann zeichnen wir dieses in die Liste (max. 21)
						// Wenn wir das Schiff markiert haben, dann die Markierung zeichnen, haben wir kein spezielles Schiff
						// angeklickt, so wird das 1. Schiff in der Liste markiert
						if (j == m_iClickedOnShip || m_iClickedOnShip == -1)
						{
							fontBrush.SetColor(markColor);
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
							// Markierung worauf wir geklickt haben
							g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(15,120+j*25,185,25));
							g->DrawLine(&Gdiplus::Pen(penColor), 15, 120+j*25, 200, 120+j*25);
							g->DrawLine(&Gdiplus::Pen(penColor), 15, 145+j*25, 200, 145+j*25);
						}
						CString s = pDoc->m_ShipInfoArray.GetAt(i).GetShipClass();
						g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(25, 120 + j * 25, 175, 25), &fontFormat, &fontBrush);
						j++;
					}
				}
	m_iClickedOnShip = oldClickedShip;

	// Hier jetzt Informationen zum angeklickten Schiff anzeigen
	if (ShipNumber != -1)
	{
		// Bild des Schiffes zeichnen
		CString s;
		s.Format("Ships\\%s.png",pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetShipClass());
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.png");
		if (graphic)
		{
			g->DrawImage(graphic, 388, 90, 200, 150);			
			graphic = NULL;
		}
		// allgemeine Schiffsinformationen anzeigen
		pDoc->m_ShipInfoArray.GetAt(ShipNumber).DrawShipInformation(g, CRect(200,250,780,440), &Gdiplus::Font(fontName.AllocSysString(), fontSize), normalColor, markColor, pMajor->GetEmpire()->GetResearch());
		// Baukosten des Schiffes anzeigen
		fontBrush.SetColor(markColor);
		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentNear);
		g->DrawString(CResourceManager::GetString("BUILDCOSTS").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,440,580,25), &fontFormat, &fontBrush);
		
		fontBrush.SetColor(normalColor);
		s.Format("%s: %d  %s: %d  %s: %d",CResourceManager::GetString("INDUSTRY"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededIndustry(),
			CResourceManager::GetString("TITAN"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededTitan(),
			CResourceManager::GetString("DEUTERIUM"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededDeuterium());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,465,580,25), &fontFormat, &fontBrush);
		
		s.Format("%s: %d  %s: %d  %s: %d  %s: %d",CResourceManager::GetString("DURANIUM"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededDuranium(),
			CResourceManager::GetString("CRYSTAL"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededCrystal(),
			CResourceManager::GetString("IRIDIUM"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededIridium(),
			CResourceManager::GetString("DILITHIUM"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetNeededDilithium());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,490,580,25), &fontFormat, &fontBrush);	
		
		// Die Buttons zur Eigenschafts�nderung in der Rechten Seite der Ansicht anzeigen
		// zuerst �berpr�fen wir die Beamwaffen, wir k�nnen den Typ der Beamwaffe ver�ndern, wenn wir mindst. ein anderes
		// Schiff des Imperiums finden, welches DIESE Beamwaffe mit einem h�heren Typ oder einem niedrigeren Typ besitzt
		
		graphic = NULL;
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.png");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);

		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentCenter);

		// Nach Beamwaffen suchen
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetSize() > m_iBeamWeaponNumber)
		{	
			// gibt es schon von dieser Beamwaffe hier auf dem Schiff einen h�heren Typ?
			USHORT maxTyp =	pMajor->GetWeaponObserver()->GetMaxBeamType(pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamName());
			if (maxTyp > pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType())
			{
				// Dann k�nnen wir den Typ unserer Beamwaffe(n) verbessern
				if (graphic)
					g->DrawImage(graphic, 930, 120, 120, 30);
				g->DrawString(CResourceManager::GetString("BTN_STRONGER").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(930,120,120,30), &fontFormat, &btnBrush);
				m_bFoundBetterBeam = TRUE;
			}
			// Wenn wir einen gr��eren Typ als Typ 1 haben, dann k�nnen wir diesen verringern
			if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType() > 1)
			{
				// Dann k�nnen wir den Typ unserer Beamwaffe(n) verkleinern
				if (graphic)
					g->DrawImage(graphic, 800, 120, 120, 30);
				g->DrawString(CResourceManager::GetString("BTN_WEAKER").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,120,120,30), &fontFormat, &btnBrush);
				m_bFoundWorseBeam = TRUE;
			}
			
			// Typ und Name der Beamwaffe zeichnen
			fontBrush.SetColor(normalColor);
			s.Format("%s %d %s",CResourceManager::GetString("TYPE"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType(),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamName());
			fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(845,80,160,25), &fontFormat, &fontBrush);
			fontFormat.SetTrimming(StringTrimmingNone);
		}
		
		// Nach anderer Torpedowaffe suchen
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetTorpedoWeapons()->GetSize() > m_iTorpedoWeaponNumber)
		{
			// den aktuellen Torpedotyp holen
			USHORT currentTorpType = pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTorpedoType();
			// Torpedoname zeichnen
			fontBrush.SetColor(normalColor);
			s.Format("%s",pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeName());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,170,300,25), &fontFormat, &fontBrush);
			
			s.Format("%s",pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTorpedoName());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,195,300,25), &fontFormat, &fontBrush);
			
			if (graphic)
				g->DrawImage(graphic, 800, 230, 120, 30);
			g->DrawString(CResourceManager::GetString("BTN_LAUNCHER").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,230,120,30), &fontFormat, &btnBrush);
			if (graphic)
				g->DrawImage(graphic, 930, 230, 120, 30);
			g->DrawString(CResourceManager::GetString("BTN_TORPEDO").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(930,230,120,30), &fontFormat, &btnBrush);
		}
		
		// hier M�glichkeit anderes H�llenmaterial anzubringen eingebaut
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
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,380,300,30), &fontFormat, &fontBrush);
				
		// Hier kann man den Schildtyp �ndern
		// zuerst Anzeige der jetzt aktuellen Schilde. Beim Romulaner eine schwarze Schriftart w�hlen. Wenn dies
		// sp�ter auch bei der F�d heller unterlegt ist kann auch dort eine schwarze Schriftfarbe gew�hlt werden.
		s.Format("%s %d %s",CResourceManager::GetString("TYPE"),pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetShield()->GetShieldType(),CResourceManager::GetString("SHIELDS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,490,300,30), &fontFormat, &fontBrush);
			
		// Ab jetzt die Buttons zum �ndern der jeweiligen Komponenten
		if (graphic)
			g->DrawImage(graphic, 800, 420, 120, 30);
		g->DrawString(CResourceManager::GetString("BTN_MATERIAL").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,420,120,30), &fontFormat, &btnBrush);
		if (graphic)
			g->DrawImage(graphic, 930, 420, 120, 30);
		g->DrawString(CResourceManager::GetString("BTN_HULLTYPE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(930,420,120,30), &fontFormat, &btnBrush);
		
		// Schildtyp schw�cher Button einblenden
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetShield()->GetShieldType() > 0)
		{
			if (graphic)
				g->DrawImage(graphic, 800, 540, 120, 30);
			g->DrawString(CResourceManager::GetString("BTN_WEAKER").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,540,120,30), &fontFormat, &btnBrush);			
		}
		// Schildtyp st�rker Button einblenden
		if (pDoc->m_ShipInfoArray.GetAt(ShipNumber).GetShield()->GetShieldType() < pMajor->GetWeaponObserver()->GetMaxShieldType())
		{
			if (graphic)
				g->DrawImage(graphic, 930, 540, 120, 30);
			g->DrawString(CResourceManager::GetString("BTN_STRONGER").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(930,540,120,30), &fontFormat, &btnBrush);
		}
	}
	// Wenn das Schiff in irgendeinem unserer Systeme gebaut wird, dann gro�en Text ausgeben, in welchem System das Schiff
	// gerade gebaut wird
	CString systemName = CheckIfShipIsBuilding(ShipNumber);
	if (!systemName.IsEmpty())
	{
		COverlayBanner *banner = new COverlayBanner(CPoint(200,300), CSize(580, 200),
			CResourceManager::GetString("NO_CHANGE_POSSIBLE", FALSE, systemName), RGB(220,0,0));
		banner->Draw(g, &Gdiplus::Font(fontName.AllocSysString(), fontSize));
		delete banner;
	}

	// "Schiffsdesign" in der Mitte zeichnen
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	g->DrawString(CResourceManager::GetString("SHIPDESIGN").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(188,10,600,50), &fontFormat, &fontBrush);
}

void CShipDesignMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

	// Wenn wir in der Schiffsdesignansicht sind
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	// Schiffsinfoarray durchgehen und nach zum Imperium geh�rende baubare Schiffe suchen
	
	short j = 0;
	short counter = m_iClickedOnShip - 23 + m_iOldClickedOnShip;
	short add = 0;
	short n = -1;
	
	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
		if (pDoc->m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			if (pDoc->m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
				if (pDoc->m_ShipInfoArray.GetAt(i).GetShipType() != OUTPOST && pDoc->m_ShipInfoArray.GetAt(i).GetShipType() != STARBASE)
				{
					// wurde dieses Schiff durch kein anderes jetzt baubares Schiff schon obsolet?
					BOOLEAN foundObsolet = FALSE;
					for (int m = 0; m < pDoc->m_ShipInfoArray.GetSize(); m++)
						if (pDoc->m_ShipInfoArray.GetAt(m).GetRace() == pMajor->GetRaceShipNumber())
							if (pDoc->m_ShipInfoArray.GetAt(m).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
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
						// M�ssen sp�ter noch die Seitenzahl beachten, bis jetzt aber wie in der Diplomatieansicht
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
	// Bevor wir irgendetwas �ndern k�nnen m�ssen wir �berpr�fen, dass das gerade angeklickte Schiff nicht gerade
	// gebaut wird. Wenn das der Fall sein sollte k�nnen wir nix �ndern. Es kommt dann eine Meldung in welchem
	// System das Schiff gerade gebaut wird
	if (CRect(r.right-300,80,r.right,r.bottom-80).PtInRect(point))
		if (!CheckIfShipIsBuilding(n).IsEmpty())
		{
			Invalidate(FALSE);
			return;
		}		
	counter = 0;
	// �berpr�fen ob irgendetwas an den Beamwaffen �ndern m�chte
	if (n != -1 && pDoc->m_ShipInfoArray.GetAt(n).GetBeamWeapons()->GetSize() > 0)
	{
		// Hat das Schiff mehr als eine Beamwaffe k�nnen wir auf die n�chste zugreifen indem wir hier klicken
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
			// Dann wird der Typ bei der aktuellen Beamwaffe um eins erh�ht
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
			// Dann wird der Typ bei der aktuellen Beamwaffe um eins erh�ht
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

	// �berpr�fen ob wir irgendetwas an einer Torpedowaffe �ndern m�chten
	if (n != -1 && pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetSize() > 0)
	{
		// Hat das Schiff mehr als eine Torpedowaffe k�nnen wir auf die n�chste zugreifen indem wir hier klicken
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
		// Haben wir auf den Button geklickt um den Torpedowerfer zu �ndern
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
			TupeWeaponsObserverStruct twos = pMajor->GetWeaponObserver()->GetNextTupe(oldTupeName,oldTorpType);
			// hier aktualisieren
			pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).ModifyTorpedoWeapon(oldTorpType,twos.number,twos.fireRate,oldTupeNumber,twos.TupeName,twos.onlyMicro,oldAcc);
			pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
			Invalidate();
		}
		// Haben wir auf den Button geklicht um den Torpedotyp zu �ndern
		else if (CRect(r.right-145,140+counter*90,r.right-25,170+counter*90).PtInRect(point))
		{
			BYTE oldTorpType = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTorpedoType();
			BYTE oldNumber   = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetNumber();
			BYTE oldFirerate = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeFirerate();
			BYTE oldTupeNumber=pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetNumberOfTupes();
			CString oldTupeName= pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeName();
			BOOLEAN oldOnlyMicro  = pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetOnlyMicroPhoton();
			BYTE oldAcc		= pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetAccuracy();
			BYTE newTorpType = pMajor->GetWeaponObserver()->GetNextTorpedo(oldTorpType, oldOnlyMicro);
			// hier aktualisieren
			pDoc->m_ShipInfoArray.GetAt(n).GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).ModifyTorpedoWeapon(newTorpType,oldNumber,oldFirerate,oldTupeNumber,oldTupeName,oldOnlyMicro,oldAcc);
			pDoc->m_ShipInfoArray.GetAt(n).CalculateFinalCosts();
			Invalidate();
		}
	}
	counter++;
	// �berpr�fen ob wir das H�llenmaterial �ndern m�chten, also ob wir auf den Button "H�llenmaterial �ndern" geklickt haben
	if (n != -1 && CRect(r.right-275,180+counter*120,r.right-155,210+counter*120).PtInRect(point))
	{
		BOOLEAN oldDoubleHull = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetDoubleHull();
		ULONG oldBaseHull = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetBaseHull();
		BOOLEAN ablative = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetAblative();
		BOOLEAN polarisation = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetPolarisation();
		// Dann bekommt das n�chste Schiff ein neues H�llenmaterial
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
	// �berpr�fen ob wir geklickt haben um die H�llenart zu wechseln (also Einzel- oder Doppelh�lle)
	else if (n != -1 && CRect(r.right-145,180+counter*120,r.right-25,210+counter*120).PtInRect(point))
	{
		BOOLEAN oldDoubleHull = pDoc->m_ShipInfoArray.GetAt(n).GetHull()->GetDoubleHull();
		// Wenn die alte H�lle eine Einzelh�lle war und man eine Doppelh�lle anbaut, dann verringert sich die
		// Man�vriebarkeit um -1. Wenn man eine Einzelh�lle anbaut, dann kommt zur Man� +1 dazu. Schiffe mit
		// Man� 1 oder Man� 8 sind von dieser Reglung ausgeschlossen.
		if (pDoc->m_ShipInfoArray.GetAt(n).GetManeuverability() > 1 && pDoc->m_ShipInfoArray.GetAt(n).GetManeuverability() < 8)
		{
			// wollen Doppelh�lle draus machen
			if (oldDoubleHull == FALSE)
				pDoc->m_ShipInfoArray.GetAt(n).SetManeuverability(pDoc->m_ShipInfoArray.GetAt(n).GetManeuverability()-1);
			// wollen eine Einzelh�lle draus machen
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
	// �berpr�fen ob ich geklickt habe um den Schildtyp zu verringern
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
	// �berpr�fen ob ich geklickt habe um den Schildtyp zu erh�hen
	else if (n != -1 && CRect(r.right-145,300+counter*120,r.right-25,325+counter*120).PtInRect(point))
	{
		USHORT oldShieldType= pDoc->m_ShipInfoArray.GetAt(n).GetShield()->GetShieldType();
		if (pMajor->GetWeaponObserver()->GetMaxShieldType() > oldShieldType)
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

/// Funktion �berpr�ft ob das in der Designansicht angeklickte Schiff in einem unserer Systeme gerade gebaut wird
/// Man ben�tigt diesen Check da man keine Schiffe �ndern kann, welche gerade gebaut werden.
/// @param n Index des zu pr�fenden Schiffes aus der Schiffsliste
/// @return CString mit dem Namen des Systems, wird das Schiff nirgends gebaut ist der String leer
CString CShipDesignMenuView::CheckIfShipIsBuilding(int n) const
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);
	if (!pMajor)
		return "";

	if (m_iClickedOnShip != -1 && n < pDoc->m_ShipInfoArray.GetSize() && n >= 0)
	{
		USHORT ID = pDoc->m_ShipInfoArray.GetAt(n).GetID();
		// alle eigenen Systeme durchgehen und schauen, ob an erster Stelle in der Bauliste so ein Schiff steht
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (pDoc->GetSystem(x,y).GetOwnerOfSystem() == pMajor->GetRaceID())
					for (int i = 0; i < ALE; i++)
						if (pDoc->GetSystem(x,y).GetAssemblyList()->GetAssemblyListEntry(i) == ID)
							return pDoc->GetSector(x,y).GetName();
	}
	return "";
}

void CShipDesignMenuView::CreateButtons()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);

	// alle Buttons in der View anlegen und Grafiken laden	
}