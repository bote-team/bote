#pragma once
#include "BuildingInfo.h"

// CProductionDlg dialog

class CProductionDlg : public CDialog
{
	DECLARE_DYNAMIC(CProductionDlg)

public:
	CProductionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProductionDlg();

	// Memberfunktionen
	virtual void DialogToData(CBuildingInfo* building, int language);
	virtual void DataToDialog(CBuildingInfo* building, int language);

// Dialog Data
	enum { IDD = IDD_PRODUCTIONDLG };
	UINT	m_iCrystalProd;
	UINT	m_iDeuteriumProd;
	UINT	m_iDilithiumProd;
	UINT	m_iDuraniumProd;
	UINT	m_iEnergyProd;
	short	m_iFoodProd;
	UINT	m_iFPProd;
	UINT	m_iIPProd;
	UINT	m_iIridiumProd;
	short	m_iLatinumProd;
	UINT	m_iSPProd;
	UINT	m_iTitanProd;
	short	m_iAllResBoni;
	short	m_iCrystalBoni;
	short	m_iDeuteriumBoni;
	short	m_iDilithiumBoni;
	short	m_iDuraniumBoni;
	short	m_iEnergyBoni;
	short	m_iFoodBoni;
	short	m_iFPBoni;
	short	m_iIPBoni;
	short	m_iIridiumBoni;
	short	m_iLatinumBoni;
	short	m_iSPBoni;
	short	m_iTitanBoni;
	short	m_iBioTechBoni;
	short	m_iCompTechBoni;
	short	m_iConstructionTechBoni;
	short	m_iEnergyTechBoni;
	short	m_iPropulsionTechBoni;
	short	m_iWeaponTechBoni;
	short	m_iEconomySabotageBoni;
	short	m_iEconomySpyBoni;
	short	m_iInnerSecurityBoni;
	short	m_iMilitarySabotageBoni;
	short	m_iMilitarySpyBoni;
	short	m_iResearchSabotageBoni;
	short	m_iResearchSpyBoni;
	short	m_iMoralSys;
	short	m_iMoralEmpirewide;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
