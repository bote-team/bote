// CombatSimulator.h : Hauptheaderdatei für die CombatSimulator-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error "\"stdafx.h\" vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"       // Hauptsymbole


// CCombatSimulatorApp:
// Siehe CombatSimulator.cpp für die Implementierung dieser Klasse
//

class CCombatSimulatorApp : public CWinApp
{
public:
	CCombatSimulatorApp();


// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCombatSimulatorApp theApp;