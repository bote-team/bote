#include "stdafx.h"
#include "EventScreen.h"
#include "Botf2.h"

IMPLEMENT_SERIAL (CEventScreen, CObject, 1)

CEventScreen::CEventScreen(void)
{
	m_pBGImage = NULL;
}

CEventScreen::CEventScreen(BYTE playersRace, const CString &imageName, const CString &headline, const CString &text)
{
	ASSERT(playersRace >= HUMAN && playersRace <= DOMINION);

	m_byRace	   = playersRace;
	m_strImagePath = "Graphics\\Events\\" + imageName + ".jpg";
	m_strHeadline  = headline;
	m_strText      = text;
	m_pBGImage	   = NULL;
}

CEventScreen::~CEventScreen(void)
{
	if (m_pBGImage)
	{
		delete m_pBGImage;
		m_pBGImage = NULL;
	}
	for (int i = 0; i < m_Buttons.GetSize(); i++)
	{
		delete m_Buttons[i];
		m_Buttons[i] = 0;
	}
	m_Buttons.RemoveAll();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEventScreen::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_byRace;
		ar << m_strImagePath;
		ar << m_strHeadline;
		ar << m_strText;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_byRace;
		ar >> m_strImagePath;
		ar >> m_strHeadline;
		ar >> m_strText;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CEventScreen::Create(void)
{
	if (m_pBGImage != NULL)
		return;

	m_pBGImage = Bitmap::FromFile(*((CBotf2App*)AfxGetApp())->GetPath() + "\\" + m_strImagePath.AllocSysString());
	ASSERT(m_pBGImage);

	// alle Buttons in der View anlegen und Grafiken laden
	switch(m_byRace)
	{
	case HUMAN:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2.png";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2i.png";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE1_PREFIX") + "button_big2a.png";
			m_Buttons.Add(new CMyButton(CPoint(560,950), CSize(160,40), CResourceManager::GetString("BTN_OKAY"),  fileN, fileI, fileA));			
			break;
		}
	case FERENGI:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE2_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(560,950), CSize(160,40), CResourceManager::GetString("BTN_OKAY"),  fileN, fileI, fileA));
			break;
		}
	case KLINGON:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2i.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE3_PREFIX") + "button2a.jpg";
			m_Buttons.Add(new CMyButton(CPoint(560,950), CSize(160,40), CResourceManager::GetString("BTN_OKAY"),  fileN, fileI, fileA));
			break;
		}
	case ROMULAN:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE4_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(560,950), CSize(160,40), CResourceManager::GetString("BTN_OKAY"),  fileN, fileI, fileA));
			break;
		}
	case CARDASSIAN:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE5_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(560,950), CSize(160,40), CResourceManager::GetString("BTN_OKAY"),  fileN, fileI, fileA));
			break;
		}
	case DOMINION:
		{
			CString fileN = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "button.jpg";
			CString fileI = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttoni.jpg";
			CString fileA = "Other\\" + CResourceManager::GetString("RACE6_PREFIX") + "buttona.jpg";
			m_Buttons.Add(new CMyButton(CPoint(560,950), CSize(160,40), CResourceManager::GetString("BTN_OKAY"),  fileN, fileI, fileA));
			break;
		}
	}
}

void CEventScreen::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	if (m_pBGImage)
		g->DrawImage(m_pBGImage, 0, 0, 1280, 1024);	
}