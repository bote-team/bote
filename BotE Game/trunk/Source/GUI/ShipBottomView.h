/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "BottomBaseView.h"

// CShipBottomView view

struct stDrawingContext {
	stDrawingContext() : g(NULL), gp(NULL), pDoc(NULL), r(0, 0, 0, 0), fontName(""), fontBrush(NULL),
		fontFormat(), fontSize(0), normalColor(0, 0, 0)
	{
		memset(researchLevels, 1, sizeof(researchLevels));
	}
	Graphics *g;
	CGraphicPool *gp;
	CBotf2Doc* pDoc;
	CRect r;
	CString fontName;
	SolidBrush *fontBrush;
	StringFormat fontFormat;
	Gdiplus::REAL fontSize;
	Gdiplus::Color normalColor;
	BYTE researchLevels[6];
};

class CShipBottomView : public CBottomBaseView
{
	DECLARE_DYNCREATE(CShipBottomView)

protected:
	CShipBottomView();           // protected constructor used by dynamic creation
	virtual ~CShipBottomView();

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
	virtual void OnNewRound(void);

	/// Funktion lädt die rassenspezifischen Grafiken.
	virtual void LoadRaceGraphics();

	/// Funktion gibt zurück, ob in der View die Station gezeigt werden soll.
	/// Ist dies nicht der Fall, so werden Schiffe gezeigt.
	/// @return <code>TRUE</code> wenn Station gezeigt wird, bei Schiffen <code>FALSE</code>
	static BOOLEAN IsShowStation(void) {return m_bShowStation;}

	/// Funktion legt fest, ob eine Station oder Schiffe in der View angezeigt werden sollen.
	/// @param is Wahrheitswert: <code>TRUE</code> für Stationen, <code>FALSE</code> für Schiffe
	static void SetShowStation(BOOLEAN is) {m_bShowStation = is;}

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

private:
	// Funktionen
	/// Funktion ermittelt die Nummer des Schiffes im Array, über welches die Maus bewegt wurde.
	/// @param pt Mauskoordinate
	/// @return Nummer des Schiffes im Schiffsarray (<code>-1</code> wenn kein Schiff markiert wurde)
	//int GetMouseOverShip(CPoint& pt);

	// Attribute
	CRect m_RectForTheShip;						///< Das Rechteck für das Schiff worüber wir die Maus bewegt haben
	USHORT m_iPage;								///< bei mehr als 9 Schiffen können wir auf die nächste Seite wechseln
	CPoint m_LastKO;							///< Hilfsvariable, die den zuletzt angeklickten Sektor sich merkt
	BOOLEAN m_bShowNextButton;
	Bitmap* m_pShipOrderButton;					///< kleiner Button für die Schiffbefehle
	CRect m_ShipOrders[SHIP_ORDER::DECLOAK + 1];			///< die ganzen Rechtecke für die Schiffsbefehlsbuttons
	CRect m_MainShipOrders[3];					///< die 3 Hauptbuttons für die Schiffsbefehle
	short m_iTimeCounter;
	enum MAIN_BUTTON {
		MAIN_BUTTON_NONE,
		MAIN_BUTTON_TACTICS,
		MAIN_BUTTON_ORDERS,
		MAIN_BUTTON_ACTIONS
	};
	MAIN_BUTTON m_iWhichMainShipOrderButton;			///< welchen Hauptschiffsbefehlsbutton haben wir gedrückt
	static BOOLEAN m_bShowStation;

	vector<pair<CRect, CShips*> > m_vShipRects;	///< alle angezeigten Schiffe mit ihren Rechtecken zu draufklicken
	CRect m_rLastMarkedRect;					///< zuletzt markiertes Rechteck

	// OnDraw Helper Functions
	stDrawingContext m_dc;

	bool CheckDisplayShip(CShips *pShip, CSector *csec );
	void DrawShipContent();
	
	void DrawMaincommandMenu();
	short DrawTacticsMenu();
	short DrawOrdersMenu();
	short DrawActionsMenu(bool isStation);

	void DrawStationData();
	void DrawMenu();

	void DrawColonyshipOrders(short &counter);
	void DrawTransportshipOrders(short &counter);

	void DrawImage( CString resName, CRect r );
	void DrawSmallButton( CString resString, CPoint coords, int shiporder = -1 );
	void SetupDrawing();


protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


