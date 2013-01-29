#include "stdafx.h"
#include "EventVictory.h"
#include "FontLoader.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"
#include "IniLoader.h"
#include "IOData.h"
#include "General/ResourceManager.h"

IMPLEMENT_SERIAL (CEventVictory, CObject, 1)

CEventVictory::CEventVictory(void) :
	m_nType(VICTORYTYPE_ELIMINATION)
{
}

CEventVictory::CEventVictory(const CString& sPlayersRace, const CString& sWinnerRace, int nType, const CString &imageName)
	: CEventScreen(sPlayersRace, imageName)
{
	m_sWinnerRace = sWinnerRace;
	m_nType = nType;
}

CEventVictory::~CEventVictory(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEventVictory::Serialize(CArchive &ar)
{
	__super::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sWinnerRace;
		ar << m_nType;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_sWinnerRace;
		ar >> m_nType;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CEventVictory::Create(void)
{
	if (m_pBGImage != NULL)
		return;

	/*network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(m_sRace);*/

	CIniLoader* pIni = CIniLoader::GetInstance();
	ASSERT(pIni);

	float fMusicVolume;
	pIni->ReadValue("Audio", "MUSICVOLUME", fMusicVolume);

	CSoundManager* pSoundManager = CSoundManager::GetInstance();
	ASSERT(pSoundManager);

	CString sTheme = "";
	if (m_sWinnerRace == m_sRace)
		sTheme = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + "VictoryTheme.ogg";
	else
		sTheme = CIOData::GetInstance()->GetAppPath() + "Sounds\\" + "LosingTheme.ogg";
	pSoundManager->StartMusic(sTheme, fMusicVolume);

	__super::Create();
}

void CEventVictory::Close(void)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);
	pDoc->GameOver();
}

void CEventVictory::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	CEventScreen::Draw(g, graphicPool);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);

	CBotf2Doc* pDoc = resources::pDoc;
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

	CString sType = "";
	switch (m_nType)
	{
	case VICTORYTYPE_ELIMINATION:	sType = CResourceManager::GetString("VC_ELIMINATION");	break;
	case VICTORYTYPE_DIPLOMACY:		sType = CResourceManager::GetString("VC_DIPLOMACY");	break;
	case VICTORYTYPE_CONQUEST:		sType = CResourceManager::GetString("VC_CONQUEST");		break;
	case VICTORYTYPE_RESEARCH:		sType = CResourceManager::GetString("VC_RESEARCH");		break;
	case VICTORYTYPE_COMBATWINS:	sType = CResourceManager::GetString("VC_COMBAT");		break;
	case VICTORYTYPE_SABOTAGE:		sType = CResourceManager::GetString("VC_SECURITY");		break;
	}

	if (m_sRace == m_sWinnerRace)
	{
		CString s = CResourceManager::GetString("VICTORY");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize * 2.5f), RectF(0, 0, 1280, 1024 / 3.0f), &fontFormat, &fontBrush);

		CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
		s = CResourceManager::GetString("CONGRATULATIONS");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize * 1.5), RectF(0, 75, 1280, 1024 / 3 + 75), &fontFormat, &fontBrush);

		fontFormat.SetAlignment(StringAlignmentNear);

		int nGamePoints = pDoc->GetStatistics()->GetGamePoints(m_sRace, pDoc->GetCurrentRound(), pDoc->GetDifficultyLevel());
		// bei Sieg werden die Punkte verdoppelt!
		nGamePoints *= 2;

		CString sStarDate;
		sStarDate.Format("%s: %.1lf", CResourceManager::GetString("STARDATE"), pDoc->m_fStardate);
		s.Format("%s:\n\n%s\n\n%s\n%s\n\n%s %d",
			CResourceManager::GetString("LOGBOOK_ENTRY"),
			sStarDate,
			CResourceManager::GetString("WE_REACHED_VICTORY_IN_TYPE", FALSE, sType),
			CResourceManager::GetString("BIRTH_OF_THE_EMPIRE_END"),
			CResourceManager::GetString("REACHED_GAMEPOINTS"),
			nGamePoints);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(75, 1024 / 1.5f - 75, 1280 - 75, 1024 / 3 - 75), &fontFormat, &fontBrush);
	}
	else
	{
		CString s = CResourceManager::GetString("DEFEAT");
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize * 2.5f), RectF(0, 0, 1280, 1024 / 3.0f), &fontFormat, &fontBrush);

		CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
		fontFormat.SetAlignment(StringAlignmentNear);

		CMajor* pWinner = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sWinnerRace));
		CString sRaceName = "?";
		if (pWinner)
		{
			sRaceName = pWinner->GetEmpireNameWithArticle();
			// Groß beginnen
			CString sUpper = (CString)sRaceName.GetAt(0);
			sRaceName.SetAt(0, sUpper.MakeUpper().GetAt(0));
		}

		int nGamePoints = pDoc->GetStatistics()->GetGamePoints(m_sRace, pDoc->GetCurrentRound(), pDoc->GetDifficultyLevel());
		s.Format("%s\n\n\n%s %d",
			CResourceManager::GetString("OTHER_REACHED_VICTORY_IN_TYPE", FALSE, sRaceName, sType),
			CResourceManager::GetString("REACHED_GAMEPOINTS"),
			nGamePoints);
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(75, 1024 / 1.5f - 75, 1280 - 75, 1024 / 3 - 75), &fontFormat, &fontBrush);
	}


	fontFormat.SetAlignment(StringAlignmentFar);
	CString sLogo = "Birth of the Empires";
	g->DrawString(CComBSTR(sLogo), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(1280 - 200, 1024 - 25, 200, 25), &fontFormat, &fontBrush);

	// Buttons zeichnen

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontBrush.SetColor(color);
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, graphicPool, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}
