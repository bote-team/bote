// GenShipName.cpp: Implementierung der Klasse CGenShipName.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenShipName.h"
#include "Botf2.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

IMPLEMENT_SERIAL (CGenShipName, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CGenShipName::CGenShipName()
{	
}

CGenShipName::~CGenShipName()
{	
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CGenShipName::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_mShipNames.size();
		for (map<CString, vector<CString> >::iterator it = m_mShipNames.begin(); it != m_mShipNames.end(); it++)
		{
			ar << it->first;
			// nun den Vektor speichern
			ar << it->second.size();
			for (size_t i = 0; i < it->second.size(); i++)
				ar << it->second[i];
		}

		ar << m_mUsedNames.size();
		for (map<CString, vector<CString> >::iterator it = m_mUsedNames.begin(); it != m_mUsedNames.end(); it++)
		{
			ar << it->first;
			// nun den Vektor speichern
			ar << it->second.size();
			for (size_t i = 0; i < it->second.size(); i++)
				ar << it->second[i];
		}
	
		ar << m_mCounter.size();
		for (map<CString, USHORT>::const_iterator it = m_mCounter.begin(); it != m_mCounter.end(); it++)
			ar << it->first << it->second;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		m_mShipNames.clear();
		m_mUsedNames.clear();
		m_mCounter.clear();

		size_t mapSize = 0;
		ar >> mapSize;		
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			ar >> key;
			// Vektor empfangen
			vector<CString> value;
			size_t vectorSize = 0;
			ar >> vectorSize;
			for (size_t j = 0; j < vectorSize; j++)
			{
				CString s;
				ar >> s;
				value.push_back(s);
			}
			
			m_mShipNames[key] = value;
		}

		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			ar >> key;
			// Vektor empfangen
			vector<CString> value;
			size_t vectorSize = 0;
			ar >> vectorSize;
			for (size_t j = 0; j < vectorSize; j++)
			{
				CString s;
				ar >> s;
				value.push_back(s);
			}
			
			m_mUsedNames[key] = value;
		}

		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			USHORT value;
			ar >> key;
			ar >> value;
			m_mCounter[key] = value;
		}		
	}
}

/// Funktion zum einmaligen Einlesen der ganzen Schiffsnamen.
void CGenShipName::Init(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	m_mShipNames.clear();
	m_mUsedNames.clear();
	m_mCounter.clear();

	// nun alle Rassen durchgehen
	map<CString, CRace*>* mRaces = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(mRaces);

	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); it++)
	{
		CString sID = it->first;
		// für jede Rasse dem Counter festlegen
		m_mCounter[sID] = 64;		// 65 == ASCII-Wert für 'A'; hier eins weniger

		// Varibale vom Typ CStdioFile	
		CStdioFile file;
		// Name des zu öffnenden Files zusammensetzen (die RaceID ist da mit drin)
		CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\" + sID + "ShipNames.data";
		// Datei öffnen
		if (file.Open(fileName, CFile::modeRead | CFile::typeText))
		{
			// auf sInput wird die jeweilige Zeile eingelesen
			CString sInput;
			while (file.ReadString(sInput))
				m_mShipNames[sID].push_back(sInput);
			// Datei schließen
			file.Close();
		}
	}
}

/// Diese Funktion generiert einen einmaligen Schiffsnamen. Als Parameter werden dafür die Rasse <code>sRaceID</code>
/// und ein Parameter, welcher angibt ob es sich um eine Station handelt <code>station</code> übergeben.
CString CGenShipName::GenerateShipName(const CString& sRaceID, BOOLEAN station)
{
	if (m_mShipNames[sRaceID].size() == 0)
	{
		m_mShipNames.clear();
		for (UINT i = 0; i < m_mUsedNames[sRaceID].size(); i++)
			m_mShipNames[sRaceID].push_back(m_mUsedNames[sRaceID].at(i));
		
		m_mUsedNames[sRaceID].clear();
				
		m_mCounter[sRaceID] += 1;
		// Wenn der ASCII Wert vom Z erreicht wurde, so fängt es automatisch wieder mit A an
		if (m_mCounter[sRaceID] > 90)	// 90 entspricht Z
			m_mCounter[sRaceID] = 65;	// 65 entspricht A
		else if (m_mCounter[sRaceID] < 65)
			m_mCounter[sRaceID] = 65;

		// wenn keine Namen existieren
		if (m_mShipNames[sRaceID].size() == 0)
		{
			CString sName;
			sName.Format("%s %c", sRaceID, (char)m_mCounter[sRaceID]);
			return sName;
		}
	}
	
	int random = rand()%m_mShipNames[sRaceID].size();
	CString sName;
	
	if (!station)
	{
		if (m_mCounter[sRaceID] > 64)
			sName.Format("%s %c",m_mShipNames[sRaceID].at(random), (char)m_mCounter[sRaceID]);
		else
			sName.Format("%s",m_mShipNames[sRaceID].at(random));			
	}
	else
		sName.Format("%s Station", m_mShipNames[sRaceID].at(random));
	
	m_mUsedNames[sRaceID].push_back(m_mShipNames[sRaceID].at(random));
	m_mShipNames[sRaceID].erase(m_mShipNames[sRaceID].begin() + random);
	
	return sName;	
}
