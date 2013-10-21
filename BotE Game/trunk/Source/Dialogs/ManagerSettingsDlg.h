#pragma once
#include "afxcmn.h"


// CManagerSettingsDlg-Dialogfeld

class CSystemManager;

class CManagerSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CManagerSettingsDlg)

public:
	CManagerSettingsDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	CManagerSettingsDlg(CSystemManager* manager, CWnd* pParent = NULL);
	virtual ~CManagerSettingsDlg();

// Dialogfelddaten
	enum { IDD = IDD_MANAGER_SETTINGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedCheckActive();

	afx_msg void OnNMCustomdrawSliderProduction(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderSecurity(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderResearch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderTitan(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderDeuterium(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderDuranium(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderCrystal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderIridium(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual void OnOK();

private:
	void SetState(int item, BOOL active);
	void SetStates(BOOL active);
	void SetDlgItem(int item, const CString& text);
	void SetDisplayedPrio(int item, int prio);

	static const int tick_frequ = 5;


	CSystemManager* m_Manager;

	BOOL m_bActive;
	BOOL m_bSafeMoral;
	BOOL m_bMaxIndustry;
	BOOL m_bNeglectFood;

	CSliderCtrl m_ctrlSecuritySlider;
	CSliderCtrl m_ctrlResearchSlider;
	CSliderCtrl m_ctrlTitanSlider;
	CSliderCtrl m_ctrlDeuteriumSlider;
	CSliderCtrl m_ctrlDuraniumSlider;
	CSliderCtrl m_ctrlCrystalSlider;
	CSliderCtrl m_ctrlIridiumSlider;
	CSliderCtrl m_ctrlProductionSlider;

};
