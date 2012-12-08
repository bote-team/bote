// CMenuChooseView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "botf2.h"
#include "GalaxyMenuView.h"
#include "EmpireMenuView.h"
#include "MenuChooseView.h"
#include "PlanetBottomView.h"
#include "Botf2Doc.h"
#include "MainFrm.h"
#include "BotEClient.h"
#include "Races\RaceController.h"
#include "IniLoader.h"
#include "General/ResourceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMenuChooseView
CMajor* CMenuChooseView::m_pPlayersRace = NULL;

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

CMenuChooseView::CMenuChooseView() :
	m_LastSystem(-1, -1)
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
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// TEST-OPTION (only for Debug)
	// automatisch bis zu einer gewissen Runde durchzuklicken falls per Startparamter aktiviert
	if (const CCommandLineParameters* const clp = resources::pClp)
	{
		int nAutoTurns = clp->GetAutoTurns();
		if (!pDoc->m_bRoundEndPressed && pDoc->GetCurrentRound() < nAutoTurns)
		{
			pDoc->m_bRoundEndPressed = true;
			CSoundManager::GetInstance()->StopMessages(TRUE);
			client.EndOfRound(pDoc);
		}
	}

	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);

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
	g->SetSmoothingMode(SmoothingModeHighQuality);
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g->SetPixelOffsetMode(PixelOffsetModeHighQuality);
	g->ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);
	g->Clear(Color::Black);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);

	// Die Rassenspezifischen Styles laden und zeichnen
	Color color;
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontBrush.SetColor(color);
	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);

	// Grafiken zeichnen
	CString prefix = pMajor->GetPrefix();
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + prefix + "menuV2.bop");
	if (graphic)
		g->DrawImage(graphic, 0, 0, 200, 750);

	// Buttons zeichnen
	// Den Rundenendebutton
	if (pDoc->m_bRoundEndPressed)
		m_RoundEnd->SetState(2);
	else if (m_RoundEnd->GetState() != 1)
		m_RoundEnd->SetState(0);
	m_RoundEnd->DrawButton(*g, pDoc->GetGraphicPool(), Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
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
		m_Buttons.GetAt(i)->DrawButton(*g, pDoc->GetGraphicPool(), Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);

		//********************************************************************************
	// Hier testweise paar Informationen zum Imperium
	CString s;
	fontBrush.SetColor(markColor);

	// Die aktuelle Runde darstellen, schauen ob schon gedrückt oder nicht
	s.Format("%s %i",CResourceManager::GetString("ROUND"), pDoc->GetCurrentRound());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+20, 50, m_TotalSize.cx-40, 30), &fontFormat, &fontBrush);

	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	CFontLoader::GetGDIFontColor(pMajor, 3, color);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontBrush.SetColor(color);

	s.Format("%s:",CResourceManager::GetString("CREDITS"), pMajor->GetEmpire()->GetCredits());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 90, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	s.Format("%i",pMajor->GetEmpire()->GetCredits());
	fontFormat.SetAlignment(StringAlignmentFar);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 90, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);

	s.Format("%s:",CResourceManager::GetString("CHANGE"));
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 115, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentFar);
	if (pMajor->GetEmpire()->GetCreditsChange() >= 0)
	{
		fontBrush.SetColor(Color(0,200,0));
		s.Format("+%i", pMajor->GetEmpire()->GetCreditsChange());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 115, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	}
	else
	{
		fontBrush.SetColor(Color(200,0,0));
		s.Format("%i",pMajor->GetEmpire()->GetCreditsChange());
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 115, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	}
	fontBrush.SetColor(markColor);
	s = CResourceManager::GetString("SHIPS");
	fontFormat.SetAlignment(StringAlignmentCenter);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 140, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);

	fontBrush.SetColor(color);
	s.Format("%s:",CResourceManager::GetString("SHIPCOSTS"));
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 165, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentFar);
	s.Format("%i",pMajor->GetEmpire()->GetShipCosts());
	if (pMajor->GetEmpire()->GetShipCosts() > pMajor->GetEmpire()->GetPopSupportCosts())
		fontBrush.SetColor(Color(200,0,0));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 165, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);

	fontBrush.SetColor(color);
	s.Format("%s:",CResourceManager::GetString("POPSUPPORT"));
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 190, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentFar);
	s.Format("%i",pMajor->GetEmpire()->GetPopSupportCosts());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 190, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);

	fontFormat.SetAlignment(StringAlignmentNear);
	s.Format("%s:",CResourceManager::GetString("NEWS"));
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 240, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentFar);
	s.Format("%d",pMajor->GetEmpire()->GetMessages()->GetSize());
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 240, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);

	// Bewertung Gesamt anzeigen
	float fMark = 0.0f;
	int nPlace = 1;
	float fValue, fAverage, fFirst, fLast;
	CString sRaceID = pMajor->GetRaceID();
	CStatistics *cs = pDoc->GetStatistics();

	cs->GetDemographicsBSP(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	fMark += (float)(nPlace);

	cs->GetDemographicsProductivity(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	fMark += (float)(nPlace);

	cs->GetDemographicsMilitary(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	fMark += (float)(nPlace);

	cs->GetDemographicsResearch(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	fMark += (float)(nPlace);

	cs->GetDemographicsMoral(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	fMark += (float)(nPlace);

	fMark /= 5;
	s.Format("%s:",CResourceManager::GetString("RATING"));
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 290, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);

	fontFormat.SetAlignment(StringAlignmentFar);
	s.Format("%.1lf",fMark);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+30, 290, m_TotalSize.cx-60, 25), &fontFormat, &fontBrush);


	// Sternzeit anzeigen
	fontBrush.SetColor(color);
	fontFormat.SetAlignment(StringAlignmentCenter);
	s.Format("%s: %.1lf",CResourceManager::GetString("STARDATE"), pDoc->m_fStardate);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(r.left+20, r.bottom-65, m_TotalSize.cx-40, 25), &fontFormat, &fontBrush);
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

	m_LastSystem = CPoint(-1,-1);

	bool bHideMenu;
	CIniLoader::GetInstance()->ReadValue("Control", "HIDEMENUBAR", bHideMenu);
	if (bHideMenu)
		GetTopLevelFrame()->SetMenuBarState(AFX_MBS_HIDDEN);
}

/// Funktion lädt die rassenspezifischen Grafiken.
void  CMenuChooseView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CString sID = pDoc->GetPlayersRaceID();
	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(sID));
	ASSERT(pMajor);

	// alle Buttons in der View anlegen (erstmal 7) und Grafiken laden
	CString sPrefix = pMajor->GetPrefix();

	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	m_Buttons.Add(new CMyButton(CPoint(20,360), CSize(160,40), CResourceManager::GetString("BTN_GALAXY"), fileN, fileI, fileA));
	m_Buttons.Add(new CMyButton(CPoint(20,405), CSize(160,40), CResourceManager::GetString("BTN_SYSTEM"), fileN, fileI, fileA));
	m_Buttons.Add(new CMyButton(CPoint(20,450), CSize(160,40), CResourceManager::GetString("BTN_RESEARCH"), fileN, fileI, fileA));
	m_Buttons.Add(new CMyButton(CPoint(20,495), CSize(160,40), CResourceManager::GetString("BTN_SECURITY"), fileN, fileI, fileA));
	m_Buttons.Add(new CMyButton(CPoint(20,540), CSize(160,40), CResourceManager::GetString("BTN_DIPLOMACY"), fileN, fileI, fileA));
	m_Buttons.Add(new CMyButton(CPoint(20,585), CSize(160,40), CResourceManager::GetString("BTN_TRADE"), fileN, fileI, fileA));
	m_Buttons.Add(new CMyButton(CPoint(20,630), CSize(160,40), CResourceManager::GetString("BTN_EMPIRE"), fileN, fileI, fileA));
	// Rundenendebutton
	fileN = "Other\\" + sPrefix + "button_roundend.bop";
	fileI = "Other\\" + sPrefix + "button_roundendi.bop";
	fileA = "Other\\" + sPrefix + "button_roundenda.bop";
	m_RoundEnd = new CMyButton(CPoint(20,5), CSize(160,40), CResourceManager::GetString("BTN_ROUNDEND"), fileN, fileI, fileA);
}

BOOL CMenuChooseView::OnEraseBkgnd(CDC* /*pDC*/)
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
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);
	CGalaxyMenuView::SetMoveShip(FALSE);

	if (m_RoundEnd->ClickedOnButton(point) && !pDoc->m_bRoundEndPressed)
	{
		pDoc->m_bRoundEndPressed = true;
		CRect r = m_RoundEnd->GetRect();
		CalcDeviceRect(r);
		InvalidateRect(r, FALSE);
		CSoundManager::GetInstance()->StopMessages(TRUE);
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
		pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pMajor->GetRaceID());
	// Systembutton
	else if (button == 1)
	{
		// System suchen, welches dem Spieler auch gehört
		if (pDoc->CurrentSystem().GetOwnerOfSystem() != pMajor->GetRaceID())
		{
			 if (m_LastSystem != CPoint(-1,-1))
				 pDoc->SetKO(m_LastSystem.x, m_LastSystem.y);
			 else
			 {
				 for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
					 for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
						 if (pDoc->GetSector(x, y).GetSunSystem() == TRUE
							 && pDoc->GetSystem(x, y).GetOwnerOfSystem() == pMajor->GetRaceID())
							{
								pDoc->SetKO(x,y);
								break;
							}
			 }
		}
		if (pDoc->CurrentSystem().GetOwnerOfSystem() == pMajor->GetRaceID() &&
			pDoc->CurrentSector().GetSunSystem() == TRUE)
		{
			pDoc->GetMainFrame()->SelectMainView(SYSTEM_VIEW, pMajor->GetRaceID());
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CPlanetBottomView));
			m_LastSystem = pDoc->GetKO();
		}
	}
	// Forschungsbutton
	else if (button == 2)
		pDoc->GetMainFrame()->SelectMainView(RESEARCH_VIEW, pMajor->GetRaceID());
	// Geheimdienstbutton
	else if (button == 3)
		pDoc->GetMainFrame()->SelectMainView(INTEL_VIEW, pMajor->GetRaceID());
	// Diplomatiebutton
	else if (button == 4)
		pDoc->GetMainFrame()->SelectMainView(DIPLOMACY_VIEW, pMajor->GetRaceID());
	// Handelsbutton
	else if (button == 5)
		pDoc->GetMainFrame()->SelectMainView(TRADE_VIEW, pMajor->GetRaceID());
	// Imperiumsbutton
	else if (button == 6)
		pDoc->GetMainFrame()->SelectMainView(EMPIRE_VIEW, pMajor->GetRaceID());
	CView::OnLButtonUp(nFlags, point);
}

void CMenuChooseView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	bool bHideMenu;
	CIniLoader::GetInstance()->ReadValue("Control", "HIDEMENUBAR", bHideMenu);

	if (bHideMenu && point.y > 5)
	{
		if (GetTopLevelFrame()->SetMenuBarState(AFX_MBS_HIDDEN))
		{
			//ReleaseCapture();
			return;
		}
	}
	else
	{
		if (GetTopLevelFrame()->SetMenuBarState(AFX_MBS_VISIBLE))
		{
			//SetCapture();
			return;
		}
	}

	CalcLogicalPoint(point);

	// Rundenendebutton
	if (m_RoundEnd->ClickedOnButton(point))
	{
		if (m_RoundEnd->Activate())
		{
			CSoundManager::GetInstance()->PlaySound(SNDMGR_SOUND_MAINMENU);
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
				CSoundManager::GetInstance()->PlaySound(SNDMGR_SOUND_MAINMENU);
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
