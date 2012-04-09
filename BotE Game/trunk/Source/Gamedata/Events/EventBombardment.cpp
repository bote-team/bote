#include "stdafx.h"
#include "EventBombardment.h"
#include "FontLoader.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

IMPLEMENT_SERIAL (CEventBombardment, CObject, 1)

CEventBombardment::CEventBombardment(const CString& sPlayersRace, const CString &imageName, const CString &headline, const CString &text)
	: CEventScreen(sPlayersRace, imageName, headline, text)
{	
}

CEventBombardment::~CEventBombardment(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEventBombardment::Serialize(CArchive &ar)		
{
	__super::Serialize(ar);
	
	// wenn gespeichert wird
	if (ar.IsStoring())
	{		
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{		
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CEventBombardment::Create(void)
{
	CEventScreen::Create();
}

void CEventBombardment::Draw(Graphics* g, CGraphicPool* graphicPool) const
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

	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	Gdiplus::Color color;
	CFontLoader::GetGDIFontColor(pMajor, 3, color);
	fontBrush.SetColor(color);

	g->DrawString(m_strHeadline.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,1280,96), &fontFormat, &fontBrush);

	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(m_strText.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(100,450,1080,128), &fontFormat, &fontBrush);

	// Buttons zeichnen
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontBrush.SetColor(color);
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, graphicPool, Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);	
}
