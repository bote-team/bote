// IntelMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "IntelMenuView.h"
#include "IntelBottomView.h"


// CIntelMenuView

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
}

BEGIN_MESSAGE_MAP(CIntelMenuView, CMainBaseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CIntelMenuView drawing

void CIntelMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here
	CMemDC pDC(dc);
	pDC->SetBkMode(TRANSPARENT);
	if (pDoc->m_pIniLoader->GetValue("SMOOTHSCALING"))
		pDC->SetStretchBltMode(HALFTONE);
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	LoadRaceFont(pDC);
	// ***************************** DIE GEHEIMDIENSTANSICHT ZEICHNEN **********************************
	if (m_bySubMenu == 0)
		DrawIntelAssignmentMenu(pDC, r);
	else if (m_bySubMenu == 1)
		DrawIntelSpyMenu(pDC, r);
	else if (m_bySubMenu == 2)
		DrawIntelSabotageMenu(pDC, r);
	else if (m_bySubMenu == 3)
		DrawIntelInfoMenu(pDC, r);
	else if (m_bySubMenu == 4)
		DrawIntelReportsMenu(pDC, r);
	else if (m_bySubMenu == 5)
		DrawIntelAttackMenu(pDC, r);	
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
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	CreateButtons();

	bg_intelassignmenu.DeleteObject();
	bg_intelspymenu.DeleteObject();
	bg_intelsabmenu.DeleteObject();
	bg_intelreportmenu.DeleteObject();
	bg_intelinfomenu.DeleteObject();
	bg_intelattackmenu.DeleteObject();

	CString race;
	switch (pDoc->GetPlayersRace())
	{
	case HUMAN:		race = CResourceManager::GetString("RACE1_PREFIX"); break;
	case FERENGI:	race = CResourceManager::GetString("RACE2_PREFIX"); break;
	case KLINGON:	race = CResourceManager::GetString("RACE3_PREFIX"); break;
	case ROMULAN:	race = CResourceManager::GetString("RACE4_PREFIX"); break;
	case CARDASSIAN:race = CResourceManager::GetString("RACE5_PREFIX"); break;
	case DOMINION:	race = CResourceManager::GetString("RACE6_PREFIX"); break;
	}
	FCObjImage img;
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"intelassignmenu.jpg");
	bg_intelassignmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"intelspymenu.jpg");
	bg_intelspymenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"intelsabmenu.jpg");
	bg_intelsabmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"intelreportmenu.jpg");
	bg_intelreportmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"intelinfomenu.jpg");
	bg_intelinfomenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"intelattackmenu.jpg");
	bg_intelattackmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Destroy();

	// Geheimdienstansicht
	m_bySubMenu = 0;
	m_byActiveIntelRace = 0;
	// kleine Rassensymbole laden
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		CBitmap* logo = NULL;
		switch (i)
		{
		case HUMAN:		logo = pDoc->GetGraphicPool()->GetGraphic("RaceLogos\\Race1.jpg");	break;
		case FERENGI:	logo = pDoc->GetGraphicPool()->GetGraphic("RaceLogos\\Race2.jpg");	break;
		case KLINGON:	logo = pDoc->GetGraphicPool()->GetGraphic("RaceLogos\\Race3.jpg");	break;
		case ROMULAN:	logo = pDoc->GetGraphicPool()->GetGraphic("RaceLogos\\Race4.jpg");	break;
		case CARDASSIAN:logo = pDoc->GetGraphicPool()->GetGraphic("RaceLogos\\Race5.jpg");	break;
		case DOMINION:	logo = pDoc->GetGraphicPool()->GetGraphic("RaceLogos\\Race6.jpg");	break;
		}
		if (logo != NULL)
		{
			m_RaceLogos[i-1].Detach();
			m_RaceLogos[i-1].Attach(*logo);
			FCObjImage img;
			FCWin32::CreateImageFromDDB((HBITMAP)logo->GetSafeHandle(), img);
			img.ConvertTo24Bit();
			img.SinglePixelProcessProc(FCPixelBrightness(0));
			img.SinglePixelProcessProc(FCPixelGamma(0.6));
			m_RaceLogosDark[i-1].Detach();
			m_RaceLogosDark[i-1].Attach(FCWin32::CreateDDBHandle(img));
		}
	}
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
void CIntelMenuView::DrawIntelAssignmentMenu(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	COLORREF oldColor = pDC->GetTextColor();
	CString s;
	CRect timber[100];
	
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_intelassignmenu);
	pDC->BitBlt(0,0,1075,750, &mdc, 0, 0, SRCCOPY);

/*	CPen mark(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&mark);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.top+120);
	pDC->LineTo(theClientRect.right,theClientRect.top+120);
	pDC->MoveTo(theClientRect.left+100,theClientRect.top+120);
	pDC->LineTo(theClientRect.left+100,theClientRect.bottom-60);
	pDC->MoveTo(theClientRect.right-250,theClientRect.top+120);
	pDC->LineTo(theClientRect.right-250,theClientRect.bottom-60);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-60);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-60);
*/
	// rechtes Informationsmenü zeichnen
	DrawIntelInformation(pDC);
	pDC->SetTextColor(oldColor);

	CPen pen(PS_NULL, 0, RGB(42,46,30));
	pDC->SelectObject(&pen);
	CBrush brush;

	// die einzelnen Rassensymbole zeichnen
	DrawRaceLogosInIntelView(pDC);
	
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		// den Spionage- und Sabotagebalken zeichnen
		BYTE spyPerc = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetGlobalSpyPercentage(i);
		s.Format("%d%%", spyPerc);
		pDC->DrawText(s, CRect(415,80+i*90,490,110+i*90), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		BYTE sabPerc = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(i);
		s.Format("%d%%", sabPerc);
		pDC->DrawText(s, CRect(775,80+i*90,850,110+i*90), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		// den Zuweisungsbalken zeichnen
		for (int j = 0; j < 100; j++)
		{
			brush.DeleteObject();
			if (pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == FALSE || i == pDoc->GetPlayersRace())
				brush.CreateSolidBrush(RGB(22,26,15));
			else if (j < spyPerc)
			{
				COLORREF color = RGB(250-j*2.5,50+j*2,0);
				brush.CreateSolidBrush(color);
			}
			else
				brush.CreateSolidBrush(RGB(42,46,30));
			pDC->SelectObject(&brush);
			timber[j].SetRect(110+j*3, 80+i*90, 112+j*3, 110+i*90);
			pDC->Rectangle(&timber[j]);
			
			brush.DeleteObject();
			if (pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == FALSE || i == pDoc->GetPlayersRace())
				brush.CreateSolidBrush(RGB(22,26,15));	
			else if (j < sabPerc)
			{
				COLORREF color = RGB(250-j*2.5,50+j*2,0);
				brush.CreateSolidBrush(color);				
			}
			else
				brush.CreateSolidBrush(RGB(42,46,30));
			pDC->SelectObject(&brush);
			timber[j].SetRect(470+j*3, 80+i*90, 472+j*3, 110+i*90);
			pDC->Rectangle(&timber[j]);
		}
	}
	// Spionage und Sabotage oben über die Balken zeichnen
	CFont font;
	CFontLoader::CreateFont(pDoc->GetPlayersRace(), 3, &font);
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("SPY"), CRect(110,130,409,160), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SABOTAGE"), CRect(470,130,769,160), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// den Balken für die innere Sicherheit zeichnen
	BYTE innerSecurityPerc = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetInnerSecurityPercentage();
	pDC->DrawText(CResourceManager::GetString("INNER_SECURITY")+":", CRect(20,70,190,120), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", innerSecurityPerc);
	pDC->DrawText(s, CRect(915,70,theClientRect.right,120), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	font.DeleteObject();

	for (int i = 0; i < 100; i++)
	{
		brush.DeleteObject();
		if (i < innerSecurityPerc)
		{
			COLORREF color = RGB(250-i*2.5,50+i*2,0);
			brush.CreateSolidBrush(color);			
		}
		else
			brush.CreateSolidBrush(RGB(42,46,30));
		pDC->SelectObject(&brush);
		timber[i].SetRect(200+i*7, 75, 205+i*7, 115);
		pDC->Rectangle(&timber[i]);
	}

	// Buttons am unteren Bildschirmrand zeichnen
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_IntelligenceMainButtons, m_bySubMenu);
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("SECURITY_HEADQUARTERS"),
		CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CIntelMenuView::DrawIntelSpyMenu(CDC *pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	COLORREF oldColor = pDC->GetTextColor();
	CString s;
	CRect timber[100];
	
	if (m_byActiveIntelRace == pDoc->GetPlayersRace())
		m_byActiveIntelRace = NOBODY;
	// Wenn noch keine Rasse ausgewählt wurde, so wird versucht eine bekannte Rasse auszuwählen
	if (m_byActiveIntelRace == NOBODY)
		for (int i = HUMAN; i <= DOMINION; i++)
			if (i != pDoc->GetPlayersRace() && pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE)
			{
				m_byActiveIntelRace = i;
				break;
			}
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_intelspymenu);
	pDC->BitBlt(0,0,1075,750, &mdc, 0, 0, SRCCOPY);

/*	CPen mark(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&mark);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.top+120);
	pDC->LineTo(theClientRect.right,theClientRect.top+120);
	pDC->MoveTo(theClientRect.left+100,theClientRect.top+120);
	pDC->LineTo(theClientRect.left+100,theClientRect.bottom-60);
	pDC->MoveTo(theClientRect.right-250,theClientRect.top+120);
	pDC->LineTo(theClientRect.right-250,theClientRect.bottom-60);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-60);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-60);
*/
	// kleinen Button mit welchem man die Aggressivität einstellen kann zeichnen
	if (m_byActiveIntelRace != NOBODY)
	{
		pDC->DrawText(CResourceManager::GetString("AGGRESSIVENESS")+":" , CRect(200,140,390,170), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		LoadFontForLittleButton(pDC);
		mdc.SelectObject(bm);
		pDC->BitBlt(400,140,120,30,&mdc,0,0,SRCCOPY);
		switch(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAggressiveness(0,m_byActiveIntelRace))
		{
		case 0: pDC->DrawText(CResourceManager::GetString("CAREFUL"), CRect(400,140,520,170), DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
		case 1: pDC->DrawText(CResourceManager::GetString("NORMAL"), CRect(400,140,520,170), DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
		case 2: pDC->DrawText(CResourceManager::GetString("AGGRESSIVE"), CRect(400,140,520,170), DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
		}
		pDC->SetTextColor(oldColor);
	}

	// die einzelnen Rassensymbole zeichnen
	DrawRaceLogosInIntelView(pDC);
	// rechtes Informationsmenü zeichnen
	DrawIntelInformation(pDC);
	
	pDC->SetTextColor(oldColor);

	CPen pen(PS_NULL, 0, RGB(42,46,30));
	pDC->SelectObject(&pen);
	CBrush brush;

	// Die einzelnen Spionagezuweisungsbalken zeichnen
	if (m_byActiveIntelRace != NOBODY)
	{
		// Bild der aktiven Rasse im Hintergrund zeichnen
		mdc.SelectObject(m_RaceLogosDark[m_byActiveIntelRace-1]);
		int oldStretchMode = pDC->GetStretchBltMode();
		pDC->SetStretchBltMode(HALFTONE);
		pDC->StretchBlt(310,230,300,300,&mdc,0,0,200,200,SRCPAINT);
		pDC->SetStretchBltMode(oldStretchMode);

		pDC->DrawText(CResourceManager::GetString("ECONOMY")+":", CRect(100,230,290,260), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("SCIENCE")+":", CRect(100,320,290,350), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("MILITARY")+":", CRect(100,410,290,440), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("DIPLOMACY")+":", CRect(100,500,290,530), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		for (int i = 0; i < 4; i++)
		{
			BYTE spyPerc = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetSpyPercentages(m_byActiveIntelRace, i);
			int gp =  pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetSecurityPoints() *
				pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetGlobalSpyPercentage(m_byActiveIntelRace) *
				pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetSpyPercentages(m_byActiveIntelRace, i) / 10000
				// + den Depotwert
				+ (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetSPStorage(0, m_byActiveIntelRace) * pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetSpyPercentages(m_byActiveIntelRace, i) / 100);
			// eventuellen Geheimdienstbonus noch dazurechnen
			gp += gp * pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetBonus(i, 0) / 100;
			s.Format("%d%% (%d %s)", spyPerc, gp, CResourceManager::GetString("SP"));
			pDC->DrawText(s, CRect(625,230+i*90,900,260+i*90), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			// den Zuweisungsbalken zeichnen
			for (int j = 0; j < 100; j++)
			{
				brush.DeleteObject();
				if (j < spyPerc)
				{
					COLORREF color = RGB(250-j*2.5,50+j*2,0);
					brush.CreateSolidBrush(color);
				}
				else
					brush.CreateSolidBrush(RGB(42,46,30));
				pDC->SelectObject(&brush);
				timber[j].SetRect(310+j*3, 230+i*90, 312+j*3, 260+i*90);
				pDC->Rectangle(&timber[j]);
			}
		}
		s.Format("%d",pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetGlobalSpyPercentage(m_byActiveIntelRace));
		pDC->DrawText(CResourceManager::GetString("SPY_OF_ALL", FALSE, s), CRect(100,180,theClientRect.right-250,210), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		// kurze Erklärung und Punkte im Depot hinschreiben
		CString race;
		switch (m_byActiveIntelRace)
		{
		case HUMAN:		{race = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE"); break;}
		case FERENGI:	{race = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE"); break;}
		case KLINGON:	{race = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE"); break;}
		case ROMULAN:	{race = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE"); break;}
		case CARDASSIAN:{race = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE"); break;}
		case DOMINION:	{race = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE"); break;}
		}
		s.Format("%d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetSPStorage(0, m_byActiveIntelRace));
		pDC->DrawText(CResourceManager::GetString("USE_SP_FROM_DEPOT", FALSE, s, race), CRect(150,550,theClientRect.right-300,650), DT_CENTER | DT_WORDBREAK);
	}

	CFont font;
	CFontLoader::CreateFont(pDoc->GetPlayersRace(), 3, &font);
	pDC->SelectObject(&font);
	
	// den Balken für die "was ins Lager kommt" Spionagepunkte zeichnen
	BYTE spyToStore = 100;
	if (m_byActiveIntelRace != NOBODY)
		for (int i = 0; i < 4; i++)
			spyToStore -= pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetSpyPercentages(m_byActiveIntelRace, i);
	pDC->DrawText(CResourceManager::GetString("INTEL_RESERVE")+":", CRect(20,70,190,120), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", spyToStore);
	pDC->DrawText(s, CRect(915,70,theClientRect.right,120), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	for (int i = 0; i < 100; i++)
	{
		brush.DeleteObject();
		if (i < spyToStore)
		{
			COLORREF color = RGB(250-i*2.5,50+i*2,0);
			brush.CreateSolidBrush(color);			
		}
		else
			brush.CreateSolidBrush(RGB(42,46,30));
		pDC->SelectObject(&brush);
		timber[i].SetRect(200+i*7, 75, 205+i*7, 115);
		pDC->Rectangle(&timber[i]);
	}
	
	// Buttons am unteren Bildschirmrand zeichnen
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_IntelligenceMainButtons, m_bySubMenu);
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	font.DeleteObject();
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("SPY"),
		CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CIntelMenuView::DrawIntelSabotageMenu(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	COLORREF oldColor = pDC->GetTextColor();
	CString s;
	CRect timber[100];
	
	if (m_byActiveIntelRace == pDoc->GetPlayersRace())
		m_byActiveIntelRace = NOBODY;
	// Wenn noch keine Rasse ausgewählt wurde, so wird versucht eine bekannte Rasse auszuwählen
	if (m_byActiveIntelRace == NOBODY)
		for (int i = HUMAN; i <= DOMINION; i++)
			if (i != pDoc->GetPlayersRace() && pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE)
			{
				m_byActiveIntelRace = i;
				break;
			}
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_intelsabmenu);
	pDC->BitBlt(0,0,1075,750, &mdc, 0, 0, SRCCOPY);

/*	CPen mark(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&mark);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.top+120);
	pDC->LineTo(theClientRect.right,theClientRect.top+120);
	pDC->MoveTo(theClientRect.left+100,theClientRect.top+120);
	pDC->LineTo(theClientRect.left+100,theClientRect.bottom-60);
	pDC->MoveTo(theClientRect.right-250,theClientRect.top+120);
	pDC->LineTo(theClientRect.right-250,theClientRect.bottom-60);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-60);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-60);
*/
	// kleinen Button mit welchem man die Aggressivität einstellen kann zeichnen
	if (m_byActiveIntelRace != NOBODY)
	{
		pDC->DrawText(CResourceManager::GetString("AGGRESSIVENESS")+":" , CRect(200,140,390,170), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		LoadFontForLittleButton(pDC);
		mdc.SelectObject(bm);
		pDC->BitBlt(400,140,120,30,&mdc,0,0,SRCCOPY);
		switch(pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAggressiveness(1,m_byActiveIntelRace))
		{
		case 0: pDC->DrawText(CResourceManager::GetString("CAREFUL"), CRect(400,140,520,170), DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
		case 1: pDC->DrawText(CResourceManager::GetString("NORMAL"), CRect(400,140,520,170), DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
		case 2: pDC->DrawText(CResourceManager::GetString("AGGRESSIVE"), CRect(400,140,520,170), DT_CENTER | DT_VCENTER | DT_SINGLELINE); break;
		}
		pDC->SetTextColor(oldColor);
	}

	// die einzelnen Rassensymbole zeichnen
	DrawRaceLogosInIntelView(pDC);
	// rechtes Informationsmenü zeichnen
	DrawIntelInformation(pDC);
	pDC->SetTextColor(oldColor);

	CPen pen(PS_NULL, 0, RGB(42,46,30));
	pDC->SelectObject(&pen);
	CBrush brush;

	// Die einzelnen Sabotagezuweisungsbalken zeichnen
	if (m_byActiveIntelRace != NOBODY)
	{
		// Bild der aktiven Rasse im Hintergrund zeichnen
		mdc.SelectObject(m_RaceLogosDark[m_byActiveIntelRace-1]);
		int oldStretchMode = pDC->GetStretchBltMode();
		pDC->SetStretchBltMode(HALFTONE);
		pDC->StretchBlt(310,230,300,300,&mdc,0,0,200,200,SRCPAINT);
		pDC->SetStretchBltMode(oldStretchMode);

		pDC->DrawText(CResourceManager::GetString("ECONOMY")+":", CRect(100,230,290,260), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("SCIENCE")+":", CRect(100,320,290,350), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("MILITARY")+":", CRect(100,410,290,440), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("DIPLOMACY")+":", CRect(100,500,290,530), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		for (int i = 0; i < 4; i++)
		{
			BYTE sabPerc = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetSabotagePercentages(m_byActiveIntelRace, i);
			int gp =  pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetSecurityPoints() *
				pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(m_byActiveIntelRace) *
				pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetSabotagePercentages(m_byActiveIntelRace, i) / 10000
				// + den Depotwert
				+ (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetSPStorage(1, m_byActiveIntelRace) * pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetSabotagePercentages(m_byActiveIntelRace, i) / 100);
			// eventuellen Geheimdienstbonus noch dazurechnen
			gp += gp * pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetBonus(i, 1) / 100;
			s.Format("%d%% (%d %s)", sabPerc, gp, CResourceManager::GetString("SP"));
			pDC->DrawText(s, CRect(625,230+i*90,900,260+i*90), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			// den Zuweisungsbalken zeichnen
			for (int j = 0; j < 100; j++)
			{
				brush.DeleteObject();
				if (j < sabPerc)
				{
					COLORREF color = RGB(250-j*2.5,50+j*2,0);
					brush.CreateSolidBrush(color);
				}
				else
					brush.CreateSolidBrush(RGB(42,46,30));
				pDC->SelectObject(&brush);
				timber[j].SetRect(310+j*3, 230+i*90, 312+j*3, 260+i*90);
				pDC->Rectangle(&timber[j]);
			}
		}
		s.Format("%d",pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetGlobalSabotagePercentage(m_byActiveIntelRace));
		pDC->DrawText(CResourceManager::GetString("SABOTAGE_OF_ALL", FALSE, s), CRect(100,180,theClientRect.right-250,210), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		// kurze Erklärung und Punkte im Depot hinschreiben
		CString race;
		switch (m_byActiveIntelRace)
		{
		case HUMAN:		{race = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE"); break;}
		case FERENGI:	{race = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE"); break;}
		case KLINGON:	{race = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE"); break;}
		case ROMULAN:	{race = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE"); break;}
		case CARDASSIAN:{race = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE"); break;}
		case DOMINION:	{race = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE"); break;}
		}
		s.Format("%d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetSPStorage(1, m_byActiveIntelRace));
		pDC->DrawText(CResourceManager::GetString("USE_SP_FROM_DEPOT", FALSE, s, race), CRect(150,550,theClientRect.right-300,650), DT_CENTER | DT_WORDBREAK);
	}

	CFont font;
	CFontLoader::CreateFont(pDoc->GetPlayersRace(), 3, &font);
	pDC->SelectObject(&font);
	
	// den Balken für die "was ins Lager kommt" Sabotagepunkte zeichnen
	BYTE sabToStore = 100;
	if (m_byActiveIntelRace != NOBODY)
		for (int i = 0; i < 4; i++)
			sabToStore -= pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAssignment()->GetSabotagePercentages(m_byActiveIntelRace, i);
	pDC->DrawText(CResourceManager::GetString("INTEL_RESERVE")+":", CRect(20,70,190,120), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", sabToStore);
	pDC->DrawText(s, CRect(915,70,theClientRect.right,120), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	for (int i = 0; i < 100; i++)
	{
		brush.DeleteObject();
		if (i < sabToStore)
		{
			COLORREF color = RGB(250-i*2.5,50+i*2,0);
			brush.CreateSolidBrush(color);			
		}
		else
			brush.CreateSolidBrush(RGB(42,46,30));
		pDC->SelectObject(&brush);
		timber[i].SetRect(200+i*7, 75, 205+i*7, 115);
		pDC->Rectangle(&timber[i]);
	}	
	// Buttons am unteren Bildschirmrand zeichnen
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_IntelligenceMainButtons, m_bySubMenu);
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	font.DeleteObject();
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("SABOTAGE"),
		CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CIntelMenuView::DrawIntelReportsMenu(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	COLORREF oldColor = pDC->GetTextColor();
	CRect r = theClientRect;
	CString s;
	BYTE race = pDoc->GetPlayersRace();
		
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_intelreportmenu);
	pDC->BitBlt(0,0,1075,750, &mdc, 0, 0, SRCCOPY);

/*	CPen line(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&line);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-60);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-60);
*/
	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	CPen mark;
	COLORREF markColor;
	if (pDoc->GetPlayersRace() == HUMAN)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
	}
	else if (pDoc->GetPlayersRace() == FERENGI)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
	}
	else if (race == CARDASSIAN)
	{
		markColor = RGB(255,128,0);
		mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
	}
	else
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
	}
	pDC->SelectObject(&mark);

	// Es gehen nur 21 Berichte auf die Seite, deshalb muss abgebrochen werden
	// wenn noch kein Bericht angeklickt wurde, es aber Berichte gibt, dann den ersten Bericht in der Reihe markieren
	if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() == -1 && pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports() > 0)
	{
		pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(0);
		m_iOldClickedIntelReport = 0;
	}

	int j = 0;
	short counter = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 20 + m_iOldClickedIntelReport;
	short oldClickedNews = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport();
	for (int i = 0; i < pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
	{
		if (counter > 0)
		{
			pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 1);
			counter--;
			continue;
		}
		if (j < 21)
		{
			CIntelObject* intelObj = (CIntelObject*)pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAllReports()->GetAt(i);
			// Die News markieren
			if (j == pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport())
			{
				pDC->SetTextColor(markColor);
				pDC->MoveTo(r.left+100,r.top+140+j*25); pDC->LineTo(r.right-100,r.top+140+j*25);
				pDC->MoveTo(r.left+100,r.top+140+j*25+25); pDC->LineTo(r.right-100,r.top+140+j*25+25);
			}
			else
			{
				// Wenn wir selbst Opfer sind, dann Text grau darstellen
				if (intelObj->GetEnemy() == pDoc->GetPlayersRace())
					pDC->SetTextColor(RGB(150,150,150));
				else
					pDC->SetTextColor(oldColor);
			}
			
			s.Format("%d",	intelObj->GetRound());
			pDC->DrawText(s, CRect(r.left+100,r.top+140+j*25,r.left+200,r.top+165+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			switch (intelObj->GetEnemy())
			{
				case HUMAN:		{s = CResourceManager::GetString("RACE1_EMPIRE"); break;}
				case FERENGI:	{s = CResourceManager::GetString("RACE2_EMPIRE"); break;}
				case KLINGON:	{s = CResourceManager::GetString("RACE3_EMPIRE"); break;}
				case ROMULAN:	{s = CResourceManager::GetString("RACE4_EMPIRE"); break;}
				case CARDASSIAN:{s = CResourceManager::GetString("RACE5_EMPIRE"); break;}
				case DOMINION:	{s = CResourceManager::GetString("RACE6_EMPIRE"); break;}
			}
			pDC->DrawText(s, CRect(r.left+200,r.top+140+j*25,r.left+600,r.top+165+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			if (intelObj->GetIsSpy())
				s = CResourceManager::GetString("SPY");
			else
				s = CResourceManager::GetString("SABOTAGE");
			pDC->DrawText(s, CRect(r.left+600,r.top+140+j*25,r.left+800,r.top+165+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			switch (intelObj->GetType())
			{
			case 0: s = CResourceManager::GetString("ECONOMY"); break;
			case 1: s = CResourceManager::GetString("SCIENCE"); break;
			case 2: s = CResourceManager::GetString("MILITARY"); break;
			case 3: s = CResourceManager::GetString("DIPLOMACY"); break;
			default: s = CResourceManager::GetString("UNKNOWN");
			}
			pDC->DrawText(s, CRect(r.left+800,r.top+140+j*25,r.left+1000,r.top+165+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			pDC->SetTextColor(oldColor);
			j++;
		}
	}
	pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(oldClickedNews);
		
	// Buttons am unteren Bildschirmrand zeichnen
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_IntelligenceMainButtons, m_bySubMenu);

	// Tabellenüberschriften zeichnen
	if (pDoc->GetPlayersRace() == ROMULAN)
		pDC->SetTextColor(RGB(4,75,35));
	else
		pDC->SetTextColor(RGB(200,200,200));
	pDC->DrawText(CResourceManager::GetString("ROUND"),CRect(r.left+100,r.top+100,r.left+200,r.top+130), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("ENEMY")+" ("+(CResourceManager::GetString("TARGET"))+")",CRect(r.left+200,r.top+100,r.left+600,r.top+130), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("KIND"),CRect(r.left+600,r.top+100,r.left+800,r.top+130), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("TYPE"),CRect(r.left+800,r.top+100,r.left+1000,r.top+130), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	s.Format(" (%d)", pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports());
	pDC->DrawText(CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("REPORTS")+s,
		CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CIntelMenuView::DrawIntelAttackMenu(CDC *pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	COLORREF oldColor = pDC->GetTextColor();
	CRect r = theClientRect;
	CString s;
	BYTE race = pDoc->GetPlayersRace();
		
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_intelattackmenu);
	pDC->BitBlt(0,0,1075,750, &mdc, 0, 0, SRCCOPY);

/*	CPen line(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&line);
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-60);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-60);
	pDC->MoveTo(theClientRect.left, theClientRect.top+430);
	pDC->LineTo(theClientRect.right,theClientRect.top+430);
*/
	// Farbe der Schrift und Markierung wählen, wenn wir auf eine Rasse geklickt haben
	CPen mark;
	COLORREF markColor;
	if (pDoc->GetPlayersRace() == HUMAN)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(219,111,194));
	}
	else if (pDoc->GetPlayersRace() == FERENGI)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(195,195,0));
	}
	else if (pDoc->GetPlayersRace() == KLINGON)
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(250,80,30));
	}
	else if (race == CARDASSIAN)
	{
		markColor = RGB(255,128,0);
		mark.CreatePen(PS_SOLID, 1, RGB(74,146,138));
	}
	else
	{
		markColor = RGB(220,220,220);
		mark.CreatePen(PS_SOLID, 1, RGB(140,196,203));
	}
	pDC->SelectObject(&mark);
	
	// Es gehen nur 10 Berichte auf die Seite, deshalb muss abgebrochen werden
	// wenn noch kein Bericht angeklickt wurde, es aber Berichte gibt, dann den ersten Bericht in der Reihe markieren
	int numberOfReports = 0;
	if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() == -1)
	{
		for (int i = 0; i < pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
		{
			CIntelObject* intelObj = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(i);
			if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != pDoc->GetPlayersRace() && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
				numberOfReports++;
		}
		if (numberOfReports > 0)
		{
			pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(0);
			m_iOldClickedIntelReport = 0;
		}
	}
	int j = 0;
	short counter = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 10 + m_iOldClickedIntelReport;
	short oldClickedNews = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport();
	numberOfReports = 0;
	short activeReport = 0;
	for (int i = 0; i < pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
	{
		CIntelObject* intelObj = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(i);
		if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != pDoc->GetPlayersRace() && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
		{
			numberOfReports++;
			if (counter > 0)
			{
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 1);
				counter--;
				continue;
			}
			if (j < 11)
			{
				// Die News markieren
				if (j == pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport())
				{
					activeReport = i;
					pDC->SetTextColor(markColor);
					pDC->MoveTo(r.left+100,r.top+140+j*25); pDC->LineTo(r.right-100,r.top+140+j*25);
					pDC->MoveTo(r.left+100,r.top+140+j*25+25); pDC->LineTo(r.right-100,r.top+140+j*25+25);
				//	s.Format("aktiver Report: %d\n", activeReport);
				//	AfxMessageBox(s + *intelObj->GetOwnerDesc() + "\n\n" + *intelObj->GetEnemyDesc());
				}
				else
					pDC->SetTextColor(oldColor);
								
				s.Format("%d",	intelObj->GetRound());
				pDC->DrawText(s, CRect(r.left+100,r.top+140+j*25,r.left+200,r.top+165+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				switch (intelObj->GetEnemy())
				{
					case HUMAN:		{s = CResourceManager::GetString("RACE1_EMPIRE"); break;}
					case FERENGI:	{s = CResourceManager::GetString("RACE2_EMPIRE"); break;}
					case KLINGON:	{s = CResourceManager::GetString("RACE3_EMPIRE"); break;}
					case ROMULAN:	{s = CResourceManager::GetString("RACE4_EMPIRE"); break;}
					case CARDASSIAN:{s = CResourceManager::GetString("RACE5_EMPIRE"); break;}
					case DOMINION:	{s = CResourceManager::GetString("RACE6_EMPIRE"); break;}
				}
				pDC->DrawText(s, CRect(r.left+200,r.top+140+j*25,r.left+600,r.top+165+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				if (intelObj->GetIsSpy())
					s = CResourceManager::GetString("SPY");
				else if (intelObj->GetIsSabotage())
					s = CResourceManager::GetString("SABOTAGE");
				pDC->DrawText(s, CRect(r.left+600,r.top+140+j*25,r.left+800,r.top+165+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				switch (intelObj->GetType())
				{
				case 0: s = CResourceManager::GetString("ECONOMY"); break;
				case 1: s = CResourceManager::GetString("SCIENCE"); break;
				case 2: s = CResourceManager::GetString("MILITARY"); break;
				case 3: s = CResourceManager::GetString("DIPLOMACY"); break;
				default: s = CResourceManager::GetString("UNKNOWN");
				}
				pDC->DrawText(s, CRect(r.left+800,r.top+140+j*25,r.left+1000,r.top+165+j*25), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				pDC->SetTextColor(oldColor);
				j++;
			}
		}
	}
//	pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(oldClickedNews);
	pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(activeReport);
	
	// Beschreibung und Auswahlmöglichkeiten zeichnen
	pDC->DrawText(CResourceManager::GetString("ATTEMPT_DESC"), CRect(r.left+100, r.top+450, r.right-100, r.top+500), DT_CENTER | DT_WORDBREAK);
	if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAttemptObject())
	{
		pDC->SetTextColor(RGB(200,200,200));
		CIntelObject* attemptObj = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAttemptObject();
		s.Format("%d",	attemptObj->GetRound());
		pDC->DrawText(s, CRect(r.left+100,565,r.left+200,590), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		switch (attemptObj->GetEnemy())
		{
			case HUMAN:		{s = CResourceManager::GetString("RACE1_EMPIRE"); break;}
			case FERENGI:	{s = CResourceManager::GetString("RACE2_EMPIRE"); break;}
			case KLINGON:	{s = CResourceManager::GetString("RACE3_EMPIRE"); break;}
			case ROMULAN:	{s = CResourceManager::GetString("RACE4_EMPIRE"); break;}
			case CARDASSIAN:{s = CResourceManager::GetString("RACE5_EMPIRE"); break;}
			case DOMINION:	{s = CResourceManager::GetString("RACE6_EMPIRE"); break;}
		}
		pDC->DrawText(s, CRect(r.left+200,565,r.left+600,590), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		if (attemptObj->GetIsSpy())
			s = CResourceManager::GetString("SPY");
		else if (attemptObj->GetIsSabotage())
			s = CResourceManager::GetString("SABOTAGE");
		pDC->DrawText(s, CRect(r.left+600,565,r.left+800,590), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		switch (attemptObj->GetType())
		{
		case 0: s = CResourceManager::GetString("ECONOMY"); break;
		case 1: s = CResourceManager::GetString("SCIENCE"); break;
		case 2: s = CResourceManager::GetString("MILITARY"); break;
		case 3: s = CResourceManager::GetString("DIPLOMACY"); break;
		default: s = CResourceManager::GetString("UNKNOWN");
		}
		pDC->DrawText(s, CRect(r.left+800,565,r.left+1000,590), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->SetTextColor(oldColor);
		pDC->DrawText(*attemptObj->GetOwnerDesc(), CRect(100,600,975,700), DT_CENTER | DT_WORDBREAK);
	}
		
	LoadFontForLittleButton(pDC);
	mdc.SelectObject(bm);
	pDC->BitBlt(400, 510, 120, 30, &mdc, 0, 0, SRCCOPY);
	pDC->BitBlt(555, 510, 120, 30, &mdc, 0, 0, SRCCOPY);
	pDC->DrawText(CResourceManager::GetString("BTN_SELECT"), CRect(400,510,520,540), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("BTN_CANCEL"), CRect(555,510,675,540), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
		
	// Buttons am unteren Bildschirmrand zeichnen
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_IntelligenceMainButtons, m_bySubMenu);

	// Tabellenüberschriften zeichnen
	if (pDoc->GetPlayersRace() == ROMULAN)
		pDC->SetTextColor(RGB(4,75,35));
	else
		pDC->SetTextColor(RGB(200,200,200));
	pDC->DrawText(CResourceManager::GetString("ROUND"),CRect(r.left+100,r.top+100,r.left+200,r.top+130), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("ENEMY")+" ("+(CResourceManager::GetString("TARGET"))+")",CRect(r.left+200,r.top+100,r.left+600,r.top+130), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("KIND"),CRect(r.left+600,r.top+100,r.left+800,r.top+130), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("TYPE"),CRect(r.left+800,r.top+100,r.left+1000,r.top+130), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	s.Format(" (%d)", numberOfReports);
	pDC->DrawText(CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("POSSIBLE_ATTEMPTS")+s,
		CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CIntelMenuView::DrawIntelInfoMenu(CDC *pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	// Daten für die Geheimdienstinformationen berechnen lassen. 
	// Dies wird einmalig pro Runde durchgeführt, sobald man in das Geheimdienstinfomenü geht
	pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelInfo()->CalcIntelInfo(pDoc, pDoc->GetPlayersRace());

	COLORREF oldColor = pDC->GetTextColor();
	CString s;
		
	// Wenn noch keine Rasse ausgewählt wurde, so wird versucht eine bekannte Rasse auszuwählen
	if (m_byActiveIntelRace == NOBODY)
		for (int i = HUMAN; i <= DOMINION; i++)
			if (pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE)
			{
				m_byActiveIntelRace = i;
				break;
			}
	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	mdc.SelectObject(bg_intelinfomenu);
	pDC->BitBlt(0,0,1075,750, &mdc, 0, 0, SRCCOPY);

/*	CPen mark(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&mark);
//	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
//	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.left,theClientRect.top+120);
	pDC->LineTo(theClientRect.right,theClientRect.top+120);
	pDC->MoveTo(theClientRect.left+100,theClientRect.top+120);
	pDC->LineTo(theClientRect.left+100,theClientRect.bottom-60);
//	pDC->MoveTo(theClientRect.right-250,theClientRect.top+120);
//	pDC->LineTo(theClientRect.right-250,theClientRect.bottom-60);
	pDC->MoveTo(theClientRect.left,theClientRect.bottom-60);
	pDC->LineTo(theClientRect.right,theClientRect.bottom-60);
*/
	// Die einzelnen Spionagezuweisungsbalken zeichnen
	if (m_byActiveIntelRace != NOBODY)
	{
		// Bild der aktiven Rasse im Hintergrund zeichnen
		mdc.SelectObject(m_RaceLogosDark[m_byActiveIntelRace-1]);
		int oldStretchMode = pDC->GetStretchBltMode();
		pDC->SetStretchBltMode(HALFTONE);
		pDC->StretchBlt(210,260,300,300,&mdc,0,0,200,200,SRCCOPY);
		pDC->SetStretchBltMode(oldStretchMode);

		s.Format("%s:", CResourceManager::GetString("CONTROLLED_SECTORS"));
		pDC->DrawText(s, CRect(100,285,425,310), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		s.Format(" %d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelInfo()->GetControlledSectors(m_byActiveIntelRace));
		pDC->DrawText(s, CRect(425,285,600,310), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		s.Format("%s:", CResourceManager::GetString("CONTROLLED_SYSTEMS"));
		pDC->DrawText(s, CRect(100,335,425,360), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		s.Format(" %d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelInfo()->GetOwnedSystems(m_byActiveIntelRace));
		pDC->DrawText(s, CRect(425,335,600,360), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		s.Format("%s:", CResourceManager::GetString("INHABITED_SYSTEMS"));
		pDC->DrawText(s, CRect(100,385,425,410), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		s.Format(" %d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelInfo()->GetInhabitedSystems(m_byActiveIntelRace));
		pDC->DrawText(s, CRect(425,385,600,410), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		s.Format("%s:", CResourceManager::GetString("KNOWN_MINORRACES"));
		pDC->DrawText(s, CRect(100,435,425,460), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		s.Format(" %d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelInfo()->GetKnownMinors(m_byActiveIntelRace));
		pDC->DrawText(s, CRect(425,435,600,460), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		s.Format("%s:", CResourceManager::GetString("NUMBER_OF_MINORMEMBERS"));
		pDC->DrawText(s, CRect(100,485,425,510), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		s.Format(" %d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelInfo()->GetMinorMembers(m_byActiveIntelRace));
		pDC->DrawText(s, CRect(425,485,600,510), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	// die einzelnen Rassensymbole zeichnen
	DrawRaceLogosInIntelView(pDC, TRUE);
	int oldStretchMode = pDC->GetStretchBltMode();
	pDC->SetStretchBltMode(HALFTONE);
	mdc.SelectObject(m_RaceLogos[pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetResponsibleRace()-1]);
	pDC->StretchBlt(737,435,75,75,&mdc,0,0,200,200,SRCCOPY);
	pDC->SetStretchBltMode(oldStretchMode);

	pDC->DrawText(CResourceManager::GetString("CHOOSE_RESPONSIBLE_RACE"), CRect(600,285,950,385), DT_CENTER | DT_WORDBREAK);
	LoadFontForLittleButton(pDC);
	mdc.SelectObject(bm);
	pDC->BitBlt(715,400,120,30,&mdc,0,0,SRCCOPY);
	switch (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetResponsibleRace())
	{
		case HUMAN:		{s = CResourceManager::GetString("RACE1");	break;}
		case FERENGI:	{s = CResourceManager::GetString("RACE2");	break;}
		case KLINGON:	{s = CResourceManager::GetString("RACE3");	break;}
		case ROMULAN:	{s = CResourceManager::GetString("RACE4");	break;}
		case CARDASSIAN:{s = CResourceManager::GetString("RACE5");	break;}
		case DOMINION:	{s = CResourceManager::GetString("RACE6");	break;}
		default: s = "Error";
	}
	pDC->DrawText(s, CRect(715,400,835,430), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		
	// Buttons am unteren Bildschirmrand zeichnen
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_IntelligenceMainButtons, m_bySubMenu);
	
	// Geheimdienst mit größerer Schrift in der Mitte zeichnen
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	pDC->DrawText(CResourceManager::GetString("SECURITY")+" - "+CResourceManager::GetString("INFORMATION"),
		CRect(theClientRect.left,theClientRect.top+10,theClientRect.right,theClientRect.top+60), DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CIntelMenuView::DrawRaceLogosInIntelView(CDC *pDC, BOOLEAN highlightPlayersRace)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	int oldStretchMode = pDC->GetStretchBltMode();
	pDC->SetStretchBltMode(HALFTONE);
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (i != pDoc->GetPlayersRace() && pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE
			|| i == pDoc->GetPlayersRace() && highlightPlayersRace == TRUE)
			mdc.SelectObject(m_RaceLogos[i-1]);
		else
			mdc.SelectObject(m_RaceLogosDark[i-1]);
		pDC->StretchBlt(20,60+i*90,75,75,&mdc,0,0,200,200,SRCCOPY);
	}
	pDC->SetStretchBltMode(oldStretchMode);
}

void CIntelMenuView::DrawIntelInformation(CDC *pDC)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	CString s;

	pDC->DrawText(CResourceManager::GetString("SECURITYPOINTS"), CRect(825,130,1075,155), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SECURITYBONI"), CRect(825,200,1075,225), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SPY"), CRect(825,270,1075,295), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SABOTAGE"), CRect(825,410,1075,435), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("DEPOTS"), CRect(825,550,1075,575), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	pDC->SetTextColor(RGB(200,200,200));
	pDC->DrawText(CResourceManager::GetString("TOTAL").MakeUpper()+":", CRect(850,165,1040,190), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d "+CResourceManager::GetString("SP"), pDoc->GetEmpire(pDoc->GetPlayersRace())->GetSP());
	pDC->DrawText(s,  CRect(860,165,1040,190), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("INNER_SECURITY_SHORT").MakeUpper()+":",  CRect(850,235,1040,260), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetInnerSecurityBoni());
	pDC->DrawText(s, CRect(850,235,1040,260), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("ECONOMY").MakeUpper()+":", CRect(850,305,1040,330), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetEconomyBonus(0));
	pDC->DrawText(s, CRect(850,305,1040,330), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SCIENCE").MakeUpper()+":", CRect(850,340,1040,365), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetScienceBonus(0));
	pDC->DrawText(s, CRect(850,340,1040,365), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("MILITARY").MakeUpper()+":", CRect(850,375,1040,400), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetMilitaryBonus(0));
	pDC->DrawText(s, CRect(850,375,1040,400), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("ECONOMY").MakeUpper()+":", CRect(850,445,1040,470), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetEconomyBonus(1));
	pDC->DrawText(s, CRect(850,445,1040,470), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("SCIENCE").MakeUpper()+":", CRect(850,480,1040,505), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetScienceBonus(1));
	pDC->DrawText(s, CRect(850,480,1040,505), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	pDC->DrawText(CResourceManager::GetString("MILITARY").MakeUpper()+":", CRect(850,515,1040,540), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d%%", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetMilitaryBonus(1));
	pDC->DrawText(s, CRect(850,515,1040,540), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	// Depotlager
	pDC->DrawText(CResourceManager::GetString("INNER_SECURITY_SHORT").MakeUpper()+":", CRect(850,585,1040,610), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetInnerSecurityStorage());
	pDC->DrawText(s, CRect(850,585,1040,610), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	if (m_byActiveIntelRace)
	{
		pDC->DrawText(CResourceManager::GetString("SPY").MakeUpper()+":", CRect(850,620,1040,645), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		s.Format("%d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetSPStorage(0, m_byActiveIntelRace));
		pDC->DrawText(s, CRect(850,620,1040,645), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		pDC->DrawText(CResourceManager::GetString("SABOTAGE").MakeUpper()+":", CRect(850,655,1040,680), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		s.Format("%d", pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetSPStorage(1, m_byActiveIntelRace));
		pDC->DrawText(s, CRect(850,655,1040,680), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}
}

void CIntelMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);
	// Wenn wir uns in der Geheimdienstansicht befinden		
	// Checken, ob ich auf einen Button geklickt habe um in ein anderes Untermenü zu gelangen
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_IntelligenceMainButtons, temp))
	{
		m_bySubMenu = temp;
		// Wenn wir ins Anschlagsmenü gehen, dann den aktiven Bericht auf keinen setzen
		if (temp = 5)
			pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetIntelReports()->SetActiveReport(-1);
		pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
		return;
	}		
	// befinden wir uns in der globalen Zuweisungsansicht (IntelAssignmentMenu)
	if (m_bySubMenu == 0)
	{
		// wurde in den Bereich des Balkens der inneren Sicherheit geklickt
		if (CRect(200,75,900,115).PtInRect(point))
		{
			// hier Zuweisung vornehmen
			pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAssignment()->SetGlobalPercentage
				(2, ((point.x - 196) / 7), pDoc->GetMajorRace(pDoc->GetPlayersRace()), NOBODY, pDoc->GetPlayersRace());
			Invalidate(FALSE);
			return;
		}
		
		for (int i = HUMAN; i <= DOMINION; i++)
		{
			if (i != pDoc->GetPlayersRace() && pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i))
			{
				// wurde in den Bereich der Balken zur Spionage geklickt
				if (CRect(110, 80+i*90, 410, 110+i*90).PtInRect(point))
				{
					// hier Zuweisung vornehmen
					pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAssignment()->SetGlobalPercentage
						(0, ((point.x - 108) / 3), pDoc->GetMajorRace(pDoc->GetPlayersRace()), i, pDoc->GetPlayersRace());
					m_byActiveIntelRace = i;
					Invalidate(FALSE);
					return;
				}
				// wurde in den Bereich der Balken zur Sabotage geklickt
				else if (CRect(470, 80+i*90, 770, 110+i*90).PtInRect(point))
				{
					// hier Zuweisung vornehmen
					pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAssignment()->SetGlobalPercentage
						(1, ((point.x - 468) / 3), pDoc->GetMajorRace(pDoc->GetPlayersRace()), i, pDoc->GetPlayersRace());
					m_byActiveIntelRace = i;
					Invalidate(FALSE);
					return;
				}
			}
		}
	}
	// befinden wir uns im Spionagemenü (IntelSpyMenu)
	else if (m_bySubMenu == 1)
	{
		// Wurde auf das Rassensymbol geklickt um eine Rasse zu aktivieren
		for (int i = HUMAN; i <= DOMINION; i++)
			if (CRect(20,60+i*90,95,135+i*90).PtInRect(point) && i != pDoc->GetPlayersRace() && 
				pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE)
			{
				m_byActiveIntelRace = i;
				Invalidate(FALSE);
				return;
			}

		if (m_byActiveIntelRace != NOBODY && m_byActiveIntelRace != pDoc->GetPlayersRace())
		{
			// wurde auf den Button für die Steuerung der Aggressivität geklickt
			if (CRect(400,140,520,170).PtInRect(point))
			{
				BYTE oldAgg = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAggressiveness(0, m_byActiveIntelRace);
				if (++oldAgg == 3)
					oldAgg = 0;
				pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAggressiveness(0, m_byActiveIntelRace, oldAgg);
				Invalidate(FALSE);
				return;
			}
			// wurde in den Bereich des Balkens für das Spionagedepot geklickt
			if (CRect(200,75,900,115).PtInRect(point))
			{
				// hier Zuweisung vornehmen
				pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAssignment()->SetSpyPercentage
					(4, ((point.x - 196) / 7), m_byActiveIntelRace);
				Invalidate(FALSE);
				return;
			}
			// wurde in den Bereich der Balken zur speziellen Spionage geklickt
			for (int i = 0; i < 4; i++)
				if (CRect(310, 230+i*90, 610, 260+i*90).PtInRect(point))
				{
					// hier Zuweisung vornehmen
					pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAssignment()->SetSpyPercentage
						(i, ((point.x - 308) / 3), m_byActiveIntelRace);
					Invalidate(FALSE);
					return;
				}
		}
	}
	// befinden wir uns im Sabotagemenü (IntelSabotageMenu)
	else if (m_bySubMenu == 2)
	{
		// Wurde auf das Rassensymbol geklickt um eine Rasse zu aktivieren
		for (int i = HUMAN; i <= DOMINION; i++)
			if (CRect(20,60+i*90,95,135+i*90).PtInRect(point) && i != pDoc->GetPlayersRace() && 
				pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE)
			{
				m_byActiveIntelRace = i;
				Invalidate(FALSE);
				return;
			}

		if (m_byActiveIntelRace != NOBODY && m_byActiveIntelRace != pDoc->GetPlayersRace())
		{
			// wurde auf den Button für die Steuerung der Aggressivität geklickt
			if (CRect(400,140,520,170).PtInRect(point))
			{
				BYTE oldAgg = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetAggressiveness(1, m_byActiveIntelRace);
				if (++oldAgg == 3)
					oldAgg = 0;
				pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAggressiveness(1, m_byActiveIntelRace, oldAgg);
				Invalidate(FALSE);
				return;
			}
			// wurde in den Bereich des Balkens für das Sabotagedepot geklickt
			if (CRect(200,75,900,115).PtInRect(point))
			{
				// hier Zuweisung vornehmen
				pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAssignment()->SetSabotagePercentage
					(4, ((point.x - 196) / 7), m_byActiveIntelRace);
				Invalidate(FALSE);
				return;
			}
			// wurde in den Bereich der Balken zur speziellen Sabotage geklickt
			for (int i = 0; i < 4; i++)
				if (CRect(310, 230+i*90, 610, 260+i*90).PtInRect(point))
				{
					// hier Zuweisung vornehmen
					pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetAssignment()->SetSabotagePercentage
						(i, ((point.x - 308) / 3), m_byActiveIntelRace);
					Invalidate(FALSE);
					return;
				}
		}
	}
	// befinden wir uns im Geheimdienstinformationsmenü
	else if (m_bySubMenu == 3)
	{
		// Wurde auf das Rassensymbol geklickt um eine Rasse zu aktivieren
		for (int i = HUMAN; i <= DOMINION; i++)
			if (CRect(20,60+i*90,95,135+i*90).PtInRect(point) && ((i != pDoc->GetPlayersRace() && 
				pDoc->m_MajorRace[pDoc->GetPlayersRace()].GetKnownMajorRace(i) == TRUE) || i == pDoc->GetPlayersRace()))
			{
				m_byActiveIntelRace = i;
				Invalidate(FALSE);
				return;
			}

		// Wurde auf den Button geklickt um die verantwortliche Rasse zu verändern
		if (CRect(715,400,835,430).PtInRect(point))
		{
			BYTE respRace = pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->GetResponsibleRace();
			while (1)
			{
				if (respRace == DOMINION)
					respRace = HUMAN;
				else
					respRace++;
				if (pDoc->GetMajorRace(pDoc->GetPlayersRace())->GetKnownMajorRace(respRace) == TRUE || respRace == pDoc->GetPlayersRace())
				{
					pDoc->GetEmpire(pDoc->GetPlayersRace())->GetIntelligence()->SetResponsibleRace(respRace);
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
		BYTE race = pDoc->GetPlayersRace();
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		// Wurde auf eine Tabellenüberschrift geklickt, um die Berichte zu sortieren.
	/*	if (CRect(r.left+100,r.top+100,r.left+200,r.top+130).PtInRect(point))
		{
			// Sortierung der Berichte nach der Runde
			c_arraysort<CArray<CIntelObject*>, CIntelObject*>(*(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_round);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+200,r.top+100,r.left+600,r.top+130).PtInRect(point))
		{
			// Sortierung der Berichte nach dem Gegner
			c_arraysort<CArray<CIntelObject*>, CIntelObject*>(*(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_enemy);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+600,r.top+100,r.left+800,r.top+130).PtInRect(point))
		{
			// Sortierung der Berichte nach der Art (Spionage/Sabotage)
			c_arraysort<CArray<CIntelObject*>, CIntelObject>(*(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_kind);
			Invalidate(FALSE);
			return;
		}
		else if (CRect(r.left+800,r.top+100,r.left+1000,r.top+130).PtInRect(point))
		{
			// Sortierung der Berichte nach dem Typ (Wirtschaft, Forschung...)
			c_arraysort<CArray<CIntelObject*>, CIntelObject>(*(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetAllReports()), CIntelObject::sort_by_type);
			Invalidate(FALSE);
			return;
		}
*/
		// Wenn wir auf einen Bericht geklickt haben, diese Markieren
		unsigned short j = 0;
		short counter = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 20 + m_iOldClickedIntelReport;
		short add = 0;
		for (int i = 0; i < pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
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
					pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(j + add);
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
		BYTE race = pDoc->GetPlayersRace();
		CRect r;
		r.SetRect(0,0,m_TotalSize.cx,m_TotalSize.cy);
		short activeReport = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport();

		// prüfen ob wir auf den Button zur Auswahl oder zum Abbrechen eines Anschlags geklickt haben
		if (CRect(400,510,520,540).PtInRect(point) && activeReport != -1)
		{
			//AfxMessageBox(*pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(activeReport)->GetOwnerDesc());
			pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->CreateAttemptObject(
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(activeReport));
			Invalidate(FALSE);
			return;
		}
		if (CRect(555,510,675,540).PtInRect(point))
		{
			pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->RemoveAttemptObject();
			Invalidate(FALSE);
			return;
		}
		// Wenn wir auf einen Bericht geklickt haben, diesen markieren
		unsigned short j = 0;
		short counter = activeReport - 10 + m_iOldClickedIntelReport;
		short add = 0;
		// Es werden nur Spionageberichte mit weiteren besonderen Eigenschaften angezeigt
		for (int i = 0; i < pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
		{
			CIntelObject* intelObj = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(i);
			if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != race && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
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
						pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(j + add);
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
	// Wenn wir uns in der Geheimdienstansicht befinden
	CalcLogicalPoint(point);
	ButtonReactOnMouseOver(point, &m_IntelligenceMainButtons);
	CMainBaseView::OnMouseMove(nFlags, point);
}

BOOL CIntelMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// wenn wir im Geheimdienstberichtsmenü sind
	if (m_bySubMenu == 4)
	{
		BYTE race = pDoc->GetPlayersRace();
		if (zDelta < 0)
		{
			if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports() > pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() + 1)
			{
				if (m_iOldClickedIntelReport > 0)
					m_iOldClickedIntelReport--;
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() + 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
		else if (zDelta > 0)
		{
			if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() > 0)
			{
				if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() > 20 && m_iOldClickedIntelReport < 20)
					m_iOldClickedIntelReport++;
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
	}
	// wenn wir im Geheimdienstanschlagsmenü sind
	else if (m_bySubMenu == 5)
	{
		BYTE race = pDoc->GetPlayersRace();
		int maxReports = 0;
		// nur Berichte anzeigen, welche für einen Anschlag ausgewählt werden können
		for (int i = 0; i < pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
		{
			CIntelObject* intelObj = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(i);
			if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != race && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
							maxReports++;
		}		
		if (zDelta < 0)
		{
			if (maxReports > pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() + 1)
			{
				if (m_iOldClickedIntelReport > 0)
					m_iOldClickedIntelReport--;
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() + 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
		else if (zDelta > 0)
		{
			if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() > 0)
			{
				if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() > 10 && m_iOldClickedIntelReport < 10)
					m_iOldClickedIntelReport++;
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
	}
	return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

void CIntelMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// wenn wir in der Geheimdienstberichtsansicht sind
	if (m_bySubMenu == 4)
	{
		BYTE race = pDoc->GetPlayersRace();
		if (nChar == VK_DOWN)
		{
			if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports() > pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() + 1)
			{
				if (m_iOldClickedIntelReport > 0)
					m_iOldClickedIntelReport--;
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() + 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
		else if (nChar == VK_UP)
		{
			if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() > 0)
			{
				if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() > 20 && m_iOldClickedIntelReport < 20)
					m_iOldClickedIntelReport++;
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
	}
	// wenn wir im Geheimdienstanschlagsmenü sind
	else if (m_bySubMenu == 5)
	{
		BYTE race = pDoc->GetPlayersRace();
		int maxReports = 0;
		// nur Berichte anzeigen, welche für einen Anschlag ausgewählt werden können
		for (int i = 0; i < pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
		{
			CIntelObject* intelObj = pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetReport(i);
			if (intelObj->GetIsSpy() == TRUE && intelObj->GetEnemy() != race && intelObj->GetRound() > pDoc->GetCurrentRound() - 10)
							maxReports++;
		}		
		if (nChar == VK_DOWN)
		{
			if (maxReports > pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() + 1)
			{
				if (m_iOldClickedIntelReport > 0)
					m_iOldClickedIntelReport--;
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() + 1);
				Invalidate(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CIntelBottomView));
			}
		}
		else if (nChar == VK_UP)
		{
			if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() > 0)
			{
				if (pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() > 10 && m_iOldClickedIntelReport < 10)
					m_iOldClickedIntelReport++;
				pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->SetActiveReport(pDoc->GetEmpire(race)->GetIntelligence()->GetIntelReports()->GetActiveReport() - 1);
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
	// alle Buttons in der View anlegen und Grafiken laden
	switch(pDoc->GetPlayersRace())
	{
	case HUMAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2i.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2a.jpg";
			// Buttons in der Geheimdienstansicht
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_ASSIGNMENT"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SPY"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SABOTAGE"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("INFORMATION"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_REPORTS"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(860,690), CSize(160,40), CResourceManager::GetString("BTN_ATTEMPT"), fileN, fileI, fileA));
			break;
		}
	case FERENGI:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttona.jpg";
			// Buttons in der Geheimdienstansicht
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_ASSIGNMENT"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SPY"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SABOTAGE"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("INFORMATION"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_REPORTS"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(860,690), CSize(160,40), CResourceManager::GetString("BTN_ATTEMPT"), fileN, fileI, fileA));
			break;
		}
	case KLINGON:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "buttona.jpg";
			// Buttons in der Geheimdienstansicht
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_ASSIGNMENT"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SPY"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SABOTAGE"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("INFORMATION"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_REPORTS"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(860,690), CSize(160,40), CResourceManager::GetString("BTN_ATTEMPT"), fileN, fileI, fileA));
			break;
		}
	case ROMULAN:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttona.jpg";
			// Buttons in der Geheimdienstansicht
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_ASSIGNMENT"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SPY"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SABOTAGE"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("INFORMATION"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_REPORTS"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(860,690), CSize(160,40), CResourceManager::GetString("BTN_ATTEMPT"), fileN, fileI, fileA));
			break;
		}
	case CARDASSIAN:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttona.jpg";
			// Buttons in der Geheimdienstansicht
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_ASSIGNMENT"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SPY"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SABOTAGE"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("INFORMATION"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_REPORTS"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(860,690), CSize(160,40), CResourceManager::GetString("BTN_ATTEMPT"), fileN, fileI, fileA));
			break;
		}
	case DOMINION:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttona.jpg";
			// Buttons in der Geheimdienstansicht
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(10,690), CSize(160,40), CResourceManager::GetString("BTN_ASSIGNMENT"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_SPY"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_SABOTAGE"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("INFORMATION"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_REPORTS"), fileN, fileI, fileA));
			m_IntelligenceMainButtons.Add(new CMyButton(CPoint(860,690), CSize(160,40), CResourceManager::GetString("BTN_ATTEMPT"), fileN, fileI, fileA));
			break;
		}
	}
}