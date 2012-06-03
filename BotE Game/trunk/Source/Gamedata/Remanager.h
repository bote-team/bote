#pragma once
#include "Races/Major.h"


enum GLOBALEVENTTYPE
{
	GLOBALEVENTRESEARCH=0,
	GLOBALEVENTMINOR=1


};

enum SYSTEMEVENTTYPE
{
	SYSTEMEVENTMORALBOOST=0,// Moral bonus
	SYSTEMEVENTMORALMALUS=1,// Moral malus
	SYSTEMEVENTPLANETMOVEMENT=2,
	SYSTEMEVENTDEMOGRAPHIC=3
};
enum EXPLOREEVENTTYPE
{
	ALIENTEC=0,
	EVENTSHIPXP=1
};
#define GLOBALEVENTTYPES 2
#define SYSTEMEVENTTYPES 3
#define EXPLOREEVENTTYPES 2
//Klasse zum berchnen der Random Events(Random Event Manager)
class CReManager : public CObject
{
	public:
	//DECLARE_SERIAL (CReManager)
	//Konstruktor/Destruktor
	CReManager(void);
	~CReManager(void);

	///Berechnet Randomevents für die gegebene Majorrace
	void CalcEvents(CMajor* pRace);

	///Berechnet Events wenn ein Sektor erkundet wird
	void CalcExploreEvent(const CPoint& ko, CMajor* pRace, CArray<CShip, CShip>* ships);

	///Gibt Wahrscheinlichkeit zurück
	USHORT getProbability() {return m_Probability;};

	///Setzt die Wahrscheinlichkeit auf newProb
	void setProbability(USHORT newProb) {m_Probability=((newProb<101)?newProb:100);};

	private:

	//Für Random Events die nur ein System betreffen
	bool SystemEvent(CPoint &ko, CMajor* pRace);

	//Für globale Events
	bool GlobalEvent(CMajor* pRace);

	USHORT m_Probability;


};
