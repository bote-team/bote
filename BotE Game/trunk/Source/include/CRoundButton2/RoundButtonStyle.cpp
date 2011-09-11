/********************************************************************
	created:	2005/06/03
	created:	03:06:2005   11:27
	filename: 	x:\Software\Mfc\Source\Controls\Buttons\RoundButtonStyle.cpp
	file path:	x:\Software\Mfc\Source\Controls\Buttons
	file base:	RoundButtonStyle
	file ext:	cpp
	author:		Markus Zocholl
	
	purpose:	CRoundButtonStyle manages the Style of CRoundButton, a 
				Button-Control with round Design.
				Because the generation of the Button-Images is time consuming
				this is only done once in the Style-Class, and all Buttons 
				associated with this class take the same Images.
*********************************************************************/

#include "StdAfx.h"
#include "math.h"
#include ".\roundbuttonstyle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************/
/* Construction / Destruction                                           */
/************************************************************************/

//! Constructor
CRoundButtonStyle::CRoundButtonStyle(void)
: m_bButtonDrawn(false)
{
	// No Image => No Size
	m_tBtnSize	= CSize(0, 0);

	// Set Standard-AntiAliasing-Zone
	m_tButtonStyle.m_dSizeAA					= 2.0;

	// Set Standard-Position of HighLight
	m_tButtonStyle.m_dHighLightX				=  0.0;
	m_tButtonStyle.m_dHighLightY				= -7.0;

	// Set Radii of Edges
	m_tButtonStyle.m_dRadius					= 10.0;
	m_tButtonStyle.m_dBorderRatio				= 0.2;

	// Set Heights of Button
	m_tButtonStyle.m_dHeightBorder				= 0.5;
	m_tButtonStyle.m_dHeightButton				= 0.5;

	// Set Data of Highlight
	m_tButtonStyle.m_dRadiusHighLight			= 7.0;
	m_tButtonStyle.m_dPowerHighLight			= 0.4;

	// Set Colors for different States
	m_tButtonStyle.m_tColorBack.m_tDisabled		= GetSysColor(COLOR_3DFACE);
	m_tButtonStyle.m_tColorBorder.m_tDisabled	= RGB(128, 128, 128);
	m_tButtonStyle.m_tColorFace.m_tDisabled		= RGB(128, 128, 128);

	m_tButtonStyle.m_tColorBack.m_tEnabled		= GetSysColor(COLOR_3DFACE);
	m_tButtonStyle.m_tColorBorder.m_tEnabled	= RGB(164, 128, 128);
	m_tButtonStyle.m_tColorFace.m_tEnabled		= RGB(164, 164, 164);

	m_tButtonStyle.m_tColorBack.m_tClicked		= GetSysColor(COLOR_3DFACE);
	m_tButtonStyle.m_tColorBorder.m_tClicked	= RGB(255, 255,   0);
	m_tButtonStyle.m_tColorFace.m_tClicked		= RGB(164, 164, 164);

	m_tButtonStyle.m_tColorBack.m_tPressed		= GetSysColor(COLOR_3DFACE);
	m_tButtonStyle.m_tColorBorder.m_tPressed	= RGB(164, 128, 128);
	m_tButtonStyle.m_tColorFace.m_tPressed		= RGB( 64,  64,  64);

	m_tButtonStyle.m_tColorBack.m_tHot			= GetSysColor(COLOR_3DFACE);
	m_tButtonStyle.m_tColorBorder.m_tHot		= RGB(164, 128, 128);
	m_tButtonStyle.m_tColorFace.m_tHot			= RGB(192, 192, 192);
}

//! Destructor
CRoundButtonStyle::~CRoundButtonStyle(void)
{
}

/************************************************************************/
/* Access to Button-Style                                               */
/************************************************************************/

// Get current set Button-Style
bool CRoundButtonStyle::GetButtonStyle(tButtonStyle* _ptButtonStyle)
{
	// Check, if Pointer to a Button-Style-Struct is given
	if (_ptButtonStyle == NULL)
		return false;

	// Copy Style to given Struct
	memcpy(_ptButtonStyle, &m_tButtonStyle, sizeof(tButtonStyle));

	// All done
	return true;
}

// Set Style of Button to new value
bool CRoundButtonStyle::SetButtonStyle(tButtonStyle* _ptButtonStyle)
{
	// Check, if new Button-Style is given
	if (_ptButtonStyle == NULL)
		return false;

	// Copy new Style to Construct
	memcpy(&m_tButtonStyle, _ptButtonStyle, sizeof(tButtonStyle));

	// Set Flag to redraw Buttons
	m_bButtonDrawn = false;

	// All done
	return true;
}

/************************************************************************/
/* Request for graphical objects                                        */
/************************************************************************/

// Get Pointer to Bitmap containing Edges of Button-Face
CBitmap* CRoundButtonStyle::GetButtonEdge(CDC* _pDC)
{
	// Check, if Button needs to be redrawn
	if (!m_bButtonDrawn)
	{
		// Draw Masks of Button
		DrawMasks(_pDC);

		m_bButtonDrawn = true;
	}

	// Return Pointer to Bitmap
	return &m_tBmpButtonEdge;
}

/************************************************************************/
/* Drawing-Routines                                                     */
/************************************************************************/

// Draw all Masks of Button
bool CRoundButtonStyle::DrawMasks(CDC* _pDC)
{
	CDC MemDC;

	// Create DC in Memory
	if (MemDC.CreateCompatibleDC(_pDC) == FALSE)
		return false;

	/************************************************************************/
	/* Generate Variables                                                   */
	/************************************************************************/

	// Distance from Center of Button
	double		fDistCenter	= 0.0;
	// Distance from Highlight-Center
	double		fDistHigh	= 0.0;
	// X-Position of Highlight
	double		fXHigh;
	// Y-Position of Highlight
	double		fYHigh;
	// Color-Factor of Background-Color
	double		fFacBack	= 0.0;
	// Color-Factor of Border-Color
	double		fFacBorder	= 0.0;
	// Color-Factor of Button-Face-Color
	double		fFacFace	= 0.0;
	// Color-Factor of Highlight-Color
	double		fFacHigh	= 0.0;
	// Color-Factor Red
	double		fFacR;
	// Color-Factor Green
	double		fFacG;
	// Color-Factor Blue
	double		fFacB;
	// Color of actual Pixel
	COLORREF	tColPixel;
	// Size of Anti-Aliasing-Region
	double		fSizeAA;
	// Radius of Outer Rim (between Border and Nirvana)
	double		fRadOuter;
	// Radius of Inner Rim (between Button-Face and Border)
	double		fRadInner;
	// Ratio of Border
	double		fRatioBorder;
	// Height of Border
	double		fHeightBorder;
	// Height of Button-Face
	double		fHeightButton;
	// Radius of Highlight
	double		fRadHigh;
	// Power of Highlight
	double		fPowHigh;
	// Size of single Edge
	int			nSizeEdge = 0;

	/************************************************************************/
	/* Load Infos of Style                                                  */
	/************************************************************************/

	// Load Position of HighLight
	fSizeAA	= m_tButtonStyle.m_dSizeAA;

	fXHigh	= m_tButtonStyle.m_dHighLightX;
	fYHigh	= m_tButtonStyle.m_dHighLightY;

	fRadOuter		= m_tButtonStyle.m_dRadius;
	fRatioBorder	= m_tButtonStyle.m_dBorderRatio;

	fHeightBorder	= m_tButtonStyle.m_dHeightBorder;
	fHeightButton	= m_tButtonStyle.m_dHeightButton;

	fRadHigh		= m_tButtonStyle.m_dRadiusHighLight;
	fPowHigh		= m_tButtonStyle.m_dPowerHighLight;

	// Calculate Radius of Inner Border
	fRadInner = __min(fRadOuter, __max(0.0f, fRadOuter * (1.0f - fRatioBorder)));

	// Calculate Size of an Edge
	nSizeEdge = (int)ceil(fRadOuter + fSizeAA / 2.0);

	// Store Size of Mask in global var
	m_tBtnSize.SetSize(nSizeEdge, nSizeEdge);

	// Delete old Bitmap, if present
	if (m_tBmpButtonEdge.m_hObject != NULL)
		m_tBmpButtonEdge.DeleteObject();

	// Generate new Bitmap
	m_tBmpButtonEdge.CreateCompatibleBitmap(
		_pDC, 
		2 * nSizeEdge + 1, 
		(2 * nSizeEdge + 1) * BS_LAST_STATE);

	// Select Bitmap of Button-Edge into DC
	HGDIOBJ hOldBmp = MemDC.SelectObject(m_tBmpButtonEdge);

	// Draw Button-Edge
	int nX;
	int nY;
	int nState;

	COLORREF	tColorBack;
	COLORREF	tColorBorder;
	COLORREF	tColorFace;
	
	for (nX = -nSizeEdge; nX <= nSizeEdge; nX++)
	{
		for (nY = -nSizeEdge; nY <= nSizeEdge; nY++)
		{
			// Calculate Distance of Point from Center of Button
			fDistCenter	= sqrt((double)nX * (double)nX + (double)nY * (double)nY);

			// Calculate factor of Background
			fFacBack	= __max(0.0, __min(1.0, 0.5 + (fDistCenter - fRadOuter) * 2.0 / fSizeAA));

			// Calculate Factor for Border
			fFacBorder	= 1.0 - fHeightBorder * pow((fRadOuter + fRadInner - fDistCenter * 2.0) / (fRadOuter - fRadInner) ,2);
			fFacBorder	= __max(0.0, __min(1.0, 0.5 - (fDistCenter - fRadOuter) * 2.0 / fSizeAA)) * fFacBorder;
			fFacBorder	= __max(0.0, __min(1.0, 0.5 + (fDistCenter - fRadInner) * 2.0 / fSizeAA)) * fFacBorder;

			for (nState = 0; nState < BS_LAST_STATE; nState++)
			{
				// Get Colors of State
				switch(nState)
				{
				case BS_ENABLED:
					tColorBack		= m_tButtonStyle.m_tColorBack.m_tEnabled;
					tColorBorder	= m_tButtonStyle.m_tColorBorder.m_tEnabled;
					tColorFace		= m_tButtonStyle.m_tColorFace.m_tEnabled;
					break;
				case BS_CLICKED:
					tColorBack		= m_tButtonStyle.m_tColorBack.m_tClicked;
					tColorBorder	= m_tButtonStyle.m_tColorBorder.m_tClicked;
					tColorFace		= m_tButtonStyle.m_tColorFace.m_tClicked;
					break;
				case BS_PRESSED:
					tColorBack		= m_tButtonStyle.m_tColorBack.m_tPressed;
					tColorBorder	= m_tButtonStyle.m_tColorBorder.m_tPressed;
					tColorFace		= m_tButtonStyle.m_tColorFace.m_tPressed;
					break;
				case BS_HOT:
					tColorBack		= m_tButtonStyle.m_tColorBack.m_tHot;
					tColorBorder	= m_tButtonStyle.m_tColorBorder.m_tHot;
					tColorFace		= m_tButtonStyle.m_tColorFace.m_tHot;
					break;
				case BS_DISABLED:
				default:
					tColorBack		= m_tButtonStyle.m_tColorBack.m_tDisabled;
					tColorBorder	= m_tButtonStyle.m_tColorBorder.m_tDisabled;
					tColorFace		= m_tButtonStyle.m_tColorFace.m_tDisabled;
					break;
				}

				// Calculate Distance of Point from Highlight of Button
				fDistHigh	= sqrt(((double)nX - fXHigh) * ((double)nX - fXHigh) + ((double)nY - fYHigh) * ((double)nY - fYHigh));

				// Calculate Factor of Inner Surface
				if (fHeightButton > 0)
					fFacFace	= 1.0 - fHeightButton * (fDistCenter / fRadInner) * (fDistCenter / fRadInner);
				else
					fFacFace	= 1.0 + fHeightButton - fHeightButton * (fDistCenter / fRadInner) * (fDistCenter / fRadInner);
				fFacFace	= __max(0.0, __min(1.0, 0.5 - (fDistCenter - fRadInner) * 2.0 / fSizeAA)) * fFacFace;

				// Calculate Factor of Highlight
				fFacHigh	= 1.0 + __max(-1.0, __min(1.0, 1.0 - fHeightButton * fDistHigh / fRadHigh)) * fPowHigh;
				fFacFace = fFacFace * fFacHigh;

				// Calculate Color-Factors
				fFacR = 
					(float)GetRValue(tColorBack)	* fFacBack +
					(float)GetRValue(tColorBorder)	* fFacBorder +
					(float)GetRValue(tColorFace)	* fFacFace;
				fFacG = 
					(float)GetGValue(tColorBack)	* fFacBack +
					(float)GetGValue(tColorBorder)	* fFacBorder +
					(float)GetGValue(tColorFace)	* fFacFace;
				fFacB = 
					(float)GetBValue(tColorBack)	* fFacBack +
					(float)GetBValue(tColorBorder)	* fFacBorder +
					(float)GetBValue(tColorFace)	* fFacFace;

				// Calculate actual Color of Pixel
				tColPixel = RGB(
					__max(0, __min(255, (int)fFacR)),
					__max(0, __min(255, (int)fFacG)),
					__max(0, __min(255, (int)fFacB))
					);

				// Draw Pixels
				MemDC.SetPixel(nSizeEdge + nX, nSizeEdge + nY + (2 * nSizeEdge + 1) * nState, tColPixel);
			}
		}
	}

	// Select Old Bitmap into DC
	MemDC.SelectObject(hOldBmp);

	return true;
}
