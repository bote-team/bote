// EmpireNews.cpp: Implementierung der Klasse CEmpireNews.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EmpireNews.h"
#include "General/Loc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CEmpireNews, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CEmpireNews::CEmpireNews()
{
	m_sText = "";
	m_nType = EMPIRE_NEWS_TYPE::NO_TYPE;
	m_ptKO.x = 0;
	m_ptKO.y = 0;
	m_byFlag = 0;
}

CEmpireNews::~CEmpireNews()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CEmpireNews::CEmpireNews(const CEmpireNews & rhs)
{
	m_sText = rhs.m_sText;
	m_nType = rhs.m_nType;
	m_ptKO = rhs.m_ptKO;
	m_byFlag = rhs.m_byFlag;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CEmpireNews & CEmpireNews::operator=(const CEmpireNews & rhs)
{
	if (this == &rhs)
		return *this;
	m_sText = rhs.m_sText;
	m_nType = rhs.m_nType;
	m_ptKO = rhs.m_ptKO;
	m_byFlag = rhs.m_byFlag;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEmpireNews::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sText;
		ar << m_nType;
		ar << m_ptKO;
		ar << m_byFlag;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_sText;
		int nType;
		ar >> nType;
		m_nType = (EMPIRE_NEWS_TYPE::Typ)nType;
		ar >> m_ptKO;
		ar >> m_byFlag;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CEmpireNews::CreateNews(const CString& sMessage, EMPIRE_NEWS_TYPE::Typ nMessageType, const CString& sSystemName /* = "" */, const CPoint& ptKO /* = CPoint(-1, -1) */, bool bUpdate /*= false*/, BYTE byFlag /*= 0*/)
{
	m_nType = nMessageType;
	m_ptKO = ptKO;
	m_byFlag = byFlag;	// wird häufig benutzt, um die Message mit einem Menü zu verbinden
	switch (nMessageType)
	{
	case EMPIRE_NEWS_TYPE::ECONOMY:
		{
			if (!sSystemName.IsEmpty())
			{
				if (!bUpdate)
					m_sText = CLoc::GetString("BUILDING_FINISH", FALSE, sMessage, sSystemName);
				else
					m_sText = CLoc::GetString("UPGRADE_FINISH", FALSE, sMessage, sSystemName);
			}
			else
				m_sText.Format(sMessage);
			break;
		}
	case EMPIRE_NEWS_TYPE::SOMETHING:
		{
			m_sText.Format(sMessage);
			break;
		}
	case EMPIRE_NEWS_TYPE::RESEARCH:
		{
			m_sText.Format(sMessage);
			break;
		}
	case EMPIRE_NEWS_TYPE::DIPLOMACY:
		{
			m_sText.Format(sMessage);
			break;
		}
	default:
		{
			m_sText.Format(sMessage);
		}
	}
}

void CEmpireNews::CreateNews(const CString& sMessage, EMPIRE_NEWS_TYPE::Typ nMessageType, BYTE byFlag)
{
	CreateNews(sMessage, nMessageType, "", CPoint(-1, -1), false, byFlag);
}
