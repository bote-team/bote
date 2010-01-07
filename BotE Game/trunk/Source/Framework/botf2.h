/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole
#include "ChatDlg.h"

#define WM_UPDATEVIEWS	(WM_APP+10)
#define WM_SHOWCHATDLG	(WM_APP+11)

/////////////////////////////////////////////////////////////////////////////
// CBotf2App:
// Siehe botf2.cpp für die Implementierung dieser Klasse
class CBotf2App : public CWinApp
{
private:
	CChatDlg *m_pChatDlg;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

public:
	/// Konstruktor
	CBotf2App();

	/// Destruktor
	virtual ~CBotf2App();

	/// Funktion gibt einen Zeiger auf das zugehörige Dokument der SDI Anwendung zurück.
	CBotf2Doc* GetDocument() const;

	/// Funktion gibt einen Zeiger auf den ChatDialog zurück.
	CChatDlg* GetChatDlg() {return m_pChatDlg;}

// Überschreibungen
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	
// Implementierung
	DECLARE_MESSAGE_MAP()	
	afx_msg void OnAppAbout();
	
private:
	afx_msg void UpdateViews(WPARAM, LPARAM);
	afx_msg void ShowChatDlg(WPARAM, LPARAM);	
	afx_msg void OnChat();	
public:
	afx_msg void OnOpenManual();
	afx_msg void OnOpenReadme();
	afx_msg void OnOpenCredits();
};