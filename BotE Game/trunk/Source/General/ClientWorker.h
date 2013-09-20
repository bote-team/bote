#pragma once

#include "network.h"
#include "Constants.h"

#include <vector>

class CMajor;

class CClientWorker
{
private:

	//Welche View soll in der MainView angezeigt werden? z.B. Galaxie oder System
	std::vector<VIEWS::MAIN_VIEWS> m_SelectedView;

	CClientWorker(void);
	CClientWorker(const CClientWorker& o);

public:
	static CClientWorker* GetInstance();

	~CClientWorker(void);

	void Serialize(CArchive& ar);

	/// Funktion gibt die zu einer Majorrace zugehörige Netzwerkclient-ID zurück.
	/// Maximal können sechs Clients bestehen. Jede Majorrace benötigt eine eindeutige
	/// Client-ID.
	/// @param sRaceID Rassen-ID einer Majorrace
	/// @return Netzwerk-Client-ID
	/// ALPHA5 -> noch fest!
	static network::RACE GetMappedClientID(const CString& sRaceID);

	/// Funktion gibt die zu einer Client-ID zugehörige Major-ID zurück.
	/// Maximal können sechs Clients bestehen. Jede Majorrace benötigt eine eindeutige
	/// Client-ID.
	/// @param client-ID Client-ID eines Spielers
	/// @return Rassen-ID
	/// ALPHA5 -> noch fest!
	static CString GetMappedRaceID(network::RACE clientID);

	unsigned short GetSelectedViewFor(const CString& sRaceID);
	void SetSelectedViewForTo(network::RACE race, unsigned short to);

	void SetToEmpireViewFor(const CMajor& major);

	void DoViewWorkOnNewRound(const CMajor& PlayersRace);

	void ResetViews();

};
