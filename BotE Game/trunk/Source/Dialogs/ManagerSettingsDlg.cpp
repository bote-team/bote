// ManagerSettingsDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BotE.h"
#include "ManagerSettingsDlg.h"
#include "System/Manager.h"
#include "System/system.h"

#include <cassert>


// CManagerSettingsDlg-Dialogfeld

IMPLEMENT_DYNAMIC(CManagerSettingsDlg, CDialog)

CManagerSettingsDlg::CManagerSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManagerSettingsDlg::IDD, pParent), m_Manager(NULL)
	, m_bActive(FALSE)
	, m_bSafeMoral(FALSE)
	, m_bMaxIndustry(FALSE)
	, m_bNeglectFood(FALSE)
	, m_bBombWarning(FALSE)
	, m_bOnOffline(FALSE)
	, m_System(NULL)
{

}

CManagerSettingsDlg::CManagerSettingsDlg(CSystemManager* manager, const CSystem& system, CWnd* pParent)
	: CDialog(CManagerSettingsDlg::IDD, pParent), m_Manager(manager),
	m_bActive(FALSE),
	m_bSafeMoral(FALSE),
	m_bMaxIndustry(FALSE),
	m_bNeglectFood(FALSE),
	m_bBombWarning(FALSE),
	m_bOnOffline(FALSE),
	m_System(&system)
{

}

CManagerSettingsDlg::~CManagerSettingsDlg()
{
}

void CManagerSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ACTIVE, m_bActive);
	DDX_Control(pDX, IDC_SLIDER_SECURITY, m_ctrlSecuritySlider);
	DDX_Control(pDX, IDC_SLIDER_RESEARCH, m_ctrlResearchSlider);
	DDX_Control(pDX, IDC_SLIDER_TITAN, m_ctrlTitanSlider);
	DDX_Control(pDX, IDC_SLIDER_DEUTERIUM, m_ctrlDeuteriumSlider);
	DDX_Control(pDX, IDC_SLIDER_DURANIUM, m_ctrlDuraniumSlider);
	DDX_Control(pDX, IDC_SLIDER_CRYSTAL, m_ctrlCrystalSlider);
	DDX_Control(pDX, IDC_SLIDER_IRIDIUM, m_ctrlIridiumSlider);
	DDX_Check(pDX, IDC_CHECK_SAFE_MORAL, m_bSafeMoral);
	DDX_Check(pDX, IDC_CHECK_MAX_INDUSTRY, m_bMaxIndustry);
	DDX_Check(pDX, IDC_CHECK_NEGLECT_FOOD, m_bNeglectFood);
	DDX_Control(pDX, IDC_SLIDER_PRODUCTION, m_ctrlProductionSlider);
	DDX_Check(pDX, IDC_CHECK_BOMB_WARNING, m_bBombWarning);
	DDX_Control(pDX, IDC_SLIDER_MIN_MORAL, m_ctrlMinMoralSlider);
	DDX_Control(pDX, IDC_SLIDER_MIN_MORAL_PROD, m_ctrlMinMoralProdSlider);
	DDX_Check(pDX, IDC_CHECK_ON_OFFLINE, m_bOnOffline);
}


BEGIN_MESSAGE_MAP(CManagerSettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE, &CManagerSettingsDlg::OnBnClickedCheckActive)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PRODUCTION, &CManagerSettingsDlg::OnNMCustomdrawSliderProduction)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SECURITY, &CManagerSettingsDlg::OnNMCustomdrawSliderSecurity)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RESEARCH, &CManagerSettingsDlg::OnNMCustomdrawSliderResearch)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_TITAN, &CManagerSettingsDlg::OnNMCustomdrawSliderTitan)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DEUTERIUM, &CManagerSettingsDlg::OnNMCustomdrawSliderDeuterium)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_DURANIUM, &CManagerSettingsDlg::OnNMCustomdrawSliderDuranium)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CRYSTAL, &CManagerSettingsDlg::OnNMCustomdrawSliderCrystal)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_IRIDIUM, &CManagerSettingsDlg::OnNMCustomdrawSliderIridium)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MIN_MORAL, &CManagerSettingsDlg::OnNMCustomdrawSliderMinMoral)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MIN_MORAL_PROD, &CManagerSettingsDlg::OnNMCustomdrawSliderMinMoralProd)
	ON_BN_CLICKED(IDC_CHECK_ON_OFFLINE, &CManagerSettingsDlg::OnBnClickedCheckOnOffline)
END_MESSAGE_MAP()


// CManagerSettingsDlg-Meldungshandler

int CManagerSettingsDlg::SliderPos(WORKER::Typ type) const
{
	return m_System->HasWorkerBuilding(type) ? m_Manager->Priority(type) : CSystemManager::min_priority;
}

BOOL CManagerSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bActive = m_Manager->Active();
	m_bSafeMoral = m_Manager->SafeMoral();
	m_bMaxIndustry = m_Manager->MaxIndustry();
	m_bNeglectFood = m_Manager->NeglectFood();
	m_bBombWarning = m_Manager->BombWarning();
	m_bOnOffline = m_Manager->OnOffline();

	m_ctrlProductionSlider.SetRange(CSystemManager::min_priority, CSystemManager::max_priority);
	m_ctrlProductionSlider.SetTicFreq(tick_frequ);
	m_ctrlSecuritySlider.SetRange(CSystemManager::min_priority, CSystemManager::max_priority);
	m_ctrlSecuritySlider.SetTicFreq(tick_frequ);
	m_ctrlResearchSlider.SetRange(CSystemManager::min_priority, CSystemManager::max_priority);
	m_ctrlResearchSlider.SetTicFreq(tick_frequ);
	m_ctrlTitanSlider.SetRange(CSystemManager::min_priority, CSystemManager::max_priority);
	m_ctrlTitanSlider.SetTicFreq(tick_frequ);
	m_ctrlDeuteriumSlider.SetRange(CSystemManager::min_priority, CSystemManager::max_priority);
	m_ctrlDeuteriumSlider.SetTicFreq(tick_frequ);
	m_ctrlDuraniumSlider.SetRange(CSystemManager::min_priority, CSystemManager::max_priority);
	m_ctrlDuraniumSlider.SetTicFreq(tick_frequ);
	m_ctrlCrystalSlider.SetRange(CSystemManager::min_priority, CSystemManager::max_priority);
	m_ctrlCrystalSlider.SetTicFreq(tick_frequ);
	m_ctrlIridiumSlider.SetRange(CSystemManager::min_priority, CSystemManager::max_priority);
	m_ctrlIridiumSlider.SetTicFreq(tick_frequ);

	const float factor = static_cast<float>(tick_frequ) /
		(CSystemManager::max_priority - CSystemManager::min_priority);
	const int min_moral_tick_frequ = factor * (CSystemManager::max_min_moral - CSystemManager::min_min_moral);
	m_ctrlMinMoralSlider.SetRange(CSystemManager::min_min_moral, CSystemManager::max_min_moral);
	m_ctrlMinMoralSlider.SetTicFreq(min_moral_tick_frequ);
	const int min_moral_prod_tick_frequ = factor *
		(CSystemManager::max_min_moral_prod - CSystemManager::min_min_moral_prod);
	m_ctrlMinMoralProdSlider.SetRange(CSystemManager::min_min_moral_prod, CSystemManager::max_min_moral_prod);
	m_ctrlMinMoralProdSlider.SetTicFreq(min_moral_prod_tick_frequ);

	m_ctrlProductionSlider.SetPos(SliderPos(WORKER::INDUSTRY_WORKER));
	m_ctrlSecuritySlider.SetPos(SliderPos(WORKER::SECURITY_WORKER));
	m_ctrlResearchSlider.SetPos(SliderPos(WORKER::RESEARCH_WORKER));
	m_ctrlTitanSlider.SetPos(SliderPos(WORKER::TITAN_WORKER));
	m_ctrlDeuteriumSlider.SetPos(SliderPos(WORKER::DEUTERIUM_WORKER));
	m_ctrlDuraniumSlider.SetPos(SliderPos(WORKER::DURANIUM_WORKER));
	m_ctrlCrystalSlider.SetPos(SliderPos(WORKER::CRYSTAL_WORKER));
	m_ctrlIridiumSlider.SetPos(SliderPos(WORKER::IRIDIUM_WORKER));

	m_ctrlMinMoralSlider.SetPos(m_Manager->MinMoral());
	m_ctrlMinMoralProdSlider.SetPos(m_Manager->MinMoralProd());

	SetDisplayedStaticText(IDC_SLIDER_PRODUCTION, m_ctrlProductionSlider.GetPos());
	SetDisplayedStaticText(IDC_SLIDER_SECURITY, m_ctrlSecuritySlider.GetPos());
	SetDisplayedStaticText(IDC_SLIDER_RESEARCH, m_ctrlResearchSlider.GetPos());
	SetDisplayedStaticText(IDC_SLIDER_TITAN, m_ctrlTitanSlider.GetPos());
	SetDisplayedStaticText(IDC_SLIDER_DEUTERIUM, m_ctrlDeuteriumSlider.GetPos());
	SetDisplayedStaticText(IDC_SLIDER_DURANIUM, m_ctrlDuraniumSlider.GetPos());
	SetDisplayedStaticText(IDC_SLIDER_CRYSTAL, m_ctrlCrystalSlider.GetPos());
	SetDisplayedStaticText(IDC_SLIDER_IRIDIUM, m_ctrlIridiumSlider.GetPos());

	SetDisplayedStaticText(IDC_SLIDER_MIN_MORAL, m_ctrlMinMoralSlider.GetPos(), true);
	SetDisplayedStaticText(IDC_SLIDER_MIN_MORAL_PROD, m_ctrlMinMoralProdSlider.GetPos(), true);

	UpdateData(false);
	SetStates(m_bActive);

	return TRUE;  // return TRUE unless you set the focus to a control
	// AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

void CManagerSettingsDlg::OnOK()
{
	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.

	UpdateData(true);

	m_Manager->SetActive(m_bActive);
	m_Manager->SetSafeMoral(m_bSafeMoral);
	m_Manager->SetMaxIndustry(m_bMaxIndustry);
	m_Manager->SetNeglectFood(m_bNeglectFood);
	m_Manager->SetBombWarning(m_bBombWarning);
	m_Manager->SetOnOffline(m_bOnOffline);

	m_Manager->ClearPriorities();

	m_Manager->AddPriority(WORKER::INDUSTRY_WORKER, m_ctrlProductionSlider.GetPos());
	m_Manager->AddPriority(WORKER::SECURITY_WORKER, m_ctrlSecuritySlider.GetPos());
	m_Manager->AddPriority(WORKER::RESEARCH_WORKER, m_ctrlResearchSlider.GetPos());
	m_Manager->AddPriority(WORKER::TITAN_WORKER, m_ctrlTitanSlider.GetPos());
	m_Manager->AddPriority(WORKER::DEUTERIUM_WORKER, m_ctrlDeuteriumSlider.GetPos());
	m_Manager->AddPriority(WORKER::DURANIUM_WORKER, m_ctrlDuraniumSlider.GetPos());
	m_Manager->AddPriority(WORKER::CRYSTAL_WORKER, m_ctrlCrystalSlider.GetPos());
	m_Manager->AddPriority(WORKER::IRIDIUM_WORKER, m_ctrlIridiumSlider.GetPos());

	m_Manager->SetMinMoral(m_ctrlMinMoralSlider.GetPos());
	m_Manager->SetMinMoralProd(m_ctrlMinMoralProdSlider.GetPos());

	CDialog::OnOK();
}

void CManagerSettingsDlg::SetState(int item, BOOL active)
{
	CWnd* pWnd = GetDlgItem(item);
	assert(pWnd);
	pWnd->EnableWindow(active);
}

void CManagerSettingsDlg::SetStates(BOOL active)
{
	SetState(IDC_SLIDER_PRODUCTION, active && m_System->HasWorkerBuilding(WORKER::INDUSTRY_WORKER));
	SetState(IDC_SLIDER_SECURITY, active && m_System->HasWorkerBuilding(WORKER::SECURITY_WORKER));
	SetState(IDC_SLIDER_RESEARCH, active && m_System->HasWorkerBuilding(WORKER::RESEARCH_WORKER));
	SetState(IDC_SLIDER_TITAN, active && m_System->HasWorkerBuilding(WORKER::TITAN_WORKER));
	SetState(IDC_SLIDER_DEUTERIUM, active && m_System->HasWorkerBuilding(WORKER::DEUTERIUM_WORKER));
	SetState(IDC_SLIDER_DURANIUM, active && m_System->HasWorkerBuilding(WORKER::DURANIUM_WORKER));
	SetState(IDC_SLIDER_CRYSTAL, active && m_System->HasWorkerBuilding(WORKER::CRYSTAL_WORKER));
	SetState(IDC_SLIDER_IRIDIUM, active && m_System->HasWorkerBuilding(WORKER::IRIDIUM_WORKER));

	SetState(IDC_CHECK_SAFE_MORAL, active);
	SetState(IDC_CHECK_MAX_INDUSTRY, active);
	SetState(IDC_CHECK_NEGLECT_FOOD, active);

	SetState(IDC_CHECK_BOMB_WARNING, active);
	SetState(IDC_CHECK_ON_OFFLINE, active);

	SetState(IDC_SLIDER_MIN_MORAL, m_bOnOffline && active);
	SetState(IDC_SLIDER_MIN_MORAL_PROD, m_bOnOffline && active);
}

void CManagerSettingsDlg::OnBnClickedCheckActive()
{
	UpdateData(true);
	SetStates(m_bActive);
}

void CManagerSettingsDlg::SetDlgItem(int item, const CString& text)
{
	CWnd* pCtrl = GetDlgItem(item);
	assert(pCtrl);
	pCtrl->SetWindowText(text);
}

void CManagerSettingsDlg::SetDisplayedStaticText(int item, int value, bool moral)
{
	CString text;
	text.Format("%i", value);
	if(!moral)
	{
		if(value == CSystemManager::min_priority)
			text = "None";
		else if(value == CSystemManager::max_priority)
			text = "Fill All";
	}
	SetDlgItem(item, text);
}

void CManagerSettingsDlg::OnNMCustomdrawSliderProduction(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_PRODUCTION, m_ctrlProductionSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderSecurity(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_SECURITY, m_ctrlSecuritySlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderResearch(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_RESEARCH, m_ctrlResearchSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderTitan(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_TITAN, m_ctrlTitanSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderDeuterium(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_DEUTERIUM, m_ctrlDeuteriumSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderDuranium(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_DURANIUM, m_ctrlDuraniumSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderCrystal(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_CRYSTAL, m_ctrlCrystalSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderIridium(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_IRIDIUM, m_ctrlIridiumSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderMinMoral(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_MIN_MORAL, m_ctrlMinMoralSlider.GetPos(), true);
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderMinMoralProd(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedStaticText(IDC_STATIC_MIN_MORAL_PROD, m_ctrlMinMoralProdSlider.GetPos(), true);
	*pResult = 0;
}

void CManagerSettingsDlg::OnBnClickedCheckOnOffline()
{
	UpdateData(true);
	SetStates(m_bActive);
}
