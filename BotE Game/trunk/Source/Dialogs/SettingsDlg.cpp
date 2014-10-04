// SettingsDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BotE.h"
#include "SettingsDlg.h"
#include "IniLoader.h"
#include "AssertBotE.h"


// CSettingsDlg-Dialogfeld

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(bool bDisableNonWorking/* = false*/, CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
	, m_bDisable(bDisableNonWorking)
	, m_bAutoave(FALSE)
	, m_bHardwaresound(FALSE)
	, m_bSound(FALSE)
	, m_bMusic(FALSE)
	, m_bShowTraderoutes(FALSE)
	, m_bAnimatedIcon(FALSE)
	, m_bShowMiniMap(TRUE)
	, m_bShowEventPictures(TRUE)
	, m_bShowScrollBars(FALSE)
	, m_bInvertMouse(FALSE)
	, m_bHideMenu(FALSE)
	, m_bRandomEvents(TRUE)
	, m_bVCElimination(FALSE)
	, m_bVCDiplomacy(FALSE)
	, m_bVCConquest(FALSE)
	, m_bVCResearch(FALSE)
	, m_bVCCombat(FALSE)
	, m_bVCSabotage(FALSE)
{
}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_AUTOSAVE, m_bAutoave);
	DDX_Control(pDX, IDC_SLIDER_DIFFICULTY, m_ctrlDifficultySlider);
	DDX_Check(pDX, IDC_CHECK_HARDWARESOUND, m_bHardwaresound);
	DDX_Check(pDX, IDC_CHECK_SOUND, m_bSound);
	DDX_Check(pDX, IDC_CHECK_MUSIC, m_bMusic);
	DDX_Control(pDX, IDC_SLIDER_MUSICVOLUME, m_ctrlMusicvolume);
	DDX_Check(pDX, IDC_CHECK_SHOWTRADEROUTES, m_bShowTraderoutes);
	DDX_Check(pDX, IDC_CHECK_ANIMATEDICON, m_bAnimatedIcon);
	DDX_Check(pDX, IDC_CHECK_SHOWMINIMAP, m_bShowMiniMap);
	DDX_Control(pDX, IDC_SLIDER_TOOLTIPDELAY, m_ctrlTooltipDelay);
	DDX_Check(pDX, IDC_CHECK_SHOWSCROLLBARS, m_bShowScrollBars);
	DDX_Check(pDX, IDC_CHECK_INVERTNOUSE, m_bInvertMouse);
	DDX_Control(pDX, IDC_SLIDER_STARDENSITY, m_ctrlStarDensity);
	DDX_Control(pDX, IDC_SLIDER_MINORDENSITY, m_ctrlMinorDensity);
	DDX_Control(pDX, IDC_SLIDER_ANOMALYDENSITY, m_ctrlAnomalyDensity);
	DDX_Control(pDX, IDC_SLIDER_ALIEN_FREQUENCY, m_ctrlAlienFrequency);
	DDX_Control(pDX, IDC_EDIT_RANDOMSEED, m_edtRandomSeed);
	DDX_Control(pDX, IDC_COMBOGALAXYSHAPE, m_comboGalaxyshape);
	DDX_Control(pDX, IDC_COMBOGALAXYSIZE, m_comboGalaxysize);
	DDX_Check(pDX, IDC_CHECK_HIDEMENUBAR, m_bHideMenu);
	DDX_Check(pDX, IDC_CHECK_VC_ELIMINATION, m_bVCElimination);
	DDX_Check(pDX, IDC_CHECK_VC_DIPLOMACY, m_bVCDiplomacy);
	DDX_Check(pDX, IDC_CHECK_VC_CONQUEST, m_bVCConquest);
	DDX_Check(pDX, IDC_CHECK_VC_RESEARCH, m_bVCResearch);
	DDX_Check(pDX, IDC_CHECK_VC_COMBAT, m_bVCCombat);
	DDX_Check(pDX, IDC_CHECK_VC_SABOTAGE, m_bVCSabotage);
	DDX_Check(pDX, IDC_CHECK_SHOWEVENTPICTURES, m_bShowEventPictures);
	DDX_Check(pDX, IDC_CHECK_RANDOMEVENTS, m_bRandomEvents);
	DDX_Control(pDX, IDC_EDIT_NEARBYSYSTEMS, m_edtNearbysystems);
	DDX_Control(pDX, IDC_COMBOEXPANSIONSPEED, m_comboExpansionspeed);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DIFFICULTY, &CSettingsDlg::OnNMCustomdrawSliderDifficulty)
END_MESSAGE_MAP()


// CSettingsDlg-Meldungshandler

BOOL CSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  hier zusätzliche Initialisierung hinzufügen.
	m_ctrlDifficultySlider.SetRange(0,4);
	m_ctrlDifficultySlider.SetTicFreq(1);
	m_ctrlMusicvolume.SetRange(0,100);
	m_ctrlMusicvolume.SetTicFreq(1);
	m_ctrlTooltipDelay.SetRange(0,2000);
	m_ctrlTooltipDelay.SetTicFreq(50);
	m_ctrlStarDensity.SetRange(0,100);
	m_ctrlStarDensity.SetTicFreq(1);
	m_ctrlMinorDensity.SetRange(0,100);
	m_ctrlMinorDensity.SetTicFreq(1);
	m_ctrlAnomalyDensity.SetRange(0,100);
	m_ctrlAnomalyDensity.SetTicFreq(1);
	m_ctrlAlienFrequency.SetRange(0, max_alien_frequency);
	m_ctrlAlienFrequency.SetTicFreq(1);

	CIniLoader* pIni = CIniLoader::GetInstance();
	AssertBotE(pIni);

	// General
	bool bAutosave = true;
	if (!pIni->ReadValue("General", "AUTOSAVE", bAutosave))
		AssertBotE(false);
	m_bAutoave = bAutosave;

	if (!pIni->ReadValue("General", "DIFFICULTY", m_sDifficulty))
		AssertBotE(false);
	m_sDifficulty.MakeUpper();
	if (m_sDifficulty == "BABY")
		m_ctrlDifficultySlider.SetPos(0);
	else if (m_sDifficulty == "EASY")
		m_ctrlDifficultySlider.SetPos(1);
	else if (m_sDifficulty == "NORMAL")
		m_ctrlDifficultySlider.SetPos(2);
	else if (m_sDifficulty == "HARD")
		m_ctrlDifficultySlider.SetPos(3);
	else if (m_sDifficulty == "IMPOSSIBLE")
		m_ctrlDifficultySlider.SetPos(4);

	CWnd* pCtrl = GetDlgItem(IDC_STATIC_DIFFICULTY);
	if (pCtrl)
		pCtrl->SetWindowText(m_sDifficulty);

	//Galaxy
	int nearbysystems = 2;
	if (!pIni->ReadValue("Galaxy", "NEARBYSYSTEMS", nearbysystems))
		AssertBotE(false);
	nearbysystems = max(min(nearbysystems, 8), 0);
	CString sNearbysystems;
	sNearbysystems.Format("%i", nearbysystems);
	m_edtNearbysystems.SetWindowText(sNearbysystems);

	int nStarDensity = 35;
	if (!pIni->ReadValue("Galaxy", "STARDENSITY", nStarDensity))
		AssertBotE(false);
	m_ctrlStarDensity.SetPos(nStarDensity);

	int nMinorDensity = 30;
	if (!pIni->ReadValue("Galaxy", "MINORDENSITY", nMinorDensity))
		AssertBotE(false);
	m_ctrlMinorDensity.SetPos(nMinorDensity);

	int nAnomalyDensity = 9;
	if (!pIni->ReadValue("Galaxy", "ANOMALYDENSITY", nAnomalyDensity))
		AssertBotE(false);
	m_ctrlAnomalyDensity.SetPos(nAnomalyDensity);

	//Galaxyshape
	m_comboGalaxyshape.AddString("irregular");
	m_comboGalaxyshape.AddString("elliptic");
	m_comboGalaxyshape.AddString("spiral");
	m_comboGalaxyshape.AddString("ring");
	m_comboGalaxyshape.AddString("lenticular");
	m_comboGalaxyshape.AddString("islands");
    int genMode=0;
	pIni->ReadValue("Galaxy", "GENERATIONMODE", genMode);
	m_comboGalaxyshape.SetCurSel(genMode);

	m_comboGalaxysize.AddString("TINY 15x10");
	m_comboGalaxysize.AddString("SMALL 20x15");
	m_comboGalaxysize.AddString("CLASSIC 30x20");
	m_comboGalaxysize.AddString("HUGE 40x30");
	int sizeh=30,sizev=20;
	pIni->ReadValue("Galaxy", "MAPSIZEH", sizeh);
	pIni->ReadValue("Galaxy", "MAPSIZEV", sizev);
	if(sizeh==15&&sizev==10)
	{
		m_comboGalaxysize.SetCurSel(0);
	}
	else if(sizeh==20&&sizev==15)
	{
		m_comboGalaxysize.SetCurSel(1);
	}
	else if(sizeh==30&&sizev==20)
	{
		m_comboGalaxysize.SetCurSel(2);
	}
	else if(sizeh==40&&sizev==30)
	{
		m_comboGalaxysize.SetCurSel(3);
	}
	else
	{
		CString s;
		s.Format("Custom %dx%d",sizeh,sizev);
		m_comboGalaxysize.AddString(s);
		m_comboGalaxysize.SetCurSel(3);
	}


	// Audio
	bool bHardwareSound = true;
	if (!pIni->ReadValue("Audio", "HARDWARESOUND", bHardwareSound))
		AssertBotE(false);
	m_bHardwaresound = bHardwareSound;

	bool bSound = true;
	if (!pIni->ReadValue("Audio", "SOUND", bSound))
		AssertBotE(false);
	m_bSound = bSound;

	bool bMusic = true;
	if (!pIni->ReadValue("Audio", "MUSIC", bMusic))
		AssertBotE(false);
	m_bMusic = bMusic;

	float fMusicVolume = 0.3f;
	if (!pIni->ReadValue("Audio", "MUSICVOLUME", fMusicVolume))
		AssertBotE(false);
	m_ctrlMusicvolume.SetPos(fMusicVolume * 100);

	// Video
	bool bShowTraderoutes = true;
	if (!pIni->ReadValue("Video", "SHOWTRADEROUTES", bShowTraderoutes))
		AssertBotE(false);
	m_bShowTraderoutes = bShowTraderoutes;

	bool bAnimatedIcon = true;
	if (!pIni->ReadValue("Video", "ANIMATEDICON", bAnimatedIcon))
		AssertBotE(false);
	m_bAnimatedIcon = bAnimatedIcon;

	bool bShowMiniMap = true;
	if (!pIni->ReadValue("Video", "SHOWMINIMAP", bShowMiniMap))
		AssertBotE(false);
	m_bShowMiniMap = bShowMiniMap;

	bool bShowEventPictures = true;
	if (!pIni->ReadValue("Video", "SHOWEVENTPICTURES", bShowEventPictures))
		AssertBotE(false);
	m_bShowEventPictures = bShowEventPictures;

	int nTooltipDelay = 750;
	if (!pIni->ReadValue("Video", "TOOLTIPDELAY", nTooltipDelay))
		AssertBotE(false);
	m_ctrlTooltipDelay.SetPos(nTooltipDelay);

	// Control
	bool bShowScrollbars = false;
	if (!pIni->ReadValue("Control", "SHOWSCROLLBARS", bShowScrollbars))
		AssertBotE(false);
	m_bShowScrollBars = bShowScrollbars;

	bool bInvertMouse = false;
	if (!pIni->ReadValue("Control", "INVERTMOUSE", bInvertMouse))
		AssertBotE(false);
	m_bInvertMouse = bInvertMouse;

	bool bHideMenu = false;
	if (!pIni->ReadValue("Control", "HIDEMENUBAR", bHideMenu))
		AssertBotE(false);
	m_bHideMenu = bHideMenu;

	// Special (Ingame)
	int nRandomSeed = -1;
	if (!pIni->ReadValue("Special", "RANDOMSEED", nRandomSeed))
		AssertBotE(false);
	if (nRandomSeed < -1)
		nRandomSeed = -1;
	CString sRandomSeed;
	sRandomSeed.Format("%d", nRandomSeed);
	m_edtRandomSeed.SetWindowText(sRandomSeed);

	float nAlienFrequency = 0;
	if (!pIni->ReadValue("Special", "ALIENENTITIES", nAlienFrequency))
		AssertBotE(false);
	m_ctrlAlienFrequency.SetPos(nAlienFrequency);

	bool bRandomEvents = true;
	if (!pIni->ReadValue("Special", "RANDOMEVENTS", bRandomEvents))
		AssertBotE(false);
	m_bRandomEvents = bRandomEvents;

	m_comboExpansionspeed.AddString("SLOW");
	m_comboExpansionspeed.AddString("MEDIUM");
	m_comboExpansionspeed.AddString("CLASSIC");
	m_comboExpansionspeed.AddString("FAST");
	CString speed;
	if (!pIni->ReadValue("Special", "EXPANSIONSPEED", speed))
		AssertBotE(false);
	const int found = m_comboExpansionspeed.FindString(-1, speed);
	if(found == CB_ERR)
		m_comboExpansionspeed.SetCurSel(2);
	else
		m_comboExpansionspeed.SetCurSel(found);

	// Victory Conditions
	bool bVCElimination = true;
	if (!pIni->ReadValue("Victory_Conditions", "Elimination", bVCElimination))
		AssertBotE(false);
	m_bVCElimination = bVCElimination;

	bool bVCDiplomacy = false;
	if (!pIni->ReadValue("Victory_Conditions", "Diplomacy", bVCDiplomacy))
		AssertBotE(false);
	m_bVCDiplomacy = bVCDiplomacy;

	bool bVCConquest = false;
	if (!pIni->ReadValue("Victory_Conditions", "Conquest", bVCConquest))
		AssertBotE(false);
	m_bVCConquest = bVCConquest;

	bool bVCResearch = false;
	if (!pIni->ReadValue("Victory_Conditions", "Research", bVCResearch))
		AssertBotE(false);
	m_bVCResearch = bVCResearch;

	bool bVCCombat = false;
	if (!pIni->ReadValue("Victory_Conditions", "Combat", bVCCombat))
		AssertBotE(false);
	m_bVCCombat = bVCCombat;

	bool bVCSabotage = false;
	if (!pIni->ReadValue("Victory_Conditions", "Sabotage", bVCSabotage))
		AssertBotE(false);
	m_bVCSabotage = bVCSabotage;

	// alle nicht während des Spiels änderbaren Einstellungen deaktivieren
	if (m_bDisable)
	{
		CWnd* pWnd = GetDlgItem(IDC_SLIDER_STARDENSITY);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_SLIDER_MINORDENSITY);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_SLIDER_ANOMALYDENSITY);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_CHECK_VC_ELIMINATION);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_CHECK_VC_DIPLOMACY);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_CHECK_VC_CONQUEST);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_CHECK_VC_RESEARCH);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_CHECK_VC_COMBAT);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_CHECK_VC_SABOTAGE);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_COMBOGALAXYSIZE);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_COMBOGALAXYSHAPE);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_EDIT_RANDOMSEED);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_CHECK_RANDOMEVENTS);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

void CSettingsDlg::OnOK()
{
	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	// neue Werte in die Ini-Datei schreiben
	UpdateData(true);

	CIniLoader* pIni = CIniLoader::GetInstance();
	AssertBotE(pIni);

	CString s;
	// General
	m_bAutoave == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("General", "AUTOSAVE", s);
	pIni->WriteValue("General", "DIFFICULTY", m_sDifficulty);

	// Audio
	m_bHardwaresound == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Audio", "HARDWARESOUND", s);
	m_bSound == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Audio", "SOUND", s);
	m_bMusic == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Audio", "MUSIC", s);
	s.Format("%.2lf", (float)m_ctrlMusicvolume.GetPos() / 100.0f);
	pIni->WriteValue("Audio", "MUSICVOLUME", s);

	// Video
	m_bShowTraderoutes == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Video", "SHOWTRADEROUTES", s);
	m_bAnimatedIcon == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Video", "ANIMATEDICON", s);
	m_bShowMiniMap == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Video", "SHOWMINIMAP", s);
	m_bShowEventPictures == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Video", "SHOWEVENTPICTURES", s);
	s.Format("%d", m_ctrlTooltipDelay.GetPos());
	pIni->WriteValue("Video", "TOOLTIPDELAY", s);

	// Control
	m_bShowScrollBars == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Control", "SHOWSCROLLBARS", s);
	m_bInvertMouse == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Control", "INVERTMOUSE", s);
	m_bHideMenu == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Control", "HIDEMENUBAR", s);

	// Special (Ingame)
	m_edtRandomSeed.GetWindowText(s);
	pIni->WriteValue("Special", "RANDOMSEED", s);
	s.Format("%d", m_ctrlAlienFrequency.GetPos());
	pIni->WriteValue("Special", "ALIENENTITIES", s);
	m_bRandomEvents == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Special", "RANDOMEVENTS", s);
	m_comboExpansionspeed.GetLBText(m_comboExpansionspeed.GetCurSel(), s);
	pIni->WriteValue("Special", "EXPANSIONSPEED", s);

	//Galaxy
	m_edtNearbysystems.GetWindowText(s);
	pIni->WriteValue("Galaxy", "NEARBYSYSTEMS", s);
	s.Format("%d", m_ctrlStarDensity.GetPos());
	pIni->WriteValue("Galaxy", "STARDENSITY", s);
	s.Format("%d", m_ctrlMinorDensity.GetPos());
	pIni->WriteValue("Galaxy", "MINORDENSITY", s);
	s.Format("%d", m_ctrlAnomalyDensity.GetPos());
	pIni->WriteValue("Galaxy", "ANOMALYDENSITY", s);

	int choosen=m_comboGalaxysize.GetCurSel();
	if(choosen==0)
	{
		s.Format("%d",15);
		pIni->WriteValue("Galaxy", "MAPSIZEH", s);//Tiny
		s.Format("%d",10);
		pIni->WriteValue("Galaxy", "MAPSIZEV", s);
	}
	else if(choosen==1)
	{
		s.Format("%d",20);
		pIni->WriteValue("Galaxy", "MAPSIZEH", s);//Small
		s.Format("%d",15);
		pIni->WriteValue("Galaxy", "MAPSIZEV", s);
	}
	else if(choosen==2)
	{
		s.Format("%d",30);
		pIni->WriteValue("Galaxy", "MAPSIZEH", s);//Classic
		s.Format("%d",20);
		pIni->WriteValue("Galaxy", "MAPSIZEV", s);
	}
	else if(choosen==3)
	{
		s.Format("%d",40);
		pIni->WriteValue("Galaxy", "MAPSIZEH", s);//Huge
		s.Format("%d",30);
		pIni->WriteValue("Galaxy", "MAPSIZEV", s);
	}
	s.Format("%d", m_comboGalaxyshape.GetCurSel());
	pIni->WriteValue("Galaxy", "GENERATIONMODE", s);

	// Victory Conditions
	m_bVCElimination == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Victory_Conditions", "Elimination", s);
	m_bVCDiplomacy == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Victory_Conditions", "Diplomacy", s);
	m_bVCConquest == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Victory_Conditions", "Conquest", s);
	m_bVCResearch == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Victory_Conditions", "Research", s);
	m_bVCCombat == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Victory_Conditions", "Combat", s);
	m_bVCSabotage == TRUE ? s = "ON" : s = "OFF";
	pIni->WriteValue("Victory_Conditions", "Sabotage", s);

	CDialog::OnOK();
}

void CSettingsDlg::OnNMCustomdrawSliderDifficulty(NMHDR* /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	UpdateData(true);

	if (m_ctrlDifficultySlider.GetPos() == 0)
		m_sDifficulty	= "BABY";
	else if (m_ctrlDifficultySlider.GetPos() == 1)
		m_sDifficulty	= "EASY";
	else if (m_ctrlDifficultySlider.GetPos() == 2)
		m_sDifficulty	= "NORMAL";
	else if (m_ctrlDifficultySlider.GetPos() == 3)
		m_sDifficulty	= "HARD";
	else if (m_ctrlDifficultySlider.GetPos() == 4)
		m_sDifficulty	= "IMPOSSIBLE";
	CWnd* pCtrl = GetDlgItem(IDC_STATIC_DIFFICULTY);
	if (pCtrl)
		pCtrl->SetWindowText(m_sDifficulty);
	*pResult = 0;
}
