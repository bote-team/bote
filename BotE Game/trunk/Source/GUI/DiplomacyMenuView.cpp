// DiplomacyMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "BotE.h"
#include "MainFrm.h"
#include "DiplomacyMenuView.h"
#include "DiplomacyBottomView.h"
#include "MenuChooseView.h"
#include "Races\GenDiploMessage.h"
#include "Graphic\memdc.h"
#include <algorithm>
#include "HTMLStringBuilder.h"
#include "General/Loc.h"
#include "GraphicPool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMajor* pPlayer;//Böser globaler Zeiger der nötig ist um große Verrenkungen wegen sort und Memberfunktionen zu Vermeiden

/// Funktion zum Vergleichen zweier Rassen nach deren Namen (Zuerst Majors, dann Minors, dann sonstige)
bool CompareRaceName(const CRace* pRace1, const CRace* pRace2)
{
	if (pRace1->IsMajor() && !pRace2->IsMajor())
		return true;

	if (!pRace1->IsMajor() && pRace2->IsMajor())
		return false;

	if (!pRace1->IsAlien() && pRace2->IsAlien())
		return true;

	if (!pRace2->IsAlien() && pRace1->IsAlien())
		return false;

	return pRace1->GetRaceName() < pRace2->GetRaceName();
}

/// Funktion soriert Rassen nach Wertigkeit des Vertrages
bool ComareRaceAgreement(const CRace* pRace1, const CRace* pRace2)
{
	if (pRace1->IsMinor() && ((CMinor*)pRace1)->GetSubjugated() && pRace2->IsMinor() && ((CMinor*)pRace2)->GetSubjugated())
		return pRace1->GetRaceName() < pRace2->GetRaceName();

	if (pRace1->IsMinor() && ((CMinor*)pRace1)->GetSubjugated())
		return true;

	if (pRace2->IsMinor() && ((CMinor*)pRace2)->GetSubjugated())
		return false;

	if (pPlayer->GetAgreement(pRace1->GetRaceID()) == pPlayer->GetAgreement(pRace2->GetRaceID()))
		return pRace1->GetRaceName() < pRace2->GetRaceName();

	return pPlayer->GetAgreement(pRace1->GetRaceID()) < pPlayer->GetAgreement(pRace2->GetRaceID());
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

CDiplomacyMenuView::CDiplomacyMenuView() :
	bg_diploinfomenu(),
	bg_diploinmenu(),
	bg_diplooutmenu(),
	m_bySubMenu(0),
	m_byWhichResourceIsChosen(RESOURCES::TITAN),
	m_bSortRaceList(false),
	m_bShowSendButton(true),
	m_bShowDeclineButton(true),
	m_bCanSend(true),
	m_pIncomingInfo()
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
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

	m_OutgoingInfo.Reset();
	m_pIncomingInfo = NULL;
	m_sClickedOnRace = "";
	m_byWhichResourceIsChosen = RESOURCES::TITAN;


	const CString& race = pPlayer->GetRaceID();

	const CPoint& ko = pPlayer->GetRaceKO();
	if (ko != CPoint(-1,-1) && pDoc->GetSystem(ko.x, ko.y).OwnerID() == race)
		m_ptResourceFromSystem = pPlayer->GetRaceKO();
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
		if (pPlayer->IsRaceContacted(it->first) && it->second->GetEmpire()->CountSystems() > 0)
			vMajors.push_back(it->second);
	std::sort(vMajors.begin(), vMajors.end(), CompareRaceName);

	map<CString, CMinor*>* pmMinors = pDoc->GetRaceCtrl()->GetMinors();
	for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
		if (pPlayer->IsRaceContacted(it->first))
			vMinors.push_back(it->second);
	std::sort(vMinors.begin(), vMinors.end(), CompareRaceName);

	m_vRaceList.clear();
	m_vRaceList.insert(m_vRaceList.end(), vMajors.begin(), vMajors.end());
	m_vRaceList.insert(m_vRaceList.end(), vMinors.begin(), vMinors.end());

	m_vIncomeList.clear();
	for (vector<CRace*>::const_iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
		for (UINT i = 0; i < pPlayer->GetIncomingDiplomacyNews()->size(); i++)
			if (pPlayer->GetIncomingDiplomacyNews()->at(i).m_sFromRace == (*it)->GetRaceID())
				m_vIncomeList.push_back(&pPlayer->GetIncomingDiplomacyNews()->at(i));

	// Falls nach Vertrag sortiert werden soll, dann Liste jetzt sortieren
	if (m_bSortRaceList)
		std::sort(m_vRaceList.begin(), m_vRaceList.end(),ComareRaceAgreement);
}

// CDiplomacyMenuView drawing
void CDiplomacyMenuView::OnDraw(CDC* dc)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	AssertBotE(pMajor);
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

	// Diplomatieansicht
	m_bySubMenu = 0;
	m_bSortRaceList = false;

	// View bei den Tooltipps anmelden
	resources::pMainFrame->AddToTooltip(this);
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CDiplomacyMenuView::LoadRaceGraphics()
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

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

BOOL CDiplomacyMenuView::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Diplomatiemenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CDiplomacyMenuView::DrawDiplomacyMenue(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

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

		bool bAlienDiplomacy = false;
		if (pRace->HasSpecialAbility(SPECIAL_ALIEN_DIPLOMACY) || pPlayer->HasSpecialAbility(SPECIAL_ALIEN_DIPLOMACY))
			bAlienDiplomacy = true;

		if (pRace->IsMajor())
		{
			for (int i = 0; i < m_DiplomacyMajorOfferButtons.GetSize(); i++)
			{
				CMyButton *button = m_DiplomacyMajorOfferButtons.GetAt(i);
				// Handelsvertrag Button
				if (button->GetText() == CLoc::GetString("BTN_TRADECONTRACT"))
				{
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::TRADE && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::TRADE)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_FRIENDSHIP"))
				{
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::FRIENDSHIP && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_COOPERATION"))
				{
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::COOPERATION && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::COOPERATION)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_AFFILIATION"))
				{
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::AFFILIATION && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::AFFILIATION)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_NAP"))
				{
					if ((pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::FRIENDSHIP && pPlayer->GetAgreement(m_sClickedOnRace) != DIPLOMATIC_AGREEMENT::NAP) && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::NAP)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_DEFENCE"))
				{
					if (!bAlienDiplomacy && (pPlayer->GetDefencePact(m_sClickedOnRace) != true && pPlayer->GetAgreement(m_sClickedOnRace) != DIPLOMATIC_AGREEMENT::WAR && pPlayer->GetAgreement(m_sClickedOnRace) != DIPLOMATIC_AGREEMENT::AFFILIATION) && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::DEFENCEPACT)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_REQUEST"))
				{
					if (!bAlienDiplomacy && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_WARPACT"))
				{
					// sobald wir noch eine andere Majorrace kennen, mit der wir nicht im Krieg sind, können wir
					// einen Kriegspakt anbieten
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) != DIPLOMATIC_AGREEMENT::WAR && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WARPACT)
					{
						bool bFoundEnemy = false;
						map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
						{
							if (it->first != pPlayer->GetRaceID() && it->first != m_sClickedOnRace && pRace->GetAgreement(it->first) != DIPLOMATIC_AGREEMENT::WAR && pPlayer->IsRaceContacted(it->first) && pRace->IsRaceContacted(it->first))
							{
								bFoundEnemy = true;
								if (button->GetState() != BUTTON_STATE::ACTIVATED)
									button->SetState(BUTTON_STATE::NORMAL);
								break;
							}
						}
						if (!bFoundEnemy)
							button->SetState(BUTTON_STATE::DEACTIVATED);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_WAR"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) != DIPLOMATIC_AGREEMENT::WAR && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WAR)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_PRESENT"))
				{
					if (!bAlienDiplomacy && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::PRESENT)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
			}
			DrawDiplomacyButtons(g, pPlayer, &m_DiplomacyMajorOfferButtons, -1);
		}
		else if (pRace->IsMinor())
		{
			// Checken ob wir ein Angebot überhaupt machen können, z.B. wenn eine andere Hauptrasse
			// z.B. Mitgliedschaft mit einer Minorrace hat, dann können wir ihr kein Angebot machen, außer
			// Geschenke geben und Bestechen
			DIPLOMATIC_AGREEMENT::Typ status = DIPLOMATIC_AGREEMENT::NONE;

			map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
				if (it->first != pPlayer->GetRaceID())
				{
					DIPLOMATIC_AGREEMENT::Typ temp = pRace->GetAgreement(it->first);
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
					button->SetState(BUTTON_STATE::DEACTIVATED);
					continue;
				}
				// Handelsvertrag Button
				if (button->GetText() == CLoc::GetString("BTN_TRADECONTRACT"))
				{
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::TRADE && status <= DIPLOMATIC_AGREEMENT::AFFILIATION && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::TRADE)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_FRIENDSHIP"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::FRIENDSHIP && status <= DIPLOMATIC_AGREEMENT::COOPERATION && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_COOPERATION"))
				{
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::COOPERATION && status <= DIPLOMATIC_AGREEMENT::FRIENDSHIP && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::COOPERATION)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_AFFILIATION"))
				{
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::AFFILIATION && status <= DIPLOMATIC_AGREEMENT::FRIENDSHIP && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::AFFILIATION)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_MEMBERSHIP"))
				{
					if (!bAlienDiplomacy && pPlayer->GetAgreement(m_sClickedOnRace) < DIPLOMATIC_AGREEMENT::MEMBERSHIP && status <= DIPLOMATIC_AGREEMENT::FRIENDSHIP && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::MEMBERSHIP)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_WAR"))
				{
					if (pPlayer->GetAgreement(m_sClickedOnRace) != DIPLOMATIC_AGREEMENT::WAR && status <= DIPLOMATIC_AGREEMENT::AFFILIATION && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WAR)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_CORRUPTION"))
				{
					if (!bAlienDiplomacy && (status >= DIPLOMATIC_AGREEMENT::AFFILIATION || (pPlayer->GetAgreement(m_sClickedOnRace) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP && status >= DIPLOMATIC_AGREEMENT::TRADE)) && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::CORRUPTION)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
				else if (button->GetText() == CLoc::GetString("BTN_PRESENT"))
				{
					if (!bAlienDiplomacy && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::PRESENT)
					{
						if (button->GetState() != BUTTON_STATE::ACTIVATED)
							button->SetState(BUTTON_STATE::NORMAL);
					}
					else
						button->SetState(BUTTON_STATE::DEACTIVATED);
				}
			}
			DrawDiplomacyButtons(g, pPlayer, &m_DiplomacyMinorOfferButtons, -1);
		}

		// Abschicken- bzw. Abbrechenbutton anzeigen
		m_bCanSend = m_bShowSendButton;
		s = "";
		// Wenn wir ein gegebenes Angebot wieder abbrechen wollen
		if (!m_bShowSendButton)
			s = CLoc::GetString("BTN_CANCEL");
		else
		{
			// Wenn wir Geld geben wollen, den Abschickenbutton einblenden
			if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
			{
				if (m_OutgoingInfo.m_nCredits > 0 || m_OutgoingInfo.m_nResources[RESOURCES::TITAN] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::DEUTERIUM] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::DURANIUM] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::CRYSTAL] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::IRIDIUM] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::DERITIUM] > 0)
					s = CLoc::GetString("BTN_SEND");
				else
					m_bCanSend = false;
			}
			// Wenn wir Freundschaft, Handelsvertrag usw. anbieten wollen, den Abschickenbutton anzeigen
			else if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::NONE && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::PRESENT && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::CORRUPTION && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WARPACT)
			{
				s = CLoc::GetString("BTN_SEND");
			}
			// Wenn wir eine kleine Rasse bestechen wollen bzw. eine Forderung an die große Rasse stellen Abschickenbutton
			// einblenden. Bei Forderung muß ein Wert ausgewählt worden sein
			else if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::PRESENT || m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::CORRUPTION || m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
			{
				if (m_OutgoingInfo.m_nCredits > 0 || m_OutgoingInfo.m_nResources[RESOURCES::TITAN] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::DEUTERIUM] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::DURANIUM] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::CRYSTAL] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::IRIDIUM] > 0 || m_OutgoingInfo.m_nResources[RESOURCES::DERITIUM] > 0)
					s = CLoc::GetString("BTN_SEND");
				else
					m_bCanSend = false;
			}
			else if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
			{
				if (m_OutgoingInfo.m_sWarpactEnemy != "")
					s = CLoc::GetString("BTN_SEND");
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
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(871,690,160,40), &fontFormat, &fontBrush);
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
		s = CLoc::GetString("BTN_ACCEPT");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(852,480,160,40), &fontFormat, &fontBrush);

		if (m_bShowDeclineButton == TRUE)
			sFile = "Other\\" + pPlayer->GetPrefix() + "button.bop";
		else
			sFile = "Other\\" + pPlayer->GetPrefix() + "buttoni.bop";
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic(sFile);
		if (graphic)
			g->DrawImage(graphic, 852,599,160,40);
		s = CLoc::GetString("BTN_DECLINE");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(852,599,160,40), &fontFormat, &fontBrush);
	}

	// "Diplomatieuebersicht" in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pPlayer, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pPlayer, 3, normalColor);
	fontBrush.SetColor(normalColor);
	if (m_bySubMenu == 0)
		s = CLoc::GetString("DIPLOMACY_MENUE_INFO");
	else if (m_bySubMenu == 1)
		s = CLoc::GetString("DIPLOMACY_MENUE_OFFERS");
	else if (m_bySubMenu == 2)
		s = CLoc::GetString("DIPLOMACY_MENUE_RECEIPTS");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);

	resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CDiplomacyBottomView));
}

void CDiplomacyMenuView::DrawRaceDiplomacyMenue(Graphics* g)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

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
	if(graphic)
		g->DrawImage(graphic, 40, 70, 80, 30);

	if (!m_bSortRaceList)
		s.Format("%s","A-Z");
	else
		s=CLoc::GetString("SORT2");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(40,70,80,30), &fontFormat, &fontBrush);

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

			// Linie zwischen Minors und Majors und Alien
			if (!m_bSortRaceList && it != m_vRaceList.begin())
			{
				vector<CRace*>::iterator itPrev = it;
				--itPrev;

				if (pRace->IsAlien())
				{
					if (it != m_vRaceList.begin() && !(*itPrev)->IsAlien())
						g->DrawLine(&Gdiplus::Pen(penColor), 8, 100+count*25, 150, 100+count*25);
				}
				else if (pRace->IsMinor())
				{
					if (it != m_vRaceList.begin() && !(*itPrev)->IsMinor())
						g->DrawLine(&Gdiplus::Pen(penColor), 8, 100+count*25, 150, 100+count*25);
				}
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
				g->DrawString(CComBSTR(pRace->GetRaceName()), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);

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
				if (pRace->IsMinor() || pRace->IsMajor() && ((CMajor*)pRace)->IsHumanPlayer() == false)
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
					if (relation < 5) s = CLoc::GetString("HATEFUL");
					else if (relation < 15) s = CLoc::GetString("FURIOUS");
					else if (relation < 25) s = CLoc::GetString("HOSTILE");
					else if (relation < 35) s = CLoc::GetString("ANGRY");
					else if (relation < 45) s = CLoc::GetString("NOT_COOPERATIVE");
					else if (relation < 55) s = CLoc::GetString("NEUTRAL");
					else if (relation < 65) s = CLoc::GetString("COOPERATIVE");
					else if (relation < 75) s = CLoc::GetString("FRIENDLY");
					else if (relation < 85) s = CLoc::GetString("OPTIMISTIC");
					else if (relation < 95) s = CLoc::GetString("ENTHUSED");
					else s = CLoc::GetString("DEVOTED");

					fontFormat.SetAlignment(StringAlignmentCenter);
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(735,403,300,25), &fontFormat, &fontBrush);
					fontFormat.SetAlignment(StringAlignmentNear);
				}
			}
			else
			{

				Gdiplus::Color color(normalColor);
				s = this->PrintDiplomacyStatus(pPlayer->GetRaceID(), pRace->GetRaceID(), color);
				fontBrush.SetColor(color);
				g->DrawString(CComBSTR(pRace->GetRaceName()), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
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
			AssertBotE(pRace);

			RectF rect(20,100+count*25,130,25);
			// handelt es sich um das angeklickte Angebot
			if (m_pIncomingInfo == *it)
			{
				// Name der Rasse zeichnen
				fontBrush.SetColor(markColor);
				g->DrawString(CComBSTR(pRace->GetRaceName()), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);

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
				if (pRace->IsMinor() || pRace->IsMajor() && ((CMajor*)pRace)->IsHumanPlayer() == false)
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
					if (relation < 5) s = CLoc::GetString("HATEFUL");
					else if (relation < 15) s = CLoc::GetString("FURIOUS");
					else if (relation < 25) s = CLoc::GetString("HOSTILE");
					else if (relation < 35) s = CLoc::GetString("ANGRY");
					else if (relation < 45) s = CLoc::GetString("NOT_COOPERATIVE");
					else if (relation < 55) s = CLoc::GetString("NEUTRAL");
					else if (relation < 65) s = CLoc::GetString("COOPERATIVE");
					else if (relation < 75) s = CLoc::GetString("FRIENDLY");
					else if (relation < 85) s = CLoc::GetString("OPTIMISTIC");
					else if (relation < 95) s = CLoc::GetString("ENTHUSED");
					else s = CLoc::GetString("DEVOTED");

					fontFormat.SetAlignment(StringAlignmentCenter);
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(735,403,300,25), &fontFormat, &fontBrush);
					fontFormat.SetAlignment(StringAlignmentNear);
				}

				// Text vom Angebot anzeigen
				CDiplomacyBottomView::SetText(m_OutgoingInfo.m_sText);
				CDiplomacyBottomView::SetHeadLine(m_OutgoingInfo.m_sHeadline);

				// Handelt es sich um eine diplomatisches Angebot (keine Antwort, kein normaler Text)
				if (m_OutgoingInfo.m_nFlag == DIPLOMACY_OFFER)
				{
					// Haben wir auf einen Button geklickt, so muß dieser gedrückt dargestellt werden
					if (m_OutgoingInfo.m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
						m_bShowSendButton = FALSE;
					else if (m_OutgoingInfo.m_nAnswerStatus == ANSWER_STATUS::DECLINED)
						m_bShowDeclineButton = FALSE;

					// Bei Kriegserklärung oder Geschenk haben wir keine Wahlmöglichkeit
					if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::WAR || m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
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
				g->DrawString(CComBSTR(pRace->GetRaceName()), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
			}

			// Handelt es sich um eine diplomatisches Angebot (keine Antwort, kein normaler Text)
			// Typ zeichnen (Vorschlag, Forderung, Geschenk)
			rect.X = 200;
			rect.Width = 150;
			if (m_OutgoingInfo.m_nFlag == DIPLOMACY_OFFER)
			{
				if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::PRESENT)
					s = CLoc::GetString("PRESENT");
				else if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
				{
					// handelt es sich um das angeklickte Angebot
					if (m_pIncomingInfo == *it)
					{
						if (m_pIncomingInfo->m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
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
							if (resource[RESOURCES::TITAN] > 0 || resource[RESOURCES::DEUTERIUM] > 0 || resource[RESOURCES::DURANIUM] > 0 || resource[RESOURCES::CRYSTAL] > 0 || resource[RESOURCES::IRIDIUM] > 0 || resource[RESOURCES::DERITIUM] > 0)
							{
								// Wenn Forderung, dann Systemauswahlbutton einblenden, wovon ich die geforderte Ressource
								// abzweigen will
								fontFormat.SetAlignment(StringAlignmentNear);
								fontBrush.SetColor(normalColor);
								s.Format("%s: ",CLoc::GetString("RESOURCE_FROM"));
								g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(350,100+count*25,150,25), &fontFormat, &fontBrush);

								Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pPlayer->GetPrefix() + "button_small.bop");
								Color btnColor;
								CFontLoader::GetGDIFontColor(pPlayer, 1, btnColor);
								SolidBrush btnBrush(btnColor);
								if (graphic)
									g->DrawImage(graphic, 500, 97+count*25, 120, 30);

								s.Format("%s",pDoc->GetSystem(m_ptResourceFromSystem.x, m_ptResourceFromSystem.y).GetName());
								// Wenn hier noch kein System eingestellt ist, dann müssen wir uns eins suchen
								if (s.IsEmpty() || pDoc->GetSystem(m_ptResourceFromSystem.x, m_ptResourceFromSystem.y).OwnerID() != pPlayer->GetRaceID())
								{
									for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
										for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
											if (pDoc->GetSystem(x,y).OwnerID() == pPlayer->GetRaceID() && pDoc->GetSystem(x,y).GetSunSystem() == TRUE)
											{
												m_ptResourceFromSystem = CPoint(x,y);
												m_pIncomingInfo->m_ptKO = m_ptResourceFromSystem;
												s.Format("%s",pDoc->GetSystem(x,y).GetName());
												break;
											}
								}
								// Überprüfen ob wir auf dem gewählten System die Menge der geforderten
								// Ressource im Lager haben und ob wir auch die geforderten Credits bezahlen können
								for (int r = RESOURCES::TITAN; r <= RESOURCES::DERITIUM; r++)
									if (resource[r] > 0 && pDoc->GetSystem(m_ptResourceFromSystem.x, m_ptResourceFromSystem.y).GetResourceStore(r) < resource[r] && pDoc->GetSystem(m_ptResourceFromSystem.x, m_ptResourceFromSystem.y).OwnerID() == pPlayer->GetRaceID())
										m_bShowSendButton = false;
								fontFormat.SetAlignment(StringAlignmentCenter);
								g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(500,97+count*25,120,30), &fontFormat, &btnBrush);
							}
							// Überprüfen ob wir auch die geforderten Credits bezahlen können
							if (pPlayer->GetEmpire()->GetCredits() < m_pIncomingInfo->m_nCredits)
								m_bShowSendButton = false;
						}
					}
					s = CLoc::GetString("REQUEST");
				}
				else if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::WAR)
					s = CLoc::GetString("WAR");
				else
					s = CLoc::GetString("SUGGESTION");
			}
			// handelt es sich um eine Antwort (also um kein Angebot und um kein Text)
			else if (m_OutgoingInfo.m_nFlag == DIPLOMACY_ANSWER)
				s = CLoc::GetString("ANSWER");

			fontBrush.SetColor(normalColor);
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
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
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

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
	g->DrawString(CComBSTR(pRace->GetRaceDesc()), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(190,475,845,190), &fontFormat, &fontBrush);

	// Ingame-Daten zu der Rasse, welche in der Box angezeigt werden
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontBrush.SetColor(markColor);
	RectF rect(215,110,260,25);
	s = CLoc::GetString("NAME").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	s = CLoc::GetString("HOMESYSTEM").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	s = CLoc::GetString("RELATIONSHIP").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 50;
	s = CLoc::GetString("KNOWN_EMPIRES").MakeUpper()+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);

	// Informationen anzeigen
	rect.Y -= 100;
	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentFar);
	s = pRace->GetRaceName();
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;

	// Name des Heimatsystems und Sektorkoordinate anzeigen
	s = pRace->GetHomesystemName();
	if (!s.IsEmpty())
	{
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		{
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			{
				if(pDoc->GetSystem(x,y).GetName()==s&&pDoc->GetSystem(x,y).GetKnown(pPlayer->GetRaceID()))
				{
					CString coords;
					coords.Format(" (%c%i)", (char)y+97,x+1);
					s += coords;
					break;
				}
				else if(pDoc->GetSystem(x,y).GetName()==s&&!pDoc->GetSystem(x,y).GetKnown(pPlayer->GetRaceID()))
				{
					CString coords;
					coords.Format(" (%s)", CLoc::GetString("UNKNOWN"));
					s += coords;
					break;
				}
			}
		}

		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}

	rect.Y += 25;

	Gdiplus::Color color(normalColor);
	s = this->PrintDiplomacyStatus(pPlayer->GetRaceID(), sWhichRace, color);
	fontBrush.SetColor(color);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;

	if (pRace->IsMinor())
	{
		fontBrush.SetColor(markColor);
		fontFormat.SetAlignment(StringAlignmentNear);
		s = CLoc::GetString("ACCEPTANCE").MakeUpper()+":";
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
		fontBrush.SetColor(normalColor);
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%d%%",(int)(((CMinor*)pRace)->GetAcceptancePoints(pPlayer->GetRaceID()) / 50));
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}
	// Wenn wir einen Verteidigungpakt mit der Rasse haben (nur bei Majors)
	else if (pRace->IsMajor() && pPlayer->GetDefencePact(sWhichRace) == true)
	{
		fontBrush.SetColor(Color(226,44,250));
		if (pPlayer->GetDefencePactDuration(sWhichRace) != 0)
			s.Format("%s (%d)", CLoc::GetString("DEFENCE_PACT"), pPlayer->GetDefencePactDuration(sWhichRace));
		else
			s = CLoc::GetString("DEFENCE_PACT");
		fontFormat.SetAlignment(StringAlignmentCenter);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}

	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentNear);
	rect.Y += 50;

	// bei Freundschaft wird der Status zu den anderen Rassen angezeigt
	if (pPlayer->GetAgreement(sWhichRace) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
	{
		map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			if (it->first != sWhichRace && it->first != pPlayer->GetRaceID())
				if (pRace->IsRaceContacted(it->first))
				{
					fontBrush.SetColor(normalColor);
					fontFormat.SetAlignment(StringAlignmentNear);
					s = it->second->GetRaceName();
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);

					fontFormat.SetAlignment(StringAlignmentFar);
					Gdiplus::Color color(normalColor);
					s = this->PrintDiplomacyStatus(it->first, sWhichRace, color);
					fontBrush.SetColor(color);
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
					rect.Y += 25;
				}
	}
	// sonst keine Angaben
	else
	{
		s = CLoc::GetString("NO_SPECS");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), rect, &fontFormat, &fontBrush);
	}

	// Hier die ganzen Buttons in der Informationsansicht zeichnen
	// Wenn wir einen Vertrag mit einer Minorrace haben so können wir diesen auch aufheben
	if (pRace->IsMinor())
	{
		// Angebote durchgehen, suchen ob wir schon eine Kündigung bei der Rasse gemacht haben
		for (UINT i = 0; i < pPlayer->GetOutgoingDiplomacyNews()->size(); i++)
		{
			m_OutgoingInfo = pPlayer->GetOutgoingDiplomacyNews()->at(i);
			if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::NONE && m_OutgoingInfo.m_sToRace == sWhichRace && m_OutgoingInfo.m_sFromRace == pPlayer->GetRaceID())
			{
				m_bShowSendButton = FALSE;
				CDiplomacyBottomView::SetHeadLine(m_OutgoingInfo.m_sHeadline);
				CDiplomacyBottomView::SetText(m_OutgoingInfo.m_sText);
				break;
			}
		}

		if (pRace->GetAgreement(pPlayer->GetRaceID()) > DIPLOMATIC_AGREEMENT::NONE)
		{
			m_OutgoingInfo.m_sHeadline = CLoc::GetString("CANCEL_AGREEMENT");
			Gdiplus::Color color;
			m_OutgoingInfo.m_sText = CLoc::GetString("CANCEL_AGREEMENT_TEXT", FALSE, this->PrintDiplomacyStatus(pPlayer->GetRaceID(), m_sClickedOnRace, color), pRace->GetRaceName());

			if (m_bShowSendButton == TRUE)
				s = CLoc::GetString("BTN_ANNUL");
			else
				s = CLoc::GetString("BTN_CANCEL");

			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pPlayer->GetPrefix() + "button_small.bop");
			Color btnColor;
			CFontLoader::GetGDIFontColor(pPlayer, 1, btnColor);
			SolidBrush btnBrush(btnColor);
			if (graphic)
				g->DrawImage(graphic, 275, 370, 120, 30);

			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(275,370,120,30), &fontFormat, &btnBrush);
		}
	}
}

void CDiplomacyMenuView::DrawDiplomacyOfferMenue(Graphics* g, const CString& sWhichRace)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

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

	s = CLoc::GetString("CHOOSE_OFFER")+":";
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(200,100,360,25), &fontFormat, &fontBrush);

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
	if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::NONE)
	{
		// Balken zeichnen, auf dem ich den Betrag auswählen kann, außer bei Kriegserklärung
		if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WAR)
		{
			// ist aus irgendeinem Grund die übergebende Menge größer als die aktuellen Credits, so
			// werden die Credits runtergerechnet
			if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && m_bShowSendButton && m_OutgoingInfo.m_nCredits > pPlayer->GetEmpire()->GetCredits())
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
			s.Format("%s: %d %s", CLoc::GetString("PAYMENT"), m_OutgoingInfo.m_nCredits, CLoc::GetString("CREDITS"));
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(195,410,255,30), &fontFormat, &fontBrush);

			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pPlayer->GetPrefix() + "button_small.bop");
			Color btnColor;
			CFontLoader::GetGDIFontColor(pPlayer, 1, btnColor);
			SolidBrush btnBrush(btnColor);

			// Ressourcengeschenke können nur gemacht werden, wenn wir mindst. einen Handelsvertrag haben oder diese Fordern
			if (pPlayer->GetAgreement(m_sClickedOnRace) >= DIPLOMATIC_AGREEMENT::TRADE || m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
			{
				// auch das Umschaltfeld auf die gegebene Ressource umstellen
				int iWhichResource = m_byWhichResourceIsChosen;

				// Balken für Ressourcengeschenk zeichnen
				CString res;
				if (iWhichResource == RESOURCES::TITAN)
					res = CLoc::GetString("TITAN");
				else if (iWhichResource == RESOURCES::DEUTERIUM)
					res = CLoc::GetString("DEUTERIUM");
				else if (iWhichResource == RESOURCES::DURANIUM)
					res = CLoc::GetString("DURANIUM");
				else if (iWhichResource == RESOURCES::CRYSTAL)
					res = CLoc::GetString("CRYSTAL");
				else if (iWhichResource == RESOURCES::IRIDIUM)
					res = CLoc::GetString("IRIDIUM");
				else if (iWhichResource == RESOURCES::DERITIUM)
					res = CLoc::GetString("DERITIUM");
				UINT nUnit = iWhichResource == RESOURCES::DERITIUM ? 5 : 1000;

				// sind aus irgendeinem Grund die übergebende Menge größer als die aktuellen vorhandenen Ressourcen, so
				// werden die Ressourcen runtergerechnet
				if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && m_bShowSendButton && m_OutgoingInfo.m_nResources[iWhichResource] > 0 && m_OutgoingInfo.m_ptKO != CPoint(-1,-1))
				{
					if (m_OutgoingInfo.m_nResources[iWhichResource] > pDoc->GetSystem(m_OutgoingInfo.m_ptKO.x, m_OutgoingInfo.m_ptKO.y).GetResourceStore(iWhichResource))
					{
						m_OutgoingInfo.m_nResources[iWhichResource] = pDoc->GetSystem(m_OutgoingInfo.m_ptKO.x, m_OutgoingInfo.m_ptKO.y).GetResourceStore(iWhichResource) / nUnit;
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
				s.Format("%s: %d %s %s",CLoc::GetString("TRANSFER"), m_OutgoingInfo.m_nResources[iWhichResource], CLoc::GetString("UNITS"),res);
				fontFormat.SetAlignment(StringAlignmentNear);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(195,490,355,30), &fontFormat, &fontBrush);

				// Wenn wir mit der Rasse mindst. einen Handelsvertrag haben, so zeigt er die ungefähre Menge des Rohstoffes an
				if (pRace->IsMinor())
				{
					CMinor* pMinor = dynamic_cast<CMinor*>(pRace);
					if (!pMinor)
						return;
					CPoint ko = pMinor->GetRaceKO();
					if (ko == CPoint(-1,-1))
						return;

					UINT nStorage = pDoc->GetSystem(ko.x, ko.y).GetResourceStore(iWhichResource);

					if (nStorage <= 1 * nUnit)
						s = CLoc::GetString("SCARCELY_EXISTING");
					else if (nStorage <= 2 * nUnit)
						s = CLoc::GetString("VERY_LESS_EXISTING");
					else if (nStorage <= 4 * nUnit)
						s = CLoc::GetString("LESS_EXISTING");
					else if (nStorage <= 8 * nUnit)
						s = CLoc::GetString("MODERATE_EXISTING");
					else if (nStorage <= 16 * nUnit)
						s = CLoc::GetString("MUCH_EXISTING");
					else if (nStorage <= 32 * nUnit)
						s = CLoc::GetString("VERY_MUCH_EXISTING");
					else
						s = CLoc::GetString("ABOUNDING_EXISTING");
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(195,545,355,25), &fontFormat, &fontBrush);
				}

				// Den Button für die Ressourcenauswahl zeichnen
				if (graphic)
					g->DrawImage(graphic, 510, 518, 120, 30);
				fontFormat.SetAlignment(StringAlignmentCenter);
				g->DrawString(CComBSTR(res), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,518,120,30), &fontFormat, &btnBrush);
				s = CLoc::GetString("RESOURCE");
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,490,120,30), &fontFormat, &fontBrush);

				// Den Button für die Systemauswahl der abzuknüpfenden Ressource zeichnen, nicht bei Forderung von Majorrace
				if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST)
				{
					if (graphic)
						g->DrawImage(graphic, 510, 605, 120, 30);
					s.Format("%s", pDoc->GetSystem(m_OutgoingInfo.m_ptKO.x, m_OutgoingInfo.m_ptKO.y).GetName());
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,605,120,30), &fontFormat, &btnBrush);
					s = CLoc::GetString("FROM_SYSTEM");
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,577,120,30), &fontFormat, &fontBrush);
				}
			}

			// Wenn wir bestechen wollen, dann möglicherweise den Auswahlbutton wen wir bestechen wollen einblenden
			if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::CORRUPTION)
			{
				if (m_OutgoingInfo.m_sCorruptedRace == "")
				{
					map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
					// haben nicht auf den Button gedrückt -> mgl. Gegner auswählen
					if (pRace->GetAgreement(pPlayer->GetRaceID()) < DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
							if (it->first != pPlayer->GetRaceID() && pRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
							{
								m_OutgoingInfo.m_sCorruptedRace = it->first;
								break;
							}
					}
					else
					{
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
							if (it->first != pPlayer->GetRaceID() && pRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::TRADE)
							{
								m_OutgoingInfo.m_sCorruptedRace = it->first;
								break;
							}
					}
				}

				if (pRace->GetAgreement(pPlayer->GetRaceID()) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
				{
					CMajor* pCorruptedMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_OutgoingInfo.m_sCorruptedRace));
					if (!pCorruptedMajor)
						return;
					s = pCorruptedMajor->GetRaceName();

					fontFormat.SetAlignment(StringAlignmentCenter);
					if (graphic)
						g->DrawImage(graphic, 510, 438, 120, 30);
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,438,120,30), &fontFormat, &btnBrush);
					s = CLoc::GetString("ENEMY");
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,410,120,30), &fontFormat, &fontBrush);
				}
			}

			// Wenn wir einen Vertrag einer Majorrace anbieten, dann Button für die Dauer dieses Vertrages einblenden
			else if (pRace->IsMajor() && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::PRESENT && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WARPACT)
			{
				if (m_OutgoingInfo.m_nDuration == 0)
					s =CLoc::GetString("UNLIMITED");
				else
					s.Format("%d %s", m_OutgoingInfo.m_nDuration, CLoc::GetString("ROUNDS"));

				fontFormat.SetAlignment(StringAlignmentCenter);
				if (graphic)
					g->DrawImage(graphic, 510, 438, 120, 30);
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,438,120,30), &fontFormat, &btnBrush);
				s = CLoc::GetString("CONTRACT_DURATION");
				g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,410,120,30), &fontFormat, &fontBrush);
			}

			// Wenn wir einen Kriegspakt anbieten wollen, dann Button zur Auswahl des Kriegsgegners mit anzeigen
			else if  (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
			{
				// Wenn wir noch keinen Gegner ausgewählt haben
				if (m_OutgoingInfo.m_sWarpactEnemy == "")
				{
					map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
					for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
						if (it->first != pPlayer->GetRaceID() && it->first != m_sClickedOnRace && pPlayer->GetAgreement(m_sClickedOnRace) != DIPLOMATIC_AGREEMENT::WAR
							&& pPlayer->IsRaceContacted(it->first) && pRace->IsRaceContacted(it->first) && pRace->GetAgreement(it->first) != DIPLOMATIC_AGREEMENT::WAR)
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
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,438,120,30), &fontFormat, &btnBrush);
					s = CLoc::GetString("WARPACT_ENEMY");
					g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(510,410,120,30), &fontFormat, &fontBrush);
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
	DrawGDIButtons(g, buttonArray, counter, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}

CString CDiplomacyMenuView::PrintDiplomacyStatus(const CString& sOurRace, const CString& sRace, Gdiplus::Color& color)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pOurRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sOurRace));
	if (!pOurRace)
		return "";

	CRace* pRace = pDoc->GetRaceCtrl()->GetRace(sRace);
	if (!pRace)
		return "";

	CString status;
	// wurde die Minorrace unterworfen?
	if (pRace->IsMinor() && ((CMinor*)pRace)->GetSubjugated())
	{
		status = CLoc::GetString("SUBJUGATED");
		color.SetFromCOLORREF(RGB(178,0,255));
		return status;
	}

	// Namen der Vertragsform und Farbe ermitteln
	switch (pOurRace->GetAgreement(sRace))
	{
	case DIPLOMATIC_AGREEMENT::NONE:
		return status = CLoc::GetString("NONE");
	case DIPLOMATIC_AGREEMENT::NAP:
		{
			if (pRace->IsMajor() && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CLoc::GetString("NON_AGGRESSION_SHORT"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CLoc::GetString("NON_AGGRESSION_SHORT");
			color.SetFromCOLORREF(RGB(139,175,172));
			return status;
		}
	case DIPLOMATIC_AGREEMENT::TRADE:
		{
			if (pRace->IsMajor() && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CLoc::GetString("TRADE_AGREEMENT_SHORT"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CLoc::GetString("TRADE_AGREEMENT_SHORT");
			color.SetFromCOLORREF(RGB(233,183,12));
			return status;
		}
	case DIPLOMATIC_AGREEMENT::FRIENDSHIP:
		{
			if (pRace->IsMajor() && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CLoc::GetString("FRIENDSHIP_SHORT"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CLoc::GetString("FRIENDSHIP_SHORT");
			color.SetFromCOLORREF(RGB(6,187,34));
			return status;
		}
	case DIPLOMATIC_AGREEMENT::COOPERATION:
		{
			if (pRace->IsMajor() && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CLoc::GetString("COOPERATION"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CLoc::GetString("COOPERATION");
			color.SetFromCOLORREF(RGB(37,159,250));
			return status;
		}
	case DIPLOMATIC_AGREEMENT::AFFILIATION:
		{
			if (pRace->IsMajor() && pOurRace->GetAgreementDuration(sRace) != 0)
				status.Format("%s (%d)",CLoc::GetString("AFFILIATION"), pOurRace->GetAgreementDuration(sRace));
			else
				status = CLoc::GetString("AFFILIATION");
			color.SetFromCOLORREF(RGB(29,29,248));
			return status;
		}
	case DIPLOMATIC_AGREEMENT::MEMBERSHIP:
		{
			color.SetFromCOLORREF(RGB(115,12,228));
			return status = CLoc::GetString("MEMBERSHIP");
		}
	case DIPLOMATIC_AGREEMENT::WAR:
		{
			color.SetFromCOLORREF(RGB(220,15,15));
			return status = CLoc::GetString("WAR");
		}
	default:
		return status = "";
	}
}

void CDiplomacyMenuView::TakeOrGetbackResLat(bool bTake)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

	// bei einer Forderung an eine andere Majorrace werden keine Ressourcen aus den Lagern genommen, auch
	// werden bei einem Abbruch des diplomatischen Angebots keine Ressourcen wieder gut geschrieben.
	if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::REQUEST && m_OutgoingInfo.m_sFromRace == pPlayer->GetRaceID())
		return;

	if (bTake)
	{
		// Credits von Creditbestand abziehen
		pPlayer->GetEmpire()->SetCredits(-m_OutgoingInfo.m_nCredits);
		m_OutgoingInfo.m_nCredits = 0;
		// Rohstoffe aus dem Lager nehmen
		for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
		{
			if (m_OutgoingInfo.m_nResources[res] > 0)
			{
				CPoint ko = m_OutgoingInfo.m_ptKO;
				if (ko == CPoint(-1,-1))
					AfxMessageBox("Error in CDiplomacyView::TakeOrGetbackResLat(): KO has no value!");
				else
				{
					pDoc->GetSystem(ko.x, ko.y).SetResourceStore(res, -m_OutgoingInfo.m_nResources[res]);
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
		for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
		{
			if (m_OutgoingInfo.m_nResources[res] > 0)
			{
				CPoint ko = m_OutgoingInfo.m_ptKO;
				if (ko == CPoint(-1,-1))
					AfxMessageBox("Error in CDiplomacyView::TakeOrGetbackResLat(): KO has no value!");
				else
				{
					pDoc->GetSystem(ko.x, ko.y).SetResourceStore(res, m_OutgoingInfo.m_nResources[res]);
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
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

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
		for (vector<CRace*>::iterator it = m_vRaceList.begin(); it != m_vRaceList.end(); ++it)
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
				if (rect.PtInRect(point) && m_pIncomingInfo->m_nAnswerStatus != ANSWER_STATUS::ACCEPTED)
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

	rect.SetRect(40,70,120,100);
	if (rect.PtInRect(point))
	{
		m_bSortRaceList=!m_bSortRaceList;
		if (m_bSortRaceList)
			std::sort(m_vRaceList.begin(), m_vRaceList.end(),ComareRaceAgreement);
		else
			std::sort(m_vRaceList.begin(), m_vRaceList.end(),CompareRaceName);
		Invalidate();
		return;
	}

	// Wenn wir in der Informationsansicht sind und eine Rasse angklickt haben
	if (m_bySubMenu == 0 && m_sClickedOnRace != "")
	{
		CRace* pClickedRace = pDoc->GetRaceCtrl()->GetRace(m_sClickedOnRace);
		// schauen ob bei einer Minorrace auf den Kündigungsbutton geklickt wurde
		if (pClickedRace && pClickedRace->IsMinor() && CRect(275,370,395,400).PtInRect(point) && pPlayer->GetAgreement(m_sClickedOnRace) > DIPLOMATIC_AGREEMENT::NONE)
		{
			// Kündigung hinzufügen
			if (m_bShowSendButton == TRUE)
			{
				m_OutgoingInfo.m_nFlag = DIPLOMACY_OFFER;
				m_OutgoingInfo.m_nSendRound = pDoc->GetCurrentRound();
				m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::NONE;
				m_OutgoingInfo.m_sFromRace = pPlayer->GetRaceID();
				m_OutgoingInfo.m_sToRace = m_sClickedOnRace;
				pPlayer->GetOutgoingDiplomacyNews()->push_back(m_OutgoingInfo);
			}
			// Kündigung wieder entfernen
			else
			{
				// Angebote durchgehen, suchen ob wir schon eine Kündigung bei der Rasse gemacht haben
				for (vector<CDiplomacyInfo>::iterator it = pPlayer->GetOutgoingDiplomacyNews()->begin(); it != pPlayer->GetOutgoingDiplomacyNews()->end(); ++it)
				{
					m_OutgoingInfo = *it;
					if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::NONE && m_OutgoingInfo.m_sToRace == m_sClickedOnRace && m_OutgoingInfo.m_sFromRace == pPlayer->GetRaceID())
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
			if (pClickedRace->IsMajor())
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
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::TRADE;
							break;
						// Freundschaftsangebot
						case 1:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
							break;
						// Kooperationsangebot
						case 2:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::COOPERATION;
							break;
						// Bündnisangebot
						case 3:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::AFFILIATION;
							break;
						// Nichtangriffspaktangebot
						case 4:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::NAP;
							break;
						// Kriegserklärung
						case 5:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::WAR;
							break;
						// Geschenk
						case 6:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::PRESENT;
							break;
						// Forderung
						case 7:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::REQUEST;
							break;
						// Verteidigungspaktangebot
						case 8:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::DEFENCEPACT;
							break;
						// Kriegspakt
						case 9:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::WARPACT;
							break;
					}
					m_OutgoingInfo.m_sFromRace = pPlayer->GetRaceID();
					m_OutgoingInfo.m_sToRace = m_sClickedOnRace;
					Invalidate();
					return;
				}
			}
			else if (pClickedRace->IsMinor())
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
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::TRADE;
							break;
						// Freundschaftsangebot
						case 1:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::FRIENDSHIP;
							break;
						// Kooperationsangebot
						case 2:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::COOPERATION;
							break;
						// Bündnisangebot
						case 3:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::AFFILIATION;
							break;
						// Mitgliedschaftsangebot
						case 4:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::MEMBERSHIP;
							break;
						// Kriegserklärung
						case 5:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::WAR;
							break;
						// Geschenk
						case 6:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::PRESENT;
							break;
						// Bestechnung
						case 7:
							m_OutgoingInfo.m_nType = DIPLOMATIC_AGREEMENT::CORRUPTION;
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
		if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::NONE)
		{
			if (m_bShowSendButton == true && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WAR)
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
						if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST)
							if (m_OutgoingInfo.m_nCredits > pPlayer->GetEmpire()->GetCredits())
							{
								m_OutgoingInfo.m_nCredits = max(0, pPlayer->GetEmpire()->GetCredits() / 250);
								m_OutgoingInfo.m_nCredits *= 250;
							}
							Invalidate();
							break;
					}

					// nur wenn wir mindst. einen Handelsvertrag besitzen können wir Ressourcen verschenken oder bei einer Forderung stellen
					if (pPlayer->GetAgreement(m_sClickedOnRace) >= DIPLOMATIC_AGREEMENT::TRADE || m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
					{
						timber[t].SetRect(183+t*12,520,193+t*12+2,545);

						// Haben wir auf den Balken gedrückt, um die Menge der zu verschenkenden bzw. zu fordernden Ressourcen zu ändern
						if (timber[t].PtInRect(point))
						{
							AssertBotE(m_byWhichResourceIsChosen >= RESOURCES::TITAN && m_byWhichResourceIsChosen <= RESOURCES::DERITIUM);

							UINT nStorage = pDoc->GetSystem(m_OutgoingInfo.m_ptKO.x, m_OutgoingInfo.m_ptKO.y).GetResourceStore(m_byWhichResourceIsChosen);
							// bei normalen Ressourcen wird in 1000er Schritten gegeben, bei Deritium in 5er
							int dUnit = m_byWhichResourceIsChosen == RESOURCES::DERITIUM ? 5 : 1000;
							m_OutgoingInfo.m_nResources[m_byWhichResourceIsChosen] = t * dUnit;
							// geben wir selbst die Ressource, dann benötigen wir genügend im Lager.
							if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && m_OutgoingInfo.m_nResources[m_byWhichResourceIsChosen] > nStorage)
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
					if (m_byWhichResourceIsChosen < RESOURCES::DERITIUM)
						m_byWhichResourceIsChosen++;
					else
						m_byWhichResourceIsChosen = RESOURCES::TITAN;

					// alle "alten" Ressourcenmengen löschen
					for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
						m_OutgoingInfo.m_nResources[res] = 0;
					Invalidate();
					return;
				}

				// Wenn wir auf den kleinen Systembutton geklickt haben um ein anderes System zu wählen
				rect.SetRect(510,605,630,635);
				if (rect.PtInRect(point) && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST)
				{
					int current = -1;
					for (int i = 0; i < pPlayer->GetEmpire()->GetSystemList()->GetSize(); i++)
						if (pPlayer->GetEmpire()->GetSystemList()->GetAt(i).ko == m_OutgoingInfo.m_ptKO)
							current = i;
					if (current != -1)
					{
						current++;
						// Auch wieder alle Mengen auf Null zurücksetzen
						for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
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
				if (rect.PtInRect(point) && m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::CORRUPTION)
				{
					map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
					// nur wenn wir eine Freundschaft oder einen höherwertigen Vertrag mit der Minorrace haben können wir die Bestechungsrasse wählen
					if (pPlayer->GetAgreement(m_sClickedOnRace) >= DIPLOMATIC_AGREEMENT::FRIENDSHIP)
					{
						map<CString, CMajor*>::iterator it = pmMajors->find(m_OutgoingInfo.m_sCorruptedRace);
						if (it == pmMajors->end())
							it = pmMajors->begin();
						while (1)
						{
							++it;
							if (it == pmMajors->end())
								it = pmMajors->begin();

							if (it->first != pPlayer->GetRaceID() && pClickedRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::TRADE)
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
						for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
						{
							if (it->first != pPlayer->GetRaceID() && pClickedRace->GetAgreement(it->first) >= DIPLOMATIC_AGREEMENT::AFFILIATION)
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
				else if (rect.PtInRect(point) && pClickedRace->IsMajor() && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::PRESENT && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WARPACT)
				{
					m_OutgoingInfo.m_nDuration += 10;
					if (m_OutgoingInfo.m_nDuration > 100)
						m_OutgoingInfo.m_nDuration = 0;
					CalcDeviceRect(rect);
					InvalidateRect(rect);
					return;
				}

				// Wenn wir auf den kleinen Button geklickt haben um bei einem Kriegspakt den Kriegsgegner wählen zu wollen
				else if (rect.PtInRect(point) && m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::WARPACT)
				{
					// Vektor mit zulässigen Kriegspaktgegnern füllen
					vector<CRace*> vEnemies;
					for (UINT i = 0; i < m_vRaceList.size(); i++)
					{
						// die Bedingungen müssen erfüllt sein
						if (m_vRaceList[i]->IsMajor() && m_sClickedOnRace != m_vRaceList[i]->GetRaceID() && pClickedRace->IsRaceContacted(m_vRaceList[i]->GetRaceID()) == true && pClickedRace->GetAgreement(m_vRaceList[i]->GetRaceID()) != DIPLOMATIC_AGREEMENT::WAR)
							vEnemies.push_back(m_vRaceList[i]);
					}
					// gibt es zulässige Kriegspaktgegner
					if (!vEnemies.empty())
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
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				Invalidate();
				return;
			}

			// Wenn wir auf den Abbrechenbutton geklicked haben
			else if (rect.PtInRect(point) && m_bShowSendButton == false)
			{
				// angebotenes Credits und Ressourcen wieder zurück ins Lager geben
				this->TakeOrGetbackResLat(false);
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
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
			m_pIncomingInfo->m_nAnswerStatus = ANSWER_STATUS::ACCEPTED;
			// Wenn wir eine Forderung annehmen, dann sofort die Credits und Ressourcen aus den Lagern nehmen
			if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::REQUEST)
			{
				this->TakeOrGetbackResLat(true);
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
			Invalidate();
		}

		// Ablehnenbutton
		rect.SetRect(852,599,1012,639);
		if (rect.PtInRect(point) && m_bShowDeclineButton == TRUE)
		{
			// Wenn wir eine Forderung zuerst angenommen hatten und diese in der selben Runde doch ablehnen
			// wollen, so die geforderten Ressourcen und Credits wieder zurückgeben
			if (m_OutgoingInfo.m_nType == DIPLOMATIC_AGREEMENT::REQUEST && m_OutgoingInfo.m_nAnswerStatus == ANSWER_STATUS::ACCEPTED)
			{
				this->TakeOrGetbackResLat(false);
				resources::pMainFrame->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
			m_pIncomingInfo->m_nAnswerStatus = ANSWER_STATUS::DECLINED;
			Invalidate();
		}
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CDiplomacyMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

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
				if (pRace->IsMajor())
					ButtonReactOnMouseOver(point, &m_DiplomacyMajorOfferButtons);
				else if (pRace->IsMinor())
					ButtonReactOnMouseOver(point, &m_DiplomacyMinorOfferButtons);
			}
		}

	CMainBaseView::OnMouseMove(nFlags, point);
}

BOOL CDiplomacyMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

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
						++it;
					}
					else if (zDelta > 0 && it != m_vRaceList.begin())
					{
						--it;
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
						++it;
					}
					else if (zDelta > 0 && it != m_vIncomeList.begin())
					{
						--it;
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
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

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
			if (!m_vRaceList.empty())
				--it;
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
							++it;
						}
						else if (nChar == VK_UP && it != m_vRaceList.begin())
						{
							--it;
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
							++it;
						}
						else if (nChar == VK_UP && it != m_vIncomeList.begin())
						{
							--it;
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
	AssertBotE((CBotEDoc*)GetDocument());

	CMajor* pPlayer = m_pPlayersRace;
	AssertBotE(pPlayer);

	CString sPrefix = pPlayer->GetPrefix();

	// alle Buttons in der View anlegen und Grafiken laden
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	// Buttons in den Diplomatieansichten
	m_DiplomacyMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CLoc::GetString("BTN_INFORMATION"), fileN, fileI, fileA));
	m_DiplomacyMainButtons.Add(new CMyButton(CPoint(200,690) , CSize(160,40), CLoc::GetString("OFFER"), fileN, fileI, fileA));
	m_DiplomacyMainButtons.Add(new CMyButton(CPoint(390,690) , CSize(160,40), CLoc::GetString("RECEIPT"), fileN, fileI, fileA));
	// Angebotsbuttons in der Diplomatieangebotsansicht
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,140) , CSize(160,40), CLoc::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,140) , CSize(160,40), CLoc::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,190) , CSize(160,40), CLoc::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,190) , CSize(160,40), CLoc::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,240) , CSize(160,40), CLoc::GetString("BTN_NAP"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,340) , CSize(160,40), CLoc::GetString("BTN_WAR"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(200,290) , CSize(160,40), CLoc::GetString("BTN_PRESENT"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,290) , CSize(160,40), CLoc::GetString("BTN_REQUEST"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,240) , CSize(160,40), CLoc::GetString("BTN_DEFENCE"), fileN, fileI, fileA));
	m_DiplomacyMajorOfferButtons.Add(new CMyButton(CPoint(400,340) , CSize(160,40), CLoc::GetString("BTN_WARPACT"), fileN, fileI, fileA));

	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,165) , CSize(160,40), CLoc::GetString("BTN_TRADECONTRACT"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,165) , CSize(160,40), CLoc::GetString("BTN_FRIENDSHIP"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,215) , CSize(160,40), CLoc::GetString("BTN_COOPERATION"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,215) , CSize(160,40), CLoc::GetString("BTN_AFFILIATION"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,265) , CSize(160,40), CLoc::GetString("BTN_MEMBERSHIP"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,265) , CSize(160,40), CLoc::GetString("BTN_WAR"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(200,315) , CSize(160,40), CLoc::GetString("BTN_PRESENT"), fileN, fileI, fileA));
	m_DiplomacyMinorOfferButtons.Add(new CMyButton(CPoint(400,315) , CSize(160,40), CLoc::GetString("BTN_CORRUPTION"), fileN, fileI, fileA));
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
		CBotEDoc* pDoc = resources::pDoc;
		AssertBotE(pDoc);

		if (!pDoc->m_bDataReceived)
			return;

		CMajor* pPlayer = m_pPlayersRace;
		AssertBotE(pPlayer);

		CalcLogicalPoint(point);

		CRace* pClickedRace = pDoc->GetRaceCtrl()->GetRace(m_sClickedOnRace);

		// Sind wir in dem Diplomatiemenue für Geschenke geben, Freundschaft, Kooperation, Angliederung usw. anbieten und handelt es sich nicht um eine Kriegserkärung
		if (m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::NONE)
		{
			if (m_bShowSendButton == true && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WAR)
			{
				// Wenn wir auf den kleinen Ressourcenbutton geklickt haben, nächste Ressource anzeigen
				CRect rect;
				rect.SetRect(510,518,630,548);
				if (rect.PtInRect(point))
				{
					if (m_byWhichResourceIsChosen > RESOURCES::TITAN)
						m_byWhichResourceIsChosen--;
					else
						m_byWhichResourceIsChosen = RESOURCES::DERITIUM;

					// alle "alten" Ressourcenmengen löschen
					for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
						m_OutgoingInfo.m_nResources[res] = 0;
					Invalidate();
					return;
				}

				// Wenn wir auf den kleinen Systembutton geklickt haben um ein anderes System zu wählen
				rect.SetRect(510,605,630,635);
				if (rect.PtInRect(point) && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST)
				{
					int current = -1;
					for (int i = 0; i < pPlayer->GetEmpire()->GetSystemList()->GetSize(); i++)
						if (pPlayer->GetEmpire()->GetSystemList()->GetAt(i).ko == m_OutgoingInfo.m_ptKO)
							current = i;
					if (current != -1)
					{
						current--;
						// Auch wieder alle Mengen auf Null zurücksetzen
						for (int res = RESOURCES::TITAN; res <= RESOURCES::DERITIUM; res++)
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
				if (rect.PtInRect(point) && pClickedRace->IsMajor() && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::PRESENT && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::REQUEST && m_OutgoingInfo.m_nType != DIPLOMATIC_AGREEMENT::WARPACT)
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
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

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
