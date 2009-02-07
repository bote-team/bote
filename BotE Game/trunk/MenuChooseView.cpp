// CMenuChooseView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "botf2.h"
#include "GalaxyMenuView.h"
#include "EmpireMenuView.h"
#include "MenuChooseView.h"
#include "PlanetBottomView.h"
#include "Botf2Doc.h"
#include "BotEClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMenuChooseView

IMPLEMENT_DYNCREATE(CMenuChooseView, CView)

BEGIN_MESSAGE_MAP(CMenuChooseView, CView)
	//{{AFX_MSG_MAP(CMenuChooseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

CMenuChooseView::CMenuChooseView()
{
	m_RoundEnd = NULL;	
}

CMenuChooseView::~CMenuChooseView()
{
	for (int i = 0; i < m_Buttons.GetSize(); i++)
	{
		delete m_Buttons[i];
		m_Buttons[i] = 0;
	}
	m_Buttons.RemoveAll();
	if (m_RoundEnd)
		delete m_RoundEnd;
}

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CMenuChooseView 

void CMenuChooseView::OnDraw(CDC* pDC)
{
	// ZU ERLEDIGEN: Code zum Zeichnen hier einfügen
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	
	CRect r;
	r.SetRect(0, 0, m_TotalSize.cx, m_TotalSize.cy);
		
	// Doublebuffering wird initialisiert
	CRect client;
	GetClientRect(&client);
	Graphics doubleBuffer(pDC->GetSafeHdc());
	doubleBuffer.SetSmoothingMode(SmoothingModeHighQuality);
	doubleBuffer.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	doubleBuffer.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Bitmap bmp(client.Width(), client.Height());
	Graphics* g = Graphics::FromImage(&bmp);
	g->Clear(Color::Black);
	g->SetSmoothingMode(SmoothingModeHighQuality);
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g->SetPixelOffsetMode(PixelOffsetModeHighQuality);
	g->ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);

	// Die Rassenspezifischen Styles laden und zeichnen
	Color color;
	CFontLoader::CreateGDIFont(pDoc->GetPlayersRace(), 3, fontName, fontSize);
	CFontLoader::GetGDIFontColor(pDoc->GetPlayersRace(), 2, color);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontBrush.SetColor(color);
	
	// Grafiken zeichnen
	Bitmap* graphic = NULL;
	CString prefix;
	prefix.Format("RACE%d_PREFIX", pDoc->GetPlayersRace());
	prefix = CResourceManager::GetString(prefix);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + prefix + "menuV2.png");
	if (graphic)
		g->DrawImage(graphic, 0, 0, 200, 750);
	
	// Buttons zeichnen
	// Den Rundenendebutton
	if (pDoc->m_bRoundEndPressed)
		m_RoundEnd->SetState(2);
	else if (m_RoundEnd->GetState() != 1)
		m_RoundEnd->SetState(0);
	m_RoundEnd->DrawButton(*g, pDoc->GetGraphicPool(), Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);
	// Wenn wir im Systemmenü sind, prüfen, ob der dazugehörige Button auch inaktiv ist.
	// müssen das machen, da wir auch mittels Doppelklick auf den Sektor in die Systemansicht gelangen können
	// oder in die Forschungs oder Imperiumsansicht
	for (int j = 0; j < m_Buttons.GetSize(); j++)
		if (pDoc->GetMainFrame()->GetActiveView(0, 1) == j+1 && m_Buttons.GetAt(j)->GetState() != 2)
		{
			for (int i = 0; i < m_Buttons.GetSize(); i++)
				if (m_Buttons.GetAt(i)->GetState() == 2)
					m_Buttons.GetAt(i)->SetState(0);
			m_Buttons.GetAt(j)->SetState(2);
			break;
		}
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, pDoc->GetGraphicPool(), Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);

		//********************************************************************************
	// Hier testweise paar Informationen zum Imperium
	CString s;
	fontBrush.SetColor(Color(200,200,200));
	
	// Die aktuelle Runde darstellen, schauen ob schon gedrückt oder nicht
	s.Format("%s %i",CResourceManager::GetString("ROUND"), pDoc->GetCurrentRound());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+20, 50, m_TotalSize.cx-40, 30), &fontFormat, &fontBrush);
	
	CFontLoader::CreateGDIFont(pDoc->GetPlayersRace(), 2, fontName, fontSize);
	CFontLoader::GetGDIFontColor(pDoc->GetPlayersRace(), 3, color);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontBrush.SetColor(color);

	s.Format("%s: %d",CResourceManager::GetString("NEWS"), pDoc->GetEmpire(pDoc->GetPlayersRace())->GetMessages()->GetSize());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+30, 140, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	
	s.Format("%s:",CResourceManager::GetString("LATINUM"), pDoc->GetEmpire(pDoc->GetPlayersRace())->GetLatinum());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+30, 90, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	s.Format("%i",pDoc->GetEmpire(pDoc->GetPlayersRace())->GetLatinum());
	fontFormat.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+30, 90, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	
	s.Format("%s:",CResourceManager::GetString("CHANGE"));
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+30, 115, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	
	// Latinumänderung zeichnen
	fontFormat.SetAlignment(StringAlignmentFar);
	if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetLatinumChange() >= 0)
	{	
		fontBrush.SetColor(Color(0,200,0));
		s.Format("+%i",pDoc->GetEmpire(pDoc->GetPlayersRace())->GetLatinumChange());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+30, 115, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);		
	}
	else
	{
		fontBrush.SetColor(Color(200,0,0));
		s.Format("%i",pDoc->GetEmpire(pDoc->GetPlayersRace())->GetLatinumChange());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+30, 115, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	}
	
	// Sternzeit anzeigen
	fontBrush.SetColor(color);
	fontFormat.SetAlignment(StringAlignmentCenter);
	s.Format("%s: %.1lf",CResourceManager::GetString("STARDATE"), pDoc->m_fStardate);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+20, r.bottom-65, m_TotalSize.cx-40, 25), &fontFormat, &fontBrush);
	//********************************************************************************	

	doubleBuffer.DrawImage(&bmp, client.left, client.top, client.right, client.bottom);
	delete g;
}

void CMenuChooseView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	m_TotalSize.cx = 200;
	m_TotalSize.cy = 750;
	
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	m_LastSystem = CPoint(-1,-1);
	
	// alle Buttons in der View anlegen (erstmal 7) und Grafiken laden
	switch(pDoc->GetPlayersRace())
	{
	case HUMAN:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big.png";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_bigi.png";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_biga.png";
			m_Buttons.Add(new CMyButton(CPoint(20,360), CSize(160,40), CResourceManager::GetString("BTN_GALAXY")+"   ", fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,405), CSize(160,40), CResourceManager::GetString("BTN_SYSTEM")+"   ", fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,450), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH")+"   ", fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,495), CSize(160,40), CResourceManager::GetString("BTN_SECURITY")+"   ", fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,540), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY")+"   ", fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,585), CSize(160,40), CResourceManager::GetString("BTN_TRADE")+"   ", fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,630), CSize(160,40), CResourceManager::GetString("BTN_EMPIRE")+"   ", fileN, fileI, fileA));
			// Rundenendebutton
			fileN = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_roundend.png";
			fileI = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_roundendi.png";
			fileA = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_roundenda.png";
			m_RoundEnd = new CMyButton(CPoint(20,5), CSize(160,40), CResourceManager::GetString("BTN_ROUNDEND"), fileN, fileI, fileA);
			break;
		}
	case FERENGI:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(20,360), CSize(160,40), CResourceManager::GetString("BTN_GALAXY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,405), CSize(160,40), CResourceManager::GetString("BTN_SYSTEM"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,450), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,495), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,540), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,585), CSize(160,40), CResourceManager::GetString("BTN_TRADE"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,630), CSize(160,40), CResourceManager::GetString("BTN_EMPIRE"), fileN, fileI, fileA));
			// Rundenendebutton
			fileN = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "button.jpg";
			fileI = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttoni.jpg";
			fileA = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttona.jpg";
			m_RoundEnd = new CMyButton(CPoint(20,5), CSize(160,40), CResourceManager::GetString("BTN_ROUNDEND"), fileN, fileI, fileA);
			break;
		}
	case KLINGON:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(20,360), CSize(160,40), CResourceManager::GetString("BTN_GALAXY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,405), CSize(160,40), CResourceManager::GetString("BTN_SYSTEM"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,450), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,495), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,540), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,585), CSize(160,40), CResourceManager::GetString("BTN_TRADE"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,630), CSize(160,40), CResourceManager::GetString("BTN_EMPIRE"), fileN, fileI, fileA));
			// Rundenendebutton
			fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button_roundend.jpg";
			fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button_roundendi.jpg";
			fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button_roundenda.jpg";
			m_RoundEnd = new CMyButton(CPoint(20,5), CSize(160,40), CResourceManager::GetString("BTN_ROUNDEND"), fileN, fileI, fileA);
			break;
		}
	case ROMULAN:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(20,360), CSize(160,40), CResourceManager::GetString("BTN_GALAXY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,405), CSize(160,40), CResourceManager::GetString("BTN_SYSTEM"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,450), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,495), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,540), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,585), CSize(160,40), CResourceManager::GetString("BTN_TRADE"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,630), CSize(160,40), CResourceManager::GetString("BTN_EMPIRE"), fileN, fileI, fileA));
			// Rundenendebutton
			fileN = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "button.jpg";
			fileI = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttoni.jpg";
			fileA = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttona.jpg";
			m_RoundEnd = new CMyButton(CPoint(20,5), CSize(160,40), CResourceManager::GetString("BTN_ROUNDEND"), fileN, fileI, fileA);
			break;
		}
	case CARDASSIAN:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(20,360), CSize(160,40), CResourceManager::GetString("BTN_GALAXY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,405), CSize(160,40), CResourceManager::GetString("BTN_SYSTEM"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,450), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,495), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,540), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,585), CSize(160,40), CResourceManager::GetString("BTN_TRADE"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,630), CSize(160,40), CResourceManager::GetString("BTN_EMPIRE"), fileN, fileI, fileA));
			// Rundenendebutton
			fileN = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "button.jpg";
			fileI = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttoni.jpg";
			fileA = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttona.jpg";
			m_RoundEnd = new CMyButton(CPoint(20,5), CSize(160,40), CResourceManager::GetString("BTN_ROUNDEND"), fileN, fileI, fileA);
			break;
		}
	case DOMINION:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(20,360), CSize(160,40), CResourceManager::GetString("BTN_GALAXY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,405), CSize(160,40), CResourceManager::GetString("BTN_SYSTEM"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,450), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,495), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,540), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,585), CSize(160,40), CResourceManager::GetString("BTN_TRADE"), fileN, fileI, fileA));
			m_Buttons.Add(new CMyButton(CPoint(20,630), CSize(160,40), CResourceManager::GetString("BTN_EMPIRE"), fileN, fileI, fileA));
			// Rundenendebutton
			fileN = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "button.jpg";
			fileI = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttoni.jpg";
			fileA = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttona.jpg";
			m_RoundEnd = new CMyButton(CPoint(20,5), CSize(160,40), CResourceManager::GetString("BTN_ROUNDEND"), fileN, fileI, fileA);
			break;
		}
	}	
}

BOOL CMenuChooseView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CMenuChooseView

#ifdef _DEBUG
void CMenuChooseView::AssertValid() const
{
	CView::AssertValid();
}

void CMenuChooseView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CMenuChooseView 

void CMenuChooseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);
}

void CMenuChooseView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
		
	CalcLogicalPoint(point);
	CGalaxyMenuView::SetMoveShip(FALSE);
	
	if (m_RoundEnd->ClickedOnButton(point) && !pDoc->m_bRoundEndPressed)
	{
		pDoc->m_bRoundEndPressed = TRUE;
		CRect r = m_RoundEnd->GetRect();
		CalcDeviceRect(r);
		InvalidateRect(r, FALSE);
		pDoc->m_bDataReceived = FALSE;
		pDoc->m_pSoundManager->StopMessages(TRUE);
		client.EndOfRound(pDoc);
	}
	short button = -1;
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		if (m_Buttons.GetAt(i)->ClickedOnButton(point))
		{
			button = i;
			// Button auf inaktiv schalten, anderen inaktiven Button auf normal schalten
			for (int j = 0; j < m_Buttons.GetSize(); j++)
				if (m_Buttons.GetAt(j)->GetState() == 2)
				{
					m_Buttons.GetAt(j)->SetState(0);
					CRect r = m_Buttons.GetAt(j)->GetRect();
					CalcDeviceRect(r);
					InvalidateRect(r, FALSE);
				}
			m_Buttons.GetAt(i)->SetState(2);
			CRect r = m_Buttons.GetAt(i)->GetRect();
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
			break;
		}
	// Galaxiebutton
	if (button == 0)
		pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pDoc->GetPlayersRace());
	// Systembutton
	else if (button == 1)
	{
		// System suchen, welches dem Spieler auch gehört
		if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetOwnerOfSystem() != pDoc->GetPlayersRace())
		{
			 if (m_LastSystem != CPoint(-1,-1))
				 pDoc->SetKO(m_LastSystem.x, m_LastSystem.y);
			 else
			 {
				 for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
					 for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
						 if (pDoc->m_Sector[x][y].GetSunSystem() == TRUE
							 && pDoc->m_System[x][y].GetOwnerOfSystem() == pDoc->GetPlayersRace())
							{
								pDoc->SetKO(x,y);
								break;
							}
			 }
		}
		if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetOwnerOfSystem() == pDoc->GetPlayersRace() &&
			pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetSunSystem() == TRUE)
		{			
			pDoc->GetMainFrame()->SelectMainView(SYSTEM_VIEW, pDoc->GetPlayersRace());
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
			m_LastSystem = pDoc->GetKO();
		}
	}
	// Forschungsbutton
	else if (button == 2)
		pDoc->GetMainFrame()->SelectMainView(RESEARCH_VIEW, pDoc->GetPlayersRace());
	// Geheimdienstbutton
	else if (button == 3)
		pDoc->GetMainFrame()->SelectMainView(INTEL_VIEW, pDoc->GetPlayersRace());
	// Diplomatiebutton
	else if (button == 4)
		pDoc->GetMainFrame()->SelectMainView(DIPLOMACY_VIEW, pDoc->GetPlayersRace());
	// Handelsbutton
	else if (button == 5)
		pDoc->GetMainFrame()->SelectMainView(TRADE_VIEW, pDoc->GetPlayersRace());		
	// Imperiumsbutton
	else if (button == 6)
		pDoc->GetMainFrame()->SelectMainView(EMPIRE_VIEW, pDoc->GetPlayersRace());
	CView::OnLButtonUp(nFlags, point);
}

void CMenuChooseView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	CalcLogicalPoint(point);

	// Rundenendebutton
	if (m_RoundEnd->ClickedOnButton(point))
	{
		if (m_RoundEnd->Activate())
		{
			pDoc->m_pSoundManager->PlaySound(SNDMGR_SOUND_MAINMENU);
			CRect r = m_RoundEnd->GetRect();
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
	}
	else
		if (m_RoundEnd->Deactivate())
		{
			CRect r = m_RoundEnd->GetRect();
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
// restlichen Buttons
	for (int i = 0; i < m_Buttons.GetSize(); i++)
	{
		if (m_Buttons.GetAt(i)->ClickedOnButton(point))
		{
			if (m_Buttons.GetAt(i)->Activate())
			{
				pDoc->m_pSoundManager->PlaySound(SNDMGR_SOUND_MAINMENU);
				CRect r = m_Buttons.GetAt(i)->GetRect();
				CalcDeviceRect(r);
				InvalidateRect(r, FALSE);
			}
		}
		else
			if (m_Buttons.GetAt(i)->Deactivate())
			{
				CRect r = m_Buttons.GetAt(i)->GetRect();
				CalcDeviceRect(r);
				InvalidateRect(r, FALSE);
			}
	}
	CView::OnMouseMove(nFlags, point);
}

void CMenuChooseView::CalcLogicalPoint(CPoint &point)
{
	CRect client;
	GetClientRect(&client);
	
	point.x *= (float)m_TotalSize.cx / (float)client.Width();
	point.y *= (float)m_TotalSize.cy / (float)client.Height();	
}

void CMenuChooseView::CalcDeviceRect(CRect &rect)
{
	CRect client;
	GetClientRect(&client);

	CPoint p1 = rect.TopLeft();
	p1.x *= (float)client.Width() / (float)m_TotalSize.cx;
	p1.y *= (float)client.Height() / (float)m_TotalSize.cy;
	
	CPoint p2 = rect.BottomRight();
	p2.x *= (float)client.Width() / (float)m_TotalSize.cx;
	p2.y *= (float)client.Height() / (float)m_TotalSize.cy;
	rect.SetRect(p1, p2);
}

void CMenuChooseView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default	
	CView::OnRButtonUp(nFlags, point);
}
