// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "BotE.h"

#include "MainFrm.h"

#include "StartMenuView.h"
#include "ChooseRaceView.h"
#include "NewGameView.h"

#include "GalaxyMenuView.h"
#include "SystemMenuView.h"
#include "ResearchMenuView.h"
#include "IntelMenuView.h"
#include "DiplomacyMenuView.h"
#include "TradeMenuView.h"
#include "EmpireMenuView.h"
#include "FleetMenuView.h"
#include "ShipDesignMenuView.h"
#include "TransportMenuView.h"
#include "EventMenuView.h"
#include "MenuChooseView.h"
#include "PlanetBottomView.h"
#include "ShipBottomView.h"
#include "ResearchBottomView.h"
#include "IntelBottomView.h"
#include "DiplomacyBottomView.h"
#include "TradeBottomView.h"
#include "ShipDesignBottomView.h"
#include "CombatMenuView.h"
#include "SmallInfoView.h"
#include "IniLoader.h"
#include "ClientWorker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	ON_WM_CREATE()
	ON_MESSAGE(WM_INITVIEWS, CMainFrame::InitViews)
	ON_MESSAGE(WM_UPDATEVIEWS, CMainFrame::UpdateViews)
	ON_MESSAGE(WM_COMBATVIEW, CMainFrame::ShowCombatView)
	ON_NOTIFY (UDM_TOOLTIP_DISPLAY, NULL, NotifyCPPTooltip)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Zerstörung

CMainFrame::CMainFrame()
{
	// ZU ERLEDIGEN: Hier Code zur Member-Initialisierung einfügen
	resources::pMainFrame = this;
}

CMainFrame::~CMainFrame()
{
	resources::pMainFrame = NULL;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	/*if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME,
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Dialogleiste konnte nicht erstellt werden\n");
		return -1;		// Fehler bei Erstellung
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Infoleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}*/

/*	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Statusleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}
*/
	// ZU ERLEDIGEN: Entfernen, wenn Sie keine QuickInfos wünschen
//	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
//		CBRS_TOOLTIPS | CBRS_FLYBY);

	m_CPPToolTip.Create(this);

	///////////////////////////
	// Behaviour
	m_CPPToolTip.SetNotify();
	m_CPPToolTip.SetBehaviour(PPTOOLTIP_MULTIPLE_SHOW);

	CIniLoader* pIni = CIniLoader::GetInstance();
	AssertBotE(pIni);
	int nDelayTime = 750;
	pIni->ReadValue("Video", "TOOLTIPDELAY", nDelayTime);
	// Times (in ms)
	m_CPPToolTip.SetDelayTime(PPTOOLTIP_TIME_INITIAL, nDelayTime);	// nach 750ms wird es angezeigt
	m_CPPToolTip.SetDelayTime(PPTOOLTIP_TIME_AUTOPOP, 200000);		// how long it stays

	//m_CPPToolTip.SetDefaultSizes(FALSE);
	m_CPPToolTip.SetTransparency(20);
	m_CPPToolTip.SetColorBk(RGB(20,20,20));
	m_CPPToolTip.SetBorder(RGB(250,250,250), 1, 1);
	m_CPPToolTip.SetMaxTipWidth(300);
	// EFFECT_SOFTBUMP
	// EFFECT_DIAGSHADE
	//m_CPPToolTip.SetEffectBk(CPPDrawManager::EFFECT_SOFTBUMP,10);

	m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_ROUNDED_CX, 1);
	m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_ROUNDED_CY, 1);
	//m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_MARGIN_CX, 5);
	//m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_MARGIN_CY, 5);
	//m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_WIDTH_ANCHOR, 0);
	m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_HEIGHT_ANCHOR, 0);
	//m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_MARGIN_ANCHOR, 0);
	//m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_OFFSET_ANCHOR_CX, 0);
	//m_CPPToolTip.SetSize(CPPToolTip::PPTTSZ_OFFSET_ANCHOR_CY, 0);
	///////////////////////////

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	// Unterdrücken der Taskbar durch die Applikation
	cs.style &= ~0x00010000;	//ich weiss nicht warum - aber es klappt

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	// Hauptfenster aufteilen:
	// +---------------------------------------------------+
	// | CMenuChooseView       0 CGalaxyMenuView / CMainBaseViews         |
	// |             0								       |
	// |             0                                     |
	// |             0                                     |
	// |             0                                     |
	// |             0                                     |
	// |		     0		    						   |
	// |		     0	    							   |
	// |			 0   								   |
	// |			 0								       |
	// |			 0								       |
	// |			 0							     	   |
	// |             0                                     |
	// |ooooooooooooooooooooooooooooooooooooooooooooooooooo|
	// | CSmallInfoView       0 CBottomBaseViews           |
	// |             0                                     |
	// |             0                                     |
	// |             0                                     |
	// |             0                                     |
	// |             0	                                   |
	// +---------------------------------------------------+

	/*	Größe der CGalaxyMenuView mit ScrollBars:	1058 | 733
							 ohne Scrollbars:	1075 | 750
		Größe der CCMenuChooseView:	200  | 750
		Größe der CView3:	1075 | 249
		Größe der CCSmallInfoView:	200  | 249

		Alle Ansichten haben bei dieser angegebenen Größe nicht mehr den Rahmen der
		SplitterWindows. Also den, womit man die Größe der einzelnen Fenster verschieben
		konnte. Die Monitorauflösung muß 1280*1024 betragen.

		(Scrollbars haben also eine Dicke von 17 Pixeln bei einer Monitorauflösung von 1280*1024)
	*/

	CRect r;
	this->GetDesktopWindow()->GetClientRect(r);

	m_wndSplitter.CreateStatic(this, 2, 2, WS_CHILD | WS_VISIBLE);
	// Größen der Splitter festlegen. Der Mulitplikator kommt zustande, so dass es auf jeder Monitorauflösung trotzdem noch
	// das selbe Verhältnis hat. Deswegen können dort keine statischen Werte verwendet werden.
//	m_wndSplitter.SetRowInfo(0, r.bottom * VERT_PROPORTION, r.bottom * VERT_PROPORTION);
//	m_wndSplitter.SetColumnInfo(0, r.right * HORZ_PROPORTION, r.right * HORZ_PROPORTION);

/*	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CMenuChooseView),		CSize(r.right * 0.15625, r.bottom * 0.732421875), pContext);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CGalaxyMenuView),	CSize(r.Width() - r.right * 0.15625, r.bottom * 0.732421875), pContext);
	m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CSmallInfoView),		CSize(r.right * 0.15625, r.Height() - r.bottom * 0.732421875), pContext);
	m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CPlanetBottomView), CSize(r.Width() - r.right * 0.15625, r.Height() - r.bottom * 0.732421875), pContext);
*/

	// View hinzufügen

	// Hauptmenü
	m_wndSplitter.AddSwitchableView(m_wndSplitter.IdFromRowCol(0, 1), RUNTIME_CLASS(CStartMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , true , VIEWS::START_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::NEWGAME_VIEW, RUNTIME_CLASS(CNewGameView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::NEWGAME_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::CHOOSERACE_VIEW, RUNTIME_CLASS(CChooseRaceView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::CHOOSERACE_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::GALAXY_VIEW, RUNTIME_CLASS(CGalaxyMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::GALAXY_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::SYSTEM_VIEW, RUNTIME_CLASS(CSystemMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::SYSTEM_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::RESEARCH_VIEW, RUNTIME_CLASS(CResearchMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::RESEARCH_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::INTEL_VIEW, RUNTIME_CLASS(CIntelMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::INTEL_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::DIPLOMACY_VIEW, RUNTIME_CLASS(CDiplomacyMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::DIPLOMACY_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::TRADE_VIEW, RUNTIME_CLASS(CTradeMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::TRADE_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::EMPIRE_VIEW, RUNTIME_CLASS(CEmpireMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::EMPIRE_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::FLEET_VIEW, RUNTIME_CLASS(CFleetMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::FLEET_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::SHIPDESIGN_VIEW, RUNTIME_CLASS(CShipDesignMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::SHIPDESIGN_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::TRANSPORT_VIEW, RUNTIME_CLASS(CTransportMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , VIEWS::TRANSPORT_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::EVENT_VIEW, RUNTIME_CLASS(CEventMenuView),
		pContext, CRect(r.right, 0, r.Width(), r.bottom) , false , VIEWS::EVENT_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::COMBAT_VIEW, RUNTIME_CLASS(CCombatMenuView),
		pContext, CRect(r.right, 0, r.Width(), r.bottom) , false , VIEWS::COMBAT_VIEW);

	// kleine View unten/links
	m_wndSplitter.AddSwitchableView(m_wndSplitter.IdFromRowCol(1, 0), RUNTIME_CLASS(CSmallInfoView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , true);

	// Planeten/Schiffsansichtsmenü
	m_wndSplitter.AddSwitchableView(m_wndSplitter.IdFromRowCol(1, 1), RUNTIME_CLASS(CPlanetBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , true, VIEWS::PLANET_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::SHIP_BOTTOM_VIEW, RUNTIME_CLASS(CShipBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , VIEWS::SHIP_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::RESEARCH_BOTTOM_VIEW, RUNTIME_CLASS(CResearchBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , VIEWS::RESEARCH_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::INTEL_BOTTOM_VIEW, RUNTIME_CLASS(CIntelBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , VIEWS::INTEL_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::DIPLOMACY_BOTTOM_VIEW, RUNTIME_CLASS(CDiplomacyBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , VIEWS::DIPLOMACY_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::TRADE_BOTTOM_VIEW, RUNTIME_CLASS(CTradeBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , VIEWS::TRADE_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(VIEWS::SHIPDESIGN_BOTTOM_VIEW, RUNTIME_CLASS(CShipDesignBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , VIEWS::SHIPDESIGN_BOTTOM_VIEW);

	// linke Menüleiste
	m_wndSplitter.AddSwitchableView(m_wndSplitter.IdFromRowCol(0, 0), RUNTIME_CLASS(CMenuChooseView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , true, VIEWS::MENU_CHOOSE_VIEW);

	//FullScreenMainView(true);

	return TRUE;
//	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::SelectMainView(VIEWS::MAIN_VIEWS whichView, const CString& sRace)
{
	if (sRace.IsEmpty())
		return;

	AssertBotE(VIEWS::NULL_VIEW <= whichView && whichView <= VIEWS::COMBAT_VIEW);

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	if (pDoc->GetPlayersRaceID() == sRace)
	{
		resources::pClientWorker->SetSelectedViewForTo(sRace, whichView);
		// konnte in die neue View gewechselt werden
		if (m_wndSplitter.SwitchView(whichView, 0, 1))
		{
			switch (whichView)
			{
			case VIEWS::SYSTEM_VIEW:		SelectBottomView(VIEWS::PLANET_BOTTOM_VIEW);		break;
			case VIEWS::RESEARCH_VIEW:		SelectBottomView(VIEWS::RESEARCH_BOTTOM_VIEW);		break;
			case VIEWS::INTEL_VIEW:		SelectBottomView(VIEWS::INTEL_BOTTOM_VIEW);		break;
			case VIEWS::DIPLOMACY_VIEW:	SelectBottomView(VIEWS::DIPLOMACY_BOTTOM_VIEW);	break;
			case VIEWS::TRADE_VIEW:		SelectBottomView(VIEWS::TRADE_BOTTOM_VIEW);		break;
			case VIEWS::SHIPDESIGN_VIEW:	SelectBottomView(VIEWS::SHIPDESIGN_BOTTOM_VIEW);	break;
			case VIEWS::FLEET_VIEW:		SelectBottomView(VIEWS::SHIP_BOTTOM_VIEW);			break;
			case VIEWS::TRANSPORT_VIEW:	SelectBottomView(VIEWS::SHIP_BOTTOM_VIEW);			break;
			default:				SelectBottomView(VIEWS::PLANET_BOTTOM_VIEW);		break;
			}

			// Wenn wir irgendwie in eine andere View, außer der Galaxieansicht gelangen, dann dürfen wir keine Handelsroute
			// oder Ressourcenroute mehr zeichnen
			if (whichView != VIEWS::GALAXY_VIEW)
			{
				CGalaxyMenuView::IsDrawResourceRoute(false);
				CGalaxyMenuView::IsDrawTradeRoute(false);
			}
		}
	}
}

void CMainFrame::SelectStartMenuView(VIEWS::START_MENU_VIEWS whichView)
{
	AssertBotE(VIEWS::START_VIEW <= whichView && whichView <= VIEWS::NEWGAME_VIEW);
	m_wndSplitter.SwitchView(whichView, 0, 1);
}

void CMainFrame::SelectBottomView(VIEWS::BOTTOM_VIEWS viewID)
{
	AssertBotE(VIEWS::PLANET_BOTTOM_VIEW <= viewID && viewID <= VIEWS::SHIPDESIGN_BOTTOM_VIEW);
	m_wndSplitter.SwitchView(viewID, 1, 1);
}

short CMainFrame::GetActiveView(int paneRow, int paneCol) const
{
	const std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	CView* curView = dynamic_cast<CView*>(dynamic_cast<CMainFrame*>(AfxGetMainWnd())->m_wndSplitter.GetPane(paneRow, paneCol)); // get current view
	AssertBotE(curView);

	for (std::map<CWnd *, UINT>::const_iterator it = views->begin(); it != views->end(); ++it)
		if (it->first == curView)
			return it->second;
	return -1;
}

CView* CMainFrame::GetView(const CRuntimeClass* className) const
{
	const std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	for (std::map<CWnd *, UINT>::const_iterator it = views->begin(); it != views->end(); ++it)
		// finden sich darunter eine View der übergebenen Klasse
		if (it->first->IsKindOf(className))
			return (CView*)(it->first);
	return NULL;
}

void CMainFrame::InvalidateView(const CRuntimeClass* className)
{
	// Alle switchable Views durchiterieren
	std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); ++it)
	{
		// finden sich darunter eine View der übergebenen Klasse
		if (it->first->IsKindOf(className))
		{
			((CView*)(it->first))->Invalidate(FALSE);
			break;
		}
	}
}

void CMainFrame::InvalidateView(USHORT viewID)
{
	// Alle switchable Views durchiterieren
	std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); ++it)
	{
		// finden sich darunter eine View der übergebenen Klasse
		if (it->second == viewID)
		{
			((CView*)(it->first))->Invalidate(FALSE);
			break;
		}
	}
}

void CMainFrame::SetSubMenu(const CRuntimeClass* viewClassName, BYTE menuID)
{
	// Alle switchable Views durchiterieren
	std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); ++it)
	{
		// finden sich darunter eine View der übergebenen Klasse
		if (it->first->IsKindOf(viewClassName))
		{
			// die Art der View noch unterscheiden
			if (viewClassName->IsDerivedFrom(RUNTIME_CLASS(CMainBaseView)))
			{
				// Untermenü setzen
				((CMainBaseView*)(it->first))->SetSubMenu(menuID);
				break;
			}
			else if (viewClassName->IsDerivedFrom(RUNTIME_CLASS(CBottomBaseView)))
			{
				// Untermenü setzen
				((CBottomBaseView*)(it->first))->SetSubMenu(menuID);
				break;
			}
		}
	}
}

BYTE CMainFrame::GetSubMenu(const CRuntimeClass* viewClassName) const
{
	// Alle switchable Views durchiterieren
	const std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	for (std::map<CWnd *, UINT>::const_iterator it = views->begin(); it != views->end(); ++it)
	{
		// finden sich darunter eine View der übergebenen Klasse
		if (it->first->IsKindOf(viewClassName))
		{
			// die Art der View noch unterscheiden
			if (viewClassName->IsDerivedFrom(RUNTIME_CLASS(CMainBaseView)))
				return ((CMainBaseView*)(it->first))->GetSubMenu();
			else if (viewClassName->IsDerivedFrom(RUNTIME_CLASS(CBottomBaseView)))
				return ((CBottomBaseView*)(it->first))->GetSubMenu();
		}
	}
	return NULL;
}

void CMainFrame::FullScreenMainView(bool fullScreen)
{
	CRect r;
	this->GetDesktopWindow()->GetClientRect(r);

	// normalen Modus setzen
	if (!fullScreen)
	{
		m_wndSplitter.SetRowInfo(0, r.bottom * VERT_PROPORTION, r.bottom * VERT_PROPORTION);
		m_wndSplitter.SetColumnInfo(0, r.right * HORZ_PROPORTION, r.right * HORZ_PROPORTION);
	}
	// Fullscreenmodus für die Mainview setzen
	else
	{
		m_wndSplitter.SetRowInfo(0, r.Height(), r.Height());
		m_wndSplitter.SetColumnInfo(0, 0, 0);
	}
	m_wndSplitter.RecalcLayout();
}

LRESULT CMainFrame::UpdateViews(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	MYTRACE("general")(MT::LEVEL_INFO, "Getting Message to UpdateViews...");

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	// Views ihre Arbeiten zu jeder neuen Runde machen lassen
	pDoc->DoViewWorkOnNewRound();
	MYTRACE("general")(MT::LEVEL_INFO, "Updating all Views done\n");

	return TRUE;
}

LRESULT CMainFrame::ShowCombatView(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	MYTRACE("general")(MT::LEVEL_INFO, "Getting Message to Show CombatView...");

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	// Combat View anzeigen
	FullScreenMainView(true);
	SelectMainView(VIEWS::COMBAT_VIEW, pDoc->GetPlayersRaceID());
	// wurde Rundenende geklickt zurücksetzen
	resources::pClientWorker->SetToEmpireViewFor(*pDoc->GetPlayersRace());
	pDoc->m_bRoundEndPressed = false;
	pDoc->m_bDataReceived = true;

	// Zuletzt gedrückten Button zurücksetzen
	CCombatMenuView* pView = ((CCombatMenuView*)GetView(RUNTIME_CLASS(CCombatMenuView)));
	pView->OnNewRound();

	pDoc->UpdateAllViews(NULL);

	MYTRACE("general")(MT::LEVEL_INFO, "Showing CombatView\n");

	return TRUE;
}

LRESULT CMainFrame::InitViews(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	MYTRACE("general")(MT::LEVEL_INFO, "Getting Message to InitViews...");

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	// alle rassenabhängigen Grafiken in jeder View laden
	pDoc->LoadViewGraphics();

	MYTRACE("general")(MT::LEVEL_INFO, "Init all Views done\n");
	return TRUE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	m_CPPToolTip.RelayEvent(pMsg);

	return CFrameWnd::PreTranslateMessage(pMsg);
}

bool CMainFrame::AddToTooltip( CWnd* pWnd, const CString& sTip )
{
	if (!pWnd)
		return false;

	if (!m_CPPToolTip.GetSafeHwnd())
	{
		MYTRACE("general")(MT::LEVEL_WARNING, "AddToTooltip not possible: tooltip hwnd is null\n");
		return false;
	}

	m_CPPToolTip.AddTool(pWnd, sTip);
	return true;
}

void CMainFrame::NotifyCPPTooltip( NMHDR* pNMHDR, LRESULT* result )
{
	*result = 0;
	NM_PPTOOLTIP_DISPLAY * pNotify = (NM_PPTOOLTIP_DISPLAY*)pNMHDR;

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);
	if (pDoc->m_bDataReceived)
	{
		CWnd* pWnd = CWnd::FromHandle(pNotify->hwndTool);
		// Galaxieview
		if  (pWnd->IsKindOf(RUNTIME_CLASS(CGalaxyMenuView)))
		{
			pNotify->ti->sTooltip = ((CGalaxyMenuView*)pWnd)->CreateTooltip();
			return;
		}
		// untere View
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CBottomBaseView)))
		{
			pNotify->ti->sTooltip = ((CBottomBaseView*)pWnd)->CreateTooltip();
			return;
		}
		// Hauptviews
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CMainBaseView)))
		{
			pNotify->ti->sTooltip = ((CMainBaseView*)pWnd)->CreateTooltip();
			return;
		}
	}
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	CRect rect;
	GetClientRect(&rect);

	pDC->FillSolidRect(rect, RGB(0,0,0));
	pDC->SetTextColor(RGB(200,200,200));
	pDC->DrawText("§loading game data - please wait...", rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	return CFrameWnd::OnEraseBkgnd(pDC);
}
