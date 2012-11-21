// Message.cpp: Implementierung der Klasse CMessage.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Message.h"
#include "General/ResourceManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CMessage, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMessage::CMessage()
{
	m_strMessage = "";
	m_nMessageType = MESSAGE_TYPE::NO_TYPE;
	m_KO.x = 0;
	m_KO.y = 0;
	m_byFlag = 0;
}

CMessage::~CMessage()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CMessage::CMessage(const CMessage & rhs)
{
	m_strMessage = rhs.m_strMessage;
	m_nMessageType = rhs.m_nMessageType;
	m_KO = rhs.m_KO;
	m_byFlag = rhs.m_byFlag;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CMessage & CMessage::operator=(const CMessage & rhs)
{
	if (this == &rhs)
		return *this;
	m_strMessage = rhs.m_strMessage;
	m_nMessageType = rhs.m_nMessageType;
	m_KO = rhs.m_KO;
	m_byFlag = rhs.m_byFlag;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMessage::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_strMessage;
		ar << m_nMessageType;
		ar << m_KO;
		ar << m_byFlag;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_strMessage;
		int nType;
		ar >> nType;
		m_nMessageType = (MESSAGE_TYPE::Typ)nType;
		ar >> m_KO;
		ar >> m_byFlag;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CMessage::GenerateMessage(const CString& sMessage, MESSAGE_TYPE::Typ nMessageType, const CString& sSystemName, const CPoint& SystemKO, bool bUpdate, BYTE byFlag)
{
	m_nMessageType = nMessageType;
	m_KO = SystemKO;
	m_byFlag = byFlag;	// wird häufig benutzt, um die Message mit einem Menü zu verbinden
	switch (nMessageType)
	{
	case MESSAGE_TYPE::ECONOMY:
		{
			if (!sSystemName.IsEmpty())
			{
				if (!bUpdate)
					m_strMessage = CResourceManager::GetString("BUILDING_FINISH", FALSE, sMessage, sSystemName);
				else
					m_strMessage = CResourceManager::GetString("UPGRADE_FINISH", FALSE, sMessage, sSystemName);
			}
			else
				m_strMessage.Format(sMessage);
			break;
		}
	case MESSAGE_TYPE::SOMETHING:
		{
			m_strMessage.Format(sMessage);
			break;
		}
	case MESSAGE_TYPE::RESEARCH:
		{
			m_strMessage.Format(sMessage);
			break;
		}
	case MESSAGE_TYPE::DIPLOMACY:
		{
			m_strMessage.Format(sMessage);
			break;
		}
	default:
		{
			m_strMessage.Format(sMessage);
		}
	}
}
