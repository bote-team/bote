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

protected:
	virtual void OnOK();

private:
	void SetState(int item, BOOL active);
	void SetStates(BOOL active);

	static const int tick_frequ = 1;


	CSystemManager* m_Manager;

	BOOL m_bActive;

	CSliderCtrl m_ctrlSecuritySlider;
	CSliderCtrl m_ctrlResearchSlider;
	CSliderCtrl m_ctrlTitanSlider;
	CSliderCtrl m_ctrlDeuteriumSlider;
	CSliderCtrl m_ctrlDuraniumSlider;
	CSliderCtrl m_ctrlCrystalSlider;
	CSliderCtrl m_ctrlIridiumSlider;

};
