// DiplomacyMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "DiplomacyMenuView.h"
#include "DiplomacyBottomView.h"
#include "MenuChooseView.h"
#include "Races\GenDiploMessage.h"
#include "Graphic\memdc.h"
#include <algorithm>
#include "HTMLStringBuilder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMajor* pPlayer;//Böser globaler Zeiger der nötig ist um große Verrenkungen wegen sort und Memberfunktionen zu Vermeiden

/// Funktion zum Vergleichen zweier Rassen
bool CmpRaces(const CRace* pRace1, const CRace* pRace2)
{
	if(pRace1->GetType()==MAJOR&&pRace2->GetType()==MINOR) return true;
	if(pRace2->GetType()==MAJOR&&pRace1->GetType()==MINOR) return false;
	return pRace1->GetRaceName() < pRace2->GetRaceName();
}

//Soriert Rassen nach Vertrag
bool CmpRaces2(const CRace* pRace1, const CRace* pRace2)
{
	return pPlayer->GetAgreement(pRace1->GetRaceID())<pPlayer->GetAgreement(pRace2->GetRaceID());
}

// CDiplomacyMenuView
IMPLEMENT_DYNCREATE(CDiplomacyMenuView, CMainBaseView)

BEGIN_MESSAGE_MAP(CDiplomacyMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_XBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

CDiplomacyMenuView::CDiplomacyMenuView()
{
}

CDiplomacyMenuView::~CDiplomacyMenuView()
{
	for (int i = 0; i < m_DiplomacyMainButtons.GetSize(); i++)
	{
		delete m_DiplomacyMainButtons[i];
		m_DiplomacyMainButtons[i] = 0;
	}
	m_DiplomacyMainButtons.RemoveAll();
	for (int i = 0; i < m_DiplomacyMajorOfferButtons.GetSize(); i++)
	{
		delete m_DiplomacyMajorOfferButtons[i];
		m_DiplomacyMajorOfferButtons[i] = 0;
	}
	m_DiplomacyMajorOfferButtons.RemoveAll();
	for (int i = 0; i < m_DiplomacyMinorOfferButtons.GetSize(); i++)
	{
		delete m_DiplomacyMinorOfferButtons[i];
		m_DiplomacyMinorOfferButtons[i] = 0;
	}
}

void CDiplomacyMenuView::OnNewRound()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	m_OutgoingInfo.Reset();
	m_pIncomingInfo = NULL;
	m_sClickedOnRace = "";
	m_byWhichResourceIsChosen = TITAN;

	
	CString race = pPlayer->GetRaceID();
	
	CPoint ko = pDoc->GetRaceKO(race);
	if (ko != CPoint(-1,-1) && pDoc->GetSystem(ko).GetOwnerOfSystem() == race)
		m_ptResourceFromSystem = pDoc->GetRaceKO(race);
	// Systeme nochmal durchgehen um ein System zu finden, aus dem wir Rohstoffe verschenken könnten
	else if (pPlayer->GetEmpire()->GetSystemList()->GetSize() > 0)
		m_ptResourceFromSystem = pPlayer->GetEmpire()->GetSystemList()->GetAt(0).ko;
	m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;

	// anzuzeigende und bekannte Rassen ermitteln.
	vector<CMajor*> vMajors;
	vector<CMinor*> vMinors;
	// zuerst werden die Majors in alphabetischer Reihenfolge dargestellt, dann die Minors in alphabetischer Reihenfolge
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		if (pPlayer->IsRaceContacted(it->first) && it->second->GetEmpire()->GetNumberOfSystems() > 0)
			vMajors.push_back(it->second);
	std::sort(vMajors.begin(), vMajors.end(), CmpRaces);

	map<CString, CMinor*>* pmMinors = pDoc->GetRaceCtrl()->GetMinors();
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
		if (pPlayer->IsRaceContacted(it->first))
			vMinors.push_back(it->second);
	std::sort(vMinors.begin(), vMinors.end(), CmpRaces);

	m_vRaceList.clear();
	m_vRaceList.insert(m_vRaceList.end(), vMajors.begin(), vMajors.end());
	m_vRaceList.insert(m_vRaceList.end(), vMinors.begin(), vMinors.end());

	m_vIncomeList.clear();
	for (vector<CRace*>::const_iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
		for (UINT i = 0; i < pPlayer->GetIncomingDiplomacyNews()->size(); i++)
			if (pPlayer->GetIncomingDiplomacyNews()->at(i).m_sFromRace == (*it)->GetRaceID())
				m_vIncomeList.push_back(&pPlayer->GetIncomingDiplomacyNews()->at(i));	
}

// CDiplomacyMenuView drawing
void CDiplomacyMenuView::OnDraw(CDC* dc)
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

	// ***************************** DIE DIPLOMATIESANSICHT ZEICHNEN **********************************
	DrawDiplomacyMenue(&g);
	// Buttons am unteren Rand zeichnen und die Buttons in der jeweiligeb Diplomatieansicht zeichnen
	DrawDiplomacyButtons(&g, pMajor, &m_DiplomacyMainButtons, m_bySubMenu);
	// ************** DIE DIPLOMATIEANSICHT ZEICHNEN ist hier zu Ende **************

	g.ReleaseHDC(pDC->GetSafeHdc());
}

// CDiplomacyMenuView diagnostics

#ifdef _DEBUG
void CDiplomacyMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CDiplomacyMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDiplomacyMenuView message handlers

void CDiplomacyMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	// Diplomatieansicht
	m_bySubMenu = 0;
	m_bSortRaceList = false;

	// View bei den Tooltipps anmelden
	pDoc->GetMainFrame()->AddToTooltip(this);
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CDiplomacyMenuView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	// Alle Buttons in der View erstellen
	CreateButtons();
	
	// alle Hintergrundgrafiken laden
	CString sPrefix = pPlayer->GetPrefix();
	
	bg_diploinfomenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "diploinfomenu.boj");
	bg_diplooutmenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "diplooutmenu.boj");
	bg_diploinmenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "diploinmenu.boj");
}

void CDiplomacyMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CDiplomacyMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Diplomatiemenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CDiplomacyMenuView::DrawDiplomacyMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);
	
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
		
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Gdiplus::Color normalColor(Color::White);
	SolidBrush fontBrush(normalColor);


	if (m_bySubMenu == 0)
	{
		if (bg_diploinfomenu)
			g->DrawImage(bg_diploinfomenu, 0, 0, 1075, 750);
	}
	else if (m_bySubMenu == 1)
	{
		if (bg_diplooutmenu)
			g->DrawImage(bg_diplooutmenu, 0, 0, 1075, 750);		
	}
	else
	{
		if (bg_diploinmenu)
			g->DrawImage(bg_diploinmenu, 0, 0, 1075, 750);		
	}
	
	m_bShowSendButton = TRUE;
	m_bShowDeclineButton = TRUE;
	CString s;

	CDiplomacyBottomView::SetText("");
	this->DrawRaceDiplomacyMenue(g);
		
	// Wenn wir uns in der Angebotsanicht des Diplomatiemenüs befinden
	if (m_bySubMenu == 1 && m_sClickedOnRace != "")
	{
		CRace* pRace = pDoc->GetRaceCtrl()->GetRace(m_sClickedOnRace);
		if (!pRace)
			return;
		if (pRace->GetType() == MAJOR)
		{
			for (int i = 0; i < m_DiplomacyMajorOfferButtons.GetSize(); i++)
			{
				CMyButton *button = m_DiplomacyMajorOfferButtons.GetAt(i);
				// Handelsvertrag Button
				if (button->GetText() == CResourceManager::GetString("BTN_TRADECONTRACT"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < TRADE_AGREEMENT && m_OutgoingInfo.m_nType != TRADE_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_FRIENDSHIP"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < FRIENDSHIP_AGREEMENT && m_OutgoingInfo.m_nType != FRIENDSHIP_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_COOPERATION"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < COOPERATION && m_OutgoingInfo.m_nType != COOPERATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_AFFILIATION"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < AFFILIATION && m_OutgoingInfo.m_nType != AFFILIATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_NAP"))
				{
					if ((pPlayer->GetAgreement(m_sClickedOnRace) < FRIENDSHIP_AGREEMENT && pPlayer->GetAgreement(m_sClickedOnRace) != NON_AGGRESSION_PACT) && m_OutgoingInfo.m_nType != NON_AGGRESSION_PACT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_DEFENCE"))
				{
					if ((pPlayer->GetDefencePact(m_sClickedOnRace) != true && pPlayer->GetAgreement(m_sClickedOnRace) != WAR && pPlayer->GetAgreement(m_sClickedOnRace) != AFFILIATION) && m_OutgoingInfo.m_nType != DEFENCE_PACT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_REQUEST"))
				{
					if (m_OutgoingInfo.m_nType != DIP_REQUEST)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_WARPACT"))
				{
					// sobald wir noch eine andere Majorrace kennen, mit der wir nicht im Krieg sind, können wir
					// einen Kriegspakt anbieten
					if (pPlayer->GetAgreement(m_sClickedOnRace) != WAR && m_OutgoingInfo.m_nType != WAR_PACT)
					{
						map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
							if (it->first != pPlayer->GetRaceID() && it->first != m_sClickedOnRace && pRace->GetAgreement(it->first) != WAR && pPlayer->IsRaceContacted(it->first) && pRace->IsRaceContacted(it->first))
							{							
								if (button->GetState() != 1)
									button->SetState(0);
								break;
							}						
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_WAR"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) != WAR && m_OutgoingInfo.m_nType != WAR)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_PRESENT"))
				{
					if (m_OutgoingInfo.m_nType != PRESENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
			}
			DrawDiplomacyButtons(g, pPlayer, &m_DiplomacyMajorOfferButtons, -1);			
		}
		else if (pRace->GetType() == MINOR)
		{
			// Checken ob wir ein Angebot überhaupt machen können, z.B. wenn eine andere Hauptrasse
			// z.B. Mitgliedschaft mit einer Minorrace hat, dann können wir ihr kein Angebot machen, außer
			// Geschenke geben und Bestechen
			short status = NO_AGREEMENT;
			
			map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
				if (it->first != pPlayer->GetRaceID())
				{
					short temp = pRace->GetAgreement(it->first);
					if (temp > status)
						status = temp;
				}

			CMinor* pMinor = dynamic_cast<CMinor*>(pRace);
			if (!pMinor)
				return;
			
			for (int i = 0; i < m_DiplomacyMinorOfferButtons.GetSize(); i++)
			{
				CMyButton *button = m_DiplomacyMinorOfferButtons.GetAt(i);
				if (pMinor->GetSubjugated() == true)
				{
					button->SetState(2);
					continue;
				}
				// Handelsvertrag Button
				if (button->GetText() == CResourceManager::GetString("BTN_TRADECONTRACT"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < TRADE_AGREEMENT && status <= AFFILIATION && m_OutgoingInfo.m_nType != TRADE_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_FRIENDSHIP"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < FRIENDSHIP_AGREEMENT && status <= COOPERATION && m_OutgoingInfo.m_nType != FRIENDSHIP_AGREEMENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_COOPERATION"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < COOPERATION && status <= FRIENDSHIP_AGREEMENT && m_OutgoingInfo.m_nType != COOPERATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_AFFILIATION"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < AFFILIATION && status <= FRIENDSHIP_AGREEMENT && m_OutgoingInfo.m_nType != AFFILIATION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_MEMBERSHIP"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < MEMBERSHIP && status <= FRIENDSHIP_AGREEMENT && m_OutgoingInfo.m_nType != MEMBERSHIP)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_WAR"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) != WAR && status <= AFFILIATION && m_OutgoingInfo.m_nType != WAR)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
				else if (button->GetText() == CResourceManager::GetString("BTN_CORRUPTION"))
				{
					if ((status >= AFFILIATION || (pPlayer->GetAgreement(m_sClickedOnRace) >= FRIENDSHIP_AGREEMENT && status >= TRADE_AGREEMENT)) && m_OutgoingInfo.m_nType != CORRUPTION)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}				
				else if (button->GetText() == CResourceManager::GetString("BTN_PRESENT"))
				{
					if (m_OutgoingInfo.m_nType != PRESENT)
					{
						if (button->GetState() != 1)
							button->SetState(0);
					}
					else
						button->SetState(2);
				}
			}
			DrawDiplomacyButtons(g, pPlayer, &m_DiplomacyMinorOfferButtons, -1);			
		}

		// Abschicken- bzw. Abbrechenbutton anzeigen
		m_bCanSend = m_bShowSendButton;
		s = "";
		// Wenn wir ein gegebenes Angebot wieder abbrechen wollen
		if (!m_bShowSendButton)
			s = CResourceManager::GetString("BTN_CANCEL");
		else
		{
			// Wenn wir Geld geben wollen, den Abschickenbutton einblenden
			if (m_OutgoingInfo.m_nType == PRESENT)
			{
				if (m_OutgoingInfo.m_nCredits > 0 || m_OutgoingInfo.m_nResources[TITAN] > 0 || m_OutgoingInfo.m_nResources[DEUTERIUM] > 0 || m_OutgoingInfo.m_nResources[DURANIUM] > 0 || m_OutgoingInfo.m_nResources[CRYSTAL] > 0 || m_OutgoingInfo.m_nResources[IRIDIUM] > 0 || m_OutgoingInfo.m_nResources[DERITIUM] > 0)
					s = CResourceManager::GetString("BTN_SEND");
				else
					m_bCanSend = false;
			}
			// Wenn wir Freundschaft, Handelsvertrag usw. anbieten wollen, den Abschickenbutton anzeigen
			else if (m_OutgoingInfo.m_nType != NO_AGREEMENT && m_OutgoingInfo.m_nType != PRESENT && m_OutgoingInfo.m_nType != CORRUPTION && m_OutgoingInfo.m_nType != DIP_REQUEST && m_OutgoingInfo.m_nType != WAR_PACT)
			{
				s = CResourceManager::GetString("BTN_SEND");
			}
			// Wenn wir eine kleine Rasse bestechen wollen bzw. eine Forderung an die große Rasse stellen Abschickenbutton
			// einblenden. Bei Forderung muß ein Wert ausgewählt worden sein
			else if (m_OutgoingInfo.m_nType == PRESENT || m_OutgoingInfo.m_nType == CORRUPTION || m_OutgoingInfo.m_nType == DIP_REQUEST)
			{
				if (m_OutgoingInfo.m_nCredits > 0 || m_OutgoingInfo.m_nResources[TITAN] > 0 || m_OutgoingInfo.m_nResources[DEUTERIUM] > 0 || m_OutgoingInfo.m_nResources[DURANIUM] > 0 || m_OutgoingInfo.m_nResources[CRYSTAL] > 0 || m_OutgoingInfo.m_nResources[IRIDIUM] > 0 || m_OutgoingInfo.m_nResources[DERITIUM] > 0)
					s = CResourceManager::GetString("BTN_SEND");
				else
					m_bCanSend = false;
			}
			else if (m_OutgoingInfo.m_nType == WAR_PACT)
			{
				if (m_OutgoingInfo.m_sWarpactEnemy != "")
					s = CResourceManager::GetString("BTN_SEND");
				else
					m_bCanSend = false;
			}
		}					
		
		if (s != "")
		{
			// Rassenspezifische Schriftart auswählen
			CFontLoader::CreateGDIFont(pPlayer, 3, fontName, fontSize);
			// Schriftfarbe wählen
			Gdiplus::Color btnColor;
			CFontLoader::GetGDIFontColor(pPlayer, 2, btnColor);
			fontBrush.SetColor(btnColor);
			CString sFile = "Other\\" + pPlayer->GetPrefix() + "button.bop";
			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(sFile);
			if (graphic)
				g->DrawImage(graphic, 871,690,160,40);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(871,690,160,40), &fontFormat, &fontBrush);
		}
	}
	
	// Buttons in der Eingangsansicht zeichnen
	else if (m_bySubMenu == 2 && m_pIncomingInfo != NULL)
	{
		// Rassenspezifische Schriftart auswählen
		CFontLoader::CreateGDIFont(pPlayer, 3, fontName, fontSize);
		// Schriftfarbe wählen
		Gdiplus::Color btnColor;
		CFontLoader::GetGDIFontColor(pPlayer, 2, btnColor);
		fontBrush.SetColor(btnColor);
		CString sFile;
		if (m_bShowSendButton == TRUE)
			sFile = "Other\\" + pPlayer->GetPrefix() + "button.bop";
		else
			sFile = "Other\\" + pPlayer->GetPrefix() + "buttoni.bop";
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(sFile);
		if (graphic)
			g->DrawImage(graphic, 852,480,160,40);
		s = CResourceManager::GetString("BTN_ACCEPT");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(852,480,160,40), &fontFormat, &fontBrush);
		
		if (m_bShowDeclineButton == TRUE)
			sFile = "Other\\" + pPlayer->GetPrefix() + "button.bop";
		else
			sFile = "Other\\" + pPlayer->GetPrefix() + "buttoni.bop";
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic(sFile);
		if (graphic)
			g->DrawImage(graphic, 852,599,160,40);
		s = CResourceManager::GetString("BTN_DECLINE");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(852,599,160,40), &fontFormat, &fontBrush);
	}

	// "Diplomatieuebersicht" in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pPlayer, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pPlayer, 3, normalColor);
	fontBrush.SetColor(normalColor);
	if (m_bySubMenu == 0)
		s = CResourceManager::GetString("DIPLOMACY_MENUE_INFO");
	else if (m_bySubMenu == 1)
		s = CResourceManager::GetString("DIPLOMACY_MENUE_OFFERS");
	else if (m_bySubMenu == 2)
		s = CResourceManager::GetString("DIPLOMACY_MENUE_RECEIPTS");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);
	
	pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CDiplomacyBottomView));
}

void CDiplomacyMenuView::DrawRaceDiplomacyMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;

	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pPlayer, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pPlayer, 1, normalColor);
	SolidBrush fontBrush(normalColor);
	
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	CString s;
	int count = 0;

	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pPlayer->GetPrefix() + "button_small.bop");//SortButton zeichnen
	if(graphic) g->DrawImage(graphic, 22, 70, 80, 30);
	if(!m_bSortRaceList) s.Format("%s","A-Z");
	else s=CResourceManager::GetString("SORT2");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(22,70,80,30), &fontFormat, &fontBrush);

	CFontLoader::GetGDIFontColor(pPlayer, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pPlayer->GetDesign()->m_clrListMarkTextColor);

	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(pPlayer->GetDesign()->m_clrListMarkPenColor);

	

	// Position im Vector suchen
	int nVecPos = 0;
	if (m_bySubMenu != 2)
	{
		if (m_sClickedOnRace != "")
		{
			for (vector<CRace*>::const_iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
			{
				if ((*it)->GetRaceID() == m_sClickedOnRace)
					break;
				nVecPos++;
			}
		}
	}
	else
	{
		if (m_pIncomingInfo)
		{
			for (vector<CDiplomacyInfo*>::const_iterator it = m_vIncomeList.begin(); it != m_vIncomeList.end(); ++it)
			{
				if (*it == m_pIncomingInfo)
					break;
				nVecPos++;
			}
		}
	}

	// Wenn wir im Eingangsbildschirm sind
	if (m_bySubMenu != 2)
	{
		// alle bekannten Rassen anzeigen
		for (vector<CRace*>::iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
		{
			if (nVecPos-- > 21)
				continue;
			CRace* pRace = *it;
			if(m_bSortRaceList==false&&pRace->GetType()==MINOR)//Linie zwischen Minors und Majors
			{
				if(it-m_vRaceList.begin()>0&&m_vRaceList.at(it-m_vRaceList.begin()-1)->GetType()==MAJOR) g->DrawLine(&Gdiplus::Pen(penColor), 8, 100+count*25, 150, 100+count*25);
			}
			
			
			RectF rect(20,100+count*25,130,25);
			// Haben wir auf die Rasse geklickt haben, dann Farbe ändern und jeweiliges Untermenü aufrufen
			if (m_sClickedOnRace == pRace->GetRaceID())
			{
				// Wenn wir im Informationsbildschirm sind
				if (m_bySubMenu == 0)
					this->DrawDiplomacyInfoMenue(g, pRace->GetRaceID());
				// Wenn wir im Angebotsbildschirm sind
				else if (m_bySubMenu == 1)
					this->DrawDiplomacyOfferMenue(g, pRace->GetRaceID());
				
				
				// Name der Rasse zeichnen
				fontBrush.SetColor(markColor);
				g->DrawString(pRace->GetRaceName().AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
								
				// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
				g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(8,100+count*25,142,25));
				g->DrawLine(&Gdiplus::Pen(penColor), 8, 100+count*25, 150, 100+count*25);
				g->DrawLine(&Gdiplus::Pen(penColor), 8, 125+count*25, 150, 125+count*25);				
				
				// Bild der Rasse zeichnen
				CString name = pRace->GetGraphicFileName();
				Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\" + name);
				if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\ImageMissing.boj");		
				if (graphic)
					g->DrawImage(graphic, 735, 100, 300, 300);
				
				// Balken zeichnen, der angibt, wie gut die Rasse uns gegenübersteht, aber nur wenn der Computer sie spielt
				if (pRace->GetType() == MINOR || pRace->GetType() == MAJOR && ((CMajor*)pRace)->IsHumanPlayer() == false)
				{
					for (int t = 0; t < 20; t++)
					{
						RectF timber(650,387-t*15,30,13);
						if (pRace->GetRelation(pPlayer->GetRaceID()) * 2 / 10 > t)
						{
							fontBrush.SetColor(Color(250-t*12, 0+t*12, 0));
							g->FillRectangle(&SolidBrush(Color(200,250-t*12, 0+t*12, 0)), timber);
						}
						else
							g->FillRectangle(&SolidBrush(Color(100,100,100,100)), timber);						
					}
					if (pRace->GetRelation(pPlayer->GetRaceID()) * 2 / 10 == 0)
						fontBrush.SetColor(Color(255, 0, 0));
					// den Text zeichnen, der Angibt wie gut uns die Rasse gegenübersteht
					USHORT relation = pRace->GetRelation(pPlayer->GetRaceID());
					if (relation < 5) s = CResourceManager::GetString("HATEFUL");
					else if (relation < 15) s = CResourceManager::GetString("FURIOUS");
					else if (relation < 25) s = CResourceManager::GetString("HOSTILE");
					else if (relation < 35) s = CResourceManager::GetString("ANGRY");
					else if (relation < 45) s = CResourceManager::GetString("NOT_COOPERATIVE");
					else if (relation < 55) s = CResourceManager::GetString("NEUTRAL");
					else if (relation < 65) s = CResourceManager::GetString("COOPERATIVE");
					else if (relation < 75) s = CResourceManager::GetString("FRIENDLY");
					else if (relation < 85) s = CResourceManager::GetString("OPTIMISTIC");
					else if (relation < 95) s = CResourceManager::GetString("ENTHUSED");
					else s = CResourceManager::GetString("DEVOTED");
					
					fontFormat.SetAlignment(StringAlignmentCenter);
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(735,403,300,25), &fontFormat, &fontBrush);
					fontFormat.SetAlignment(StringAlignmentNear);
				}
			}
			else
			{	

				Gdiplus::Color color(normalColor);
				s = this->PrintDiplomacyStatus(pPlayer->GetRaceID(), pRace->GetRaceID(), color);
				fontBrush.SetColor(color);
				g->DrawString(pRace->GetRaceName().AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
			}
			fontBrush.SetColor(normalColor);			
			count++;
			if (count > 21)
				break;
		}
	}
	// Wenn wir im Eingangsbildschirm sind
	else
	{
		// alle Angebote durchgehen
		for (vector<CDiplomacyInfo*>::const_iterator it = m_vIncomeList.begin(); it != m_vIncomeList.end(); ++it)
		{
			if (nVecPos-- > 21)
				continue;

			// wenn noch kein Angebot angeklickt wurde, dann das erste Angebot auswählen
			if (m_pIncomingInfo == NULL)
				m_pIncomingInfo = *it;

			m_OutgoingInfo = *(*it);

			CRace* pRace = pDoc->GetRaceCtrl()->GetRace((*it)->m_sFromRace);
			ASSERT(pRace);

			RectF rect(20,100+count*25,130,25);
			// handelt es sich um das angeklickte Angebot
			if (m_pIncomingInfo == *it)
			{
				// Name der Rasse zeichnen
				fontBrush.SetColor(markColor);
				g->DrawString(pRace->GetRaceName().AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
								
				// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
				g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(8,100+count*25,142,25));
				g->DrawLine(&Gdiplus::Pen(penColor), 8, 100+count*25, 150, 100+count*25);
				g->DrawLine(&Gdiplus::Pen(penColor), 8, 125+count*25, 150, 125+count*25);				
				
				// Bild der Rasse zeichnen
				CString name = pRace->GetGraphicFileName();
				Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\" + name);
				if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\ImageMissing.boj");		
				if (graphic)
					g->DrawImage(graphic, 735, 100, 300, 300);
				
				// Balken zeichnen, der angibt, wie gut die Rasse uns gegenübersteht, aber nur wenn der Computer sie spielt
				if (pRace->GetType() == MINOR || pRace->GetType() == MAJOR && ((CMajor*)pRace)->IsHumanPlayer() == false)
				{
					for (int t = 0; t < 20; t++)
					{
						RectF timber(650,387-t*15,30,13);
						if (pRace->GetRelation(pPlayer->GetRaceID()) *2 / 10 > t)
						{
							fontBrush.SetColor(Color(250-t*12, 0+t*12, 0));
							g->FillRectangle(&SolidBrush(Color(200,250-t*12, 0+t*12, 0)), timber);
						}
						else
							g->FillRectangle(&SolidBrush(Color(100,100,100,100)), timber);						
					}
					// den Text zeichnen, der Angibt wie gut uns die Rasse gegenübersteht
					USHORT relation = pRace->GetRelation(pPlayer->GetRaceID());
					if (relation < 5) s = CResourceManager::GetString("HATEFUL");
					else if (relation < 15) s = CResourceManager::GetString("FURIOUS");
					else if (relation < 25) s = CResourceManager::GetString("HOSTILE");
					else if (relation < 35) s = CResourceManager::GetString("ANGRY");
					else if (relation < 45) s = CResourceManager::GetString("NOT_COOPERATIVE");
					else if (relation < 55) s = CResourceManager::GetString("NEUTRAL");
					else if (relation < 65) s = CResourceManager::GetString("COOPERATIVE");
					else if (relation < 75) s = CResourceManager::GetString("FRIENDLY");
					else if (relation < 85) s = CResourceManager::GetString("OPTIMISTIC");
					else if (relation < 95) s = CResourceManager::GetString("ENTHUSED");
					else s = CResourceManager::GetString("DEVOTED");
					
					fontFormat.SetAlignment(StringAlignmentCenter);
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(735,403,300,25), &fontFormat, &fontBrush);
					fontFormat.SetAlignment(StringAlignmentNear);
				}

				// Text vom Angebot anzeigen
				CDiplomacyBottomView::SetText(m_OutgoingInfo.m_sText);
				CDiplomacyBottomView::SetHeadLine(m_OutgoingInfo.m_sHeadline);

				// Handelt es sich um eine diplomatisches Angebot (keine Antwort, kein normaler Text)
				if (m_OutgoingInfo.m_nFlag == DIPLOMACY_OFFER)
				{
					// Haben wir auf einen Button geklickt, so muß dieser gedrückt dargestellt werden
					if (m_OutgoingInfo.m_nAnswerStatus == ACCEPTED)
						m_bShowSendButton = FALSE;
					else if (m_OutgoingInfo.m_nAnswerStatus == DECLINED)
						m_bShowDeclineButton = FALSE;
					
					// Bei Kriegserklärung oder Geschenk haben wir keine Wahlmöglichkeit
					if (m_OutgoingInfo.m_nType == WAR || m_OutgoingInfo.m_nType == PRESENT)
					{
						m_bShowSendButton = FALSE;
						m_bShowDeclineButton = FALSE;
					}
				}
				else if (m_OutgoingInfo.m_nFlag == DIPLOMACY_ANSWER)
				{
					m_bShowSendButton = FALSE;
					m_bShowDeclineButton = FALSE;
				}
			}
			// handelt es sich nicht um das angeklickte Angebot, so muss trotzdem deren Name angezeigt werden
			else
			{						
				g->DrawString(pRace->GetRaceName().AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
			}
									
			// Handelt es sich um eine diplomatisches Angebot (keine Antwort, kein normaler Text)
			// Typ zeichnen (Vorschlag, Forderung, Geschenk)
			rect.X = 200;
			rect.Width = 150;
			if (m_OutgoingInfo.m_nFlag == DIPLOMACY_OFFER)
			{							
				if (m_OutgoingInfo.m_nType == PRESENT)
					s = CResourceManager::GetString("PRESENT");
				else if (m_OutgoingInfo.m_nType == DIP_REQUEST)
				{
					// handelt es sich um das angeklickte Angebot
					if (m_pIncomingInfo == *it)
					{
						if (m_pIncomingInfo->m_nAnswerStatus == ACCEPTED)
							m_bShowSendButton = false;
						else
						{
							if (m_pIncomingInfo->m_ptKO != CPoint(-1,-1))
								m_ptResourceFromSystem = m_pIncomingInfo->m_ptKO;
							else if (m_ptResourceFromSystem != CPoint(-1,-1))
								m_pIncomingInfo->m_ptKO = m_ptResourceFromSystem;
							// Wenn eine Forderung gestellt wurde, welche Ressourcen beinhaltet, dann Annehmenbutton
							// nur einblenden, wenn wir diese Forderung auch erfüllen können
							USHORT *resource = m_pIncomingInfo->m_nResources;
							if (resource[TITAN] > 0 || resource[DEUTERIUM] > 0 || resource[DURANIUM] > 0 || resource[CRYSTAL] > 0 || resource[IRIDIUM] > 0 || resource[DERITIUM] > 0)
							{
								// Wenn Forderung, dann Systemauswahlbutton einblenden, wovon ich die geforderte Ressource
								// abzweigen will
								fontFormat.SetAlignment(StringAlignmentNear);
								fontBrush.SetColor(normalColor);
								s.Format("%s: ",CResourceManager::GetString("RESOURCE_FROM"));
								g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(350,100+count*25,150,25), &fontFormat, &fontBrush);
																
								Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pPlayer->GetPrefix() + "button_small.bop");
								Color btnColor;
								CFontLoader::GetGDIFontColor(pPlayer, 1, btnColor);
								SolidBrush btnBrush(btnColor);
								if (graphic)
									g->DrawImage(graphic, 500, 97+count*25, 120, 30);

								s.Format("%s",pDoc->GetSector(m_ptResourceFromSystem).GetName());
								// Wenn hier noch kein System eingestellt ist, dann müssen wir uns eins suchen
								if (s.IsEmpty() || pDoc->GetSystem(m_ptResourceFromSystem).GetOwnerOfSystem() != pPlayer->GetRaceID())
								{
									for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
										for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
											if (pDoc->GetSystem(x,y).GetOwnerOfSystem() == pPlayer->GetRaceID() && pDoc->GetSector(x,y).GetSunSystem() == TRUE)
											{
												m_ptResourceFromSystem = CPoint(x,y);
												m_pIncomingInfo->m_ptKO = m_ptResourceFromSystem;
												s.Format("%s",pDoc->GetSector(x,y).GetName());
												break;
											}
								}
								// Überprüfen ob wir auf dem gewählten System die Menge der geforderten
								// Ressource im Lager haben und ob wir auch die geforderten Credits bezahlen können
								for (int r = TITAN; r <= DERITIUM; r++)
									if (resource[r] > 0 && pDoc->GetSystem(m_ptResourceFromSystem).GetResourceStore(r) < resource[r] && pDoc->GetSystem(m_ptResourceFromSystem).GetOwnerOfSystem() == pPlayer->GetRaceID())
										m_bShowSendButton = false;
								fontFormat.SetAlignment(StringAlignmentCenter);
								g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(500,97+count*25,120,30), &fontFormat, &btnBrush);											
							}
							// Überprüfen ob wir auch die geforderten Credits bezahlen können
							if (pPlayer->GetEmpire()->GetCredits() < m_pIncomingInfo->m_nCredits)
								m_bShowSendButton = false;
						}
					}							
					s = CResourceManager::GetString("REQUEST");
				}
				else if (m_OutgoingInfo.m_nType == WAR)
					s = CResourceManager::GetString("WAR");
				else
					s = CResourceManager::GetString("SUGGESTION");
			}
			// handelt es sich um eine Antwort (also um kein Angebot und um kein Text)
			else if (m_OutgoingInfo.m_nFlag == DIPLOMACY_ANSWER)
				s = CResourceManager::GetString("ANSWER");

			fontBrush.SetColor(normalColor);
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
			count++;
			if (count > 21)
				break;
		}

		if (m_pIncomingInfo != NULL)
			m_OutgoingInfo = *m_pIncomingInfo;
	}
}

void CDiplomacyMenuView::DrawDiplomacyInfoMenue(Graphics* g, const CString& sWhichRace)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	CRace* pRace = pDoc->GetRaceCtrl()->GetRace(sWhichRace);
	if (!pRace)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pPlayer, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pPlayer, 3, normalColor);
	SolidBrush fontBrush(normalColor);
	
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pPlayer->GetDesign()->m_clrListMarkTextColor);	

	CString s;
	
	// Beschreibung der Rasse
	g->DrawString(pRace->GetRaceDesc().AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(190,475,845,190), &fontFormat, &fontBrush);
		
	// Ingame-Daten zu der Rasse, welche in der Box angezeigt werden
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontBrush.SetColor(markColor);
	RectF rect(215,110,260,25);
	s = CResourceManager::GetString("NAME").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	s = CResourceManager::GetString("HOMESYSTEM").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	s = CResourceManager::GetString("RELATIONSHIP").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 50;
	s = CResourceManager::GetString("KNOWN_EMPIRES").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
			
	// Informationen anzeigen
	rect.Y -= 100;
	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentFar);
	s = pRace->GetRaceName();
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	s = pRace->GetHomesystemName();
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)//Sector anzeigen
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if(pDoc->GetSector(x,y).GetName()==s&&pDoc->GetSector(x,y).GetKnown(pPlayer->GetRaceID()))
				{
					s.Format("%s (%c%i)", s,(char)y+97,x+1);
					break;
				}else if(pDoc->GetSector(x,y).GetName()==s&&!pDoc->GetSector(x,y).GetKnown(pPlayer->GetRaceID()))
				{	
					s.Format("%s (%s)",s,CResourceManager::GetString("UNKNOWN"));
					break;
				}
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	
	Gdiplus::Color color(normalColor);
	s = this->PrintDiplomacyStatus(pPlayer->GetRaceID(), sWhichRace, color);
	fontBrush.SetColor(color);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	
	if (pRace->GetType() == MINOR)
	{
		fontBrush.SetColor(markColor);
		fontFormat.SetAlignment(StringAlignmentNear);
		s = CResourceManager::GetString("ACCEPTANCE").MakeUpper()+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
		fontBrush.SetColor(normalColor);
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%d%%",(int)(((CMinor*)pRace)->GetAcceptancePoints(pPlayer->GetRaceID()) / 50));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}	
	// Wenn wir einen Verteidigungpakt mit der Rasse haben (nur bei Majors)
	else if (pRace->GetType() == MAJOR && pPlayer->GetDefencePact(sWhichRace) == true)
	{
		fontBrush.SetColor(Color(226,44,250));
		if (pPlayer->GetDefencePactDuration(sWhichRace) != 0)
			s.Format("%s (%d)", CResourceManager::GetString("DEFENCE_PACT"), pPlayer->GetDefencePactDuration(sWhichRace));
		else
			s = CResourceManager::GetString("DEFENCE_PACT");
		fontFormat.SetAlignment(StringAlignmentCenter);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}

	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentNear);
	rect.Y += 50;

	// bei Freundschaft wird der Status zu den anderen Rassen angezeigt
	if (pPlayer->GetAgreement(sWhichRace) >= FRIENDSHIP_AGREEMENT)
	{
		map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
			if (it->first != sWhichRace && it->first != pPlayer->GetRaceID())
				if (pRace->IsRaceContacted(it->first))
				{
					fontBrush.SetColor(normalColor);
					fontFormat.SetAlignment(StringAlignmentNear);
					s = it->second->GetRaceName();
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
						
					fontFormat.SetAlignment(StringAlignmentFar);
					Gdiplus::Color color(normalColor);
					s = this->PrintDiplomacyStatus(it->first, sWhichRace, color);
					fontBrush.SetColor(color);
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
					rect.Y += 25;
				}
	}	
	// sonst keine Angaben
	else
	{
		s = CResourceManager::GetString("NO_SPECS");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);		
	}
		
	// Hier die ganzen Buttons in der Informationsansicht zeichnen
	// Wenn wir einen Vertrag mit einer Minorrace haben so können wir diesen auch aufheben
	if (pRace->GetType() == MINOR)
	{
		// Angebote durchgehen, suchen ob wir schon eine Kündigung bei der Rasse gemacht haben
		for (UINT i = 0; i < pPlayer->GetOutgoingDiplomacyNews()->size(); i++)
		{
			m_OutgoingInfo = pPlayer->GetOutgoingDiplomacyNews()->at(i);
			if (m_OutgoingInfo.m_nType == NO_AGREEMENT && m_OutgoingInfo.m_sToRace == sWhichRace &&	m_OutgoingInfo.m_sFromRace == pPlayer->GetRaceID())
			{
				m_bShowSendButton = FALSE;
				CDiplomacyBottomView::SetHeadLine(m_OutgoingInfo.m_sHeadline);			
				CDiplomacyBottomView::SetText(m_OutgoingInfo.m_sText);
				break;
			}
		}

		if (pRace->GetAgreement(pPlayer->GetRaceID()) > NO_AGREEMENT)
		{
			m_OutgoingInfo.m_sHeadline = CResourceManager::GetString("CANCEL_AGREEMENT");
			Gdiplus::Color color;
			m_OutgoingInfo.m_sText = CResourceManager::GetString("CANCEL_AGREEMENT_TEXT", FALSE, this->PrintDiplomacyStatus(pPlayer->GetRaceID(), m_sClickedOnRace, color), pRace->GetRaceName());
			
			if (m_bShowSendButton == TRUE)
				s = CResourceManager::GetString("BTN_ANNUL");
			else
				s = CResourceManager::GetString("BTN_CANCEL");
			
			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pPlayer->GetPrefix() + "button_small.bop");
			Color btnColor;
			CFontLoader::GetGDIFontColor(pPlayer, 1, btnColor);
			SolidBrush btnBrush(btnColor);
			if (graphic)
				g->DrawImage(graphic, 275, 370, 120, 30);
			
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(275,370,120,30), &fontFormat, &btnBrush);
		}
	}
}

void CDiplomacyMenuView::DrawDiplomacyOfferMenue(Graphics* g, const CString& sWhichRace)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	CRace* pRace = pDoc->GetRaceCtrl()->GetRace(sWhichRace);
	if (!pRace)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pPlayer, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pPlayer, 3, normalColor);
	SolidBrush fontBrush(normalColor);
	
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pPlayer->GetDesign()->m_clrListMarkTextColor);

	CString s;
	
	s = CResourceManager::GetString("CHOOSE_OFFER")+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,100,360,25), &fontFormat, &fontBrush);
	
	// Abfragen, ob wir ein Angebot schon gemacht haben in dieser Runde
	// wenn ja, können wir keine weiteren Angebote machen!
	// Den jeweiligen Angebotsbildschirm dann auch einblenden
	for (UINT l = 0; l < pPlayer->GetOutgoingDiplomacyNews()->size(); l++)
	{
		CDiplomacyInfo* pInfo = &pPlayer->GetOutgoingDiplomacyNews()->at(l);
		if (pInfo->m_nSendRound == pDoc->GetCurrentRound() && pInfo->m_nFlag == DIPLOMACY_OFFER && pInfo->m_sToRace == sWhichRace && pInfo->m_sFromRace == pPlayer->GetRaceID())
		{
			m_OutgoingInfo = *pInfo;
			m_bShowSendButton = FALSE;
			break;
		}
	}
	
	// Wenn wir auf den Geschenkbutton, Freundschaftsbutton, Kooperationsbutton usw. gedrückt haben
	if (m_OutgoingInfo.m_nType != NO_AGREEMENT)
	{
		// Balken zeichnen, auf dem ich den Betrag auswählen kann, außer bei Kriegserklärung
		if (m_OutgoingInfo.m_nType != WAR)
		{	
			// ist aus irgendeinem Grund die übergebende Menge größer als die aktuellen Credits, so
			// werden die Credits runtergerechnet
			if (m_OutgoingInfo.m_nType != DIP_REQUEST && m_bShowSendButton && m_OutgoingInfo.m_nCredits > pPlayer->GetEmpire()->GetCredits())
			{
				m_OutgoingInfo.m_nCredits = max(0, pPlayer->GetEmpire()->GetCredits() / 250);
				m_OutgoingInfo.m_nCredits *= 250;
			}
			// Balken für Creditgeschenk zeichnen
			for (int t = 0; t < 20; t++)
			{
				RectF timber(195+t*12,440,10,25);
				if ((USHORT)(m_OutgoingInfo.m_nCredits / 250) > t)
					g->FillRectangle(&SolidBrush(Color(200-t*10,200,0)), timber);
				else
					g->FillRectangle(&SolidBrush(Color(100,100,100,100)), timber);
			}
			s.Format("%s: %d %s", CResourceManager::GetString("PAYMENT"), m_OutgoingInfo.m_nCredits, CResourceManager::GetString("CREDITS"));
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(195,410,255,30), &fontFormat, &fontBrush);

			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pPlayer->GetPrefix() + "button_small.bop");
			Color btnColor;
			CFontLoader::GetGDIFontColor(pPlayer, 1, btnColor);
			SolidBrush btnBrush(btnColor);

			// Ressourcengeschenke können nur gemacht werden, wenn wir mindst. einen Handelsvertrag haben oder diese Fordern
			if (pPlayer->GetAgreement(m_sClickedOnRace) >= TRADE_AGREEMENT || m_OutgoingInfo.m_nType == DIP_REQUEST)
			{
				// auch das Umschaltfeld auf die gegebene Ressource umstellen
				int iWhichResource = m_byWhichResourceIsChosen;
				
				// Balken für Ressourcengeschenk zeichnen
				CString res;
				if (iWhichResource == TITAN)
					res = CResourceManager::GetString("TITAN");
				else if (iWhichResource == DEUTERIUM)
					res = CResourceManager::GetString("DEUTERIUM");
				else if (iWhichResource == DURANIUM)
					res = CResourceManager::GetString("DURANIUM");
				else if (iWhichResource == CRYSTAL)
					res = CResourceManager::GetString("CRYSTAL");
				else if (iWhichResource == IRIDIUM)
					res = CResourceManager::GetString("IRIDIUM");
				else if (iWhichResource == DERITIUM)
					res = CResourceManager::GetString("DERITIUM");
				UINT nUnit = iWhichResource == DERITIUM ? 5 : 1000;
				
				// sind aus irgendeinem Grund die übergebende Menge größer als die aktuellen vorhandenen Ressourcen, so
				// werden die Ressourcen runtergerechnet
				if (m_OutgoingInfo.m_nType != DIP_REQUEST && m_bShowSendButton && m_OutgoingInfo.m_nResources[iWhichResource] > 0 && m_OutgoingInfo.m_ptKO != CPoint(-1,-1))
				{
					if (m_OutgoingInfo.m_nResources[iWhichResource] > pDoc->GetSystem(m_OutgoingInfo.m_ptKO).GetResourceStore(iWhichResource))
					{
						m_OutgoingInfo.m_nResources[iWhichResource] = pDoc->GetSystem(m_OutgoingInfo.m_ptKO).GetResourceStore(iWhichResource) / nUnit;
						m_OutgoingInfo.m_nResources[iWhichResource] *= nUnit;						
					}
				}
				
				for (int t = 0; t < 20; t++)
				{
					RectF timber(195+t*12,520,10,25);
					if ((USHORT)(m_OutgoingInfo.m_nResources[iWhichResource]/nUnit) > t)
						g->FillRectangle(&SolidBrush(Color(200-t*10,200,0)), timber);
					else
						g->FillRectangle(&SolidBrush(Color(100,100,100,100)), timber);					
				}
				s.Format("%s: %d %s %s",CResourceManager::GetString("TRANSFER"), m_OutgoingInfo.m_nResources[iWhichResource], CResourceManager::GetString("UNITS"),res);
				fontFormat.SetAlignment(StringAlignmentNear);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(195,490,355,30), &fontFormat, &fontBrush);

				// Wenn wir mit der Rasse mindst. einen Handelsvertrag haben, so zeigt er die ungefähre Menge des Rohstoffes an
				if (pRace->GetType() == MINOR)
				{
					CMinor* pMinor = dynamic_cast<CMinor*>(pRace);
					if (!pMinor)
						return;
					CPoint ko = pMinor->GetRaceKO();
					if (ko == CPoint(-1,-1))
						return;
					
					UINT nStorage = pDoc->GetSystem(ko).GetResourceStore(iWhichResource);

					if (nStorage <= 1 * nUnit)
						s = CResourceManager::GetString("SCARCELY_EXISTING");
					else if (nStorage <= 2 * nUnit)
						s = CResourceManager::GetString("VERY_LESS_EXISTING");
					else if (nStorage <= 4 * nUnit)
						s = CResourceManager::GetString("LESS_EXISTING");
					else if (nStorage <= 8 * nUnit)
						s = CResourceManager::GetString("MODERATE_EXISTING");
					else if (nStorage <= 16 * nUnit)
						s = CResourceManager::GetString("MUCH_EXISTING");
					else if (nStorage <= 32 * nUnit)
						s = CResourceManager::GetString("VERY_MUCH_EXISTING");
					else
						s = CResourceManager::GetString("ABOUNDING_EXISTING");
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(195,545,355,25), &fontFormat, &fontBrush);
				}
				
				// Den Button für die Ressourcenauswahl zeichnen
				if (graphic)
					g->DrawImage(graphic, 510, 518, 120, 30);
				fontFormat.SetAlignment(StringAlignmentCenter);
				g->DrawString(res.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,518,120,30), &fontFormat, &btnBrush);
				s = CResourceManager::GetString("RESOURCE");
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,490,120,30), &fontFormat, &fontBrush);
				
				// Den Button für die Systemauswahl der abzuknüpfenden Ressource zeichnen, nicht bei Forderung von Majorrace
				if (m_OutgoingInfo.m_nType != DIP_REQUEST)
				{
					if (graphic)
						g->DrawImage(graphic, 510, 605, 120, 30);
					s.Format("%s", pDoc->GetSector(m_OutgoingInfo.m_ptKO).GetName());
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,605,120,30), &fontFormat, &btnBrush);
					s = CResourceManager::GetString("FROM_SYSTEM");
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,577,120,30), &fontFormat, &fontBrush);
				}
			}
						
			// Wenn wir bestechen wollen, dann möglicherweise den Auswahlbutton wen wir bestechen wollen einblenden
			if (m_OutgoingInfo.m_nType == CORRUPTION)
			{
				if (m_OutgoingInfo.m_sCorruptedRace == "")
				{	
					map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
					// haben nicht auf den Button gedrückt -> mgl. Gegner auswählen
					if (pRace->GetAgreement(pPlayer->GetRaceID()) < FRIENDSHIP_AGREEMENT)
					{						
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
							if (it->first != pPlayer->GetRaceID() && pRace->GetAgreement(it->first) >= AFFILIATION)
							{
								m_OutgoingInfo.m_sCorruptedRace = it->first;
								break;
							}
					}
					else 
					{
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
							if (it->first != pPlayer->GetRaceID() && pRace->GetAgreement(it->first) >= TRADE_AGREEMENT)
							{
								m_OutgoingInfo.m_sCorruptedRace = it->first;
								break;
							}
					}
				}
				
				if (pRace->GetAgreement(pPlayer->GetRaceID()) >= FRIENDSHIP_AGREEMENT)
				{
					CMajor* pCorruptedMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_OutgoingInfo.m_sCorruptedRace));
					if (!pCorruptedMajor)
						return;
					s = pCorruptedMajor->GetRaceName();

					fontFormat.SetAlignment(StringAlignmentCenter);
					if (graphic)
						g->DrawImage(graphic, 510, 438, 120, 30);
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,438,120,30), &fontFormat, &btnBrush);
					s = CResourceManager::GetString("ENEMY");
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,410,120,30), &fontFormat, &fontBrush);
				}
			}
			
			// Wenn wir einen Vertrag einer Majorrace anbieten, dann Button für die Dauer dieses Vertrages einblenden
			else if (pRace->GetType() == MAJOR && m_OutgoingInfo.m_nType != PRESENT && m_OutgoingInfo.m_nType != DIP_REQUEST && m_OutgoingInfo.m_nType != WAR_PACT)
			{
				if (m_OutgoingInfo.m_nDuration == 0)
					s =CResourceManager::GetString("UNLIMITED");
				else
					s.Format("%d %s", m_OutgoingInfo.m_nDuration, CResourceManager::GetString("ROUNDS"));
				
				fontFormat.SetAlignment(StringAlignmentCenter);
				if (graphic)
					g->DrawImage(graphic, 510, 438, 120, 30);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,438,120,30), &fontFormat, &btnBrush);
				s = CResourceManager::GetString("CONTRACT_DURATION");
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,410,120,30), &fontFormat, &fontBrush);
			}

			// Wenn wir einen Kriegspakt anbieten wollen, dann Button zur Auswahl des Kriegsgegners mit anzeigen
			else if  (m_OutgoingInfo.m_nType == WAR_PACT)
			{
				// Wenn wir noch keinen Gegner ausgewählt haben
				if (m_OutgoingInfo.m_sWarpactEnemy == "")
				{
					map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
					for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
						if (it->first != pPlayer->GetRaceID() && it->first != m_sClickedOnRace && pPlayer->GetAgreement(m_sClickedOnRace) != WAR
							&& pPlayer->IsRaceContacted(it->first) && pRace->IsRaceContacted(it->first) && pRace->GetAgreement(it->first) != WAR)
						{
							m_OutgoingInfo.m_sWarpactEnemy = it->first;
							break;
						}
				}
				if (m_OutgoingInfo.m_sWarpactEnemy != "")
				{						
					CMajor* pWarpactEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_OutgoingInfo.m_sWarpactEnemy));
					if (!pWarpactEnemy)
						return;
					s = pWarpactEnemy->GetRaceName();						
					
					fontFormat.SetAlignment(StringAlignmentCenter);
					if (graphic)
						g->DrawImage(graphic, 510, 438, 120, 30);
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,438,120,30), &fontFormat, &btnBrush);
					s = CResourceManager::GetString("WARPACT_ENEMY");
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(510,410,120,30), &fontFormat, &fontBrush);					
				}
			}			
		}

		if (CGenDiploMessage::GenerateMajorOffer(m_OutgoingInfo))
			CDiplomacyBottomView::SetText(m_OutgoingInfo.m_sText);
		
		CDiplomacyBottomView::SetHeadLine(m_OutgoingInfo.m_sHeadline);
	}
}

/// Funktion zeichnet die Buttons in den Diplomatiemenüs.
/// @param g Zeiger auf GDI+ Grafikobjekt
/// @param pMajor Spielerrasse
void CDiplomacyMenuView::DrawDiplomacyButtons(Graphics* g, CMajor* pMajor, CArray<CMyButton*>* buttonArray, int counter)
{	
	CString fontName;
	REAL fontSize;
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush fontBrush(btnColor);
	DrawGDIButtons(g, buttonArray, counter, Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);
}

CString CDiplomacyMenuView::PrintDiplomacyStatus(const CString& sOurRace, const CString& sRace, Gdiplus::Color& color)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pOurRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sOurRace));
	if (!pOurRace)
		return "";

	CRace* pRace = pDoc->GetRaceCtrl()->GetRace(sRace);
	if (!pRace)
		return "";

	CString status;
	// wurde die Minorrace unterworfen?
	if (pRace->GetType() == MINOR && ((CMinor*)pRace)->GetSubjugated())
	{
		status = CResourceManager::GetString("SUBJUGATED");
		color.SetFromCOLORREF(RGB(178,0,255));
		return status;
	}	
	
	// Namen der Vertragsform und Farbe ermitteln
	short stat;	
	stat = pOurRace->GetAgreement(sRace);
	switch(stat)
	{
	case NO_AGREEMENT:
		return status = CResourceManager::GetString("NONE");
	case NON_AGGRESSION_PACT:
		{
			if (pRace->GetType() == MAJOR && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("NON_AGGRESSION_SHORT"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CResourceManager::GetString("NON_AGGRESSION_SHORT");
			color.SetFromCOLORREF(RGB(139,175,172));
			return status;
		}
	case TRADE_AGREEMENT:
		{
			if (pRace->GetType() == MAJOR && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("TRADE_AGREEMENT_SHORT"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CResourceManager::GetString("TRADE_AGREEMENT_SHORT");
			color.SetFromCOLORREF(RGB(233,183,12));
			return status;
		}
	case FRIENDSHIP_AGREEMENT:
		{			
			if (pRace->GetType() == MAJOR && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("FRIENDSHIP_SHORT"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CResourceManager::GetString("FRIENDSHIP_SHORT");
			color.SetFromCOLORREF(RGB(6,187,34));
			return status;
		}
	case COOPERATION:
		{
			if (pRace->GetType() == MAJOR && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("COOPERATION"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CResourceManager::GetString("COOPERATION");
			color.SetFromCOLORREF(RGB(37,159,250));
			return status;
		}
	case AFFILIATION:
		{
			if (pRace->GetType() == MAJOR && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CResourceManager::GetString("AFFILIATION"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CResourceManager::GetString("AFFILIATION");
			color.SetFromCOLORREF(RGB(29,29,248));
			return status;
		}
	case MEMBERSHIP:
		{
			color.SetFromCOLORREF(RGB(115,12,228));
			return status = CResourceManager::GetString("MEMBERSHIP");
		}
	case WAR:
		{
			color.SetFromCOLORREF(RGB(220,15,15));
			return status = CResourceManager::GetString("WAR");
		}
	default:
		return status = "";
	}
}

void CDiplomacyMenuView::TakeOrGetbackResLat(bool bTake)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	// bei einer Forderung an eine andere Majorrace werden keine Ressourcen aus den Lagern genommen, auch
	// werden bei einem Abbruch des diplomatischen Angebots keine Ressourcen wieder gut geschrieben.
	if (m_OutgoingInfo.m_nType == DIP_REQUEST && m_OutgoingInfo.m_sFromRace == pPlayer->GetRaceID())
		return;

	if (bTake)
	{
		// Credits von Creditbestand abziehen
		pPlayer->GetEmpire()->SetCredits(-m_OutgoingInfo.m_nCredits);
		m_OutgoingInfo.m_nCredits = 0;
		// Rohstoffe aus dem Lager nehmen
		for (int res = TITAN; res <= DERITIUM; res++)
		{
			if (m_OutgoingInfo.m_nResources[res] > 0)
			{
				CPoint ko = m_OutgoingInfo.m_ptKO;
				if (ko == CPoint(-1,-1))
					AfxMessageBox("Error in CDiplomacyView::TakeOrGetbackResLat(): KO has no value!");
				else
				{
					pDoc->GetSystem(ko).SetResourceStore(res, -m_OutgoingInfo.m_nResources[res]);
					m_OutgoingInfo.m_nResources[res] = 0;
				}
			}
		}
	}
	else
	{
		// Credits wieder zurück zum Imperium geben
		pPlayer->GetEmpire()->SetCredits(m_OutgoingInfo.m_nCredits);
		m_OutgoingInfo.m_nCredits = 0;
		// Die Rohstoffe wieder zurück in das System geben
		for (int res = TITAN; res <= DERITIUM; res++)
		{
			if (m_OutgoingInfo.m_nResources[res] > 0)
			{
				CPoint ko = m_OutgoingInfo.m_ptKO;
				if (ko == CPoint(-1,-1))
					AfxMessageBox("Error in CDiplomacyView::TakeOrGetbackResLat(): KO has no value!");
				else
				{
					pDoc->GetSystem(ko).SetResourceStore(res, m_OutgoingInfo.m_nResources[res]);
					m_OutgoingInfo.m_nResources[res] = 0;
				}
			}
		}
	}
}

void CDiplomacyMenuView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CDiplomacyMenuView::OnLButtonDown(nFlags, point);

	CMainBaseView::OnLButtonDblClk(nFlags, point);
}


void CDiplomacyMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	CalcLogicalPoint(point);

	// Wenn wir uns in der Diplomatieansicht befinden		
	// Überprüfen ob auf einen der Mainbuttons am unteren Rand der Diplomatieansicht geklickt wurde
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_DiplomacyMainButtons, temp, FALSE))
	{
		m_bySubMenu = temp;
		m_OutgoingInfo.Reset();
		m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
		
		m_pIncomingInfo = NULL;
		Invalidate();
		return;
	}
	
	// Position im Vector der aktuell angeklickten Rasse suchen
	int nVecPos = 0;
	// nur wenn nicht Eingangsbildschirm sind
	if (m_bySubMenu != 2)
	{
		if (m_sClickedOnRace != "")
		{
			for (vector<CRace*>::const_iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
			{
				if ((*it)->GetRaceID() == m_sClickedOnRace)
					break;
				nVecPos++;
			}
		}
	}
	else
	{
		if (m_pIncomingInfo)
		{
			for (vector<CDiplomacyInfo*>::const_iterator it = m_vIncomeList.begin(); it != m_vIncomeList.end(); ++it)
			{
				if (*it == m_pIncomingInfo)
					break;
				nVecPos++;
			}
		}
	}
	
	// Dem Imperium bekannte Rassen durchgehen
	CRect rect;
	int count = 0;
	// wenn wir nicht im Angebotseingangsbildschirm sind
	if (m_bySubMenu != 2)
	{
		for (vector<CRace*>::iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); it++)
		{
			CRace* pRace = *it;
			// wenn wir nicht im Angebotseingangsbildschirm sind
			if (m_bySubMenu != 2)
			{
				if (nVecPos-- > 21)
					continue;

				rect.SetRect(20,100+count*25,120,125+count*25);
				if (rect.PtInRect(point))
				{
					m_sClickedOnRace = pRace->GetRaceID();
					
					m_OutgoingInfo.Reset();
					m_OutgoingInfo.m_sFromRace = pPlayer->GetRaceID();
					m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
					
					m_pIncomingInfo = NULL;
					Invalidate();
					return;
				}
				count++;
				if (count > 21)
					break;
			}
		}
	}
	// Wenn wir im Angebotseingangsbildschirm sind
	else
	{	
		for (vector<CDiplomacyInfo*>::const_iterator it = m_vIncomeList.begin(); it != m_vIncomeList.end(); ++it)
		{
			if (nVecPos-- > 21)
				continue;

			rect.SetRect(20,100+count*25,620,125+count*25);
			if (rect.PtInRect(point))
			{
				m_pIncomingInfo = *it;

				// Systemauswahlbutton, mit dem wir das System wählen können, woraus wir die Ressourcen abzapfen, wenn wir
				// eine Forderung einer anderen Majorrasse erfüllen wollen, welche Ressourcen beinhaltet
				rect.SetRect(500,100+count*25,620,125+count*25);
				if (rect.PtInRect(point) && m_pIncomingInfo->m_nAnswerStatus != ACCEPTED)
				{
					// Nächstes System finden, wenn wir durchklicken
					if (m_pIncomingInfo->m_ptKO != CPoint(-1,-1))
						m_ptResourceFromSystem = m_pIncomingInfo->m_ptKO;
					CPoint curPoint = m_ptResourceFromSystem;
					int current = -1;
					for (int i = 0; i < pPlayer->GetEmpire()->GetSystemList()->GetSize(); i++)
						if (pPlayer->GetEmpire()->GetSystemList()->GetAt(i).ko == curPoint)
							current = i;
					if (current != -1)
						current++;
					if (current == pPlayer->GetEmpire()->GetSystemList()->GetSize())
						current = 0;
					m_ptResourceFromSystem = pPlayer->GetEmpire()->GetSystemList()->GetAt(current).ko;
					m_pIncomingInfo->m_ptKO = m_ptResourceFromSystem;
				}
				Invalidate();
				return;
			}
			count++;
			if (count > 21)
				break;
		}				
	}	

	rect.SetRect(22,70,102,100);
	if (rect.PtInRect(point))
	{
		m_bSortRaceList=!m_bSortRaceList;
		if(m_bSortRaceList) std::sort(m_vRaceList.begin(), m_vRaceList.end(),CmpRaces2);
		else std::sort(m_vRaceList.begin(), m_vRaceList.end(),CmpRaces);
		Invalidate();
		return;
	}
		
	// Wenn wir in der Informationsansicht sind und eine Rasse angklickt haben
	if (m_bySubMenu == 0 && m_sClickedOnRace != "")
	{
		CRace* pClickedRace = pDoc->GetRaceCtrl()->GetRace(m_sClickedOnRace);
		// schauen ob bei einer Minorrace auf den Kündigungsbutton geklickt wurde
		if (pClickedRace && pClickedRace->GetType() == MINOR && CRect(275,370,395,400).PtInRect(point) && pPlayer->GetAgreement(m_sClickedOnRace) > NO_AGREEMENT)
		{		
			// Kündigung hinzufügen
			if (m_bShowSendButton == TRUE)
			{
				m_OutgoingInfo.m_nFlag = DIPLOMACY_OFFER;
				m_OutgoingInfo.m_nSendRound = pDoc->GetCurrentRound();
				m_OutgoingInfo.m_nType = NO_AGREEMENT;
				m_OutgoingInfo.m_sFromRace = pPlayer->GetRaceID();
				m_OutgoingInfo.m_sToRace = m_sClickedOnRace;
				pPlayer->GetOutgoingDiplomacyNews()->push_back(m_OutgoingInfo);
			}
			// Kündigung wieder entfernen
			else
			{
				// Angebote durchgehen, suchen ob wir schon eine Kündigung bei der Rasse gemacht haben
				for (vector<CDiplomacyInfo>::iterator it = pPlayer->GetOutgoingDiplomacyNews()->begin(); it != pPlayer->GetOutgoingDiplomacyNews()->end(); it++)
				{
					m_OutgoingInfo = *it;
					if (m_OutgoingInfo.m_nType == NO_AGREEMENT && m_OutgoingInfo.m_sToRace == m_sClickedOnRace && m_OutgoingInfo.m_sFromRace == pPlayer->GetRaceID())
					{
						m_OutgoingInfo.Reset();
						m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
						pPlayer->GetOutgoingDiplomacyNews()->erase(it);
						break;
					}
				}				
			}
			CRect r(275,370,395,400);
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
	}
	
	// Wenn wir in der Angebotsansicht sind, überprüfen, auf welchen Angebotsbutton wir gedrückt haben
	else if (m_bySubMenu == 1 && m_sClickedOnRace != "")
	{
		CRace* pClickedRace = pDoc->GetRaceCtrl()->GetRace(m_sClickedOnRace);
		
		// Checken ob wir ein Angebot überhaupt machen können, z.B. wenn eine andere Hauptrasse
		// z.B. Mitgliedschaft mit einer Minorrace hat, dann können wir ihr kein Angebot machen, außer
		// Krieg erklären, Geschenke geben und Bestechen
		if (m_bShowSendButton == TRUE)
		{
			int nWhichOfferButtonIsPressed = INT_MIN;
			// überprüfen, auf welchen Angebotsbutton geklickt wurde
			if (pClickedRace->GetType() == MAJOR)
			{
				if (ButtonReactOnLeftClick(point, &m_DiplomacyMajorOfferButtons, nWhichOfferButtonIsPressed, FALSE, TRUE))
				{
					// altes Diplomatieobjekt löschen
					m_OutgoingInfo.Reset();
					m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;

					switch (nWhichOfferButtonIsPressed)
					{
						// Handelsvertragsangebot
						case 0:
							m_OutgoingInfo.m_nType = TRADE_AGREEMENT;
							break;
						// Freundschaftsangebot
						case 1:
							m_OutgoingInfo.m_nType = FRIENDSHIP_AGREEMENT;
							break;
						// Kooperationsangebot
						case 2:
							m_OutgoingInfo.m_nType = COOPERATION;
							break;
						// Bündnisangebot
						case 3:
							m_OutgoingInfo.m_nType = AFFILIATION;
							break;
						// Nichtangriffspaktangebot
						case 4:
							m_OutgoingInfo.m_nType = NON_AGGRESSION_PACT;
							break;
						// Kriegserklärung
						case 5:
							m_OutgoingInfo.m_nType = WAR;
							break;
						// Geschenk
						case 6:
							m_OutgoingInfo.m_nType = PRESENT;
							break;
						// Forderung
						case 7:
							m_OutgoingInfo.m_nType = DIP_REQUEST;
							break;
						// Verteidigungspaktangebot
						case 8:
							m_OutgoingInfo.m_nType = DEFENCE_PACT;
							break;
						// Kriegspakt
						case 9:
							m_OutgoingInfo.m_nType = WAR_PACT;
							break;
					}
					m_OutgoingInfo.m_sFromRace = pPlayer->GetRaceID();
					m_OutgoingInfo.m_sToRace = m_sClickedOnRace;
					Invalidate();
					return;
				}
			}
			else if (pClickedRace->GetType() == MINOR)
			{
				if (ButtonReactOnLeftClick(point, &m_DiplomacyMinorOfferButtons, nWhichOfferButtonIsPressed, FALSE, TRUE))
				{
					// altes Diplomatieobjekt löschen
					m_OutgoingInfo.Reset();
					m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;

					switch (nWhichOfferButtonIsPressed)
					{
						// Handelsvertragsangebot
						case 0:
							m_OutgoingInfo.m_nType = TRADE_AGREEMENT;
							break;
						// Freundschaftsangebot
						case 1:
							m_OutgoingInfo.m_nType = FRIENDSHIP_AGREEMENT;
							break;
						// Kooperationsangebot
						case 2:
							m_OutgoingInfo.m_nType = COOPERATION;
							break;
						// Bündnisangebot
						case 3:
							m_OutgoingInfo.m_nType = AFFILIATION;
							break;
						// Mitgliedschaftsangebot
						case 4:
							m_OutgoingInfo.m_nType = MEMBERSHIP;
							break;
						// Kriegserklärung
						case 5:
							m_OutgoingInfo.m_nType = WAR;
							break;
						// Geschenk
						case 6:
							m_OutgoingInfo.m_nType = PRESENT;
							break;
						// Bestechnung
						case 7:
							m_OutgoingInfo.m_nType = CORRUPTION;
							break;
					}
					m_OutgoingInfo.m_sFromRace = pPlayer->GetRaceID();
					m_OutgoingInfo.m_sToRace = m_sClickedOnRace;
					Invalidate();
					return;
				}
			}			
		}			
		// Sind wir in dem Diplomatiemenue für Geschenke geben, Freundschaft, Kooperation, Angliederung usw. anbieten und handelt es sich nicht um eine Kriegserkärung
		if (m_OutgoingInfo.m_nType != NO_AGREEMENT)
		{
			if (m_bShowSendButton == true && m_OutgoingInfo.m_nType != WAR)
			{
				// Balken zur Auswahl welche Menge wir verschenken
				CRect timber[21];
				for (int t = 0; t < 21; t++)
				{
					timber[t].SetRect(183+t*12,440,193+t*12+2,465);
					// Haben wir auf den Balken gedrückt, um den Wert des Geschenkes zu ändern
					if (timber[t].PtInRect(point))
					{
						m_OutgoingInfo.m_nCredits = t * 250;
						if (m_OutgoingInfo.m_nType != DIP_REQUEST)
							if (m_OutgoingInfo.m_nCredits > pPlayer->GetEmpire()->GetCredits())
							{
								m_OutgoingInfo.m_nCredits = max(0, pPlayer->GetEmpire()->GetCredits() / 250);
								m_OutgoingInfo.m_nCredits *= 250;								
							}
							Invalidate();
							break;
					}
					
					// nur wenn wir mindst. einen Handelsvertrag besitzen können wir Ressourcen verschenken oder bei einer Forderung stellen
					if (pPlayer->GetAgreement(m_sClickedOnRace) >= TRADE_AGREEMENT || m_OutgoingInfo.m_nType == DIP_REQUEST)					
					{
						timber[t].SetRect(183+t*12,520,193+t*12+2,545);
						
						// Haben wir auf den Balken gedrückt, um die Menge der zu verschenkenden bzw. zu fordernden Ressourcen zu ändern
						if (timber[t].PtInRect(point))
						{
							ASSERT(m_byWhichResourceIsChosen >= TITAN && m_byWhichResourceIsChosen <= DERITIUM);

							UINT nStorage = pDoc->GetSystem(m_OutgoingInfo.m_ptKO).GetResourceStore(m_byWhichResourceIsChosen);
							// bei normalen Ressourcen wird in 1000er Schritten gegeben, bei Deritium in 5er
							int dUnit = m_byWhichResourceIsChosen == DERITIUM ? 5 : 1000;
							m_OutgoingInfo.m_nResources[m_byWhichResourceIsChosen] = t * dUnit;
							// geben wir selbst die Ressource, dann benötigen wir genügend im Lager.
							if (m_OutgoingInfo.m_nType != DIP_REQUEST && m_OutgoingInfo.m_nResources[m_byWhichResourceIsChosen] > nStorage)
							{
								// Runden auf Runde Beträge
								m_OutgoingInfo.m_nResources[m_byWhichResourceIsChosen] = nStorage / dUnit;
								m_OutgoingInfo.m_nResources[m_byWhichResourceIsChosen] *= dUnit;
							}
							Invalidate();
							break;
						}
					}
				}
				
				// Wenn wir auf den kleinen Ressourcenbutton geklickt haben, nächste Ressource anzeigen
				rect.SetRect(510,518,630,548);
				if (rect.PtInRect(point))
				{
					if (m_byWhichResourceIsChosen < DERITIUM)
						m_byWhichResourceIsChosen++;
					else
						m_byWhichResourceIsChosen = TITAN;
					
					// alle "alten" Ressourcenmengen löschen
					for (int res = TITAN; res <= DERITIUM; res++)
						m_OutgoingInfo.m_nResources[res] = 0;
					Invalidate();
					return;
				}
				
				// Wenn wir auf den kleinen Systembutton geklickt haben um ein anderes System zu wählen
				rect.SetRect(510,605,630,635);
				if (rect.PtInRect(point) && m_OutgoingInfo.m_nType != DIP_REQUEST)
				{
					int current = -1;
					for (int i = 0; i < pPlayer->GetEmpire()->GetSystemList()->GetSize(); i++)
						if (pPlayer->GetEmpire()->GetSystemList()->GetAt(i).ko == m_OutgoingInfo.m_ptKO)
							current = i;
					if (current != -1)
					{
						current++;
						// Auch wieder alle Mengen auf Null zurücksetzen
						for (int res = TITAN; res <= DERITIUM; res++)
							m_OutgoingInfo.m_nResources[res] = 0;
					}
					if (current == pPlayer->GetEmpire()->GetSystemList()->GetSize())
						current = 0;
					m_OutgoingInfo.m_ptKO = pPlayer->GetEmpire()->GetSystemList()->GetAt(current).ko;
					Invalidate();
					return;
				}
				
				// Wenn wir auf den kleinen Button gedrückt haben um bei einer Bestechung den Gegner zu wählen
				rect.SetRect(510,438,630,468);
				if (rect.PtInRect(point) && m_OutgoingInfo.m_nType == CORRUPTION)
				{
					map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
					// nur wenn wir eine Freundschaft oder einen höherwertigen Vertrag mit der Minorrace haben können wir die Bestechungsrasse wählen
					if (pPlayer->GetAgreement(m_sClickedOnRace) >= FRIENDSHIP_AGREEMENT)
					{
						map<CString, CMajor*>::iterator it = pmMajors->find(m_OutgoingInfo.m_sCorruptedRace);
						if (it == pmMajors->end())
							it = pmMajors->begin();
						while (1)
						{
							it++;
							if (it == pmMajors->end())
								it = pmMajors->begin();

							if (it->first != pPlayer->GetRaceID() && pClickedRace->GetAgreement(it->first) >= TRADE_AGREEMENT)
							{
								m_OutgoingInfo.m_sCorruptedRace = it->first;
								CalcDeviceRect(rect);
								InvalidateRect(rect);
								break;
							}
							else if (it->first == m_OutgoingInfo.m_sCorruptedRace)
								break;
						}
					}
					// ansonsten können wir nur bestechen, wenn eine andere Rasse mindst. ein Bündnis mit der Minor hat
					else
					{
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
						{
							if (it->first != pPlayer->GetRaceID() && pClickedRace->GetAgreement(it->first) >= AFFILIATION)
							{
								m_OutgoingInfo.m_sCorruptedRace = it->first;
								CalcDeviceRect(rect);
								InvalidateRect(rect);
								break;
							}
						}
					}
				}
				// Wenn wir auf den kleinen Button geklickt haben um die Vertragsdauer zu ändern, geht nur bei Angebot an Majorrace
				else if (rect.PtInRect(point) && pClickedRace->GetType() == MAJOR && m_OutgoingInfo.m_nType != PRESENT && m_OutgoingInfo.m_nType != DIP_REQUEST && m_OutgoingInfo.m_nType != WAR_PACT)
				{
					m_OutgoingInfo.m_nDuration += 10;
					if (m_OutgoingInfo.m_nDuration > 100)
						m_OutgoingInfo.m_nDuration = 0;
					CalcDeviceRect(rect);
					InvalidateRect(rect);
					return;
				}
				
				// Wenn wir auf den kleinen Button geklickt haben um bei einem Kriegspakt den Kriegsgegner wählen zu wollen
				else if (rect.PtInRect(point) && m_OutgoingInfo.m_nType == WAR_PACT)
				{
					// Vektor mit zulässigen Kriegspaktgegnern füllen
					vector<CRace*> vEnemies;
					for (UINT i = 0; i < m_vRaceList.size(); i++)
					{
						// die Bedingungen müssen erfüllt sein
						if (m_vRaceList[i]->GetType() == MAJOR && m_sClickedOnRace != m_vRaceList[i]->GetRaceID() && pClickedRace->IsRaceContacted(m_vRaceList[i]->GetRaceID()) == true && pClickedRace->GetAgreement(m_vRaceList[i]->GetRaceID()) != WAR)
							vEnemies.push_back(m_vRaceList[i]);
					}
					// gibt es zulässige Kriegspaktgegner
					if (vEnemies.size())
					{
						// wenn noch kein Gegner gewählt wurde, dann den ersten im Feld auswählen (sollte so jedoch nicht vorkommen)
						if (m_OutgoingInfo.m_sWarpactEnemy == "")
							m_OutgoingInfo.m_sWarpactEnemy = vEnemies[0]->GetRaceID();
						else
						{
							// aktueller Kriegspaktgegner im Vektor suchen
							for (UINT i = 0; i < vEnemies.size(); i++)
								if (vEnemies[i]->GetRaceID() == m_OutgoingInfo.m_sWarpactEnemy)
								{
									if (i < (vEnemies.size() - 1))
										m_OutgoingInfo.m_sWarpactEnemy = vEnemies[i + 1]->GetRaceID();
									else
										m_OutgoingInfo.m_sWarpactEnemy = vEnemies[0]->GetRaceID();									
									break;
								}							
						}
						CalcDeviceRect(rect);
						InvalidateRect(rect);
						return;
					}
				}
			}
			
			// Wenn wir auf den Abschicken Button geklicked haben
			rect.SetRect(871,690,1031,730);
			if (rect.PtInRect(point) && m_bShowSendButton == true && m_bCanSend == true)
			{
				m_OutgoingInfo.m_nFlag = DIPLOMACY_OFFER;
				m_OutgoingInfo.m_nSendRound = pDoc->GetCurrentRound();				
				pPlayer->GetOutgoingDiplomacyNews()->push_back(m_OutgoingInfo);
				
				// angebotenes Credits und Ressourcen aus den Lagern nehmen
				this->TakeOrGetbackResLat(true);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				Invalidate();
				return;
			}
			
			// Wenn wir auf den Abbrechenbutton geklicked haben
			else if (rect.PtInRect(point) && m_bShowSendButton == false)
			{
				// angebotenes Credits und Ressourcen wieder zurück ins Lager geben
				this->TakeOrGetbackResLat(false);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				// Angebot aus dem Feld entfernen
				for (UINT i = 0; i < pPlayer->GetOutgoingDiplomacyNews()->size(); i++)
				{
					CDiplomacyInfo* pInfo = &pPlayer->GetOutgoingDiplomacyNews()->at(i);
					if (pInfo->m_nFlag == DIPLOMACY_OFFER && pInfo->m_sToRace == m_sClickedOnRace && pInfo->m_sFromRace == pPlayer->GetRaceID())
					{
						pPlayer->GetOutgoingDiplomacyNews()->erase(pPlayer->GetOutgoingDiplomacyNews()->begin() + i);
						break;
					}
				}
				Invalidate();
				return;
			}
		}
	}

	// Wenn wir in dem Eingangsbildschirm sind
	else if (m_bySubMenu == 2)
	{
		// Annehmenbutton
		rect.SetRect(852,480,1012,520);
		if (rect.PtInRect(point) && m_bShowSendButton == true)
		{
			m_pIncomingInfo->m_nAnswerStatus = ACCEPTED;
			// Wenn wir eine Forderung annehmen, dann sofort die Credits und Ressourcen aus den Lagern nehmen
			if (m_OutgoingInfo.m_nType == DIP_REQUEST)
			{
				this->TakeOrGetbackResLat(true);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
			Invalidate();
		}
		
		// Ablehnenbutton
		rect.SetRect(852,599,1012,639);
		if (rect.PtInRect(point) && m_bShowDeclineButton == TRUE)
		{
			// Wenn wir eine Forderung zuerst angenommen hatten und diese in der selben Runde doch ablehnen
			// wollen, so die geforderten Ressourcen und Credits wieder zurückgeben
			if (m_OutgoingInfo.m_nType == DIP_REQUEST && m_OutgoingInfo.m_nAnswerStatus == ACCEPTED)
			{
				this->TakeOrGetbackResLat(false);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
			m_pIncomingInfo->m_nAnswerStatus = DECLINED;			
			Invalidate();			
		}
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CDiplomacyMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);

	// Wenn wir uns in der Diplomatieansicht befinden
	ButtonReactOnMouseOver(point, &m_DiplomacyMainButtons);
	if (m_bySubMenu == 1)
		if (m_bShowSendButton && m_sClickedOnRace != "")
		{
			CRace* pRace = pDoc->GetRaceCtrl()->GetRace(m_sClickedOnRace);
			if (pRace)
			{
				if (pRace->GetType() == MAJOR)
					ButtonReactOnMouseOver(point, &m_DiplomacyMajorOfferButtons);
				else if (pRace->GetType() == MINOR)
					ButtonReactOnMouseOver(point, &m_DiplomacyMinorOfferButtons);
			}
		}

	CMainBaseView::OnMouseMove(nFlags, point);
}

BOOL CDiplomacyMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
	
	// wenn wir nicht im Eingangsmenü sind
	if (m_bySubMenu != 2)
	{
		if (m_sClickedOnRace != "")
		{
			for (vector<CRace*>::iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
			{
				if ((*it)->GetRaceID() == m_sClickedOnRace)
				{
					if (zDelta < 0)
					{
						it++;
					}
					else if (zDelta > 0 && it != m_vRaceList.begin())
					{
						it--;
					}
					if (it != m_vRaceList.end())
					{
						m_sClickedOnRace = (*it)->GetRaceID();
						
						m_OutgoingInfo.Reset();
						m_OutgoingInfo.m_sFromRace = m_pPlayersRace->GetRaceID();
						m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
				
						m_pIncomingInfo = NULL;
						Invalidate();
					}
					break;
				}
			}
		}
	}
	else
	{
		if (m_pIncomingInfo)
		{
			for (vector<CDiplomacyInfo*>::iterator it = m_vIncomeList.begin(); it != m_vIncomeList.end(); ++it)
			{
				if (*it == m_pIncomingInfo)
				{
					if (zDelta < 0)
					{
						it++;
					}
					else if (zDelta > 0 && it != m_vIncomeList.begin())
					{
						it--;
					}
					if (it != m_vIncomeList.end())
					{
						m_pIncomingInfo = *it;
						
						m_OutgoingInfo.Reset();
						m_OutgoingInfo.m_sFromRace = m_pPlayersRace->GetRaceID();
						m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
						Invalidate();
					}
					break;
				}
			}
		}
	}

	return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

void CDiplomacyMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	// wenn wir nicht im Eingangsmenü sind
	if (m_bySubMenu != 2)
	{	
		if (nChar == VK_ESCAPE)
		{
			m_sClickedOnRace = "";

			m_OutgoingInfo.Reset();
			m_OutgoingInfo.m_sFromRace = m_pPlayersRace->GetRaceID();
			m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;

			m_pIncomingInfo = NULL;
			Invalidate();
		}
		else if (nChar == VK_HOME)
		{
			vector<CRace*>::iterator it = m_vRaceList.begin();
			if (it != m_vRaceList.end())
			{
				m_sClickedOnRace = (*it)->GetRaceID();
				m_OutgoingInfo.Reset();
				m_OutgoingInfo.m_sFromRace = m_pPlayersRace->GetRaceID();
				m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
					
				m_pIncomingInfo = NULL;
				Invalidate();
			}
		}
		else if (nChar == VK_END)
		{
			vector<CRace*>::iterator it = m_vRaceList.end();
			if (m_vRaceList.size())
				it--;
			if (it != m_vRaceList.end())
			{			
				if (it != m_vRaceList.end())
				{
					m_sClickedOnRace = (*it)->GetRaceID();
					m_OutgoingInfo.Reset();
					m_OutgoingInfo.m_sFromRace = m_pPlayersRace->GetRaceID();
					m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
			
					m_pIncomingInfo = NULL;
					Invalidate();
				}
			}
		}
		else if (nChar == VK_UP || nChar == VK_DOWN)
		{
			if (m_sClickedOnRace != "")
			{
				for (vector<CRace*>::iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
				{
					if ((*it)->GetRaceID() == m_sClickedOnRace)
					{
						if (nChar == VK_DOWN)
						{
							it++;
						}
						else if (nChar == VK_UP && it != m_vRaceList.begin())
						{
							it--;
						}
						if (it != m_vRaceList.end())
						{
							m_sClickedOnRace = (*it)->GetRaceID();
							m_OutgoingInfo.Reset();
							m_OutgoingInfo.m_sFromRace = m_pPlayersRace->GetRaceID();
							m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
					
							m_pIncomingInfo = NULL;
							Invalidate();
						}
						break;
					}
				}
			}
		}
	}
	// wenn im Eingangsbildschirm
	else
	{
		if (nChar == VK_UP || nChar == VK_DOWN)
		{
			if (m_pIncomingInfo)
			{
				for (vector<CDiplomacyInfo*>::iterator it = m_vIncomeList.begin(); it != m_vIncomeList.end(); ++it)
				{
					if (*it == m_pIncomingInfo)
					{
						if (nChar == VK_DOWN)
						{
							it++;
						}
						else if (nChar == VK_UP && it != m_vIncomeList.begin())
						{
							it--;
						}
						if (it != m_vIncomeList.end())
						{
							m_pIncomingInfo = *it;
							m_OutgoingInfo.Reset();
							m_OutgoingInfo.m_sFromRace = m_pPlayersRace->GetRaceID();
							m_OutgoingInfo.m_ptKO = m_ptResourceFromSystem;
							Invalidate();
						}
						break;
					}
				}
			}
		}
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDiplomacyMenuView::CreateButtons()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	ASSERT(pPlayer);

	CString sPrefix = pPlayer->GetPrefix();

	// alle Buttons in der View anlegen und Grafiken laden	
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	// Buttons in den Diplomatieansichten
	m_DiplomacyMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_INFORMATION"), fileN, fileI, fileA));
	m_DiplomacyMainButtons.Add(new CMyButton(CPoint(200,690) , CSize(160,40), CResourceManager::GetString("OFFER"), fileN, fileI, fileA));
	m_DiplomacyMainButtons.Add(new CMyButton(CPoint(390,690) , CSize(160,40), CResourceManager::GetString("RECEIPT"), fileN, fileI, fileA));
	// Angebotsbuttons in der Diplomatieangebotsansicht
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,140) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,140) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,190) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,190) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,240) , CSize(160,40), CResourceManager::GetString("BTN_NAP"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,340) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,290) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,290) , CSize(160,40), CResourceManager::GetString("BTN_REQUEST"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,240) , CSize(160,40), CResourceManager::GetString("BTN_DEFENCE"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,340) , CSize(160,40), CResourceManager::GetString("BTN_WARPACT"), fileN, fileI, fileA));
		
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,165) , CSize(160,40), CResourceManager::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,165) , CSize(160,40), CResourceManager::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,215) , CSize(160,40), CResourceManager::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,215) , CSize(160,40), CResourceManager::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,265) , CSize(160,40), CResourceManager::GetString("BTN_MEMBERSHIP"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,265) , CSize(160,40), CResourceManager::GetString("BTN_WAR"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,315) , CSize(160,40), CResourceManager::GetString("BTN_PRESENT"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,315) , CSize(160,40), CResourceManager::GetString("BTN_CORRUPTION"), fileN, fileI, fileA));	
}
void CDiplomacyMenuView::OnXButtonDown(UINT nFlags, UINT nButton, CPoint point)
{
	// Dieses Feature erfordert mindestens Windows 2000.
	// Die Symbole _WIN32_WINNT und WINVER müssen >= 0x0500 sein.
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	
	CMainBaseView::OnXButtonDown(nFlags, nButton, point);
}

void CDiplomacyMenuView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	
	// Wenn wir in der Angebotsansicht sind, überprüfen, auf welchen Angebotsbutton wir gedrückt haben
	if (m_bySubMenu == 1 && m_sClickedOnRace != "")
	{
		CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
		ASSERT(pDoc);

		if (!pDoc->m_bDataReceived)
			return;

		CMajor* pPlayer = m_pPlayersRace;
		ASSERT(pPlayer);

		CalcLogicalPoint(point);

		CRace* pClickedRace = pDoc->GetRaceCtrl()->GetRace(m_sClickedOnRace);

		// Sind wir in dem Diplomatiemenue für Geschenke geben, Freundschaft, Kooperation, Angliederung usw. anbieten und handelt es sich nicht um eine Kriegserkärung
		if (m_OutgoingInfo.m_nType != NO_AGREEMENT)
		{
			if (m_bShowSendButton == true && m_OutgoingInfo.m_nType != WAR)
			{				
				// Wenn wir auf den kleinen Ressourcenbutton geklickt haben, nächste Ressource anzeigen
				CRect rect;
				rect.SetRect(510,518,630,548);
				if (rect.PtInRect(point))
				{
					if (m_byWhichResourceIsChosen > TITAN)
						m_byWhichResourceIsChosen--;
					else
						m_byWhichResourceIsChosen = DERITIUM;
					
					// alle "alten" Ressourcenmengen löschen
					for (int res = TITAN; res <= DERITIUM; res++)
						m_OutgoingInfo.m_nResources[res] = 0;
					Invalidate();
					return;
				}
				
				// Wenn wir auf den kleinen Systembutton geklickt haben um ein anderes System zu wählen
				rect.SetRect(510,605,630,635);
				if (rect.PtInRect(point) && m_OutgoingInfo.m_nType != DIP_REQUEST)
				{
					int current = -1;
					for (int i = 0; i < pPlayer->GetEmpire()->GetSystemList()->GetSize(); i++)
						if (pPlayer->GetEmpire()->GetSystemList()->GetAt(i).ko == m_OutgoingInfo.m_ptKO)
							current = i;
					if (current != -1)
					{
						current--;
						// Auch wieder alle Mengen auf Null zurücksetzen
						for (int res = TITAN; res <= DERITIUM; res++)
							m_OutgoingInfo.m_nResources[res] = 0;
					}
					if (current == -1)
						current = pPlayer->GetEmpire()->GetSystemList()->GetUpperBound();
					m_OutgoingInfo.m_ptKO = pPlayer->GetEmpire()->GetSystemList()->GetAt(current).ko;
					Invalidate();
					return;
				}
				
				// Wenn wir auf den kleinen Button geklickt haben um die Vertragsdauer zu ändern, geht nur bei Angebot an Majorrace
				rect.SetRect(510,438,630,468);				
				if (rect.PtInRect(point) && pClickedRace->GetType() == MAJOR && m_OutgoingInfo.m_nType != PRESENT && m_OutgoingInfo.m_nType != DIP_REQUEST && m_OutgoingInfo.m_nType != WAR_PACT)
				{
					m_OutgoingInfo.m_nDuration -= 10;
					if (m_OutgoingInfo.m_nDuration < 0)
						m_OutgoingInfo.m_nDuration = 100;
					CalcDeviceRect(rect);
					InvalidateRect(rect);
					return;
				}
			}
		}
	}

	CMainBaseView::OnRButtonDown(nFlags, point);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CDiplomacyMenuView::CreateTooltip(void)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);
	
	// Tooltips in der Angebots und Informationsansicht
	if (m_bySubMenu != 2)
	{
		// wurde auf das Rassenbild gehalten, dann Beschreibung der Rasse anzeigen
		if (CRect(735, 100, 735 + 300, 100 + 300).PtInRect(pt))
		{
			CRace* pRace = pDoc->GetRaceCtrl()->GetRace(m_sClickedOnRace);
			if (!pRace)
				return "";

			CString sTip = pRace->GetRaceDesc();
			sTip = CHTMLStringBuilder::GetHTMLColor(sTip);
			sTip = CHTMLStringBuilder::GetHTMLHeader(sTip, _T("h4"));

			return sTip;
		}

		if (pt.x >= 20 && pt.x <= 120)
		{
			// Position im Vector der aktuell angeklickten Rasse suchen
			int nVecPos = 0;
			if (m_sClickedOnRace != "")
			{
				for (vector<CRace*>::const_iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
				{
					if ((*it)->GetRaceID() == m_sClickedOnRace)
						break;
					nVecPos++;
				}
			}
		
			// Dem Imperium bekannte Rassen durchgehen
			int nCount = 0;
			for (vector<CRace*>::iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
			{
				if (nVecPos-- > 21)
					continue;

				if (CRect(20, 100 + nCount * 25, 120, 125 + nCount * 25).PtInRect(pt))
				{
					CRace* pRace = *it;
					if (!pRace)
						return "";

					return pRace->GetTooltip();					
				}

				nCount++;
				if (nCount > 21)
					break;
			}
		}
	}	
	// Tooltips in der Eingangsansicht
	else
	{
		// wurde auf das Rassenbild gehalten, dann Beschreibung der Rasse anzeigen
		if (m_pIncomingInfo && CRect(735, 100, 735 + 300, 100 + 300).PtInRect(pt))
		{
			CRace* pRace = pDoc->GetRaceCtrl()->GetRace(m_pIncomingInfo->m_sFromRace);
			if (!pRace)
				return "";

			CString sTip = pRace->GetRaceDesc();
			sTip = CHTMLStringBuilder::GetHTMLColor(sTip);
			sTip = CHTMLStringBuilder::GetHTMLHeader(sTip, _T("h4"));

			return sTip;
		}

		if (pt.x >= 20 && pt.x <= 120)
		{
			// Position im Vector der aktuell angeklickten Rasse suchen
			int nVecPos = 0;
			if (m_pIncomingInfo)
			{
				for (vector<CDiplomacyInfo*>::const_iterator it = m_vIncomeList.begin(); it != m_vIncomeList.end(); ++it)
				{
					if (*it == m_pIncomingInfo)
						break;
					nVecPos++;
				}
			}

			int nCount = 0;
			for (vector<CDiplomacyInfo*>::const_iterator it = m_vIncomeList.begin(); it != m_vIncomeList.end(); ++it)
			{
				if (nVecPos-- > 21)
					continue;

				if (CRect(20, 100 + nCount * 25, 620, 125 + nCount * 25).PtInRect(pt))
				{
					CString sRaceID = (*it)->m_sFromRace;
					CRace* pRace = pDoc->GetRaceCtrl()->GetRace(sRaceID);
					if (!pRace)
						return "";
					
					return pRace->GetTooltip();
				}
				
				nCount++;
				if (nCount > 21)
					break;
			}
		}
	}

	return "";
}