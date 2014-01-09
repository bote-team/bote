// MultiSplitterView.h: interface for the CMultiSplitterView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTISPLITTERVIEW_H__9FCE12F5_0445_4619_89F9_54235E58F2A2__INCLUDED_)
#define AFX_MULTISPLITTERVIEW_H__9FCE12F5_0445_4619_89F9_54235E58F2A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MySplitterWnd.h"
#include <map>
using namespace std;

/*****************************************************************************
*
* Class   : CMultiSplitterView
* Author  : Dan Clark  (www.dancclark.com)
* Purpose : Can switch X views in a splitter wnd
* Date    : 2/6/04
*
* Notes   :
******************************************************************************/
class CMultiSplitterView  : public  CMySplitterWnd
{
public:
	CWnd * GetViewPtr(UINT id);
	bool SwitchView(UINT id, int paneRow, int paneCol);
	bool AddSwitchableView(UINT id , CRuntimeClass*, CCreateContext* pContext,
					const CRect & size, bool isFirstView = false, UINT altId= 0);
	CMultiSplitterView();
	~CMultiSplitterView();

	map<CWnd *, UINT> views;

};

#endif // !defined(AFX_MULTISPLITTERVIEW_H__9FCE12F5_0445_4619_89F9_54235E58F2A2__INCLUDED_)
