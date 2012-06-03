#include "stdafx.h"
#include "EventScreen.h"
#include "IOData.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

IMPLEMENT_SERIAL (CEventScreen, CObject, 1)

CEventScreen::CEventScreen(void)
{
	m_pBGImage = NULL;
}

CEventScreen::CEventScreen(const CString &sPlayersRaceID, const CString &imageName, const CString &headline, const CString &text)
{
	m_sRace	   = sPlayersRaceID;
	m_strImagePath = "Graphics\\Events\\" + imageName + ".boj";
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
		ar << m_sRace;
		ar << m_strImagePath;
		ar << m_strHeadline;
		ar << m_strText;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_sRace;
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

	CString sFile = CIOData::GetInstance()->GetAppPath() + m_strImagePath;
	m_pBGImage = Bitmap::FromFile(sFile.AllocSysString());
	ASSERT(m_pBGImage);
	if (m_pBGImage->GetLastStatus() != Ok)
		return;

	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	// alle Buttons in der View anlegen und Grafiken laden
	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sRace));
	ASSERT(pMajor);

	CString sPrefix = pMajor->GetPrefix();

	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	m_Buttons.Add(new CMyButton(CPoint(560,970), CSize(160,40), CResourceManager::GetString("BTN_OKAY"),  fileN, fileI, fileA));
}

void CEventScreen::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	if (m_pBGImage)
		g->DrawImage(m_pBGImage, 0, 0, 1280, 1024);
}
