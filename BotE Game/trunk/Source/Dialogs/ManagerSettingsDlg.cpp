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
{

}

CManagerSettingsDlg::CManagerSettingsDlg(CSystemManager* manager, CWnd* pParent)
	: CDialog(CManagerSettingsDlg::IDD, pParent), m_Manager(manager),
	m_bActive(FALSE)
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
}


BEGIN_MESSAGE_MAP(CManagerSettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE, &CManagerSettingsDlg::OnBnClickedCheckActive)
END_MESSAGE_MAP()


// CManagerSettingsDlg-Meldungshandler

BOOL CManagerSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bActive = m_Manager->Active();

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


	m_Manager->ClearPriorities(false);

	m_Manager->AddPriority(m_ctrlSecuritySlider.GetPos(), WORKER::SECURITY_WORKER);
	m_Manager->AddPriority(m_ctrlResearchSlider.GetPos(), WORKER::RESEARCH_WORKER);
	m_Manager->AddPriority(m_ctrlTitanSlider.GetPos(), WORKER::TITAN_WORKER);
	m_Manager->AddPriority(m_ctrlDeuteriumSlider.GetPos(), WORKER::DEUTERIUM_WORKER);
	m_Manager->AddPriority(m_ctrlDuraniumSlider.GetPos(), WORKER::DURANIUM_WORKER);
	m_Manager->AddPriority(m_ctrlCrystalSlider.GetPos(), WORKER::CRYSTAL_WORKER);
	m_Manager->AddPriority(m_ctrlIridiumSlider.GetPos(), WORKER::IRIDIUM_WORKER);

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
}

void CManagerSettingsDlg::OnBnClickedCheckActive()
{
	UpdateData(true);
	SetStates(m_bActive);
}
