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

CReManager::CReManager(void)
{
	m_Probability=5;
}
CReManager::~CReManager(void)
{
}
void CReManager::CalcEvents(CMajor* pRace)
{
	//Berechnet ob ein Event für diese Race eintritt. Wenn nicht wird returnt
	int randnumber=rand()%101;
	if(randnumber>m_Probability) return; //Es findet kein Ereignis statt
	
	if(rand()%2==1)//Ein nur 1 System betreffendes Event
	{
		int wichsystem=rand()%(pRace->GetEmpire()->GetSystemList()->GetSize());//wählt System aus
		CPoint KO = pRace->GetEmpire()->GetSystemList()->GetAt(wichsystem).ko; //sucht koordinaten des Systems
		for(int i=0;i<100&&(!SystemEvent(KO, pRace));i++)
		{
			wichsystem=rand()%(pRace->GetEmpire()->GetSystemList()->GetSize());//wählt System aus
			CPoint KO = pRace->GetEmpire()->GetSystemList()->GetAt(wichsystem).ko; //sucht koordinaten des Systems
		};

	} else{  //Ein globales Event
		for(int i=0;i<100&&(!GlobalEvent(pRace));i++);
	}
	
}

bool CReManager::SystemEvent(CPoint &ko, CMajor* pRace)
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);
	//ko= Systemkoordinate
	CString messagetext;//Nachrichtentext
	bool succes=true;
	int eventnumber=rand()%4;
	if(eventnumber==SYSTEMEVENTMORALBOOST)//If abfrage mit allen möglichen Randomevents; evtl. hier bedingungen einfügen
	{
		//CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),imagename,headline,text);
		//pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
		messagetext=CResourceManager::GetString("SYSTEMEVENTMORALBOOST",false,pDoc->GetSector(ko).GetName());
		pDoc->GetSystem(ko).SetMoral(10);
	}else if(eventnumber==SYSTEMEVENTMORALMALUS)
	{
		messagetext=CResourceManager::GetString("SYSTEMEVENTMORALMALUS",false,pDoc->GetSector(ko).GetName());
		pDoc->GetSystem(ko).SetMoral(-10);
	}else if(eventnumber==SYSTEMEVENTPLANETMOVEMENT)//Planetenveränderung
	{
		CArray<CPlanet>* Planets=pDoc->GetSector(ko).GetPlanets();
		int planet=rand()%Planets->GetSize();
		while(!(Planets->GetAt(planet).GetHabitable())) planet=rand()%Planets->GetSize();
		Planets->GetAt(planet).SetMaxHabitant(Planets->GetAt(planet).GetMaxHabitant()+rand()%7-3);
		messagetext=CResourceManager::GetString("SYSTEMEVENTPLANETMOVEMENT",false,Planets->GetAt(planet).GetPlanetName());
	}else if(eventnumber==SYSTEMEVENTDEMOGRAPHIC)
	{
	
		CArray<CPlanet>* Planets=pDoc->GetSector(ko).GetPlanets();
		int planet=rand()%Planets->GetSize();
		while(!(Planets->GetAt(planet).GetHabitable())) planet=rand()%Planets->GetSize();
		Planets->GetAt(planet).SetCurrentHabitant(Planets->GetAt(planet).GetCurrentHabitant()-rand()%(int)(Planets->GetAt(planet).GetCurrentHabitant()));
		messagetext=CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHIC",false,Planets->GetAt(planet).GetPlanetName());
		CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"demographic",CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHICTITLE"),CResourceManager::GetString("SYSTEMEVENTPLANETDEMOGRAPHICLONG",false,Planets->GetAt(planet).GetPlanetName()));
		pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
	}
	if(succes)
	{CMessage message;
	message.GenerateMessage(messagetext,MESSAGE_TYPE::SOMETHING,"",ko,FALSE,0);//Nachricht über Randomevent erstellen
	pRace->GetEmpire()->AddMessage(message);
	}
	return succes;

}

bool CReManager::GlobalEvent(CMajor *pRace)
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);
	int eventnumber=rand()%2;
	CString messagetext;
	MESSAGE_TYPE::Typ typ = MESSAGE_TYPE::NO_TYPE;
	bool succes=true;
	if(eventnumber==GLOBALEVENTRESEARCH)//If abfrage mit allen möglichen Randomevents; evtl. hier bedingungen einfügen
	{
		CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"Breakthrough",CResourceManager::GetString("BREAKTHROUGH"),CResourceManager::GetString("GLOBALEVENTRESEARCH"));
		pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
		messagetext=CResourceManager::GetString("GLOBALEVENTRESEARCH");
		typ=MESSAGE_TYPE::RESEARCH;
		pRace->GetEmpire()->AddFP((int)(pRace->GetEmpire()->GetFP()));
	}else if(eventnumber==GLOBALEVENTMINOR)// Für Minorregierungswechsel
	{
		vector<CMinor*> PossibleMinors;
		map<CString, CMinor*>* pmMinors = pDoc->GetRaceCtrl()->GetMinors();
		for (map<CString, CMinor*>::const_iterator it = pmMinors->begin(); it != pmMinors->end(); ++it)
		{
			CMinor* pMinor = it->second;
			if(pRace->IsRaceContacted(pMinor->GetRaceID())&& pMinor->GetRelation(pRace->GetRaceID())<85)
				PossibleMinors.insert(PossibleMinors.end(),pMinor);
		}
		if(PossibleMinors.size()>0)
		{
			CMinor* pMinor=PossibleMinors.at(rand()%PossibleMinors.size());
			pMinor->SetRelation(pRace->GetRaceID(), (rand() % 100) - pMinor->GetRelation(pRace->GetRaceID()));
			messagetext=CResourceManager::GetString("GLOBALEVENTMINOR",false,pMinor->GetRaceName());
			typ=MESSAGE_TYPE::DIPLOMACY;
		}else succes=false;
	}
	
	if(succes)
	{	CMessage message;
		message.GenerateMessage(messagetext,typ,"",NULL,FALSE,0);//Nachricht für Randomevent erstellen
		pRace->GetEmpire()->AddMessage(message);
	}
	return succes;
}

void CReManager::CalcExploreEvent(const CPoint &ko, CMajor *pRace, CArray<CShip, CShip>* ships)
{
	int randnumber=rand()%101;
	if(randnumber>m_Probability) return; //Es findet kein Ereignis statt

	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);
	//ko=Koordinate wo es passiert, pRace = die Rasse der es passiert
	int eventnumber=rand()%2;
	CString messagetext="";
	MESSAGE_TYPE::Typ typ = MESSAGE_TYPE::NO_TYPE;
	if(eventnumber==ALIENTEC)
	{
		CEventRandom* EmpireEvent=new CEventRandom(pRace->GetRaceID(),"alientech",CResourceManager::GetString("ALIENTECHEADLINE"),CResourceManager::GetString("ALIENTECLONG",false,pDoc->GetSector(ko).GetName(true)));
		pRace->GetEmpire()->GetEventMessages()->Add(EmpireEvent);
		messagetext=CResourceManager::GetString("ALIENTEC",false,pDoc->GetSector(ko).GetName(true));
		typ=MESSAGE_TYPE::RESEARCH;
		pRace->GetEmpire()->AddFP(100);
	}else if(eventnumber==EVENTSHIPXP)
	{
		for(int i=0;i<ships->GetSize();i++)
		{
			if(pDoc->GetShip(i).GetOwnerOfShip()==pRace->GetRaceID()&&pDoc->GetShip(i).GetKO()==ko)
				ships->GetAt(i).SetCrewExperiance(4000);
			
		}
		messagetext=CResourceManager::GetString("EVENTSHIPXP",false,pDoc->GetSector(ko).GetName(true));
		typ=MESSAGE_TYPE::MILITARY;

	}
	CMessage message;
	message.GenerateMessage(messagetext,typ,"",ko,FALSE,0);//Nachricht erstellen
	pRace->GetEmpire()->AddMessage(message);

};