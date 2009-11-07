// ShowStatsDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShipEditor.h"
#include "ShowStatsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CShowStatsDlg 


CShowStatsDlg::CShowStatsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowStatsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowStatsDlg)
	m_iOverallBeamDamage = 0;
	m_iAverageTorpedoDamage = 0;
	m_iMaxHull = 0;
	m_iMaxShield = 0;
	m_iShieldRecharge = 0;
	m_iCrystal = 0;
	m_iDeuterium = 0;
	m_iDilithium = 0;
	m_iDuranium = 0;
	m_iIndustry = 0;
	m_iIridium = 0;
	m_iTitan = 0;
	//}}AFX_DATA_INIT
}


void CShowStatsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowStatsDlg)
	DDX_Text(pDX, IDC_OVERALLBEAMDAMAGE, m_iOverallBeamDamage);
	DDX_Text(pDX, IDC_AVERAGETORPEDODAMAGE, m_iAverageTorpedoDamage);
	DDX_Text(pDX, IDC_MAXHULL, m_iMaxHull);
	DDX_Text(pDX, IDC_MAXSHIELD, m_iMaxShield);
	DDX_Text(pDX, IDC_SHIELDRECHARGE, m_iShieldRecharge);
	DDX_Text(pDX, IDC_CRYSTAL, m_iCrystal);
	DDX_Text(pDX, IDC_DEUTERIUM, m_iDeuterium);
	DDX_Text(pDX, IDC_DILITHIUM, m_iDilithium);
	DDX_Text(pDX, IDC_DURANIUM, m_iDuranium);
	DDX_Text(pDX, IDC_INDUSTRY, m_iIndustry);
	DDX_Text(pDX, IDC_IRIDIUM, m_iIridium);
	DDX_Text(pDX, IDC_TITAN, m_iTitan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowStatsDlg, CDialog)
	//{{AFX_MSG_MAP(CShowStatsDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CShowStatsDlg 

void CShowStatsDlg::SetShowStatsShip(CShipInfo* shipinfo)
{
	m_iOverallBeamDamage = 0;
	m_iAverageTorpedoDamage = 0;
	m_iMaxHull = 0;
	m_iMaxShield = 0;
	m_iShieldRecharge = 0;
	m_iIndustry = 0;
	m_iTitan = 0;
	m_iDeuterium = 0;
	m_iDuranium = 0;
	m_iCrystal = 0;
	m_iIridium = 0;
	m_iDilithium = 0;
	shipinfo->CalculateFinalCosts();
	for (int i = 0; i < shipinfo->GetBeamWeapons()->GetSize(); i++)
	{
		short counter = 0;
		for (int j = 0; j < 100; j++)
		{
			if (counter == 0)
				counter = shipinfo->GetBeamWeapons()->GetAt(i).GetBeamLenght() 
							+ shipinfo->GetBeamWeapons()->GetAt(i).GetRechargeTime();
			if (counter > shipinfo->GetBeamWeapons()->GetAt(i).GetRechargeTime())
				m_iOverallBeamDamage += (UINT)shipinfo->GetBeamWeapons()->GetAt(i).GetBeamPower()
										* shipinfo->GetBeamWeapons()->GetAt(i).GetBeamNumber()
										* shipinfo->GetBeamWeapons()->GetAt(i).GetShootNumber();
			counter--;			
		}
	/*	
		m_iOverallBeamDamage += (UINT)shipinfo->GetBeamWeapons()->GetAt(i).GetBeamPower() * 
			shipinfo->GetBeamWeapons()->GetAt(i).GetBeamNumber() *
			shipinfo->GetBeamWeapons()->GetAt(i).GetShootNumber() * 
			shipinfo->GetBeamWeapons()->GetAt(i).GetBeamLenght() * 100 / 
			shipinfo->GetBeamWeapons()->GetAt(i).GetRechargeTime();
	*/
	}
	for (int i = 0; i < shipinfo->GetTorpedoWeapons()->GetSize(); i++)
		m_iAverageTorpedoDamage += (UINT)(shipinfo->GetTorpedoWeapons()->GetAt(i).GetTorpedoPower() *
									shipinfo->GetTorpedoWeapons()->GetAt(i).GetNumber() * 100 *
									shipinfo->GetTorpedoWeapons()->GetAt(i).GetNumberOfTupes() /
									shipinfo->GetTorpedoWeapons()->GetAt(i).GetTupeFirerate());
	
	m_iMaxHull = shipinfo->GetHull()->GetMaxHull();
	m_iMaxShield = shipinfo->GetShield()->GetMaxShield();
	m_iShieldRecharge = (shipinfo->GetShield()->GetMaxShield() / 300 + 2 * shipinfo->GetShield()->GetShieldType()) * 100;
	m_iIndustry = shipinfo->GetNeededIndustry();
	m_iTitan = shipinfo->GetNeededTitan();
	m_iDeuterium = shipinfo->GetNeededDeuterium();
	m_iDuranium = shipinfo->GetNeededDuranium();
	m_iCrystal = shipinfo->GetNeededCrystal();
	m_iIridium = shipinfo->GetNeededIridium();
	m_iDilithium = shipinfo->GetNeededDilithium();
}
