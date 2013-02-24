#include "stdafx.h"
#include "RandomEventCtrl.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Races/Major.h"
#include "Races/Minor.h"
#include "Races/RaceController.h"
#include "Galaxy/planet.h"
#include "Events/EventRandom.h"
#include "General/EmpireNews.h"
#include "Ships/Ships.h"
#include "General/Loc.h"
#include "IniLoader.h"

#include <cassert>

CRandomEventCtrl::CRandomEventCtrl(void) :
	m_uiGlobalProb(2),
	m_uiProbPerSystem(7),
	m_uiProbPerMinor(7)
{
	m_bIsActivated = true;
	if (CIniLoader* pIni = CIniLoader::GetInstance())
		m_bIsActivated = pIni->ReadValueDefault("Special", "RANDOMEVENTS", true);
}

CRandomEventCtrl::~CRandomEventCtrl(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CRandomEventCtrl* CRandomEventCtrl::GetInstance(void)
{
	static CRandomEventCtrl instance;
	return &instance;
}

//In this function, calculate whether an event fails or succeeds due to given probabilities.
//Functions executed in succession only decide whether an event fails due to constraints
//such as population already being at minimum.
void CRandomEventCtrl::CalcEvents(CMajor* pRace)
{
	if (!IsActivated())
		return;

	CBotEDoc const* pDoc = resources::pDoc;
	const unsigned event_type = rand() % 3;
	if(event_type == GLOBALEVENTSYSTEM)//system affecting event
	{
		//Calculate whether such event happens. The more systems we have, the more
		//likely it is that one of them is affected.
		CArray<EMPIRE_SYSTEMS> const* systems = pRace->GetEmpire()->GetSystemList();
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

bool CRandomEventCtrl::SystemEvent(const CPoint &ko, CMajor* pRace)
{
	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);
	//ko= Systemkoordinate
	CString sMsgText;//Nachrichtentext
	bool bSuccess=true;
	int nEventNumber=rand()%4;

	if(nEventNumber==SYSTEMEVENTMORALBOOST)//If abfrage mit allen möglichen Randomevents; evtl. hier bedingungen einfügen
	{
		sMsgText=CLoc::GetString("SYSTEMEVENTMORALBOOST",false,pDoc->GetSector(ko.x, ko.y).GetName());
		pDoc->GetSystem(ko.x, ko.y).SetMoral(10);
	}
	else if(nEventNumber==SYSTEMEVENTMORALMALUS)
	{
		sMsgText=CLoc::GetString("SYSTEMEVENTMORALMALUS",false,pDoc->GetSector(ko.x, ko.y).GetName());
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
			sMsgText=CLoc::GetString("SYSTEMEVENTPLANETMOVEMENT",false,vPlanets.at(nPlanet).GetPlanetName(), habitants_change_string);
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
				sMsgText=CLoc::GetString("SYSTEMEVENTPLANETDEMOGRAPHICLONG",false,planets.at(planet).GetPlanetName(), habitants_change_string);

				if (pRace->IsHumanPlayer())
				{
					network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pRace->GetRaceID());
					pDoc->m_iSelectedView[client] = EMPIRE_VIEW;

					CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"demographic",CLoc::GetString("SYSTEMEVENTPLANETDEMOGRAPHICTITLE"),sMsgText);
					pRace->GetEmpire()->GetEvents()->Add(EmpireEvent);
				}
			}
		}
	}

	if(bSuccess && !sMsgText.IsEmpty())
	{
		CEmpireNews message;
		message.CreateNews(sMsgText,EMPIRE_NEWS_TYPE::SOMETHING,"",ko);//Nachricht über Randomevent erstellen
		pRace->GetEmpire()->AddMsg(message);

		if (pRace->IsHumanPlayer())
		{
			network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pRace->GetRaceID());
			pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
		}
	}
	return bSuccess;
}

void CRandomEventCtrl::GlobalEventResearch(CMajor *pRace)
{
	CString messagetext=CLoc::GetString("GLOBALEVENTRESEARCH");
	pRace->GetEmpire()->AddFP((int)(pRace->GetEmpire()->GetFP()));

	CEmpireNews message;
	message.CreateNews(messagetext,EMPIRE_NEWS_TYPE::RESEARCH);//Nachricht für Randomevent erstellen
	pRace->GetEmpire()->AddMsg(message);

	if (pRace->IsHumanPlayer())
	{
		CBotEDoc* pDoc = resources::pDoc;
		ASSERT(pDoc);

		network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pRace->GetRaceID());
		pDoc->m_iSelectedView[client] = EMPIRE_VIEW;

		CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"Breakthrough",CLoc::GetString("BREAKTHROUGH"),CLoc::GetString("GLOBALEVENTRESEARCH"));
		pRace->GetEmpire()->GetEvents()->Add(EmpireEvent);
	}
}

void CRandomEventCtrl::GlobalEventMinor(CMajor* pRace, CMinor* pMinor)
{
	pMinor->SetRelation(pRace->GetRaceID(), (rand() % 101) - pMinor->GetRelation(pRace->GetRaceID()));
	CString messagetext=CLoc::GetString("GLOBALEVENTMINOR",false,pMinor->GetRaceName());
	CEmpireNews message;
	message.CreateNews(messagetext,EMPIRE_NEWS_TYPE::DIPLOMACY);//Nachricht für Randomevent erstellen
	pRace->GetEmpire()->AddMsg(message);
}

void CRandomEventCtrl::CalcExploreEvent(const CPoint &ko, CMajor *pRace, CShipMap* ships)
{
	if (!IsActivated())
		return;

	if(rand() % 99 >= static_cast<int>(m_uiGlobalProb))
		return; //Es findet kein Ereignis statt

	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	//ko=Koordinate wo es passiert, pRace = die Rasse der es passiert
	int eventnumber=rand()%2;
	CString sMessageText = "";
	EMPIRE_NEWS_TYPE::Typ typ = EMPIRE_NEWS_TYPE::NO_TYPE;

	if (eventnumber == ALIENTEC)
	{
		// Forschung insgesamt um (50 bis 150) * Anzahl Systeme erhöhen
		int nSystemCount = pRace->GetEmpire()->GetSystemList()->GetSize();
		int nAdd = (50 + rand()%101) * nSystemCount;
		pRace->GetEmpire()->AddFP(nAdd);

		CString sSectorName = pDoc->GetSector(ko.x, ko.y).GetName(false);
		if (sSectorName.IsEmpty())
			sSectorName.Format("%c%i", (char)(ko.y+97), ko.x + 1);

		sMessageText = CLoc::GetString("ALIENTEC",false,sSectorName);
		typ = EMPIRE_NEWS_TYPE::RESEARCH;

		if (pRace->IsHumanPlayer())
		{
			CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"alientech",CLoc::GetString("ALIENTECHEADLINE"),CLoc::GetString("ALIENTECLONG",false,sSectorName));
			pRace->GetEmpire()->GetEvents()->Add(EmpireEvent);
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

		sMessageText = CLoc::GetString("EVENTSHIPXP",false,pDoc->GetSector(ko.x, ko.y).GetName(true));
		typ = EMPIRE_NEWS_TYPE::MILITARY;
	}

	if (!sMessageText.IsEmpty())
	{
		CEmpireNews message;
		message.CreateNews(sMessageText,typ,"",ko);//Nachricht erstellen
		pRace->GetEmpire()->AddMsg(message);

		if (pRace->IsHumanPlayer())
		{
			network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pRace->GetRaceID());
			pDoc->m_iSelectedView[client] = EMPIRE_VIEW;
		}
	}
}

void CRandomEventCtrl::CalcShipEvents() const
{
	if (!IsActivated())
		return;

	CBotEDoc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	// Hüllenvirus
	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
	{
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		{
			// 0.1% Wahrscheinlichkeit für einen Hüllenvirus pro Sektor
			if (rand()%1000 != 0)
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

				CString sMessageText = CLoc::GetString("EVENTHULLVIRUS", false, sSectorName);
				CEmpireNews message;
				message.CreateNews(sMessageText,EMPIRE_NEWS_TYPE::MILITARY,pSector->GetName(),pSector->GetKO());
				pMajor->GetEmpire()->AddMsg(message);

				if (pMajor->IsHumanPlayer())
				{
					network::RACE client = pDoc->GetRaceCtrl()->GetMappedClientID(pMajor->GetRaceID());
					pDoc->m_iSelectedView[client] = EMPIRE_VIEW;

					CEventRandom* EmpireEvent = new CEventRandom(pMajor->GetRaceID(), "HullVirus", sMessageText, "");
					pMajor->GetEmpire()->GetEvents()->Add(EmpireEvent);
				}
			}
		}
	}
}
