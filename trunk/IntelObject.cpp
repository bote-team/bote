#include "stdafx.h"
#include "IntelObject.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelObject::CIntelObject(void) : m_byOwner(NOBODY), m_byEnemy(NOBODY), m_nRound(0), m_bIsSpy(TRUE), m_byType(0)
{
	m_strEnemyDesc = "";
	m_strOwnerDesc = "";
}

/// Konstruktor mit Paramterübergabe
CIntelObject::CIntelObject(BYTE owner, BYTE enemy, USHORT round, BOOLEAN isSpy, BYTE type)
	: m_byOwner(owner), m_byEnemy(enemy), m_nRound(round), m_bIsSpy(isSpy), m_byType(type)
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
	m_byOwner = rhs.m_byOwner;
	m_byEnemy = rhs.m_byEnemy;
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
		ar << m_byOwner;
		ar << m_byEnemy;
		ar << m_nRound;
		ar << m_bIsSpy;
		ar << m_byType;
		ar << m_strEnemyDesc;
		ar << m_strOwnerDesc;
	}
	else if (ar.IsLoading())
	{
		ar >> m_byOwner;
		ar >> m_byEnemy;
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