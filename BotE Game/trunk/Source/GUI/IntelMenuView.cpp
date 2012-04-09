// IntelMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "IntelMenuView.h"
#include "IntelBottomView.h"
#include "Races\RaceController.h"
#include "Graphic\memdc.h"

// CIntelMenuView
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CIntelMenuView, CMainBaseView)

CIntelMenuView::CIntelMenuView()
{
}

CIntelMenuView::~CIntelMenuView()
{
	for (int i = 0; i < m_IntelligenceMainButtons.GetSize(); i++)
	{
		delete m_IntelligenceMainButtons[i];
		m_IntelligenceMainButtons[i] = 0;
	}	
	m_IntelligenceMainButtons.RemoveAll();

	for (int i = 0; i < m_RaceLogoButtons.GetSize(); i++)
	{
		delete m_RaceLogoButtons[i];
		m_RaceLogoButtons[i] = 0;
	}
	m_RaceLogoButtons.RemoveAll();
	
}

BEGIN_MESSAGE_MAP(CIntelMenuView, CMainBaseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CIntelMenuView::OnNewRound()
{
	memset(m_bSortDesc, true, sizeof(m_bSortDesc));
	m_nScrollPos = 0;
	m_bySubMenu = 0;
}

// CIntelMenuView drawing

void CIntelMenuView::OnDraw(CDC* dc)
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

	// ***************************** DIE GEHEIMDIENSTANSICHT ZEICHNEN **********************************
	if (m_bySubMenu == 0)
		DrawIntelAssignmentMenu(&g);
	else if (m_bySubMenu == 1)
		DrawIntelSpyMenu(&g);
	else if (m_bySubMenu == 2)
		DrawIntelSabotageMenu(&g);
	else if (m_bySubMenu == 3)
		DrawIntelInfoMenu(&g);
	else if (m_bySubMenu == 4)
		DrawIntelReportsMenu(&g);
	else if (m_bySubMenu == 5)
		DrawIntelAttackMenu(&g);
	
	// Buttons am unteren Bildschirmrand zeichnen
	DrawIntelMainButtons(&g, pMajor);

	g.ReleaseHDC(pDC->GetSafeHdc());
}

// CIntelMenuView diagnostics

#ifdef _DEBUG
void CIntelMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIntelMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIntelMenuView message handlers
void CIntelMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	
	// Geheimdienstansicht
	m_bySubMenu = 0;
	m_sActiveIntelRace = "";
	memset(m_bSortDesc, true, sizeof(m_bSortDesc));
	m_nScrollPos = 0;
}

/// Funktion lädt die rassenspezifischen Grafiken.
void CIntelMenuView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CreateButtons();

	CString sPrefix = pMajor->GetPrefix();
	
	bg_intelassignmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "intelassignmenu.boj");
	bg_intelspymenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "intelspymenu.boj");
	bg_intelspymenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "intelsabmenu.boj");
	bg_intelreportmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "intelreportmenu.boj");
	bg_intelattackmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "intelattackmenu.boj");
	bg_intelinfomenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "intelinfomenu.boj");
}

BOOL CIntelMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
//	return CMainBaseView::OnEraseBkgnd(pDC);
}

void CIntelMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Geheimdienstmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CIntelMenuView::DrawIntelAssignmentMenu(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;
	
	CString s;
	CRect timber[100];

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_intelassignmenu)
		g->DrawImage(bg_intelassignmenu, 0, 0, 1075, 750);

	if (m_sActiveIntelRace == pMajor->GetRaceID())
		m_sActiveIntelRace = "";
	// Wenn noch keine Rasse ausgewählt wurde, so wird versucht eine bekannte Rasse auszuwählen
	if (m_sActiveIntelRace == "")
	{
		int nCount = 0;
		map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			nCount++;
			if (it->first != pMajor->GetRaceID() && pMajor->IsRaceContacted(it->first) == true)
			{
				if (nCount > 6)
					m_nScrollPos = nCount - 6;
				m_sActiveIntelRace = it->first;
				break;
			}
		}
	}
	
	// rechtes Informationsmenü zeichnen
	DrawIntelInformation(g, &Gdiplus::Font(fontName.AllocSysString(), fontSize), normalColor);
	
	// die einzelnen Rassensymbole zeichnen
	DrawRaceLogosInIntelView(g);
	
	SolidBrush timberBrush(Color::White);
	int count = 1;
	int nPos = 0;
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (nPos++ < m_nScrollPos)
			continue;
		// den Spionage- und Sabotagebalken zeichnen
		BYTE spyPerc = pMajor->GetEmpire()->GetIntelligence()->GetAssignment()->GetGlobalSpyPercentage(it->first);
		s.Format("%d%%", spyPerc);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(415,80+count*90,75,30), &fontFormat, &fontBrush);
		BYTE sabPerc = pMajor->GetEmpire()->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(it->first);
		s.Format("%d%%", sabPerc);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(775,80+count*90,75,30), &fontFormat, &fontBrush);

		// den Zuweisungsbalken für Spionage zeichnen
		if (pMajor->IsRaceContacted(it->first) == false || it->first == pMajor->GetRaceID())
			timberBrush.SetColor(Color(22,26,15));
		else
			timberBrush.SetColor(Color(42,46,30));
		for (int j = 99; j >= 0; j--)
		{
			if (j < spyPerc)
				timberBrush.SetColor(Color(250-j*2.5,50+j*2,0));
			timber[j].SetRect(110+j*3, 80+count*90, 112+j*3, 110+count*90);
			g->FillRectangle(&timberBrush, RectF(timber[j].left, timber[j].top, timber[j].Width(), timber[j].Height()));
		}

		// den Zuweisungsbalken für Sabotage zeichnen
		if (pMajor->IsRaceContacted(it->first) == false || it->first == pMajor->GetRaceID())
			timberBrush.SetColor(Color(22,26,15));					
		else
			timberBrush.SetColor(Color(42,46,30));
		for (int j = 99; j >= 0; j--)
		{
			if (j < sabPerc)
				timberBrush.SetColor(Color(250-j*2.5,50+j*2,0));
			timber[j].SetRect(470+j*3, 80+count*90, 472+j*3, 110+count*90);
			g->FillRectangle(&timberBrush, RectF(timber[j].left, timber[j].top, timber[j].Width(), timber[j].Height()));
		}
		
		if (count++ == 6)
			break;
	}
	
	// Spionage und Sabotage oben über die Balken zeichnen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	s = CResourceManager::GetString("SPY");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(110,130,300,30), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("SABOTAGE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(470,130,300,30), &fontFormat, &fontBrush);

	// den Balken für die innere Sicherheit zeichnen
	BYTE innerSecurityPerc = pMajor->GetEmpire()->GetIntelligence()->GetAssignment()->GetInnerSecurityPercentage();
	fontFormat.SetAlignment(StringAlignmentFar);
	s = CResourceManager::GetString("INNER_SECURITY")+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(20,70,170,50), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentNear);
	s.Format("%d%%", innerSecurityPerc);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(915,70,160,50), &fontFormat, &fontBrush);
	
	timberBrush.SetColor(Color(42,46,30));
	for (int i = 99; i >= 0; i--)
	{
		if (i < innerSecurityPerc)
			timberBrush.SetColor(Color(250-i*2.5,50+i*2,0));
		timber[i].SetRect(200+i*7, 75, 205+i*7, 115);
		g->FillRectangle(&timberBrush, RectF(timber[i].left, timber[i].top, timber[i].Width(), timber[i].Height()));
	}
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentCenter);
	s = CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("SECURITY_HEADQUARTERS");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);	
}

void CIntelMenuView::DrawIntelSpyMenu(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;
	
	CString s;
	CRect timber[100];
	
	if (m_sActiveIntelRace == pMajor->GetRaceID())
		m_sActiveIntelRace = "";
	// Wenn noch keine Rasse ausgewählt wurde, so wird versucht eine bekannte Rasse auszuwählen
	if (m_sActiveIntelRace == "")
	{
		int nCount = 0;
		map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			nCount++;
			if (it->first != pMajor->GetRaceID() && pMajor->IsRaceContacted(it->first) == true)
			{
				if (nCount > 6)
					m_nScrollPos = nCount - 6;
				m_sActiveIntelRace = it->first;
				break;
			}
		}
	}
	
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_intelspymenu)
		g->DrawImage(bg_intelspymenu, 0, 0, 1075, 750);

	// kleinen Button mit welchem man die Aggressivität einstellen kann zeichnen
	if (m_sActiveIntelRace != "")
	{
		fontFormat.SetAlignment(StringAlignmentFar);
		s = CResourceManager::GetString("AGGRESSIVENESS")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,140,190,30), &fontFormat, &fontBrush);	

		Bitmap* graphic = NULL;
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);
		if (graphic)
			g->DrawImage(graphic, 400, 140, 120, 30);		
		
		switch(pMajor->GetEmpire()->GetIntelligence()->GetAggressiveness(0, m_sActiveIntelRace))
		{
		case 0: s = CResourceManager::GetString("CAREFUL");		break;
		case 1: s = CResourceManager::GetString("NORMAL");		break;
		case 2: s = CResourceManager::GetString("AGGRESSIVE");	break;
		}
		fontFormat.SetAlignment(StringAlignmentCenter);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,140,120,30), &fontFormat, &btnBrush);
	}

	// die einzelnen Rassensymbole zeichnen
	DrawRaceLogosInIntelView(g);
	// rechtes Informationsmenü zeichnen
	DrawIntelInformation(g, &Gdiplus::Font(fontName.AllocSysString(), fontSize), normalColor);
	
	SolidBrush timberBrush(Color(42,46,30));
	
	CIntelligence* pIntel = pMajor->GetEmpire()->GetIntelligence();

	// Die einzelnen Spionagezuweisungsbalken zeichnen
	if (m_sActiveIntelRace != "")
	{
		// Bild der aktiven Rasse im Hintergrund zeichnen
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\" + m_sActiveIntelRace + ".bop");
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\Default.bop");		
		if (graphic)
			g->DrawImage(graphic, 310, 230, 300, 300);
		Gdiplus::SolidBrush brush(Gdiplus::Color(160, 0, 0, 0));
		g->FillRectangle(&brush, RectF(310,230,300,300));

		fontFormat.SetAlignment(StringAlignmentFar);
		s = CResourceManager::GetString("ECONOMY")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,230,190,30), &fontFormat, &fontBrush);
		s = CResourceManager::GetString("SCIENCE")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,320,190,30), &fontFormat, &fontBrush);
		s = CResourceManager::GetString("MILITARY")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,410,190,30), &fontFormat, &fontBrush);
		s = CResourceManager::GetString("DIPLOMACY")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,500,190,30), &fontFormat, &fontBrush);
		
		for (int i = 0; i < 4; i++)
		{
			BYTE spyPerc = pIntel->GetAssignment()->GetSpyPercentages(m_sActiveIntelRace, i);
			int gp =  pIntel->GetSecurityPoints() *	pIntel->GetAssignment()->GetGlobalSpyPercentage(m_sActiveIntelRace) *
				pIntel->GetAssignment()->GetSpyPercentages(m_sActiveIntelRace, i) / 10000
				// + den Depotwert
				+ (pIntel->GetSPStorage(0, m_sActiveIntelRace) * pIntel->GetAssignment()->GetSpyPercentages(m_sActiveIntelRace, i) / 100);
			// eventuellen Geheimdienstbonus noch dazurechnen
			gp += gp * pIntel->GetBonus(i, 0) / 100;
			fontFormat.SetAlignment(StringAlignmentNear);
			s.Format("%d%% (%d %s)", spyPerc, gp, CResourceManager::GetString("SP"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(625,230+i*90,275,30), &fontFormat, &fontBrush);
			// den Zuweisungsbalken zeichnen
			timberBrush.SetColor(Color(42,46,30));
			for (int j = 99; j >= 0; j--)
			{
				if (j < spyPerc)
					timberBrush.SetColor(Color(250-j*2.5,50+j*2,0));	
				timber[j].SetRect(310+j*3, 230+i*90, 312+j*3, 260+i*90);
				g->FillRectangle(&timberBrush, RectF(timber[j].left, timber[j].top, timber[j].Width(), timber[j].Height()));
			}
		}
		fontFormat.SetAlignment(StringAlignmentCenter);
		CString sPerc;
		sPerc.Format("%d", pIntel->GetAssignment()->GetGlobalSpyPercentage(m_sActiveIntelRace));
		s = CResourceManager::GetString("SPY_OF_ALL", FALSE, sPerc);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,180,725,30), &fontFormat, &fontBrush);
		
		// kurze Erklärung und Punkte im Depot hinschreiben
		CString race;
		CMajor* pActiveRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sActiveIntelRace));
		if (pActiveRace)
			race = pActiveRace->GetEmpireNameWithArticle();
		sPerc.Format("%d", pIntel->GetSPStorage(0, m_sActiveIntelRace));
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CResourceManager::GetString("USE_SP_FROM_DEPOT", FALSE, sPerc, race);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(150,550,625,100), &fontFormat, &fontBrush);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	}

	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// den Balken für die "was ins Lager kommt" Spionagepunkte zeichnen
	BYTE spyToStore = 100;
	if (m_sActiveIntelRace != "")
		for (int i = 0; i < 4; i++)
			spyToStore -= pIntel->GetAssignment()->GetSpyPercentages(m_sActiveIntelRace, i);
	fontFormat.SetAlignment(StringAlignmentFar);
	s = CResourceManager::GetString("INTEL_RESERVE")+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(20,70,170,50), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentNear);
	s.Format("%d%%", spyToStore);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(915,70,160,50), &fontFormat, &fontBrush);
		
	timberBrush.SetColor(Color(42,46,30));
	for (int i = 99; i >= 0; i--)
	{
		if (i < spyToStore)
			timberBrush.SetColor(Color(250-i*2.5,50+i*2,0));
		timber[i].SetRect(200+i*7, 75, 205+i*7, 115);
		g->FillRectangle(&timberBrush, RectF(timber[i].left, timber[i].top, timber[i].Width(), timber[i].Height()));
	}
		
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentCenter);
	s = CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("SPY");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);	
}

void CIntelMenuView::DrawIntelSabotageMenu(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;
	
	CString s;
	CRect timber[100];
	
	if (m_sActiveIntelRace == pMajor->GetRaceID())
		m_sActiveIntelRace = "";
	// Wenn noch keine Rasse ausgewählt wurde, so wird versucht eine bekannte Rasse auszuwählen
	if (m_sActiveIntelRace == "")
	{
		int nCount = 0;
		map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			nCount++;
			if (it->first != pMajor->GetRaceID() && pMajor->IsRaceContacted(it->first) == true)
			{
				if (nCount > 6)
					m_nScrollPos = nCount - 6;
				m_sActiveIntelRace = it->first;
				break;
			}
		}
	}

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_intelspymenu)
		g->DrawImage(bg_intelspymenu, 0, 0, 1075, 750);

	// kleinen Button mit welchem man die Aggressivität einstellen kann zeichnen
	if (m_sActiveIntelRace != "")
	{
		fontFormat.SetAlignment(StringAlignmentFar);
		s = CResourceManager::GetString("AGGRESSIVENESS")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,140,190,30), &fontFormat, &fontBrush);	

		Bitmap* graphic = NULL;
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);
		if (graphic)
			g->DrawImage(graphic, 400, 140, 120, 30);		
		
		switch(pMajor->GetEmpire()->GetIntelligence()->GetAggressiveness(1, m_sActiveIntelRace))
		{
		case 0: s = CResourceManager::GetString("CAREFUL");		break;
		case 1: s = CResourceManager::GetString("NORMAL");		break;
		case 2: s = CResourceManager::GetString("AGGRESSIVE");	break;
		}
		fontFormat.SetAlignment(StringAlignmentCenter);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,140,120,30), &fontFormat, &btnBrush);
	}

	// die einzelnen Rassensymbole zeichnen
	DrawRaceLogosInIntelView(g);
	// rechtes Informationsmenü zeichnen
	DrawIntelInformation(g, &Gdiplus::Font(fontName.AllocSysString(), fontSize), normalColor);
	
	SolidBrush timberBrush(Color(42,46,30));
	
	CIntelligence* pIntel = pMajor->GetEmpire()->GetIntelligence();

	// Die einzelnen Spionagezuweisungsbalken zeichnen
	if (m_sActiveIntelRace != "")
	{
		// Bild der aktiven Rasse im Hintergrund zeichnen
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\" + m_sActiveIntelRace + ".bop");
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\Default.bop");		
		if (graphic)
			g->DrawImage(graphic, 310, 230, 300, 300);
		Gdiplus::SolidBrush brush(Gdiplus::Color(160, 0, 0, 0));
		g->FillRectangle(&brush, RectF(310,230,300,300));

		fontFormat.SetAlignment(StringAlignmentFar);
		s = CResourceManager::GetString("ECONOMY")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,230,190,30), &fontFormat, &fontBrush);
		s = CResourceManager::GetString("SCIENCE")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,320,190,30), &fontFormat, &fontBrush);
		s = CResourceManager::GetString("MILITARY")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,410,190,30), &fontFormat, &fontBrush);
		s = CResourceManager::GetString("DIPLOMACY")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,500,190,30), &fontFormat, &fontBrush);
		
		for (int i = 0; i < 4; i++)
		{
			BYTE sabPerc = pIntel->GetAssignment()->GetSabotagePercentages(m_sActiveIntelRace, i);
			int gp =  pIntel->GetSecurityPoints() *	pIntel->GetAssignment()->GetGlobalSabotagePercentage(m_sActiveIntelRace) *
				pIntel->GetAssignment()->GetSabotagePercentages(m_sActiveIntelRace, i) / 10000
				// + den Depotwert
				+ (pIntel->GetSPStorage(1, m_sActiveIntelRace) * pIntel->GetAssignment()->GetSabotagePercentages(m_sActiveIntelRace, i) / 100);
			// eventuellen Geheimdienstbonus noch dazurechnen
			gp += gp * pIntel->GetBonus(i, 1) / 100;
			fontFormat.SetAlignment(StringAlignmentNear);
			s.Format("%d%% (%d %s)", sabPerc, gp, CResourceManager::GetString("SP"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(625,230+i*90,275,30), &fontFormat, &fontBrush);
			// den Zuweisungsbalken zeichnen
			timberBrush.SetColor(Color(42,46,30));
			for (int j = 99; j >= 0; j--)
			{
				if (j < sabPerc)
					timberBrush.SetColor(Color(250-j*2.5,50+j*2,0));	
				timber[j].SetRect(310+j*3, 230+i*90, 312+j*3, 260+i*90);
				g->FillRectangle(&timberBrush, RectF(timber[j].left, timber[j].top, timber[j].Width(), timber[j].Height()));
			}
		}
		fontFormat.SetAlignment(StringAlignmentCenter);
		CString sPerc;
		sPerc.Format("%d", pIntel->GetAssignment()->GetGlobalSabotagePercentage(m_sActiveIntelRace));
		s = CResourceManager::GetString("SABOTAGE_OF_ALL", FALSE, sPerc);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,180,725,30), &fontFormat, &fontBrush);
		
		// kurze Erklärung und Punkte im Depot hinschreiben
		CString race;
		CMajor* pActiveRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sActiveIntelRace));
		if (pActiveRace)
			race = pActiveRace->GetEmpireNameWithArticle();
		sPerc.Format("%d", pIntel->GetSPStorage(1, m_sActiveIntelRace));
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CResourceManager::GetString("USE_SP_FROM_DEPOT", FALSE, sPerc, race);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(150,550,625,100), &fontFormat, &fontBrush);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	}

	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// den Balken für die "was ins Lager kommt" Spionagepunkte zeichnen
	BYTE sabToStore = 100;
	if (m_sActiveIntelRace != "")
		for (int i = 0; i < 4; i++)
			sabToStore -= pIntel->GetAssignment()->GetSabotagePercentages(m_sActiveIntelRace, i);
	fontFormat.SetAlignment(StringAlignmentFar);
	s = CResourceManager::GetString("INTEL_RESERVE")+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(20,70,170,50), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentNear);
	s.Format("%d%%", sabToStore);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(915,70,160,50), &fontFormat, &fontBrush);
		
	timberBrush.SetColor(Color(42,46,30));
	for (int i = 99; i >= 0; i--)
	{
		if (i < sabToStore)
			timberBrush.SetColor(Color(250-i*2.5,50+i*2,0));
		timber[i].SetRect(200+i*7, 75, 205+i*7, 115);
		g->FillRectangle(&timberBrush, RectF(timber[i].left, timber[i].top, timber[i].Width(), timber[i].Height()));
	}
		
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentCenter);
	s = CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("SABOTAGE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);	
}

void CIntelMenuView::DrawIntelReportsMenu(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString s;
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_intelreportmenu)
		g->DrawImage(bg_intelreportmenu, 0, 0, 1075, 750);

	// Farbe für die Markierungen auswählen
	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	Gdiplus::Pen pen(markColor);
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
		
	CIntelligence* pIntel = pMajor->GetEmpire()->GetIntelligence();
	// Es gehen nur 21 Berichte auf die Seite, deshalb muss abgebrochen werden
	// wenn noch kein Bericht angeklickt wurde, es aber Berichte gibt, dann den ersten Bericht in der Reihe markieren
	if (pIntel->GetIntelReports()->GetActiveReport() == -1 && pIntel->GetIntelReports()->GetNumberOfReports() > 0)
	{
		pIntel->GetIntelReports()->SetActiveReport(0);
		m_iOldClickedIntelReport = 0;
	}

	int j = 0;
	short counter = pIntel->GetIntelReports()->GetActiveReport() - 20 + m_iOldClickedIntelReport;
	short oldClickedNews = pIntel->GetIntelReports()->GetActiveReport();
	for (int i = 0; i < pIntel->GetIntelReports()->GetNumberOfReports(); i++)
	{
		if (counter > 0)
		{
			pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() - 1);
			counter--;
			continue;
		}
		if (j < 21)
		{
			CIntelObject* intelObj = (CIntelObject*)pIntel->GetIntelReports()->GetAllReports()->GetAt(i);
			// Die News markieren
			if (j == pIntel->GetIntelReports()->GetActiveReport())
			{
				// Markierung worauf wir geklickt haben
				g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(100,140+j*25,875,25));
				g->DrawLine(&pen, 100, 140+j*25, 975, 140+j*25);
				g->DrawLine(&pen, 100, 140+j*25+25, 975, 140+j*25+25);
				// Farbe der Schrift wählen, wenn wir den Eintrag markiert haben
				fontBrush.SetColor(markColor);
			}
			else
			{
				// Wenn wir selbst Opfer sind, dann Text grau darstellen
				if (intelObj->GetEnemy() == pMajor->GetRaceID())
					fontBrush.SetColor(Color(150,150,150));
				else
					fontBrush.SetColor(normalColor);					
			}
			
			s.Format("%d",	intelObj->GetRound());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,140+j*25,100,25), &fontFormat, &fontBrush);
						
			CMajor* pEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(intelObj->GetEnemy()));
			if (pEnemy)
				s = pEnemy->GetEmpiresName();
			else
				s = CResourceManager::GetString("UNKNOWN");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,140+j*25,400,25), &fontFormat, &fontBrush);
			if (intelObj->GetIsSpy())
				s = CResourceManager::GetString("SPY");
			else
				s = CResourceManager::GetString("SABOTAGE");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(600,140+j*25,200,25), &fontFormat, &fontBrush);
			switch (intelObj->GetType())
			{
			case 0: s = CResourceManager::GetString("ECONOMY"); break;
			case 1: s = CResourceManager::GetString("SCIENCE"); break;
			case 2: s = CResourceManager::GetString("MILITARY"); break;
			case 3: s = CResourceManager::GetString("DIPLOMACY"); break;
			default: s = CResourceManager::GetString("UNKNOWN");
			}
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,140+j*25,200,25), &fontFormat, &fontBrush);
			fontBrush.SetColor(normalColor);			
			j++;
		}
	}
	pIntel->GetIntelReports()->SetActiveReport(oldClickedNews);
		
	// größere Schriftart für Tabellenüberschriften holen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Tabellenüberschriften zeichnen
	fontBrush.SetColor(markColor);
	s = CResourceManager::GetString("ROUND").AllocSysString();
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,100,100,30), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("ENEMY")+" ("+(CResourceManager::GetString("TARGET"))+")";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,100,400,30), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("KIND");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(600,100,200,30), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("TYPE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,100,200,30), &fontFormat, &fontBrush);
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentCenter);
	CString sReports;
	sReports.Format(" (%d)", pIntel->GetIntelReports()->GetNumberOfReports());
	s = CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("REPORTS") + sReports;
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);
}

void CIntelMenuView::DrawIntelAttackMenu(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString s;
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_intelattackmenu)
		g->DrawImage(bg_intelattackmenu, 0, 0, 1075, 750);

	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	Gdiplus::Pen pen(markColor);
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	
	CIntelligence* pIntel = pMajor->GetEmpire()->GetIntelligence();
	// Es gehen nur 10 Berichte auf die Seite, deshalb muss abgebrochen werden
	// wenn noch kein Bericht angeklickt wurde, es aber Berichte gibt, dann den ersten Bericht in der Reihe markieren
	int numberOfReports = 0;
	if (pIntel->GetIntelReports()->GetActiveReport() == -1)
	{
		for (int i = 0; i < pIntel->GetIntelReports()->GetNumberOfReports(); i++)
		{
			CIntelObject* intelObj = pIntel->GetIntelReports()->GetReport(i);
			if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != pMajor->GetRaceID() && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
				numberOfReports++;
		}
		if (numberOfReports > 0)
		{
			pIntel->GetIntelReports()->SetActiveReport(0);
			m_iOldClickedIntelReport = 0;
		}
	}
	int j = 0;
	short counter = pIntel->GetIntelReports()->GetActiveReport() - 10 + m_iOldClickedIntelReport;
	short oldClickedNews = pIntel->GetIntelReports()->GetActiveReport();
	numberOfReports = 0;
	short activeReport = 0;
	for (int i = 0; i < pIntel->GetIntelReports()->GetNumberOfReports(); i++)
	{
		CIntelObject* intelObj = pIntel->GetIntelReports()->GetReport(i);
		if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != pMajor->GetRaceID() && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
		{
			numberOfReports++;
			if (counter > 0)
			{
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() - 1);
				counter--;
				continue;
			}
			if (j < 11)
			{
				// Die News markieren
				if (j == pIntel->GetIntelReports()->GetActiveReport())
				{
					activeReport = i;					
				//	s.Format("aktiver Report: %d\n", activeReport);
				//	AfxMessageBox(s + *intelObj->GetOwnerDesc() + "\n\n" + *intelObj->GetEnemyDesc());
					// Markierung worauf wir geklickt haben
					g->FillRectangle(&SolidBrush(Color(100,200,200,200)), RectF(100,140+j*25,875,25));
					g->DrawLine(&pen, 100, 140+j*25, 975, 140+j*25);
					g->DrawLine(&pen, 100, 140+j*25+25, 975, 140+j*25+25);
					// Farbe der Schrift wählen, wenn wir den Eintrag markiert haben
					fontBrush.SetColor(markColor);					
				}
				else
					fontBrush.SetColor(normalColor);					
								
				s.Format("%d",	intelObj->GetRound());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,140+j*25,100,25), &fontFormat, &fontBrush);
				s = "";
				CMajor* pEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(intelObj->GetEnemy()));
				if (pEnemy)
					s = pEnemy->GetEmpiresName();
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,140+j*25,400,25), &fontFormat, &fontBrush);
				if (intelObj->GetIsSpy())
					s = CResourceManager::GetString("SPY");
				else if (intelObj->GetIsSabotage())
					s = CResourceManager::GetString("SABOTAGE");
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(600,140+j*25,200,25), &fontFormat, &fontBrush);
				switch (intelObj->GetType())
				{
				case 0: s = CResourceManager::GetString("ECONOMY"); break;
				case 1: s = CResourceManager::GetString("SCIENCE"); break;
				case 2: s = CResourceManager::GetString("MILITARY"); break;
				case 3: s = CResourceManager::GetString("DIPLOMACY"); break;
				default: s = CResourceManager::GetString("UNKNOWN");
				}
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,140+j*25,200,25), &fontFormat, &fontBrush);
				fontBrush.SetColor(normalColor);
				j++;
			}
		}
	}
//	pIntel->GetIntelReports()->SetActiveReport(oldClickedNews);
	pIntel->GetIntelReports()->SetActiveReport(activeReport);
	
	// Beschreibung und Auswahlmöglichkeiten zeichnen
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	s = CResourceManager::GetString("ATTEMPT_DESC");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,450,875,50), &fontFormat, &fontBrush);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetAlignment(StringAlignmentNear);
	
	if (pIntel->GetIntelReports()->GetAttemptObject())
	{
		fontBrush.SetColor(markColor);
		CIntelObject* attemptObj = pIntel->GetIntelReports()->GetAttemptObject();
		s.Format("%d",	attemptObj->GetRound());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,565,200,25), &fontFormat, &fontBrush);
		s = "";
		CMajor* pEnemy = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(attemptObj->GetEnemy()));
		if (pEnemy)
			s = pEnemy->GetEmpiresName();
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,565,400,25), &fontFormat, &fontBrush);
		if (attemptObj->GetIsSpy())
			s = CResourceManager::GetString("SPY");
		else if (attemptObj->GetIsSabotage())
			s = CResourceManager::GetString("SABOTAGE");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(600,565,200,25), &fontFormat, &fontBrush);
		switch (attemptObj->GetType())
		{
		case 0: s = CResourceManager::GetString("ECONOMY"); break;
		case 1: s = CResourceManager::GetString("SCIENCE"); break;
		case 2: s = CResourceManager::GetString("MILITARY"); break;
		case 3: s = CResourceManager::GetString("DIPLOMACY"); break;
		default: s = CResourceManager::GetString("UNKNOWN");
		}
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,565,200,25), &fontFormat, &fontBrush);
		fontBrush.SetColor(normalColor);
		s = *attemptObj->GetOwnerDesc();
		fontFormat.SetAlignment(StringAlignmentCenter);	
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,600,875,100), &fontFormat, &fontBrush);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);		
		fontFormat.SetAlignment(StringAlignmentNear);
	}
		

	Bitmap* graphic = NULL;
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);
	if (graphic)
	{
		g->DrawImage(graphic, 400, 510, 120, 30);
		g->DrawImage(graphic, 555, 510, 120, 30);
	}
	fontFormat.SetAlignment(StringAlignmentCenter);
	s = CResourceManager::GetString("BTN_SELECT");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(400,510,120,30), &fontFormat, &btnBrush);
	s = CResourceManager::GetString("BTN_CANCEL");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(555,510,120,30), &fontFormat, &btnBrush);
		
	// größere Schriftart für Tabellenüberschriften holen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Tabellenüberschriften zeichnen
	fontBrush.SetColor(markColor);
	fontFormat.SetAlignment(StringAlignmentNear);
	s = CResourceManager::GetString("ROUND").AllocSysString();
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,100,100,30), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("ENEMY")+" ("+(CResourceManager::GetString("TARGET"))+")";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,100,400,30), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("KIND");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(600,100,200,30), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("TYPE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,100,200,30), &fontFormat, &fontBrush);
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentCenter);
	CString sReports;
	sReports.Format(" (%d)", numberOfReports);
	s = CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("POSSIBLE_ATTEMPTS") + sReports;
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);
}

void CIntelMenuView::DrawIntelInfoMenu(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CIntelligence* pIntel = pMajor->GetEmpire()->GetIntelligence();
	// Daten für die Geheimdienstinformationen berechnen lassen. 
	// Dies wird einmalig pro Runde durchgeführt, sobald man in das Geheimdienstinfomenü geht
	pIntel->GetIntelInfo()->CalcIntelInfo(pDoc, pMajor);

	CString s;
		
	// Wenn noch keine Rasse ausgewählt wurde, so wird versucht eine bekannte Rasse auszuwählen
	if (m_sActiveIntelRace == "")
	{
		map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			if (pMajor->IsRaceContacted(it->first) == true)
			{
				m_sActiveIntelRace = it->first;
				break;
			}
	}
	if (m_sActiveIntelRace == "")
		m_sActiveIntelRace = pMajor->GetRaceID();

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_intelinfomenu)
		g->DrawImage(bg_intelinfomenu, 0, 0, 1075, 750);

	// die Geheimdienstinfos der angeklickten Rasse zeichnen
	if (m_sActiveIntelRace != "")
	{
		// Bild der aktiven Rasse im Hintergrund zeichnen
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\" + m_sActiveIntelRace + ".bop");
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\Default.bop");		
		if (graphic)
			g->DrawImage(graphic, 210, 260, 300, 300);
		Gdiplus::SolidBrush brush(Gdiplus::Color(160, 0, 0, 0));
		g->FillRectangle(&brush, RectF(210,260,300,300));
		
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%s:", CResourceManager::GetString("CONTROLLED_SECTORS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,285,325,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format(" %d", pIntel->GetIntelInfo()->GetControlledSectors(m_sActiveIntelRace));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(425,285,175,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%s:", CResourceManager::GetString("CONTROLLED_SYSTEMS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,335,325,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format(" %d", pIntel->GetIntelInfo()->GetOwnedSystems(m_sActiveIntelRace));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(425,335,175,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%s:", CResourceManager::GetString("INHABITED_SYSTEMS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,385,325,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format(" %d", pIntel->GetIntelInfo()->GetInhabitedSystems(m_sActiveIntelRace));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(425,385,175,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%s:", CResourceManager::GetString("KNOWN_MINORRACES"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,435,325,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format(" %d", pIntel->GetIntelInfo()->GetKnownMinors(m_sActiveIntelRace));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(425,435,175,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%s:", CResourceManager::GetString("NUMBER_OF_MINORMEMBERS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,485,325,25), &fontFormat, &fontBrush);
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format(" %d", pIntel->GetIntelInfo()->GetMinorMembers(m_sActiveIntelRace));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(425,485,175,25), &fontFormat, &fontBrush);		
	}

	// die einzelnen Rassensymbole zeichnen
	DrawRaceLogosInIntelView(g, true);
		
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\" + pIntel->GetResponsibleRace() + ".bop");
	if (graphic == NULL)
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\Default.bop");		
	if (graphic)
		g->DrawImage(graphic, 737,435,75,75);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	s = CResourceManager::GetString("CHOOSE_RESPONSIBLE_RACE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(600,285,350,100), &fontFormat, &fontBrush);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);
	if (graphic)
		g->DrawImage(graphic, 715, 400, 120, 30);
	
	CMajor* pResponsibleRace = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pIntel->GetResponsibleRace()));
	if (pResponsibleRace)
		s = pResponsibleRace->GetRaceName();
	else
		s = "ID_ERROR";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(715,400,120,30), &fontFormat, &btnBrush);
		
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);
	fontFormat.SetAlignment(StringAlignmentCenter);
	s = CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("INFORMATION");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx, 50), &fontFormat, &fontBrush);
}

void CIntelMenuView::DrawRaceLogosInIntelView(Graphics* g, BOOLEAN highlightPlayersRace)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;
	
	int count = 1;
	int nPos = 0;
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		if (nPos++ < m_nScrollPos)
			continue;

		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\" + it->first + ".bop");
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\Default.bop");
		
		if (graphic)
		{
			if (it->first != pMajor->GetRaceID() && pMajor->IsRaceContacted(it->first) == true
				|| it->first == pMajor->GetRaceID() && highlightPlayersRace == TRUE)
			{
				g->DrawImage(graphic, 20, 60+count*90, 75, 75);
			}
			else
			{
				RectF dest(20, 60+count*90, 75, 75);
				// Create an ImageAttributes object and set the gamma
				/*ImageAttributes* imageAttr = new ImageAttributes();
				imageAttr->SetGamma(3.0f, ColorAdjustTypeBitmap);
				//g->DrawImage(graphic, dest, 0, 0, graphic->GetWidth(), graphic->GetHeight(), Gdiplus::UnitPixel, imageAttr);
				delete imageAttr;*/
				g->DrawImage(graphic, 20, 60+count*90, 75, 75);
				Gdiplus::SolidBrush brush(Gdiplus::Color(160, 0, 0, 0));
				g->FillRectangle(&brush, dest);
			}
		}
		if (count++ == 6)
			break;
	}

	if (m_nScrollPos > 0)
	{
		CArray<CMyButton*> vButtons;
		vButtons.Add(m_RaceLogoButtons[0]);
		DrawGDIButtons(g, &vButtons, -1, Gdiplus::Font(NULL), SolidBrush(Color::Black));
	}
	if ((int)pmMajors->size() > m_nScrollPos + 6)
	{
		CArray<CMyButton*> vButtons;
		vButtons.Add(m_RaceLogoButtons[1]);
		DrawGDIButtons(g, &vButtons, -1, Gdiplus::Font(NULL), SolidBrush(Color::Black));
	}	
}

void CIntelMenuView::DrawIntelInformation(Graphics* g, Gdiplus::Font* font, Gdiplus::Color color)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	SolidBrush fontBrush(color);
	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);	

	CIntelligence* pIntel = pMajor->GetEmpire()->GetIntelligence();

	CString s;

	g->DrawString(CResourceManager::GetString("SECURITYPOINTS").AllocSysString(), -1, font, RectF(825,130,250,25), &fontFormatCenter, &fontBrush);
	g->DrawString(CResourceManager::GetString("SECURITYBONI").AllocSysString(), -1, font, RectF(825,200,250,25), &fontFormatCenter, &fontBrush);
	g->DrawString(CResourceManager::GetString("SPY").AllocSysString(), -1, font, RectF(825,270,250,25), &fontFormatCenter, &fontBrush);
	g->DrawString(CResourceManager::GetString("SABOTAGE").AllocSysString(), -1, font, RectF(825,410,250,25), &fontFormatCenter, &fontBrush);
	g->DrawString(CResourceManager::GetString("DEPOTS").AllocSysString(), -1, font, RectF(825,550,250,25), &fontFormatCenter, &fontBrush);
	
	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	fontBrush.SetColor(markColor);
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	
	s = CResourceManager::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,165,190,25), &fontFormatCenter, &fontBrush);
	s = CResourceManager::GetString("INNER_SECURITY_SHORT").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,235,190,25), &fontFormatCenter, &fontBrush);
	s = CResourceManager::GetString("ECONOMY").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,305,190,25), &fontFormatCenter, &fontBrush);
	s = CResourceManager::GetString("SCIENCE").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,340,190,25), &fontFormatCenter, &fontBrush);
	s = CResourceManager::GetString("MILITARY").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,375,190,25), &fontFormatCenter, &fontBrush);
	s = CResourceManager::GetString("ECONOMY").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,445,190,25), &fontFormatCenter, &fontBrush);
	s = CResourceManager::GetString("SCIENCE").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,480,190,25), &fontFormatCenter, &fontBrush);
	s = CResourceManager::GetString("MILITARY").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,515,190,25), &fontFormatCenter, &fontBrush);

	fontFormatCenter.SetAlignment(StringAlignmentFar);
	s.Format("%d "+CResourceManager::GetString("SP"), pMajor->GetEmpire()->GetSP());
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,165,190,25), &fontFormatCenter, &fontBrush);	
	s.Format("%d%%", pIntel->GetInnerSecurityBoni());
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,235,190,25), &fontFormatCenter, &fontBrush);	
	s.Format("%d%%", pIntel->GetEconomyBonus(0));
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,305,190,25), &fontFormatCenter, &fontBrush);	
	s.Format("%d%%", pIntel->GetScienceBonus(0));
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,340,190,25), &fontFormatCenter, &fontBrush);	
	s.Format("%d%%", pIntel->GetMilitaryBonus(0));
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,375,190,25), &fontFormatCenter, &fontBrush);	
	s.Format("%d%%", pIntel->GetEconomyBonus(1));
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,445,190,25), &fontFormatCenter, &fontBrush);	
	s.Format("%d%%", pIntel->GetScienceBonus(1));
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,480,190,25), &fontFormatCenter, &fontBrush);	
	s.Format("%d%%", pIntel->GetMilitaryBonus(1));
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,515,190,25), &fontFormatCenter, &fontBrush);	
		
	// Depotlager
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CResourceManager::GetString("INNER_SECURITY_SHORT").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,585,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	s.Format("%d", pIntel->GetInnerSecurityStorage());
	g->DrawString(s.AllocSysString(), -1, font, RectF(850,585,190,25), &fontFormatCenter, &fontBrush);	
	
	if (m_sActiveIntelRace != "" && m_sActiveIntelRace != m_pPlayersRace->GetRaceID())
	{
		fontFormatCenter.SetAlignment(StringAlignmentNear);
		s =CResourceManager::GetString("SPY").MakeUpper()+":";
		g->DrawString(s.AllocSysString(), -1, font, RectF(850,620,190,25), &fontFormatCenter, &fontBrush);
		
		fontFormatCenter.SetAlignment(StringAlignmentFar);
		s.Format("%d", pIntel->GetSPStorage(0, m_sActiveIntelRace));
		g->DrawString(s.AllocSysString(), -1, font, RectF(850,620,190,25), &fontFormatCenter, &fontBrush);
		
		fontFormatCenter.SetAlignment(StringAlignmentNear);
		s = CResourceManager::GetString("SABOTAGE").MakeUpper()+":";
		g->DrawString(s.AllocSysString(), -1, font, RectF(850,655,190,25), &fontFormatCenter, &fontBrush);
		
		fontFormatCenter.SetAlignment(StringAlignmentFar);
		s.Format("%d", pIntel->GetSPStorage(1, m_sActiveIntelRace));
		g->DrawString(s.AllocSysString(), -1, font, RectF(850,655,190,25), &fontFormatCenter, &fontBrush);		
	}
}

/// Funktion zeichnet die Buttons unter den Intelmenüs.
/// @param g Zeiger auf GDI+ Grafikobjekt
/// @param pMajor Spielerrasse
void CIntelMenuView::DrawIntelMainButtons(Graphics* g, CMajor* pMajor)
{	
	CString fontName;
	REAL fontSize;
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush fontBrush(btnColor);
	DrawGDIButtons(g, &m_IntelligenceMainButtons, m_bySubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);
}

void CIntelMenuView::OnLButtonDown(UINT nFlags, CPoint point)
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
	// Wenn wir uns in der Geheimdienstansicht befinden		
	// Checken, ob ich auf einen Button geklickt habe um in ein anderes Untermenü zu gelangen
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_IntelligenceMainButtons, temp))
	{
		m_bySubMenu = temp;
		// Wenn wir ins Anschlagsmenü gehen, dann den aktiven Bericht auf keinen setzen
		if (temp == 5)
			pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->SetActiveReport(-1);
		pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
		return;
	}

	// Wenn wir uns in einem Menü befinden, in welchem die Rassenlogoliste links angezeigt wird
	if (m_bySubMenu < 4)
	{
		temp = -1;
		if (ButtonReactOnLeftClick(point, &m_RaceLogoButtons, temp, FALSE, TRUE))
		{
			if (temp == 0)
			{
				if (m_nScrollPos > 0)
				{
					m_nScrollPos--;
					Invalidate(FALSE);
				}				
			}
			else if (temp == 1)
			{
				if ((int)pDoc->GetRaceCtrl()->GetMajors()->size() > m_nScrollPos + 6)
				{
					m_nScrollPos++;
					Invalidate(FALSE);
				}				
			}
			return;
		}
	}

	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	// befinden wir uns in der globalen Zuweisungsansicht (IntelAssignmentMenu)
	if (m_bySubMenu == 0)
	{
		// wurde in den Bereich des Balkens der inneren Sicherheit geklickt
		if (CRect(200,75,900,115).PtInRect(point))
		{
			// hier Zuweisung vornehmen
			pMajor->GetEmpire()->GetIntelligence()->SetAssignment()->SetGlobalPercentage
				(2, ((point.x - 196) / 7), pMajor, "", pmMajors);
			Invalidate(FALSE);
			return;
		}
		
		int count = 1;
		int nPos = 0;
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			if (nPos++ < m_nScrollPos)
				continue;
			if (it->first != pMajor->GetRaceID() && pMajor->IsRaceContacted(it->first))
			{
				// wurde in den Bereich der Balken zur Spionage geklickt
				if (CRect(110, 80+count*90, 410, 110+count*90).PtInRect(point))
				{
					// hier Zuweisung vornehmen
					pMajor->GetEmpire()->GetIntelligence()->SetAssignment()->SetGlobalPercentage
						(0, ((point.x - 108) / 3), pMajor, it->first, pmMajors);
					m_sActiveIntelRace = it->first;
					Invalidate(FALSE);
					return;
				}
				// wurde in den Bereich der Balken zur Sabotage geklickt
				else if (CRect(470, 80+count*90, 770, 110+count*90).PtInRect(point))
				{
					// hier Zuweisung vornehmen
					pMajor->GetEmpire()->GetIntelligence()->SetAssignment()->SetGlobalPercentage
						(1, ((point.x - 468) / 3), pMajor, it->first, pmMajors);
					m_sActiveIntelRace = it->first;
					Invalidate(FALSE);
					return;
				}
			}
			if (count++ == 6)
				break;
		}
	}
	// befinden wir uns im Spionagemenü (IntelSpyMenu)
	else if (m_bySubMenu == 1)
	{
		// Wurde auf das Rassensymbol geklickt um eine Rasse zu aktivieren
		int count = 1;
		int nPos = 0;
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			if (nPos++ < m_nScrollPos)
				continue;
			if (CRect(20,60+count*90,95,135+count*90).PtInRect(point) && it->first != pMajor->GetRaceID() && 
				pMajor->IsRaceContacted(it->first) == true)
			{
				m_sActiveIntelRace = it->first;
				Invalidate(FALSE);
				return;
			}
			if (count++ == 6)
				break;
		}

		if (m_sActiveIntelRace != "" && m_sActiveIntelRace != pMajor->GetRaceID())
		{
			// wurde auf den Button für die Steuerung der Aggressivität geklickt
			if (CRect(400,140,520,170).PtInRect(point))
			{
				BYTE oldAgg = pMajor->GetEmpire()->GetIntelligence()->GetAggressiveness(0, m_sActiveIntelRace);
				if (++oldAgg == 3)
					oldAgg = 0;
				pMajor->GetEmpire()->GetIntelligence()->SetAggressiveness(0, m_sActiveIntelRace, oldAgg);
				Invalidate(FALSE);
				return;
			}
			// wurde in den Bereich des Balkens für das Spionagedepot geklickt
			if (CRect(200,75,900,115).PtInRect(point))
			{
				// hier Zuweisung vornehmen
				pMajor->GetEmpire()->GetIntelligence()->SetAssignment()->SetSpyPercentage(4, ((point.x - 196) / 7), m_sActiveIntelRace);
				Invalidate(FALSE);
				return;
			}
			// wurde in den Bereich der Balken zur speziellen Spionage geklickt
			for (int i = 0; i < 4; i++)
				if (CRect(310, 230+i*90, 610, 260+i*90).PtInRect(point))
				{
					// hier Zuweisung vornehmen
					pMajor->GetEmpire()->GetIntelligence()->SetAssignment()->SetSpyPercentage(i, ((point.x - 308) / 3), m_sActiveIntelRace);
					Invalidate(FALSE);
					return;
				}
		}
	}
	// befinden wir uns im Sabotagemenü (IntelSabotageMenu)
	else if (m_bySubMenu == 2)
	{
		// Wurde auf das Rassensymbol geklickt um eine Rasse zu aktivieren
		int count = 1;
		int nPos = 0;
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			if (nPos++ < m_nScrollPos)
				continue;
			if (CRect(20,60+count*90,95,135+count*90).PtInRect(point) && it->first != pMajor->GetRaceID() && 
				pMajor->IsRaceContacted(it->first) == true)
			{
				m_sActiveIntelRace = it->first;
				Invalidate(FALSE);
				return;
			}
			if (count++ == 6)
				break;
		}
		if (m_sActiveIntelRace != "" && m_sActiveIntelRace != pMajor->GetRaceID())
		{
			// wurde auf den Button für die Steuerung der Aggressivität geklickt
			if (CRect(400,140,520,170).PtInRect(point))
			{
				BYTE oldAgg = pMajor->GetEmpire()->GetIntelligence()->GetAggressiveness(1, m_sActiveIntelRace);
				if (++oldAgg == 3)
					oldAgg = 0;
				pMajor->GetEmpire()->GetIntelligence()->SetAggressiveness(1, m_sActiveIntelRace, oldAgg);
				Invalidate(FALSE);
				return;
			}
			// wurde in den Bereich des Balkens für das Sabotagedepot geklickt
			if (CRect(200,75,900,115).PtInRect(point))
			{
				// hier Zuweisung vornehmen
				pMajor->GetEmpire()->GetIntelligence()->SetAssignment()->SetSabotagePercentage(4, ((point.x - 196) / 7), m_sActiveIntelRace);
				Invalidate(FALSE);
				return;
			}
			// wurde in den Bereich der Balken zur speziellen Sabotage geklickt
			for (int i = 0; i < 4; i++)
				if (CRect(310, 230+i*90, 610, 260+i*90).PtInRect(point))
				{
					// hier Zuweisung vornehmen
					pMajor->GetEmpire()->GetIntelligence()->SetAssignment()->SetSabotagePercentage(i, ((point.x - 308) / 3), m_sActiveIntelRace);
					Invalidate(FALSE);
					return;
				}
		}
	}
	// befinden wir uns im Geheimdienstinformationsmenü
	else if (m_bySubMenu == 3)
	{
		// Wurde auf das Rassensymbol geklickt um eine Rasse zu aktivieren
		int count = 1;
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			if (CRect(20,60+count*90,95,135+count*90).PtInRect(point) && ((it->first != pMajor->GetRaceID() && 
				pMajor->IsRaceContacted(it->first) == true) || it->first == pMajor->GetRaceID()))
			{
				m_sActiveIntelRace = it->first;
				Invalidate(FALSE);
				return;
			}
			count++;
		}

		// Wurde auf den Button geklickt um die verantwortliche Rasse zu verändern
		if (CRect(715,400,835,430).PtInRect(point))
		{
			CString respRace = pMajor->GetEmpire()->GetIntelligence()->GetResponsibleRace();
			map<CString, CMajor*>::iterator it = pmMajors->find(respRace);			
			while (1)
			{
				++it;
				if (it == pmMajors->end())
					it = pmMajors->begin();
				respRace = it->first;
				if (pMajor->IsRaceContacted(it->first) == true || it->first == pMajor->GetRaceID())
				{
					pMajor->GetEmpire()->GetIntelligence()->SetResponsibleRace(it->first);
					break;
				}
			}
			Invalidate(FALSE);
			return;
		}
		
	}
	// befinden wir uns im Geheimdienstberichtmenü
	else if (m_bySubMenu == 4)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		// Wurde auf eine Tabellenüberschrift geklickt, um die Berichte zu sortieren.
		if (CRect(r.left+100,r.top+100,r.left+200,r.top+130).PtInRect(point))
		{
			// Sortierung der Berichte nach der Runde
			if (m_bSortDesc[0])
				c_arraysort<CObArray, CIntelObject*>(*(pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_round_desc);
			else
				c_arraysort<CObArray, CIntelObject*>(*(pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_round_asc);
			m_bSortDesc[0] = !m_bSortDesc[0];
			Invalidate(FALSE);
			return;
		}
		
		else if (CRect(r.left+200,r.top+100,r.left+600,r.top+130).PtInRect(point))
		{
			// Sortierung der Berichte nach dem Gegner
			if (m_bSortDesc[1])
				c_arraysort<CObArray, CIntelObject*>(*(pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_enemy_desc);
			else
				c_arraysort<CObArray, CIntelObject*>(*(pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_enemy_asc);
			m_bSortDesc[1] = !m_bSortDesc[1];
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+600,r.top+100,r.left+800,r.top+130).PtInRect(point))
		{
			// Sortierung der Berichte nach der Art (Spionage/Sabotage)
			if (m_bSortDesc[2])
				c_arraysort<CObArray, CIntelObject*>(*(pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_kind_desc);
			else
				c_arraysort<CObArray, CIntelObject*>(*(pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_kind_asc);
			m_bSortDesc[2] = !m_bSortDesc[2];
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+800,r.top+100,r.left+1000,r.top+130).PtInRect(point))
		{
			// Sortierung der Berichte nach dem Typ (Wirtschaft, Forschung...)
			if (m_bSortDesc[3])
				c_arraysort<CObArray, CIntelObject*>(*(pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_type_desc);
			else
				c_arraysort<CObArray, CIntelObject*>(*(pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_type_asc);
			m_bSortDesc[3] = !m_bSortDesc[3];
			Invalidate(FALSE);
			return;
		}

		// Wenn wir auf einen Bericht geklickt haben, diese Markieren
		unsigned short j = 0;
		short counter = pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetActiveReport() - 20 + m_iOldClickedIntelReport;
		short add = 0;
		for (int i = 0; i < pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
		{
			if (counter > 0)
			{
				add++;
				counter--;
				continue;
			}
			if (j < 21)
			{
				if (CRect(r.left+50,r.top+140+j*25,r.right-50,r.top+165+j*25).PtInRect(point))
				{
					pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->SetActiveReport(j + add);
					m_iOldClickedIntelReport = 20-(j)%21;
					Invalidate(FALSE);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
					break;
				}
			j++;
			}
		}
	}
	// befinden wir uns im Geheimdienstanschlagsmenü
	else if (m_bySubMenu == 5)
	{
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		short activeReport = pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetActiveReport();

		// prüfen ob wir auf den Button zur Auswahl oder zum Abbrechen eines Anschlags geklickt haben
		if (CRect(400,510,520,540).PtInRect(point) && activeReport != -1)
		{
			//AfxMessageBox(*pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(activeReport)->GetOwnerDesc());
			pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->CreateAttemptObject(
				pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(activeReport));
			Invalidate(FALSE);
			return;
		}
		if (CRect(555,510,675,540).PtInRect(point))
		{
			pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->RemoveAttemptObject();
			Invalidate(FALSE);
			return;
		}
		// Wenn wir auf einen Bericht geklickt haben, diesen markieren
		unsigned short j = 0;
		short counter = activeReport - 10 + m_iOldClickedIntelReport;
		short add = 0;
		// Es werden nur Spionageberichte mit weiteren besonderen Eigenschaften angezeigt
		for (int i = 0; i < pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
		{
			CIntelObject* intelObj = pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(i);
			if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != pMajor->GetRaceID() && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
			{
				if (counter > 0)
				{
					add++;
					counter--;
					continue;
				}
				if (j < 11)
				{
					if (CRect(r.left+50,r.top+140+j*25,r.right-50,r.top+165+j*25).PtInRect(point))
					{
						pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->SetActiveReport(j + add);
						m_iOldClickedIntelReport = 10-(j)%11;
						Invalidate(FALSE);
						pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
						break;
					}
				j++;
				}
			}
		}
	}
	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CIntelMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	
	// Wenn wir uns in der Geheimdienstansicht befinden
	CalcLogicalPoint(point);
	ButtonReactOnMouseOver(point, &m_IntelligenceMainButtons);
	ButtonReactOnMouseOver(point, &m_RaceLogoButtons);
	CMainBaseView::OnMouseMove(nFlags, point);
}

BOOL CIntelMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);

	CIntelligence* pIntel = pMajor->GetEmpire()->GetIntelligence();
	// wenn wir im Geheimdienstberichtsmenü sind
	if (m_bySubMenu == 4)
	{
		if (zDelta < 0)
		{
			if (pIntel->GetIntelReports()->GetNumberOfReports() > pIntel->GetIntelReports()->GetActiveReport() + 1)
			{
				if (m_iOldClickedIntelReport > 0)
					m_iOldClickedIntelReport--;
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() + 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
		else if (zDelta > 0)
		{
			if (pIntel->GetIntelReports()->GetActiveReport() > 0)
			{
				if (pIntel->GetIntelReports()->GetActiveReport() > 20 && m_iOldClickedIntelReport < 20)
					m_iOldClickedIntelReport++;
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() - 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
	}
	// wenn wir im Geheimdienstanschlagsmenü sind
	else if (m_bySubMenu == 5)
	{
		int maxReports = 0;
		// nur Berichte anzeigen, welche für einen Anschlag ausgewählt werden können
		for (int i = 0; i < pIntel->GetIntelReports()->GetNumberOfReports(); i++)
		{
			CIntelObject* intelObj = pIntel->GetIntelReports()->GetReport(i);
			if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != pMajor->GetRaceID() && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
							maxReports++;
		}		
		if (zDelta < 0)
		{
			if (maxReports > pIntel->GetIntelReports()->GetActiveReport() + 1)
			{
				if (m_iOldClickedIntelReport > 0)
					m_iOldClickedIntelReport--;
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() + 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
		else if (zDelta > 0)
		{
			if (pIntel->GetIntelReports()->GetActiveReport() > 0)
			{
				if (pIntel->GetIntelReports()->GetActiveReport() > 10 && m_iOldClickedIntelReport < 10)
					m_iOldClickedIntelReport++;
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() - 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
	}
	// Wenn wir in einem Menü sind, in welchem die Rassenlogos links angezeigt werden
	else if (m_bySubMenu < 4)
	{
		if (zDelta < 0)
		{
			if ((int)pDoc->GetRaceCtrl()->GetMajors()->size() > m_nScrollPos + 6)
			{
				m_nScrollPos++;
				Invalidate(FALSE);
			}
		}
		else if (zDelta > 0)
		{
			if (m_nScrollPos > 0)
			{
				m_nScrollPos--;
				Invalidate(FALSE);
			}
		}
	}
	return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

void CIntelMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

	CIntelligence* pIntel = pMajor->GetEmpire()->GetIntelligence();

	// wenn wir in der Geheimdienstberichtsansicht sind
	if (m_bySubMenu == 4)
	{
		if (nChar == VK_DOWN)
		{
			if (pIntel->GetIntelReports()->GetNumberOfReports() > pIntel->GetIntelReports()->GetActiveReport() + 1)
			{
				if (m_iOldClickedIntelReport > 0)
					m_iOldClickedIntelReport--;
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() + 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
		else if (nChar == VK_UP)
		{
			if (pIntel->GetIntelReports()->GetActiveReport() > 0)
			{
				if (pIntel->GetIntelReports()->GetActiveReport() > 20 && m_iOldClickedIntelReport < 20)
					m_iOldClickedIntelReport++;
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() - 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
	}
	// wenn wir im Geheimdienstanschlagsmenü sind
	else if (m_bySubMenu == 5)
	{
		int maxReports = 0;
		// nur Berichte anzeigen, welche für einen Anschlag ausgewählt werden können
		for (int i = 0; i < pIntel->GetIntelReports()->GetNumberOfReports(); i++)
		{
			CIntelObject* intelObj = pIntel->GetIntelReports()->GetReport(i);
			if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != pMajor->GetRaceID() && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
				maxReports++;
		}		
		if (nChar == VK_DOWN)
		{
			if (maxReports > pIntel->GetIntelReports()->GetActiveReport() + 1)
			{
				if (m_iOldClickedIntelReport > 0)
					m_iOldClickedIntelReport--;
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() + 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
		else if (nChar == VK_UP)
		{
			if (pIntel->GetIntelReports()->GetActiveReport() > 0)
			{
				if (pIntel->GetIntelReports()->GetActiveReport() > 10 && m_iOldClickedIntelReport < 10)
					m_iOldClickedIntelReport++;
				pIntel->GetIntelReports()->SetActiveReport(pIntel->GetIntelReports()->GetActiveReport() - 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CIntelMenuView::CreateButtons()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CString sPrefix = pMajor->GetPrefix();
	// alle Buttons in der View anlegen und Grafiken laden
	
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	// Buttons in der Geheimdienstansicht
	m_IntelligenceMainButtons.Add(new CMyButton(CPoint(35,690), CSize(160,40), CResourceManager::GetString("BTN_ASSIGNMENT"), fileN, fileI, fileA));
	m_IntelligenceMainButtons.Add(new CMyButton(CPoint(195,690), CSize(160,40), CResourceManager::GetString("BTN_SPY"), fileN, fileI, fileA));
	m_IntelligenceMainButtons.Add(new CMyButton(CPoint(355,690), CSize(160,40), CResourceManager::GetString("BTN_SABOTAGE"), fileN, fileI, fileA));
	m_IntelligenceMainButtons.Add(new CMyButton(CPoint(515,690), CSize(160,40), CResourceManager::GetString("INFORMATION"), fileN, fileI, fileA));
	m_IntelligenceMainButtons.Add(new CMyButton(CPoint(675,690), CSize(160,40), CResourceManager::GetString("BTN_REPORTS"), fileN, fileI, fileA));
	m_IntelligenceMainButtons.Add(new CMyButton(CPoint(835,690), CSize(160,40), CResourceManager::GetString("BTN_ATTEMPT"), fileN, fileI, fileA));	

	// Buttons um Rassenlogos durchschalten zu können
	fileN = "Other\\" + sPrefix + "buttonminus.bop";
	fileA = "Other\\" + sPrefix + "buttonminusa.bop";
	m_RaceLogoButtons.Add(new CMyButton(CPoint(45,130) , CSize(25,25), "", fileN, fileN, fileA));
	fileN = "Other\\" + sPrefix + "buttonplus.bop";
	fileA = "Other\\" + sPrefix + "buttonplusa.bop";
	m_RaceLogoButtons.Add(new CMyButton(CPoint(45,660) , CSize(25,25), "", fileN, fileN, fileA));	
}