#include "StdAfx.h"
#include "MyButton.h"
#include "mytrace.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMyButton::CMyButton(CPoint point, CSize size, CString text, CString normGraphicName,
					 CString inactiveGraphicName, CString activeGraphicName) : m_KO(point), m_Size(size),
					 m_strText(text), m_strNormal(normGraphicName), m_strInactive(inactiveGraphicName),
					 m_strActive(activeGraphicName), m_byStatus(0)
{	
}

CMyButton::~CMyButton()
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
BOOLEAN CMyButton::ClickedOnButton(const CPoint pt)
{
	return CRect(m_KO.x, m_KO.y, m_KO.x+m_Size.cx, m_KO.y+m_Size.cy).PtInRect(pt);
}

void CMyButton::DrawButton(Gdiplus::Graphics &g, CGraphicPool* graphicPool, Gdiplus::Font &font, Gdiplus::SolidBrush &brush)
{
	ASSERT(graphicPool);

	Bitmap* graphic = NULL;
	CString sFile;		
	switch (m_byStatus)
	{
	case 0:  sFile = m_strNormal; break;
	case 1:  sFile = m_strActive; break;
	default: sFile = m_strInactive;
	}

	graphic = graphicPool->GetGDIGraphic(sFile);
	if (graphic)
	{
		// Buttongrafik zeichnen
		g.DrawImage(graphic, m_KO.x, m_KO.y, m_Size.cx, m_Size.cy);
		// Text auf dem Button zeichnen
		Gdiplus::StringFormat fontFormat;
		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
		g.DrawString(m_strText.AllocSysString(), -1, &font, RectF((REAL)m_KO.x, (REAL)m_KO.y + 2, (REAL)m_Size.cx, (REAL)m_Size.cy), &fontFormat, &brush);
	}
	else
		MYTRACE(MT::LEVEL_WARNING, "Could not load buttongraphic" + sFile + "\n");
}

BOOLEAN CMyButton::Activate()
{
	// nicht deaktiviert
	if (m_byStatus == 0)
	{		
		m_byStatus = 1;
		return TRUE;
	}
	return FALSE;
}

BOOLEAN CMyButton::Deactivate()
{
	// nicht deaktiviert
	if (m_byStatus == 1)
	{
		m_byStatus = 0;
		return TRUE;
	}
	return FALSE;
}