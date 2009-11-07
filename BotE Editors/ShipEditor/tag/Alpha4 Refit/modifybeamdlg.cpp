// ModifyBeamDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShipEditor.h"
#include "ModifyBeamDlg.h"
#include <set>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CModifyBeamDlg 


CModifyBeamDlg::CModifyBeamDlg(CArray<CShipInfo*>* pShipInfos, int nClickedShip, int nClickedBeam, CWnd* pParent /*=NULL*/)
	: CDialog(CModifyBeamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyBeamDlg)
	ASSERT(pShipInfos);
	m_pShipInfos = pShipInfos;
	m_nClickedShip = nClickedShip;
	m_nClickedBeam = nClickedBeam;
	
	//}}AFX_DATA_INIT
}


void CModifyBeamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyBeamDlg)
	DDX_Control(pDX, IDC_BEAMNAME, m_CtrlBeamList);
	DDX_CBString(pDX, IDC_BEAMNAME, m_strBeamName);
	DDX_Text(pDX, IDC_BEAMDAMAGE, m_iBeamDamage);
	DDV_MinMaxUInt(pDX, m_iBeamDamage, 0, 64000);
	DDX_Text(pDX, IDC_BEAMNUMBER, m_iBeamNumber);
	DDV_MinMaxUInt(pDX, m_iBeamNumber, 1, 255);
	DDX_Text(pDX, IDC_BEAMTYPE, m_iBeamType);
	DDV_MinMaxUInt(pDX, m_iBeamType, 1, 255);
	DDX_Check(pDX, IDC_MODULATING, m_bModulating);
	DDX_Text(pDX, IDC_BEAMTIME, m_byBeamtime);
	DDV_MinMaxByte(pDX, m_byBeamtime, 1, 255);
	DDX_Text(pDX, IDC_BONUS, m_byBonus);
	DDX_Text(pDX, IDC_RECHARGETIME, m_byRechargeTime);
	DDV_MinMaxByte(pDX, m_byRechargeTime, 1, 255);
	DDX_Text(pDX, IDC_SHOOTNUMBER, m_byShootNumber);
	DDV_MinMaxByte(pDX, m_byShootNumber, 1, 255);
	DDX_Check(pDX, IDC_PIERCING, m_bPiercing);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyBeamDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyBeamDlg)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_BEAMNAME, &CModifyBeamDlg::OnCbnSelchangeBeamname)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CModifyBeamDlg 

BOOL CModifyBeamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Zusätzliche Initialisierung hier einfügen
	if (m_nClickedBeam != CB_ERR)
	{
		CBeamWeapons* weapon = &(m_pShipInfos->GetAt(m_nClickedShip)->GetBeamWeapons()->GetAt(m_nClickedBeam));
			
		m_iBeamDamage	= weapon->GetBeamPower();
		m_strBeamName	= weapon->GetBeamName();
		m_iBeamNumber	= weapon->GetBeamNumber();
		m_iBeamType		= weapon->GetBeamType();
		m_bModulating	= weapon->GetModulating();
		m_byBeamtime	= weapon->GetBeamLenght();
		m_byBonus		= weapon->GetBonus();
		m_byRechargeTime= weapon->GetRechargeTime();
		m_byShootNumber = weapon->GetShootNumber();
		m_bPiercing		= weapon->GetPiercing();
	}
	else
	{
		m_iBeamDamage = 10;
		m_strBeamName = _T("noname");
		m_iBeamNumber = 1;
		m_iBeamType = 1;
		m_bModulating = FALSE;
		m_byBeamtime = 10;
		m_byBonus = 0;
		m_byRechargeTime = 50;
		m_byShootNumber = 1;
		m_bPiercing = FALSE;
	}

	// Beamtypen anlegen
	std::set<CString> sBeams;
	for (int i = 0; i < m_pShipInfos->GetSize(); i++)
	{
		for (int j = 0; j < m_pShipInfos->GetAt(i)->GetBeamWeapons()->GetSize(); j++)
		{
			sBeams.insert(m_pShipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j).GetBeamName());
		}
	}
	for (std::set<CString>::const_iterator it = sBeams.begin(); it != sBeams.end(); it++)
		m_CtrlBeamList.AddString(*it);

	int idx = m_CtrlBeamList.SelectString(-1, m_strBeamName);
	if (idx == CB_ERR)
		m_CtrlBeamList.SetWindowText(m_strBeamName);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CModifyBeamDlg::OnCbnSelchangeBeamname()
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	int curSel = m_CtrlBeamList.GetCurSel();
	
	CString s;		
	m_CtrlBeamList.GetLBText(curSel, s);

	for (int i = 0; i < m_pShipInfos->GetSize(); i++)
	{
		for (int j = 0; j < m_pShipInfos->GetAt(i)->GetBeamWeapons()->GetSize(); j++)
		{
			CBeamWeapons* weapon = &(m_pShipInfos->GetAt(i)->GetBeamWeapons()->GetAt(j));
			if (weapon->GetBeamName() == s)
			{
				m_iBeamDamage	= weapon->GetBeamPower();
				m_strBeamName	= weapon->GetBeamName();
				m_bModulating	= weapon->GetModulating();
				m_byBeamtime	= weapon->GetBeamLenght();
				m_byBonus		= weapon->GetBonus();
				m_byRechargeTime= weapon->GetRechargeTime();
				m_byShootNumber = weapon->GetShootNumber();
				m_bPiercing		= weapon->GetPiercing();
				
				UpdateData(FALSE);
				return;
			}
		}
	}
}
