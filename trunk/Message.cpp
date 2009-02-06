// Message.cpp: Implementierung der Klasse CMessage.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Message.h"

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
	m_iOwnerOfMessage = 0;
	m_iMessageType = NO_TYPE;
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
	m_iOwnerOfMessage = rhs.m_iOwnerOfMessage;
	m_iMessageType = rhs.m_iMessageType;
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
	m_iOwnerOfMessage = rhs.m_iOwnerOfMessage;
	m_iMessageType = rhs.m_iMessageType;
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
		ar << m_iOwnerOfMessage;
		ar << m_iMessageType;
		ar << m_KO;
		ar << m_byFlag;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_strMessage;
		ar >> m_iOwnerOfMessage;
		ar >> m_iMessageType;
		ar >> m_KO;
		ar >> m_byFlag;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CMessage::GenerateMessage(const CString& Message, USHORT OwnerOfMessage, USHORT MessageType, 
							   const CString& SystemName, CPoint SystemKO, BOOLEAN Update, BYTE flag)
{
	m_iOwnerOfMessage = OwnerOfMessage;
	m_iMessageType = MessageType;
	m_KO = SystemKO;
	m_byFlag = flag;	// wird häufig benutzt, um die Message mit einem Menü zu verbinden
	switch (MessageType)
	{
	case ECONOMY:
		{
			if (!SystemName.IsEmpty())
			{
				if (Update == FALSE)
					m_strMessage = CResourceManager::GetString("BUILDING_FINISH", FALSE, Message, SystemName);
				else
					m_strMessage = CResourceManager::GetString("UPGRADE_FINISH", FALSE, Message, SystemName);
			}
			else
				m_strMessage.Format(Message);
			break;
		}
	case SOMETHING:
		{
			m_strMessage.Format(Message);
			break;
		}
	case RESEARCH:
		{
			m_strMessage.Format(Message);
			break;
		}
	case DIPLOMACY:
		{
			m_strMessage.Format(Message);
			break;
		}
	default:
		{
			m_strMessage.Format(Message);
		}
	}
}

void CMessage::ShowMessage(CDC* pDC, CRect r)
{
	pDC->DrawText(m_strMessage,r,DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}