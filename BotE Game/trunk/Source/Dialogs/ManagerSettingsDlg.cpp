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
}


BEGIN_MESSAGE_MAP(CManagerSettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE, &CManagerSettingsDlg::OnBnClickedCheckActive)
END_MESSAGE_MAP()


// CManagerSettingsDlg-Meldungshandler

BOOL CManagerSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bActive = m_Manager->Active();
	m_bSafeMoral = m_Manager->SafeMoral();
	m_bMaxIndustry = m_Manager->MaxIndustry();
	m_bNeglectFood = m_Manager->NeglectFood();

	m_ctrlSecuritySlider.SetRange(1, CSystemManager::max_priority);
	m_ctrlSecuritySlider.SetTicFreq(tick_frequ);
	m_ctrlResearchSlider.SetRange(1, CSystemManager::max_priority);
	m_ctrlResearchSlider.SetTicFreq(tick_frequ);
	m_ctrlTitanSlider.SetRange(1, CSystemManager::max_priority);
	m_ctrlTitanSlider.SetTicFreq(tick_frequ);
	m_ctrlDeuteriumSlider.SetRange(1, CSystemManager::max_priority);
	m_ctrlDeuteriumSlider.SetTicFreq(tick_frequ);
	m_ctrlDuraniumSlider.SetRange(1, CSystemManager::max_priority);
	m_ctrlDuraniumSlider.SetTicFreq(tick_frequ);
	m_ctrlCrystalSlider.SetRange(1, CSystemManager::max_priority);
	m_ctrlCrystalSlider.SetTicFreq(tick_frequ);
	m_ctrlIridiumSlider.SetRange(1, CSystemManager::max_priority);
	m_ctrlIridiumSlider.SetTicFreq(tick_frequ);

	m_ctrlSecuritySlider.SetPos(m_Manager->Priority(WORKER::SECURITY_WORKER));
	m_ctrlResearchSlider.SetPos(m_Manager->Priority(WORKER::RESEARCH_WORKER));
	m_ctrlTitanSlider.SetPos(m_Manager->Priority(WORKER::TITAN_WORKER));
	m_ctrlDeuteriumSlider.SetPos(m_Manager->Priority(WORKER::DEUTERIUM_WORKER));
	m_ctrlDuraniumSlider.SetPos(m_Manager->Priority(WORKER::DURANIUM_WORKER));
	m_ctrlCrystalSlider.SetPos(m_Manager->Priority(WORKER::CRYSTAL_WORKER));
	m_ctrlIridiumSlider.SetPos(m_Manager->Priority(WORKER::IRIDIUM_WORKER));

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

	m_Manager->ClearPriorities(false);

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

	SetState(IDC_CHECK_SAFE_MORAL, active);
	SetState(IDC_CHECK_MAX_INDUSTRY, active);
	SetState(IDC_CHECK_NEGLECT_FOOD, active);
}

void CManagerSettingsDlg::OnBnClickedCheckActive()
{
	UpdateData(true);
	SetStates(m_bActive);
}
