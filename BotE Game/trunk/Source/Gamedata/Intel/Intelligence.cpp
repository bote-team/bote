#include "stdafx.h"
#include "Intelligence.h"

IMPLEMENT_SERIAL (CIntelligence, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CIntelligence::CIntelligence(void)
{
	Reset();
}

CIntelligence::~CIntelligence(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CIntelligence::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	m_Assignment.Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_sRace;
		ar << m_sResponsibleRace;
		ar << m_iSecurityPoints;
		ar << m_iInnerStorage;
		ar << m_nInnerSecurityBoni;

		for (int i = 0; i < 2; i++)
		{
			ar << m_iSPStorage[i].size();
			for (map<CString, UINT>::const_iterator it = m_iSPStorage[i].begin(); it != m_iSPStorage[i].end(); ++it)
			{
				ar << it->first << it->second;
			}
			ar << m_byAggressiveness[i].size();
			for (map<CString, BYTE>::const_iterator it = m_byAggressiveness[i].begin(); it != m_byAggressiveness[i].end(); ++it)
			{
				ar << it->first << it->second;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			ar << m_nEconomyBoni[i];
			ar << m_nScienceBoni[i];
			ar << m_nMilitaryBoni[i];
		}
	}
	else if (ar.IsLoading())
	{
		ar >> m_sRace;
		ar >> m_sResponsibleRace;
		ar >> m_iSecurityPoints;
		ar >> m_iInnerStorage;
		ar >> m_nInnerSecurityBoni;

		for (int i = 0; i < 2; i++)
		{
			// alte Maps löschen
			m_iSPStorage[i].clear();
			m_byAggressiveness[i].clear();

			size_t mapSize = 0;

			ar >> mapSize;
			for (size_t t = 0; t < mapSize; t++)
			{
				CString key;
				UINT value = 0;
				ar >> key;
				ar >> value;
				m_iSPStorage[i][key] = value;
			}
			ar >> mapSize;
			for (size_t t = 0; t < mapSize; t++)
			{
				CString key;
				BYTE value = 0;
				ar >> key;
				ar >> value;
				m_byAggressiveness[i][key] = value;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			ar >> m_nEconomyBoni[i];
			ar >> m_nScienceBoni[i];
			ar >> m_nMilitaryBoni[i];
		}
	}

	// Man kann die Textnachrichten in einem Report löschen, wenn sie eh nicht an die betroffene Rasse gerichtet sind
	// dies spart Speicherplatz
	for (int i = 0; i < m_Reports.GetNumberOfReports(); i++)
	{
		if (m_Reports.GetReport(i)->GetOwner() == m_sRace)
			m_Reports.GetReport(i)->SetEnemyDesc("");
		else
			m_Reports.GetReport(i)->SetOwnersDesc("");
	}
	m_Reports.Serialize(ar);

	// Wird nicht serialisiert, da es nur bei Bedarf vom jeweiligen Client neu berechnet wird. Dafür werden hier aber alle
	// Geheimdienstinformationen reseted.
	m_IntelInfo.Reset();
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion addiert einen Wert zum vorhandenen Wert der inneren Sicherheit. Funktion überprüft auch, ob der neue
/// Wert im richtigen Bereich liegt.
/// Funktion gibt einen Bonus auf ein Geheimdienstressort zurück.
short CIntelligence::GetBonus(BYTE bonus, BOOLEAN type) const
{
	switch (bonus)
	{
		// Wirtschaft
		case 0: return this->GetEconomyBonus(type); break;
		// Forschung
		case 1: return this->GetScienceBonus(type); break;
		// Militär
		case 2: return this->GetMilitaryBonus(type); break;
		// Diplomatie
		case 3: return 0; break;
		// innere Sicherheit
		case 4: return this->GetInnerSecurityBoni(); break;
		default: return 0;
	}
	return 0;
}

void CIntelligence::AddInnerSecurityPoints(int add)
{
	if (((long)m_iInnerStorage + add) < NULL)
		m_iInnerStorage = NULL;
	else
		m_iInnerStorage += add;
}

/// Funktion addiert die übergebenen Punkte zum jeweiligen Lager einer Rasse. Gleichzeitig wird überprüft, dass
/// ihr neuer Wert im richtigen Bereich liegt.
void CIntelligence::AddSPStoragePoints(BOOLEAN type, const CString& sRace, int add)
{
	if (((long)m_iSPStorage[type][sRace] + add) < NULL)
		m_iSPStorage[type][sRace] = NULL;
	else
		m_iSPStorage[type][sRace] += add;
}

/// Funktion addiert den im Paramter übergebenen Wert zu den aktuell produzierten Geheimdienstpunkten.
void CIntelligence::AddSecurityPoints(int add)
{
	if (((long)m_iSecurityPoints + add) < NULL)
		m_iSecurityPoints = NULL;
	else
		m_iSecurityPoints += add;
}

/// Funktion löscht alle Geheimdienstboni.
void CIntelligence::ClearBoni()
{
	m_nInnerSecurityBoni = 0;
	memset(m_nEconomyBoni, 0, sizeof(m_nEconomyBoni));
	memset(m_nScienceBoni, 0, sizeof(m_nScienceBoni));
	memset(m_nMilitaryBoni, 0, sizeof(m_nMilitaryBoni));
}

/// Resetfunktion für das CIntelligence-Objekt.
void CIntelligence::Reset()
{
	for (int i = 0; i < 2; i++)
	{
		m_iSPStorage[i].clear();
		m_byAggressiveness[i].clear();
	}
	m_sRace = "";
	m_sResponsibleRace = "";
	m_iSecurityPoints = 0;
	m_iInnerStorage = 0;
	m_Assignment.Reset();
	m_Reports.RemoveAllReports();
	m_IntelInfo.Reset();

	ClearBoni();
}
