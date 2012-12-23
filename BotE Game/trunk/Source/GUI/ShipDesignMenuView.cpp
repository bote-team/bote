// ShipDesignMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "ShipDesignMenuView.h"
#include "ShipDesignBottomView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"
#include "General/ResourceManager.h"

IMPLEMENT_DYNCREATE(CShipDesignMenuView, CMainBaseView)

CShipDesignMenuView::CShipDesignMenuView() :
	bg_designmenu(),
	m_iClickedOnShip(-1),
	m_iOldClickedOnShip(-1),
	m_nSizeOfShipDesignList(0),
	m_iBeamWeaponNumber(0),
	m_iTorpedoWeaponNumber(0),
	m_bFoundBetterBeam(FALSE),
	m_bFoundWorseBeam(FALSE),
	m_pShownShip(NULL)
{

}

CShipDesignMenuView::~CShipDesignMenuView()
{
}

BEGIN_MESSAGE_MAP(CShipDesignMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CShipDesignMenuView::OnNewRound()
{
	m_iClickedOnShip = -1;
	m_iOldClickedOnShip = -1;
	m_iBeamWeaponNumber = 0;
	m_iTorpedoWeaponNumber = 0;
	m_bFoundBetterBeam = FALSE;
	m_bFoundWorseBeam = FALSE;
	m_pShownShip = NULL;
}
// CShipDesignMenuView drawing

void CShipDesignMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	// TODO: add draw code here
	// Doublebuffering wird initialisiert
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

	DrawShipDesignMenue(&g);

	g.ReleaseHDC(pDC->GetSafeHdc());
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

	// Schiffsdesignansicht
	m_iClickedOnShip = -1;
	m_iOldClickedOnShip = -1;
	m_iBeamWeaponNumber = 0;
	m_iTorpedoWeaponNumber = 0;
	m_bFoundBetterBeam = FALSE;
	m_bFoundWorseBeam = FALSE;
	m_pShownShip = NULL;

	// View bei den Tooltipps anmelden
	resources::pMainFrame->AddToTooltip(this);
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CShipDesignMenuView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CreateButtons();

	CString sPrefix = pMajor->GetPrefix();
	bg_designmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "designmenu.boj");
}

void CShipDesignMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CShipDesignMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Schiffsdesignmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CShipDesignMenuView::DrawShipDesignMenue(Graphics* g)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pDoc);
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


	// Links im Bild die veränderbaren Schiffklassen zeichnen (bis jetzt darf man keine Stationen verändern,
	// weil deren Baukosten allein von den Industriekosten berechnet werden. Diese aber nicht steigen wenn
	// man die Hülle oder Schilde verbessert. Somit könnte man bessere Stationen für den gleichen Preis bauen.

	// Schiffsinfoarray durchgehen und nach zum Imperium gehörende baubare Schiffe suchen
	short j = 0;
	short counter = m_iClickedOnShip - 23 + m_iOldClickedOnShip;
	short oldClickedShip = m_iClickedOnShip;

	BYTE researchLevels[6] =
	{
		pMajor->GetEmpire()->GetResearch()->GetBioTech(),
		pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
		pMajor->GetEmpire()->GetResearch()->GetCompTech(),
		pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
		pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
		pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
	};

	m_pShownShip = NULL;
	m_nSizeOfShipDesignList = 0;
	// Es gehen nur 21 Einträge auf die Seite, deshalb muss abgebrochen werden
	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
		if (pDoc->m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			if (!pDoc->m_ShipInfoArray.GetAt(i).IsStation())
				if (pDoc->m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(researchLevels))
				{
					// wurde dieses Schiff durch kein anderes jetzt baubares Schiff schon obsolet?
					BOOLEAN foundObsolet = FALSE;
					for (int m = 0; m < pDoc->m_ShipInfoArray.GetSize(); m++)
						if (pDoc->m_ShipInfoArray.GetAt(m).GetRace() == pMajor->GetRaceShipNumber())
							if (pDoc->m_ShipInfoArray.GetAt(m).GetObsoleteShipClass() == pDoc->m_ShipInfoArray.GetAt(i).GetShipClass())
								if (pDoc->m_ShipInfoArray.GetAt(m).IsThisShipBuildableNow(researchLevels))
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

							m_pShownShip = &pDoc->m_ShipInfoArray.GetAt(i);
							// Infos in View 3 aktualisieren
							if (pDoc->m_iShowWhichShipInfoInView3 != i)
							{
								pDoc->m_iShowWhichShipInfoInView3 = i;
								resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CShipDesignBottomView));
							}
							// Markierung worauf wir geklickt haben
							g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(15,120+j*25,183,25));
							g->DrawLine(&Gdiplus::Pen(penColor), 15, 120+j*25, 198, 120+j*25);
							g->DrawLine(&Gdiplus::Pen(penColor), 15, 145+j*25, 198, 145+j*25);
						}
						CString s = pDoc->m_ShipInfoArray.GetAt(i).GetShipClass();
						g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(25, 120 + j * 25, 175, 25), &fontFormat, &fontBrush);
						j++;
					}
				}
	m_iClickedOnShip = oldClickedShip;

	// Hier jetzt Informationen zum angeklickten Schiff anzeigen
	if (m_pShownShip)
	{
		// Bild des Schiffes zeichnen
		CString s;
		s.Format("Ships\\%s.bop",m_pShownShip->GetShipClass());
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
		if (graphic)
		{
			g->DrawImage(graphic, 388, 90, 200, 150);
			graphic = NULL;
		}
		// allgemeine Schiffsinformationen anzeigen
		m_pShownShip->DrawShipInformation(g, CRect(220,250,740,440), &Gdiplus::Font(CComBSTR(fontName), fontSize), normalColor, markColor, pMajor->GetEmpire()->GetResearch());
		// Baukosten des Schiffes anzeigen
		fontBrush.SetColor(markColor);
		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentNear);
		g->DrawString(CComBSTR(CResourceManager::GetString("BUILDCOSTS")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190,440,580,25), &fontFormat, &fontBrush);

		fontBrush.SetColor(normalColor);
		s.Format("%s: %d  %s: %d  %s: %d",CResourceManager::GetString("INDUSTRY"),m_pShownShip->GetNeededIndustry(),
			CResourceManager::GetString("TITAN"),m_pShownShip->GetNeededTitan(),
			CResourceManager::GetString("DEUTERIUM"),m_pShownShip->GetNeededDeuterium());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190,465,580,25), &fontFormat, &fontBrush);

		s.Format("%s: %d  %s: %d  %s: %d  %s: %d",CResourceManager::GetString("DURANIUM"),m_pShownShip->GetNeededDuranium(),
			CResourceManager::GetString("CRYSTAL"),m_pShownShip->GetNeededCrystal(),
			CResourceManager::GetString("IRIDIUM"),m_pShownShip->GetNeededIridium(),
			CResourceManager::GetString("DERITIUM"),m_pShownShip->GetNeededDeritium());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190,490,580,25), &fontFormat, &fontBrush);

		// Die Buttons zur Eigenschaftsänderung in der Rechten Seite der Ansicht anzeigen
		// zuerst überprüfen wir die Beamwaffen, wir können den Typ der Beamwaffe verändern, wenn wir mindst. ein anderes
		// Schiff des Imperiums finden, welches DIESE Beamwaffe mit einem höheren Typ oder einem niedrigeren Typ besitzt

		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);

		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentCenter);

		// Nach Beamwaffen suchen
		if (m_pShownShip->GetBeamWeapons()->GetSize() > m_iBeamWeaponNumber)
		{
			// gibt es schon von dieser Beamwaffe hier auf dem Schiff einen höheren Typ?
			USHORT maxTyp =	pMajor->GetWeaponObserver()->GetMaxBeamType(m_pShownShip->GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamName());
			if (maxTyp > m_pShownShip->GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType())
			{
				// Dann können wir den Typ unserer Beamwaffe(n) verbessern
				if (graphic)
					g->DrawImage(graphic, 930, 120, 120, 30);
				g->DrawString(CComBSTR(CResourceManager::GetString("BTN_STRONGER")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(930,120,120,30), &fontFormat, &btnBrush);
				m_bFoundBetterBeam = TRUE;
			}
			// Wenn wir einen größeren Typ als Typ 1 haben, dann können wir diesen verringern
			if (m_pShownShip->GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType() > 1)
			{
				// Dann können wir den Typ unserer Beamwaffe(n) verkleinern
				if (graphic)
					g->DrawImage(graphic, 800, 120, 120, 30);
				g->DrawString(CComBSTR(CResourceManager::GetString("BTN_WEAKER")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(800,120,120,30), &fontFormat, &btnBrush);
				m_bFoundWorseBeam = TRUE;
			}

			// Typ und Name der Beamwaffe zeichnen
			fontBrush.SetColor(normalColor);
			s.Format("%s %d %s",CResourceManager::GetString("TYPE"),m_pShownShip->GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamType(),m_pShownShip->GetBeamWeapons()->GetAt(m_iBeamWeaponNumber).GetBeamName());
			fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(845,80,160,25), &fontFormat, &fontBrush);
			fontFormat.SetTrimming(StringTrimmingNone);
		}

		// Nach anderer Torpedowaffe suchen
		if (m_pShownShip->GetTorpedoWeapons()->GetSize() > m_iTorpedoWeaponNumber)
		{
			// den aktuellen Torpedotyp holen
			BYTE currentTorpType = m_pShownShip->GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTorpedoType();
			// Torpedoname zeichnen
			fontBrush.SetColor(normalColor);
			s.Format("%s (%d°)",m_pShownShip->GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetTupeName(), m_pShownShip->GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber).GetFirearc()->GetAngle());
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(775,170,300,25), &fontFormat, &fontBrush);

			s.Format("%s (%d)", CTorpedoInfo::GetName(currentTorpType), CTorpedoInfo::GetPower(currentTorpType));
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(775,195,300,25), &fontFormat, &fontBrush);

			if (graphic)
				g->DrawImage(graphic, 800, 230, 120, 30);
			g->DrawString(CComBSTR(CResourceManager::GetString("BTN_LAUNCHER")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(800,230,120,30), &fontFormat, &btnBrush);
			if (graphic)
				g->DrawImage(graphic, 930, 230, 120, 30);
			g->DrawString(CComBSTR(CResourceManager::GetString("BTN_TORPEDO")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(930,230,120,30), &fontFormat, &btnBrush);
		}

		// hier Möglichkeit anderes Hüllenmaterial anzubringen eingebaut
		CString material;
		switch (m_pShownShip->GetHull()->GetHullMaterial())
		{
			case TITAN:		material = CResourceManager::GetString("TITAN");; break;
			case DURANIUM:	material = CResourceManager::GetString("DURANIUM");; break;
			case IRIDIUM:	material = CResourceManager::GetString("IRIDIUM");; break;
			default: material = "";
		}

		BOOLEAN bDoubleHull = m_pShownShip->GetHull()->GetDoubleHull();
		if (bDoubleHull == TRUE)
			s.Format("%s%s",material, CResourceManager::GetString("DOUBLE_HULL_ARMOUR"));
		else
			s.Format("%s%s",material, CResourceManager::GetString("HULL_ARMOR"));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(775,380,300,30), &fontFormat, &fontBrush);

		// Hier kann man den Schildtyp ändern
		// zuerst Anzeige der jetzt aktuellen Schilde. Beim Romulaner eine schwarze Schriftart wählen. Wenn dies
		// später auch bei der Föd heller unterlegt ist kann auch dort eine schwarze Schriftfarbe gewählt werden.
		s.Format("%s %d %s",CResourceManager::GetString("TYPE"),m_pShownShip->GetShield()->GetShieldType(),CResourceManager::GetString("SHIELDS"));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(775,490,300,30), &fontFormat, &fontBrush);

		// Ab jetzt die Buttons zum Ändern der jeweiligen Komponenten
		if (graphic)
			g->DrawImage(graphic, 800, 420, 120, 30);
		g->DrawString(CComBSTR(CResourceManager::GetString("BTN_MATERIAL")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(800,420,120,30), &fontFormat, &btnBrush);

		// wenn eine Doppelhülle draus gemacht werden soll dann darf die Manövrierbarkeit nicht schon "keine" oder nur 1 sein
		// wenn eine Einzelhülle draus gemacht werden soll, dann darf die Manövrierbarkeit nicht schon phänomenal sein
		if ((bDoubleHull == FALSE && m_pShownShip->GetManeuverability() > 1) || (bDoubleHull == TRUE && m_pShownShip->GetManeuverability() < 9))
		{
			if (graphic)
				g->DrawImage(graphic, 930, 420, 120, 30);
			g->DrawString(CComBSTR(CResourceManager::GetString("BTN_HULLTYPE")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(930,420,120,30), &fontFormat, &btnBrush);
		}

		// Schildtyp schwächer Button einblenden
		if (m_pShownShip->GetShield()->GetShieldType() > 0)
		{
			if (graphic)
				g->DrawImage(graphic, 800, 540, 120, 30);
			g->DrawString(CComBSTR(CResourceManager::GetString("BTN_WEAKER")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(800,540,120,30), &fontFormat, &btnBrush);
		}
		// Schildtyp stärker Button einblenden
		if (m_pShownShip->GetShield()->GetShieldType() < pMajor->GetWeaponObserver()->GetMaxShieldType())
		{
			if (graphic)
				g->DrawImage(graphic, 930, 540, 120, 30);
			g->DrawString(CComBSTR(CResourceManager::GetString("BTN_STRONGER")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(930,540,120,30), &fontFormat, &btnBrush);
		}
	}
	// Wenn das Schiff in irgendeinem unserer Systeme gebaut wird, dann großen Text ausgeben, in welchem System das Schiff
	// gerade gebaut wird
	CString systemName = CheckIfShipIsBuilding(m_pShownShip);
	if (!systemName.IsEmpty())
	{
		COverlayBanner *banner = new COverlayBanner(CPoint(200,300), CSize(580, 200),
			CResourceManager::GetString("NO_CHANGE_POSSIBLE", FALSE, systemName), RGB(220,0,0));
		banner->Draw(g, &Gdiplus::Font(CComBSTR(fontName), fontSize));
		delete banner;
	}

	// "Schiffsdesign" in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	g->DrawString(CComBSTR(CResourceManager::GetString("SHIPDESIGN")), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(188,10,600,50), &fontFormat, &fontBrush);
}

void CShipDesignMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

	// Wenn wir in der Schiffsdesignansicht sind
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	// Schiffsinfoarray durchgehen und nach zum Imperium gehörende baubare Schiffe suchen

	short j = 0;
	short counter = m_iClickedOnShip - 23 + m_iOldClickedOnShip;
	short add = 0;

	BYTE researchLevels[6] =
	{
		pMajor->GetEmpire()->GetResearch()->GetBioTech(),
		pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
		pMajor->GetEmpire()->GetResearch()->GetCompTech(),
		pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
		pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
		pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
	};

	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
		if (pDoc->m_ShipInfoArray.GetAt(i).GetRace() == pMajor->GetRaceShipNumber())
			if (!pDoc->m_ShipInfoArray.GetAt(i).IsStation())
				if (pDoc->m_ShipInfoArray.GetAt(i).IsThisShipBuildableNow(researchLevels))
				{
					// wurde dieses Schiff durch kein anderes jetzt baubares Schiff schon obsolet?
					BOOLEAN foundObsolet = FALSE;
					for (int m = 0; m < pDoc->m_ShipInfoArray.GetSize(); m++)
						if (pDoc->m_ShipInfoArray.GetAt(m).GetRace() == pMajor->GetRaceShipNumber())
							if (pDoc->m_ShipInfoArray.GetAt(m).GetObsoleteShipClass() == pDoc->m_ShipInfoArray.GetAt(i).GetShipClass())
								if (pDoc->m_ShipInfoArray.GetAt(m).IsThisShipBuildableNow(researchLevels))
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
							m_pShownShip = &pDoc->m_ShipInfoArray.GetAt(i);
						j++;
					}
				}
	// Bevor wir irgendetwas ändern können müssen wir überprüfen, dass das gerade angeklickte Schiff nicht gerade
	// gebaut wird. Wenn das der Fall sein sollte können wir nix ändern. Es kommt dann eine Meldung in welchem
	// System das Schiff gerade gebaut wird
	if (CRect(r.right-300,80,r.right,r.bottom-80).PtInRect(point))
		if (!CheckIfShipIsBuilding(m_pShownShip).IsEmpty())
		{
			Invalidate(FALSE);
			return;
		}
	counter = 0;
	// Überprüfen ob irgendetwas an den Beamwaffen ändern möchte
	if (m_pShownShip && m_pShownShip->GetBeamWeapons()->GetSize() > 0)
	{
		// Hat das Schiff mehr als eine Beamwaffe können wir auf die nächste zugreifen indem wir hier klicken
		if (CRect(r.right-300,80,r.right,105).PtInRect(point))
		{
			if (m_pShownShip->GetBeamWeapons()->GetUpperBound() > m_iBeamWeaponNumber)
			{
				m_iBeamWeaponNumber++;
				m_bFoundBetterBeam = FALSE;
				m_bFoundWorseBeam  = FALSE;
				Invalidate();
			}
			else if (m_pShownShip->GetBeamWeapons()->GetUpperBound() == m_iBeamWeaponNumber)
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
			CBeamWeapons* pWeapon = &m_pShownShip->GetBeamWeapons()->GetAt(m_iBeamWeaponNumber);

			BYTE oldType		= pWeapon->GetBeamType();
			USHORT oldPower		= pWeapon->GetBeamPower();
			BYTE oldNumber		= pWeapon->GetBeamNumber();
			BYTE oldShootNumber = pWeapon->GetShootNumber();
			CString oldName		= pWeapon->GetBeamName();
			BYTE oldBonus		= pWeapon->GetBonus();
			BYTE oldLenght		= pWeapon->GetBeamLenght();
			BYTE oldRechargeTime= pWeapon->GetRechargeTime();
			BOOLEAN piercing	= pWeapon->GetPiercing();
			BOOLEAN modulating	= pWeapon->GetModulating();
			// hier aktualisieren -> Reichweite erhöhen
			pWeapon->ModifyBeamWeapon((oldType+1),oldPower,oldNumber,oldName,modulating,piercing,oldBonus,oldLenght,oldRechargeTime,oldShootNumber);
			// Feuerwinkel bleiben alle beim alten
			m_pShownShip->CalculateFinalCosts();
			m_bFoundBetterBeam = FALSE;
			m_bFoundWorseBeam  = FALSE;
			Invalidate();
		}
		// Haben wir auf den Button geklickt um den Typ der Beamwaffe zu verringern
		else if (m_bFoundWorseBeam == TRUE && CRect(r.right-275,120,r.right-155,150).PtInRect(point))
		{
			// Dann wird der Typ bei der aktuellen Beamwaffe um eins erhöht
			CBeamWeapons* pWeapon = &m_pShownShip->GetBeamWeapons()->GetAt(m_iBeamWeaponNumber);

			BYTE oldType		= pWeapon->GetBeamType();
			USHORT oldPower		= pWeapon->GetBeamPower();
			BYTE oldNumber		= pWeapon->GetBeamNumber();
			BYTE oldShootNumber = pWeapon->GetShootNumber();
			CString oldName		= pWeapon->GetBeamName();
			BYTE oldBonus		= pWeapon->GetBonus();
			BYTE oldLenght		= pWeapon->GetBeamLenght();
			BYTE oldRechargeTime= pWeapon->GetRechargeTime();
			BOOLEAN piercing	= pWeapon->GetPiercing();
			BOOLEAN modulating	= pWeapon->GetModulating();

			// hier aktualisieren -> Reichweite erhöhen
			pWeapon->ModifyBeamWeapon((oldType-1),oldPower,oldNumber,oldName,modulating,piercing,oldBonus,oldLenght,oldRechargeTime,oldShootNumber);
			// Feuerwinkel bleiben alle beim alten
			m_pShownShip->CalculateFinalCosts();
			m_bFoundBetterBeam = FALSE;
			m_bFoundWorseBeam  = FALSE;
			Invalidate();
		}
	}
	counter++;

	// Überprüfen ob wir irgendetwas an einer Torpedowaffe ändern möchten
	if (m_pShownShip && m_pShownShip->GetTorpedoWeapons()->GetSize() > 0)
	{
		// Hat das Schiff mehr als eine Torpedowaffe können wir auf die nächste zugreifen indem wir hier klicken
		if (CRect(r.right-300,80+counter*90,r.right,125+counter*90).PtInRect(point))
		{
			if (m_pShownShip->GetTorpedoWeapons()->GetUpperBound() > m_iTorpedoWeaponNumber)
			{
				m_iTorpedoWeaponNumber++;
				Invalidate();
			}
			else if (m_pShownShip->GetTorpedoWeapons()->GetUpperBound() == m_iTorpedoWeaponNumber)
			{
				m_iTorpedoWeaponNumber = 0;
				Invalidate();
			}
		}
		// Haben wir auf den Button geklickt um den Torpedowerfer zu ändern
		else if (CRect(r.right-275,140+counter*90,r.right-145,170+counter*90).PtInRect(point))
		{
			CTorpedoWeapons* pWeapon = &m_pShownShip->GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber);

			BYTE oldTorpType	= pWeapon->GetTorpedoType();
			BYTE oldTupeNumber	= pWeapon->GetNumberOfTupes();
			BYTE oldAcc			= pWeapon->GetAccuracy();
			CString oldTupeName	= pWeapon->GetTupeName();

			TupeWeaponsObserverStruct twos = pMajor->GetWeaponObserver()->GetNextTupe(oldTupeName,oldTorpType);
			// hier aktualisieren
			pWeapon->ModifyTorpedoWeapon(oldTorpType,twos.number,twos.fireRate,oldTupeNumber,twos.TupeName,twos.onlyMicro,oldAcc);
			USHORT nMountPos	= pWeapon->GetFirearc()->GetPosition();
			USHORT nAngle		= twos.fireAngle;
			pWeapon->GetFirearc()->SetValues(nMountPos, nAngle);

			m_pShownShip->CalculateFinalCosts();
			Invalidate();
		}
		// Haben wir auf den Button geklicht um den Torpedotyp zu ändern
		else if (CRect(r.right-145,140+counter*90,r.right-25,170+counter*90).PtInRect(point))
		{
			CTorpedoWeapons* pWeapon = &m_pShownShip->GetTorpedoWeapons()->GetAt(m_iTorpedoWeaponNumber);

			BYTE oldNumber		= pWeapon->GetNumber();
			BYTE oldFirerate	= pWeapon->GetTupeFirerate();
			BYTE oldTupeNumber	= pWeapon->GetNumberOfTupes();
			BOOLEAN oldOnlyMicro= pWeapon->GetOnlyMicroPhoton();
			BYTE oldAcc			= pWeapon->GetAccuracy();
			BYTE oldTorpType	= pWeapon->GetTorpedoType();
			CString oldTupeName	= pWeapon->GetTupeName();

			BYTE newTorpType = pMajor->GetWeaponObserver()->GetNextTorpedo(oldTorpType, oldOnlyMicro);
			// hier aktualisieren
			pWeapon->ModifyTorpedoWeapon(newTorpType,oldNumber,oldFirerate,oldTupeNumber,oldTupeName,oldOnlyMicro,oldAcc);
			// Feuerwinkel bleiben gleich
			m_pShownShip->CalculateFinalCosts();
			Invalidate();
		}
	}
	counter++;

	// Überprüfen ob wir das Hüllenmaterial ändern möchten, also ob wir auf den Button "Hüllenmaterial ändern" geklickt haben
	if (m_pShownShip && CRect(r.right-275,180+counter*120,r.right-155,210+counter*120).PtInRect(point))
	{
		CHull* pHull			= m_pShownShip->GetHull();

		BOOLEAN oldDoubleHull	= pHull->GetDoubleHull();
		ULONG oldBaseHull		= pHull->GetBaseHull();
		BOOLEAN ablative		= pHull->GetAblative();
		BOOLEAN polarisation	 = pHull->GetPolarisation();
		// Dann bekommt das nächste Schiff ein neues Hüllenmaterial
		switch (pHull->GetHullMaterial())
		{
		case TITAN: pHull->ModifyHull(oldDoubleHull,oldBaseHull,DURANIUM,ablative,polarisation);
			break;
		case DURANIUM: pHull->ModifyHull(oldDoubleHull,oldBaseHull,IRIDIUM,ablative,polarisation);
			break;
		case IRIDIUM: pHull->ModifyHull(oldDoubleHull,oldBaseHull,TITAN,ablative,polarisation);
			break;
		}
		m_pShownShip->CalculateFinalCosts();
		Invalidate();
	}
	// Überprüfen ob wir geklickt haben um die Hüllenart zu wechseln (also Einzel- oder Doppelhülle)
	else if (m_pShownShip && CRect(r.right-145,180+counter*120,r.right-25,210+counter*120).PtInRect(point))
	{
		CHull* pHull			= m_pShownShip->GetHull();

		BOOLEAN oldDoubleHull	= pHull->GetDoubleHull();

		// wenn eine Doppelhülle draus gemacht werden soll dann darf die Manövrierbarkeit nicht schon "keine" oder nur 1 sein
		if (oldDoubleHull == FALSE && m_pShownShip->GetManeuverability() <= 1)
			return;
		// wenn eine Einzelhülle draus gemacht werden soll, dann darf die Manövrierbarkeit nicht schon phänomenal sein
		if (oldDoubleHull == TRUE && m_pShownShip->GetManeuverability() == 9)
			return;

		// Wenn die alte Hülle eine Einzelhülle war und man eine Doppelhülle anbaut, dann verringert sich die
		// Manövriebarkeit um -1. Wenn man eine Einzelhülle anbaut, dann kommt zur Manö +1 dazu. Schiffe mit
		// Manö 0 oder Manö 9 sind von dieser Reglung ausgeschlossen.
		if (m_pShownShip->GetManeuverability() >= 0 && m_pShownShip->GetManeuverability() <= 9)
		{
			// wollen Doppelhülle draus machen
			if (oldDoubleHull == FALSE)
				m_pShownShip->SetManeuverability(m_pShownShip->GetManeuverability()-1);
			// wollen eine Einzelhülle draus machen
			else
				m_pShownShip->SetManeuverability(m_pShownShip->GetManeuverability()+1);
		}
		BOOLEAN ablative		= pHull->GetAblative();
		BOOLEAN polarisation	= pHull->GetPolarisation();
		ULONG oldBaseHull		= pHull->GetBaseHull();
		BYTE oldHullMaterial	= pHull->GetHullMaterial();

		pHull->ModifyHull(!oldDoubleHull,oldBaseHull,oldHullMaterial,ablative,polarisation);
		m_pShownShip->CalculateFinalCosts();
		Invalidate();
	}
	// Überprüfen ob ich geklickt habe um den Schildtyp zu verringern
	else if (m_pShownShip && CRect(r.right-275,300+counter*120,r.right-155,325+counter*120).PtInRect(point))
	{
		if (m_pShownShip->GetShield()->GetShieldType() > 0)
		{
			CShield* pShield = m_pShownShip->GetShield();

			UINT oldMaxShield	= pShield->GetMaxShield();
			BYTE oldShieldType	= pShield->GetShieldType();
			BOOLEAN regenerative= pShield->GetRegenerative();

			pShield->ModifyShield(oldMaxShield, (oldShieldType - 1), regenerative);
			m_pShownShip->CalculateFinalCosts();
			Invalidate();
		}
	}
	// Überprüfen ob ich geklickt habe um den Schildtyp zu erhöhen
	else if (m_pShownShip && CRect(r.right-145,300+counter*120,r.right-25,325+counter*120).PtInRect(point))
	{
		CShield* pShield = m_pShownShip->GetShield();

		USHORT oldShieldType = pShield->GetShieldType();
		if (pMajor->GetWeaponObserver()->GetMaxShieldType() > oldShieldType)
		{
			UINT oldMaxShield	= pShield->GetMaxShield();
			BOOLEAN regenerative= pShield->GetRegenerative();

			pShield->ModifyShield(oldMaxShield, (oldShieldType + 1), regenerative);
			m_pShownShip->CalculateFinalCosts();
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

void CShipDesignMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	if (nChar == VK_DOWN)
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
	else if (nChar == VK_UP)
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

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/// Funktion überprüft ob das in der Designansicht angeklickte Schiff in einem unserer Systeme gerade gebaut wird
/// Man benötigt diesen Check da man keine Schiffe ändern kann, welche gerade gebaut werden.
/// @param pShipInfo Zeiger des zu prüfenden Schiffes aus der Schiffsliste
/// @return CString mit dem Namen des Systems, wird das Schiff nirgends gebaut ist der String leer
CString CShipDesignMenuView::CheckIfShipIsBuilding(const CShipInfo* pShipInfo) const
{
	if (!pShipInfo)
		return "";

	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return "";

	USHORT ID = pShipInfo->GetID();
	// alle eigenen Systeme durchgehen und schauen, ob an erster Stelle in der Bauliste so ein Schiff steht
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).GetOwnerOfSystem() == pMajor->GetRaceID())
				for (int i = 0; i < ALE; i++)
					if (pDoc->GetSystem(x,y).GetAssemblyList()->GetAssemblyListEntry(i) == ID)
						return pDoc->GetSector(x,y).GetName();

	return "";
}

void CShipDesignMenuView::CreateButtons()
{
	ASSERT((CBotf2Doc*)GetDocument());

	ASSERT(m_pPlayersRace);

	// alle Buttons in der View anlegen und Grafiken laden
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CShipDesignMenuView::CreateTooltip(void)
{
	if (!m_pShownShip)
		return "";

	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	if (CRect(388, 90, 388 + 200, 90 + 150).PtInRect(pt))
	{
		// Schiff erzeugen und Spezialforschungen einbeziehen
		CShip ship = pDoc->m_ShipInfoArray[m_pShownShip->GetID() - 10000];
		pDoc->AddSpecialResearchBoniToShip(&ship, m_pPlayersRace);
		return ship.GetTooltip();
	}

	return "";
}
