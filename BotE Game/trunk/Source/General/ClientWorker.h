#pragma once

#include "network.h"
#include "Constants.h"

class CMajor;

class CClientWorker
{
private:

	//Welche View soll in der MainView angezeigt werden? z.B. Galaxie oder System
	VIEWS::MAIN_VIEWS m_iSelectedView[network::RACE_ALL];

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

	unsigned short GetSelectedViewFor(const CString& sRaceID);
	void SetSelectedViewForTo(network::RACE race, unsigned short to);

	void SetToEmpireViewFor(const CMajor& major);

	void DoViewWorkOnNewRound(const CMajor& PlayersRace);

	void ResetViews();

};
