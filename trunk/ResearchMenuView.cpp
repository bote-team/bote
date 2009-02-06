// ResearchMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "ResearchMenuView.h"
#include "ResearchBottomView.h"


// CResearchMenuView

IMPLEMENT_DYNCREATE(CResearchMenuView, CMainBaseView)

CResearchMenuView::CResearchMenuView()
{

}

CResearchMenuView::~CResearchMenuView()
{
	for (int i = 0; i < m_ResearchMainButtons.GetSize(); i++)
	{
		delete m_ResearchMainButtons[i];
		m_ResearchMainButtons[i] = 0;
	}
	m_ResearchMainButtons.RemoveAll();
}

BEGIN_MESSAGE_MAP(CResearchMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CResearchMenuView drawing

void CResearchMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// TODO: add draw code here
	CMemDC pDC(dc);
	pDC->SetBkMode(TRANSPARENT);
	if (pDoc->m_pIniLoader->GetValue("SMOOTHSCALING"))
		pDC->SetStretchBltMode(HALFTONE);
	CRect r(0, 0, m_TotalSize.cx, m_TotalSize.cy);

	LoadRaceFont(pDC);
	// ***************************** DIE FORSCHUNGSANSICHT ZEICHNEN **********************************
	if (m_bySubMenu == 0)
		DrawResearchMenue(pDC,r);
	else if (m_bySubMenu == 1)
		DrawUniqueResearchMenue(pDC,r);
	// ************** DIE FORSCHUNGSANSICHT ZEICHNEN ist hier zu Ende **************
}

// CResearchMenuView diagnostics

#ifdef _DEBUG
void CResearchMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CResearchMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CResearchMenuView message handlers

void CResearchMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();

	// Alle Buttons in der View erstellen
	CreateButtons();
	// alle Hintergrundgrafiken laden
	bg_researchmenu.DeleteObject();
	bg_urmenu.DeleteObject();
	bg_emptyur.DeleteObject();
	
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
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"researchmenu.jpg");
	bg_researchmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"urmenu.jpg");
	bg_urmenu.Attach(FCWin32::CreateDDBHandle(img));
	img.Load(*((CBotf2App*)AfxGetApp())->GetPath() + "Graphics/Backgrounds/"+race+"emptyur.jpg");
	bg_emptyur.Attach(FCWin32::CreateDDBHandle(img));
	img.Destroy();

	// Zuweisungsbalken in der Forschungsübersicht
	// Die Koodinaten der Rechtecke für die ganzen Buttons
	CRect r;
	GetClientRect(&r);
	// i == 0->Bio, 1->Energy, 2->Computer, 3->Bautechnik, 4->Antrieb, 5->Waffen, 6->Unique
	for (int i = 0; i < 3; i++)	// Unique Forschung muß seperat initialisiert werden, daher nur bis 5
		for (int j = 0; j < 101; j++)
			// Wir haben ne Breite von 200 für die 100 Striche zur Auswahl -> jeder Strich 2 breit
			ResearchTimber[i][j].SetRect(r.left+33+j*2+i*260,r.top+350,r.left+33+j*2+1+i*260,r.top+350+20);
	// Jetzt die Striche für die unteren Forschungsgebiete
	for (int i = 3; i < 6; i++)	// Unique Forschung muß seperat initialisiert werden, daher nur bis 5
		for (int j = 0; j < 101; j++)
			// Wir haben ne Breite von 200 für die 100 Striche zur Auswahl -> jeder Strich 2 breit
			ResearchTimber[i][j].SetRect(r.left+33+j*2+(i-3)*260,r.top+693,r.left+33+j*2+1+(i-3)*260,r.top+693+20);
	// Nun alles für die Unique Forschung
	for (int i = 0; i < 101; i++)
		// Wir haben ne Breite von 200 für die 100 Striche zur Auswahl -> ieder Strich 2 breit
		ResearchTimber[6][i].SetRect(r.left+33+i*2,r.top+350,r.left+33+i*2+1,r.top+350+20);
	LockStatusRect[6].SetRect(r.left+75,r.top+120,r.left+195,r.top+290);
	
	// Die kleinen Rechtecke für den Status, ob die Forschung gelocked ist oder nicht
	for (int i = 0; i < 3; i++)
		LockStatusRect[i].SetRect(r.left+75+i*260,r.top+170,r.left+195+i*260,r.top+290);
	for (int i = 3; i < 6; i++)
		LockStatusRect[i].SetRect(r.left+75+(i-3)*260,r.top+510,r.left+195+(i-3)*260,r.top+633);
	m_bySubMenu = 0;
}

void CResearchMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class

	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

BOOL CResearchMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Forschungsmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CResearchMenuView::DrawResearchMenue(CDC *pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
/*	CBrush nb(RGB(0,0,0));
	pDC->SelectObject(nb);
	pDC->Rectangle(theClientRect);
*/
	CBitmap* graphic = NULL;
	HGDIOBJ oldGraphic;

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);

	mdc.SelectObject(bg_researchmenu);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);
	
	COLORREF oldColor = pDC->GetTextColor();
/*	CPen mark(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&mark);

	// grobe Linien zeichnen
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.right-250,theClientRect.top+70);
	pDC->LineTo(theClientRect.right-250,theClientRect.bottom);
*/
	CString s;
	CRect rect;
	USHORT level;
	USHORT race = pDoc->GetPlayersRace();
	USHORT techPercentage[7];
		
	int oldStretchMode = pDC->GetStretchBltMode();
	pDC->SetStretchBltMode(HALFTONE);
	// großen Rechtecke für die einzelnen Forschungsgebiete zeichnen
	// Biogenetik
	CRect TechRect;
	TechRect.SetRect(theClientRect.left+10,theClientRect.top+80,theClientRect.left+260,theClientRect.top+380);
	//pDC->Rectangle(TechRect);
	level = pDoc->GetEmpire(race)->GetResearch()->GetBioTech();
	s.Format("%s - %s %d",CResourceManager::GetString("BIOTECH"),CResourceManager::GetString("LEVEL"),level);
	pDC->DrawText(s,TechRect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	s = pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetTechName(0);
	pDC->DrawText(s,CRect(10,110,260,135), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	graphic = pDoc->GetGraphicPool()->GetGraphic("Research\\biotech.jpg");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->StretchBlt(60,140,150,125,&mdc,0,0,270,225,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	rect.SetRect(theClientRect.left+10,theClientRect.top+290,theClientRect.left+260,theClientRect.top+315);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pDoc->GetEmpire(race)->GetResearch()->GetBioFP()*100)/(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetBio(level))));
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(theClientRect.left+10,theClientRect.top+315,theClientRect.left+260,theClientRect.top+340);
	techPercentage[0] = pDoc->GetEmpire(race)->GetResearch()->GetBioPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[0]);
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Bautechnik
	TechRect.SetRect(theClientRect.left+10,theClientRect.bottom-327,theClientRect.left+260,theClientRect.bottom-27);
//	pDC->Rectangle(TechRect);
	level = pDoc->GetEmpire(race)->GetResearch()->GetConstructionTech();
	s.Format("%s - %s %d",CResourceManager::GetString("CONSTRUCTIONTECH"),CResourceManager::GetString("LEVEL"),level);
	pDC->DrawText(s,TechRect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	s = pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetTechName(4);
	pDC->DrawText(s,CRect(10,455,260,480), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	graphic = pDoc->GetGraphicPool()->GetGraphic("Research\\constructiontech.jpg");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->StretchBlt(60,480,150,125,&mdc,0,0,270,225,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	rect.SetRect(theClientRect.left+10,theClientRect.bottom-117,theClientRect.left+260,theClientRect.bottom-92);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pDoc->GetEmpire(race)->GetResearch()->GetConstructionFP()*100)/(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetConstruction(level))));
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(theClientRect.left+10,theClientRect.bottom-92,theClientRect.left+260,theClientRect.bottom-67);
	techPercentage[3] = pDoc->GetEmpire(race)->GetResearch()->GetConstructionPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[3]);
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Energietechnik
	TechRect.SetRect(theClientRect.left+270,theClientRect.top+80,theClientRect.left+520,theClientRect.top+380);
//	pDC->Rectangle(TechRect);
	level = pDoc->GetEmpire(race)->GetResearch()->GetEnergyTech();
	s.Format("%s - %s %d",CResourceManager::GetString("ENERGYTECH"),CResourceManager::GetString("LEVEL"),level);
	pDC->DrawText(s,TechRect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	s = pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetTechName(1);
	pDC->DrawText(s,CRect(270,110,520,135), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	graphic = pDoc->GetGraphicPool()->GetGraphic("Research\\energytech.jpg");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->StretchBlt(320,140,150,125,&mdc,0,0,270,225,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	rect.SetRect(theClientRect.left+270,theClientRect.top+290,theClientRect.left+520,theClientRect.top+315);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pDoc->GetEmpire(race)->GetResearch()->GetEnergyFP()*100)/(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetEnergy(level))));
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(theClientRect.left+270,theClientRect.top+315,theClientRect.left+520,theClientRect.top+340);
	techPercentage[1] = pDoc->GetEmpire(race)->GetResearch()->GetEnergyPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[1]);
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Antriebstechnik
	TechRect.SetRect(theClientRect.left+270,theClientRect.bottom-327,theClientRect.left+520,theClientRect.bottom-27);
//	pDC->Rectangle(TechRect);
	level = pDoc->GetEmpire(race)->GetResearch()->GetPropulsionTech();
	s.Format("%s - %s %d",CResourceManager::GetString("PROPULSIONTECH"),CResourceManager::GetString("LEVEL"),level);
	pDC->DrawText(s,TechRect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	s = pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetTechName(3);
	pDC->DrawText(s,CRect(270,455,520,480), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	graphic = pDoc->GetGraphicPool()->GetGraphic("Research\\propulsiontech.jpg");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->StretchBlt(320,480,150,125,&mdc,0,0,270,225,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	rect.SetRect(theClientRect.left+295,theClientRect.bottom-117,theClientRect.left+520,theClientRect.bottom-92);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pDoc->GetEmpire(race)->GetResearch()->GetPropulsionFP()*100)/(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetPropulsion(level))));
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(theClientRect.left+295,theClientRect.bottom-92,theClientRect.left+520,theClientRect.bottom-67);
	techPercentage[4] = pDoc->GetEmpire(race)->GetResearch()->GetPropulsionPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[4]);
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Computertechnik
	TechRect.SetRect(theClientRect.left+530,theClientRect.top+80,theClientRect.left+780,theClientRect.top+380);
//	pDC->Rectangle(TechRect);
	level = pDoc->GetEmpire(race)->GetResearch()->GetCompTech();
	s.Format("%s - %s %d",CResourceManager::GetString("COMPUTERTECH"),CResourceManager::GetString("LEVEL"),level);
	pDC->DrawText(s,TechRect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	s = pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetTechName(2);
	pDC->DrawText(s,CRect(530,110,780,135), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	graphic = pDoc->GetGraphicPool()->GetGraphic("Research\\computertech.jpg");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->StretchBlt(580,140,150,125,&mdc,0,0,270,225,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}	
	rect.SetRect(theClientRect.left+555,theClientRect.top+290,theClientRect.left+780,theClientRect.top+315);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pDoc->GetEmpire(race)->GetResearch()->GetComputerFP()*100)/(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetComp(level))));
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(theClientRect.left+555,theClientRect.top+315,theClientRect.left+780,theClientRect.top+340);
	techPercentage[2] = pDoc->GetEmpire(race)->GetResearch()->GetCompPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[2]);
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Waffentechnik
	TechRect.SetRect(theClientRect.left+530,theClientRect.bottom-327,theClientRect.left+780,theClientRect.bottom-17);
//	pDC->Rectangle(TechRect);
	level = pDoc->GetEmpire(race)->GetResearch()->GetWeaponTech();
	s.Format("%s - %s %d",CResourceManager::GetString("WEAPONTECH"),CResourceManager::GetString("LEVEL"),level);
	pDC->DrawText(s,TechRect,DT_CENTER | DT_TOP | DT_SINGLELINE);
	s = pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetTechName(5);
	pDC->DrawText(s,CRect(530,455,780,480), DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);
	graphic = pDoc->GetGraphicPool()->GetGraphic("Research\\weapontech.jpg");
	if (graphic)
	{
		oldGraphic = mdc.SelectObject(*graphic);
		pDC->StretchBlt(580,480,150,125,&mdc,0,0,270,225,SRCCOPY);
		mdc.SelectObject(oldGraphic);
		graphic = NULL;
	}
	rect.SetRect(theClientRect.left+555,theClientRect.bottom-117,theClientRect.left+780,theClientRect.bottom-92);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pDoc->GetEmpire(race)->GetResearch()->GetWeaponFP()*100)/(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetWeapon(level))));
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	rect.SetRect(theClientRect.left+555,theClientRect.bottom-92,theClientRect.left+780,theClientRect.bottom-67);
	techPercentage[5] = pDoc->GetEmpire(race)->GetResearch()->GetWeaponPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[5]);
	pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	
	pDC->SetStretchBltMode(oldStretchMode);

	// Die gesamten Forschungspunkte rechts anzeigen
	rect.SetRect(theClientRect.right-240,theClientRect.top+80,theClientRect.right,theClientRect.top+105);
	pDC->DrawText(CResourceManager::GetString("RESEARCHPOINTS"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SetTextColor(RGB(200,200,200));
	if (pDoc->GetPlayersRace() == HUMAN)
		rect.SetRect(theClientRect.right-195,theClientRect.top+115,theClientRect.right-30,theClientRect.top+140);
	else
		rect.SetRect(theClientRect.right-220,theClientRect.top+115,theClientRect.right-30,theClientRect.top+140);
	pDC->DrawText(CResourceManager::GetString("TOTAL").MakeUpper()+":", rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d %s",pDoc->GetEmpire(race)->GetFP(),CResourceManager::GetString("FP"));
	pDC->DrawText(s,rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	// Die einzelnen Forschungsboni anzeigen
	pDC->SetTextColor(oldColor);
	rect.SetRect(theClientRect.right-240,theClientRect.top+145,theClientRect.right,theClientRect.top+170);
	pDC->DrawText(CResourceManager::GetString("RESEARCH_BONI"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SetTextColor(RGB(200,200,200));
	if (pDoc->GetPlayersRace() == HUMAN)
		rect.SetRect(theClientRect.right-195,theClientRect.top+180,theClientRect.right-30,theClientRect.top+205);
	else
		rect.SetRect(theClientRect.right-220,theClientRect.top+180,theClientRect.right-30,theClientRect.top+205);
	pDC->DrawText(CResourceManager::GetString("BIOTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetBioTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("ENERGYTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetEnergyTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("COMPUTERTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetCompTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetConstructionTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetPropulsionTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("WEAPONTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetWeaponTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	pDC->SetTextColor(oldColor);

	CPen black(PS_SOLID, 0, RGB(90,90,90));
	for (int i = 0; i < 6; i++)
		for (int j = 1; j < 101; j++)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf drücken können
		{
			CPen green(PS_SOLID, 0, RGB(250-j*2.5,50+j*2,0));
			if (j <= techPercentage[i])
				pDC->SelectObject(&green);
			else
				pDC->SelectObject(&black);
			pDC->Rectangle(ResearchTimber[i][j]);
		}

		pDC->SetTextColor(RGB(255,255,255));
		BOOL lockstatus;
		for (int i = 0; i < 6; i++)
		{
			lockstatus = pDoc->GetEmpire(race)->GetResearch()->GetLockStatus(i);
			if (lockstatus == FALSE)
				s = CResourceManager::GetString("UNLOCKED");
			else
				s = CResourceManager::GetString("LOCKED");
			pDC->DrawText(s,LockStatusRect[i],DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		}
		pDC->SetTextColor(oldColor);
		this->LoadFontForBigButton(pDC);

		// Die einzelnen Menübuttons für Spezialforschng, Datenbank und Schiffskonstruktion darstellen
		DrawButtons(pDC, &m_ResearchMainButtons, m_bySubMenu);
		
		// "Forschungsübersicht" in der Mitte zeichnen
		CFont font;
		pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
		pDC->SelectObject(&font);
		if (pDoc->GetPlayersRace() == FERENGI)
			rect.SetRect(theClientRect.left,theClientRect.top,theClientRect.right-80,theClientRect.top+50);
		else if (pDoc->GetPlayersRace() == KLINGON)
			rect.SetRect(theClientRect.left,theClientRect.top,theClientRect.right-50,theClientRect.top+60);
		else
			rect.SetRect(theClientRect.left,theClientRect.top,theClientRect.right,theClientRect.top+60);
		pDC->DrawText(CResourceManager::GetString("RESEARCH_MENUE"),rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Uniqueforschungsmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CResearchMenuView::DrawUniqueResearchMenue(CDC* pDC, CRect theClientRect)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CRect rect;
	CString s;
	USHORT race = pDoc->GetPlayersRace();
	USHORT techPercentage;

	CDC mdc;
	mdc.CreateCompatibleDC(pDC);
	// Wenn wir eine Unique Forschung zur Auswahl haben
	if (pDoc->GetEmpire(race)->GetResearch()->GetUniqueReady() == FALSE)
		mdc.SelectObject(bg_urmenu);
	else
		mdc.SelectObject(bg_emptyur);
	pDC->BitBlt(0,0,1075,750,&mdc,0,0,SRCCOPY);
	
	COLORREF oldColor = pDC->GetTextColor();
/*	CPen mark(PS_SOLID, 1, RGB(125,175,255));
	pDC->SelectObject(&mark);

	// grobe Linien zeichnen
	pDC->MoveTo(theClientRect.left,theClientRect.top+70);
	pDC->LineTo(theClientRect.right,theClientRect.top+70);
	pDC->MoveTo(theClientRect.right-250,theClientRect.top+70);
	pDC->LineTo(theClientRect.right-250,theClientRect.bottom);
*/
	// Die gesamten Forschungspunkte rechts anzeigen
	rect.SetRect(theClientRect.right-240,theClientRect.top+80,theClientRect.right,theClientRect.top+105);
	pDC->DrawText(CResourceManager::GetString("RESEARCHPOINTS"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SetTextColor(RGB(200,200,200));
	if (pDoc->GetPlayersRace() == HUMAN)
		rect.SetRect(theClientRect.right-195,theClientRect.top+115,theClientRect.right-30,theClientRect.top+140);
	else
		rect.SetRect(theClientRect.right-220,theClientRect.top+115,theClientRect.right-30,theClientRect.top+140);
	pDC->DrawText(CResourceManager::GetString("TOTAL").MakeUpper()+":", rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	s.Format("%d %s",pDoc->GetEmpire(race)->GetFP(),CResourceManager::GetString("FP"));
	pDC->DrawText(s,rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	// Die einzelnen Forschungsboni anzeigen
	pDC->SetTextColor(oldColor);
	rect.SetRect(theClientRect.right-240,theClientRect.top+145,theClientRect.right,theClientRect.top+170);
	pDC->DrawText(CResourceManager::GetString("RESEARCH_BONI"),rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SetTextColor(RGB(200,200,200));
	if (pDoc->GetPlayersRace() == HUMAN)
		rect.SetRect(theClientRect.right-195,theClientRect.top+180,theClientRect.right-30,theClientRect.top+205);
	else
		rect.SetRect(theClientRect.right-220,theClientRect.top+180,theClientRect.right-30,theClientRect.top+205);
	pDC->DrawText(CResourceManager::GetString("BIOTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetBioTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("ENERGYTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetEnergyTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("COMPUTERTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetCompTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetConstructionTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetPropulsionTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	rect += CSize(0, 30);
	pDC->DrawText(CResourceManager::GetString("WEAPONTECH_SHORT").MakeUpper()+":", rect, DT_LEFT | DT_TOP);
	s.Format("%d%%",pDoc->GetEmpire(race)->GetResearch()->GetWeaponTechBoni());
	pDC->DrawText(s, rect, DT_RIGHT | DT_TOP);
	pDC->SetTextColor(oldColor);

	// großen Rechtecke für die einzelnen Forschungsgebiete zeichnen
	// Uniqueforschung
	// Wenn wir eine Unique Forschung zur Auswahl haben, dann auch anzeigen
	if (pDoc->GetEmpire(race)->GetResearch()->GetUniqueReady() == FALSE)
	{
		CRect TechRect;
		USHORT level = pDoc->GetEmpire(race)->GetResearch()->GetNumberOfUniqueResearch();
		// UniqueForschung braucht ja soviele FO wie alle anderen des Level zusammen, deswegen die lange Anweisung
		ULONG allOthers = pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetBio(level)+
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetEnergy(level)+
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetComp(level)+
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetPropulsion(level)+
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetConstruction(level)+
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetWeapon(level);
		allOthers /= 1.5;
		TechRect.SetRect(theClientRect.left+10,theClientRect.top+80,theClientRect.left+260,theClientRect.top+380);
//		pDC->Rectangle(TechRect);
		pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName(),TechRect,DT_CENTER | DT_TOP | DT_SINGLELINE);
		HGDIOBJ oldGraphic;
		CBitmap* graphic = pDoc->GetGraphicPool()->GetGraphic("Research\\specialtech.jpg");
		if (graphic)
		{
			int oldStretchMode = pDC->GetStretchBltMode();
			pDC->SetStretchBltMode(HALFTONE);
			oldGraphic = mdc.SelectObject(*graphic);
			pDC->StretchBlt(45,100,180,150,&mdc,0,0,270,225,SRCCOPY);
			mdc.SelectObject(oldGraphic);
			graphic = NULL;
			pDC->SetStretchBltMode(oldStretchMode);
		}
		rect.SetRect(theClientRect.left+10,theClientRect.top+290,theClientRect.left+260,theClientRect.top+315);
		s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pDoc->GetEmpire(race)->GetResearch()->GetUniqueFP()*100)/(allOthers)));
		pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		rect.SetRect(theClientRect.left+10,theClientRect.top+315,theClientRect.left+260,theClientRect.top+340);
		techPercentage = pDoc->GetEmpire(race)->GetResearch()->GetUniquePercentage();
		s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage);
		pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		// Den Zuweisungsbalken und den Lockstatus zeichnen
		CPen black(PS_SOLID, 0, RGB(90,90,90));
		for (int j = 1; j < 101; j++)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf drücken können
		{
			CPen green(PS_SOLID, 0, RGB(250-j*2.5,50+j*2,0));
			if (j <= techPercentage)
				pDC->SelectObject(&green);
			else
				pDC->SelectObject(&black);
			pDC->Rectangle(ResearchTimber[6][j]);	// 6 steht für Unique
		}

		pDC->SetTextColor(RGB(255,255,255));
		BOOLEAN lockstatus;
		lockstatus = pDoc->GetEmpire(race)->GetResearch()->GetLockStatus(6);
		if (lockstatus == FALSE)
			s = CResourceManager::GetString("UNLOCKED");
		else
			s = CResourceManager::GetString("LOCKED");
		pDC->DrawText(s,LockStatusRect[6],DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		pDC->SetTextColor(oldColor);
		// Den Namen des Unique Komplexes hinschreiben
		CRect r;
		r.SetRect(theClientRect.right-750,theClientRect.top+75,theClientRect.right-250,theClientRect.top+100);
		pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName(),r, DT_CENTER | DT_TOP | DT_SINGLELINE);
		// Die Beschreibung des Unique Complexes hinschreiben
		r.SetRect(theClientRect.right-690,theClientRect.top+120,theClientRect.right-310,theClientRect.top+380);
//		pDC->SelectObject(&mark);
//		pDC->Rectangle(r);
		pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexDescription(),r, DT_CENTER | DT_TOP | DT_WORDBREAK);
		// hier die 3 Wahlmöglichkeiten
		pDC->SetTextColor(RGB(200,200,200));
		// 1. Möglichkeit
		if (pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCHING)
		{
			r.SetRect(theClientRect.left+10,theClientRect.bottom-310,theClientRect.left+260,theClientRect.bottom-10);
//			pDC->Rectangle(r);
			pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(1),r,DT_CENTER | DT_TOP | DT_WORDBREAK);
		}
		// 2. Möglichkeit
		if (pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCHING)
		{
			r.SetRect(theClientRect.left+270,theClientRect.bottom-310,theClientRect.left+520,theClientRect.bottom-10);
//			pDC->Rectangle(r);
			pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(2),r,DT_CENTER | DT_TOP | DT_WORDBREAK);
		}
		// 3. Möglichkeit
		if (pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCHING)
		{
			r.SetRect(theClientRect.left+530,theClientRect.bottom-310,theClientRect.left+780,theClientRect.bottom-10);
//			pDC->Rectangle(r);
			pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(3),r,DT_CENTER | DT_TOP | DT_WORDBREAK);
		}

		// ab hier die Beschreibungen zu den einzelnen Wahlmöglichkeiten
		pDC->SetTextColor(oldColor);
		if (pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCHING)
		{
			r.SetRect(theClientRect.left+10,theClientRect.bottom-260,theClientRect.left+260,theClientRect.bottom-55);
			pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(1),r,DT_CENTER | DT_TOP | DT_WORDBREAK);
		}
		if (pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCHING)
		{
			r.SetRect(theClientRect.left+270,theClientRect.bottom-260,theClientRect.left+520,theClientRect.bottom-55);
			pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(2),r,DT_CENTER | DT_TOP | DT_WORDBREAK);
		}
		if (pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCHING)
		{
			r.SetRect(theClientRect.left+530,theClientRect.bottom-260,theClientRect.left+780,theClientRect.bottom-55);
			pDC->DrawText(pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(3),r,DT_CENTER | DT_TOP | DT_WORDBREAK);
		}
	}

	// Die Buttons in der Forschungsansicht zeichnen + die Buttons zum Auswählen einer Uniqueforschung
	LoadFontForBigButton(pDC);
	DrawButtons(pDC, &m_ResearchMainButtons, m_bySubMenu);

	// Nun die Buttons zur Auswahl einer Uniqueforschung
	if (pDoc->GetEmpire(race)->GetResearch()->GetUniqueReady() == FALSE && pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
	{
		mdc.SelectObject(bm);
		s = CResourceManager::GetString("BTN_SELECT");
		pDC->BitBlt(theClientRect.left+55,theClientRect.bottom-52,theClientRect.left+195,theClientRect.bottom-12,&mdc,0,0,SRCCOPY);
		rect.SetRect(theClientRect.left+10,theClientRect.bottom-52,theClientRect.left+260,theClientRect.bottom-12);
		pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->BitBlt(theClientRect.left+315,theClientRect.bottom-52,theClientRect.left+565,theClientRect.bottom-12,&mdc,0,0,SRCCOPY);
		rect.SetRect(theClientRect.left+270,theClientRect.bottom-52,theClientRect.left+520,theClientRect.bottom-12);
		pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->BitBlt(theClientRect.left+575,theClientRect.bottom-52,theClientRect.left+825,theClientRect.bottom-12,&mdc,0,0,SRCCOPY);
		rect.SetRect(theClientRect.left+530,theClientRect.bottom-52,theClientRect.left+780,theClientRect.bottom-12);
		pDC->DrawText(s,rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	// "Spezialforschung" in der Mitte zeichnen + wenn mgl. den Namen des Forschungskomplexes
	CFont font;
	pDC->SetTextColor(CFontLoader::CreateFont(pDoc->GetPlayersRace(), 5, 3, &font));
	pDC->SelectObject(&font);
	if (pDoc->GetPlayersRace() == FERENGI)
		rect.SetRect(theClientRect.left,theClientRect.top,theClientRect.right-170,theClientRect.top+45);
	else if (pDoc->GetPlayersRace() == KLINGON)
		rect.SetRect(theClientRect.left,theClientRect.top,theClientRect.right-150,theClientRect.top+55);
	else
		rect.SetRect(theClientRect.left,theClientRect.top,theClientRect.right,theClientRect.top+55);
	// Wenn wir eine Uniqueforschung zur Auswahl haben
	if (pDoc->GetEmpire(race)->GetResearch()->GetUniqueReady() == FALSE)
		s.Format("%s - %s",CResourceManager::GetString("SPECIAL_RESEARCH"),	pDoc->GetEmpire(race)->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName());
	// Wenn wir keine Uniqueforschung zur Auswahl haben
	else
		s = CResourceManager::GetString("SPECIAL_RESEARCH");
	pDC->DrawText(s,rect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	// Wenn wir keine Unique Forschung zur Auswahl haben
	if (pDoc->GetEmpire(race)->GetResearch()->GetUniqueReady() != FALSE)
	{
		rect.SetRect(190,300,690,theClientRect.bottom-90);
		pDC->DrawText(CResourceManager::GetString("NO_SPECIAL_RESEARCH_AVAILABLE"), rect, DT_CENTER | DT_TOP | DT_WORDBREAK);
	}
}

void CResearchMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	// Hier überprüfen auf welchen Menübutton wir in den Forschungsansichten (normal und unique) geklickt haben
	// wechsel zwischen normaler <--> unique Forschung
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_ResearchMainButtons, temp, FALSE))
	{
		m_bySubMenu = temp;
		// Haben wir auf den Schiffsdesign Button geklickt
		if (m_bySubMenu == 2)
		{
			pDoc->GetMainFrame()->SelectMainView(9, pDoc->GetPlayersRace());	// Schiffsdesignansicht zeichnen
			Invalidate(FALSE);
			m_bySubMenu = 0;
			return;
		}
		else
		{
			Invalidate(FALSE);
			return;
		}
	}
	// Wenn wir uns im normalen Forschungsmenue befinden
	if (m_bySubMenu == 0)
	{
		// haben wir in die Balken geklickt, um den Forschungsanteil zu ändern
		for (int i = 0; i < 6; i++)
			for (int j = 0; j < 101; j++)
			{
				// Rechteck leicht in der Größe modifizieren, damit man es besser anklicken kann
				ResearchTimber[i][j].SetRect(ResearchTimber[i][j].left,ResearchTimber[i][j].top,ResearchTimber[i][j].right+1,ResearchTimber[i][j].bottom);
				if (ResearchTimber[i][j].PtInRect(point))
				{
					pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch()->SetPercentage(i,j);
					Invalidate();
				}
				// modifizerte Rechteckgröße wieder rückgängig machen
				ResearchTimber[i][j].SetRect(ResearchTimber[i][j].left,ResearchTimber[i][j].top,ResearchTimber[i][j].right-1,ResearchTimber[i][j].bottom);
			}

			// Haben wir auf ein Bild gedrückt, um es zu locken oder unlocken
			for (int i = 0; i < 6; i++)
				if (LockStatusRect[i].PtInRect(point))
				{
					pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch()->SetLock(i,!pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetLockStatus(i));
					CRect r = LockStatusRect[i];
					CalcDeviceRect(r);
					InvalidateRect(r, FALSE);
					break;
				}
	}
	// Wenn wir uns im Uniqueforschungsmenue befinden
	else if (m_bySubMenu == 1)
	{
		// Ist die Uniqueforschung möglich?
		if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetUniqueReady() == FALSE)
		{
			// 6 steht hier überall für Unique Forschung!!
			for (int j = 0; j < 101; j++)
			{
				// Rechteck leicht in der Größe modifizieren, damit man es besser anklicken kann
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right+1,ResearchTimber[6][j].bottom);
				if (ResearchTimber[6][j].PtInRect(point))
				{
					pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch()->SetPercentage(6,j);
					Invalidate();
				}
				// modifizerte Rechteckgröße wieder rückgängig machen
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right-1,ResearchTimber[6][j].bottom);
			}
			// Prüfen ob wir auf ein Rechteck gedrückt haben, um eine der 3 Möglichkeiten auszuwählen
			if (pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
			{
				CRect r[3];
				for (int i = 0; i < 3; i++)
				{
					r[i].SetRect(55+i*260,680,215+i*260,720);
					if (r[i].PtInRect(point))
					{
						pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(i+1);
						Invalidate();
					}
				}
			}
			// Wollen wir den Lockstatus ändern?
			if (LockStatusRect[6].PtInRect(point))
			{
				pDoc->m_Empire[pDoc->GetPlayersRace()].GetResearch()->SetLock(6,!pDoc->GetEmpire(pDoc->GetPlayersRace())->GetResearch()->GetLockStatus(6));
				CRect r = LockStatusRect[6];
				CalcDeviceRect(r);
				InvalidateRect(r, FALSE);
			}
		}
	}

	CMainBaseView::OnLButtonDown(nFlags, point);
}

void CResearchMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CalcLogicalPoint(point);

	ButtonReactOnMouseOver(point, &m_ResearchMainButtons);
	if (m_bySubMenu == 0)
	{
		if (pDoc->m_iShowWhichTechInView3 != 0 && CRect(10,80,260,380).PtInRect(point))
		{
			pDoc->m_iShowWhichTechInView3 = 0;
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CResearchBottomView));
		}
		else if (pDoc->m_iShowWhichTechInView3 != 1 && CRect(270,80,520,380).PtInRect(point))
		{
			pDoc->m_iShowWhichTechInView3 = 1;
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CResearchBottomView));
		}
		else if (pDoc->m_iShowWhichTechInView3 != 2 && CRect(530,80,780,380).PtInRect(point))
		{
			pDoc->m_iShowWhichTechInView3 = 2;
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CResearchBottomView));
		}
		else if (pDoc->m_iShowWhichTechInView3 != 3 && CRect(270,425,520,725).PtInRect(point))
		{
			pDoc->m_iShowWhichTechInView3 = 3;
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CResearchBottomView));
		}
		else if (pDoc->m_iShowWhichTechInView3 != 4 && CRect(10,425,260,725).PtInRect(point))
		{
			pDoc->m_iShowWhichTechInView3 = 4;
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CResearchBottomView));
		}
		else if (pDoc->m_iShowWhichTechInView3 != 5 && CRect(530,425,780,725).PtInRect(point))
		{
			pDoc->m_iShowWhichTechInView3 = 5;
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CResearchBottomView));
		}
	}

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CResearchMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	// Unique Forschung einstellen
	if (nChar == VK_NUMPAD1)
	{
		pDoc->m_Empire[HUMAN].GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(1);
	}
	else if (nChar == VK_NUMPAD2)
	{
		pDoc->m_Empire[HUMAN].GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(2);
	}
	else if (nChar == VK_NUMPAD3)
	{
		pDoc->m_Empire[HUMAN].GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(3);
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CResearchMenuView::CreateButtons()
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
			// Buttons in den Forschungsansichten
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,450) , CSize(160,40), CResourceManager::GetString("BTN_NORMAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,500) , CSize(160,40), CResourceManager::GetString("BTN_SPECIAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,550) , CSize(160,40), CResourceManager::GetString("BTN_SHIPDESIGN"), fileN, fileI, fileA));
			break;			
		}
	case FERENGI:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttona.jpg";
			// Buttons in den Forschungsansichten
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,450) , CSize(160,40), CResourceManager::GetString("BTN_NORMAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,500) , CSize(160,40), CResourceManager::GetString("BTN_SPECIAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,550) , CSize(160,40), CResourceManager::GetString("BTN_SHIPDESIGN"), fileN, fileI, fileA));
			break;
		}
	case KLINGON:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2i.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2a.jpg";
			// Buttons in den Forschungsansichten
			m_ResearchMainButtons.Add(new CMyButton(CPoint(875,450) , CSize(160,40), CResourceManager::GetString("BTN_NORMAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(875,500) , CSize(160,40), CResourceManager::GetString("BTN_SPECIAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(875,550) , CSize(160,40), CResourceManager::GetString("BTN_SHIPDESIGN"), fileN, fileI, fileA));
			break;
		}
	case ROMULAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttona.jpg";
			// Buttons in den Forschungsansichten
			m_ResearchMainButtons.Add(new CMyButton(CPoint(872,450) , CSize(160,40), CResourceManager::GetString("BTN_NORMAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(872,500) , CSize(160,40), CResourceManager::GetString("BTN_SPECIAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(872,550) , CSize(160,40), CResourceManager::GetString("BTN_SHIPDESIGN"), fileN, fileI, fileA));
			break;
		}
	case CARDASSIAN:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttona.jpg";
			// Buttons in den Forschungsansichten
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,450) , CSize(160,40), CResourceManager::GetString("BTN_NORMAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,500) , CSize(160,40), CResourceManager::GetString("BTN_SPECIAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,550) , CSize(160,40), CResourceManager::GetString("BTN_SHIPDESIGN"), fileN, fileI, fileA));
			break;
		}
	case DOMINION:
		{
			// Buttons in der Systemansicht
			CString fileN = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttona.jpg";
			// Buttons in den Forschungsansichten
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,450) , CSize(160,40), CResourceManager::GetString("BTN_NORMAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,500) , CSize(160,40), CResourceManager::GetString("BTN_SPECIAL"), fileN, fileI, fileA));
			m_ResearchMainButtons.Add(new CMyButton(CPoint(867,550) , CSize(160,40), CResourceManager::GetString("BTN_SHIPDESIGN"), fileN, fileI, fileA));
			break;
		}
	}
}