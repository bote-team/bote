// ShipBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "IOData.h"
#include "GalaxyMenuView.h"
#include "FleetMenuView.h"
#include "TransportMenuView.h"
#include "ShipBottomView.h"
#include "PlanetBottomView.h"
#include "SmallInfoView.h"
#include "Races\RaceController.h"
#include "Ships\Fleet.h"
#include "HTMLStringBuilder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CShipBottomView
BOOLEAN CShipBottomView::m_bShowStation = FALSE;

IMPLEMENT_DYNCREATE(CShipBottomView, CBottomBaseView)

CShipBottomView::CShipBottomView()
{
	m_pShipOrderButton = NULL;
}

CShipBottomView::~CShipBottomView()
{
	if (m_pShipOrderButton)
	{
		delete m_pShipOrderButton;
		m_pShipOrderButton = NULL;
	}
}

BEGIN_MESSAGE_MAP(CShipBottomView, CBottomBaseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CShipBottomView::OnNewRound()
{
	m_iPage = 1;
	m_vShipRects.clear();
	m_pMarkedShip = NULL;
	m_rLastMarkedRect = CRect(0,0,0,0);
}

// CShipBottomView drawing

void CShipBottomView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;
	// TODO: add draw code here

	// Doublebuffering wird initialisiert
	CMemDC pDC(dc);
	CRect client;
	GetClientRect(&client);
		
	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());
	
	g.Clear(Color::Black);
	/*g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);*/
	g.SetSmoothingMode(SmoothingModeHighQuality);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);
				
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);

	StringFormat fontFormat;	
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);
	
	Bitmap* graphic = NULL;		
	
	CRect r;
	r.SetRect(0, 0, m_TotalSize.cx, m_TotalSize.cy);
				
	USHORT counter = 0;
	USHORT column = 0;
	USHORT row = 0;
	USHORT oneShip = 0;
	
	CString s;
	if (m_LastKO != pDoc->GetKO() || m_bShowStation)	// Wir zeigen nen Außenposten
	{
		m_iPage = 1;
		m_LastKO = pDoc->GetKO();
	}
	// Galaxie im Hintergrund zeichnen
	CString sPrefix = pMajor->GetPrefix();	
	Bitmap* background = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "galaxyV3.bop");
	if (background)
		g.DrawImage(background, 0, 0, 1075, 249);

	// Bis jetzt nur eine Anzeige bis max. 9 Schiffe
	if (m_iTimeCounter == 0)
	{
		if (!CGalaxyMenuView::IsMoveShip())
			m_vShipRects.clear();

		Gdiplus::Color markColor;
		markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
		Gdiplus::Font font(fontName.AllocSysString(), fontSize);

		for (int i = 0; i < pDoc->m_ShipArray.GetSize(); i++)
		{
			CShip* pShip = &pDoc->m_ShipArray[i];
			if (pDoc->GetKO() != pShip->GetKO())
				continue;			

			// Wenn eine Station angezeigt werden soll, dann muss der Typ von einer Station sein
			if (m_bShowStation && pShip->GetShipType() != OUTPOST && pShip->GetShipType() != STARBASE)
				continue;

			// Wenn keine Station angezeigt werden soll, dann darf der Typ nicht von einer Station sein
			if (!m_bShowStation && (pShip->GetShipType() == OUTPOST || pShip->GetShipType() == STARBASE))
				continue;
			
			// Schiffe mit zu guter Stealthpower werden hier nicht angezeigt.
			USHORT stealthPower = MAXBYTE;
			if (pShip->GetFleet() == NULL)
			{					
				stealthPower = pShip->GetStealthPower() * 20;
				if (pShip->GetStealthPower() > 3 && pShip->GetCloak() == FALSE)
					stealthPower = 3 * 20;
			}
			else
			{
				stealthPower = pShip->GetFleet()->GetFleetStealthPower(pShip);
			}

			if (pShip->GetOwnerOfShip() != pMajor->GetRaceID() && pDoc->GetSector(pDoc->GetKO()).GetScanPower(pMajor->GetRaceID()) < stealthPower)
				continue;
				
			// mehrere Spalten anlegen, falls mehr als 3 Schiffe in dem System sind
			if (counter != 0 && counter%3 == 0)
			{
				column++;
				row = 0;
			}
			// Wenn wir eine Seite vollhaben
			if (counter%9 == 0)
				column = 0;

			if (counter < m_iPage*9 && counter >= (m_iPage-1)*9)
			{
				// Kennen wir den Besizter des Schiffes?
				bool bUnknown = (pMajor->GetRaceID() != pShip->GetOwnerOfShip() && pMajor->IsRaceContacted(pShip->GetOwnerOfShip()) == false);
				// ist das Schiff gerade markiert?
				bool bMarked = (i == pDoc->GetCurrentShipIndex());
				CPoint pt(250 * column, 65 * row);
				pShip->DrawShip(&g, pDoc->GetGraphicPool(), pt, bMarked, bUnknown, TRUE, normalColor, markColor, font);
				m_vShipRects.push_back(pair<CRect, CShip*>(CRect(pt.x, pt.y + 20, pt.x + 250, pt.y + 85), pShip));
			}

			// großes Bild der Station zeichnen
			if (m_bShowStation)
			{
				map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
				for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
				{
					if (pDoc->GetSector(pDoc->GetKO()).GetOutpost(it->first) || pDoc->GetSector(pDoc->GetKO()).GetStarbase(it->first))
					{
						s.Format("Other\\" + it->second->GetPrefix() + "Starbase.bop");
						graphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
						if (graphic)
							g.DrawImage(graphic, 550, 20, 235, 200);
						break;
					}
				}
				break;
			}
			
			row++;
			counter++;
			oneShip = i;

			if (counter > m_iPage*9)
				break;
		}

		// Wenn nur ein Schiff in dem System ist, so wird es automatisch ausgewählt
		if (counter == 1 && !m_bShowStation && pDoc->m_ShipArray[oneShip].GetCurrentOrder() <= ATTACK
			&& pDoc->m_ShipArray[oneShip].GetOwnerOfShip() == pMajor->GetRaceID())
		{
			// Wenn wenn wir auf der Galaxiekarte sind
			if (pDoc->GetMainFrame()->GetActiveView(0, 1) == GALAXY_VIEW)
			{
				this->SetTimer(1,100,NULL);
				pDoc->SetCurrentShipIndex(oneShip);
				CGalaxyMenuView::SetMoveShip(TRUE);
				CSmallInfoView::SetShipInfo(true);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
				m_iWhichMainShipOrderButton = -1;
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CGalaxyMenuView));
			}
		}

		// Die Buttons für vor und zurück darstellen, wenn wir mehr als 9 Schiffe in dem Sektor sehen
		fontBrush.SetColor(this->GetFontColorForSmallButton());
		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
		m_bShowNextButton = FALSE;
		if (counter > 9 && counter > m_iPage*9)
		{
			m_bShowNextButton = TRUE;
			g.DrawImage(m_pShipOrderButton, r.right-120, r.top+210, 120, 30);
			g.DrawString(CResourceManager::GetString("BTN_NEXT").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120,r.top+210,120,30), &fontFormat, &fontBrush);
		}
		// back-Button
		if (m_iPage > 1)
		{
			g.DrawImage(m_pShipOrderButton, r.right-120, r.top, 120, 30);
			g.DrawString(CResourceManager::GetString("BTN_BACK").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120,r.top,120,30), &fontFormat, &fontBrush);					
		}
	}
	
	// Die ganzen Befehlsbuttons für die Schiffe anzeigen
	if (CGalaxyMenuView::IsMoveShip() == TRUE && 
		pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetOwnerOfShip() == pMajor->GetRaceID())
	{
		BYTE researchLevels[6] =
		{
			pMajor->GetEmpire()->GetResearch()->GetBioTech(),
			pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
			pMajor->GetEmpire()->GetResearch()->GetCompTech(),
			pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
			pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
			pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
		};

		fontBrush.SetColor(this->GetFontColorForSmallButton());
		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

		short counter = 0;	// Zähler der die Anzahl der einzelnen Unterbuttons zählt
		CRect rect;
		// Taktik
		if (m_iTimeCounter > 0)
		{
			g.DrawImage(m_pShipOrderButton, r.right-120, r.top+70, 120, 30);
			g.DrawString(CResourceManager::GetString("BTN_TACTIC").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120,r.top+70,120,30), &fontFormat, &fontBrush);
		}
		// Befehl
		if (m_iTimeCounter > 1)
		{
			g.DrawImage(m_pShipOrderButton, r.right-120, r.top+105, 120, 30);
			g.DrawString(CResourceManager::GetString("BTN_ORDER").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120,r.top+105,120,30), &fontFormat, &fontBrush);					
		}
		// Aktion
		if (m_iTimeCounter > 2)
		{
			g.DrawImage(m_pShipOrderButton, r.right-120, r.top+140, 120, 30);
			g.DrawString(CResourceManager::GetString("BTN_ACTION").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-120,r.top+140,120,30), &fontFormat, &fontBrush);					
			if (m_iWhichMainShipOrderButton == -1)
			{
				this->KillTimer(1);
				m_iTimeCounter = 0;
			}
		}
		// Alle Rechtecke für die Buttons der Schiffsbefehle erstmal auf NULL setzen, damit wir nicht draufklicken
		// können. Wir dürfen ja nur auf Buttons klicken können, die wir auch sehen
		for (int j = 0; j <= TRAIN_SHIP; j++)
			m_ShipOrders[j].SetRect(0,0,0,0);

		// angreifen
		if (m_iTimeCounter > 3 && m_iWhichMainShipOrderButton == 0 &&
			pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() == AVOID)
		{
			g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70, 120, 30);
			m_ShipOrders[ATTACK].SetRect(r.right-245,r.top+70,r.right-125,r.top+100);
			g.DrawString(CResourceManager::GetString("BTN_ATTACK").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70,120,30), &fontFormat, &fontBrush);
			counter++;
		}
		// meiden
		else if (m_iTimeCounter > 3 && m_iWhichMainShipOrderButton == 0 &&
			pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() > AVOID)
		{
			g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70, 120, 30);
			m_ShipOrders[AVOID].SetRect(r.right-245,r.top+70,r.right-125,r.top+100);
			g.DrawString(CResourceManager::GetString("BTN_AVOID").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70,120,30), &fontFormat, &fontBrush);
			counter++;
		}
		// folgende Befehle gehen alle nur, wenn es keine Station ist
		if (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetShipType() != OUTPOST &&
			pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetShipType() != STARBASE)
		{
			// gruppieren
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 0)
			{
				g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70+counter*35, 120, 30);
				m_ShipOrders[CREATE_FLEET].SetRect(r.right-245,r.top+70+counter*35,r.right-125,r.top+100+counter*35);
				g.DrawString(CResourceManager::GetString("BTN_CREATE_FLEET").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70+counter*35,120,30), &fontFormat, &fontBrush);
				counter++;
			}
			// trainieren
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 0)
			{
				// Wenn in dem System die Möglichkeit des Schiffstrainings besteht
				if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetSunSystem() == TRUE && 
					pDoc->GetSystem(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSystem() == pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetOwnerOfShip() &&
					pDoc->GetSystem(pDoc->GetKO().x,pDoc->GetKO().y).GetProduction()->GetShipTraining() > 0)
				{
					g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70+counter*35, 120, 30);
					m_ShipOrders[TRAIN_SHIP].SetRect(r.right-245,r.top+70+counter*35,r.right-125,r.top+100+counter*35);
					g.DrawString(CResourceManager::GetString("BTN_TRAIN_SHIP").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70+counter*35,120,30), &fontFormat, &fontBrush);
					counter++;
				}
			}
			// tarnen (hier beachten wenn es eine Flotte ist, dort schauen ob sich jedes Schiff in der Flotte auch
			// tarnen kann)
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 0 &&
				// Ab hier check wegen Flotten, darum wirds lang
				((pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetStealthPower() > 3)
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()),CLOAK) == TRUE)))
			{
				g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70+counter*35, 120, 30);
				if (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCloak())
					s = CResourceManager::GetString("BTN_DECLOAK");
				else
					s = CResourceManager::GetString("BTN_CLOAK");
				m_ShipOrders[CLOAK].SetRect(r.right-245,r.top+70+counter*35,r.right-125,r.top+100+counter*35);
				g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70+counter*35,120,30), &fontFormat, &fontBrush);
				counter++;
			}
			// Systemangriff
			if (m_iTimeCounter > 3 && m_iWhichMainShipOrderButton == 1)
			{
				// Wenn im Sektor ein Sonnensystem ist
				if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetSunSystem() == TRUE &&
					// Wenn im System noch Bevölkerung vorhanden ist
					pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetCurrentHabitants() > 0.0f &&
					// Wenn das System nicht der Rasse gehört, der auch das Schiff gehört
					pDoc->GetSystem(pDoc->GetKO().x, pDoc->GetKO().y).GetOwnerOfSystem() != pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetOwnerOfShip())
				{	
					CRace* pOwnerOfSector = pDoc->GetRaceCtrl()->GetRace(pDoc->GetSector(pDoc->GetKO().x, pDoc->GetKO().y).GetOwnerOfSector());

					// Wenn im System eine Rasse lebt und wir mit ihr im Krieg sind
					if (pOwnerOfSector != NULL && pMajor->GetAgreement(pOwnerOfSector->GetRaceID()) == WAR
					// Wenn das System niemanden mehr gehört, aber noch Bevölkerung drauf lebt (z.B. durch Rebellion)
						|| pDoc->GetSystem(pDoc->GetKO().x, pDoc->GetKO().y).GetOwnerOfSystem() == "" && pDoc->GetSector(pDoc->GetKO().x, pDoc->GetKO().y).GetMinorRace() == FALSE)
					{
						// nur wenn die Schiffe ungetarnt sind können sie Bombardieren
						// Ab hier check wegen Flotten, darum wirds lang
						if ((pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() == 0
							&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCloak() == FALSE)
							|| (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() != 0
							&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()), ATTACK_SYSTEM) == TRUE))
						{
							g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70, 120, 30);
							s = CResourceManager::GetString("BTN_ATTACK_SYSTEM");
							m_ShipOrders[ATTACK_SYSTEM].SetRect(r.right-245,r.top+70,r.right-125,r.top+100);
							g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70,120,30), &fontFormat, &fontBrush);
							counter++;
						}
					}
				}
			}
/*			// Systemüberfall
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 1)
			{
				pDC->BitBlt(r.right-245,r.top+70+counter*35,120,30,&mdc,0,0,SRCCOPY);
				s = CResourceManager::GetString("BTN_RAID_SYSTEM");
				m_ShipOrders[RAID_SYSTEM].SetRect(r.right-245,r.top+70+counter*35,r.right-125,r.top+100+counter*35);
				pDC->DrawText(s, m_ShipOrders[RAID_SYSTEM],DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				counter++;
			}
*/			// Systemblockade
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 1 &&
				// Ab hier check wegen Flotten, darum wirds lang
				((pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).HasSpecial(BLOCKADESHIP))
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()), BLOCKADE_SYSTEM) == TRUE)))
			{
				// Überprüfen ob man eine Blockade im System überhaupt errichten kann
				// Wenn das System nicht der Rasse gehört, der auch das Schiff gehört
				CRace* pOwnerOfSystem = pDoc->GetRaceCtrl()->GetRace(pDoc->GetSystem(pDoc->GetKO().x, pDoc->GetKO().y).GetOwnerOfSystem());
				if (pOwnerOfSystem != NULL && pOwnerOfSystem->GetRaceID() != pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetOwnerOfShip()
					&& pMajor->GetAgreement(pOwnerOfSystem->GetRaceID()) < FRIENDSHIP_AGREEMENT)
				{
					g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70+counter*35, 120, 30);
					s = CResourceManager::GetString("BTN_BLOCKADE_SYSTEM");
					m_ShipOrders[BLOCKADE_SYSTEM].SetRect(r.right-245,r.top+70+counter*35,r.right-125,r.top+100+counter*35);
					g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70+counter*35,120,30), &fontFormat, &fontBrush);
					counter++;
				}
			}
			// Flagschiffernennung, geht nur wenn es keine Flotte ist
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 1
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() != ASSIGN_FLAGSHIP
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetIsShipFlagShip() != TRUE)
			{
				g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70+counter*35, 120, 30);
				s = CResourceManager::GetString("BTN_ASSIGN_FLAGSHIP");
				m_ShipOrders[ASSIGN_FLAGSHIP].SetRect(r.right-245,r.top+70+counter*35,r.right-125,r.top+100+counter*35);
				g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70+counter*35,120,30), &fontFormat, &fontBrush);
				counter++;
			}
/*					// einem anderen Schiff folgen
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 1)
			{
				pDC->BitBlt(r.right-245,r.top+70+counter*35,120,30,&mdc,0,0,SRCCOPY);
				s = CResourceManager::GetString("BTN_FOLLOW_SHIP");
				m_ShipOrders[FOLLOW_SHIP].SetRect(r.right-245,r.top+70+counter*35,r.right-125,r.top+100+counter*35);
				pDC->DrawText(s,m_ShipOrders[FOLLOW_SHIP],DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				counter++;
			}
*/					
			// Kolonisierung (hier beachten wenn es eine Flotte ist, dort schauen ob auch jedes Schiff in
			// der Flotte auch kolonisieren kann)
			if (m_iTimeCounter > 3 && m_iWhichMainShipOrderButton == 2 &&
				pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() != COLONIZE &&
				// Ab hier check wegen Flotten, darum wirds lang
				((pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetColonizePoints() > 0)
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()),COLONIZE) == TRUE)))
			{
				// Wenn das System uns bzw. niemanden gehört können wir nur kolonisieren
				if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == ""
					|| pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetOwnerOfShip())
					for (int l = 0; l < pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetNumberOfPlanets(); l++)
						if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetPlanet(l)->GetTerraformed() == TRUE
							&& pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetPlanet(l)->GetCurrentHabitant() == 0.0f)
						{
							g.DrawImage(m_pShipOrderButton, r.right-245, r.top+140, 120, 30);
							s = CResourceManager::GetString("BTN_COLONIZE");
							m_ShipOrders[COLONIZE].SetRect(r.right-245,r.top+140,r.right-125,r.top+170);
							g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+140,120,30), &fontFormat, &fontBrush);
							counter++;
							break;
						}
			}
			// Terraforming (hier beachten wenn es eine Flotte ist, dort schauen ob auch jedes Schiff in
			// der Flotte auch terraformen kann)
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 2 &&
				//pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() != TERRAFORM &&
				// Ab hier check wegen Flotten, darum wirds lang
				((pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetColonizePoints() > 0)
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()),TERRAFORM) == TRUE)))

			{
				for (int l = 0; l < pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetNumberOfPlanets(); l++)
					if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetPlanet(l)->GetHabitable() == TRUE &&
						pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetPlanet(l)->GetTerraformed() == FALSE)
					{
						g.DrawImage(m_pShipOrderButton, r.right-245, r.top+140-counter*35, 120, 30);
						s = CResourceManager::GetString("BTN_TERRAFORM");
						m_ShipOrders[TERRAFORM].SetRect(r.right-245,r.top+140-counter*35,r.right-125,r.top+170-counter*35);
						g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+140-counter*35,120,30), &fontFormat, &fontBrush);
						counter++;
						break;
					}
			}
			// Außenposten/Sternbasis bauen (hier beachten wenn es eine Flotte ist, dort schauen ob auch jedes
			// Schiff in der Flotte Stationen bauen kann)
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 2 &&
				pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() != BUILD_OUTPOST &&
				pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() != BUILD_STARBASE &&
				// Ab hier check wegen Flotten, darum wirds lang (müssen nur einen der Befehle (egal ob Outpost oder
				// Starbase gebaut werden soll) übergeben, weil wenn das eine geht, geht auch das andere
				((pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetStationBuildPoints() > 0)
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()),BUILD_OUTPOST) == TRUE)))
			{
				USHORT n = pDoc->GetCurrentShipIndex();
				CPoint ShipKO = pDoc->GetKO();
				// hier schauen, ob ich in der Schiffsinfoliste schon einen Außenposten habe den ich bauen kann, wenn in dem
				// Sector noch kein Außenposten steht und ob ich diesen in dem Sector überhaupt bauen kann. Das geht nur
				// wenn der Sektor mir oder niemanden gehört
				if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOutpost(pDoc->m_ShipArray.GetAt(n).GetOwnerOfShip()) == FALSE
					&& pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetStarbase(pDoc->m_ShipArray.GetAt(n).GetOwnerOfShip()) == FALSE
					&& (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == ""
					|| pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetOwnerOfShip()))
				{
					// Hier überprüfen, ob ich einen Außenposten technologisch überhaupt bauen kann
					for (int l = 0; l < pDoc->m_ShipInfoArray.GetSize(); l++)
						if (pDoc->m_ShipInfoArray.GetAt(l).GetRace() == pMajor->GetRaceShipNumber()
							&& pDoc->m_ShipInfoArray.GetAt(l).GetShipType() == OUTPOST
							&& pDoc->m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(researchLevels))
							{
								// Wenn ja dann Schaltfläche zum Außenpostenbau einblenden
								g.DrawImage(m_pShipOrderButton, r.right-245, r.top+140-counter*35, 120, 30);
								s = CResourceManager::GetString("BTN_BUILD_OUTPOST");
								m_ShipOrders[BUILD_OUTPOST].SetRect(r.right-245,r.top+140-counter*35,r.right-125,r.top+170-counter*35);
								g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+140-counter*35,120,30), &fontFormat, &fontBrush);
								counter++;
								break;
							}						
				}
				// Wenn hier schon ein Außenposten steht, können wir vielleicht auch eine Sternbasis bauen
				else if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOutpost(pDoc->m_ShipArray.GetAt(n).GetOwnerOfShip()) == TRUE
					&& pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetStarbase(pDoc->m_ShipArray.GetAt(n).GetOwnerOfShip()) == FALSE
					&& pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetOwnerOfShip())
				{
					// Hier überprüfen, ob ich eine Sternbasis technologisch überhaupt bauen kann
					for (int l = 0; l < pDoc->m_ShipInfoArray.GetSize(); l++)
						if (pDoc->m_ShipInfoArray.GetAt(l).GetRace() == pMajor->GetRaceShipNumber()
							&& pDoc->m_ShipInfoArray.GetAt(l).GetShipType() == STARBASE
							&& pDoc->m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(researchLevels))
							{
								// Wenn ja dann Schaltfläche zum Außenpostenbau einblenden
								g.DrawImage(m_pShipOrderButton, r.right-245, r.top+140-counter*35, 120, 30);
								s = CResourceManager::GetString("BTN_BUILD_STARBASE");
								m_ShipOrders[BUILD_STARBASE].SetRect(r.right-245,r.top+140-counter*35,r.right-125,r.top+170-counter*35);
								g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+140-counter*35,120,30), &fontFormat, &fontBrush);
								counter++;
								break;
							}						
				}
			}
			// Transport
			if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 2 &&
				pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetStorageRoom() > 0)
			{
				g.DrawImage(m_pShipOrderButton, r.right-245, r.top+140-counter*35, 120, 30);
				s = CResourceManager::GetString("BTN_TRANSPORT");
				m_ShipOrders[TRANSPORT].SetRect(r.right-245,r.top+140-counter*35,r.right-125,r.top+170-counter*35);
				g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+140-counter*35,120,30), &fontFormat, &fontBrush);
				counter++;
			}
		}
		// Schiff abwracken/zerstören
		if (m_iTimeCounter > (3 + counter) && m_iWhichMainShipOrderButton == 2 &&
			pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() != DESTROY_SHIP)
		{
			g.DrawImage(m_pShipOrderButton, r.right-245, r.top+140-counter*35, 120, 30);
			s = CResourceManager::GetString("BTN_DESTROY_SHIP");
			m_ShipOrders[DESTROY_SHIP].SetRect(r.right-245,r.top+140-counter*35,r.right-125,r.top+170-counter*35);
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+140-counter*35,120,30), &fontFormat, &fontBrush);
			counter++;
		}
		if (m_iTimeCounter >= (4 + counter))
		{
			this->KillTimer(1);
			m_iTimeCounter = 0;
		}				
	}
	
	// Wenn wir in dem Sektor gerade einen Außenposten bauen, dann prozentualen Fortschritt anzeigen.
	// Es kann auch passieren, das mehrere Rassen gleichzeitig dort einen Außenposten bauen, dann müssen wir
	// von jeder der Rasse den Fortschritt beim Stationsbau angeben
	if (m_bShowStation)	// Stationsansicht
	{
		BYTE count = 0;
		map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetIsStationBuilding(it->first) == TRUE)
			{
				CString station;
				if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOutpost(it->first) == FALSE)
					station = CResourceManager::GetString("OUTPOST");
				else
					station = CResourceManager::GetString("STARBASE");
				fontBrush.SetColor(Color(170,170,170));
				CString percent;
				percent.Format("%d",((pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetStartStationPoints(it->first)
					- pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetNeededStationPoints(it->first)) * 100
					/ pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetStartStationPoints(it->first)));
				
				CString sRaceName;
				if (pMajor == it->second || pMajor->IsRaceContacted(it->first))
					sRaceName = it->second->GetRaceName();
				else
					sRaceName = CResourceManager::GetString("UNKNOWN");
						
				s = station + CResourceManager::GetString("STATION_BUILDING", FALSE, sRaceName, percent);
				fontFormat.SetAlignment(StringAlignmentCenter);
				fontFormat.SetLineAlignment(StringAlignmentCenter);
				g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(250, 30+count*25), &fontFormat, &fontBrush);
				count++;
			}
	}

	// Wenn wir dem Schiff einen neuen Befehl geben, ohne das die Buttons über den Timer vollständig gezeichnet wurden
	// verschwindet das Schiff in der Anzeige, da der TimeCounter nicht wieder auf NULL gesetzt wird. Dies machen
	// wir hier manuell, falls dieser Fall eintritt
	if (m_iTimeCounter > 10)
	{
		KillTimer(1);
		m_iTimeCounter =0;
	}

	g.ReleaseHDC(pDC->GetSafeHdc());
}	

// CShipBottomView diagnostics

#ifdef _DEBUG
void CShipBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CShipBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CShipBottomView message handlers

void CShipBottomView::OnInitialUpdate()
{
	CBottomBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	m_LastKO = pDoc->GetKO();
	m_iPage = 1;
	m_iTimeCounter = 0;
	m_bShowNextButton = FALSE;
	for (int i = 0; i <= TRAIN_SHIP; i++)
		m_ShipOrders[i].SetRect(0,0,0,0);
	m_iWhichMainShipOrderButton = -1;
	
	CString sPrefix = pPlayer->GetPrefix();
	CString s = CIOData::GetInstance()->GetAppPath() + "Graphics\\Other\\" + sPrefix + "button_shiporder.bop";		
	m_pShipOrderButton = Bitmap::FromFile(s.AllocSysString());
}

BOOL CShipBottomView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}


void CShipBottomView::OnLButtonDown(UINT nFlags, CPoint point)
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

	if (m_RectForTheShip.PtInRect(point))
	{
		if (pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetOwnerOfShip() == pMajor->GetRaceID())
		{
			CSmallInfoView::SetShipInfo(true);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			// Wenn wir in der MainView nicht im Flottenmenü sind
			if (pDoc->GetMainFrame()->GetActiveView(0, 1) != FLEET_VIEW)
			{
				CGalaxyMenuView::SetMoveShip(TRUE);
				this->SetTimer(1,100,NULL);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CGalaxyMenuView));
			}
			// Wenn wir in der MainView im Flottenmenü sind, dann stecken wir das angeklickte Schiff in die
			// gerade angezeigte Flotte
			// Fremde Flotten können nicht bearbeitet werden
			else if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetOwnerOfShip() == pMajor->GetRaceID())
			{
				// Wenn das Schiff noch keine Flotte hat, dann müssen wir erstmal eine Flotte bilden
				if (pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet() == 0 && pDoc->GetNumberOfFleetShip() != pDoc->GetCurrentShipIndex())
					pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].CreateFleet();
				// Jetzt fügen wir der Flotte das angeklickte Schiff hinzu, wenn es nicht das Schiff selbst ist,
				// welches die Flotte anführt
				if (pDoc->GetNumberOfFleetShip() != pDoc->GetCurrentShipIndex())
				{
					// sicherheitshalber wird hier nochmal überprüft, dass keine Station hinzugefügt werden kann und
					// das sich das Schiff auch im gleichen Sektor befindet
					if (pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetKO() == pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetKO()
						&& pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetOwnerOfShip() == pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetOwnerOfShip()
						&& pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetShipType() != OUTPOST
						&& pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetShipType() != STARBASE)
					{							
						// Wenn das Schiff welches wir hinzufügen wollen selbst eine Flotte besizt, so müssen
						// wir diese Flotte natürlich auch noch hinzugügen
						if (pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetFleet() != 0 && pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetFleet()->GetFleetSize() > 0)
						{
							for (USHORT i = 0; i < pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetFleet()->GetFleetSize(); i++)
								pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->AddShipToFleet(pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetFleet()->GetShipFromFleet(i));
							// Jetzt haben wir die Schiffe auch noch hinzugefügt und können die Flotte nun löschen
							pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].DeleteFleet();
							// Zu allerletzt das ehemalige Flottenschiff hinzufügen, da es jetzt keine Flotte mehr besitzt
							pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->AddShipToFleet(&pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()]);
						}
						// besitzt das Schiff welches wir hinzufügen wollen keine Flotte, so können wir es direkt hinzufügen
						else
							pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->AddShipToFleet(&pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()]);
						// Wenn wir hier removen und ein Schiff im Feld entfernen, welches vor unserem FleetShip
						// ist, dann müssen wir die Nummer des FleetShips um eins verringern
						if (pDoc->GetCurrentShipIndex() < pDoc->GetNumberOfFleetShip())
							pDoc->SetNumberOfFleetShip(pDoc->GetNumberOfFleetShip()-1);
						// Wenn wir das Schiff da hinzugefügt haben, dann müssen wir das aus der normalen Schiffsliste
						// rausnehmen, damit es nicht zweimal im Spiel vorkommt
						pDoc->m_ShipArray.RemoveAt(pDoc->GetCurrentShipIndex());
						// Wenn wir das letzte Schiff entfernt haben, dann müssen wir die angeklickte Nummer im Spiel
						// um eins zurücknehmen
						if (pDoc->GetCurrentShipIndex() == pDoc->m_ShipArray.GetSize())
							pDoc->SetCurrentShipIndex(pDoc->GetCurrentShipIndex()-1);
						// wenn das angeklickte Schiff aus irgendeinen Grund nicht mehr uns gehören sollte, so wird das Flottenschiff ausegwählt
						if (pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetOwnerOfShip() != pMajor->GetRaceID())
							pDoc->SetCurrentShipIndex(pDoc->GetNumberOfFleetShip());

						Invalidate(FALSE);
						pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CFleetMenuView));						
						return;
					}
				}
			}				
		}
	}
	
	// Überprüfen ob wir auf den "Next"-Button geklickt haben, falls wir mehr als 9 Schiffe in dem Sektor haben
	CRect r;
	r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
	if (m_bShowNextButton == TRUE)
	{
		CRect next;
		next.SetRect(r.right-120,r.top+210,r.right,r.top+240);
		if (next.PtInRect(point))
		{
			CGalaxyMenuView::SetMoveShip(FALSE);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			m_iPage++;
			Invalidate(FALSE);
		}
	}
	if (m_iPage > 1)	// haben wir auf den "back-button" geklickt
	{
		CRect back;
		back.SetRect(r.right-120,r.top,r.right,r.top+30);
		if (back.PtInRect(point))
		{
			CGalaxyMenuView::SetMoveShip(FALSE);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			m_iPage--;
			Invalidate(FALSE);
		}
	}
	// Überprüfen, auf welchen Schiffsoberbefehle-Button ich geklickt habe
	if (CGalaxyMenuView::IsMoveShip() == TRUE)
	{
		CRect rect;
		// Taktik
		rect.SetRect(r.right-120,r.top+70,r.right,r.top+100);
		if (rect.PtInRect(point))
		{
			m_iWhichMainShipOrderButton = 0;
			m_iTimeCounter = 3;
			this->SetTimer(1,100,NULL);
		}
		// Befehl
		rect.SetRect(r.right-120,r.top+105,r.right,r.top+135);
		if (rect.PtInRect(point))
		{
			m_iWhichMainShipOrderButton = 1;
			m_iTimeCounter = 3;
			this->SetTimer(1,100,NULL);
		}
		// Aktion
		rect.SetRect(r.right-120,r.top+140,r.right,r.top+170);
		if (rect.PtInRect(point))
		{
			m_iWhichMainShipOrderButton = 2;
			m_iTimeCounter = 3;
			this->SetTimer(1,100,NULL);
		}
		// Ab jetzt die kleinen Buttons für die einzelnen genauen Schiffsbefehle
		network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pMajor->GetRaceID());
		for (int i = 0; i <= TRAIN_SHIP; i++)
			if (m_ShipOrders[i].PtInRect(point))
			{
				short nOldTerraformingPlanet = pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetTerraformingPlanet();
				// Bei manchen Befehlen müssen wir einen möglichen Zielkurs wieder zurücknehmen.				
				if (i != AVOID && i != ATTACK && i != CLOAK && i != ASSIGN_FLAGSHIP && i != CREATE_FLEET && i != TRANSPORT)
					pDoc->m_ShipArray.ElementAt(pDoc->GetCurrentShipIndex()).SetTargetKO(pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetKO(), 0);
				// Wenn wir eine Flotte bilden wollen (Schiffe gruppieren), dann in der MainView die Flottenansicht zeigen
				if (i == CREATE_FLEET)
				{
					pDoc->SetNumberOfFleetShip(pDoc->GetCurrentShipIndex());				// Dieses Schiff soll die Flotte beinhalten
					pDoc->GetMainFrame()->SelectMainView(FLEET_VIEW, pMajor->GetRaceID());		// Flottenansicht in der MainView anzeigen							
				}
				// wenn wir ein Schiff zum Flagschiff ernennen wollen müssen wir schauen das diesen Befehl kein anderes
				// Schiff des Imperiums hat, wenn ja dann diesen Befehl löschen
				else if (i == ASSIGN_FLAGSHIP)
				{
					// Das ganze Schiffsarray und auch die Flotten durchgehen, wenn wir ein altes Flagschiff finden, diesem den
					// Titel wegnehmen
					for (USHORT n = 0; n < pDoc->m_ShipArray.GetSize(); n++)
						if (pDoc->m_ShipArray[n].GetOwnerOfShip() == pDoc->m_ShipArray[pDoc->GetCurrentShipIndex()].GetOwnerOfShip())
						{
							if (pDoc->m_ShipArray[n].GetCurrentOrder() == ASSIGN_FLAGSHIP)
							{
								pDoc->m_ShipArray[n].SetCurrentOrder(ATTACK);
								break;
							}
							// überprüfen ob ein Flagschiff in einer Flotte ist
							else if (pDoc->m_ShipArray[n].GetFleet() != 0)
							{
								for (USHORT m = 0; m < pDoc->m_ShipArray[n].GetFleet()->GetFleetSize(); m++)
									if (pDoc->m_ShipArray[n].GetFleet()->GetShipFromFleet(m)->GetCurrentOrder() == ASSIGN_FLAGSHIP)
									{
										pDoc->m_ShipArray.ElementAt(n).GetFleet()->GetShipFromFleet(m)->SetCurrentOrder(ATTACK);
										break;
									}
							}
						}
						pDoc->m_ShipArray.ElementAt(pDoc->GetCurrentShipIndex()).SetCurrentOrder(ASSIGN_FLAGSHIP);
				}
				// Bei einem Transportbefehl muss in der MainView auch die Transportansicht angeblendet werden
				else if (i == TRANSPORT)
				{
					pDoc->GetMainFrame()->SelectMainView(10, pMajor->GetRaceID());	// Transportansicht in der MainView anzeigen
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CTransportMenuView));
				}
				// ansonsten ganz normal den Befehl geben
				else
					pDoc->m_ShipArray.ElementAt(pDoc->GetCurrentShipIndex()).SetCurrentOrder(i);
				
				// bei Terraforming wird die Planetenansicht eingeblendet
				if (i == TERRAFORM)
				{
					pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).SetTerraformingPlanet(nOldTerraformingPlanet);
					pDoc->GetMainFrame()->SelectBottomView(PLANET_BOTTOM_VIEW);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
					CSoundManager::GetInstance()->PlaySound(SNDMGR_MSG_TERRAFORM_SELECT, SNDMGR_PRIO_HIGH, 1.0f, client);
				}
				else
				{
					if (i == COLONIZE)
						CSoundManager::GetInstance()->PlaySound(SNDMGR_MSG_COLONIZING, SNDMGR_PRIO_HIGH, 1.0f, client);
					else if (i == BUILD_OUTPOST)
						CSoundManager::GetInstance()->PlaySound(SNDMGR_MSG_OUTPOST_CONSTRUCT, SNDMGR_PRIO_HIGH, 1.0f, client);
					else if (i == BUILD_STARBASE)
						CSoundManager::GetInstance()->PlaySound(SNDMGR_MSG_STARBASE_CONSTRUCT, SNDMGR_PRIO_HIGH, 1.0f, client);
					CGalaxyMenuView::SetMoveShip(FALSE);
					CSmallInfoView::SetShipInfo(true);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
				}
				Invalidate();
				break;
			}
	}

	CBottomBaseView::OnLButtonDown(nFlags, point);
}

void CShipBottomView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	if (pDoc->GetMainFrame()->GetActiveView(0, 1) == FLEET_VIEW)
		return;

	CalcLogicalPoint(point);
	
	if (m_RectForTheShip.PtInRect(point))
	{
		CMajor* pMajor = m_pPlayersRace;
		ASSERT(pMajor);
		if (!pMajor)
			return;

		CGalaxyMenuView::SetMoveShip(FALSE);
		this->KillTimer(1);
		m_iTimeCounter = 0;
		pDoc->SetNumberOfFleetShip(pDoc->GetCurrentShipIndex());				// Dieses Schiff soll die Flotte beinhalten
		pDoc->GetMainFrame()->SelectMainView(FLEET_VIEW, pMajor->GetRaceID());	// Flottenansicht in der MainView anzeigen				
	}	

	CBottomBaseView::OnLButtonDblClk(nFlags, point);
}

void CShipBottomView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (CGalaxyMenuView::IsMoveShip())
		return;
	
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CalcLogicalPoint(point);	

	// wurde die Maus über ein Schiff gehalten
	for (UINT i = 0; i < m_vShipRects.size(); i++)
	{
		if (m_vShipRects[i].first.PtInRect(point))
		{
			bool bNewMarkedShip = m_vShipRects[i].second != m_pMarkedShip;
			if (bNewMarkedShip)
			{
				for (int j = 0; j < pDoc->m_ShipArray.GetSize(); j++)
					if (&pDoc->m_ShipArray[j] == m_vShipRects[i].second)
					{
						pDoc->SetCurrentShipIndex(j);
						CSmallInfoView::SetShipInfo(true);
						pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
						m_iWhichMainShipOrderButton = -1;
						break;
					}

				m_RectForTheShip = m_vShipRects[i].first;
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

	CBottomBaseView::OnMouseMove(nFlags, point);
}

void CShipBottomView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);

	if (CGalaxyMenuView::IsMoveShip() == TRUE)
	{
		CGalaxyMenuView::SetMoveShip(FALSE);
		if (pDoc->GetMainFrame()->GetActiveView(1, 1) == PLANET_BOTTOM_VIEW)	// Wenn wir kolon oder terraformen abbrechen wollen, zurück zum Schiffsmenü
		{
			pDoc->m_ShipArray.ElementAt(pDoc->GetCurrentShipIndex()).SetCurrentOrder(AVOID);
			m_bShowStation = false;
			Invalidate();
		}
		else // wenn wir schon im Schiffsmenü sind, dann brauchen wir nur die Befehlsbutton neu zeichnen
		{
			CRect r(809,30,1069,208);
			CalcDeviceRect(r);
			//InvalidateRect(r, FALSE);
			Invalidate();
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CGalaxyMenuView));
		}
		CSmallInfoView::SetShipInfo(true);
		pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
	}

	CBottomBaseView::OnRButtonDown(nFlags, point);
}


void CShipBottomView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_iTimeCounter++;
	CRect r(830,20,m_TotalSize.cx, m_TotalSize.cy);
	CalcDeviceRect(r);
	InvalidateRect(r, FALSE);	

	CBottomBaseView::OnTimer(nIDEvent);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CShipBottomView::CreateTooltip(void)
{
	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return "";

	CShip* pShip = NULL;
	for (UINT i = 0; i < m_vShipRects.size(); i++)
		if (m_vShipRects[i].first.PtInRect(pt))
		{
			pShip = m_vShipRects[i].second;
			break;
		}
	
	if (!pShip)
		return "";	

	// ist der Besitzer des Schiffes unbekannt?
	if (pMajor->GetRaceID() != pShip->GetOwnerOfShip() && pMajor->IsRaceContacted(pShip->GetOwnerOfShip()) == false)
	{
		CString s = CResourceManager::GetString("UNKNOWN");
		s = CHTMLStringBuilder::GetHTMLColor(s);
		s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));		
		s = CHTMLStringBuilder::GetHTMLCenter(s);
		return s;
	}

	return pShip->GetTooltip();
}

/// Funktion ermittelt die Nummer des Schiffes im Array, über welches die Maus bewegt wurde.
/// @param pt Mauskoordinate
/// @return Nummer des Schiffes im Schiffsarray (<code>-1</code> wenn kein Schiff markiert wurde)
int CShipBottomView::GetMouseOverShip(CPoint& pt)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return -1;
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return -1;

	USHORT counter = 0;
	USHORT row = 0;
	USHORT column = 0;
	m_RectForTheShip.SetRect(0,0,0,0);
	
	if (CGalaxyMenuView::IsMoveShip() == FALSE)
		for (int i = 0; i < pDoc->m_ShipArray.GetSize(); i++)
			if ((pDoc->GetKO() == pDoc->m_ShipArray.GetAt(i).GetKO() && pDoc->m_ShipArray.GetAt(i).GetShipType() != OUTPOST && pDoc->m_ShipArray.GetAt(i).GetShipType() != STARBASE && !m_bShowStation)
				|| (pDoc->GetKO() == pDoc->m_ShipArray.GetAt(i).GetKO() && (pDoc->m_ShipArray.GetAt(i).GetShipType() == OUTPOST || pDoc->m_ShipArray.GetAt(i).GetShipType() == STARBASE) && m_bShowStation))
			{
				// Schiffe mit zu guter Stealthpower werden hier nicht angezeigt.
				// Schiffe mit zu guter Stealthpower werden hier nicht angezeigt.
				USHORT stealthPower = MAXBYTE;
				if (pDoc->m_ShipArray[i].GetFleet() == NULL)
				{					
					stealthPower = pDoc->m_ShipArray[i].GetStealthPower() * 20;
					if (pDoc->m_ShipArray[i].GetStealthPower() > 3 && pDoc->m_ShipArray[i].GetCloak() == FALSE)
						stealthPower = 3 * 20;
				}
				else
					stealthPower = pDoc->m_ShipArray[i].GetFleet()->GetFleetStealthPower(&pDoc->m_ShipArray[i]);

				if (pDoc->m_ShipArray[i].GetOwnerOfShip() != pMajor->GetRaceID() && pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetScanPower(pMajor->GetRaceID()) <= stealthPower)
					continue;
				if (counter < m_iPage*9 && counter >= (m_iPage-1)*9)
				{
					// mehrere Spalten anlegen, falls mehr als 3 Schiffe in dem System sind
					if (counter != 0 && counter%3 == 0)
					{
						column++;
						row = 0;
					}
					if (counter%9 == 0)
						column = 0;
					m_RectForTheShip.SetRect(column*250+37,row*65+30,column*250+187,row*65+80);
					if (m_RectForTheShip.PtInRect(pt))
						// Maus wurde über ein Schiff bewegt -> Nummer zurückgeben
						return i;
				}
				row++;
				counter++;
				if (counter > m_iPage*9)
					break;
			}

	// kein Schiff markiert
	return -1;
}