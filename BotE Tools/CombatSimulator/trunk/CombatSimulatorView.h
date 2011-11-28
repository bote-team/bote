// CombatSimulatorView.h : Schnittstelle der Klasse CCombatSimulatorView
//

#if !defined(COMBAT_SIMULATOR_VIEW_H)
#define COMBAT_SIMULATOR_VIEW_H

#pragma once

#include <afxmt.h>
#include "EventReceiver.h"





/*	class irr::IrrlichtDevice;
	class irr::video::IVideoDriver;
	class irr::scene::ISceneManager;
	class irr::gui::IGUIEnvironment;
	*/

class CTimerAdapter : public CWinThread
{
public:
    virtual BOOL InitInstance() ;
    virtual int Run() ;
    CCombatSimulatorView* p_view ;
	bool go;
	int count;
} ;


class CCombatSimulatorView : public CView
{


protected: // Nur aus Serialisierung erstellen
	CCombatSimulatorView();
	DECLARE_DYNCREATE(CCombatSimulatorView)
	void OnInitialUpdate( );

	

// Attribute
	CRect r;
	UINT counter;
	BOOLEAN xy_ebene;
	std::map<CString, SColor> color;
	std::map<CString, ITexture*> Texture;
	std::map<CString, BYTE> winner;

public:
	CCombatSimulatorDoc* GetDocument() const;
//	void Paint3D();
	//int OnCreate2();

// Vorgänge
public:

// Überschreibungen
public:

	virtual void OnDraw(CDC* pDC);  // Überschrieben, um diese Ansicht darzustellen

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
//	void OnMouseMove(UINT nFlags, CPoint point );
	virtual BOOL PreTranslateMessage(MSG* pMsg );

// Implementierung
public:
	virtual ~CCombatSimulatorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	ISceneNode* SelectNode(position2di mouse_pos);
	CPoint m_mouse;

	IrrlichtDevice* p_device;
	video::IVideoDriver* p_driver; 
	ISceneManager* p_smgr; 
	IGUIEnvironment* p_guienv; 
	IGUIStaticText* text1;
	IGUIStaticText* text2;
	IMeshSceneNode* p_node;
	ITexture* p_Texture;
	ISceneNode* p_ShipParent; //Container für die Schiffe
	ICameraSceneNode* p_camera;
	EventReceiver* p_receiver;
	CTimerAdapter* p_timer;
	int m_nTimer;
	
	CSemaphore  m_lock;
	f32  m_iCameraZoom;
	f32  m_iCameraX;
	f32  m_iCameraY;
	bool r_button;

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual void OnInitialUpdate();
protected:
//	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);



public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
//	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};







#ifndef _DEBUG  // Debugversion in CombatSimulatorView.cpp
inline CCombatSimulatorDoc* CCombatSimulatorView::GetDocument() const
   { return reinterpret_cast<CCombatSimulatorDoc*>(m_pDocument); }
#endif

#endif


