#include "stdafx.h"

#include <cmath>
#include "float.h"

#include "MajorJoining.h"
#include "IniLoader.h"
#include "RaceController.h"
#include "General/Statistics.h"
#include "Major.h"
#include "Ships/ShipMap.h"
#include "Ships/Ships.h"
#include "Races/Minor.h"
#include "General/Loc.h"


//////////////////////////////////////////////////////////////////////
// construction
//////////////////////////////////////////////////////////////////////

CMajorJoining::CMajorJoining(void) :
	m_TimesOccured(0),
	m_TimesShouldOccur(0),
	m_StartTurn(120),
	m_RisingTurns(140),
	m_Pause(80),
	m_Randomize_by(20),
	m_Joined()
{
}

CMajorJoining::~CMajorJoining(void)
{
}

/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CMajorJoining* CMajorJoining::GetInstance(void)
{
	static CMajorJoining instance;
	return &instance;
}

void CMajorJoining::InitFromIni()
{
	m_TimesShouldOccur = CIniLoader::GetInstance()->ReadValueDefault("Special", "MAJORJOINING", 0);
}

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////

void CMajorJoining::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_TimesOccured;
		ar << m_TimesShouldOccur;
		ar << m_StartTurn;
		ar << m_Joined.size();
		for(std::set<CString>::const_iterator it = m_Joined.begin(); it != m_Joined.end(); ++it)
			ar << *it;
	}
	else
	{
		ar >> m_TimesOccured;
		ar >> m_TimesShouldOccur;
		ar >> m_StartTurn;
		unsigned size;
		ar >> size;
		for(unsigned i = 0; i < size; ++i)
		{
			CString id;
			ar >> id;
			m_Joined.insert(id);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

static int randomize_by(int amount)
{
	const int sign = ((rand() % 2) == 0) ? 1 : -1;
	return  sign * (rand() % (amount + 1));
}

void CMajorJoining::Calculate(int turn, const CStatistics& stats, CRaceController& race_ctrl,
		CShipMap& ships, std::vector<CSystem>& systems, float difficulty)
{
	if(turn == 2)
		m_StartTurn += randomize_by(m_Randomize_by);

	const std::vector<std::pair<CString, float>>& marks = stats.GetSortedMarks();

	CMajor* best_ai = NULL;
	const CMajor* worst_ai = NULL;
	int count_of_ais = 0;
	int count_of_humans = 0;
	float human_average_mark = 0.0f;
	std::vector<std::pair<CMajor*, float>> majors;
	for(std::vector<std::pair<CString, float>>::const_iterator it = marks.begin(); it != marks.end(); ++it)
	{
		CMajor& major = *race_ctrl.GetMajor(it->first);
		majors.push_back(std::pair<CMajor*, float>(&major, it->second));
		if(major.IsHumanPlayer())
		{
			++count_of_humans;
			human_average_mark += it->second;
		}
		else
		{
			++count_of_ais;
			worst_ai = &major;
			if(!best_ai)
			{
				const std::set<CString>::const_iterator it = m_Joined.find(major.GetRaceID());
				if(it == m_Joined.end()
					//70% chance to look for another one that hasn't been joined yet
					|| rand() % 100 >= 70)
						best_ai = &major;
			}
		}
	}
	AssertBotE(count_of_humans >= 1);
	human_average_mark /= count_of_humans;
	if(count_of_ais < 2)
		return;
	if(!best_ai || best_ai == worst_ai)
		return;
	AssertBotE(worst_ai);
	AssertBotE(best_ai->GetRaceID() != worst_ai->GetRaceID());

	if(turn >= 20)
		CalcStartTurnChangeDueToHumansStrength(human_average_mark, majors.size(), difficulty);

	if(!ShouldHappenNow(turn))
		return;

	m_Joined.insert(best_ai->GetRaceID());
	m_TimesOccured++;
	m_StartTurn = turn + m_Pause + randomize_by(m_Randomize_by);

	const int credits = worst_ai->GetEmpire()->GetCredits();
	if(credits > 0)
		best_ai->GetEmpire()->SetCredits(credits);

	for(CShipMap::iterator it = ships.begin(); it != ships.end(); ++it)
	{
		if(it->second->OwnerID() == worst_ai->GetRaceID())
		{
			it->second->SetOwner(best_ai->GetRaceID());
			it->second->SetIsShipFlagShip(FALSE);
			it->second->UnsetCurrentOrder();
			it->second->SetCombatTacticAccordingToType();
			it->second->SetTargetKO(CPoint(-1, -1));
		}
	}

	for(std::vector<CSystem>::iterator it = systems.begin(); it != systems.end(); ++it)
	{
		if(!it->Majorized() || it->OwnerID() != worst_ai->GetRaceID())
			continue;

		const bool taken = it->Taken();
		it->ChangeOwner(
			best_ai->GetRaceID(),
			taken ? CSystem::OWNING_STATUS_TAKEN : CSystem::OWNING_STATUS_COLONIZED_MEMBERSHIP_OR_HOME,
			false);
		it->SetFullKnown(best_ai->GetRaceID());
		boost::shared_ptr<CRace> home_of = it->HomeOf();
		if(home_of && home_of->IsMinor())
		{
			boost::shared_ptr<CMinor> minor = boost::dynamic_pointer_cast<CMinor>(home_of);
			minor->SetOwner(race_ctrl.GetMajorSafe(best_ai->GetRaceID()));

			if(!taken)
			{
				minor->SetRelation(best_ai->GetRaceID(), minor->GetRelation(worst_ai->GetRaceID()));
				minor->SetAgreement(best_ai->GetRaceID(), DIPLOMATIC_AGREEMENT::MEMBERSHIP);
			}
		}
	}

	CEmpireNews message1;
	CString sMsgText = CLoc::GetString("MAJOR_JOINING_PART_ONE", false, worst_ai->GetRaceNameWithArticle(),
		best_ai->GetRaceNameWithArticle());
	message1.CreateNews(sMsgText,EMPIRE_NEWS_TYPE::DIPLOMACY);
	CEmpireNews message2;
	sMsgText = CLoc::GetString("MAJOR_JOINING_PART_TWO", false, worst_ai->GetHomesystemName(),
		best_ai->GetRaceNameWithArticle());
	message2.CreateNews(sMsgText,EMPIRE_NEWS_TYPE::DIPLOMACY);

	for(std::vector<std::pair<CMajor*, float>>::const_iterator it = majors.begin(); it != majors.end(); ++it)
	{
		it->first->GetEmpire()->AddMsg(message1);
		it->first->GetEmpire()->AddMsg(message2);
	}
}

bool CMajorJoining::ShouldHappenNow(int turn) const
{
	if(m_TimesOccured >= m_TimesShouldOccur)
		return false;
	if(turn < m_StartTurn)
		return false;
	if(turn > m_StartTurn + m_RisingTurns)
		return true;

	const int prob = 100.0f *
		(pow(2.0f, pow((turn -  m_StartTurn) / static_cast<float>(m_RisingTurns), 3.0f)) - 1);
	return rand() % 100 < prob;
}

void CMajorJoining::CalcStartTurnChangeDueToHumansStrength(float human_average_mark, int count_of_majors,
		float difficulty)
{
	AssertBotE(1.0f <= human_average_mark && human_average_mark <= count_of_majors);

	float fraction = 0.0f;
	int turn_change = 0;
	if(difficulty == DIFFICULTIES::EASY)
	{
		fraction = 0.15f;
		turn_change = 1;
	}
	else if (difficulty == DIFFICULTIES::NORMAL)
	{
		fraction = 0.3f;
		turn_change = 1;
	}
	else if (difficulty == DIFFICULTIES::HARD)
	{
		fraction = 0.45f;
		turn_change = 2;
	}
	else if (difficulty == DIFFICULTIES::IMPOSSIBLE)
	{
		fraction = 0.6f;
		turn_change = 2;
	}

	if(human_average_mark > fraction * count_of_majors)
		return;

	m_StartTurn -= turn_change;
	m_StartTurn = max(m_StartTurn, 1);
}