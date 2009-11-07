// ModifyTubeDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShipEditor.h"
#include "ModifyTubeDlg.h"
#include "TorpedoInfo.h"
#include <set>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CModifyTubeDlg 


CModifyTubeDlg::CModifyTubeDlg(CArray<CShipInfo*>* pShipInfos, int nClickedShip, int nClickedTube, CWnd* pParent /*=NULL*/)
	: CDialog(CModifyTubeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyTubeDlg)
	ASSERT(pShipInfos);
	m_pShipInfos = pShipInfos;
	m_nClickedShip = nClickedShip;
	m_nClickedTube = nClickedTube;

	m_bPenetrating = FALSE;
	m_bCollapseShields = FALSE;
	m_bDoubleHullDmg = FALSE;
	m_bDoubleShieldDmg = FALSE;
	m_bIgnoreAllShields = FALSE;
	m_bReduceManeuver = FALSE;
	m_bMicro = FALSE;
	//}}AFX_DATA_INIT
}


void CModifyTubeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyTubeDlg)
	DDX_Control(pDX, IDC_TUBETYPE, m_TubeList);
	DDX_Control(pDX, IDC_TORPEDOTYPE, m_TorpedoTypeList);
	DDX_Text(pDX, IDC_NUMBEROFTORPEDOS, m_iTorpedoNumber);
	DDV_MinMaxUInt(pDX, m_iTorpedoNumber, 1, 255);
	DDX_Text(pDX, IDC_NUMBEROFTUBES, m_iTubeNumber);
	DDV_MinMaxUInt(pDX, m_iTubeNumber, 1, 255);
	DDX_Text(pDX, IDC_TUBEFIRERATE, m_iTubeFirerate);
	DDV_MinMaxUInt(pDX, m_iTubeFirerate, 1, 255);
	DDX_CBString(pDX, IDC_TUBETYPE, m_strTubeName);
	DDX_Check(pDX, IDC_ONLYMICRO, m_bOnlyMicroPhoton);
	DDX_Text(pDX, IDC_ACCURACY, m_byAccuracy);
	DDV_MinMaxByte(pDX, m_byAccuracy, 0, 100);
	DDX_Check(pDX, IDC_PENETRATING, m_bPenetrating);
	DDX_Check(pDX, IDC_COLLAPSESHIELDS, m_bCollapseShields);
	DDX_Check(pDX, IDC_DOUBLEHULLDMG, m_bDoubleHullDmg);
	DDX_Check(pDX, IDC_DOUBLESHIELDDMG, m_bDoubleShieldDmg);
	DDX_Check(pDX, IDC_IGNOREALLSHIELDS, m_bIgnoreAllShields);
	DDX_Check(pDX, IDC_REDUCEMANEUVER, m_bReduceManeuver);
	DDX_Check(pDX, IDC_MICROTORP, m_bMicro);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyTubeDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyTubeDlg)
	ON_LBN_SELCHANGE(IDC_TORPEDOTYPE, OnSelchangeTorpedotype)
	ON_CBN_SELCHANGE(IDC_TUBETYPE, OnSelchangeTubetype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CModifyTubeDlg 

BOOL CModifyTubeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Zusätzliche Initialisierung hier einfügen
	
	if (m_nClickedTube != CB_ERR)
	{
		CTorpedoWeapons* weapon = &(m_pShipInfos->GetAt(m_nClickedShip)->GetTorpedoWeapons()->GetAt(m_nClickedTube));
		m_strTubeName		= weapon->GetTupeName();
		m_iTorpedoType		= weapon->GetTorpedoType();
		m_iTorpedoNumber	= weapon->GetNumber();
		m_iTubeNumber		= weapon->GetNumberOfTupes();
		m_iTubeFirerate		= weapon->GetTupeFirerate();
		m_bOnlyMicroPhoton	= weapon->GetOnlyMicroPhoton();
		m_byAccuracy		= weapon->GetAccuracy();
	}
	else
	{
		m_strTubeName		= _T("noname");
		m_iTorpedoType		= 0;
		m_iTorpedoNumber	= 1;
		m_iTubeNumber		= 1;
		m_iTubeFirerate		= 50;		
		m_bOnlyMicroPhoton	= FALSE;
		m_byAccuracy		= 50;
	}

	for (int i = 0; i < DIFFERENT_TORPEDOS; i++)
		m_TorpedoTypeList.AddString(CTorpedoInfo::GetName(i));
	m_TorpedoTypeList.SetCurSel(m_iTorpedoType);

	// Werfer anlegen
	std::set<CString> sTubes;
	for (int i = 0; i < m_pShipInfos->GetSize(); i++)
	{
		for (int j = 0; j < m_pShipInfos->GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
		{
			sTubes.insert(m_pShipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTupeName());
		}
	}
	for (std::set<CString>::const_iterator it = sTubes.begin(); it != sTubes.end(); it++)
		m_TubeList.AddString(*it);

	int idx = m_TubeList.SelectString(-1, m_strTubeName);
	if (idx == CB_ERR)
		m_TubeList.SetWindowText(m_strTubeName);

	OnSelchangeTorpedotype();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CModifyTubeDlg::OnSelchangeTorpedotype() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	m_iTorpedoType = m_TorpedoTypeList.GetCurSel();

	m_bPenetrating = CTorpedoInfo::GetPenetrating(m_iTorpedoType);
	m_bDoubleHullDmg = CTorpedoInfo::GetDoubleHullDmg(m_iTorpedoType);
	m_bDoubleShieldDmg = CTorpedoInfo::GetDoubleShieldDmg(m_iTorpedoType);
	m_bCollapseShields = CTorpedoInfo::GetCollapseShields(m_iTorpedoType);
	m_bIgnoreAllShields = CTorpedoInfo::GetIgnoreAllShields(m_iTorpedoType);
	m_bReduceManeuver = CTorpedoInfo::GetReduceManeuver(m_iTorpedoType);
	m_bMicro = CTorpedoInfo::GetMicro(m_iTorpedoType);

	if (m_bOnlyMicroPhoton)
	{
		if (!m_bMicro)
			m_bOnlyMicroPhoton = false;
	}

	UpdateData(FALSE);
}


void CModifyTubeDlg::OnSelchangeTubetype() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	// Wenn ein voreingestellter Werfer ausgewählt wurde, so werden auch gleich die voreingestellten
	// Parameter für die Tube ausgewählt
	int curSel = m_TubeList.GetCurSel();
	
	CString s;		
	m_TubeList.GetLBText(curSel, s);

	for (int i = 0; i < m_pShipInfos->GetSize(); i++)
	{
		for (int j = 0; j < m_pShipInfos->GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
		{
			CTorpedoWeapons* weapon = &(m_pShipInfos->GetAt(i)->GetTorpedoWeapons()->GetAt(j));
			if (weapon->GetTupeName() == s)
			{
				m_strTubeName		= weapon->GetTupeName();
				m_iTubeFirerate		= weapon->GetTupeFirerate();
				m_iTorpedoNumber	= weapon->GetNumber();
				m_bOnlyMicroPhoton	= weapon->GetOnlyMicroPhoton();
				m_byAccuracy		= weapon->GetAccuracy();

				UpdateData(FALSE);
				return;
			}
		}
	}
}
