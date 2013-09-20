#include "stdafx.h"

#include <cassert>

#include "resources.h"
#include "ClientWorker.h"
#include "Constants.h"
#include "Races/Major.h"
#include "MainFrm.h"

CClientWorker::CClientWorker(void)
{
	resources::pClientWorker = this;

	memset(m_iSelectedView, START_VIEW, sizeof(m_iSelectedView));
}

CClientWorker::~CClientWorker(void)
{
	resources::pClientWorker = NULL;
}

CClientWorker* CClientWorker::GetInstance() {
	static CClientWorker instance;
	return &instance;
}

void CClientWorker::Serialize(CArchive& ar) {
	if(ar.IsStoring()) {
		for (int i = network::RACE_1; i < network::RACE_ALL; i++)
			ar << m_iSelectedView[i];
	}
	else {
		for (int i = network::RACE_1; i < network::RACE_ALL; i++)
			ar >> m_iSelectedView[i];
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

unsigned short CClientWorker::GetSelectedViewFor(const CString& sRaceID)
{
	return m_iSelectedView[GetMappedClientID(sRaceID)];
}

void CClientWorker::SetSelectedViewForTo(network::RACE race, unsigned short to)
{
	m_iSelectedView[race] = to;
}

void CClientWorker::SetToEmpireViewFor(const CMajor& major)
{
	if(!major.IsHumanPlayer())
		return;
	const network::RACE client = GetMappedClientID(major.GetRaceID());
	m_iSelectedView[client] = EMPIRE_VIEW;
}

void CClientWorker::DoViewWorkOnNewRound(const CMajor& PlayersRace)
{
	const network::RACE client = GetMappedClientID(PlayersRace.GetRaceID());

	// anzuzeigende View in neuer Runde auswählen
	// Wenn EventScreens für den Spieler vorhanden sind, so werden diese angezeigt.
	if (PlayersRace.GetEmpire()->GetEvents()->GetSize() > 0)
	{
		resources::pMainFrame->FullScreenMainView(true);
		resources::pMainFrame->SelectMainView(EVENT_VIEW, PlayersRace.GetRaceID());
	}
	else
	{
		resources::pMainFrame->FullScreenMainView(false);
		resources::pMainFrame->SelectMainView(m_iSelectedView[client], PlayersRace.GetRaceID());
		m_iSelectedView[client] = NULL_VIEW;
	}
}

void CClientWorker::ResetViews()
{
	for (int i = network::RACE_1; i < network::RACE_ALL; i++)
	if (m_iSelectedView[i] == FLEET_VIEW)
		m_iSelectedView[i] = GALAXY_VIEW;
}
