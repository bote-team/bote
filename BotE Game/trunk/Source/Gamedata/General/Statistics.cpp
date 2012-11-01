#include "stdafx.h"
#include "Statistics.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"
#include "AI\SectorAI.h"
#include "Ships\Fleet.h"
#include <algorithm>

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
		for (int i = TITAN; i <= DERITIUM; i++)
			ar << m_nAverageResourceStorages[i];
		ar << m_mShipPowers.size();
		for (map<CString, UINT>::const_iterator it = m_mShipPowers.begin(); it != m_mShipPowers.end(); ++it)
			ar << it->first << it->second;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_byAverageTechLevel;
		for (int i = TITAN; i <= DERITIUM; i++)
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

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsBSP(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	std::map<CString, float> mMap;
	// es werden alle Creditseinnahmen aller Systeme betrachtet
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).GetOwnerOfSystem() != "")
				mMap[pDoc->GetSystem(x,y).GetOwnerOfSystem()] += pDoc->GetSystem(x,y).GetProduction()->GetCreditsProd();

	CalcDemoValues(sRaceID, &mMap, nPlace, fValue, fAverage, fFirst, fLast);
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsProductivity(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	std::map<CString, float> mMap;
	// Es wird die komplette Industrieproduktion und Ressourcenproduktion aller Rassen betrachtet
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).GetOwnerOfSystem() != "")
			{
				float fResProd = 0.0f;
				for (int i = TITAN; i <= IRIDIUM; i++)
					fResProd += pDoc->GetSystem(x,y).GetProduction()->GetResourceProd(i);
				fResProd /= 2.5;
				mMap[pDoc->GetSystem(x,y).GetOwnerOfSystem()] += pDoc->GetSystem(x,y).GetProduction()->GetIndustryProd() + fResProd;
			}

	CalcDemoValues(sRaceID, &mMap, nPlace, fValue, fAverage, fFirst, fLast);
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsMilitary(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	std::map<CString, float> mMap;
	// Map mit allen vorhandenen Majors und einem NULL Wert füllen, so dass auch Majors ohne Militär in der
	// Liste aufgeführt sind
	std::map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
	for (std::map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		mMap[it->first] = 0.0f;

	// Es werden alle Schiffe aller Rassen betrachtet
	for (int i = 0; i < pDoc->m_ShipArray.GetSize(); i++)
	{
		CShip* pShip = &pDoc->m_ShipArray[i];
		// Stationen und Alienschiffe werden nicht mit einbezogen
		if (!pShip->IsBase() && pShip->GetShipType() != SHIP_TYPE::ALIEN)
			mMap[pShip->GetOwnerOfShip()] += pShip->GetCompleteOffensivePower() + pShip->GetCompleteOffensivePower() / 2;
		// Schiffe in der Flotte beachten
		if (pShip->GetFleet())
		{
			for (int j = 0; j < pShip->GetFleet()->GetFleetSize(); j++)
			{
				CShip* pFleetShip = pShip->GetFleet()->GetShipFromFleet(j);
				if (!pFleetShip->IsBase() && pFleetShip->GetShipType() != SHIP_TYPE::ALIEN)
					mMap[pFleetShip->GetOwnerOfShip()] += pFleetShip->GetCompleteOffensivePower() + pFleetShip->GetCompleteOffensivePower() / 2;
			}
		}
	}

	CalcDemoValues(sRaceID, &mMap, nPlace, fValue, fAverage, fFirst, fLast);
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsResearch(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	std::map<CString, float> mMap;
	// Es wird die komplette Industrieproduktion aller Rassen betrachtet
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).GetOwnerOfSystem() != "")
				mMap[pDoc->GetSystem(x,y).GetOwnerOfSystem()] += pDoc->GetSystem(x,y).GetProduction()->GetResearchProd();

	CalcDemoValues(sRaceID, &mMap, nPlace, fValue, fAverage, fFirst, fLast);
}

/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
/// @param sRaceID gewünschte Rasse
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::GetDemographicsMoral(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	std::map<CString, float> mMap;
	std::map<CString, int> mCount;
	// Es wird die komplette Industrieproduktion aller Rassen betrachtet
	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (pDoc->GetSystem(x,y).GetOwnerOfSystem() != "")
			{
				mMap[pDoc->GetSystem(x,y).GetOwnerOfSystem()] += pDoc->GetSystem(x,y).GetMoral();
				mCount[pDoc->GetSystem(x,y).GetOwnerOfSystem()] += 1;
			}

	for (std::map<CString, float>::iterator it = mMap.begin(); it != mMap.end(); ++it)
		it->second /= mCount[it->first];

	CalcDemoValues(sRaceID, &mMap, nPlace, fValue, fAverage, fFirst, fLast);
}

/// Funktion gibt die aktuellen Spielpunkte einer Rasse zurück.
int CStatistics::GetGamePoints(const CString& sRaceID, int nCurrentRound, float fDifficultyLevel) const
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

	return max(0, nGamePoints);
}

/// Funktion errechnet eine sortierte Liste der aktuellen Topsysteme.
/// @param nLimit Anzahl der zu errechnenden Topsystem (z.B. Top 5)
/// @param [out] lSystems Liste in welche die Koordinaten der Topsysteme abgelegt werden.
void CStatistics::GetTopSystems(int nLimit, std::list<CPoint>& lSystems) const
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

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
			if (pDoc->GetSystem(x,y).GetOwnerOfSystem() != "")
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
	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
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
	for (int i = TITAN; i <= DERITIUM; i++)
	{
		m_nAverageResourceStorages[i] = 0;
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
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

	for (int i = TITAN; i <= DERITIUM; i++)
		m_nAverageResourceStorages[i] = 0;

	m_mShipPowers.clear();
}

/// Funktion zum Berechnen der gesamten militärischen Schiffsstärken aller Rassen.
/// @param pDoc Zeiger auf das Dokument
void CStatistics::CalcShipPowers(CBotf2Doc* pDoc)
{
	map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();

	for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		m_mShipPowers[it->first] = pDoc->GetSectorAI()->GetCompleteDanger(it->first);
}

/// @param sRaceID ID der gewünschten Rasse
/// @param pmMap auszuwertende Map
/// @param [out] nPlace Platzierung
/// @param [out] fValue eigener Wert
/// @param [out] fAverage Durchschnittswert
/// @param [out] fFirst bester Wert
/// @param [out] fLast schlechtester Wert
void CStatistics::CalcDemoValues(const CString& sRaceID, const std::map<CString, float>* pmMap, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast) const
{
	nPlace = 1;
	fValue = fAverage = fFirst = fLast = 0.0f;

	std::vector<float> vSortedVec;
	for (std::map<CString, float>::const_iterator it = pmMap->begin(); it != pmMap->end(); ++it)
		vSortedVec.push_back(it->second);
	std::sort(vSortedVec.begin(), vSortedVec.end());
	std::reverse(vSortedVec.begin(), vSortedVec.end());

	if (vSortedVec.empty())
		return;

	// Platz ermitteln
	std::map<CString, float>::const_iterator it = pmMap->find(sRaceID);
	if (it == pmMap->end())
		return;

	for (UINT i = 0; i < vSortedVec.size(); i++)
		if (vSortedVec[i] == it->second)
		{
			nPlace = ++i;
			break;
		}

	// Durchschnitt ermitteln
	for (UINT i = 0; i < vSortedVec.size(); i++)
		fAverage += vSortedVec[i];
	fAverage /= vSortedVec.size();

	// eigener Wert
	fValue = it->second;
	// bester Wert
	fFirst = vSortedVec.front();
	// schlechtester Wert
	fLast = vSortedVec.back();
}
