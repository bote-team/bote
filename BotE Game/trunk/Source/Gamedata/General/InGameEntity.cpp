
#include "stdafx.h"
#include "General/InGameEntity.h"


//////////////////////////////////////////////////////////////////////
// construction/destruction
//////////////////////////////////////////////////////////////////////
CInGameEntity::CInGameEntity(void) :
	m_Co(-1, -1)
{
	Reset();
}

CInGameEntity::CInGameEntity(int x, int y) :
	m_Co(x, y)
{
	Reset();
}

CInGameEntity::CInGameEntity(const CInGameEntity& other) :
	m_Co(other.m_Co),
	m_sName(other.m_sName)
{
}

CInGameEntity& CInGameEntity::operator=(const CInGameEntity& other){
	if(this == &other )
		return *this;

	m_Co = other.m_Co;
	m_sName = other.m_sName;

	return *this;
};

CInGameEntity::~CInGameEntity(void)
{
	Reset();
}

/// Resetfunktion für die Klasse CMapTile
void CInGameEntity::Reset()
{
	m_sName.Empty();
}

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////
void CInGameEntity::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		ar << m_Co;
		ar << m_sName;
	}
	else
	{
		ar >> m_Co;
		ar >> m_sName;
	}
}