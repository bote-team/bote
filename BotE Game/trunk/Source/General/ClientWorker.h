#pragma once

#include "StdAfx.h"

#include "network.h"
#include "Constants.h"
#include "SoundManager.h"

#include <vector>
#include <map>

class CMajor;
class CRace;
class CIniLoader;
class CSoundManager;
class CRoundButton2;

class CClientWorker
{
private:

	//Welche View soll in der MainView angezeigt werden? z.B. Galaxie oder System
	std::vector<VIEWS::MAIN_VIEWS> m_SelectedView;

	///< Die einzelnen Sprachmitteilungen zur neuen Runde
	CArray<SNDMGR_MESSAGEENTRY> m_SoundMessages[network::RACE_ALL];

	void ClearSoundMessages(network::RACE client);

	CClientWorker(void);
	CClientWorker(const CClientWorker& o);

public:
	static CClientWorker* GetInstance();

	~CClientWorker(void);

	void Serialize(CArchive& ar, bool sounds);

	/// Funktion gibt die zu einer Client-ID zugeh�rige Major-ID zur�ck.
	/// Maximal k�nnen sechs Clients bestehen. Jede Majorrace ben�tigt eine eindeutige
	/// Client-ID.
	/// @param client-ID Client-ID eines Spielers
	/// @return Rassen-ID
	/// ALPHA5 -> noch fest!
	static CString GetMappedRaceID(network::RACE clientID);

	unsigned short GetSelectedViewFor(const CString& sRaceID);
	void SetSelectedViewForTo(network::RACE race, unsigned short to);
	void SetSelectedViewForTo(const CMajor& major, unsigned short to);
	void SetSelectedViewForTo(const CString& major, unsigned short to);

	void SetToEmpireViewFor(const CMajor& major);

	void DoViewWorkOnNewRound(const CMajor& PlayersRace);

	void ResetViews();

	void CommitSoundMessages(CSoundManager* pSoundManager, const CMajor& player) const;
	void ClearSoundMessages();
	void ClearSoundMessages(const CMajor& race);
	void AddSoundMessage(SNDMGR_VALUE type, const CMajor& major, int priority, const CRace* contacted = NULL);

	void CalcContact(CMajor& Major, const CRace& ContactedRace);
	void CalcStationReady(const SHIP_TYPE::Typ typ, const CMajor& race);

	static void SetMajorToHumanOrAi(CMajor& major);
	static void SetMajorsToHumanOrAi(const std::map<CString, CMajor*>& Majors);

	static void StartMusic(const CIniLoader& ini, CSoundManager& sm, const CMajor& player);
	static void PlaySound(SNDMGR_VALUE type, const CString& major);
	void CreateButtons(std::vector<std::pair<CRoundButton2*, CString>>& MajorBtns,
		const std::map<CString, CMajor*>& majors, CWnd* parent) const;


};
