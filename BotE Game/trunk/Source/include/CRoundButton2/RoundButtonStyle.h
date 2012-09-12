/********************************************************************
	created:	2005/06/03
	created:	03:06:2005   11:23
	filename: 	x:\Software\Mfc\Source\Controls\Buttons\RoundButtonStyle.h
	file path:	x:\Software\Mfc\Source\Controls\Buttons
	file base:	RoundButtonStyle
	file ext:	h
	author:		Markus Zocholl

	purpose:	CRoundButtonStyle manages the Style of CRoundButton, a
				Button-Control with round Design.
				Because the generation of the Button-Images is time consuming
				this is only done once in the Style-Class, and all Buttons
				associated with this class take the same Images.
*********************************************************************/

#pragma once
#include "afxwin.h"

//! State of Button
static enum ButtonState {
	BS_DISABLED		= 0,	//<! Button is disabled
	BS_ENABLED		= 1,	//<! Button is enabled, but not clicked
	BS_CLICKED		= 2,	//<! Button is clicked, meaning selected as check button
	BS_PRESSED		= 3,	//<! Button is pressed with the mouse right now
	BS_HOT			= 4,	//<! Button is hot, meaning mouse cursor is over button
	BS_LAST_STATE	= 5		//<! last known State
};

//! Structure declaring Color-Scheme
struct tColorScheme {
	// Button is disabled
	COLORREF	m_tDisabled;
	// Button is enabled but not clicked
	COLORREF	m_tEnabled;
	// Button is clicked, meaning checked as check box or selected as radio button
	COLORREF	m_tClicked;
	// Button is pressed, Mouse is on button and left mouse button pressed
	COLORREF	m_tPressed;
	// Button is hot, not yet implemented
	COLORREF	m_tHot;
};

//! Structure declaring Button-Style
struct tButtonStyle {
	// Size of Anti-Aliasing-Zone
	double	m_dSizeAA;

	// Radius of Button
	double	m_dRadius;
	// ...Part of Radius is Border
	double	m_dBorderRatio;

	// Height of Border
	double	m_dHeightBorder;
	// Height of Button face
	double	m_dHeightButton;

	// Position of HighLight
	double	m_dHighLightX;
	double	m_dHighLightY;

	// Radius of HighLight
	double	m_dRadiusHighLight;

	// Power of Highlight
	double	m_dPowerHighLight;

	// Color-Scheme of Button
	tColorScheme	m_tColorBack;
	tColorScheme	m_tColorBorder;
	tColorScheme	m_tColorFace;
};

class CRoundButtonStyle
{
public:
	/************************************************************************/
	/* Construction / Destruction                                           */
	/************************************************************************/

	//! Constructor
	CRoundButtonStyle(void);
	//! Destructor
	~CRoundButtonStyle(void);

	/************************************************************************/
	/* Access to Button-Style                                               */
	/************************************************************************/

	// Get current set Button-Style
	bool GetButtonStyle(tButtonStyle* _ptButtonStyle);
	// Set Style of Button to new value
	bool SetButtonStyle(tButtonStyle* _ptButtonStyle);

	/************************************************************************/
	/* Request for graphical objects                                        */
	/************************************************************************/

	// Get Size of Edges
	CSize GetEdgeSize(void) const
	{
		return m_tBtnSize;
	}
	// Get Size of Masks
	CSize GetMaskSize(void)
	{
		return CSize(
			2 * m_tBtnSize.cx + 1,
			2 * m_tBtnSize.cy + 1);
	}

	// Get Pointer to Bitmap containing Edges of Button-Face
	CBitmap* GetButtonEdge(CDC* _pDC);

private:

	/************************************************************************/
	/* Status-variables                                                     */
	/************************************************************************/

	// Is Button already drawn?
	bool m_bButtonDrawn;
	// Size of generated Mask-Edge (use 2 * m_tBtnSize + 1 for whole mask)
	CSize	m_tBtnSize;

	/************************************************************************/
	/* Graphical objects                                                    */
	/************************************************************************/

	// Bitmap of Button-Edge
	CBitmap m_tBmpButtonEdge;
	// Bitmap of stated Buttons
	CBitmap m_tBMPButtons;

	/************************************************************************/
	/* Variables of Button-Style                                            */
	/************************************************************************/

	// Current Style of Button
	tButtonStyle m_tButtonStyle;

	/************************************************************************/
	/* Drawing-Routines                                                     */
	/************************************************************************/

	// Draw all Masks of Button
	bool DrawMasks(CDC* _pDC);
};
