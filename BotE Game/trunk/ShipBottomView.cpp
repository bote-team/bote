// ShipBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "GalaxyMenuView.h"
#include "FleetMenuView.h"
#include "TransportMenuView.h"
#include "ShipBottomView.h"
#include "PlanetBottomView.h"
#include "SmallInfoView.h"
#include "RaceController.h"
#include "Fleet.h"

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
}

// CShipBottomView drawing

void CShipBottomView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = pDoc->GetPlayersRace();
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
	g.SetSmoothingMode(SmoothingModeHighSpeed);
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
	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	
	Gdiplus::Color normalColorCloaked(50, normalColor.GetR(), normalColor.GetG(), normalColor.GetB());
	Gdiplus::Color markColorCloaked(50, markColor.GetR(), markColor.GetG(), markColor.GetB());

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
	
	Bitmap* background = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "galaxyV3.png");
	if (background)
		g.DrawImage(background, 0, 0, 1075, 249);

	// Bis jetzt nur eine Anzeige bis max. 9 Schiffe
	if (m_iTimeCounter == 0)
	{
		for (int i = 0; i < pDoc->m_ShipArray.GetSize(); i++)
		{
			if ((pDoc->GetKO() == pDoc->m_ShipArray.GetAt(i).GetKO() && pDoc->m_ShipArray.GetAt(i).GetShipType() != OUTPOST && pDoc->m_ShipArray.GetAt(i).GetShipType() != STARBASE && !m_bShowStation)
				|| (pDoc->GetKO() == pDoc->m_ShipArray.GetAt(i).GetKO() && (pDoc->m_ShipArray.GetAt(i).GetShipType() == OUTPOST || pDoc->m_ShipArray.GetAt(i).GetShipType() == STARBASE) && m_bShowStation))
			{
				// Schiffe mit zu guter Stealthpower werden hier nicht angezeigt.
				USHORT stealthPower = pDoc->m_ShipArray[i].GetStealthPower() * 20;
				if (pDoc->m_ShipArray[i].GetStealthPower() > 3 && pDoc->m_ShipArray[i].GetCloak() == FALSE)
					stealthPower = 3 * 20;
				if (pDoc->m_ShipArray[i].GetOwnerOfShip() != pMajor->GetRaceID() && pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetScanPower(pMajor->GetRaceID()) <= stealthPower)
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
					// Wenn wir hier ein einzelnes Schiff anzeigen und keine Flotte
					if (pDoc->m_ShipArray.GetAt(i).GetFleet() == 0 || (pDoc->m_ShipArray.GetAt(i).GetFleet() != 0 && pDoc->m_ShipArray.GetAt(i).GetFleet()->GetFleetSize() == 0))
					{
						graphic = NULL;
						s.Format("Ships\\%s.png", pDoc->m_ShipArray.GetAt(i).GetShipClass());
						graphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
						if (graphic == NULL)
							graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.png");
						if (graphic)
							g.DrawImage(graphic, 250*column+37, row*65+30, 65, 49);							
						// Erfahrungsstufen des Schiffes anzeigen
						switch (pDoc->m_ShipArray.GetAt(i).GetExpLevel())
						{
						case 1: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\xp_beginner.png");	break;
						case 2: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\xp_normal.png");	break;
						case 3: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\xp_profi.png");		break;
						case 4: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\xp_veteran.png");	break;
						case 5: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\xp_elite.png");		break;
						case 6: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\xp_legend.png");	break;
						default: graphic = NULL;
						}
						if (graphic)
							g.DrawImage(graphic, 250*column + 29, row*65 +79 - graphic->GetHeight(), 8, graphic->GetHeight());
						
						// Hier die Striche für Schilde und Hülle neben dem Schiffsbild anzeigen (jeweils max. 20)
						USHORT hullProz = (UINT)(pDoc->m_ShipArray.GetAt(i).GetHull()->GetCurrentHull() * 20 / pDoc->m_ShipArray.GetAt(i).GetHull()->GetMaxHull());
						Gdiplus::Pen pen(Color(240-hullProz*12,0+hullProz*12,0));
						for (USHORT n = 0; n <= hullProz; n++)
							g.DrawRectangle(&pen, RectF(250*column+102,row*65+75-n*2,5,0.5));
							
						USHORT shieldProz = 0;
						if (pDoc->m_ShipArray.GetAt(i).GetShield()->GetMaxShield() > 0)
							shieldProz = (UINT)(pDoc->m_ShipArray.GetAt(i).GetShield()->GetCurrentShield() * 20 / pDoc->m_ShipArray.GetAt(i).GetShield()->GetMaxShield());
						pen.SetColor(Color(240-shieldProz*12,80,0+shieldProz*12));
						if (shieldProz > 0)
							for (USHORT n = 0; n <= shieldProz; n++)
								g.DrawRectangle(&pen, RectF(250*column+109,row*65+75-n*2,5,0.5));									

						// Wenn es das Flagschiff unseres Imperiums ist, dann kleines Zeichen zeichnen
						if (pDoc->m_ShipArray.GetAt(i).GetIsShipFlagShip() == TRUE)
						{
							fontBrush.SetColor(Color::White);
							g.DrawString(L"Flag", -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(250*column + 41, row*65 + 35), &fontFormat, &fontBrush);
						}
						// Wenn das Schiff getarnt ist ein die Schrift etwas dunkler darstellen
						if (pDoc->m_ShipArray.GetAt(i).GetCloak())
							fontBrush.SetColor(normalColorCloaked);
						else
							fontBrush.SetColor(normalColor);
						// Wenn wir ein Schiff markiert haben, dann Markierung zeichnen
						if (i == pDoc->GetNumberOfTheShipInArray())
						{
							if (pDoc->m_ShipArray.GetAt(i).GetCloak())
								fontBrush.SetColor(markColorCloaked);
							else
								fontBrush.SetColor(markColor);
						}
						s = pDoc->m_ShipArray.GetAt(i).GetShipName();
						g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(250*column+120,row*65+30), &fontFormat, &fontBrush);
						s = pDoc->m_ShipArray.GetAt(i).GetShipClass() + "-" + CResourceManager::GetString("CLASS");
						g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(250*column+120,row*65+50), &fontFormat, &fontBrush);
						// Wenn wir eine Station zeigen
						if (m_bShowStation)	
						{
							map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
							for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
								if (pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetOutpost(it->first) || pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetStarbase(it->first))
									s.Format("Other\\" + it->second->GetPrefix() + "Starbase.png");
							graphic = NULL;
							graphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
							if (graphic)
								g.DrawImage(graphic, 550, 20, 235, 200);
							break;
						}
					}
					// Wir haben eine Flotte
					else
					{
						if (pDoc->m_ShipArray.GetAt(i).GetFleet()->GetFleetShipType(&pDoc->m_ShipArray.GetAt(i)) != -1)
							s.Format("Ships\\%s.png",pDoc->m_ShipArray.GetAt(i).GetShipClass());
						else
							// Lade leeres Bild
							s.Format("Ships\\%s.png",pDoc->m_ShipArray.GetAt(i).GetShipClass());
						graphic = NULL;
						graphic = pDoc->GetGraphicPool()->GetGDIGraphic(s);
						if (graphic == NULL)
							graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.png");
						if (graphic)
							g.DrawImage(graphic, 250*column+37, row*65+30, 65, 49);
														
						// Anzahl der Schiffe in der Flotte (+1 weil das Führerschiff mitgezählt werden muß)
						fontBrush.SetColor(Color::White);
						s.Format("%d",pDoc->m_ShipArray.GetAt(i).GetFleet()->GetFleetSize()+1);
						g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(250*column+45,row*65+35), &fontFormat, &fontBrush);
						// Wenn das Schiff getarnt ist ein die Schrift etwas dunkler darstellen
						if (pDoc->m_ShipArray.GetAt(i).GetCloak())
							fontBrush.SetColor(normalColorCloaked);
						else
							fontBrush.SetColor(normalColor);									
						// Schiffsnamen holen und die ersten 4 Zeichen (z.B. USS_) und die lezten 2 Zeichen (z.B. _A) entfernen
						s.Format("%s",pDoc->m_ShipArray.GetAt(i).GetShipName());
						if (s.GetLength() > 4)
							s.Delete(0,4);
						if (s.GetLength() > 2 && s.ReverseFind(' ') == s.GetLength()-2)
							s.Delete(s.GetLength()-2,2);
						s.Append(" "+CResourceManager::GetString("GROUP"));
						// Wenn wir ein Schiff markiert haben, dann Markierung zeichnen
						if (i == pDoc->GetNumberOfTheShipInArray())
						{
							if (pDoc->m_ShipArray.GetAt(i).GetCloak())
								fontBrush.SetColor(markColorCloaked);
							else
								fontBrush.SetColor(markColor);
						}
						// Hier jetzt Namen und Schiffstype zur Flotte
						g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(250*column+120,row*65+30), &fontFormat, &fontBrush);
						if (pDoc->m_ShipArray.GetAt(i).GetFleet()->GetFleetShipType(&pDoc->m_ShipArray.GetAt(i)) == -1)
							g.DrawString(CResourceManager::GetString("MIXED_FLEET").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(250*column+120,row*65+50), &fontFormat, &fontBrush);
						else
							g.DrawString(pDoc->m_ShipArray.GetAt(i).GetShipTypeAsString(TRUE).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(250*column+120,row*65+50), &fontFormat, &fontBrush);
					}
				}
				row++;
				counter++;
				oneShip = i;
			}
			if (counter > m_iPage*9)
				break;
		}
		// Wenn nur ein Schiff in dem System ist, so wird es automatisch ausgewählt
		if (counter == 1 && !m_bShowStation && pDoc->m_ShipArray[oneShip].GetCurrentOrder() <= ATTACK
			&& pDoc->m_ShipArray[oneShip].GetOwnerOfShip() == pMajor->GetRaceID())
		{
			this->SetTimer(1,100,NULL);
			pDoc->SetNumberOfTheShipInArray(oneShip);
			CGalaxyMenuView::SetMoveShip(TRUE);
			CSmallInfoView::SetShipInfo(true);
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			m_iWhichMainShipOrderButton = -1;
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CGalaxyMenuView));
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
		pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip() == pMajor->GetRaceID())
	{
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
			pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() == AVOID)
		{
			g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70, 120, 30);
			m_ShipOrders[ATTACK].SetRect(r.right-245,r.top+70,r.right-125,r.top+100);
			g.DrawString(CResourceManager::GetString("BTN_ATTACK").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70,120,30), &fontFormat, &fontBrush);
			counter++;
		}
		// meiden
		else if (m_iTimeCounter > 3 && m_iWhichMainShipOrderButton == 0 &&
			pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() > AVOID)
		{
			g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70, 120, 30);
			m_ShipOrders[AVOID].SetRect(r.right-245,r.top+70,r.right-125,r.top+100);
			g.DrawString(CResourceManager::GetString("BTN_AVOID").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.right-245,r.top+70,120,30), &fontFormat, &fontBrush);
			counter++;
		}
		// folgende Befehle gehen alle nur, wenn es keine Station ist
		if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetShipType() != OUTPOST &&
			pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetShipType() != STARBASE)
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
					pDoc->GetSystem(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSystem() == pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip() &&
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
				((pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetStealthPower() > 3)
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()),CLOAK) == TRUE)))
			{
				g.DrawImage(m_pShipOrderButton, r.right-245, r.top+70+counter*35, 120, 30);
				if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCloak())
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
					pDoc->GetSystem(pDoc->GetKO().x, pDoc->GetKO().y).GetOwnerOfSystem() != pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip())
				{	
					CRace* pOwnerOfSector = pDoc->GetRaceCtrl()->GetRace(pDoc->GetSector(pDoc->GetKO().x, pDoc->GetKO().y).GetOwnerOfSector());

					// Wenn im System eine Rasse lebt und wir mit ihr im Krieg sind
					if (pOwnerOfSector != NULL && pMajor->GetAgreement(pOwnerOfSector->GetRaceID()) == WAR
					// Wenn das System niemanden mehr gehört, aber noch Bevölkerung drauf lebt (z.B. durch Rebellion)
						|| pDoc->GetSystem(pDoc->GetKO().x, pDoc->GetKO().y).GetOwnerOfSystem() == "" && pDoc->GetSector(pDoc->GetKO().x, pDoc->GetKO().y).GetMinorRace() == FALSE)
					{
						// nur wenn die Schiffe ungetarnt sind können sie Bombardieren
						// Ab hier check wegen Flotten, darum wirds lang
						if ((pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0
							&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCloak() == FALSE)
							|| (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0
							&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()), ATTACK_SYSTEM) == TRUE))
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
				((pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).HasSpecial(BLOCKADESHIP))
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()), BLOCKADE_SYSTEM) == TRUE)))
			{
				// Überprüfen ob man eine Blockade im System überhaupt errichten kann
				// Wenn das System nicht der Rasse gehört, der auch das Schiff gehört
				CRace* pOwnerOfSystem = pDoc->GetRaceCtrl()->GetRace(pDoc->GetSystem(pDoc->GetKO().x, pDoc->GetKO().y).GetOwnerOfSystem());
				if (pOwnerOfSystem != NULL && pOwnerOfSystem->GetRaceID() != pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip()
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
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() != ASSIGN_FLAGSHIP
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetIsShipFlagShip() != TRUE)
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
				pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() != COLONIZE &&
				// Ab hier check wegen Flotten, darum wirds lang
				((pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetColonizePoints() > 0)
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()),COLONIZE) == TRUE)))
			{
				// Wenn das System uns bzw. niemanden gehört können wir nur kolonisieren
				if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == ""
					|| pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip())
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
				//pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() != TERRAFORM &&
				// Ab hier check wegen Flotten, darum wirds lang
				((pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetColonizePoints() > 0)
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()),TERRAFORM) == TRUE)))

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
				pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() != BUILD_OUTPOST &&
				pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() != BUILD_STARBASE &&
				// Ab hier check wegen Flotten, darum wirds lang (müssen nur einen der Befehle (egal ob Outpost oder
				// Starbase gebaut werden soll) übergeben, weil wenn das eine geht, geht auch das andere
				((pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() == 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetStationBuildPoints() > 0)
				|| (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet() != 0
				&& pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetFleet()->CheckOrder(&pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()),BUILD_OUTPOST) == TRUE)))
			{
				USHORT n = pDoc->GetNumberOfTheShipInArray();
				CPoint ShipKO = pDoc->GetKO();
				// hier schauen, ob ich in der Schiffsinfoliste schon einen Außenposten habe den ich bauen kann, wenn in dem
				// Sector noch kein Außenposten steht und ob ich diesen in dem Sector überhaupt bauen kann. Das geht nur
				// wenn der Sektor mir oder niemanden gehört
				if (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOutpost(pDoc->m_ShipArray.GetAt(n).GetOwnerOfShip()) == FALSE
					&& pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetStarbase(pDoc->m_ShipArray.GetAt(n).GetOwnerOfShip()) == FALSE
					&& (pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == ""
					|| pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip()))
				{
					// Hier überprüfen, ob ich einen Außenposten technologisch überhaupt bauen kann
					for (int l = 0; l < pDoc->m_ShipInfoArray.GetSize(); l++)
						if (pDoc->m_ShipInfoArray.GetAt(l).GetRace() == pMajor->GetRaceShipNumber()
							&& pDoc->m_ShipInfoArray.GetAt(l).GetShipType() == OUTPOST
							&& pDoc->m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
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
					&& pDoc->GetSector(pDoc->GetKO().x,pDoc->GetKO().y).GetOwnerOfSector() == pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip())
				{
					// Hier überprüfen, ob ich eine Sternbasis technologisch überhaupt bauen kann
					for (int l = 0; l < pDoc->m_ShipInfoArray.GetSize(); l++)
						if (pDoc->m_ShipInfoArray.GetAt(l).GetRace() == pMajor->GetRaceShipNumber()
							&& pDoc->m_ShipInfoArray.GetAt(l).GetShipType() == STARBASE
							&& pDoc->m_ShipInfoArray.GetAt(l).IsThisShipBuildableNow(pMajor->GetEmpire()->GetResearch()))
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
				pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetStorageRoom() > 0)
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
			pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetCurrentOrder() != DESTROY_SHIP)
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
				s = station + CResourceManager::GetString("STATION_BUILDING", FALSE, it->second->GetRaceName(), percent);
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

	CMajor* pPlayer = pDoc->GetPlayersRace();
	ASSERT(pPlayer);

	m_LastKO = pDoc->GetKO();
	m_iPage = 1;
	m_iTimeCounter = 0;
	m_bShowNextButton = FALSE;
	for (int i = 0; i <= TRAIN_SHIP; i++)
		m_ShipOrders[i].SetRect(0,0,0,0);
	m_iWhichMainShipOrderButton = -1;
	
	CString sPrefix = pPlayer->GetPrefix();
	CString s = *((CBotf2App*)AfxGetApp())->GetPath() + "Graphics\\Other\\" + sPrefix + "button_small.png";		
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
	
	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

	if (m_RectForTheShip.PtInRect(point))
	{
		if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetOwnerOfShip() == pMajor->GetRaceID())
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
				//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			// Wenn wir in der MainView im Flottenmenü sind, dann stecken wir das angeklickte Schiff in die
			// gerade angezeigte Flotte
			// Fremde Flotten können nicht bearbeitet werden
			else if (pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfFleetShip()).GetOwnerOfShip() == pMajor->GetRaceID())
			{
				// Wenn das Schiff noch keine Flotte hat, dann müssen wir erstmal eine Flotte bilden
				if (pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet() == 0 && pDoc->GetNumberOfFleetShip() != pDoc->GetNumberOfTheShipInArray())
					pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].CreateFleet();
				// Jetzt fügen wir der Flotte das angeklickte Schiff hinzu, wenn es nicht das Schiff selbst ist,
				// welches die Flotte anführt
				if (pDoc->GetNumberOfFleetShip() != pDoc->GetNumberOfTheShipInArray())
				{
					// sicherheitshalber wird hier nochmal überprüft, dass keine Station hinzugefügt werden kann und
					// das sich das Schiff auch im gleichen Sektor befindet
					if (pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetKO() == pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetKO()
						&& pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetOwnerOfShip() == pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetOwnerOfShip()
						&& pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetShipType() != OUTPOST
						&& pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetShipType() != STARBASE)
					{							
						// Wenn das Schiff welches wir hinzufügen wollen selbst eine Flotte besizt, so müssen
						// wir diese Flotte natürlich auch noch hinzugügen
						if (pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetFleet() != 0 && pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetFleet()->GetFleetSize() > 0)
						{
							for (USHORT i = 0; i < pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetFleet()->GetFleetSize(); i++)
								pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->AddShipToFleet(pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetFleet()->GetShipFromFleet(i));
							// Jetzt haben wir die Schiffe auch noch hinzugefügt und können die Flotte nun löschen
							pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetFleet()->DeleteFleet();
							pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].DeleteFleet();
							// Zu allerletzt das ehemalige Flottenschiff hinzufügen, da es jetzt keine Flotte mehr besitzt
							pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->AddShipToFleet(pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()]);
						}
						// besitzt das Schiff welches wir hinzufügen wollen keine Flotte, so können wir es direkt hinzufügen
						else
							pDoc->m_ShipArray[pDoc->GetNumberOfFleetShip()].GetFleet()->AddShipToFleet(pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()]);
						// Wenn wir hier removen und ein Schiff im Feld entfernen, welches vor unserem FleetShip
						// ist, dann müssen wir die Nummer des FleetShips um eins verringern
						if (pDoc->GetNumberOfTheShipInArray() < pDoc->GetNumberOfFleetShip())
							pDoc->SetNumberOfFleetShip(pDoc->GetNumberOfFleetShip()-1);
						// Wenn wir das Schiff da hinzugefügt haben, dann müssen wir das aus der normalen Schiffsliste
						// rausnehmen, damit es nicht zweimal im Spiel vorkommt
						pDoc->m_ShipArray.RemoveAt(pDoc->GetNumberOfTheShipInArray());
						// Wenn wir das letzte Schiff entfernt haben, dann müssen wir die angeklickte Nummer im Spiel
						// um eins zurücknehmen
						if (pDoc->GetNumberOfTheShipInArray() == pDoc->m_ShipArray.GetSize())
							pDoc->SetNumberOfTheShipInArray(pDoc->GetNumberOfTheShipInArray()-1);
						Invalidate(FALSE);
						pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CFleetMenuView));
						//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep.wav", NULL, SND_FILENAME | SND_ASYNC);
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
				// Bei manchen Befehlen müssen wir einen möglichen Zielkurs wieder zurücknehmen
				if (i != AVOID && i != ATTACK && i != CLOAK && i != ASSIGN_FLAGSHIP && i != CREATE_FLEET && i != TRANSPORT)
					pDoc->m_ShipArray.ElementAt(pDoc->GetNumberOfTheShipInArray()).SetTargetKO(pDoc->m_ShipArray.GetAt(pDoc->GetNumberOfTheShipInArray()).GetKO(), 0);
				// Wenn wir eine Flotte bilden wollen (Schiffe gruppieren), dann in der MainView die Flottenansicht zeigen
				if (i == CREATE_FLEET)
				{
					pDoc->SetNumberOfFleetShip(pDoc->GetNumberOfTheShipInArray());				// Dieses Schiff soll die Flotte beinhalten
					pDoc->GetMainFrame()->SelectMainView(FLEET_VIEW, pMajor->GetRaceID());		// Flottenansicht in der MainView anzeigen							
				}
				// wenn wir ein Schiff zum Flagschiff ernennen wollen müssen wir schauen das diesen Befehl kein anderes
				// Schiff des Imperiums hat, wenn ja dann diesen Befehl löschen
				else if (i == ASSIGN_FLAGSHIP)
				{
					// Das ganze Schiffsarray und auch die Flotten durchgehen, wenn wir ein altes Flagschiff finden, diesem den
					// Titel wegnehmen
					for (USHORT n = 0; n < pDoc->m_ShipArray.GetSize(); n++)
						if (pDoc->m_ShipArray[n].GetOwnerOfShip() == pDoc->m_ShipArray[pDoc->GetNumberOfTheShipInArray()].GetOwnerOfShip())
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
									if (pDoc->m_ShipArray[n].GetFleet()->GetShipFromFleet(m).GetCurrentOrder() == ASSIGN_FLAGSHIP)
									{
										pDoc->m_ShipArray.ElementAt(n).GetFleet()->GetPointerOfShipFromFleet(m)->SetCurrentOrder(ATTACK);
										break;
									}
							}
						}
						pDoc->m_ShipArray.ElementAt(pDoc->GetNumberOfTheShipInArray()).SetCurrentOrder(ASSIGN_FLAGSHIP);
				}
				// Bei einem Transportbefehl muss in der MainView auch die Transportansicht angeblendet werden
				else if (i == TRANSPORT)
				{
					pDoc->GetMainFrame()->SelectMainView(10, pMajor->GetRaceID());	// Transportansicht in der MainView anzeigen
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CTransportMenuView));
				}
				// ansonsten ganz normal den Befehl geben
				else
					pDoc->m_ShipArray.ElementAt(pDoc->GetNumberOfTheShipInArray()).SetCurrentOrder(i);
				// bei Terraforming wird die Planetenansicht eingeblendet
				if (i == TERRAFORM)
				{
					pDoc->GetMainFrame()->SelectBottomView(PLANET_BOTTOM_VIEW);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
					pDoc->m_pSoundManager->PlaySound(SNDMGR_MSG_TERRAFORM_SELECT, SNDMGR_PRIO_HIGH, 1.0f, client);
				}
				else
				{
					if (i == COLONIZE)
						pDoc->m_pSoundManager->PlaySound(SNDMGR_MSG_COLONIZING, SNDMGR_PRIO_HIGH, 1.0f, client);
					else if (i == BUILD_OUTPOST)
						pDoc->m_pSoundManager->PlaySound(SNDMGR_MSG_OUTPOST_CONSTRUCT, SNDMGR_PRIO_HIGH, 1.0f, client);
					else if (i == BUILD_STARBASE)
						pDoc->m_pSoundManager->PlaySound(SNDMGR_MSG_STARBASE_CONSTRUCT, SNDMGR_PRIO_HIGH, 1.0f, client);
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

	CalcLogicalPoint(point);

	if (/*CGalaxyMenuView::IsMoveShip() && */m_RectForTheShip.PtInRect(point))
	{
		CMajor* pMajor = pDoc->GetPlayersRace();
		ASSERT(pMajor);
		if (!pMajor)
			return;

		CGalaxyMenuView::SetMoveShip(FALSE);
		this->KillTimer(1);
		m_iTimeCounter = 0;
		pDoc->SetNumberOfFleetShip(pDoc->GetNumberOfTheShipInArray());				// Dieses Schiff soll die Flotte beinhalten
		pDoc->GetMainFrame()->SelectMainView(FLEET_VIEW, pMajor->GetRaceID());		// Flottenansicht in der MainView anzeigen				
	}

	CBottomBaseView::OnLButtonDblClk(nFlags, point);
}

void CShipBottomView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = pDoc->GetPlayersRace();
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

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
				USHORT stealthPower = pDoc->m_ShipArray[i].GetStealthPower() * 20;
				if (pDoc->m_ShipArray[i].GetStealthPower() > 3 && pDoc->m_ShipArray[i].GetCloak() == FALSE)
					stealthPower = 3 * 20;
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
					if (m_RectForTheShip.PtInRect(point))
					{
						if (i != pDoc->GetNumberOfTheShipInArray())
						{
							pDoc->SetNumberOfTheShipInArray(i);
							CSmallInfoView::SetShipInfo(true);
							pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
							m_iWhichMainShipOrderButton = -1;
							Invalidate(FALSE);
						}
						break;
					}
				}
				row++;
				counter++;
				if (counter > m_iPage*9)
					break;
			}

	CBottomBaseView::OnMouseMove(nFlags, point);
}

void CShipBottomView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	CalcLogicalPoint(point);

	if (CGalaxyMenuView::IsMoveShip() == TRUE)
	{
		CGalaxyMenuView::SetMoveShip(FALSE);
		if (pDoc->GetMainFrame()->GetActiveView(1, 1) == PLANET_BOTTOM_VIEW)	// Wenn wir kolon oder terraformen abbrechen wollen, zurück zum Schiffsmenü
		{
			pDoc->m_ShipArray.ElementAt(pDoc->GetNumberOfTheShipInArray()).SetCurrentOrder(AVOID);
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