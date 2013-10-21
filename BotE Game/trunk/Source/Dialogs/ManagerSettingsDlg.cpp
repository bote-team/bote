// ManagerSettingsDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BotE.h"
#include "ManagerSettingsDlg.h"
#include "System/Manager.h"

#include <cassert>


// CManagerSettingsDlg-Dialogfeld

IMPLEMENT_DYNAMIC(CManagerSettingsDlg, CDialog)

CManagerSettingsDlg::CManagerSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManagerSettingsDlg::IDD, pParent), m_Manager(NULL)
	, m_bActive(FALSE)
	, m_bSafeMoral(FALSE)
	, m_bMaxIndustry(FALSE)
	, m_bNeglectFood(FALSE)
{

}

CManagerSettingsDlg::CManagerSettingsDlg(CSystemManager* manager, CWnd* pParent)
	: CDialog(CManagerSettingsDlg::IDD, pParent), m_Manager(manager),
	m_bActive(FALSE),
	m_bSafeMoral(FALSE),
	m_bMaxIndustry(FALSE),
	m_bNeglectFood(FALSE)
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
END_MESSAGE_MAP()


// CManagerSettingsDlg-Meldungshandler

BOOL CManagerSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bActive = m_Manager->Active();
	m_bSafeMoral = m_Manager->SafeMoral();
	m_bMaxIndustry = m_Manager->MaxIndustry();
	m_bNeglectFood = m_Manager->NeglectFood();

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

	m_ctrlProductionSlider.SetPos(m_Manager->Priority(WORKER::INDUSTRY_WORKER));
	m_ctrlSecuritySlider.SetPos(m_Manager->Priority(WORKER::SECURITY_WORKER));
	m_ctrlResearchSlider.SetPos(m_Manager->Priority(WORKER::RESEARCH_WORKER));
	m_ctrlTitanSlider.SetPos(m_Manager->Priority(WORKER::TITAN_WORKER));
	m_ctrlDeuteriumSlider.SetPos(m_Manager->Priority(WORKER::DEUTERIUM_WORKER));
	m_ctrlDuraniumSlider.SetPos(m_Manager->Priority(WORKER::DURANIUM_WORKER));
	m_ctrlCrystalSlider.SetPos(m_Manager->Priority(WORKER::CRYSTAL_WORKER));
	m_ctrlIridiumSlider.SetPos(m_Manager->Priority(WORKER::IRIDIUM_WORKER));

	SetDisplayedPrio(IDC_SLIDER_PRODUCTION, m_ctrlProductionSlider.GetPos());
	SetDisplayedPrio(IDC_SLIDER_SECURITY, m_ctrlSecuritySlider.GetPos());
	SetDisplayedPrio(IDC_SLIDER_RESEARCH, m_ctrlResearchSlider.GetPos());
	SetDisplayedPrio(IDC_SLIDER_TITAN, m_ctrlTitanSlider.GetPos());
	SetDisplayedPrio(IDC_SLIDER_DEUTERIUM, m_ctrlProductionSlider.GetPos());
	SetDisplayedPrio(IDC_SLIDER_DURANIUM, m_ctrlDuraniumSlider.GetPos());
	SetDisplayedPrio(IDC_SLIDER_CRYSTAL, m_ctrlCrystalSlider.GetPos());
	SetDisplayedPrio(IDC_SLIDER_IRIDIUM, m_ctrlIridiumSlider.GetPos());

	UpdateData(false);
	SetStates(m_bActive);

	return TRUE;  // return TRUE unless you set the focus to a control
	// AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}

void CManagerSettingsDlg::OnOK()
{
	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.

	UpdateData(true);

	m_Manager->SetActive(m_bActive ? true : false);
	m_Manager->SetSafeMoral(m_bSafeMoral ? true : false);
	m_Manager->SetMaxIndustry(m_bMaxIndustry ? true : false);
	m_Manager->SetNeglectFood(m_bNeglectFood ? true : false);

	m_Manager->ClearPriorities();

	m_Manager->AddPriority(WORKER::INDUSTRY_WORKER, m_ctrlProductionSlider.GetPos());
	m_Manager->AddPriority(WORKER::SECURITY_WORKER, m_ctrlSecuritySlider.GetPos());
	m_Manager->AddPriority(WORKER::RESEARCH_WORKER, m_ctrlResearchSlider.GetPos());
	m_Manager->AddPriority(WORKER::TITAN_WORKER, m_ctrlTitanSlider.GetPos());
	m_Manager->AddPriority(WORKER::DEUTERIUM_WORKER, m_ctrlDeuteriumSlider.GetPos());
	m_Manager->AddPriority(WORKER::DURANIUM_WORKER, m_ctrlDuraniumSlider.GetPos());
	m_Manager->AddPriority(WORKER::CRYSTAL_WORKER, m_ctrlCrystalSlider.GetPos());
	m_Manager->AddPriority(WORKER::IRIDIUM_WORKER, m_ctrlIridiumSlider.GetPos());

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
	SetState(IDC_SLIDER_SECURITY, active);
	SetState(IDC_SLIDER_RESEARCH, active);
	SetState(IDC_SLIDER_TITAN, active);
	SetState(IDC_SLIDER_DEUTERIUM, active);
	SetState(IDC_SLIDER_DURANIUM, active);
	SetState(IDC_SLIDER_CRYSTAL, active);
	SetState(IDC_SLIDER_IRIDIUM, active);
	SetState(IDC_SLIDER_PRODUCTION, active);

	SetState(IDC_CHECK_SAFE_MORAL, active);
	SetState(IDC_CHECK_MAX_INDUSTRY, active);
	SetState(IDC_CHECK_NEGLECT_FOOD, active);
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

void CManagerSettingsDlg::SetDisplayedPrio(int item, int prio)
{
	CString text;
	text.Format("%u", prio);
	SetDlgItem(item, text);
}

void CManagerSettingsDlg::OnNMCustomdrawSliderProduction(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedPrio(IDC_STATIC_PRODUCTION, m_ctrlProductionSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderSecurity(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedPrio(IDC_STATIC_SECURITY, m_ctrlSecuritySlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderResearch(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedPrio(IDC_STATIC_RESEARCH, m_ctrlResearchSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderTitan(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedPrio(IDC_STATIC_TITAN, m_ctrlTitanSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderDeuterium(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedPrio(IDC_STATIC_DEUTERIUM, m_ctrlDeuteriumSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderDuranium(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedPrio(IDC_STATIC_DURANIUM, m_ctrlDuraniumSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderCrystal(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedPrio(IDC_STATIC_CRYSTAL, m_ctrlCrystalSlider.GetPos());
	*pResult = 0;
}

void CManagerSettingsDlg::OnNMCustomdrawSliderIridium(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
	/*LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);*/
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	SetDisplayedPrio(IDC_STATIC_IRIDIUM, m_ctrlIridiumSlider.GetPos());
	*pResult = 0;
}
