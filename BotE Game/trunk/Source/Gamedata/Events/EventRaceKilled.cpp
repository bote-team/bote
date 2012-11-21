#include "stdafx.h"
#include "EventRaceKilled.h"
#include "FontLoader.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"
#include "General/ResourceManager.h"

IMPLEMENT_SERIAL (CEventRaceKilled, CObject, 1)

CEventRaceKilled::CEventRaceKilled(const CString& sPlayersRace, const CString& sKilledRace, const CString& sKilledRaceName, const CString& sKilledRaceImage)
	: CEventScreen(sPlayersRace, "RaceKilled")
{
	m_sKilledRace = sKilledRace;
	m_sKilledRaceName = sKilledRaceName;
	m_sKilledRaceImage = sKilledRaceImage;
}

CEventRaceKilled::~CEventRaceKilled(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEventRaceKilled::Serialize(CArchive &ar)
{
	__super::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sKilledRace;
		ar << m_sKilledRaceName;
		ar << m_sKilledRaceImage;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_sKilledRace;
		ar >> m_sKilledRaceName;
		ar >> m_sKilledRaceImage;
	}
}

void CEventRaceKilled::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	CEventScreen::Draw(g, graphicPool);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);

	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sRace));
	ASSERT(pMajor);

	// Bild der ausgelöschten Rasse zeichnen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\" + m_sKilledRaceImage);
	if (graphic == NULL)
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\ImageMissing.boj");
	if (graphic)
		g->DrawImage(graphic, 50, 400, 300, 300);
	// Rassensymbol noch in die Ecke zeichnen
	graphic	= pDoc->GetGraphicPool()->GetGDIGraphic("Symbols\\" + m_sKilledRace + ".bop");
	if (graphic)
		g->DrawImage(graphic, 275, 625, 75, 75);
	g->DrawRectangle(&Pen(Color(200,200,200), 2), 50, 400, 300, 300);

	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);

	Gdiplus::Color color;
	CFontLoader::GetGDIFontColor(pMajor, 3, color);
	fontBrush.SetColor(color);

	CString s = CResourceManager::GetString("IMPORTANT_NEWS");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,1280,96), &fontFormat, &fontBrush);

	fontFormat.SetAlignment(StringAlignmentNear);
	s = CResourceManager::GetString("RACE_ELIMINATED", FALSE, m_sKilledRaceName);
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(375, 400, 800, 300), &fontFormat, &fontBrush);

	// Buttons zeichnen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontBrush.SetColor(color);
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, graphicPool, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}
