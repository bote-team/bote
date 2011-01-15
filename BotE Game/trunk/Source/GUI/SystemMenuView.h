/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Botf2Doc.h"
#include "MainBaseView.h"
#include "OverlayBanner.h"

// CSystemMenuView view

class CSystemMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CSystemMenuView)

protected:
	CSystemMenuView();           // protected constructor used by dynamic creation
	virtual ~CSystemMenuView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgeführt werden müssen.
	void OnNewRound(void);

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

	void SetSubMenu(BYTE menuID) {m_bySubMenu = menuID;}

	BYTE GetSubMenu() const {return m_bySubMenu;}

	static void SetMarkedBuildListEntry(short entry) {m_iClickedOn = entry;}

	static BYTE& GetResourceRouteRes() {return m_byResourceRouteRes;}

private:
	// Funktionen
	/// Funkion legt alle Buttons für die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen der Baumenüansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawBuildMenue(Graphics* g);	
	
	/// Funktion zum Zeichnen des Arbeiterzuweisungsmenues
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawWorkersMenue(Graphics* g);
	
	/// Funktion zum Zeichnen des Energiezuweisungsmenues 
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawEnergyMenue(Graphics* g);
	
	// Funktion zum zeichnen der Bauwerkeansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawBuildingsOverviewMenue(Graphics* g);
	
	/// Funktion zum zeichnen der Systemhandelsansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawSystemTradeMenue(Graphics* g);
	
	/// Funktion zeichnet die großen Buttons (Energy, Arbeiter...) unten in der Systemansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawButtonsUnderSystemView(Graphics* g);
	
	/// Funktion zeichnet die Informationen rechts in der Systemansicht (Nahrung, Industrie usw.)
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawSystemProduction(Graphics* g);

	/// Fkt. berechnet und zeichnet die Produktion eines Gebäudes (z.B. es macht 50 Nahrung) 
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawBuildingProduction(Graphics* g);
	
	/// Funktion zeichnet die Bauliste in der Baumenü- und Arbeitermenüansicht
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawBuildList(Graphics* g);

	// Attribute

	// Grafiken
	Bitmap* bg_buildmenu;			// Baumenü
	Bitmap* bg_workmenu;			// Arbeitermenü
	Bitmap* bg_energymenu;			// Energiemenü
	Bitmap* bg_overviewmenu;		// Gebäudeübersichtsmenü
	Bitmap* bg_systrademenu;		// Handelsmenü im System

	// Buttons
	CArray<CMyButton*> m_BuildMenueMainButtons;		///< die unteren Buttons in der Baumenüansicht (also Baumenü, Arbeiter...)
	CArray<CMyButton*> m_WorkerButtons;				///< die Buttons in der Arbeitermenüansicht zum Ändern der zugewiesenen Arbeiter
	CArray<CMyButton*> m_SystemTradeButtons;		///< die Buttons in der Systemmenühandelsansicht zum Ändern der Mengen im stellaren Lager

	// sonstige Variablen
	// Hier die Rechtecke zum Klicken ín der Baumenüansicht
	// Dafür eine Struktur, die das Rechteck und die RunningNumber des Gebäudes/Updates beinhaltet
	CArray<int> m_vBuildlist;
	static short m_iClickedOn;			// Hilfsvariable, die mir sagt, auf welches Gebäude ich geklickt habe
	CRect BuildingDescription;			// Rechteck, wo die Beschreibung des Gebäudes steht
	CRect BuildingInfo;					// Rechteck, wo Informationen (Kosten) zu dem Gebäude stehen
	CRect AssemblyListRect;				// Rechteck, wo die Bauliste steht
	USHORT m_iWhichSubMenu;				// in welcher Liste sind wir 0 = Gebäude 1 = Schiffe 2 = Truppen
	BYTE m_byStartList;					// ab welchem Eintrag der Bauliste werden die Aufträge angezeigt
	BYTE m_byEndList;					// bis zu welchem Eintrag in der Bauliste werden die Aufträge angezeigt

	// Hier die Variablen in der Bauwerksübersicht
	struct BuildingOverviewStruct {CRect rect; USHORT runningNumber;};
	CArray<BuildingOverviewStruct> m_BuildingOverview;	// Feld, in dem die Anzahl der Gebäude in einem System abgelegt werden
	USHORT m_iBOPage;									// aktuelle Seite in der Gebäudeübersichtsansicht

	// Hier die Variablen in der Energieübersicht
	struct ENERGYSTRUCT {CRect rect; USHORT index; BOOLEAN status;};
	CArray<ENERGYSTRUCT> m_EnergyList;		// Feld, in dem die Anzahl der Gebäude, welche Energie benötigen, abgelegt werden
	USHORT m_iELPage;						// aktuelle Seite im Energiemenü

	// Hier die Variablen in der Systemhandelsansicht
	USHORT m_iSTPage;						// aktuelle Seite für die Anzeige der Handelsrouten

	// Ab hier die Rechtecke für die ganzen Buttons
	CRect ShipyardListButton;				// Rechteck für den Werftbutton
	CRect TroopListButton;					// Rechteck für den Truppenbutton
	CRect BuildingListButton;				// Rechteck für den Gebäude/Updateliste Button
	CRect BuyButton;						// Rechteck für den Kaufenbutton
	CRect DeleteButton;						// Rechteck für den Abbruchbutton
	CRect OkayButton;						// Rechteck für den Okaybutton
	CRect CancelButton;						// Rechteck für den Cancelbutton
	CRect BuildingInfoButton;				// Rechteck für den Gebäudeinfobutton
	CRect BuildingDescriptionButton;		// Rechteck für den Gebäudebeschreibungsbutton
	CRect ChangeWorkersButton;				// Rechteck für den Button für Umschaltung zwischen "normalen" und RES-Arbeitern
	CRect Timber[5][200];					// Die Balken die die Arbeiterzuweisung anzeigen 5 Stück a max. 200 Arbeiter

	BYTE m_bySubMenu;						// Welcher Button wurde gedrückt (0 für Baumenü, 1 für Arbeiter usw.)
	BOOLEAN m_bClickedOnBuyButton;			// Wurde auf den "kaufen" Button in der Bauansicht geklickt
	BOOLEAN m_bClickedOnDeleteButton;		// Wurde auf den "Bau_abbrechen" Button geklickt in der Bauansicht
	BOOLEAN m_bClickedOnBuildingInfoButton;	// Wurde auf den "Info" Button für ein Gebäude geklickt
	BOOLEAN m_bClickedOnBuildingDescriptionButton;	// Wurde auf den "Beschreibung" Button für ein Gebäude geklickt
	// Hier die Variablen, wenn wir beim Globalen Lager sind
	USHORT m_iGlobalStoreageQuantity;		// Wieviel Ressourcen werden pro Klick verschoben
	// Hier Variablen, wenn wir eine Ressourcenroute ziehen wollen
	static BYTE m_byResourceRouteRes;		// welche Ressource soll für die Ressourcenroute gelten
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
};


