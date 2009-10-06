// ResearchMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "ResearchMenuView.h"
#include "ResearchBottomView.h"
#include "Races\RaceController.h"
#include "IniLoader.h"


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
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
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
	
	// ***************************** DIE FORSCHUNGSANSICHT ZEICHNEN **********************************
	if (m_bySubMenu == 0)
		DrawResearchMenue(&g);
	else if (m_bySubMenu == 1)
		DrawUniqueResearchMenue(&g);
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
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// Alle Buttons in der View erstellen
	CreateButtons();
	// alle Hintergrundgrafiken laden
	CString sPrefix = pMajor->GetPrefix();	
	bg_researchmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "researchmenu.jpg");
	bg_urmenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "urmenu.jpg");
	bg_emptyur		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "emptyur.jpg");

	// Zuweisungsbalken in der Forschungsübersicht
	// Die Koodinaten der Rechtecke für die ganzen Buttons
	CRect r;
	GetClientRect(&r);
	// i == 0->Bio, 1->Energy, 2->Computer, 3->Bautechnik, 4->Antrieb, 5->Waffen, 6->Unique
	for (int i = 0; i < 3; i++)	// Unique Forschung muß seperat initialisiert werden, daher nur bis 5
		for (int j = 0; j < 101; j++)
			// Wir haben ne Breite von 200 für die 100 Striche zur Auswahl -> jeder Strich 2 breit
			ResearchTimber[i][j].SetRect(r.left+33+j*2+i*260,r.top+350,r.left+33+j*2+2+i*260,r.top+350+20);
	// Jetzt die Striche für die unteren Forschungsgebiete
	for (int i = 3; i < 6; i++)	// Unique Forschung muß seperat initialisiert werden, daher nur bis 5
		for (int j = 0; j < 101; j++)
			// Wir haben ne Breite von 200 für die 100 Striche zur Auswahl -> jeder Strich 2 breit
			ResearchTimber[i][j].SetRect(r.left+33+j*2+(i-3)*260,r.top+693,r.left+33+j*2+2+(i-3)*260,r.top+693+20);
	// Nun alles für die Unique Forschung
	for (int i = 0; i < 101; i++)
		// Wir haben ne Breite von 200 für die 100 Striche zur Auswahl -> ieder Strich 2 breit
		ResearchTimber[6][i].SetRect(r.left+33+i*2,r.top+350,r.left+33+i*2+2,r.top+350+20);
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
void CResearchMenuView::DrawResearchMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
			
	StringFormat fontFormatTop;
	fontFormatTop.SetAlignment(StringAlignmentCenter);
	fontFormatTop.SetLineAlignment(StringAlignmentNear);
	fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);

	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormatCenter.SetTrimming(StringTrimmingEllipsisCharacter);

	if (bg_researchmenu)
		g->DrawImage(bg_researchmenu, 0, 0, 1075, 750);

	CString s;
	USHORT level;
	USHORT techPercentage[7];
		
	// großen Rechtecke für die einzelnen Forschungsgebiete zeichnen
	// Biogenetik
	level = pMajor->GetEmpire()->GetResearch()->GetBioTech();
	s.Format("%s - %s %d",CResourceManager::GetString("BIOTECH"),CResourceManager::GetString("LEVEL"),level);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(0);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,110,250,25), &fontFormatCenter, &fontBrush);
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\biotech.png");
	if (graphic)
		g->DrawImage(graphic, 60,140,150,125);		
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetBioFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetBio(level))));
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[0] = pMajor->GetEmpire()->GetResearch()->GetBioPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[0]);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,315,250,25), &fontFormatCenter, &fontBrush);
	
	// Bautechnik
	level = pMajor->GetEmpire()->GetResearch()->GetConstructionTech();
	s.Format("%s - %s %d",CResourceManager::GetString("CONSTRUCTIONTECH"),CResourceManager::GetString("LEVEL"),level);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(4);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\constructiontech.png");
	if (graphic)
		g->DrawImage(graphic, 60,480,150,125);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetConstructionFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetConstruction(level))));
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[3] = pMajor->GetEmpire()->GetResearch()->GetConstructionPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[3]);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,655,250,25), &fontFormatCenter, &fontBrush);
	
	// Energietechnik
	level = pMajor->GetEmpire()->GetResearch()->GetEnergyTech();
	s.Format("%s - %s %d",CResourceManager::GetString("ENERGYTECH"),CResourceManager::GetString("LEVEL"),level);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(1);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,110,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\energytech.png");
	if (graphic)
		g->DrawImage(graphic, 320,140,150,125);		
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetEnergyFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetEnergy(level))));
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[1] = pMajor->GetEmpire()->GetResearch()->GetEnergyPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[1]);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,315,250,25), &fontFormatCenter, &fontBrush);

	// Antriebstechnik
	level = pMajor->GetEmpire()->GetResearch()->GetPropulsionTech();
	s.Format("%s - %s %d",CResourceManager::GetString("PROPULSIONTECH"),CResourceManager::GetString("LEVEL"),level);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(3);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\propulsiontech.png");
	if (graphic)
		g->DrawImage(graphic, 320,480,150,125);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetPropulsionFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetPropulsion(level))));
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[4] = pMajor->GetEmpire()->GetResearch()->GetPropulsionPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[4]);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,655,250,25), &fontFormatCenter, &fontBrush);

	// Computertechnik
	level = pMajor->GetEmpire()->GetResearch()->GetCompTech();
	s.Format("%s - %s %d",CResourceManager::GetString("COMPUTERTECH"),CResourceManager::GetString("LEVEL"),level);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,80,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(2);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,110,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\computertech.png");
	if (graphic)
		g->DrawImage(graphic, 580,140,150,125);		
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetComputerFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetComp(level))));
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,290,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[2] = pMajor->GetEmpire()->GetResearch()->GetCompPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[2]);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,315,250,25), &fontFormatCenter, &fontBrush);

	// Waffentechnik
	level = pMajor->GetEmpire()->GetResearch()->GetWeaponTech();
	s.Format("%s - %s %d",CResourceManager::GetString("WEAPONTECH"),CResourceManager::GetString("LEVEL"),level);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,425,250,25), &fontFormatTop, &fontBrush);
	s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetTechName(5);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,455,250,25), &fontFormatCenter, &fontBrush);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\weapontech.png");
	if (graphic)
		g->DrawImage(graphic, 580,480,150,125);
	s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetWeaponFP()*100)/(pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetWeapon(level))));
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,630,250,25), &fontFormatCenter, &fontBrush);
	techPercentage[5] = pMajor->GetEmpire()->GetResearch()->GetWeaponPercentage();
	s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage[5]);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,655,250,25), &fontFormatCenter, &fontBrush);

	
	// Die gesamten Forschungspunkte rechts anzeigen
	g->DrawString(CResourceManager::GetString("RESEARCHPOINTS").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(835,80,240,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);
		
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CResourceManager::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	
	s.Format("%d %s",pMajor->GetEmpire()->GetFP(),CResourceManager::GetString("FP"));
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	// Die einzelnen Forschungsboni anzeigen
	fontBrush.SetColor(normalColor);
	g->DrawString(CResourceManager::GetString("RESEARCH_BONI").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(835,145,245,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);
	
	RectF rect(855,180,190,25);
	s = CResourceManager::GetString("BIOTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetBioTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("ENERGYTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("COMPUTERTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetCompTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetConstructionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetPropulsionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("WEAPONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetWeaponTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	
	fontBrush.SetColor(normalColor);
	
	Color penMark;
	penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 1; j < 101; j += 5)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf drücken können
		{
			RectF timber(ResearchTimber[i][j].left, ResearchTimber[i][j].top, ResearchTimber[i][j].Width() + 6, ResearchTimber[i][j].Height());
			if (j <= techPercentage[i])
				g->FillRectangle(&SolidBrush(Color(250-j*2.5,50+j*2,0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(75,75,75,75)), timber);
		}
		// farbige Kante um Zuweisungsbalken zeichnen
		RectF markRect(ResearchTimber[i][1].left, ResearchTimber[i][1].top, ResearchTimber[i][100].left - ResearchTimber[i][1].left, ResearchTimber[i][1].Height());
		g->DrawRectangle(&Gdiplus::Pen(penMark), markRect);
	}
	
	BOOL lockstatus;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentFar);
	for (int i = 0; i < 6; i++)
	{
		lockstatus = pMajor->GetEmpire()->GetResearch()->GetLockStatus(i);
		if (lockstatus == FALSE)
			s = CResourceManager::GetString("UNLOCKED");
		else
			s = CResourceManager::GetString("LOCKED");
		RectF lock(LockStatusRect[i].left, LockStatusRect[i].top, LockStatusRect[i].Width(), LockStatusRect[i].Height());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), lock, &fontFormatCenter, &SolidBrush(Color::White));
	}
	
	// Die einzelnen Menübuttons für Spezialforschng, Datenbank und Schiffskonstruktion darstellen
	// Schriftfarbe wählen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	fontBrush.SetColor(btnColor);		
	DrawGDIButtons(g, &m_ResearchMainButtons, m_bySubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);
		
	// "Forschungsübersicht" in der Mitte zeichnen
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);

	g->DrawString(CResourceManager::GetString("RESEARCH_MENUE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormatCenter, &fontBrush);	
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Uniqueforschungsmenüs
/////////////////////////////////////////////////////////////////////////////////////////
void CResearchMenuView::DrawUniqueResearchMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString s;
	USHORT techPercentage;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe wählen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
			
	StringFormat fontFormatTop;
	fontFormatTop.SetAlignment(StringAlignmentCenter);
	fontFormatTop.SetLineAlignment(StringAlignmentNear);
	fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);

	StringFormat fontFormatCenter;
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormatCenter.SetTrimming(StringTrimmingEllipsisCharacter);

	// Wenn wir eine Unique Forschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	{
		if (bg_urmenu)
			g->DrawImage(bg_urmenu, 0, 0, 1075, 750);
	}
	else
	{
		if (bg_emptyur)
			g->DrawImage(bg_emptyur, 0, 0, 1075, 750);
	}

	// Die gesamten Forschungspunkte rechts anzeigen
	g->DrawString(CResourceManager::GetString("RESEARCHPOINTS").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(835,80,240,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);
		
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	s = CResourceManager::GetString("TOTAL").MakeUpper()+":";
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	
	s.Format("%d %s",pMajor->GetEmpire()->GetFP(),CResourceManager::GetString("FP"));
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(855,115,190,25), &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);

	// Die einzelnen Forschungsboni anzeigen
	fontBrush.SetColor(normalColor);
	g->DrawString(CResourceManager::GetString("RESEARCH_BONI").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(835,145,245,25), &fontFormatCenter, &fontBrush);
	fontBrush.SetColor(markColor);
	
	RectF rect(855,180,190,25);
	s = CResourceManager::GetString("BIOTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetBioTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("ENERGYTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetEnergyTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("COMPUTERTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetCompTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("CONSTRUCTIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetConstructionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("PROPULSIONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetPropulsionTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	rect.Y += 30;

	s = CResourceManager::GetString("WEAPONTECH_SHORT").MakeUpper()+":";
	fontFormatCenter.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	s.Format("%d%%",pMajor->GetEmpire()->GetResearch()->GetWeaponTechBoni());
	fontFormatCenter.SetAlignment(StringAlignmentFar);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormatCenter, &fontBrush);
	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	
	fontBrush.SetColor(normalColor);

	// großen Rechtecke für die einzelnen Forschungsgebiete zeichnen
	// Uniqueforschung
	// Wenn wir eine Unique Forschung zur Auswahl haben, dann auch anzeigen
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
	{
		RectF TechRect(10,80,250,25);
		USHORT level = pMajor->GetEmpire()->GetResearch()->GetNumberOfUniqueResearch();
		// UniqueForschung braucht ja soviele FP wie alle anderen des Level zusammen, deswegen die lange Anweisung
		ULONG allOthers = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetBio(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetEnergy(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetComp(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetPropulsion(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetConstruction(level)+
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetWeapon(level);
		// man benötigt etwas weniger Punkte als alle Techs zusammen
		allOthers /= SPECIAL_RESEARCH_DIV;
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), TechRect, &fontFormatTop, &fontBrush);
		
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\specialtech.png");
		if (graphic)
			g->DrawImage(graphic, 45, 100, 180, 150);
		
		s.Format("%s: %i%%",CResourceManager::GetString("PROGRESS"),(int)((pMajor->GetEmpire()->GetResearch()->GetUniqueFP()*100)/(allOthers)));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,290,250,25), &fontFormatCenter, &fontBrush);
		
		techPercentage = pMajor->GetEmpire()->GetResearch()->GetUniquePercentage();
		s.Format("%s: %i%%",CResourceManager::GetString("ASSIGNMENT"),techPercentage);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,315,250,25), &fontFormatCenter, &fontBrush);
		
		Color penMark;
		penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
		for (int j = 1; j < 101; j += 5)	// bis 100, weil das 0te Element nicht mit gezeichnet wird, man soll nur drauf drücken können
		{
			RectF timber(ResearchTimber[6][j].left, ResearchTimber[6][j].top, ResearchTimber[6][j].Width() + 6, ResearchTimber[6][j].Height());
			if (j <= techPercentage)
				g->FillRectangle(&SolidBrush(Color(250-j*2.5,50+j*2,0)), timber);
			else
				g->FillRectangle(&SolidBrush(Color(75,75,75,75)), timber);
		}
		// farbige Kante um Zuweisungsbalken zeichnen
		RectF markRect(ResearchTimber[6][1].left, ResearchTimber[6][1].top, ResearchTimber[6][100].left - ResearchTimber[6][1].left, ResearchTimber[6][1].Height());
		g->DrawRectangle(&Gdiplus::Pen(penMark), markRect);
				
		BOOL lockstatus;
		fontFormatCenter.SetAlignment(StringAlignmentCenter);
		fontFormatCenter.SetLineAlignment(StringAlignmentFar);
		
		lockstatus = pMajor->GetEmpire()->GetResearch()->GetLockStatus(6);
		if (lockstatus == FALSE)
			s = CResourceManager::GetString("UNLOCKED");
		else
			s = CResourceManager::GetString("LOCKED");
		RectF lock(LockStatusRect[6].left, LockStatusRect[6].top, LockStatusRect[6].Width(), LockStatusRect[6].Height());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), lock, &fontFormatCenter, &SolidBrush(Color::White));
		fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
		
		// Den Namen des Unique Komplexes hinschreiben
		fontBrush.SetColor(markColor);
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName();
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(325,75,500,25), &fontFormatTop, &fontBrush);
		fontBrush.SetColor(normalColor);
		
		// Die Beschreibung des Unique Complexes hinschreiben
		s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexDescription();
		fontFormatTop.SetFormatFlags(!StringFormatFlagsNoWrap);
		fontFormatTop.SetTrimming(StringTrimmingEllipsisCharacter);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(385,120,380,260), &fontFormatTop, &fontBrush);
		fontFormatTop.SetTrimming(StringTrimmingNone);
		
		// hier die 3 Wahlmöglichkeiten
		fontBrush.SetColor(markColor);
		// 1. Möglichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCHING)
		{			
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(1);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 2. Möglichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(2);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,445,250,300), &fontFormatTop, &fontBrush);
		}
		// 3. Möglichkeit
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldName(3);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,445,250,300), &fontFormatTop, &fontBrush);
		}

		// ab hier die Beschreibungen zu den einzelnen Wahlmöglichkeiten
		fontBrush.SetColor(normalColor);
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(1) == RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(1);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(10,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(2) == RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(2);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(270,490,250,205), &fontFormatTop, &fontBrush);
		}
		if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE || 
			pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldStatus(3) == RESEARCHING)
		{
			s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetFieldDescription(3);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(530,490,250,205), &fontFormatTop, &fontBrush);			
		}
		fontFormatTop.SetFormatFlags(StringFormatFlagsNoWrap);
	}
	
	// Die einzelnen Menübuttons für Spezialforschng, Datenbank und Schiffskonstruktion darstellen
	// Schriftfarbe wählen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	fontBrush.SetColor(btnColor);		
	DrawGDIButtons(g, &m_ResearchMainButtons, m_bySubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);
	// Nun die Buttons zur Auswahl einer Uniqueforschung
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE && pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
	{
		CString sFile = pMajor->GetPrefix() + "button.png";
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + sFile);
		if (graphic)
		{
			g->DrawImage(graphic, 55, 698, 160, 40);
			g->DrawImage(graphic, 315, 698, 160, 40);
			g->DrawImage(graphic, 575, 698, 160, 40);
		}
		s = CResourceManager::GetString("BTN_SELECT");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(55,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(315,698,160,40), &fontFormatCenter, &fontBrush);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(575,698,160,40), &fontFormatCenter, &fontBrush);
	}
		
	// "Spezialforschung" in der Mitte zeichnen + wenn mgl. den Namen des Forschungskomplexes
	// Rassenspezifische Schriftart auswählen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe wählen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormatCenter.SetAlignment(StringAlignmentCenter);
	fontFormatCenter.SetLineAlignment(StringAlignmentCenter);
	fontFormatCenter.SetFormatFlags(StringFormatFlagsNoWrap);

	// Wenn wir eine Uniqueforschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		s.Format("%s - %s",CResourceManager::GetString("SPECIAL_RESEARCH"),	pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetCurrentResearchComplex()->GetComplexName());
	// Wenn wir keine Uniqueforschung zur Auswahl haben
	else
		s = CResourceManager::GetString("SPECIAL_RESEARCH");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,m_TotalSize.cx,55), &fontFormatCenter, &fontBrush);
	
	// Wenn wir keine Unique Forschung zur Auswahl haben
	if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() != FALSE)
	{
		fontFormatCenter.SetLineAlignment(StringAlignmentNear);
		fontFormatCenter.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CResourceManager::GetString("NO_SPECIAL_RESEARCH_AVAILABLE");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(190,200,500,100), &fontFormatCenter, &fontBrush);

		// darunter die schon erforschten Spezialforschungen anzeigen
		CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);

		fontBrush.SetColor(markColor);
		s = CResourceManager::GetString("RESEARCHED_SPECIALTECHS");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(190,300,500,30), &fontFormatCenter, &fontBrush);
		fontBrush.SetColor(normalColor);

		int nCount = 0;
		for (int i = 0; i < NoUC; i++)
		{
			// Bereich ermitteln
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(i)->GetComplexStatus() == RESEARCHED)
			{
				// Unterbereich bestimmen
				for (int j = 1; j <= 3; j++)
				{
					if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(i)->GetFieldStatus(j) == RESEARCHED)
					{
						s = pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(i)->GetComplexName() + ": " +
							pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(i)->GetFieldName(j);
						g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(190,350 + nCount * 30 , 500, 30), &fontFormatCenter, &fontBrush);
						nCount++;
						break;
					}
				}
			}
		}
		if (nCount == 0)
		{
			s = CResourceManager::GetString("NONE");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(190,350,500,30), &fontFormatCenter, &fontBrush);
		}
	}	
}

void CResearchMenuView::OnLButtonDown(UINT nFlags, CPoint point)
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

	// Hier überprüfen auf welchen Menübutton wir in den Forschungsansichten (normal und unique) geklickt haben
	// wechsel zwischen normaler <--> unique Forschung
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_ResearchMainButtons, temp, FALSE))
	{
		m_bySubMenu = temp;
		// Haben wir auf den Schiffsdesign Button geklickt
		if (m_bySubMenu == 2)
		{
			pDoc->GetMainFrame()->SelectMainView(9, pMajor->GetRaceID());	// Schiffsdesignansicht zeichnen
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
					pMajor->GetEmpire()->GetResearch()->SetPercentage(i,j);
					Invalidate();
				}
				// modifizerte Rechteckgröße wieder rückgängig machen
				ResearchTimber[i][j].SetRect(ResearchTimber[i][j].left,ResearchTimber[i][j].top,ResearchTimber[i][j].right-1,ResearchTimber[i][j].bottom);
			}

			// Haben wir auf ein Bild gedrückt, um es zu locken oder unlocken
			for (int i = 0; i < 6; i++)
				if (LockStatusRect[i].PtInRect(point))
				{
					pMajor->GetEmpire()->GetResearch()->SetLock(i,!pMajor->GetEmpire()->GetResearch()->GetLockStatus(i));
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
		if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
		{
			// 6 steht hier überall für Unique Forschung!!
			for (int j = 0; j < 101; j++)
			{
				// Rechteck leicht in der Größe modifizieren, damit man es besser anklicken kann
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right+1,ResearchTimber[6][j].bottom);
				if (ResearchTimber[6][j].PtInRect(point))
				{
					pMajor->GetEmpire()->GetResearch()->SetPercentage(6,j);
					Invalidate();
				}
				// modifizerte Rechteckgröße wieder rückgängig machen
				ResearchTimber[6][j].SetRect(ResearchTimber[6][j].left,ResearchTimber[6][j].top,ResearchTimber[6][j].right-1,ResearchTimber[6][j].bottom);
			}
			// Prüfen ob wir auf ein Rechteck gedrückt haben, um eine der 3 Möglichkeiten auszuwählen
			if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetChoiceTaken() == FALSE)
			{
				CRect r[3];
				for (int i = 0; i < 3; i++)
				{
					r[i].SetRect(55+i*260,680,215+i*260,720);
					if (r[i].PtInRect(point))
					{
						pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(i+1);
						Invalidate();
					}
				}
			}
			// Wollen wir den Lockstatus ändern?
			if (LockStatusRect[6].PtInRect(point))
			{
				pMajor->GetEmpire()->GetResearch()->SetLock(6,!pMajor->GetEmpire()->GetResearch()->GetLockStatus(6));
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
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

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
	else if (m_bySubMenu == 1)
	{
		if (pDoc->m_iShowWhichTechInView3 != 6)
		{
			CMajor* pMajor = m_pPlayersRace;
			ASSERT(pMajor);
			if (pMajor->GetEmpire()->GetResearch()->GetUniqueReady() == FALSE)
			{
				pDoc->m_iShowWhichTechInView3 = 6;
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CResearchBottomView));
			}
		}
	}	

	CMainBaseView::OnMouseMove(nFlags, point);
}

void CResearchMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

	// Unique Forschung einstellen
	if (nChar == VK_NUMPAD1)
	{
		pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(1);
	}
	else if (nChar == VK_NUMPAD2)
	{
		pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(2);
	}
	else if (nChar == VK_NUMPAD3)
	{
		pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->SetUniqueResearchChoosePossibility(3);
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CResearchMenuView::CreateButtons()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// alle Buttons in der View anlegen und Grafiken laden
	CString sPrefix = pMajor->GetPrefix();
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.png";
	CString fileI = "Other\\" + sPrefix + "buttoni.png";
	CString fileA = "Other\\" + sPrefix + "buttona.png";
	// Buttons in den Forschungsansichten
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,450) , CSize(160,40), CResourceManager::GetString("BTN_NORMAL"), fileN, fileI, fileA));
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,500) , CSize(160,40), CResourceManager::GetString("BTN_SPECIAL"), fileN, fileI, fileA));
	m_ResearchMainButtons.Add(new CMyButton(CPoint(867,550) , CSize(160,40), CResourceManager::GetString("BTN_SHIPDESIGN"), fileN, fileI, fileA));	
}