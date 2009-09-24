#include "stdafx.h"
#include "Statistics.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"
#include "SectorAI.h"

IMPLEMENT_SERIAL (CStatistics, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CStatistics::CStatistics(void)
{
}

CStatistics::~CStatistics(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CStatistics::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_byAverageTechLevel;
		for (int i = TITAN; i <= DILITHIUM; i++)
			ar << m_nAverageResourceStorages[i];
		ar << m_mShipPowers.size();
		for (map<CString, UINT>::const_iterator it = m_mShipPowers.begin(); it != m_mShipPowers.end(); it++)
			ar << it->first << it->second;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_byAverageTechLevel;
		for (int i = TITAN; i <= DILITHIUM; i++)
			ar >> m_nAverageResourceStorages[i];
		
		m_mShipPowers.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			UINT value;
			ar >> key;
			ar >> value;
			m_mShipPowers[key] = value;
		}
	}
}
//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion zum Berechnen aller Statistiken.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcStats(CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	this->Reset();

	this->CalcAverageTechLevel(pDoc);
	
	this->CalcAverageResourceStorages(pDoc);

	this->CalcShipPowers(pDoc);
}

/// Funktion gibt Map mit den Schiffsstärken aller Rassen zurück.
/// @param sRaceID Rassen-ID für die die Schiffsstärke erfragt werden soll
/// @return Schiffsstärke der Kriegsschiffe
UINT CStatistics::GetShipPower(const CString& sRaceID) const
{
	map<CString, UINT>::const_iterator it = m_mShipPowers.find(sRaceID);

	if (it != m_mShipPowers.end())
		return it->second;
	else
		MYTRACE(MT::LEVEL_WARNING, "CStatistics::GetShipPower: getting Shippower from race %s", sRaceID);

	return 0;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum Berechnen des universumweiten Techdurchschnittlevels.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcAverageTechLevel(CBotf2Doc* pDoc)
{
	// Wieviel Imperien gibt es noch? Hier anhand der Anzahl der Systeme geschaut
	USHORT nRaces = 0;
	m_byAverageTechLevel = 0;

	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
	{
		CEmpire* empire = it->second->GetEmpire();
		if (empire->GetNumberOfSystems() > 0)
		{
			m_byAverageTechLevel += empire->GetResearch()->GetBioTech();
			m_byAverageTechLevel += empire->GetResearch()->GetEnergyTech();
			m_byAverageTechLevel += empire->GetResearch()->GetCompTech();
			m_byAverageTechLevel += empire->GetResearch()->GetPropulsionTech();
			m_byAverageTechLevel += empire->GetResearch()->GetConstructionTech();
			m_byAverageTechLevel += empire->GetResearch()->GetWeaponTech();
			nRaces++;
		}
	}
	if (nRaces)
		m_byAverageTechLevel = m_byAverageTechLevel / (6 * nRaces);
}

/// Funktion zum Berechnen der durchschnittlichen Befüllung der Ressourcenlager.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcAverageResourceStorages(CBotf2Doc* pDoc)
{
	// Wieviel Imperien gibt es noch? Hier anhand der Anzahl der Systeme geschaut
	USHORT nRaces = 0;
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (int i = TITAN; i <= DILITHIUM; i++)
	{
		m_nAverageResourceStorages[i] = 0;
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
		{
			CEmpire* empire = it->second->GetEmpire();
			if (empire->GetNumberOfSystems() > 0)
			{
				m_nAverageResourceStorages[i] += empire->GetStorage()[i];
				nRaces++;
			}
		}
		if (nRaces)
			m_nAverageResourceStorages[i] /= nRaces;
	}
}

/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
void CStatistics::Reset(void)
{
	m_byAverageTechLevel = 0;

	for (int i = TITAN; i <= DILITHIUM; i++)
		m_nAverageResourceStorages[i] = 0;

	m_mShipPowers.clear();
}

/// Funktion zum Berechnen der gesamten militärischen Schiffsstärken aller Rassen.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcShipPowers(CBotf2Doc* pDoc)
{
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); it++)
		m_mShipPowers[it->first] = pDoc->GetSectorAI()->GetCompleteDanger(it->first);
}