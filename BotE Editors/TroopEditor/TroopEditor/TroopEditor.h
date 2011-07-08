// TroopEditor.h : Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "\"stdafx.h\" vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole
#include <Time.h>


// CTroopEditorApp:
// Siehe TroopEditor.cpp für die Implementierung dieser Klasse
//

class CTroopEditorApp : public CWinApp
{
public:
	CTroopEditorApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CTroopEditorApp theApp;