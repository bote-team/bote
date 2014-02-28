#include "stdafx.h"
#include "VictoryObserver.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Races/RaceController.h"
#include "IniLoader.h"
#include <algorithm>

IMPLEMENT_SERIAL (CVictoryObserver, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CVictoryObserver::CVictoryObserver(void) :
	m_nRivalsLeft(INT_MAX),
	m_bIsVictory(false),
	m_nVictoryType(VICTORYTYPE_ELIMINATION)
{
	memset(m_bConditionStatus, true, sizeof(m_bConditionStatus));
}

CVictoryObserver::~CVictoryObserver(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CVictoryObserver::Serialize(CArchive &ar)
{
	__super::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		for (int i = 0; i < VICTORY_CONDITION_NUMBER; i++)
			ar << m_bConditionStatus[i];

		ar << m_mDiplomacy.size();
		for (map<CString, int>::const_iterator it = m_mDiplomacy.begin(); it != m_mDiplomacy.end(); ++it)
			ar << it->first << it->second;
		ar << m_mSlavery.size();
		for (map<CString, int>::const_iterator it = m_mSlavery.begin(); it != m_mSlavery.end(); ++it)
			ar << it->first << it->second;
		ar << m_mResearch.size();
		for (map<CString, int>::const_iterator it = m_mResearch.begin(); it != m_mResearch.end(); ++it)
			ar << it->first << it->second;
		ar << m_mCombatWins.size();
		for (map<CString, int>::const_iterator it = m_mCombatWins.begin(); it != m_mCombatWins.end(); ++it)
			ar << it->first << it->second;
		ar << m_mSabotage.size();
		for (map<CString, int>::const_iterator it = m_mSabotage.begin(); it != m_mSabotage.end(); ++it)
			ar << it->first << it->second;

		ar << m_nRivalsLeft;
		ar << m_bIsVictory;
		ar << (int)m_nVictoryType;
		ar << m_sVictoryRace;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		Reset();

		if (VERSION >= 0.72)
		{
			for (int i = 0; i < VICTORY_CONDITION_NUMBER; i++)
				ar >> m_bConditionStatus[i];

			size_t mapSize = 0;
			ar >> mapSize;
			for (size_t i = 0; i < mapSize; i++)
			{
				CString key;
				int value;
				ar >> key;
				ar >> value;
				m_mDiplomacy[key] = value;
			}

			mapSize = 0;
			ar >> mapSize;
			for (size_t i = 0; i < mapSize; i++)
			{
				CString key;
				int value;
				ar >> key;
				ar >> value;
				m_mSlavery[key] = value;
			}

			mapSize = 0;
			ar >> mapSize;
			for (size_t i = 0; i < mapSize; i++)
			{
				CString key;
				int value;
				ar >> key;
				ar >> value;
				m_mResearch[key] = value;
			}

			mapSize = 0;
			ar >> mapSize;
			for (size_t i = 0; i < mapSize; i++)
			{
				CString key;
				int value;
				ar >> key;
				ar >> value;
				m_mCombatWins[key] = value;
			}

			mapSize = 0;
			ar >> mapSize;
			for (size_t i = 0; i < mapSize; i++)
			{
				CString key;
				int value;
				ar >> key;
				ar >> value;
				m_mSabotage[key] = value;
			}

			ar >> m_nRivalsLeft;
			ar >> m_bIsVictory;
			int nType;
			ar >> nType;
			m_nVictoryType = (VICTORYTYPE)nType;
			ar >> m_sVictoryRace;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CVictoryObserver::Init(void)
{
	Reset();

	CIniLoader* pIni = CIniLoader::GetInstance();

	pIni->ReadValue("Victory_Conditions", "Elimination", m_bConditionStatus[VICTORYTYPE_ELIMINATION]);
	MYTRACE("general")(MT::LEVEL_INFO, "Victory_Conditions - Elimination: %s", m_bConditionStatus[VICTORYTYPE_ELIMINATION] ? "true" : "false");
	pIni->ReadValue("Victory_Conditions", "Diplomacy", m_bConditionStatus[VICTORYTYPE_DIPLOMACY]);
	MYTRACE("general")(MT::LEVEL_INFO, "Victory_Conditions - Diplomacy: %s", m_bConditionStatus[VICTORYTYPE_DIPLOMACY] ? "true" : "false");
	pIni->ReadValue("Victory_Conditions", "Conquest", m_bConditionStatus[VICTORYTYPE_CONQUEST]);
	MYTRACE("general")(MT::LEVEL_INFO, "Victory_Conditions - Conquest: %s", m_bConditionStatus[VICTORYTYPE_CONQUEST] ? "true" : "false");
	pIni->ReadValue("Victory_Conditions", "Research", m_bConditionStatus[VICTORYTYPE_RESEARCH]);
	MYTRACE("general")(MT::LEVEL_INFO, "Victory_Conditions - Research: %s", m_bConditionStatus[VICTORYTYPE_RESEARCH] ? "true" : "false");
	pIni->ReadValue("Victory_Conditions", "Combat", m_bConditionStatus[VICTORYTYPE_COMBATWINS]);
	MYTRACE("general")(MT::LEVEL_INFO, "Victory_Conditions - Combat: %s", m_bConditionStatus[VICTORYTYPE_COMBATWINS] ? "true" : "false");
	pIni->ReadValue("Victory_Conditions", "Sabotage", m_bConditionStatus[VICTORYTYPE_SABOTAGE]);
	MYTRACE("general")(MT::LEVEL_INFO, "Victory_Conditions - Sabotage: %s", m_bConditionStatus[VICTORYTYPE_SABOTAGE] ? "true" : "false");
}

int CVictoryObserver::GetVictoryStatus(const CString& sRaceID, VICTORYTYPE nType) const
{
	map<CString, int>::const_iterator it;

	switch (nType)
	{
	case VICTORYTYPE_ELIMINATION:
		return m_nRivalsLeft;

	case VICTORYTYPE_DIPLOMACY:
		it = m_mDiplomacy.find(sRaceID);
		if (it != m_mDiplomacy.end())
			return it->second;
		else
			return 0;

	case VICTORYTYPE_CONQUEST:
		it = m_mSlavery.find(sRaceID);
		if (it != m_mSlavery.end())
			return it->second;
		else
			return 0;

	case VICTORYTYPE_RESEARCH:
		it = m_mResearch.find(sRaceID);
		if (it != m_mResearch.end())
			return it->second;
		else
			return 0;

	case VICTORYTYPE_COMBATWINS:
		it = m_mCombatWins.find(sRaceID);
		if (it != m_mCombatWins.end())
			return it->second;
		else
			return 0;

	case VICTORYTYPE_SABOTAGE:
		it = m_mSabotage.find(sRaceID);
		if (it != m_mSabotage.end())
			return it->second;
		else
			return 0;

	default:
		return 0;
	}
}

int CVictoryObserver::GetBestVictoryValue(VICTORYTYPE nType) const
{
	vector<int> vValues;

	// Vektor mit allen Werten aus einem Bereich füllen
	switch (nType)
	{
	case VICTORYTYPE_ELIMINATION:
		return m_nRivalsLeft;

	case VICTORYTYPE_DIPLOMACY:
		for (map<CString, int>::const_iterator it = m_mDiplomacy.begin(); it != m_mDiplomacy.end(); ++it)
			vValues.push_back(it->second);
		break;

	case VICTORYTYPE_CONQUEST:
		for (map<CString, int>::const_iterator it = m_mSlavery.begin(); it != m_mSlavery.end(); ++it)
			vValues.push_back(it->second);
		break;

	case VICTORYTYPE_RESEARCH:
		for (map<CString, int>::const_iterator it = m_mResearch.begin(); it != m_mResearch.end(); ++it)
			vValues.push_back(it->second);
		break;

	case VICTORYTYPE_COMBATWINS:
		for (map<CString, int>::const_iterator it = m_mCombatWins.begin(); it != m_mCombatWins.end(); ++it)
			vValues.push_back(it->second);
		break;

	case VICTORYTYPE_SABOTAGE:
		for (map<CString, int>::const_iterator it = m_mSabotage.begin(); it != m_mSabotage.end(); ++it)
			vValues.push_back(it->second);
		break;
	}

	// Vektor mit gefüllten Werten sortieren
	std::sort(vValues.begin(), vValues.end());
	if (!vValues.empty())
		return vValues.back();

	return 0;
}

int CVictoryObserver::GetNeededVictoryValue(VICTORYTYPE nType) const
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	// aktuelle Runde
	int nCurrentRound = pDoc->GetCurrentRound();
	int nValue = 0;

	// Vektor mit allen Werten aus einem Bereich füllen
	switch (nType)
	{
	case VICTORYTYPE_ELIMINATION:
		// alle anderen müssen ausgelöscht sein
		return 0;

	case VICTORYTYPE_DIPLOMACY:
		// über 50% der Rassen auf Mitgliedschaft bzw. Bündnis (mindestens 10)
		nValue = pDoc->GetRaceCtrl()->size();
		// eigene Rasse nicht mitzählen
		nValue--;
		MYTRACE("general")(MT::LEVEL_INFO, "VICTORYTYPE_DIPLOMACY - needed value: %i", nValue/2);
		return max(nValue >> 1, 10);

	case VICTORYTYPE_CONQUEST:
		// über 50% aller bewohnten Systeme sind erobert (mindestens 10)
		nValue = 0;
		for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (pDoc->GetSystem(x,y).GetCurrentHabitants() > 0.0 && !pDoc->GetSystem(x,y).Free())
					nValue++;
		MYTRACE("general")(MT::LEVEL_INFO, "VICTORYTYPE_CONQUEST - needed value: %i", nValue/2);
		return max(nValue >> 1, 10);

	case VICTORYTYPE_RESEARCH:
		// Spezialforschung 10 wurde erforscht
		return 10;

	case VICTORYTYPE_COMBATWINS:
		//nValue = 0;
		// aktuelle Runde / 2.25 Schiffskampfsiege (mindestens 125)
		//nValue = nCurrentRound / 2.25, 125;
		//MYTRACE("general")(MT::LEVEL_INFO, "VICTORYTYPE_COMBATWINS - needed value: %i", nValue);
		return max(nCurrentRound / 2.25, 125);

	case VICTORYTYPE_SABOTAGE:
		// aktuelle Runde * 1.6 erfolgreiche Sabotageaktionen (mindestens 250)
		return max(nCurrentRound * 1.6, 250);

	default: return INT_MAX;
	}
}

void CVictoryObserver::Observe(void)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	//////////////////////////////////////////////////////////////////////
	// VICTORYTYPE_ELIMINATION
	//////////////////////////////////////////////////////////////////////
	if (m_bConditionStatus[VICTORYTYPE_ELIMINATION])
	{
		// Anzahl Majorraces noch im Spiel
		int nMajorsAlive = 0;
		map<CString, CMajor*>* pmMajors	= pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			if (it->second->GetEmpire()->CountSystems())
				nMajorsAlive++;
		MYTRACE("general")(MT::LEVEL_INFO, "nMajorsAlive: %i", nMajorsAlive);
		// eigene Rasse abziehen
		m_nRivalsLeft = nMajorsAlive - 1;

		// prüfen das keine andere Rasse mehr vorhanden ist
		if (m_nRivalsLeft == GetNeededVictoryValue(VICTORYTYPE_ELIMINATION))
		{
			m_bIsVictory	= true;
			// Die ausgelöschte Rasse ist meist noch in der Map, von daher noch richtig prüfen, wer ein System hat
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
				if (it->second->GetEmpire()->CountSystems() > 0)
				{
					m_sVictoryRace = it->first;
					break;
				}

			m_nVictoryType	= VICTORYTYPE_ELIMINATION;
			return;
		}
	}

	//////////////////////////////////////////////////////////////////////
	// VICTORYTYPE_DIPLOMACY
	//////////////////////////////////////////////////////////////////////
	if (m_bConditionStatus[VICTORYTYPE_DIPLOMACY])
	{
		m_mDiplomacy.clear();
		const CRaceController& race_ctrl = *pDoc->GetRaceCtrl();
		map<CString, CMajor*>* pmMajors	= pDoc->GetRaceCtrl()->GetMajors();

		// Anzahl an Bündnissen und Mitgliedschaften zu allen anderen Rassen holen
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			int nHighAgreements = 0;
			for (CRaceController::const_iterator it2 = race_ctrl.begin(); it2 != race_ctrl.end(); ++it2)
				if (it->first != it2->first)
				{
					int nAgreement = it->second->GetAgreement(it2->first);
					if (nAgreement == DIPLOMATIC_AGREEMENT::ALLIANCE || nAgreement == DIPLOMATIC_AGREEMENT::MEMBERSHIP)
						nHighAgreements++;
				}
			if (nHighAgreements > 0)
				m_mDiplomacy[it->first] = nHighAgreements;
		}

		// prüfen ob Siegbedingung erreicht wurde
		int nNeededValue = GetNeededVictoryValue(VICTORYTYPE_DIPLOMACY);
		for (map<CString, int>::const_iterator it = m_mDiplomacy.begin(); it != m_mDiplomacy.end(); ++it)
			if (it->second >= nNeededValue)
			{
				m_bIsVictory	= true;
				m_sVictoryRace	= it->first;
				m_nVictoryType	= VICTORYTYPE_DIPLOMACY;
				return;
			}
	}

	//////////////////////////////////////////////////////////////////////
	// VICTORYTYPE_CONQUEST
	//////////////////////////////////////////////////////////////////////
	if (m_bConditionStatus[VICTORYTYPE_CONQUEST])
	{
		m_mSlavery.clear();
		map<CString, CMajor*>* pmMajors	= pDoc->GetRaceCtrl()->GetMajors();
		// Anzahl an eroberten Sektoren berechnen
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			CMajor* pMajor = it->second;
			int nValue = 0;
			for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
			{
				const EMPIRE_SYSTEMS* pInfo = &pMajor->GetEmpire()->GetSystemList()->GetAt(i);
				if (pDoc->GetSystem(pInfo->ko.x, pInfo->ko.y).Taken())
					nValue++;
			}
			if (nValue > 0)
				m_mSlavery[it->first] = nValue;
		}

		// prüfen ob Siegbedingung erreicht wurde
		int nNeededValue = GetNeededVictoryValue(VICTORYTYPE_CONQUEST);
		for (map<CString, int>::const_iterator it = m_mSlavery.begin(); it != m_mSlavery.end(); ++it)
			if (it->second >= nNeededValue)
			{
				m_bIsVictory	= true;
				m_sVictoryRace	= it->first;
				m_nVictoryType	= VICTORYTYPE_CONQUEST;
				return;
			}
	}

	//////////////////////////////////////////////////////////////////////
	// VICTORYTYPE_RESEARCH
	//////////////////////////////////////////////////////////////////////
	if (m_bConditionStatus[VICTORYTYPE_RESEARCH])
	{
		m_mResearch.clear();
		map<CString, CMajor*>* pmMajors	= pDoc->GetRaceCtrl()->GetMajors();
		// Anzahl erforschter Spezialforschungen holen
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			CMajor* pMajor = it->second;
			int nValue = pMajor->GetEmpire()->GetResearch()->GetNumberOfUniqueResearch() - 1;
			m_mResearch[it->first] = nValue;
		}

		// prüfen ob Siegbedingung erreicht wurde
		int nNeededValue = GetNeededVictoryValue(VICTORYTYPE_RESEARCH);
		for (map<CString, int>::const_iterator it = m_mResearch.begin(); it != m_mResearch.end(); ++it)
			if (it->second >= nNeededValue)
			{
				m_bIsVictory	= true;
				m_sVictoryRace	= it->first;
				m_nVictoryType	= VICTORYTYPE_RESEARCH;
				return;
			}
	}

	//////////////////////////////////////////////////////////////////////
	// VICTORYTYPE_COMBATWINS
	//////////////////////////////////////////////////////////////////////
	if (m_bConditionStatus[VICTORYTYPE_COMBATWINS])
	{
		// prüfen ob Siegbedingung erreicht wurde
		int nNeededValue = GetNeededVictoryValue(VICTORYTYPE_COMBATWINS);
		for (map<CString, int>::const_iterator it = m_mCombatWins.begin(); it != m_mCombatWins.end(); ++it)
			if (it->second >= nNeededValue)
			{
				m_bIsVictory	= true;
				m_sVictoryRace	= it->first;
				m_nVictoryType	= VICTORYTYPE_COMBATWINS;
				return;
			}
	}

	//////////////////////////////////////////////////////////////////////
	// VICTORYTYPE_SABOTAGE
	//////////////////////////////////////////////////////////////////////
	if (m_bConditionStatus[VICTORYTYPE_SABOTAGE])
	{
		m_mSabotage.clear();
		map<CString, CMajor*>* pmMajors	= pDoc->GetRaceCtrl()->GetMajors();
		// Anzahl aller Sabotagereports holen
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			CMajor* pMajor = it->second;
			int nValue = 0;
			for (int i = 0; i < pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetNumberOfReports(); i++)
				if (pMajor->GetEmpire()->GetIntelligence()->GetIntelReports()->GetReport(i)->GetIsSpy() == false)
					nValue++;
			if (nValue > 0)
				m_mSabotage[it->first] = nValue;
		}
		// prüfen ob Siegbedingung erreicht wurde
		int nNeededValue = GetNeededVictoryValue(VICTORYTYPE_SABOTAGE);
		for (map<CString, int>::const_iterator it = m_mSabotage.begin(); it != m_mSabotage.end(); ++it)
			if (it->second >= nNeededValue)
			{
				m_bIsVictory	= true;
				m_sVictoryRace	= it->first;
				m_nVictoryType	= VICTORYTYPE_SABOTAGE;
				return;
			}
	}
}

void CVictoryObserver::Reset(void)
{
	memset(m_bConditionStatus, true, sizeof(m_bConditionStatus));

	m_mDiplomacy.clear();
	m_mSlavery.clear();
	m_mResearch.clear();
	m_mCombatWins.clear();
	m_mSabotage.clear();
	m_nRivalsLeft = INT_MAX;

	m_bIsVictory = false;
	m_nVictoryType = VICTORYTYPE_ELIMINATION;
	m_sVictoryRace = "";
}
