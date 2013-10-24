#include "stdafx.h"
#include "IntelObject.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelObject::CIntelObject(void) : m_sOwner(""), m_sEnemy(""), m_byType(0), m_bIsSpy(TRUE), m_nRound(0)
{
	m_strEnemyDesc = "";
	m_strOwnerDesc = "";
}

/// Konstruktor mit Paramterübergabe
CIntelObject::CIntelObject(const CString& sOwnerID, const CString& sEnemyID, USHORT round, BOOLEAN isSpy, BYTE type)
	: m_sOwner(sOwnerID), m_sEnemy(sEnemyID), m_byType(type), m_bIsSpy(isSpy), m_nRound(round)
{
	m_strEnemyDesc = "";
	m_strOwnerDesc = "";
}

CIntelObject::~CIntelObject(void)
{
}

// Kopierkonstruktor
CIntelObject::CIntelObject(const CIntelObject & rhs)
{
	m_sOwner = rhs.m_sOwner;
	m_sEnemy = rhs.m_sEnemy;
	m_nRound = rhs.m_nRound;
	m_bIsSpy = rhs.m_bIsSpy;
	m_byType = rhs.m_byType;
	m_strEnemyDesc = rhs.m_strEnemyDesc;
	m_strOwnerDesc = rhs.m_strOwnerDesc;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CIntelObject::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_sOwner;
		ar << m_sEnemy;
		ar << m_nRound;
		ar << m_bIsSpy;
		ar << m_byType;
		ar << m_strEnemyDesc;
		ar << m_strOwnerDesc;
	}
	else if (ar.IsLoading())
	{
		ar >> m_sOwner;
		ar >> m_sEnemy;
		ar >> m_nRound;
		ar >> m_bIsSpy;
		ar >> m_byType;
		ar >> m_strEnemyDesc;
		ar >> m_strOwnerDesc;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
