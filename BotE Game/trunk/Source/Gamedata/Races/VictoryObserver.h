/*
 *   Copyright (C)2004-2010 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include <map>

// forward declaration

using namespace std;

typedef enum VICTORYTYPE
{
	VICTORYTYPE_ELIMINATION,	///< einzig überlebende Majorrace
	VICTORYTYPE_DIPLOMACY,		///< über 50% der Rassen auf Mitgliedschaft bzw. Bündnis (mindestens 10)
	VICTORYTYPE_CONQUEST,		///< über 50% aller bewohnten Systeme sind erobert (mindestens 10)
	VICTORYTYPE_RESEARCH,		///< Spezialforschung 10 wurde erforscht
	VICTORYTYPE_COMBATWINS,		///< aktuelle Runde / 4 Schiffskampfsiege (mindestens 50)
	VICTORYTYPE_SABOTAGE		///< aktuelle Runde * 2 erfolgreiche Sabotageaktionen (mindestens 500)
};

#define VICTORY_CONDITION_NUMBER	6	///< Anzahl unterschiedlicher Siegbedingungen

/// Klasse überprüft und überwacht alle Siegbedingungen
class CVictoryObserver : public CObject
{
public:
	// Klasse serialisierbar machen
	DECLARE_SERIAL (CVictoryObserver)

	/// Standardkonstruktor
	CVictoryObserver(void);
	
	/// Standarddestruktor
	virtual ~CVictoryObserver(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Funktionen
	bool IsVictory(void) const {return m_bIsVictory;}

	CString GetVictoryRace(void) const {return m_sVictoryRace;}

	VICTORYTYPE GetVictoryType(void) const {return m_nVictoryType;}

	bool IsVictoryCondition(VICTORYTYPE nType) const {return m_bConditionStatus[nType];}

	int GetRivalsLeft(void) const {return m_nRivalsLeft;}

	int GetVictoryStatus(const CString& sRaceID, VICTORYTYPE nType) const;
	
	int GetBestVictoryValue(VICTORYTYPE nType) const;

	int GetNeededVictoryValue(VICTORYTYPE nType) const;
		
	void Observe(void);

	void AddCombatWin(const CString& sRaceID) {m_mCombatWins[sRaceID] += 1;}

	void Init(void);

	void Reset(void);

private:
	// Attribute
	bool m_bConditionStatus[VICTORY_CONDITION_NUMBER];	///< ist die jeweilige Siegbedingung aktiviert?
	
	map<CString, int> m_mDiplomacy;
	map<CString, int> m_mSlavery;
	map<CString, int> m_mResearch;
	map<CString, int> m_mCombatWins;
	map<CString, int> m_mSabotage;

	int m_nRivalsLeft;

	bool m_bIsVictory;
	VICTORYTYPE m_nVictoryType;
	CString m_sVictoryRace;
};
 