// GenShipName.cpp: Implementierung der Klasse CGenShipName.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenShipName.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "IOData.h"

#include <sstream>
#include <cassert>

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
CGenShipName::CGenShipName(const CGenShipName& o) :
	m_mShipNames(o.m_mShipNames),
	m_mStillAvailableNames(o.m_mStillAvailableNames),
	m_mCounter(o.m_mCounter)
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
		for (map<CString, vector<CString> >::iterator it = m_mShipNames.begin(); it != m_mShipNames.end(); ++it)
		{
			ar << it->first;
			// nun den Vektor speichern
			ar << it->second.size();
			for (size_t i = 0; i < it->second.size(); i++)
				ar << it->second[i];
		}

		ar << m_mStillAvailableNames.size();
		for (map<CString, vector<CString> >::iterator it = m_mStillAvailableNames.begin(); it != m_mStillAvailableNames.end(); ++it)
		{
			ar << it->first;
			// nun den Vektor speichern
			ar << it->second.size();
			for (size_t i = 0; i < it->second.size(); i++)
				ar << it->second[i];
		}

		ar << m_mCounter.size();
		for (map<CString, unsigned>::const_iterator it = m_mCounter.begin(); it != m_mCounter.end(); ++it)
			ar << it->first << it->second;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		m_mShipNames.clear();
		m_mStillAvailableNames.clear();
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

			m_mStillAvailableNames[key] = value;
		}

		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			unsigned value;
			ar >> key;
			ar >> value;
			m_mCounter[key] = value;
		}
	}
}

/// Funktion zum einmaligen Einlesen der ganzen Schiffsnamen.
void CGenShipName::Init(CBotEDoc* pDoc)
{
	ASSERT(pDoc);

	m_mShipNames.clear();
	m_mStillAvailableNames.clear();
	m_mCounter.clear();

	// nun alle Rassen durchgehen
	map<CString, CRace*>* mRaces = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(mRaces);

	for (map<CString, CRace*>::const_iterator it = mRaces->begin(); it != mRaces->end(); ++it)
	{
		CString sID = it->first;
		// für jede Rasse dem Counter festlegen
		m_mCounter[sID] = 0;		// 65 == ASCII-Wert für 'A'

		// Varibale vom Typ CStdioFile
		CStdioFile file;
		// Name des zu öffnenden Files zusammensetzen (die RaceID ist da mit drin)
		CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Names\\" + sID + "ShipNames.data";
		// Datei öffnen
		if (file.Open(fileName, CFile::modeRead | CFile::typeText))
		{
			// auf sInput wird die jeweilige Zeile eingelesen
			CString sInput;
			while (file.ReadString(sInput)) {
				m_mShipNames[sID].push_back(sInput);
				m_mStillAvailableNames[sID].push_back(sInput);
			}
			// Datei schließen
			file.Close();
		}
	}
}

static std::string CStringToStdString(const CString& cs) {
	return std::string(static_cast<LPCTSTR>(cs));
}
static CString StdStringToCString(const std::string& s) {
	return CString(s.c_str());
}

/// Diese Funktion generiert einen einmaligen Schiffsnamen. Als Parameter werden dafür die Rasse <code>sRaceID</code>
/// und ein Parameter, welcher angibt ob es sich um eine Station handelt <code>station</code> übergeben.
CString CGenShipName::GenerateShipName(const CString& sRaceID, BOOLEAN station)
{
	std::vector<CString>& mStillAvailableNames = m_mStillAvailableNames[sRaceID];
	const std::vector<CString>& mShipNames = m_mShipNames[sRaceID];
	if(mStillAvailableNames.empty()) {
		m_mCounter[sRaceID] += 1;
		if(!mShipNames.empty())
			mStillAvailableNames = mShipNames;
	}

	const unsigned counter = m_mCounter[sRaceID];
	char letter[10] = "";
	if(1 <= counter) {
		int result = 0;
		if(counter <= 26)
			result = _snprintf(letter, 10, " %c", 64 + counter);//65 == A, 90 == Z
		else
			result = _snprintf(letter, 10, " %u", counter);
		assert(result > 0);
	}
	std::stringstream name;
	if(mStillAvailableNames.empty())
		name << CStringToStdString(sRaceID);
	else {
		const unsigned random = rand() % mStillAvailableNames.size();
		name << CStringToStdString(mStillAvailableNames.at(random));
		mStillAvailableNames.erase(mStillAvailableNames.begin() + random);
	}
	assert(name);
	name << letter;
	assert(name);
	if(station)
		name << " Station";
	assert(name);
	return StdStringToCString(name.str());
}
