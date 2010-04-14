/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


/// CPreferencesDlg-Dialogfeld
class CPreferencesDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreferencesDlg)

public:
	/// Standardkonstruktor
	CPreferencesDlg(CWnd* pParent = NULL);

	/// Standarddestruktor
	virtual ~CPreferencesDlg();

// Dialogfelddaten
	enum { IDD = IDD_PREFERENCESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
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
	CSliderCtrl m_ctrlTooltipDelay;
	
	// Control
	BOOL m_bShowScrollBars;
	BOOL m_bInvertMouse;
	BOOL m_bHideMenu;

	// Special (Ingame)
	CEdit m_edtRandomSeed;
	CSliderCtrl m_ctrlStarDensity;
	CSliderCtrl m_ctrlMinorDensity;

protected:
	virtual void OnOK();

public:	
	afx_msg void OnNMCustomdrawSliderDifficulty(NMHDR *pNMHDR, LRESULT *pResult);		
};
