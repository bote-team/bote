// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "botf2.h"

#include "MainFrm.h"
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
#include "SmallInfoView.h"

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
	//}}AFX_MSG_MAP
//	ON_WM_KEYDOWN()
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
}

CMainFrame::~CMainFrame()
{
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

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.
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


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
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
	// | CSmallInfoView       0 CBottomBaseViews                    |
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
	m_wndSplitter.AddSwitchableView(m_wndSplitter.IdFromRowCol(0, 1), RUNTIME_CLASS(CGalaxyMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , true , GALAXY_VIEW);
	m_wndSplitter.AddSwitchableView(SYSTEM_VIEW, RUNTIME_CLASS(CSystemMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , SYSTEM_VIEW);
	m_wndSplitter.AddSwitchableView(RESEARCH_VIEW, RUNTIME_CLASS(CResearchMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , RESEARCH_VIEW);
	m_wndSplitter.AddSwitchableView(INTEL_VIEW, RUNTIME_CLASS(CIntelMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , INTEL_VIEW);
	m_wndSplitter.AddSwitchableView(DIPLOMACY_VIEW, RUNTIME_CLASS(CDiplomacyMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , DIPLOMACY_VIEW);
	m_wndSplitter.AddSwitchableView(TRADE_VIEW, RUNTIME_CLASS(CTradeMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , TRADE_VIEW);
	m_wndSplitter.AddSwitchableView(EMPIRE_VIEW, RUNTIME_CLASS(CEmpireMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , EMPIRE_VIEW);
	m_wndSplitter.AddSwitchableView(FLEET_VIEW, RUNTIME_CLASS(CFleetMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , FLEET_VIEW);
	m_wndSplitter.AddSwitchableView(SHIPDESIGN_VIEW, RUNTIME_CLASS(CShipDesignMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , SHIPDESIGN_VIEW);
	m_wndSplitter.AddSwitchableView(TRANSPORT_VIEW, RUNTIME_CLASS(CTransportMenuView),
		pContext, CRect(r.right * HORZ_PROPORTION, 0, r.Width(), r.bottom * VERT_PROPORTION) , false , TRANSPORT_VIEW);
	m_wndSplitter.AddSwitchableView(EVENT_VIEW, RUNTIME_CLASS(CEventMenuView),
		pContext, CRect(r.right, 0, r.Width(), r.bottom) , false , EVENT_VIEW);

	// kleine View unten/links
	m_wndSplitter.AddSwitchableView(m_wndSplitter.IdFromRowCol(1, 0), RUNTIME_CLASS(CSmallInfoView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , true);

	// Planeten/Schiffsansichtsmenü
	m_wndSplitter.AddSwitchableView(m_wndSplitter.IdFromRowCol(1, 1), RUNTIME_CLASS(CPlanetBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , true, PLANET_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(SHIP_BOTTOM_VIEW, RUNTIME_CLASS(CShipBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , SHIP_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(RESEARCH_BOTTOM_VIEW, RUNTIME_CLASS(CResearchBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , RESEARCH_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(INTEL_BOTTOM_VIEW, RUNTIME_CLASS(CIntelBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , INTEL_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(DIPLOMACY_BOTTOM_VIEW, RUNTIME_CLASS(CDiplomacyBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , DIPLOMACY_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(TRADE_BOTTOM_VIEW, RUNTIME_CLASS(CTradeBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , TRADE_BOTTOM_VIEW);
	m_wndSplitter.AddSwitchableView(SHIPDESIGN_BOTTOM_VIEW, RUNTIME_CLASS(CShipDesignBottomView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , false , SHIPDESIGN_BOTTOM_VIEW);

	// linke Menüleiste
	m_wndSplitter.AddSwitchableView(m_wndSplitter.IdFromRowCol(0, 0), RUNTIME_CLASS(CMenuChooseView),
		pContext, CRect(0, 0, r.Width(), r.Height()) , true);

	FullScreenMainView(false);

	return TRUE;
//	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::SelectMainView(USHORT whichView, const CString& sRace)
{
	if (sRace.IsEmpty())
		return;
//	if (m_iSelectedView[race] != whichView)
	{
//		m_iSelectedView[race] = whichView;
		if (((CBotf2App*)AfxGetApp())->GetDocument()->GetPlayersRace()->GetRaceID() == sRace)
		{
			// konnte in die neue View gewechselt werden
			if (m_wndSplitter.SwitchView(whichView, 0, 1))
			{
				switch (whichView)
				{
				case SYSTEM_VIEW:		SelectBottomView(PLANET_BOTTOM_VIEW); break;
				case RESEARCH_VIEW:		SelectBottomView(RESEARCH_BOTTOM_VIEW); break;
				case INTEL_VIEW:		SelectBottomView(INTEL_BOTTOM_VIEW); break;
				case DIPLOMACY_VIEW:	SelectBottomView(DIPLOMACY_BOTTOM_VIEW); break;
				case TRADE_VIEW:		SelectBottomView(TRADE_BOTTOM_VIEW); break;
				case SHIPDESIGN_VIEW:	SelectBottomView(SHIPDESIGN_BOTTOM_VIEW); break;
				case FLEET_VIEW:		SelectBottomView(SHIP_BOTTOM_VIEW); break;
				default:				SelectBottomView(PLANET_BOTTOM_VIEW); break;
				}

				// Wenn wir irgendwie in eine andere View, außer der Galaxieansicht gelangen, dann dürfen wir keine Handelsroute
				// oder Ressourcenroute mehr zeichnen
				if (whichView != GALAXY_VIEW)
				{
					CGalaxyMenuView::IsDrawResourceRoute(false);
					CGalaxyMenuView::IsDrawTradeRoute(false);
				}
			}
		}
	}
}

void CMainFrame::SelectBottomView(USHORT viewID)
{
	m_wndSplitter.SwitchView(viewID, 1, 1);	
}

short CMainFrame::GetActiveView(int paneRow, int paneCol) const
{
	const std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	CView* curView = (CView*)((CMainFrame*)AfxGetMainWnd())->m_wndSplitter.GetPane(paneRow, paneCol); // get current view
	ASSERT(curView);

	for (std::map<CWnd *, UINT>::const_iterator it = views->begin(); it != views->end(); it++)
		if (it->first == curView)
			return it->second;
	return -1;
}

CView* CMainFrame::GetView(const CRuntimeClass* className) const
{
	const std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	for (std::map<CWnd *, UINT>::const_iterator it = views->begin(); it != views->end(); it++)
		// finden sich darunter eine View der übergebenen Klasse
		if (it->first->IsKindOf(className))
			return (CView*)(it->first);
	return NULL;
}

void CMainFrame::InvalidateView(const CRuntimeClass* className)
{
/*	CView *pView = NULL;
	
	POSITION pos = GetFirstViewPosition();
	do
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(className))
		{
			pView->Invalidate(FALSE);
			break;
		}
	} while (pView != NULL);
*/
	// Alle switchable Views durchiterieren
	std::map<CWnd *, UINT>* views = &m_wndSplitter.views;
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); it++)
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
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); it++)
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
	for (std::map<CWnd *, UINT>::iterator it = views->begin(); it != views->end(); it++)
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
	for (std::map<CWnd *, UINT>::const_iterator it = views->begin(); it != views->end(); it++)
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