/********************************************************************
	created:	2005/06/03
	created:	3:6:2005   13:22
	filename: 	x:\Software\Mfc\Source\Controls\Buttons\RoundButton2.cpp
	file path:	x:\Software\Mfc\Source\Controls\Buttons
	file base:	RoundButton2
	file ext:	cpp
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

#include "StdAfx.h"
#include <math.h>
#include ".\RoundButton2.h"

#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************/
/* Construction and Destruction                                         */
/************************************************************************/

//! Construction
CRoundButton2::CRoundButton2(void):
	  m_bDefaultButton(false)
	, m_bIsCheckButton(false)
	, m_bIsRadioButton(false)
	, m_bIsHotButton(false)
	, m_bMouseOnButton(false)
	, m_bIsChecked(false)
	, m_ptRoundButtonStyle(NULL)
	, m_rBtnSize(CRect(0, 0, 0, 0))
	, m_bRedraw(false)
	, m_sOldCaption(_T(""))
	  {
	// Set Standards in Font-Style
	m_tLogFont.lfHeight			= 16;
	m_tLogFont.lfWidth			= 0;
	m_tLogFont.lfEscapement		= 0;
	m_tLogFont.lfOrientation	= 0;
	m_tLogFont.lfWeight			= FW_BOLD;
	m_tLogFont.lfItalic			= false;
	m_tLogFont.lfUnderline		= false;
	m_tLogFont.lfStrikeOut		= false;
	m_tLogFont.lfCharSet		= DEFAULT_CHARSET;
	m_tLogFont.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	m_tLogFont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	m_tLogFont.lfQuality		= ANTIALIASED_QUALITY;
	m_tLogFont.lfPitchAndFamily	= DEFAULT_PITCH;
	strcpy(m_tLogFont.lfFaceName, "Tahoma");

	m_tBtnFont.CreateFontIndirect(&m_tLogFont);

	// Set Standard Font-Color
	m_tTextColor.m_tDisabled	= RGB(64, 64, 64);
	m_tTextColor.m_tEnabled		= RGB( 0,  0,  0);
	m_tTextColor.m_tClicked		= RGB( 0,  0,  0);
	m_tTextColor.m_tPressed		= RGB( 0,  0,  0);
	m_tTextColor.m_tHot			= RGB( 0,  0,  0);
}

//! Destruction
CRoundButton2::~CRoundButton2(void)
{
	assert(m_tBtnFont.m_hObject);
	m_tBtnFont.DeleteObject();
}

/************************************************************************/
/* public Functions                                                     */
/************************************************************************/

// Set Style of Button
bool CRoundButton2::SetRoundButtonStyle(CRoundButtonStyle* _ptRoundButtonStyle)
{
	// Check, if Button-Style is given
	if (_ptRoundButtonStyle == NULL)
		return false;

	// Set Pointer to ButtonStyle
	m_ptRoundButtonStyle = _ptRoundButtonStyle;

	// Redraw Button
	m_bRedraw = true;

	// All Done
	return false;
}

// Set Font of Button
bool CRoundButton2::SetFont(LOGFONT* _ptLogFont)
{
	if (_ptLogFont == NULL)
		return false;

	// Delete Font, if already given
	if (m_tBtnFont.m_hObject != NULL)
		m_tBtnFont.DeleteObject();

	// Store Infos local
	memcpy(&m_tLogFont, _ptLogFont, sizeof(LOGFONT));

	// Create new Font
	m_tBtnFont.CreateFontIndirect(&m_tLogFont);

	// Button should be redrawn
	m_bRedraw = true;

	return true;
}

// Set Font of Button
bool CRoundButton2::GetFont(LOGFONT* _ptLogFont)
{
	if (_ptLogFont == NULL)
		return false;

	// Store Infos local
	memcpy(_ptLogFont, &m_tLogFont, sizeof(LOGFONT));

	return true;
}

//! Set Color of Caption
bool CRoundButton2::SetTextColor(tColorScheme* _ptTextColor)
{
	if (_ptTextColor == NULL)
		return false;

	// Store Infos locally
	memcpy(&m_tTextColor, _ptTextColor, sizeof(tColorScheme));

	// Button should be redrawn
	m_bRedraw = true;

	return true;
}

//! Get Color of Caption
bool CRoundButton2::GetTextColor(tColorScheme* _ptTextColor)
{
	if (_ptTextColor == NULL)
		return false;

	// Store Infos locally
	memcpy(_ptTextColor, &m_tTextColor, sizeof(tColorScheme));

	return true;
}

/************************************************************************/
/* Own Drawing-Functions                                                */
/************************************************************************/

//! Generate Bitmaps to hold Buttons
void CRoundButton2::GenButtonBMPs(CDC* _pDC, CRect _rRect)
{
	if (m_tBmpBtn.m_hObject != NULL)
		m_tBmpBtn.DeleteObject();
	m_tBmpBtn.m_hObject = NULL;
	// Generate Bitmap
	if (m_tBmpBtn.CreateCompatibleBitmap(_pDC, _rRect.Width(), _rRect.Height() * BS_LAST_STATE) == FALSE)
	{
		m_rBtnSize = CRect(0, 0, 0, 0);
	}
	else
	{
		m_rBtnSize = _rRect;
	}
}

//! Draw Button-Face
void CRoundButton2::DrawButtonFace(CDC* _pDC)
{
	// We need an attached style
	if (m_ptRoundButtonStyle == NULL)
		return;

	// Get Pointer to Bitmap of Masks
	CBitmap* pButtonMasks = m_ptRoundButtonStyle->GetButtonEdge(_pDC);

	// Create Memory-DC
	CDC SourceDC;
	SourceDC.CreateCompatibleDC(_pDC);

	// Select Working Objects into DCs
	HGDIOBJ hOldBmp1 = SourceDC.SelectObject(pButtonMasks);

	int nState;

	CSize tEdgeSize = m_ptRoundButtonStyle->GetEdgeSize();
	CSize tCorrectedEdgeSize;
	CSize tMaskSize = m_ptRoundButtonStyle->GetMaskSize();

	// Correct Edge-Size for smaller Buttons
	tCorrectedEdgeSize.cx = __min(tEdgeSize.cx, __min(m_rBtnSize.Width() / 2, m_rBtnSize.Height() / 2));
	tCorrectedEdgeSize.cy = tCorrectedEdgeSize.cx;

	for (nState = 0; nState < BS_LAST_STATE; nState++)
	{
		/************************************************************************/
		/* Draw Edges                                                           */
		/************************************************************************/
		// Left-Top
		_pDC->StretchBlt(
			0,
			nState * m_rBtnSize.Height(),
			tCorrectedEdgeSize.cx,
			tCorrectedEdgeSize.cy,
			&SourceDC,
			0,
			nState * tMaskSize.cy,
			tEdgeSize.cx,
			tEdgeSize.cy,
			SRCCOPY);
		// Left-Bottom
		_pDC->StretchBlt(
			0,
			nState * m_rBtnSize.Height() + m_rBtnSize.Height() - tCorrectedEdgeSize.cy,
			tCorrectedEdgeSize.cx,
			tCorrectedEdgeSize.cy,
			&SourceDC,
			0,
			nState * tMaskSize.cy + tMaskSize.cy - tEdgeSize.cy,
			tEdgeSize.cx,
			tEdgeSize.cy,
			SRCCOPY);
		// Right-Top
		_pDC->StretchBlt(
			m_rBtnSize.Width() - tCorrectedEdgeSize.cx,
			nState * m_rBtnSize.Height(),
			tCorrectedEdgeSize.cx,
			tCorrectedEdgeSize.cy,
			&SourceDC,
			tMaskSize.cx - tEdgeSize.cx,
			nState * tMaskSize.cy,
			tEdgeSize.cx,
			tEdgeSize.cy,
			SRCCOPY);
		// Right-Bottom
		_pDC->StretchBlt(
			m_rBtnSize.Width() - tCorrectedEdgeSize.cx,
			nState * m_rBtnSize.Height() + m_rBtnSize.Height() - tCorrectedEdgeSize.cy,
			tCorrectedEdgeSize.cx,
			tCorrectedEdgeSize.cy,
			&SourceDC,
			tMaskSize.cx - tEdgeSize.cx,
			nState * tMaskSize.cy + tMaskSize.cy - tEdgeSize.cy,
			tEdgeSize.cx,
			tEdgeSize.cy,
			SRCCOPY);
		/************************************************************************/
		/* Draw Sides                                                           */
		/************************************************************************/
		// Top
		_pDC->StretchBlt(
			tCorrectedEdgeSize.cx,
			nState * m_rBtnSize.Height(),
			m_rBtnSize.Width() - 2 * tCorrectedEdgeSize.cx,
			tCorrectedEdgeSize.cy,
			&SourceDC,
			tEdgeSize.cx,
			nState * tMaskSize.cy,
			1,
			tEdgeSize.cy,
			SRCCOPY);
		// Bottom
		_pDC->StretchBlt(
			tCorrectedEdgeSize.cx,
			nState * m_rBtnSize.Height() + m_rBtnSize.Height() - tCorrectedEdgeSize.cy,
			m_rBtnSize.Width() - 2 * tCorrectedEdgeSize.cx,
			tCorrectedEdgeSize.cy,
			&SourceDC,
			tEdgeSize.cx,
			nState * tMaskSize.cy + tMaskSize.cy - tEdgeSize.cy,
			1,
			tEdgeSize.cy,
			SRCCOPY);
		// Left
		_pDC->StretchBlt(
			0,
			nState * m_rBtnSize.Height() + tCorrectedEdgeSize.cy,
			tCorrectedEdgeSize.cx,
			m_rBtnSize.Height() - 2 * tCorrectedEdgeSize.cy,
			&SourceDC,
			0,
			nState * tMaskSize.cy + tEdgeSize.cy,
			tEdgeSize.cx,
			1,
			SRCCOPY);
		// Right
		_pDC->StretchBlt(
			m_rBtnSize.Width() - tCorrectedEdgeSize.cx,
			nState * m_rBtnSize.Height() + tCorrectedEdgeSize.cy,
			tCorrectedEdgeSize.cx,
			m_rBtnSize.Height() - 2 * tCorrectedEdgeSize.cy,
			&SourceDC,
			tMaskSize.cx - tEdgeSize.cx,
			nState * tMaskSize.cy + tEdgeSize.cy,
			tEdgeSize.cx,
			1,
			SRCCOPY);
		/************************************************************************/
		/* Filling                                                              */
		/************************************************************************/
		_pDC->StretchBlt(
			tCorrectedEdgeSize.cx,
			nState * m_rBtnSize.Height() + tCorrectedEdgeSize.cy,
			m_rBtnSize.Width() - 2* tCorrectedEdgeSize.cx,
			m_rBtnSize.Height() - 2 * tCorrectedEdgeSize.cy,
			&SourceDC,
			tEdgeSize.cx,
			nState * tMaskSize.cy + tEdgeSize.cy,
			1,
			1,
			SRCCOPY);
	}

	// Select Old Objects into DCs
	SourceDC.SelectObject(hOldBmp1);
}

//! Draw Caption on Button
void CRoundButton2::DrawButtonCaption(CDC *_pDC)
{
	// Select Transparency for Background
	int nOldBckMode = _pDC->SetBkMode(TRANSPARENT);

	// Get old Text-Color
	COLORREF tOldColor = _pDC->SetTextColor(RGB(0,0,0));

	// Select Font into DC
	HGDIOBJ hOldFont = _pDC->SelectObject(&m_tBtnFont);

	// Get Caption of Button
	CString sCaption;
	this->GetWindowText(sCaption);

	for (int nState = 0; nState < BS_LAST_STATE; nState++)
	{
		switch(nState)
		{
		case BS_ENABLED:
			_pDC->SetTextColor(m_tTextColor.m_tEnabled);
			break;
		case BS_CLICKED:
			_pDC->SetTextColor(m_tTextColor.m_tClicked);
			break;
		case BS_PRESSED:
			_pDC->SetTextColor(m_tTextColor.m_tPressed);
			break;
		case BS_HOT:
			_pDC->SetTextColor(m_tTextColor.m_tHot);
			break;
		case BS_DISABLED:
		default:
			_pDC->SetTextColor(m_tTextColor.m_tDisabled);
			break;
		}

		_pDC->DrawText(
			sCaption,
			CRect(
				m_rBtnSize.left,
				nState * m_rBtnSize.Height() + m_rBtnSize.top,
				m_rBtnSize.right,
				nState * m_rBtnSize.Height() + m_rBtnSize.bottom),
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	// Select Old Font back
	_pDC->SelectObject(hOldFont);

	// Set old Background-Mode
	_pDC->SetBkMode(nOldBckMode);

	// Set old Text-Color
	_pDC->SetTextColor(tOldColor);
}

/************************************************************************/
/* Overwritten Functions for Init and Draw of Button                    */
/************************************************************************/

//! Presubclass-Window-Function
void CRoundButton2::PreSubclassWindow()
{
#ifdef _DEBUG
	// We really should be only sub classing a button control
	TCHAR buffer[255];
	GetClassName (m_hWnd, buffer, sizeof(buffer) / sizeof(TCHAR));
	ASSERT (CString (buffer) == _T("Button"));
#endif

	// Check if it's a default button
	if (GetStyle() & 0x0FL)
		m_bDefaultButton = true;

	// Make the button owner-drawn
	ModifyStyle (0x0FL, BS_OWNERDRAW | BS_AUTOCHECKBOX, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
}

//! Draw-Item-Function
/*! This Function is called each time, the Button needs a redraw
*/
void CRoundButton2::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Get DC of Item
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	ASSERT (pDC != NULL);

	// Should Buttons be generated?
	bool bGenerate = !m_rBtnSize.EqualRect(&lpDrawItemStruct->rcItem) || m_bRedraw;

	// If Rectangles of Button are not the same
	if (bGenerate)
	{
		// Generate Bitmap to hold Buttons
		GenButtonBMPs(pDC, lpDrawItemStruct->rcItem);

		// Redraw done
		m_bRedraw = false;
	}

	// Generate DC to draw in Memory
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);

	HGDIOBJ hOldBmp = MemDC.SelectObject(m_tBmpBtn);

	CString sActualCaption;
	// Get actual caption
	GetWindowText(sActualCaption);

	// Check, if caption has changed
	if (sActualCaption != m_sOldCaption)
		bGenerate = true;

	// Store old caption
	m_sOldCaption = sActualCaption;

	// If Rectangles of Button are not the same
	if (bGenerate)
	{
		// Draw Buttons
		DrawButtonFace(&MemDC);

		// Draw Button-Caption
		DrawButtonCaption(&MemDC);
	}

	int nButtonState;

	nButtonState = BS_ENABLED;

	if (m_bIsHotButton && m_bMouseOnButton)
		nButtonState = BS_HOT;

	if ((lpDrawItemStruct->itemState & ODS_DISABLED) == ODS_DISABLED)
		nButtonState = BS_DISABLED;
	else
	{
		if ((lpDrawItemStruct->itemState & ODS_SELECTED) == ODS_SELECTED)
			nButtonState = BS_PRESSED;
		else
		{
			if (this->m_bIsChecked)
			{
				nButtonState = BS_CLICKED;
			}
		}
	}

	// Copy correct Bitmap to Screen
	pDC->BitBlt(
		lpDrawItemStruct->rcItem.left,
		lpDrawItemStruct->rcItem.top,
		m_rBtnSize.Width(),
		m_rBtnSize.Height(),
		&MemDC,
		0,
		m_rBtnSize.Height() * nButtonState,
		SRCCOPY);

	MemDC.SelectObject(hOldBmp);
}


BEGIN_MESSAGE_MAP(CRoundButton2, CButton)
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_WM_ERASEBKGND()
//ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

void CRoundButton2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIsCheckButton)
	{
		m_bIsChecked = !m_bIsChecked;
	}
	if (m_bIsRadioButton)
	{
		m_bIsChecked = true;
	}

	CButton::OnLButtonUp(nFlags, point);
}

void CRoundButton2::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rcClient;

	// Get Rectangle of Client
	GetClientRect(rcClient);

	// Check, if Mouse is on Control
	if (rcClient.PtInRect(point))
	{
		// We only need to redraw, if the mouse enters
		/*bool bRedrawNeeded = !m_bMouseOnButton;*/

		// Mouse is on Control
		m_bMouseOnButton = true;

		// Set Capture to recognize, when the mouse leaves the control
		SetCapture();

		// Redraw Control, if Button is hot
		if (m_bIsHotButton)
			Invalidate();
	}
	else
	{
		// We have lost the mouse-capture, so the mouse has left the buttons face
		m_bMouseOnButton = false;

		// Mouse has left the button
		ReleaseCapture();

		// Redraw Control, if Button is hot
		if (m_bIsHotButton)
			Invalidate();
	}

	CButton::OnMouseMove(nFlags, point);
}
void CRoundButton2::OnCaptureChanged(CWnd *pWnd)
{
	// Check, if we lost the mouse-capture
	if (GetCapture() != this)
	{
		// We have lost the mouse-capture, so the mouse has left the buttons face
		m_bMouseOnButton = false;

		// Redraw Control, if Button is hot
		if (m_bIsHotButton)
			Invalidate();
	}

	CButton::OnCaptureChanged(pWnd);
}

BOOL CRoundButton2::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	return FALSE;
	//return CFormView::OnEraseBkgnd(pDC);
}