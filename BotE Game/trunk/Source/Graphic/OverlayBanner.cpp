#include "stdafx.h"
#include "OverlayBanner.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion und Destruktion
//////////////////////////////////////////////////////////////////////
COverlayBanner::COverlayBanner(const CPoint &ko, const CPoint &size, const CString &text, COLORREF textColor) :
	m_KO(ko),
	m_Size(size),
	m_TextColor(textColor),
	m_BackgroundColor(RGB(0,0,0)),
	m_BorderColor(RGB(200,0,0)),
	m_iAlphaValue(180),
	m_iBorderWidth(2),
	m_strText(text)
{
}

COverlayBanner::~COverlayBanner(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

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
	g->DrawRectangle(&pen, m_KO.x, m_KO.y, m_Size.cx - m_iBorderWidth, m_Size.cy - m_iBorderWidth);

	// Text zeichnen
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);

	color.SetFromCOLORREF(m_TextColor);
	Gdiplus::SolidBrush fontBrush(color);
	g->DrawString(CComBSTR(m_strText), -1, font, RectF((REAL)m_KO.x, (REAL)m_KO.y + m_Size.cy / 3, (REAL)m_Size.cx, (REAL)m_Size.cy), &fontFormat, &fontBrush);
}
