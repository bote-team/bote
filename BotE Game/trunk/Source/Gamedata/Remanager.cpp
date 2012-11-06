#include "stdafx.h"
#include "Remanager.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races/Major.h"
#include "Races/Minor.h"
#include "Races/RaceController.h"
#include "Galaxy/planet.h"
#include "Ships/ship.h"
#include "Events/EventRandom.h"
#include "Gamedata/General/Message.h"

#include <cassert>

CReManager::CReManager(void) :
	m_uiGlobalProb(3),
	m_uiProbPerSystem(10),
	m_uiProbPerMinor(10)
{
}
CReManager::~CReManager(void)
{
}

//In this function, calculate whether an event fails or succeeds due to given probabilities.
//Functions executed in succession only decide whether an event fails due to constraints
//such as population already being at minimum.
void CReManager::CalcEvents(CMajor* pRace)
{
	CBotf2Doc const* pDoc = dynamic_cast<CBotf2App*>(AfxGetApp())->GetDocument();
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
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);
	//ko= Systemkoordinate
	CString messagetext;//Nachrichtentext
	bool success=true;
	int eventnumber=rand()%4;
	if(eventnumber==SYSTEMEVENTMORALBOOST)//If abfrage mit allen möglichen Randomevents; evtl. hier bedingungen einfügen
	{
		//CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),imagename,headline,text);
		//pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
		messagetext=CResourceManager::GetString("SYSTEMEVENTMORALBOOST",false,pDoc->GetSector(ko.x, ko.y).GetName());
		pDoc->GetSystem(ko.x, ko.y).SetMoral(10);
	}else if(eventnumber==SYSTEMEVENTMORALMALUS)
	{
		messagetext=CResourceManager::GetString("SYSTEMEVENTMORALMALUS",false,pDoc->GetSector(ko.x, ko.y).GetName());
		pDoc->GetSystem(ko.x, ko.y).SetMoral(-10);
	}else if(eventnumber==SYSTEMEVENTPLANETMOVEMENT)//Planetenveränderung
	{
		std::vector<CPlanet>& planets = pDoc->GetSector(ko.x, ko.y).GetPlanets();
		const int size = planets.size();
		assert(size >= 1);
		int planet=rand()%size;
		while(!(planets.at(planet).GetHabitable()))
			planet=rand()%size;
		const float old_habitants = planets.at(planet).GetMaxHabitant();
		const float habitants_change = old_habitants * ((rand() % 11) / 10.0f) * ((rand() % 2 == 1) ? 1 : -1);
		const float new_habitants = min(max(old_habitants + habitants_change, 1),100);
		success = old_habitants != new_habitants;
		if(success)
		{
			planets.at(planet).SetMaxHabitant(new_habitants);
			CString habitants_change_string;
			habitants_change_string.Format("%.3f", habitants_change);
			messagetext=CResourceManager::GetString("SYSTEMEVENTPLANETMOVEMENT",false,planets.at(planet).GetPlanetName(), habitants_change_string);
		}
	}else if(eventnumber==SYSTEMEVENTDEMOGRAPHIC)
	{
		std::vector<CPlanet>& planets = pDoc->GetSector(ko.x, ko.y).GetPlanets();
		int planet = 0;
		// Es sollte hier immer mindestens 1 habitabler bewohnter Planet im System sein...
		success = false;
		for(int i = 0; i < 100; ++i) {
			planet = rand()%planets.size();
			if(planets.at(planet).GetHabitable() && planets.at(planet).GetCurrentHabitant() > 1) {
				success = true;
				break;
			}
		}
		if(success) {
			const float old_habitants = planets.at(planet).GetCurrentHabitant();
			const float habitants_change = old_habitants * ((rand() % 11) / 10.0f) * -1;
			const float new_habitants = max(old_habitants + habitants_change, 1);
			success = old_habitants != new_habitants;
			if(success)
			{
				planets.at(planet).SetCurrentHabitant(new_habitants);
				CString habitants_change_string;
				habitants_change_string.Format("%.3f", habitants_change * -1);
				messagetext=CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHICLONG",false,planets.at(planet).GetPlanetName(), habitants_change_string);
				//CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"demographic",CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHICTITLE"),CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHICLONG",false,planets.at(planet).GetPlanetName()));
				//pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
			}
		}
	}
	if(success)
	{
		CMessage message;
		message.GenerateMessage(messagetext,MESSAGE_TYPE::SOMETHING,"",ko,FALSE,0);//Nachricht über Randomevent erstellen
		pRace->GetEmpire()->AddMessage(message);
	}
	return success;
}

void CReManager::GlobalEventResearch(CMajor *pRace)
{
	//CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"Breakthrough",CResourceManager::GetString("BREAKTHROUGH"),CResourceManager::GetString("GLOBALEVENTRESEARCH"));
	//pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
	CString messagetext=CResourceManager::GetString("GLOBALEVENTRESEARCH");
	pRace->GetEmpire()->AddFP((int)(pRace->GetEmpire()->GetFP()));

	CMessage message;
	message.GenerateMessage(messagetext,MESSAGE_TYPE::RESEARCH,"",NULL,FALSE,0);//Nachricht für Randomevent erstellen
	pRace->GetEmpire()->AddMessage(message);
}

void CReManager::GlobalEventMinor(CMajor* pRace, CMinor* pMinor)
{
	pMinor->SetRelation(pRace->GetRaceID(), (rand() % 101) - pMinor->GetRelation(pRace->GetRaceID()));
	CString messagetext=CResourceManager::GetString("GLOBALEVENTMINOR",false,pMinor->GetRaceName());
	CMessage message;
	message.GenerateMessage(messagetext,MESSAGE_TYPE::DIPLOMACY,"",NULL,FALSE,0);//Nachricht für Randomevent erstellen
	pRace->GetEmpire()->AddMessage(message);
}

void CReManager::CalcExploreEvent(const CPoint &ko, CMajor *pRace, CArray<CShip, CShip>* ships)
{
	if(rand() % 99 >= static_cast<int>(m_uiGlobalProb))
		return; //Es findet kein Ereignis statt

	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);
	//ko=Koordinate wo es passiert, pRace = die Rasse der es passiert
	int eventnumber=rand()%2;
	CString messagetext="";
	MESSAGE_TYPE::Typ typ = MESSAGE_TYPE::NO_TYPE;
	if(eventnumber==ALIENTEC)
	{
		//CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"alientech",CResourceManager::GetString("ALIENTECHEADLINE"),CResourceManager::GetString("ALIENTECLONG",false,pDoc->GetSector(ko.x, ko.y).GetName(true)));
		//pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
		messagetext=CResourceManager::GetString("ALIENTEC",false,pDoc->GetSector(ko.x, ko.y).GetName(true));
		typ=MESSAGE_TYPE::RESEARCH;
		pRace->GetEmpire()->AddFP(100);
	}else if(eventnumber==EVENTSHIPXP)
	{
		for(int i=0;i<ships->GetSize();i++)
		{
			if(pDoc->GetShip(i).GetOwnerOfShip()==pRace->GetRaceID()&&pDoc->GetShip(i).GetKO()==ko)  {
				const int additional_experience = rand() % 401 + 50;
				ships->GetAt(i).SetCrewExperiance(additional_experience);
			}

		}
		messagetext=CResourceManager::GetString("EVENTSHIPXP",false,pDoc->GetSector(ko.x, ko.y).GetName(true));
		typ=MESSAGE_TYPE::MILITARY;

	}
	CMessage message;
	message.GenerateMessage(messagetext,typ,"",ko,FALSE,0);//Nachricht erstellen
	pRace->GetEmpire()->AddMessage(message);

};
