// MultiSplitterView.cpp: implementation of the CMultiSplitterView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MultiSplitterView.h"
#include "AssertBotE.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiSplitterView::CMultiSplitterView()
{
	// Hier wird die breite der Trennlinien festgesetzt. Diese  Variblen sind Attribute der Klasse
	// CSplitterWnd
	m_cxSplitter = 0;     // = 1
	m_cySplitter = 0;     // = 1
	m_cxBorderShare = 0;  // = 0
	m_cyBorderShare = 0;  // = 0
	m_cxSplitterGap = 2;  // = 1
	m_cySplitterGap = 2;  // = 1
}

CMultiSplitterView::~CMultiSplitterView()
{

}

/******************************************************************************
*
* Function	: AddSwitchableView()
* Author	: Dan Clark (www.dancclark.com)
* Date		: 2/6/2003
*
* Description:  Adds a view
*
*******************************************************************************/
bool CMultiSplitterView::AddSwitchableView(UINT id, CRuntimeClass * pView,
								 CCreateContext* pContext,
								 const CRect & size, bool isFirstView, UINT altId)
{
   CWnd* pWin;
   DWORD style;

   pWin  = (CWnd*) pView->CreateObject();
   style = WS_CHILD;

   if (isFirstView)
   {
	   style |=  WS_VISIBLE ;
   }
   pWin->Create(NULL, NULL, style, size , this, id, pContext);

   if (isFirstView) // id provided is usally diff. so use alternate
   {
	   views[pWin] =altId ;
   }
   else
   {
	   views[pWin] = id;
   }

   return true;
}

/******************************************************************************
*
* Function	: SwitchView()
* Author	: Dan Clark (www.dancclark.com)
* Date		: 2/6/2003
*
* Description:  Hides one view and shows another
*
*******************************************************************************/
bool CMultiSplitterView::SwitchView(UINT id, int paneRow, int paneCol)
{
	// Gibt es die ID der View nicht, so wird auch nichts ausgetauscht.
	if (this->GetViewPtr(id) == NULL)
		return false;

   CView* pOldView = (CView*) GetPane(paneRow, paneCol); // get current view

   if (pOldView == NULL) // serious prob
	{
#ifdef SHOW_MB
	   MessageBox("Invalid OLD view ptrs !! Cant switch views ");
#endif
	   return false;
	}

   CView* pNewView = (CView*) GetDlgItem(id); // get new view

   if(pNewView == NULL ) // bad view id or this is already the view we requested
   {
#ifdef SHOW_MB
	   MessageBox("Invalid view ptrs !! Cant switch views ")
#endif
      return false;
   }

   CFrameWnd * mainWnd = (CFrameWnd *)AfxGetMainWnd();

   if (mainWnd == NULL) // serious prob
   {
	   AssertBotE(false);
	   return false;
   }

   if(mainWnd->GetActiveView() == pOldView)
		mainWnd->SetActiveView(pNewView);

   pNewView->ShowWindow(SW_SHOW);
   pOldView->ShowWindow(SW_HIDE);

   pNewView->SetDlgCtrlID(  IdFromRowCol(paneRow, paneCol));

   CWnd * bCwnd =(CWnd *)pOldView; // upcast to CWnd ptr

  if (views.find(bCwnd) == views.end()) // search for CWnd ptr
  {
#ifdef SHOW_MB
	MessageBox("Failed to find old view ptr !!!");
#endif
	return false;
  }

   UINT oldId = views[bCwnd]; // get id of this view for future lookup

   pOldView->SetDlgCtrlID(oldId); // reset view id, so we can look it up

   RecalcLayout();

   pOldView->Invalidate();
   pNewView->Invalidate();

   return true;
}

/******************************************************************************
*
* Function	: GetViewPtr()
* Author	: Dan Clark (www.dancclark.com)
* Date		: 2/6/2003
*
* Description:  Gets a base class CWnd ptr
*
*******************************************************************************/
CWnd * CMultiSplitterView::GetViewPtr(UINT id)
{
	map<CWnd *, UINT>::iterator It, Iend = views.end();

	for (It = views.begin(); It != Iend; ++It)
	{
		if ((*It).second == id)
		{
			return (*It).first;
		}
	}

	return NULL;
}
