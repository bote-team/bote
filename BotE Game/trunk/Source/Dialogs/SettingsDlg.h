/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


/// CSettingsDlg-Dialogfeld
class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	/// Standardkonstruktor
	CSettingsDlg(bool bDisableNonWorking = false, CWnd* pParent = NULL);

	/// Standarddestruktor
	virtual ~CSettingsDlg();

// Dialogfelddaten
	enum { IDD = IDD_SETTINGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	bool m_bDisable;

	// General
	CSliderCtrl m_ctrlDifficultySlider;	///< Schiebregler zum Einstellen des Schwierigkeitsgrades
	CString m_sDifficulty;				///< Anzeige des aktuell eingestellten Schwierigkeitsgrades
	BOOL m_bAutoave;					///< Autosave aktivieren/deaktivieren

	// Audio
	BOOL m_bHardwaresound;
	BOOL m_bSound;
	BOOL m_bMusic;
	CSliderCtrl m_ctrlMusicvolume;

	// Video
	BOOL m_bShowTraderoutes;
	BOOL m_bAnimatedIcon;
	BOOL m_bShowMiniMap;
	CSliderCtrl m_ctrlTooltipDelay;

	// Control
	BOOL m_bShowScrollBars;
	BOOL m_bInvertMouse;
	BOOL m_bHideMenu;

	// Special (Ingame)
	CEdit m_edtRandomSeed;
	CSliderCtrl m_ctrlStarDensity;
	CSliderCtrl m_ctrlMinorDensity;
	CSliderCtrl m_ctrlAnomalyDensity;
	CComboBox m_comboGalaxysize;
	CComboBox m_comboGalaxyshape;

	// Victory Conditions
	BOOL m_bVCElimination;
	BOOL m_bVCDiplomacy;
	BOOL m_bVCConquest;
	BOOL m_bVCResearch;
	BOOL m_bVCCombat;
	BOOL m_bVCSabotage;

protected:
	virtual void OnOK();

public:
	afx_msg void OnNMCustomdrawSliderDifficulty(NMHDR *pNMHDR, LRESULT *pResult);
};
