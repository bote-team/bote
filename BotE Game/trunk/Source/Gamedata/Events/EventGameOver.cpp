#include "stdafx.h"
#include "EventGameOver.h"
#include "FontLoader.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "IniLoader.h"
#include "IOData.h"
#include "General/Loc.h"

IMPLEMENT_SERIAL (CEventGameOver, CObject, 1)

CEventGameOver::CEventGameOver(const CString& sPlayersRace)
	: CEventScreen(sPlayersRace, "GameOver")
{
}

CEventGameOver::~CEventGameOver(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CEventGameOver::Create(void)
{
	if (m_pBGImage != NULL)
		return;

	CIniLoader* pIni = CIniLoader::GetInstance();
	ASSERT(pIni);

	float fMusicVolume;
	pIni->ReadValue("Audio", "MUSICVOLUME", fMusicVolume);

	CSoundManager* pSoundManager = CSoundManager::GetInstance();
	ASSERT(pSoundManager);

	CString sTheme = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + "LosingTheme.ogg";
	pSoundManager->StartMusic(sTheme, fMusicVolume);

	__super::Create();
}

void CEventGameOver::Close(void)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);
	pDoc->GameOver();
}

void CEventGameOver::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	CEventScreen::Draw(g, graphicPool);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);

	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sRace));
	ASSERT(pMajor);

	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);

	Gdiplus::Color color;
	CFontLoader::GetGDIFontColor(pMajor, 3, color);
	fontBrush.SetColor(color);

	CString s = CLoc::GetString("GAMEOVER");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize * 2.5f), RectF(0, 0, 1280, 1024 / 3.0f), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentNear);
	s = CLoc::GetString("WE_LOST") + "\n" + CLoc::GetString("OUR_EMPIRES_IS_ELIMINATED");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(75, 1024 / 1.5f - 50, 1280 / 2, 1024 / 3 - 50), &fontFormat, &fontBrush);

	// Buttons zeichnen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontBrush.SetColor(color);
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, graphicPool, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}
