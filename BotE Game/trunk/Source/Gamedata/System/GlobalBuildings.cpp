#include "stdafx.h"
#include "GlobalBuildings.h"
#include <list>

IMPLEMENT_SERIAL (CGlobalBuildings, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CGlobalBuildings::CGlobalBuildings(void)
{
}

CGlobalBuildings::~CGlobalBuildings(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CGlobalBuildings::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_mGlobalBuildings.size();
		for (std::map<CString, std::list<USHORT> >::const_iterator it = m_mGlobalBuildings.begin(); it != m_mGlobalBuildings.end(); ++it)
		{
			ar << it->first;
			ar << it->second.size();
			for (std::list<USHORT>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
				ar << *it2;
		}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		m_mGlobalBuildings.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			size_t listSize = 0;
			ar >> key;
			ar >> listSize;
			for (size_t j = 0; j < listSize; j++)
			{
				USHORT value;
				ar >> value;
				m_mGlobalBuildings[key].push_back(value);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
int CGlobalBuildings::GetCountGlobalBuilding(const CString& sRaceID, USHORT nID) const
{
	std::map<CString, std::list<USHORT> >::const_iterator it = m_mGlobalBuildings.find(sRaceID);
	if (it == m_mGlobalBuildings.end())
		return 0;

	int nCount = 0;
	for (std::list<USHORT>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		if (*it2 == nID)
			nCount++;

	return nCount;
}

void CGlobalBuildings::DeleteGlobalBuilding(const CString& sRaceID, USHORT nID)
{
	std::map<CString, std::list<USHORT> >::iterator it = m_mGlobalBuildings.find(sRaceID);
	if (it == m_mGlobalBuildings.end())
		return;

	std::list<USHORT> lGlobalBuildings = m_mGlobalBuildings[sRaceID];
	for (std::list<USHORT>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
	{
		if (*it2 == nID)
		{
			it->second.erase(it2);
			return;
		}
	}
}

// Resetfunktion für die Klasse CGlobalBuildings
void CGlobalBuildings::Reset()
{
	m_mGlobalBuildings.clear();
}
