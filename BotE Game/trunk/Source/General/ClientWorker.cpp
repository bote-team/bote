#include "stdafx.h"

#include <cassert>

#include "ClientWorker.h"
#include "resources.h"
#include "Constants.h"
#include "Races/Major.h"
#include "MainFrm.h"

CClientWorker::CClientWorker(void) :
	m_SelectedView()
{
	resources::pClientWorker = this;

	//entries .at(0) are unused
	m_SelectedView.resize(network::RACE_ALL, VIEWS::NULL_VIEW);
	//memset(m_SoundMessages, CArray<SNDMGR_MESSAGEENTRY>(), sizeof(m_SoundMessages))
}

CClientWorker::~CClientWorker(void)
{
	resources::pClientWorker = NULL;
}

CClientWorker* CClientWorker::GetInstance() {
	static CClientWorker instance;
	return &instance;
}

void CClientWorker::Serialize(CArchive& ar, bool sounds)
{
	if(sounds)
	{
		for (int i = network::RACE_1; i < network::RACE_ALL; i++)
			m_SoundMessages[i].Serialize(ar);
	}

	if(ar.IsStoring()) {
		for (std::vector<VIEWS::MAIN_VIEWS>::const_iterator it = m_SelectedView.begin();
			it != m_SelectedView.end(); ++it)
		{
			assert(VIEWS::NULL_VIEW <= *it && *it <= VIEWS::COMBAT_VIEW);
			ar << static_cast<unsigned short>(*it);
		}
	}
	else {
		for (std::vector<VIEWS::MAIN_VIEWS>::iterator it = m_SelectedView.begin();
			it != m_SelectedView.end(); ++it)
		{
			unsigned short value;
			ar >> value;
			assert(VIEWS::NULL_VIEW <= value && value <= VIEWS::COMBAT_VIEW);
			*it = static_cast<VIEWS::MAIN_VIEWS>(value);
		}
	}
}

/// Funktion gibt die zu einer Majorrace zugehörige Netzwerkclient-ID zurück.
/// Maximal können sechs Clients bestehen. Jede Majorrace benötigt eine eindeutige
/// Client-ID.
/// @param sRaceID Rassen-ID einer Majorrace
/// @return Netzwerk-Client-ID
/// ALPHA5 -> noch fest!
network::RACE CClientWorker::GetMappedClientID(const CString& sRaceID)
{
	if (sRaceID == "MAJOR1")
		return network::RACE_1;
	if (sRaceID == "MAJOR2")
		return network::RACE_2;
	if (sRaceID == "MAJOR3")
		return network::RACE_3;
	if (sRaceID == "MAJOR4")
		return network::RACE_4;
	if (sRaceID == "MAJOR5")
		return network::RACE_5;
	if (sRaceID == "MAJOR6")
		return network::RACE_6;

	assert(false);
	return network::RACE_NONE;
}

/// Funktion gibt die zu einer Client-ID zugehörige Major-ID zurück.
/// Maximal können sechs Clients bestehen. Jede Majorrace benötigt eine eindeutige
/// Client-ID.
/// @param client-ID Client-ID eines Spielers
/// @return Rassen-ID
/// ALPHA5 -> noch fest!
CString CClientWorker::GetMappedRaceID(network::RACE clientID)
{
	if (clientID == network::RACE_1)
		return "MAJOR1";
	if (clientID == network::RACE_2)
		return "MAJOR2";
	if (clientID == network::RACE_3)
		return "MAJOR3";
	if (clientID == network::RACE_4)
		return "MAJOR4";
	if (clientID == network::RACE_5)
		return "MAJOR5";
	if (clientID == network::RACE_6)
		return "MAJOR6";

	assert(false);
	return "";
}

unsigned short CClientWorker::GetSelectedViewFor(const CString& sRaceID)
{
	return m_SelectedView.at(GetMappedClientID(sRaceID));
}

void CClientWorker::SetSelectedViewForTo(network::RACE race, unsigned short to)
{
	assert(VIEWS::NULL_VIEW <= to && to <= VIEWS::COMBAT_VIEW);
	m_SelectedView[race] = static_cast<VIEWS::MAIN_VIEWS>(to);
}

void CClientWorker::SetSelectedViewForTo(const CMajor& major, unsigned short to)
{
	SetSelectedViewForTo(major.GetRaceID(), to);
}

void CClientWorker::SetSelectedViewForTo(const CString& major, unsigned short to)
{
	const network::RACE client = GetMappedClientID(major);
	SetSelectedViewForTo(client, to);
}

void CClientWorker::SetToEmpireViewFor(const CMajor& major)
{
	if(!major.IsHumanPlayer())
		return;
	const network::RACE client = GetMappedClientID(major.GetRaceID());
	m_SelectedView[client] = VIEWS::EMPIRE_VIEW;
}

void CClientWorker::DoViewWorkOnNewRound(const CMajor& PlayersRace)
{
	const network::RACE client = GetMappedClientID(PlayersRace.GetRaceID());

	// anzuzeigende View in neuer Runde auswählen
	// Wenn EventScreens für den Spieler vorhanden sind, so werden diese angezeigt.
	if (PlayersRace.GetEmpire()->GetEvents()->GetSize() > 0)
	{
		resources::pMainFrame->FullScreenMainView(true);
		resources::pMainFrame->SelectMainView(VIEWS::EVENT_VIEW, PlayersRace.GetRaceID());
	}
	else
	{
		resources::pMainFrame->FullScreenMainView(false);
		resources::pMainFrame->SelectMainView(m_SelectedView.at(client), PlayersRace.GetRaceID());
		m_SelectedView[client] = VIEWS::NULL_VIEW;
	}
}

void CClientWorker::ResetViews()
{
	for (std::vector<VIEWS::MAIN_VIEWS>::iterator it = m_SelectedView.begin();
			it != m_SelectedView.end(); ++it)
		if (*it == VIEWS::FLEET_VIEW)
			*it = VIEWS::GALAXY_VIEW;
}

void CClientWorker::CommitSoundMessages(CSoundManager* pSoundManager, const CMajor& player) const
{
	assert(pSoundManager);
	const network::RACE client = GetMappedClientID(player.GetRaceID());

	pSoundManager->ClearMessages();
	for (int i = 0; i < m_SoundMessages[client].GetSize(); i++)
	{
		const SNDMGR_MESSAGEENTRY& entry = m_SoundMessages[client].GetAt(i);
		pSoundManager->AddMessage(entry.nMessage, entry.nRace, entry.nPriority, entry.fVolume);
	}
}

void CClientWorker::ClearSoundMessages(network::RACE client)
{
	m_SoundMessages[client].RemoveAll();
}

void CClientWorker::ClearSoundMessages()
{
	for (int i = network::RACE_1; i < network::RACE_ALL; i++)
		ClearSoundMessages(static_cast<network::RACE>(i));
}

void CClientWorker::ClearSoundMessages(const CMajor& race)
{
	ClearSoundMessages(GetMappedClientID(race.GetRaceID()));
}

void CClientWorker::AddSoundMessage(SNDMGR_VALUE type, const CMajor& major, int priority)
{
	if(!major.IsHumanPlayer())
		return;
	const network::RACE client = GetMappedClientID(major.GetRaceID());
	const SNDMGR_MESSAGEENTRY entry = {type, client, priority, 1.0f};
	m_SoundMessages[client].Add(entry);
}

void CClientWorker::CalcContact(CMajor& Major, const CRace& ContactedRace) {
	if(!Major.IsHumanPlayer())
		return;
	SetToEmpireViewFor(Major);
	// Audiovorstellung der kennengelernten race
	if(ContactedRace.IsMajor())
		AddSoundMessage(SNDMGR_MSG_FIRSTCONTACT, dynamic_cast<const CMajor&>(ContactedRace), 2);
	else
		AddSoundMessage(SNDMGR_MSG_ALIENCONTACT, Major, 1);
}
