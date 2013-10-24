#include "StdAfx.h"
#include "MyButton.h"
#include "mytrace.h"
#include "GraphicPool.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMyButton::CMyButton(CPoint point, CSize size, const CString& text, const CString& normGraphicName,
					 const CString& inactiveGraphicName, const CString& activeGraphicName) :
	m_nStatus(BUTTON_STATE::NORMAL),
	m_sText(text),
	m_ptKO(point),
	m_szSize(size),
	m_sNormal(normGraphicName),
	m_sInactive(inactiveGraphicName),
	m_sActive(activeGraphicName)
{
}

CMyButton::~CMyButton()
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
bool CMyButton::ClickedOnButton(const CPoint& pt) const
{
	return (CRect(m_ptKO.x, m_ptKO.y, m_ptKO.x+m_szSize.cx, m_ptKO.y+m_szSize.cy).PtInRect(pt) == TRUE);
}

void CMyButton::DrawButton(Gdiplus::Graphics &g, CGraphicPool* graphicPool, Gdiplus::Font &font, Gdiplus::SolidBrush &brush) const
{
	ASSERT(graphicPool);

	CString sFile;
	switch (m_nStatus)
	{
	case 0:  sFile = m_sNormal; break;
	case 1:  sFile = m_sActive; break;
	default: sFile = m_sInactive;
	}

	if (Bitmap* pGraphic = graphicPool->GetGDIGraphic(sFile))
	{
		// Buttongrafik zeichnen
		g.DrawImage(pGraphic, m_ptKO.x, m_ptKO.y, m_szSize.cx, m_szSize.cy);
		// Text auf dem Button zeichnen
		if (m_sText != "")
		{
			Gdiplus::StringFormat fontFormat;
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			g.DrawString(CComBSTR(m_sText), -1, &font, RectF((REAL)m_ptKO.x, (REAL)m_ptKO.y + 2, (REAL)m_szSize.cx, (REAL)m_szSize.cy), &fontFormat, &brush);
		}
	}
	else
	{
		MYTRACE("general")(MT::LEVEL_WARNING, "Could not load buttongraphic" + sFile + "\n");
	}
}

bool CMyButton::Activate()
{
	// nicht deaktiviert
	if (m_nStatus == BUTTON_STATE::NORMAL)
	{
		m_nStatus = BUTTON_STATE::ACTIVATED;
		return true;
	}
	return false;
}

bool CMyButton::Deactivate()
{
	// nicht deaktiviert
	if (m_nStatus == BUTTON_STATE::ACTIVATED)
	{
		m_nStatus = BUTTON_STATE::NORMAL;
		return true;
	}
	return false;
}
