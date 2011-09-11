/********************************************************************
	created:	2005/06/03
	created:	3:6:2005   13:21
	filename: 	x:\Software\Mfc\Source\Controls\Buttons\RoundButton2.h
	file path:	x:\Software\Mfc\Source\Controls\Buttons
	file base:	RoundButton2
	file ext:	h
	author:		Markus Zocholl
	
	purpose:	CRoundButton2 defines a universal Button-Control with the 
				following features:

				* Shape is a rounded Rectangle
				* Button includes Border and Button-Face
				* Many parameters to get an individual look
				* Functions of Button to be en- or disabled:
					- Button (disabled means a static control with userdefined styles)
					- Hover
*********************************************************************/

#pragma once
#include "afxwin.h"
#include "RoundButtonStyle.h"

class CRoundButton2 :
	public CButton
{
public:
	/************************************************************************/
	/* Con- / Destruction                                                   */
	/************************************************************************/
	//! Constructor
	CRoundButton2(void);
	//! Destructor
	~CRoundButton2(void);

	/************************************************************************/
	/* Functions for Design of Button                                       */
	/************************************************************************/
	//! Set Style of Button
	bool SetRoundButtonStyle(CRoundButtonStyle* _ptRoundButtonStyle);
	//! Get Font of Button
	bool GetFont(LOGFONT* _ptLogFont);
	//! Set Font of Button
	bool SetFont(LOGFONT* _ptLogFont);
	//! Get Color of Caption
	bool GetTextColor(tColorScheme* _ptTextColor);
	//! Set Color of Caption
	bool SetTextColor(tColorScheme*	_ptTextColor);

	/************************************************************************/
	/* Access to Functions of Button                                        */
	/************************************************************************/
	//! Button is Check button
	void SetCheckButton(bool _bCheckButton) { m_bIsCheckButton = _bCheckButton; };
	//! Is Button a Check button
	bool GetCheckButton() { return m_bIsCheckButton; };
	//! Button is Radio button
	void SetRadioButton(bool _bRadioButton) { m_bIsRadioButton = _bRadioButton; };
	//! Is Button a Radio button
	bool GetRadioButton() { return m_bIsRadioButton; };
	//! Button is Hot-button
	void SetHotButton(bool _bHotButton) { m_bIsHotButton = _bHotButton; };
	//! Is Button a Hot-button
	bool GetHotButton() { return m_bIsHotButton; };

	//! Change Check-Status of Button
	void SetCheck(bool _bIsChecked) { m_bIsChecked = _bIsChecked; Invalidate(); };
	//! Get Current Check-Status of Button
	bool GetCheck() { return m_bIsChecked; };

	/************************************************************************/
	/* Message-Map of Control                                               */
	/************************************************************************/
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

protected:

	/************************************************************************/
	/* Own Drawing-Functions                                                */
	/************************************************************************/

	//! Generate Bitmaps to hold Buttons
	void GenButtonBMPs(CDC* _pDC, CRect _rRect);
	//! Draw Button-Face
	void DrawButtonFace(CDC* _pDC);
	//! Draw Caption on Button
	void DrawButtonCaption(CDC *_pDC);

	/************************************************************************/
	/* Overwritten Functions for Init and Draw of Button                    */
	/************************************************************************/

	//! PreSubclass-Function
	virtual void PreSubclassWindow();
	//! Draw Item-Function
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

private:
	//! Size of Button-Images
	CRect	m_rBtnSize;
	//! Image of Buttons
	CBitmap	m_tBmpBtn;

	//! Font for Caption
	CFont	m_tBtnFont;
	//! Data-Block for Font
	LOGFONT	m_tLogFont;
	//! Color Scheme of Caption
	tColorScheme	m_tTextColor;

	//! Stored Old Caption to recognize the need for a redraw
	CString m_sOldCaption;

	//! Is Button Default-Button
	bool	m_bDefaultButton;
	//! Is Check-Button
	bool	m_bIsCheckButton;
	//! Is Radio-Button
	bool	m_bIsRadioButton;
	//! Is Hot-Button
	bool m_bIsHotButton;
	//! Is Checked
	bool	m_bIsChecked;

	//! The Mouse is on the Button-Area, needed for Hot-Button
	bool m_bMouseOnButton;

	//! Button should be redrawn
	bool	m_bRedraw;

	//! Structure containing Style of Button
	CRoundButtonStyle* m_ptRoundButtonStyle;
public:
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};