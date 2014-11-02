#include "stdafx.h"
#include "Statistics.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"
#include "AI\SectorAI.h"
#include <algorithm>
#include "Ships/Ships.h"

IMPLEMENT_SERIAL (CStatistics, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CStatistics::CStatistics(void) :
m_byAverageTechLevel(0)
{
	memset(m_nAverageResourceStorages, 0, sizeof(m_nAverageResourceStorages));
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
		for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
			ar << m_nAverageResourceStorages[i];
		ar << m_mShipPowers.size();
		for (map<CString, UINT>::const_iterator it = m_mShipPowers.begin(); it != m_mShipPowers.end(); ++it)
			ar << it->first << it->second;
		ar << m_Marks.size();
		for (std::map<CStatistics::DEMO_TYPE, std::map<CString, float>>::const_iterator it = m_Marks.begin();
			it != m_Marks.end(); ++it)
		{
			ar << it->first;
			ar << it->second.size();
			for(std::map<CString, float>::const_iterator itt = it->second.begin(); itt != it->second.end();
				++itt)
				ar << itt->first << itt->second;
		}
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_byAverageTechLevel;
		for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
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

		m_Marks.clear();
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; ++i)
		{
			int key;
			ar >> key;
			size_t inner_map_size = 0;
			ar >> inner_map_size;
			for(size_t j = 0; j < inner_map_size; ++j)
			{
				CString inner_key;
				ar >> inner_key;
				float value;
				ar >> value;
				m_Marks[static_cast<CStatistics::DEMO_TYPE>(key)][inner_key] = value;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion zum Berechnen aller Statistiken.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcStats(CBotEDoc* pDoc)
{
	AssertBotE(pDoc);

	this->Reset();

	this->CalcAverageTechLevel(pDoc);

	this->CalcAverageResourceStorages(pDoc);

	this->CalcShipPowers(pDoc);

	this->CalcMarks();
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsBSP(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	std::map<CString, float> mMap;
	// es werden alle Creditseinnahmen aller Systeme betrachtet
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).Majorized())
				mMap[pDoc->GetSystem(x,y).OwnerID()] += pDoc->GetSystem(x,y).GetProduction()->GetCreditsProd();

	std::map<CString, float> marks;
			MYTRACE("general")(MT::LEVEL_INFO, "Demographics - BSP: multiply with 5 ");
	CalcDemoValues(sRaceID, &mMap, marks, nPlace, fValue, fAverage, fFirst, fLast);
	m_Marks[CStatistics::BSP] = marks;
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsProductivity(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	std::map<CString, float> mMap;
	// Es wird die komplette Industrieproduktion und Ressourcenproduktion aller Rassen betrachtet
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).Majorized())
			{
				float fResProd = 0.0f;
				for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
					fResProd += pDoc->GetSystem(x,y).GetProduction()->GetResourceProd(i);
				fResProd /= 2.5;
				mMap[pDoc->GetSystem(x,y).OwnerID()] += pDoc->GetSystem(x,y).GetProduction()->GetIndustryProd() + fResProd;
			}

			MYTRACE("general")(MT::LEVEL_INFO, "Demographics - Productivity: 1:1");

	std::map<CString, float> marks;
	CalcDemoValues(sRaceID, &mMap, marks, nPlace, fValue, fAverage, fFirst, fLast);
	m_Marks[CStatistics::PRODUCTIVITY] = marks;
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsMilitary(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	std::map<CString, float> mMap;
	// Map mit allen vorhandenen Majors und einem NULL Wert füllen, so dass auch Majors ohne Militär in der
	// Liste aufgeführt sind
	std::map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (std::map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		mMap[it->first] = 0.0f;

	// Es werden alle Schiffe aller Rassen betrachtet
	for(CShipMap::const_iterator i = pDoc->m_ShipMap.begin(); i != pDoc->m_ShipMap.end(); ++i)
	{
		// Stationen und Alienschiffe werden nicht mit einbezogen
		if (!i->second->IsStation() && !i->second->IsAlien())
			mMap[i->second->OwnerID()] += i->second->GetCompleteOffensivePower() + i->second->GetCompleteOffensivePower() / 2;
		// Schiffe in der Flotte beachten
		for(CShips::const_iterator j = i->second->begin(); j != i->second->end(); ++j)
			if (!j->second->IsStation() && !j->second->IsAlien())
				mMap[j->second->OwnerID()] += j->second->GetCompleteOffensivePower() + j->second->GetCompleteOffensivePower() / 2;
	}

MYTRACE("general")(MT::LEVEL_INFO, "Demographics - Military: divide by 10");
	std::map<CString, float> marks;
	CalcDemoValues(sRaceID, &mMap, marks, nPlace, fValue, fAverage, fFirst, fLast);
	m_Marks[CStatistics::MILITARY] = marks;
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsResearch(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	std::map<CString, float> mMap;
	// Es wird die komplette Industrieproduktion aller Rassen betrachtet
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).Majorized())
				mMap[pDoc->GetSystem(x,y).OwnerID()] += pDoc->GetSystem(x,y).GetProduction()->GetResearchProd();

	MYTRACE("general")(MT::LEVEL_INFO, "Demographics - Research: multiply with 5");
	std::map<CString, float> marks;
	CalcDemoValues(sRaceID, &mMap, marks, nPlace, fValue, fAverage, fFirst, fLast);
	m_Marks[CStatistics::RESEARCH] = marks;
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsMoral(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	std::map<CString, float> mMap;
	std::map<CString, int> mCount;
	// Es wird die komplette Industrieproduktion aller Rassen betrachtet
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).Majorized())
			{
				mMap[pDoc->GetSystem(x,y).OwnerID()] += pDoc->GetSystem(x,y).GetMoral();
				mCount[pDoc->GetSystem(x,y).OwnerID()] += 1;
			}

	for (std::map<CString, float>::iterator it = mMap.begin(); it != mMap.end(); ++it)
		it->second /= mCount[it->first];

	MYTRACE("general")(MT::LEVEL_INFO, "Demographics - Moral: 1:1");
	std::map<CString, float> marks;
	CalcDemoValues(sRaceID, &mMap, marks, nPlace, fValue, fAverage, fFirst, fLast);
	m_Marks[CStatistics::MORAL] = marks;
}

/// Funktion gibt die aktuellen Spielpunkte einer Rasse zurück.
int CStatistics::GetGamePoints(const CString& sRaceID, int nCurrentRound, float fDifficultyLevel)
{
	int nGamePoints = 0;

	int nPlace;
	float fValue, fAverage, fFirst, fLast;

	GetDemographicsBSP(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	nGamePoints += (int)(fValue * 5);

	GetDemographicsProductivity(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	nGamePoints += (int)(fValue);

	GetDemographicsMilitary(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	nGamePoints += (int)(fValue / 10);

	GetDemographicsResearch(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	nGamePoints += (int)(fValue * 2);

	// aller 10 Runden verringert sich die Punktzahl um 1%
	float fTemp = nCurrentRound / 10.0;
	nGamePoints -= nGamePoints * fTemp / 100.0;

	// Schwierigkeitsgrad einberechnen
	nGamePoints /= fDifficultyLevel;

	nGamePoints /= 100;
	MYTRACE("general")(MT::LEVEL_DEBUG, "nGamePoints: %f\n", nGamePoints);

	return max(0, nGamePoints);
}

/// Funktion errechnet eine sortierte Liste der aktuellen Topsysteme.
/// @param nLimit Anzahl der zu errechnenden Topsystem (z.B. Top 5)
/// @param [out] lSystems Liste in welche die Koordinaten der Topsysteme abgelegt werden.
void CStatistics::GetTopSystems(int nLimit, std::list<CPoint>& lSystems) const
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	lSystems.clear();

	// Topsysteme ermitteln
	// Dafür alle Systeme durchgehen und Liste mit deren Werten erstellen
	struct SYSTEMLIST
	{
		CPoint	m_ptKO;
		int		m_nValue;

		bool operator< (const SYSTEMLIST& elem2) const { return m_nValue < elem2.m_nValue;}

		SYSTEMLIST() : m_ptKO(-1,-1), m_nValue(0) {}
		SYSTEMLIST(const CPoint& ptKO, int nValue) : m_ptKO(ptKO), m_nValue(nValue) {}
	};

	list<SYSTEMLIST> lSystemList;
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
	{
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		{
			if (pDoc->GetSystem(x,y).Majorized())
			{
				// Wert berechnen
				int nValue = 0;

				// Nahrung
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetMaxFoodProd() / 4;
				// Industrie
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetIndustryProd();
				// Energie
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetMaxEnergyProd();
				// Geheimdienst
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetSecurityProd();
				// Forschung
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetResearchProd();
				// Titan
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetTitanProd() / 2;
				// Deuterium
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetDeuteriumProd() / 3;
				// Duranium
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetDuraniumProd();
				// Kristalle
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetCrystalProd() * 1.5;
				// Iridium
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetIridiumProd() * 2;
				// Deritium
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetDeritiumProd() * 100;
				// Credits
				nValue += pDoc->GetSystem(x,y).GetProduction()->GetCreditsProd() * 3;

				lSystemList.push_back(SYSTEMLIST(CPoint(x,y), nValue));
			}
		}
	}
	// Liste aufsteigend sortieren
	lSystemList.sort();
	lSystemList.reverse();
	// nur die ersten gewünschten Einträge interessieren
	lSystemList.resize(nLimit);

	for (list<SYSTEMLIST>::const_iterator it = lSystemList.begin(); it != lSystemList.end(); ++it)
		lSystems.push_back(it->m_ptKO);
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
		MYTRACE("general")(MT::LEVEL_WARNING, "CStatistics::GetShipPower: getting Shippower from race %s", sRaceID);

	return 0;
}

float CStatistics::GetMark(const CString& race) const
{
	float result = 0;
	for (std::map<CStatistics::DEMO_TYPE, std::map<CString, float>>::const_iterator it = m_Marks.begin();
		it != m_Marks.end(); ++it)
	{
		const std::map<CString, float>::const_iterator itt = it->second.find(race);
		result += itt->second;
	}
	result /= (CStatistics::MORAL + 1);
	return result;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum Berechnen des universumweiten Techdurchschnittlevels.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcAverageTechLevel(CBotEDoc* pDoc)
{
	// Wieviel Imperien gibt es noch? Hier anhand der Anzahl der Systeme geschaut
	USHORT nRaces = 0;
	m_byAverageTechLevel = 0;

	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
	{
		CEmpire* empire = it->second->GetEmpire();
		if (empire->CountSystems() > 0)
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

				MYTRACE("general")(MT::LEVEL_DEBUG, "m_byAverageTechLevel: %f ", m_byAverageTechLevel);
}

/// Funktion zum Berechnen der durchschnittlichen Befüllung der Ressourcenlager.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcAverageResourceStorages(CBotEDoc* pDoc)
{
	// Wieviel Imperien gibt es noch? Hier anhand der Anzahl der Systeme geschaut
	USHORT nRaces = 0;
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
	{
		m_nAverageResourceStorages[i] = 0;
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			CEmpire* empire = it->second->GetEmpire();
			if (empire->CountSystems() > 0)
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

	for (int i = RESOURCES::TITAN; i <= RESOURCES::DERITIUM; i++)
		m_nAverageResourceStorages[i] = 0;

	m_mShipPowers.clear();
}

/// Funktion zum Berechnen der gesamten militärischen Schiffsstärken aller Rassen.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcShipPowers(CBotEDoc* pDoc)
{
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		m_mShipPowers[it->first] = pDoc->GetSectorAI()->GetCompleteDanger(it->first);
}

static bool Compare(const std::pair<CString, float>& left, const std::pair<CString, float>& right)
{
	return left.second > right.second;
}

/// @param sRaceID ID der gewünschten Rasse
/// @param pmMap auszuwertende Map
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::CalcDemoValues(const CString& sRaceID, const std::map<CString, float>* pmMap, std::map<CString, float>& marks, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const
{
	nPlace = 1;
	fValue = fAverage = fFirst = fLast = 0.0f;

	std::vector<std::pair<CString, float>> vSortedVec;
	for (std::map<CString, float>::const_iterator it = pmMap->begin(); it != pmMap->end(); ++it)
		vSortedVec.push_back(*it);
	std::sort(vSortedVec.begin(), vSortedVec.end(), Compare);

	if (vSortedVec.empty())
		return;

	for(std::vector<std::pair<CString, float>>::const_iterator it = vSortedVec.begin();
			it != vSortedVec.end(); ++it)
		marks[it->first] = it - vSortedVec.begin() + 1;

	// Platz ermitteln
	std::map<CString, float>::const_iterator it = pmMap->find(sRaceID);
	if (it == pmMap->end())
		return;

	nPlace = marks[sRaceID];

	// Durchschnitt ermitteln
	for (UINT i = 0; i < vSortedVec.size(); i++)
		fAverage += vSortedVec[i].second;
	fAverage /= vSortedVec.size();

	// eigener Wert
	fValue = it->second;

	// bester Wert
	fFirst = vSortedVec.front().second;

	// schlechtester Wert
	fLast = vSortedVec.back().second;

	MYTRACE("general")(MT::LEVEL_INFO, "First: %f ", fFirst);
	MYTRACE("general")(MT::LEVEL_INFO, "Average: %f ", fAverage);
	MYTRACE("general")(MT::LEVEL_INFO, "Last: %f", fLast);
	MYTRACE("general")(MT::LEVEL_INFO, "Value (own): %f ", fValue);
	MYTRACE("general")(MT::LEVEL_INFO, "Place: %i \n", nPlace);
}

void CStatistics::CalcMarks()
{
	// Bewertung Gesamt berechnen
	int nPlace = 1;
	float fValue, fAverage, fFirst, fLast;
	CString sRaceID;

	GetDemographicsBSP(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	GetDemographicsProductivity(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	GetDemographicsMilitary(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	GetDemographicsResearch(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);
	GetDemographicsMoral(sRaceID, nPlace, fValue, fAverage, fFirst, fLast);

}
