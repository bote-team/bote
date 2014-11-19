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
		for(std::map<CString, float>::const_iterator it = m_Marks.begin(); it != m_Marks.end(); ++it)
			ar << it->first << it ->second;
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
			CString key;
			ar >> key;
			float value;
			ar >> value;
			m_Marks.insert(std::pair<CString, float>(key, value));
		}
	}

	m_Bsp.Serialize(ar);
	m_Productivity.Serialize(ar);
	m_Military.Serialize(ar);
	m_Research.Serialize(ar);
	m_Moral.Serialize(ar);
}
//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion zum Berechnen aller Statistiken.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcStats(const CBotEDoc* const pDoc)
{
	AssertBotE(pDoc);

	this->Reset();

	this->CalcAverageTechLevel(pDoc);

	this->CalcAverageResourceStorages(pDoc);

	this->CalcShipPowers(pDoc);

	this->CalcDemographicsSystem(*pDoc);
	this->CalcDemographicsMilitary(*pDoc);

	this->CalcMarks();
}

static void insert_or_increase(std::map<CString, float>& m, float value, const CString& id)
{
	std::map<CString, float>::iterator found = m.find(id);
	if(found == m.end())
		m[id] = value;
	else
		found->second += value;
}

void CStatistics::CalcDemographicsSystem(const CBotEDoc& doc)
{
	std::map<CString, float> bsp;
	std::map<CString, float> productivity;
	std::map<CString, float> research;
	std::map<CString, float> moral;
	std::map<CString, float> count;
	const std::vector<CSystem>& systems = doc.m_Systems;
	for(std::vector<CSystem>::const_iterator it = systems.begin(); it != systems.end(); ++it)
	{
		if(!it->Majorized())
			continue;

		const CSystemProd& prod = *it->GetProduction();
		insert_or_increase(bsp, prod.GetCreditsProd(), it->OwnerID());
		float fResProd = 0.0f;
		for (int i = RESOURCES::TITAN; i <= RESOURCES::IRIDIUM; i++)
			fResProd += prod.GetResourceProd(i);
		fResProd /= 2.5;
		insert_or_increase(productivity, prod.GetIndustryProd() + fResProd, it->OwnerID());
		insert_or_increase(research, prod.GetResearchProd(), it->OwnerID());
		insert_or_increase(moral, it->GetMoral(), it->OwnerID());
		insert_or_increase(count, 1, it->OwnerID());
	}
	for (std::map<CString, float>::iterator it = moral.begin(); it != moral.end(); ++it)
		it->second /= count[it->first];

	InternalCalcDemographics(m_Bsp, bsp);
	InternalCalcDemographics(m_Productivity, productivity);
	InternalCalcDemographics(m_Research, research);
	InternalCalcDemographics(m_Moral, moral);
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
	nPlace = m_Bsp.places[sRaceID];
	fValue = m_Bsp.values[sRaceID];
	fAverage = m_Bsp.average;
	fFirst = m_Bsp.first;
	fLast = m_Bsp.last;
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
	nPlace = m_Productivity.places[sRaceID];
	fValue = m_Productivity.values[sRaceID];
	fAverage = m_Productivity.average;
	fFirst = m_Productivity.first;
	fLast = m_Productivity.last;
}

void CStatistics::CalcDemographicsMilitary(const CBotEDoc& doc)
{
	std::map<CString, float> military;
	// Map mit allen vorhandenen Majors und einem NULL Wert füllen, so dass auch Majors ohne Militär in der
	// Liste aufgeführt sind
	const std::map<CString, CMajor*>& pmMajors = *doc.GetRaceCtrl()->GetMajors();
	for (std::map<CString, CMajor*>::const_iterator it = pmMajors.begin(); it != pmMajors.end(); ++it)
		military[it->first] = 0.0f;

	// Es werden alle Schiffe aller Rassen betrachtet
	for(CShipMap::const_iterator i = doc.m_ShipMap.begin(); i != doc.m_ShipMap.end(); ++i)
	{
		// Stationen und Alienschiffe werden nicht mit einbezogen
		if(i->second->IsStation() || i->second->Owner()->IsMinor())
			continue;
		const unsigned power = i->second->GetCompleteOffensivePower(true, true, true);
		insert_or_increase(military, power + power / 2, i->second->OwnerID());
	}
	InternalCalcDemographics(m_Military, military);
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
	nPlace = m_Military.places[sRaceID];
	fValue = m_Military.values[sRaceID];
	fAverage = m_Military.average;
	fFirst = m_Military.first;
	fLast = m_Military.last;
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
	nPlace = m_Research.places[sRaceID];
	fValue = m_Research.values[sRaceID];
	fAverage = m_Research.average;
	fFirst = m_Research.first;
	fLast = m_Research.last;
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
	nPlace = m_Moral.places[sRaceID];
	fValue = m_Moral.values[sRaceID];
	fAverage = m_Moral.average;
	fFirst = m_Moral.first;
	fLast = m_Moral.last;
}

/// Funktion gibt die aktuellen Spielpunkte einer Rasse zurück.
int CStatistics::GetGamePoints(const CString& sRaceID, int nCurrentRound, float fDifficultyLevel)
{
	int nGamePoints = 0;

	AssertBotE(!m_Bsp.values.empty());
	nGamePoints += (int)(m_Bsp.values[sRaceID] * 5);
	nGamePoints += (int)(m_Productivity.values[sRaceID]);
	nGamePoints += (int)(m_Military.values[sRaceID] / 10);
	nGamePoints += (int)(m_Research.values[sRaceID] * 2);

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
	std::map<CString, float>::const_iterator it = m_Marks.find(race);
	AssertBotE(it != m_Marks.end());
	return it->second;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum Berechnen des universumweiten Techdurchschnittlevels.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcAverageTechLevel(const CBotEDoc* const pDoc)
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
void CStatistics::CalcAverageResourceStorages(const CBotEDoc* const pDoc)
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
	m_Marks.clear();
	m_Bsp.clear();
	m_Productivity.clear();
	m_Military.clear();
	m_Research.clear();
	m_Moral.clear();
}

/// Funktion zum Berechnen der gesamten militärischen Schiffsstärken aller Rassen.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcShipPowers(const CBotEDoc* const pDoc)
{
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		m_mShipPowers[it->first] = pDoc->GetSectorAI()->GetCompleteDanger(it->first);
}

static bool Compare(const std::pair<CString, float>& left, const std::pair<CString, float>& right)
{
	return left.second > right.second;
}

void CStatistics::InternalCalcDemographics(DEMOGRAPHICS_STORAGE& store, const std::map<CString, float>& m)
{
	store.values = m;

	std::vector<std::pair<CString, float>> vSortedVec;
	for (std::map<CString, float>::const_iterator it = m.begin(); it != m.end(); ++it)
		vSortedVec.push_back(*it);
	std::sort(vSortedVec.begin(), vSortedVec.end(), Compare);

	AssertBotE(!vSortedVec.empty());

	int current_rank = 1;
	float better_value = vSortedVec.begin()->second;
	//// Durchschnitt ermitteln, calculate ranks
	for(std::vector<std::pair<CString, float>>::const_iterator it = vSortedVec.begin();
			it != vSortedVec.end(); ++it)
	{
		if(it->second < better_value)
			current_rank++;
		store.places[it->first] = current_rank;
		store.average += it->second;
		better_value = it->second;
	}
	store.average /= vSortedVec.size();
	//// bester Wert
	store.first = vSortedVec.front().second;
	//// schlechtester Wert
	store.last = vSortedVec.back().second;
}

void CStatistics::CalcMarksForDemoType(const DEMOGRAPHICS_STORAGE& store, std::map<CString, float>& marks,
	bool do_average)
{
	const int count_of_majors = store.values.size();

	for(std::map<CString, float>::const_iterator it = store.values.begin(); it != store.values.end(); ++it)
	{
		const double first = store.first;
		const double last = store.last;
		float mark = 1.0f;
		if(!Equals(first, last))
			mark = (it->second - last) / (first - last) * (1.0f - count_of_majors) + count_of_majors;
		insert_or_increase(marks, mark, it->first);
		if(do_average)
			marks[it->first] /= (CStatistics::MORAL + 1.0f);
	}
}

void CStatistics::CalcMarks()
{
	CalcMarksForDemoType(m_Bsp, m_Marks, false);
	CalcMarksForDemoType(m_Productivity, m_Marks, false);
	CalcMarksForDemoType(m_Military, m_Marks, false);
	CalcMarksForDemoType(m_Research, m_Marks, false);
	CalcMarksForDemoType(m_Moral, m_Marks, true);
}

std::vector<std::pair<CString, float>> CStatistics::GetSortedMarks() const
{
	std::vector<std::pair<CString, float>> sorted_marks;
	for(std::map<CString, float>::const_iterator it = m_Marks.begin(); it != m_Marks.end(); ++it)
		sorted_marks.push_back(*it);
	std::sort(sorted_marks.begin(), sorted_marks.end(), Compare);
	std::reverse(sorted_marks.begin(), sorted_marks.end());
	return sorted_marks;
}
