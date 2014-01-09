#include "stdafx.h"
#include "EventAlienEntity.h"
#include "FontLoader.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "GraphicPool.h"

IMPLEMENT_SERIAL (CEventAlienEntity, CObject, 1)

CEventAlienEntity::CEventAlienEntity()
{
	m_Type = EVENT_SCREEN_TYPE_ALIEN_ENTITY;
}

CEventAlienEntity::CEventAlienEntity(const CString& sPlayersRace, const CString& sAlienEntityID, const CString &sHeadline, const CString &sText)
	: CEventScreen(sPlayersRace, "AlienEntity", sHeadline, sText)
{
	m_sAlienEntityID = sAlienEntityID;
	m_Type = EVENT_SCREEN_TYPE_ALIEN_ENTITY;
}

CEventAlienEntity::~CEventAlienEntity(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEventAlienEntity::Serialize(CArchive &ar)
{
	__super::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sAlienEntityID;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_sAlienEntityID;
	}
}

void CEventAlienEntity::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	CEventScreen::Draw(g, graphicPool);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(static_cast<Gdiplus::ARGB>(Color::White));

	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sRace));
	AssertBotE(pMajor);

	// Bild der Alien-Entity
	CMinor* pAlien = dynamic_cast<CMinor*>(pDoc->GetRaceCtrl()->GetRace(m_sAlienEntityID));
	AssertBotE(pAlien);

	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\" + pAlien->GetGraphicFileName());
	if (graphic == NULL)
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\ImageMissing.boj");
	if (graphic)
		g->DrawImage(graphic, 50, 400, 300, 300);

	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);

	Gdiplus::Color color;
	CFontLoader::GetGDIFontColor(pMajor, 3, color);
	fontBrush.SetColor(color);

	g->DrawString(CComBSTR(m_strHeadline), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,1280,96), &fontFormat, &fontBrush);

	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CComBSTR(m_strText), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(375, 400, 800, 300), &fontFormat, &fontBrush);

	// Buttons zeichnen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontBrush.SetColor(color);
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, graphicPool, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}
