/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Botf2Doc.h"

/*
Wird GDI Plus benutzt, so müssen noch weitere Anpassungen am Code vorgenommen werden,
- diese View sollte nur von CScrollView abgeleitet werden, nicht von CDBScrollView
- die MessageHandler müssen auf CScrollView angepasst werden, nicht wie bei GDI auf CDBScrollView
- OnEraseBackground muss in dieser View FALSE zurückgeben
- CMemDC darf beim Zeichnen der Galaxieansicht nicht verwendet werden
*/

class CGalaxyMenuView : public CScrollView
{
protected: // Nur aus Serialisierung erzeugen
	CGalaxyMenuView();
	DECLARE_DYNCREATE(CGalaxyMenuView)

	CSize m_TotalSize;				///< Größe der View in logischen Koordinaten
	BOOLEAN m_bScrollToHome;		///< soll zum Heimatsektor zu Beginn der Runde gescrollt werden
	
	static CMajor* m_pPlayersRace;	///< Spielerrasse
		
	Bitmap* m_pGalaxyBackground;	///< Zeiger auf Hintergrundbild-Objekt
	Bitmap* m_pThumbnail;
		
	double m_fZoom;					///< aktueller Zoom-Faktor
	int m_nRange;
	Sector oldtarget;
	Sector m_oldSelection;	
	bool m_bUpdateOnly;
	CPoint m_ptViewOrigin;			///< oberer linker Punkt der View, wenn Hintergrund zentriert wird (bei sehr weitem Rauszoom)
	CPoint m_ptOldMousePos;
	
	// Hier Variablen, wenn wir eine Handelroute ziehen wollen
	static BOOLEAN m_bDrawTradeRoute;		///< sollen wir bei MouseMove die Handelroute zeigen
	static CTradeRoute m_TradeRoute;		///< die Handelsroute, die wir auf der Galaxiemap ziehen können
	// Hier Variablen, wenn wir eine Ressourcenroute ziehen wollen
	static BOOLEAN m_bDrawResourceRoute;	///< sollen wir bei MouseMove die Ressourcenroute zeigen
	static CResourceRoute m_ResourceRoute;	///< die Ressourcenroute, die wir auf der Galaxiemap ziehen können
	// Hier Variablen für die Schiffsbewegung
	static BOOLEAN m_bShipMove;				///< soll ein Schiff auf der Map bewegt werden (Kurs geben)
	CArray<Sector> m_oldPath;				///< alter Pfad eines Schiffes, welcher auf der Map angezeigt wird
	
	/// Funktion zum Festlegen der Spielerrasse in der View
	/// @pPlayer Zeiger auf Spielerrasse
	static void SetPlayersRace(CMajor* pPlayer) {m_pPlayersRace = pPlayer;}
	
	/**
	 * Multipliziert die cx und cy-Komponente von pSize mit dem Zoom-Faktor, rundet anschließend auf ganze Werte.
	 */
	void Zoom(CSize *pSize) const;
	void Zoom(CPoint *pPoint) const;
	/**
	 * Dividiert die cx und cy-Komponente von pSize durch den Zoom-Faktor, rundet anschließend auf ganze Werte.
	 */
	void UnZoom(CSize *pSize) const;
	void UnZoom(CPoint *pPoint) const;

	int GetRangeBorder(const unsigned char range1, const unsigned char range2, int m_nRange) const;
	
	/// Funktion generiert die visuelle Galaxiekarte. Muss bei jeder neuen Runde neu aufgerugen werden.
	void GenerateGalaxyMap(void);
		
public:	
	/// Funktion führt Aufgaben aus, welche zu jeder neuen Runde von der View ausgeführt werden müssen.
	void OnNewRound();

	/// Funktion legt den visuellen Pfad für Schiffe fest.
	/// legt den Pfad des neu angewählten Schiffes auf der Map fest und löscht den alten Pfad.
	void SetNewShipPath();

	/// Funktion legt fest, ob eine anzulegende Ressourcenroute gerade gezeichnet werden soll. Dies wird zum manuellen
	/// Anlegen der Ressourcenrouten benötigt.
	/// @param is Wahrheitswert
	static void IsDrawResourceRoute(bool is) {m_bDrawResourceRoute = is;}

	/// Funktion legt fest, ob eine anzulegende Handelsroute gerade gezeichnet werden soll. Dies wird zum manuellen
	/// Anlegen der Handelsrouten benötigt.
	/// @param is Wahrheitswert
	static void IsDrawTradeRoute(bool is) {m_bDrawTradeRoute = is;}

	/// Funktion weißt der zu zeichnenden Ressourcenroute eine bestehende Route zu.
	/// @return zu zeichnende neue Route
	static CResourceRoute* GetDrawnResourceRoute() {return &m_ResourceRoute;}

	/// Funktion weißt der zu zeichnenden Ressourcenroute eine bestehende Route zu.
	/// @return zu zeichnende neue Route
	static CTradeRoute* GetDrawnTradeRoute() {return &m_TradeRoute;}

	/// Funktion legt fest, ob ein Schiff auf der Map bewegt werden soll
	/// @param is Wahrheitswert
	static void SetMoveShip(BOOLEAN is) {m_bShipMove = is;}

	/// Funktion gibt zurück, ob ein Schiff auf der Map bewegt werden soll.
	/// @return Wahrheitswert über die Schiffsbewegung auf der Map
	static BOOLEAN IsMoveShip() {return m_bShipMove;}

	///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
	/// @return	der erstellte Tooltip-Text
	virtual CString CreateTooltip(void);

	/// Funktion scrollt zur angegebenen Position in der Galaxiemap.
	/// @param pt Koordinate, zu welcher gescrollt werden soll.
	void ScrollToSector(const CPoint& pt);

	// Operationen
public:
	// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CGalaxyMenuView)
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnInitialUpdate(); // das erste mal nach der Konstruktion aufgerufen
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

	// Implementierung
public:
	virtual ~CGalaxyMenuView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CGalaxyMenuView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
public:
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
protected:
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);	

private:
	void HandleShipHotkeys(const UINT nChar, CBotf2Doc* pDoc);
	void SearchNextIdleShipAndJumpToIt(CBotf2Doc* pDoc, const int order = -1);

	 //remember the ship in shiparray which was jumped to by last press of n or spacebar
	struct RememberedShip {
		RememberedShip() : index(0), name("") {}
		RememberedShip(int _index, CString _name) : index(_index), name(_name) {}
		int index;
		CString name;

		RememberedShip operator=(const RememberedShip &other) {
			index = other.index; name = other.name; return *this;
		}
	};
	RememberedShip m_PreviouslyJumpedToShip;
};