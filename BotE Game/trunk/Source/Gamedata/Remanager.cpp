#include "stdafx.h"
#include "Remanager.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races/Major.h"
#include "Races/Minor.h"
#include "Races/RaceController.h"
#include "Galaxy/planet.h"
#include "Events/EventRandom.h"
#include "Gamedata/General/Message.h"
#include "Ships/Ships.h"
#include "General/ResourceManager.h"
#include "IniLoader.h"

#include <cassert>

CReManager::CReManager(void) :
	m_uiGlobalProb(3),
	m_uiProbPerSystem(10),
	m_uiProbPerMinor(10)
{
	m_bIsActivated = true;
	if (CIniLoader* pIni = CIniLoader::GetInstance())
		m_bIsActivated = pIni->ReadValueDefault("Special", "RANDOMEVENTS", true);
}

CReManager::~CReManager(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CReManager* CReManager::GetInstance(void)
{
	static CReManager instance;
	return &instance;
}

//In this function, calculate whether an event fails or succeeds due to given probabilities.
//Functions executed in succession only decide whether an event fails due to constraints
//such as population already being at minimum.
void CReManager::CalcEvents(CMajor* pRace)
{
	if (!IsActivated())
		return;

	CBotf2Doc const* pDoc = resources::pDoc;
	const unsigned event_type = rand() % 3;
	if(event_type == GLOBALEVENTSYSTEM)//system affecting event
	{
		//Calculate whether such event happens. The more systems we have, the more
		//likely it is that one of them is affected.
		CArray<SystemViewStruct> const* systems = pRace->GetEmpire()->GetSystemList();
		const unsigned size = systems->GetSize();
		const int prob = min(m_uiProbPerSystem * size, 1000);
		if(rand() % 999 >= prob)
			return;
		int whichsystem = rand() % size;//wählt System aus
		const CPoint ko = systems->GetAt(whichsystem).ko; //sucht koordinaten des Systems
		//Major home systems are generally unaffected. Unbalances too much.
		if(pDoc->GetRaceKO(pRace->GetRaceID()) == ko)
			return;
		for(int i=0;i<100&&(!SystemEvent(ko, pRace));i++);
	}
	else if (event_type == GLOBALEVENTRESEARCH){
		if(rand() % 99 >= static_cast<int>(m_uiGlobalProb))
			return; //Es findet kein Ereignis statt
		GlobalEventResearch(pRace);
	}
	else //event_type == GLOBALEVENTMINOR
	{
		std::vector<CMinor*> PossibleMinors;
		const std::map<CString, CMinor*>* pmMinors = pDoc->GetRaceCtrl()->GetMinors();
		for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
		{
			CMinor* pMinor = it->second;
			if(pRace->IsRaceContacted(pMinor->GetRaceID())&& pMinor->GetRelation(pRace->GetRaceID())<85)
				PossibleMinors.insert(PossibleMinors.end(),pMinor);
		}
		const unsigned size = PossibleMinors.size();
		const int prob = min(m_uiProbPerMinor * size, 1000);
		if(rand() % 999 >= prob)
			return;
		GlobalEventMinor(pRace, PossibleMinors.at(rand() % size));
	}
}

bool CReManager::SystemEvent(const CPoint &ko, CMajor* pRace)
{
	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);
	//ko= Systemkoordinate
	CString sMsgText;//Nachrichtentext
	bool bSuccess=true;
	int nEventNumber=rand()%4;

	if(nEventNumber==SYSTEMEVENTMORALBOOST)//If abfrage mit allen möglichen Randomevents; evtl. hier bedingungen einfügen
	{
		sMsgText=CResourceManager::GetString("SYSTEMEVENTMORALBOOST",false,pDoc->GetSector(ko.x, ko.y).GetName());
		pDoc->GetSystem(ko.x, ko.y).SetMoral(10);
	}
	else if(nEventNumber==SYSTEMEVENTMORALMALUS)
	{
		sMsgText=CResourceManager::GetString("SYSTEMEVENTMORALMALUS",false,pDoc->GetSector(ko.x, ko.y).GetName());
		pDoc->GetSystem(ko.x, ko.y).SetMoral(-10);
	}
	else if(nEventNumber==SYSTEMEVENTPLANETMOVEMENT)//Planetenveränderung
	{
		std::vector<CPlanet>& vPlanets = pDoc->GetSector(ko.x, ko.y).GetPlanets();
		const int nSize = vPlanets.size();
		assert(nSize >= 1);
		int nPlanet=rand()%nSize;
		while(!(vPlanets.at(nPlanet).GetHabitable()))
			nPlanet=rand()%nSize;
		const float old_habitants = vPlanets.at(nPlanet).GetMaxHabitant();
		const float habitants_change = old_habitants * ((rand() % 11) / 10.0f) * ((rand() % 2 == 1) ? 1 : -1);
		const float new_habitants = min(max(old_habitants + habitants_change, 1),100);
		bSuccess = old_habitants != new_habitants;
		if(bSuccess)
		{
			vPlanets.at(nPlanet).SetMaxHabitant(new_habitants);
			CString habitants_change_string;
			habitants_change_string.Format("%.3f", habitants_change);
			sMsgText=CResourceManager::GetString("SYSTEMEVENTPLANETMOVEMENT",false,vPlanets.at(nPlanet).GetPlanetName(), habitants_change_string);
		}
	}
	else if(nEventNumber==SYSTEMEVENTDEMOGRAPHIC)
	{
		std::vector<CPlanet>& planets = pDoc->GetSector(ko.x, ko.y).GetPlanets();
		int planet = 0;
		// Es sollte hier immer mindestens 1 habitabler bewohnter Planet im System sein...
		bSuccess = false;
		for(int i = 0; i < 100; ++i) {
			planet = rand()%planets.size();
			if(planets.at(planet).GetHabitable() && planets.at(planet).GetCurrentHabitant() > 1) {
				bSuccess = true;
				break;
			}
		}
		if(bSuccess) {
			const float old_habitants = planets.at(planet).GetCurrentHabitant();
			const float habitants_change = old_habitants * ((rand() % 11) / 10.0f) * -1;
			const float new_habitants = max(old_habitants + habitants_change, 1);
			bSuccess = old_habitants != new_habitants;
			if(bSuccess)
			{
				planets.at(planet).SetCurrentHabitant(new_habitants);
				CString habitants_change_string;
				habitants_change_string.Format("%.3f", habitants_change * -1);
				sMsgText=CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHICLONG",false,planets.at(planet).GetPlanetName(), habitants_change_string);

				if (pRace->IsHumanPlayer())
				{
					network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pRace->GetRaceID());
					pDoc->m_iSelectedView[client] = EMPIRE_VIEW;

					CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"demographic",CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHICTITLE"),CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHICLONG",false,planets.at(planet).GetPlanetName()));
					pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
				}
			}
		}
	}

	if(bSuccess && !sMsgText.IsEmpty())
	{
		CMessage message;
		message.GenerateMessage(sMsgText,MESSAGE_TYPE::SOMETHING,"",ko);//Nachricht über Randomevent erstellen
		pRace->GetEmpire()->AddMessage(message);

		if (pRace->IsHumanPlayer())
		{
			network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pRace->GetRaceID());
			pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
		}
	}
	return bSuccess;
}

void CReManager::GlobalEventResearch(CMajor *pRace)
{
	CString messagetext=CResourceManager::GetString("GLOBALEVENTRESEARCH");
	pRace->GetEmpire()->AddFP((int)(pRace->GetEmpire()->GetFP()));

	CMessage message;
	message.GenerateMessage(messagetext,MESSAGE_TYPE::RESEARCH);//Nachricht für Randomevent erstellen
	pRace->GetEmpire()->AddMessage(message);

	if (pRace->IsHumanPlayer())
	{
		CBotf2Doc* pDoc = resources::pDoc;
		ASSERT(pDoc);

		network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pRace->GetRaceID());
		pDoc->m_iSelectedView[client] = EMPIRE_VIEW;

		CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"Breakthrough",CResourceManager::GetString("BREAKTHROUGH"),CResourceManager::GetString("GLOBALEVENTRESEARCH"));
		pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
	}
}

void CReManager::GlobalEventMinor(CMajor* pRace, CMinor* pMinor)
{
	pMinor->SetRelation(pRace->GetRaceID(), (rand() % 101) - pMinor->GetRelation(pRace->GetRaceID()));
	CString messagetext=CResourceManager::GetString("GLOBALEVENTMINOR",false,pMinor->GetRaceName());
	CMessage message;
	message.GenerateMessage(messagetext,MESSAGE_TYPE::DIPLOMACY);//Nachricht für Randomevent erstellen
	pRace->GetEmpire()->AddMessage(message);
}

void CReManager::CalcExploreEvent(const CPoint &ko, CMajor *pRace, CShipMap* ships)
{
	if (!IsActivated())
		return;

	if(rand() % 99 >= static_cast<int>(m_uiGlobalProb))
		return; //Es findet kein Ereignis statt

	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	//ko=Koordinate wo es passiert, pRace = die Rasse der es passiert
	int eventnumber=rand()%2;
	CString sMessageText = "";
	MESSAGE_TYPE::Typ typ = MESSAGE_TYPE::NO_TYPE;

	if (eventnumber == ALIENTEC)
	{
		// Forschung insgesamt um (50 bis 150) * Anzahl Systeme erhöhen
		int nSystemCount = pRace->GetEmpire()->GetSystemList()->GetSize();
		int nAdd = (50 + rand()%101) * nSystemCount;
		pRace->GetEmpire()->AddFP(nAdd);

		CString sSectorName = pDoc->GetSector(ko.x, ko.y).GetName(false);
		if (sSectorName.IsEmpty())
			sSectorName.Format("%c%i", (char)(ko.y+97), ko.x + 1);

		sMessageText = CResourceManager::GetString("ALIENTEC",false,sSectorName);
		typ = MESSAGE_TYPE::RESEARCH;

		if (pRace->IsHumanPlayer())
		{
			CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"alientech",CResourceManager::GetString("ALIENTECHEADLINE"),CResourceManager::GetString("ALIENTECLONG",false,sSectorName));
			pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
		}
	}
	else if(eventnumber == EVENTSHIPXP)
	{
		for (CShipMap::iterator i = ships->begin(); i != ships->end(); ++i)
		{
			if (i->second->GetOwnerOfShip() == pRace->GetRaceID() && i->second->GetKO()==ko)
			{
				const int additional_experience = rand() % 401 + 50;
				i->second->SetCrewExperiance(additional_experience);
			}
		}

		sMessageText = CResourceManager::GetString("EVENTSHIPXP",false,pDoc->GetSector(ko.x, ko.y).GetName(true));
		typ = MESSAGE_TYPE::MILITARY;
	}

	if (!sMessageText.IsEmpty())
	{
		CMessage message;
		message.GenerateMessage(sMessageText,typ,"",ko);//Nachricht erstellen
		pRace->GetEmpire()->AddMessage(message);

		if (pRace->IsHumanPlayer())
		{
			network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pRace->GetRaceID());
			pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
		}
	}
}

void CReManager::CalcShipEvents() const
{
	if (!IsActivated())
		return;

	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	// Hüllenvirus
	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
	{
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		{
			// 0.75% Wahrscheinlichkeit für einen Hüllenvirus pro Sektor
			if (rand()%133 != 0)
				continue;

			// gibt es keine Schiffe im Sektor, dann macht ein Hüllenvirus auch nichts
			CSector* pSector = &(pDoc->GetSector(x, y));
			if (!pSector->GetIsShipInSector())
				continue;

			// allen Schiffe im Sektor die Hülle auf 1 reduzieren (außer Aliens)
			for (CShipMap::iterator i = pDoc->m_ShipMap.begin(); i != pDoc->m_ShipMap.end(); ++i)
			{
				if (i->second->IsAlien())
					continue;

				if (i->second->GetKO() != pSector->GetKO())
					continue;

				int nCurrentHull = i->second->GetHull()->GetCurrentHull();
				i->second->GetHull()->SetCurrentHull(-(nCurrentHull - 1), true);

				// allen Schiffen in der Flotte ebenfalls die Hülle auf 1 setzen
				for (CShips::iterator j = i->second->begin(); j != i->second->end(); ++j)
				{
					nCurrentHull = j->second->GetHull()->GetCurrentHull();
					j->second->GetHull()->SetCurrentHull(-(nCurrentHull - 1), true);
				}
			}

			// Nachrichten an alle Major welche Schiffe in diesem Sektor hatten
			const std::map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
			for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
			{
				if (!pSector->GetOwnerOfShip(it->first))
					continue;

				CMajor* pMajor = it->second;
				if (!pMajor)
					continue;

				CString sSectorName = pSector->GetName(false);
				if (sSectorName.IsEmpty())
					sSectorName.Format("%c%i", (char)(pSector->GetKO().y+97), pSector->GetKO().x + 1);

				CString sMessageText = CResourceManager::GetString("EVENTHULLVIRUS", false, sSectorName);
				CMessage message;
				message.GenerateMessage(sMessageText,MESSAGE_TYPE::MILITARY,pSector->GetName(),pSector->GetKO());
				pMajor->GetEmpire()->AddMessage(message);

				if (pMajor->IsHumanPlayer())
				{
					network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pMajor->GetRaceID());
					pDoc->m_iSelectedView[client] = EMPIRE_VIEW;

					CEventRandom* EmpireEvent = new CEventRandom(pMajor->GetRaceID(), "HullVirus", sMessageText, "");
					pMajor->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
				}
			}
		}
	}
}
