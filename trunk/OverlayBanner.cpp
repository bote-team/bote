#include "stdafx.h"
#include "OverlayBanner.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion und Destruktion
//////////////////////////////////////////////////////////////////////
COverlayBanner::COverlayBanner(const CPoint &ko, const CPoint &size, const CString &text, COLORREF textColor)
	: m_KO(ko), m_Size(size), m_strText(text), m_TextColor(textColor), m_BackgroundColor(RGB(0,0,0)),
	m_BorderColor(RGB(200,0,0)), m_iAlphaValue(180), m_iBorderWidth(2)
{
}

COverlayBanner::~COverlayBanner(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
// Funktion zeichnet das Banner auf den übergebenen Zeichenkontext mittels GDI.
void COverlayBanner::Draw(CDC *pDC)
{
	FCObjImage mark;
	// hier wurde der R Wert mit dem B Wert getauscht, da die Funktion SetPixelData sonst nicht stimmt.
	// Bitmap generieren
	if (mark.Create(1, 1, 32))
		mark.SetPixelData(0,0, m_BackgroundColor);
	
	// Alpha Channel festlegen
	mark.SetAlphaChannelValue(m_iAlphaValue);
	
	// Hintergrund zeichnen
	FCWin32::AlphaImageOnDC(mark, pDC->GetSafeHdc(), CRect(m_KO.x, m_KO.y, m_KO.x + m_Size.cx, m_KO.y + m_Size.cy));
	
	// Rahmen zeichnen
	CPen np(PS_SOLID, m_iBorderWidth, m_BorderColor);
	pDC->SelectObject(&np);
	pDC->MoveTo(m_KO);
	pDC->LineTo(m_KO.x + m_Size.cx, m_KO.y);
	pDC->LineTo(m_KO.x + m_Size.cx, m_KO.y + m_Size.cy);
	pDC->LineTo(m_KO.x, m_KO.y + m_Size.cy);
	pDC->LineTo(m_KO);
	
	// Text zeichnen
	COLORREF oldColor = pDC->GetTextColor();
	pDC->SetTextColor(m_TextColor);
	pDC->DrawText(m_strText, CRect(m_KO.x, m_KO.y + m_Size.cy / 3, m_KO.x + m_Size.cx, m_KO.y + m_Size.cy), DT_CENTER | DT_WORDBREAK | DT_VCENTER);
	pDC->SetTextColor(oldColor);
}

// Funktion zeichnet das Banner auf den übergebenen Zeichenkontext mittels GDI+
void COverlayBanner::Draw(Graphics* g, Gdiplus::Font* font)
{
	Gdiplus::SolidBrush brush(Gdiplus::Color(m_iAlphaValue, 50, 50, 50));
	
	// Hintergrund zeichnen
	g->FillRectangle(&brush, Rect(m_KO.x, m_KO.y, m_Size.cx, m_Size.cy));
	
	// Rahmen zeichnen
	Gdiplus::Color color;
	color.SetFromCOLORREF(m_BorderColor);
	Gdiplus::Pen pen(color);
	pen.SetWidth((REAL)m_iBorderWidth);
	g->DrawRectangle(&pen, m_KO.x, m_KO.y, m_Size.cx, m_Size.cy);

	// Text zeichnen
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);

	color.SetFromCOLORREF(m_TextColor);
	Gdiplus::SolidBrush fontBrush(color);
	g->DrawString(m_strText.AllocSysString(), -1, font, RectF((REAL)m_KO.x, (REAL)m_KO.y + m_Size.cy / 3, (REAL)m_Size.cx, (REAL)m_Size.cy), &fontFormat, &fontBrush);	
}
